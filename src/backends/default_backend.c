#include "default_backend.h"

#include <rlgl.h>
#include <raymath.h>

imp_DefaultBackendContext* default_ctx = NULL;

const char* curve_vs =
#include "shaders/curve.vs"
;

const char* curve_fs =
#include "shaders/curve.fs"
;

// This is from Raylib but modified to not use matrix transforms, we will handle that manually in a shader
void draw_instanced_data(Mesh mesh, Material material, imp_Vec3f* data, s32 instances) {
    unsigned int instancesVboId = 0;
    unsigned int instancesNextVboId = 0;

    // Bind shader program
    rlEnableShader(material.shader.id);

    // Send required data to shader (matrices, values)
    //-----------------------------------------------------
    // Upload to shader material.colDiffuse
    if (material.shader.locs[SHADER_LOC_COLOR_DIFFUSE] != -1)
    {
        float values[4] = {
            (float)material.maps[MATERIAL_MAP_DIFFUSE].color.r/255.0f,
            (float)material.maps[MATERIAL_MAP_DIFFUSE].color.g/255.0f,
            (float)material.maps[MATERIAL_MAP_DIFFUSE].color.b/255.0f,
            (float)material.maps[MATERIAL_MAP_DIFFUSE].color.a/255.0f
        };

        rlSetUniform(material.shader.locs[SHADER_LOC_COLOR_DIFFUSE], values, SHADER_UNIFORM_VEC4, 1);
    }

    // Upload to shader material.colSpecular (if location available)
    if (material.shader.locs[SHADER_LOC_COLOR_SPECULAR] != -1)
    {
        float values[4] = {
            (float)material.maps[SHADER_LOC_COLOR_SPECULAR].color.r/255.0f,
            (float)material.maps[SHADER_LOC_COLOR_SPECULAR].color.g/255.0f,
            (float)material.maps[SHADER_LOC_COLOR_SPECULAR].color.b/255.0f,
            (float)material.maps[SHADER_LOC_COLOR_SPECULAR].color.a/255.0f
        };

        rlSetUniform(material.shader.locs[SHADER_LOC_COLOR_SPECULAR], values, SHADER_UNIFORM_VEC4, 1);
    }

    // Get a copy of current matrices to work with,
    // just in case stereo render is required, and we need to modify them
    // NOTE: At this point the modelview matrix just contains the view matrix (camera)
    // That's because BeginMode3D() sets it and there is no model-drawing function
    // that modifies it, all use rlPushMatrix() and rlPopMatrix()
    Matrix matModel = MatrixIdentity();
    Matrix matView = rlGetMatrixModelview();
    Matrix matModelView = MatrixIdentity();
    Matrix matProjection = rlGetMatrixProjection();

    // Upload view and projection matrices (if locations available)
    if (material.shader.locs[SHADER_LOC_MATRIX_VIEW] != -1) rlSetUniformMatrix(material.shader.locs[SHADER_LOC_MATRIX_VIEW], matView);
    if (material.shader.locs[SHADER_LOC_MATRIX_PROJECTION] != -1) rlSetUniformMatrix(material.shader.locs[SHADER_LOC_MATRIX_PROJECTION], matProjection);

    // Enable mesh VAO to attach new buffer
    rlEnableVertexArray(mesh.vaoId);

    instancesVboId = rlLoadVertexBuffer(data, instances*sizeof(imp_Vec3f), false);

    rlEnableVertexAttribute(material.shader.locs[SHADER_LOC_MATRIX_MODEL]);
    rlSetVertexAttribute(material.shader.locs[SHADER_LOC_MATRIX_MODEL], 3, RL_FLOAT, 0, sizeof(imp_Vec3f), 0);
    rlSetVertexAttributeDivisor(material.shader.locs[SHADER_LOC_MATRIX_MODEL], 1);

    instancesNextVboId = rlLoadVertexBuffer(data + 1, instances*sizeof(imp_Vec3f), false);

    rlEnableVertexAttribute(material.shader.locs[SHADER_LOC_MATRIX_MODEL] + 1);
    rlSetVertexAttribute(material.shader.locs[SHADER_LOC_MATRIX_MODEL] + 1, 3, RL_FLOAT, 0, sizeof(imp_Vec3f), 0);
    rlSetVertexAttributeDivisor(material.shader.locs[SHADER_LOC_MATRIX_MODEL] + 1, 1);

    rlDisableVertexBuffer();
    rlDisableVertexArray();

    // Accumulate internal matrix transform (push/pop) and view matrix
    // NOTE: In this case, model instance transformation must be computed in the shader
    matModelView = MatrixMultiply(rlGetMatrixTransform(), matView);

    // Upload model normal matrix (if locations available)
    if (material.shader.locs[SHADER_LOC_MATRIX_NORMAL] != -1) rlSetUniformMatrix(material.shader.locs[SHADER_LOC_MATRIX_NORMAL], MatrixTranspose(MatrixInvert(matModel)));
    //-----------------------------------------------------

    // Bind active texture maps (if available)
    //for (int i = 0; i < MAX_MATERIAL_MAPS; i++)
    //{
    //    if (material.maps[i].texture.id > 0)
    //    {
    //        // Select current shader texture slot
    //        rlActiveTextureSlot(i);

    //        // Enable texture for active slot
    //        if ((i == MATERIAL_MAP_IRRADIANCE) ||
    //            (i == MATERIAL_MAP_PREFILTER) ||
    //            (i == MATERIAL_MAP_CUBEMAP)) rlEnableTextureCubemap(material.maps[i].texture.id);
    //        else rlEnableTexture(material.maps[i].texture.id);

    //        rlSetUniform(material.shader.locs[SHADER_LOC_MAP_DIFFUSE + i], &i, SHADER_UNIFORM_INT, 1);
    //    }
    //}

    // Try binding vertex array objects (VAO)
    // or use VBOs if not possible
    if (!rlEnableVertexArray(mesh.vaoId))
    {
        // Bind mesh VBO data: vertex position (shader-location = 0)
        rlEnableVertexBuffer(mesh.vboId[0]);
        rlSetVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_POSITION], 3, RL_FLOAT, 0, 0, 0);
        rlEnableVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_POSITION]);

        // Bind mesh VBO data: vertex texcoords (shader-location = 1)
        rlEnableVertexBuffer(mesh.vboId[1]);
        rlSetVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_TEXCOORD01], 2, RL_FLOAT, 0, 0, 0);
        rlEnableVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_TEXCOORD01]);

        if (material.shader.locs[SHADER_LOC_VERTEX_NORMAL] != -1)
        {
            // Bind mesh VBO data: vertex normals (shader-location = 2)
            rlEnableVertexBuffer(mesh.vboId[2]);
            rlSetVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_NORMAL], 3, RL_FLOAT, 0, 0, 0);
            rlEnableVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_NORMAL]);
        }

        // Bind mesh VBO data: vertex colors (shader-location = 3, if available)
        if (material.shader.locs[SHADER_LOC_VERTEX_COLOR] != -1)
        {
            if (mesh.vboId[3] != 0)
            {
                rlEnableVertexBuffer(mesh.vboId[3]);
                rlSetVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_COLOR], 4, RL_UNSIGNED_BYTE, 1, 0, 0);
                rlEnableVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_COLOR]);
            }
            else
            {
                // Set default value for unused attribute
                // NOTE: Required when using default shader and no VAO support
                float value[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
                rlSetVertexAttributeDefault(material.shader.locs[SHADER_LOC_VERTEX_COLOR], value, SHADER_ATTRIB_VEC4, 4);
                rlDisableVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_COLOR]);
            }
        }

        // Bind mesh VBO data: vertex tangents (shader-location = 4, if available)
        if (material.shader.locs[SHADER_LOC_VERTEX_TANGENT] != -1)
        {
            rlEnableVertexBuffer(mesh.vboId[4]);
            rlSetVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_TANGENT], 4, RL_FLOAT, 0, 0, 0);
            rlEnableVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_TANGENT]);
        }

        // Bind mesh VBO data: vertex texcoords2 (shader-location = 5, if available)
        if (material.shader.locs[SHADER_LOC_VERTEX_TEXCOORD02] != -1)
        {
            rlEnableVertexBuffer(mesh.vboId[5]);
            rlSetVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_TEXCOORD02], 2, RL_FLOAT, 0, 0, 0);
            rlEnableVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_TEXCOORD02]);
        }

        if (mesh.indices != NULL) rlEnableVertexBufferElement(mesh.vboId[6]);
    }

    // WARNING: Disable vertex attribute color input if mesh can not provide that data (despite location being enabled in shader)
    if (mesh.vboId[3] == 0) rlDisableVertexAttribute(material.shader.locs[SHADER_LOC_VERTEX_COLOR]);

    int eyeCount = 1;
    if (rlIsStereoRenderEnabled()) eyeCount = 2;

    for (int eye = 0; eye < eyeCount; eye++)
    {
        // Calculate model-view-projection matrix (MVP)
        Matrix matModelViewProjection = MatrixIdentity();
        if (eyeCount == 1) matModelViewProjection = MatrixMultiply(matModelView, matProjection);
        else
        {
            // Setup current eye viewport (half screen width)
            rlViewport(eye*rlGetFramebufferWidth()/2, 0, rlGetFramebufferWidth()/2, rlGetFramebufferHeight());
            matModelViewProjection = MatrixMultiply(MatrixMultiply(matModelView, rlGetMatrixViewOffsetStereo(eye)), rlGetMatrixProjectionStereo(eye));
        }

        // Send combined model-view-projection matrix to shader
        rlSetUniformMatrix(material.shader.locs[SHADER_LOC_MATRIX_MVP], matModelViewProjection);

        // Draw mesh instanced
        if (mesh.indices != NULL) rlDrawVertexArrayElementsInstanced(0, mesh.triangleCount*3, 0, instances);
        else rlDrawVertexArrayInstanced(0, mesh.vertexCount, instances);
    }

    // Unbind all bound texture maps
    //for (int i = 0; i < MAX_MATERIAL_MAPS; i++)
    //{
    //    if (material.maps[i].texture.id > 0)
    //    {
    //        // Select current shader texture slot
    //        rlActiveTextureSlot(i);

    //        // Disable texture for active slot
    //        if ((i == MATERIAL_MAP_IRRADIANCE) ||
    //            (i == MATERIAL_MAP_PREFILTER) ||
    //            (i == MATERIAL_MAP_CUBEMAP)) rlDisableTextureCubemap();
    //        else rlDisableTexture();
    //    }
    //}

    // Disable all possible vertex array objects (or VBOs)
    rlDisableVertexArray();
    rlDisableVertexBuffer();
    rlDisableVertexBufferElement();

    // Disable shader program
    rlDisableShader();

    // Remove instance transforms buffer
    rlUnloadVertexBuffer(instancesVboId);
    rlUnloadVertexBuffer(instancesNextVboId);
}

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

    inputs->mouse_down_left = IsMouseButtonDown(MOUSE_LEFT_BUTTON);
    inputs->mouse_down_right = IsMouseButtonDown(MOUSE_RIGHT_BUTTON);

    Vector2 mouse_pos = GetMousePosition();

    inputs->mouse = (imp_Vec2f){ mouse_pos.x, mouse_pos.y };

    inputs->mouse_scroll = GetMouseWheelMove();

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
        default_ctx->cylinder = GenMeshCylinder(1.0f, 1.0f, 3);
        default_ctx->sphere = GenMeshSphere(1.0f, 6, 6);

        default_ctx->curve_shader = LoadShaderFromMemory(curve_vs, curve_fs);

        default_ctx->curve_shader.locs[SHADER_LOC_MATRIX_MVP] = GetShaderLocation(default_ctx->curve_shader, "mvp");
        default_ctx->curve_shader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocationAttrib(default_ctx->curve_shader, "instancePosition");
        default_ctx->curve_shader.locs[SHADER_LOC_MATRIX_MODEL + 1] = GetShaderLocationAttrib(default_ctx->curve_shader, "instancePositionNext");
        default_ctx->curve_col_diffuse_loc = GetShaderLocation(default_ctx->curve_shader, "color");
        default_ctx->curve_thickness_loc = GetShaderLocation(default_ctx->curve_shader, "thickness");

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
                Vector4 use_colorf = (Vector4){ use_color.R / 255.0f, use_color.G / 255.0f, use_color.B / 255.0f, use_color.A / 255.0f };

                SetShaderValue(default_ctx->curve_shader, default_ctx->curve_col_diffuse_loc, &use_colorf, SHADER_UNIFORM_VEC4);
                SetShaderValue(default_ctx->curve_shader, default_ctx->curve_thickness_loc, &command.point_list.thickness, SHADER_UNIFORM_FLOAT);

                draw_instanced_data(default_ctx->cylinder, default_ctx->curve_material, command.point_list.data, command.point_list.num_elements - 1);
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

        free(default_ctx);
    }

    return true;
}
