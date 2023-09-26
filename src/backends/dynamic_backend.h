#pragma once

#include "../imp.h"

b32 imp_dynamic_backend_init(const char* path);

b32 imp_default_backend_set_canvas(imp_Canvas canvas, const char* title);

b32 imp_dynamic_backend_get_inputs(imp_Inputs* inputs);

b32 imp_dynamic_backend_run_commands(imp_CommandList command_list);

b32 imp_dynamic_backend_deinit();
