#include "imp.h"

#ifndef IMP_VSNSPRINTF
#define STB_SPRINTF_IMPLEMENTATION
#include "third_party/stb_sprintf.h"
#define IMP_VSNSPRINTF stbsp_vsnprintf
#endif

imp_Str imp_set_str(char* s) {
    imp_Str out = { .str = s, .len = 0 };

    while (*s++ != 0)
        out.len++;

    return out;
}

imp_Str imp_strfv(imp_Context* ctx, char* fmt, va_list args) {
    imp_Str result = (imp_Str){0};

    result.str = ctx->char_buffer + ctx->char_pos;
    result.len = IMP_VSNSPRINTF(ctx->char_buffer + ctx->char_pos, IMP_CHAR_BUFFER_SIZE - ctx->char_pos, fmt, args);

    ctx->char_pos += result.len + 1; // Include null terminator

    return result;
}

imp_Str imp_strf(imp_Context* ctx, char* fmt, ...) {
    va_list args;

    va_start(args, fmt);

    imp_Str result = imp_strfv(ctx, fmt, args);

    va_end(args);

    return result;
}

void imp_init(imp_Context* ctx, imp_Backend *backend) {
    *ctx = (imp_Context){0};

	ctx->backend = backend;

    ctx->palette = IMP_PALETTE_DARK_MODE;

    ctx->camera = IMP_DEFAULT_CAMERA3D;
}

void imp_deinit(imp_Context* ctx) {
	// Context should probably not own the backend for multiple contexts.
	//ctx->backend->deinit(ctx->backend);
}

imp_Context *imp_make_context(imp_Backend *backend) {
	imp_Context *ctx = IMP_MALLOC(sizeof(imp_Context));

	imp_init(ctx, backend);

	return ctx;
}

void imp_destroy_context(imp_Context *ctx) {
	// TODO: Maybe have IMP_ASSERT.
	if (ctx) {
		imp_deinit(ctx);
		IMP_FREE(ctx);
	}
}

void imp_canvas(imp_Context* ctx, imp_Canvas canvas, const char* title) {
    ctx->canvas = canvas;

    (*ctx->backend->set_canvas)(ctx->backend, canvas, title);
}

void imp_begin(imp_Context* ctx) {
    // Get inputs
    ctx->inputs_prev = ctx->inputs;
    (*ctx->backend->get_inputs)(ctx->backend, &ctx->inputs);

    (*ctx->backend->set_camera)(ctx->backend, ctx->camera);

    ctx->char_pos = 0;
    ctx->command_list.num_commands = 0;
}

void imp_end(imp_Context* ctx) {
    (*ctx->backend->run_commands)(ctx->backend, ctx->command_list);
}

void imp_camera(imp_Context* ctx, imp_Camera camera) {
    ctx->camera = camera;
}

void imp_orbit_camera(imp_Context* ctx, f32 move_sensitivity, f32 zoom_sensitivity) {
    imp_Vec2f diff = HMM_SubV2(ctx->inputs.mouse, ctx->inputs_prev.mouse);

    imp_Vec3f orbit = HMM_SubV3(ctx->camera.position, ctx->camera.target);
    imp_Vec3f dir = HMM_NormV3(orbit);

    imp_Vec3f up = (imp_Vec3f){ 0.0f, 1.0f, 0.0f };

    imp_Vec3f F = dir;
    imp_Vec3f S = HMM_Cross(F, up);
    imp_Vec3f U = HMM_Cross(F, S);

    f32 dist = HMM_LenV3(orbit);

    f32 new_dist = HMM_MAX(IMP_EPSILON, dist - ctx->inputs.mouse_scroll * zoom_sensitivity);

    imp_Vec3f new_dir;

    if (ctx->inputs.mouse_down_right) {
        new_dir = HMM_AddV3(dir, HMM_MulV3F(S, diff.X * move_sensitivity));
        new_dir = HMM_AddV3(new_dir, HMM_MulV3F(U, -diff.Y * move_sensitivity));
    }
    else
        new_dir = dir;

    new_dir = HMM_NormV3(new_dir);

    orbit = HMM_MulV3F(new_dir, new_dist);

    ctx->camera.position = HMM_AddV3(orbit, ctx->camera.target);
}

void imp_point_list(imp_Context* ctx, imp_Vec3f* data, s32 num_elements, imp_PointListStyle style) {
    imp_point_list_ex(ctx, data, num_elements, style, ctx->palette.point_list, IMP_DEFAULT_THICKNESS);
}

void imp_point_list_ex(imp_Context* ctx, imp_Vec3f* data, s32 num_elements, imp_PointListStyle style, imp_Color color, f32 thickness) {
    imp_Command command;
    command.type = IMP_COMMAND_DRAW_POINT_LIST;

    command.point_list.color = color;

    command.point_list.data = data;
    command.point_list.num_elements = num_elements;
    command.point_list.style = style;
    command.point_list.thickness = thickness;

    imp_command_list_add(&ctx->command_list, command);
}

void imp_axis(imp_Context* ctx, imp_Vec3f start, imp_Vec3f end, s32 num_ticks) {
    imp_axis_ex(ctx, start, end, num_ticks, ctx->palette.axis, IMP_DEFAULT_THICKNESS, IMP_DEFAULT_TICK_THICKNESS);
}

void imp_axis_ex(imp_Context* ctx, imp_Vec3f start, imp_Vec3f end, s32 num_ticks, imp_Color color, f32 thickness, f32 tick_thickness) {
    imp_Command command;
    command.type = IMP_COMMAND_DRAW_AXIS;

    command.axis.color = color;

    command.axis.start = start;
    command.axis.end = end;
    command.axis.num_ticks = num_ticks;
    command.axis.thickness = thickness;
    command.axis.tick_thickness = tick_thickness;

    imp_command_list_add(&ctx->command_list, command);
}

void imp_axes(imp_Context* ctx, s32 num_ticks) {
    imp_axes_ex(ctx, num_ticks, ctx->palette.axis, IMP_DEFAULT_THICKNESS, IMP_DEFAULT_TICK_THICKNESS);
}

void imp_axes_ex(imp_Context* ctx, s32 num_ticks, imp_Color color, f32 thickness, f32 tick_thickness) {
    imp_Vec3f x_axis = (imp_Vec3f){ 1.0f, 0.0f, 0.0f };
    imp_Vec3f y_axis = (imp_Vec3f){ 0.0f, 1.0f, 0.0f };
    imp_Vec3f z_axis = (imp_Vec3f){ 0.0f, 0.0f, 1.0f };

    imp_axis_ex(ctx, (imp_Vec3f){ -x_axis.X, -x_axis.Y, -x_axis.Z }, x_axis, num_ticks, color, thickness, tick_thickness);
    imp_axis_ex(ctx, (imp_Vec3f){ -y_axis.X, -y_axis.Y, -y_axis.Z }, y_axis, num_ticks, color, thickness, tick_thickness);
    imp_axis_ex(ctx, (imp_Vec3f){ -z_axis.X, -z_axis.Y, -z_axis.Z }, z_axis, num_ticks, color, thickness, tick_thickness);
}
