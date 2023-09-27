#include "default_backend.h"

imp_DefaultBackendContext* default_ctx = NULL;

b32 imp_default_backend_init() {
    assert(default_ctx == NULL && "Default backend already initialized!");

    // Allocate context for this backend
    default_ctx = (imp_DefaultBackendContext*)malloc(sizeof(imp_DefaultBackendContext));

    (*default_ctx) = (imp_DefaultBackendContext){0};

    SetConfigFlags(FLAG_MSAA_4X_HINT);

    return true;
}

b32 imp_default_backend_get_inputs(imp_Inputs* inputs) {
    assert(default_ctx != NULL && "Need a backend context in order to get input!");

    inputs->exit = WindowShouldClose();

    return true;
}

b32 imp_default_backend_set_canvas(imp_Canvas canvas, const char* title) {
    assert(default_ctx != NULL && "Need a backend context in order to make a canvas!");

    // If already open, make a new one
    if (default_ctx->window_open)
        CloseWindow();

    InitWindow(canvas.size.X, canvas.size.Y, title);

    default_ctx->window_open = true;
    default_ctx->canvas = canvas;

    return true;
}

b32 imp_default_backend_set_camera(imp_Camera camera) {
    assert(default_ctx != NULL && "Need a backend context in order to set a camera!");

    default_ctx->camera = camera;

    return true;
}
b32 imp_default_backend_run_commands(imp_CommandList command_list) {
    BeginDrawing();

    ClearBackground((Color){ default_ctx->canvas.clear_color.R, default_ctx->canvas.clear_color.G, default_ctx->canvas.clear_color.B, default_ctx->canvas.clear_color.A });

    Camera3D camera_3D;

    camera_3D.position = (Vector3){ default_ctx->camera.position.X, default_ctx->camera.position.Y, default_ctx->camera.position.Z };
    camera_3D.target = (Vector3){ default_ctx->camera.target.X, default_ctx->camera.target.Y, default_ctx->camera.target.Z };
    camera_3D.up = (Vector3){ default_ctx->camera.up.X, default_ctx->camera.up.Y, default_ctx->camera.up.Z };
    camera_3D.fovy = default_ctx->camera.fov_y * RAD2DEG;
    camera_3D.projection = (default_ctx->camera.mode == IMP_CAMERA_ORTHO ? CAMERA_ORTHOGRAPHIC : CAMERA_PERSPECTIVE);

    BeginMode3D(camera_3D);

    for (s32 i = 0; i < command_list.num_commands; i++) {
        imp_Command command = command_list.commands[i];

        switch (command.type) {
        case IMP_COMMAND_DRAW_POINT_LIST: {
            imp_Color use_color = command.point_list.color;

            if (command.point_list.style == IMP_POINT_LIST_STYLE_CURVE) {
                imp_Vec3f point_prev = command.point_list.data[0];

                for (int i = 1; i < command.point_list.num_elements; i++) {
                    imp_Vec3f point = command.point_list.data[i];

                    DrawLine3D((Vector3){ point_prev.X, point_prev.Y, point_prev.Z }, (Vector3){ point.X, point.Y, point.Z },
                            (Color){ use_color.R, use_color.G, use_color.B, use_color.A });

                    point_prev = point;
                }
            }
            else {
                for (int i = 0; i < command.point_list.num_elements; i++) {
                    imp_Vec3f point = command.point_list.data[i];

                    DrawSphere((Vector3){ point.X, point.Y, point.Z }, command.point_list.thickness,
                            (Color){ use_color.R, use_color.G, use_color.B, use_color.A });
                }
            }

            break;
        }
        case IMP_COMMAND_DRAW_AXIS: {
            imp_Color use_color = command.axis.color;

            DrawLine3D((Vector3){ command.axis.start.X, command.axis.start.Y, command.axis.start.Z }, (Vector3){ command.axis.end.X, command.axis.end.Y, command.axis.end.Z },
                    (Color){ use_color.R, use_color.G, use_color.B, use_color.A });

            // Draw ticks
            imp_Vec3f diff = HMM_SubV3(command.axis.end, command.axis.start);

            f32 tick_length = HMM_LenV3(diff) / command.axis.num_ticks;

            for (int i = 0; i < command.axis.num_ticks; i++) {
                f32 ratio = (f32)i / (f32)(command.axis.num_ticks);

                imp_Vec3f pos = HMM_AddV3(command.axis.start, HMM_MulV3F(diff, ratio));

                DrawSphere((Vector3){ pos.X, pos.Y, pos.Z }, command.axis.tick_thickness,
                        (Color){ use_color.R, use_color.G, use_color.B, use_color.A });
            }

            break;
        }
        case IMP_COMMAND_DRAW_ANNOTATION: {
            // Ignore for now
            break;
        }
        default: {
            assert(false);

            break;
        }
        }
    }

    EndMode3D();
    
    Camera2D camera_2D = { 0 };
    camera_2D.zoom = 1.0f;

    BeginMode2D(camera_2D);

    // Go through again but this time just render annotations
    for (s32 i = 0; i < command_list.num_commands; i++) {
        imp_Command command = command_list.commands[i];

        if (command.type == IMP_COMMAND_DRAW_ANNOTATION) {
            imp_Color use_color = command.annotation.color;

            Vector2 pos = GetWorldToScreen((Vector3){ command.annotation.position.X, command.annotation.position.Y, command.annotation.position.Z }, camera_3D);

            DrawText(command.annotation.text.str, pos.x, pos.y, (s32)command.annotation.size, 
                    (Color){ use_color.R, use_color.G, use_color.B, use_color.A });
        }
    }

    EndMode2D();

    EndDrawing();

    return true;
}

b32 imp_default_backend_deinit() {
    if (default_ctx != NULL) {
        if (default_ctx->window_open)
            CloseWindow();

        free(default_ctx);
    }

    return true;
}
