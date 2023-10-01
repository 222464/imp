#ifndef IMP_DEFAULT_BACKEND_HEADER
#define IMP_DEFAULT_BACKEND_HEADER

#include "../imp.h"

b32 imp_default_backend_init(imp_Backend *backend);
b32 imp_default_backend_deinit(imp_Backend *backend);

imp_Backend *imp_make_default_backend();
void imp_destroy_default_backend(imp_Backend *backend);

b32 imp_default_backend_set_canvas(imp_Backend *backend, imp_Canvas canvas, const char* title);
b32 imp_default_backend_set_camera(imp_Backend *backend, imp_Camera camera);
b32 imp_default_backend_get_inputs(imp_Backend *backend, imp_Inputs* inputs);
b32 imp_default_backend_run_commands(imp_Backend *backend, imp_CommandList command_list);

#endif
