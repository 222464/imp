#include "imp.h"

#include "backends/default_backend.h"
#include "backends/dynamic_backend.h"

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

void imp_init(imp_Context* ctx, b32 (*backend_set_canvas)(imp_Canvas, const char*), b32 (*backend_set_camera)(imp_Camera), b32 (*backend_run_commands)(imp_CommandList), b32 (*backend_get_inputs)(imp_Inputs* inputs)) {
    *ctx = (imp_Context){0};

    ctx->backend_set_canvas = backend_set_canvas;
    ctx->backend_set_camera = backend_set_camera;
    ctx->backend_run_commands = backend_run_commands;
    ctx->backend_get_inputs = backend_get_inputs;

    ctx->palette = IMP_PALETTE_DARK_MODE;
}

void imp_init_default(imp_Context* ctx) {
    imp_default_backend_init();

    ctx->backend_type = IMP_BACKEND_TYPE_DEFAULT;

    imp_init(ctx, &imp_default_backend_set_canvas, &imp_default_backend_set_camera, &imp_default_backend_run_commands, &imp_default_backend_get_inputs);
}

void imp_init_dynamic(imp_Context* ctx, const char* path) {
    imp_dynamic_backend_init(path);

    ctx->backend_type = IMP_BACKEND_TYPE_DYNAMIC;

    imp_init(ctx, &imp_dynamic_backend_set_canvas, &imp_dynamic_backend_set_camera, &imp_dynamic_backend_run_commands, &imp_dynamic_backend_get_inputs);
}

void imp_deinit(imp_Context* ctx) {
    if (ctx->backend_type == IMP_BACKEND_TYPE_DEFAULT)
        imp_default_backend_deinit();
    else if (ctx->backend_type == IMP_BACKEND_TYPE_DYNAMIC)
        imp_dynamic_backend_deinit();
}

void imp_canvas(imp_Context* ctx, imp_Canvas canvas, const char* title) {
    ctx->canvas = canvas;

    (*ctx->backend_set_canvas)(canvas, title);
}

void imp_begin(imp_Context* ctx) {
    // Get inputs
    (*ctx->backend_get_inputs)(&ctx->inputs);

    ctx->char_pos = 0;
    ctx->command_list.num_commands = 0;
}

void imp_end(imp_Context* ctx) {
    (*ctx->backend_run_commands)(ctx->command_list);
}

void imp_camera(imp_Context* ctx, imp_Camera camera) {
    ctx->camera = camera;

    (*ctx->backend_set_camera)(camera);
}

void imp_point_list(imp_Context* ctx, imp_Vec3f* data, s32 num_elements, imp_PointListStyle style) {
    imp_point_list_color(ctx, data, num_elements, style, ctx->palette.point_list);
}

void imp_point_list_color(imp_Context* ctx, imp_Vec3f* data, s32 num_elements, imp_PointListStyle style, imp_Color color) {
    imp_Command command;
    command.type = IMP_COMMAND_DRAW_POINT_LIST;

    command.point_list.color = color;

    command.point_list.data = data;
    command.point_list.num_elements = num_elements;
    command.point_list.style = style;

    imp_command_list_add(&ctx->command_list, command);
}

void imp_axis(imp_Context* ctx, imp_Vec3f start, imp_Vec3f end, s32 num_ticks) {
    imp_axis_color(ctx, start, end, num_ticks, ctx->palette.axis);
}

void imp_axis_color(imp_Context* ctx, imp_Vec3f start, imp_Vec3f end, s32 num_ticks, imp_Color color) {
    imp_Command command;
    command.type = IMP_COMMAND_DRAW_AXIS;

    command.axis.color = color;

    command.axis.start = start;
    command.axis.end = end;
    command.axis.num_ticks = num_ticks;

    imp_command_list_add(&ctx->command_list, command);
}
