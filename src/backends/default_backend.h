#pragma once

#include "../imp.h"

#include <raylib.h>

typedef struct {
    b32 window_open;
} imp_DefaultBackendContext;

b32 imp_default_backend_init();

b32 imp_default_backend_set_canvas(imp_Canvas canvas, const char* title);

b32 imp_default_backend_get_inputs(imp_Inputs* inputs);

b32 imp_default_backend_run_commands(imp_CommandList command_list);

b32 imp_default_backend_deinit();



