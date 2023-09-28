#pragma once

#include "../imp.h"

#include <raylib.h>

#define IMP_DEFAULT_BACKEND_MAX_INSTANCE_TRANFORMS 4096

typedef struct {
    b32 window_open;
    imp_Camera camera;
    imp_Canvas canvas;
    Mesh cylinder;
    Mesh sphere;
    Shader curve_shader;
    Material curve_material;
    s32 curve_col_diffuse_loc;

    Matrix instance_transforms[IMP_DEFAULT_BACKEND_MAX_INSTANCE_TRANFORMS];

    b32 resources_ready;
} imp_DefaultBackendContext;

b32 imp_default_backend_init();

b32 imp_default_backend_set_canvas(imp_Canvas canvas, const char* title);

b32 imp_default_backend_set_camera(imp_Camera camera);

b32 imp_default_backend_get_inputs(imp_Inputs* inputs);

b32 imp_default_backend_run_commands(imp_CommandList command_list);

b32 imp_default_backend_deinit();



