#include "default_backend.h"

imp_DefaultBackendContext* default_ctx = NULL;

const char* curve_vs =
#include "shaders/curve.vs"
;

const char* curve_fs =
#include "shaders/curve.fs"
;

b32 imp_default_backend_init() {
    assert(default_ctx == NULL && "Default backend already initialized!");

    // Allocate context for this backend
    default_ctx = (imp_DefaultBackendContext*)malloc(sizeof(imp_DefaultBackendContext));

    (*default_ctx) = (imp_DefaultBackendContext){0};

    SetConfigFlags(FLAG_MSAA_4X_HINT);

    default_ctx->instance_transforms = (Matrix*)malloc(IMP_DEFAULT_BACKEND_MAX_INSTANCE_TRANFORMS * sizeof(Matrix));

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

    SetTargetFPS(144);

    default_ctx->window_open = true;
    default_ctx->canvas = canvas;

    if (!default_ctx->resources_ready) {
        // get resources
        default_ctx->cylinder = GenMeshCylinder(1.0f, 1.0f, 8);
        default_ctx->sphere = GenMeshSphere(1.0f, 8, 8);

        default_ctx->curve_shader = LoadShaderFromMemory(curve_vs, curve_fs);

        default_ctx->curve_shader.locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(default_ctx->curve_shader, "mvp");
        default_ctx->curve_col_diffuse_loc = GetShaderLocation(default_ctx->curve_shader, "color");
        default_ctx->curve_shader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocationAttrib(default_ctx->curve_shader, "instanceTransform");

        default_ctx->curve_material = LoadMaterialDefault();
        default_ctx->curve_material.shader = default_ctx->curve_shader;

        default_ctx->t = GetTime();

        default_ctx->resources_ready = true;
    }

    return true;
}

b32 imp_default_backend_set_camera(imp_Camera camera) {
    assert(default_ctx != NULL && "Need a backend context in order to set a camera!");

    default_ctx->camera = camera;

    return true;
}

b32 imp_default_backend_run_commands(imp_CommandList command_list) {
    f64 time_prev = default_ctx->t;

    default_ctx->t = GetTime();

    f64 dt = default_ctx->t - time_prev;

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

                if (command.point_list.num_elements > IMP_DEFAULT_BACKEND_MAX_INSTANCE_TRANFORMS)
                    return false;

                for (int i = 1; i < command.point_list.num_elements; i++) {
                    imp_Vec3f point = command.point_list.data[i];

                    imp_Vec3f diff = HMM_SubV3(point, point_prev);
                    
                    f32 length = HMM_LenV3(diff);

                    // Determine transform
                    imp_Mat4 transform = HMM_Scale((imp_Vec3f){ command.point_list.thickness, length, command.point_list.thickness});

                    imp_Vec3f up;

                    if (HMM_EqV3(diff, (imp_Vec3f){ 0.0f, 1.0f, 0.0f }))
                        up = (imp_Vec3f){ 0.0f, 0.0f, 1.0f };
                    else
                        up = (imp_Vec3f){ 0.0f, 1.0f, 0.0f };

                    imp_Vec3f F = HMM_NormV3(diff);
                    imp_Vec3f S = HMM_NormV3(HMM_Cross(F, up));
                    imp_Vec3f U = HMM_Cross(S, F);

                    imp_Mat4 rot;

                    rot.Elements[0][0] = S.X;
                    rot.Elements[0][1] = S.Y;
                    rot.Elements[0][2] = S.Z;
                    rot.Elements[0][3] = 0.0f;

                    rot.Elements[1][0] = F.X;
                    rot.Elements[1][1] = F.Y;
                    rot.Elements[1][2] = F.Z;
                    rot.Elements[1][3] = 0.0f;

                    rot.Elements[2][0] = U.X;
                    rot.Elements[2][1] = U.Y;
                    rot.Elements[2][2] = U.Z;
                    rot.Elements[2][3] = 0.0f;

                    rot.Elements[3][0] = point_prev.X;
                    rot.Elements[3][1] = point_prev.Y;
                    rot.Elements[3][2] = point_prev.Z;
                    rot.Elements[3][3] = 1.0f;

                    transform = HMM_MulM4(rot, transform);

                    // Transpose for Raylib
                    transform = HMM_Transpose(transform);

                    default_ctx->instance_transforms[i - 1] = *(Matrix*)(transform.Elements);

                    point_prev = point;
                }

                Vector4 use_colorf = (Vector4){ use_color.R / 255.0f, use_color.G / 255.0f, use_color.B / 255.0f, use_color.A / 255.0f };

                SetShaderValue(default_ctx->curve_shader, default_ctx->curve_col_diffuse_loc, &use_colorf, SHADER_UNIFORM_VEC4);

                DrawMeshInstanced(default_ctx->cylinder, default_ctx->curve_material, default_ctx->instance_transforms, command.point_list.num_elements - 1);
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

            DrawCapsule((Vector3){ command.axis.start.X, command.axis.start.Y, command.axis.start.Z }, (Vector3){ command.axis.end.X, command.axis.end.Y, command.axis.end.Z }, command.axis.thickness, 8, 0,
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

    DrawText(TextFormat("FPS: %d", (int)(1.0 / HMM_MAX(0.0001, dt))), 20, 20, 30, RAYWHITE);

    EndMode2D();

    EndDrawing();

    return true;
}

b32 imp_default_backend_deinit() {
    if (default_ctx != NULL) {
        if (default_ctx->resources_ready) {
            UnloadMesh(default_ctx->cylinder);
            UnloadMesh(default_ctx->sphere);
            UnloadMaterial(default_ctx->curve_material);
        }

        if (default_ctx->window_open)
            CloseWindow();

        free(default_ctx->instance_transforms);

        free(default_ctx);
    }

    return true;
}
