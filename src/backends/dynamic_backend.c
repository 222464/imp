#include "dynamic_backend.h"

#if defined(__linux__) || defined(__linux) || defined(linux) || defined(_LINUX)
#define IMP_HAS_LINUX
#include <dlfcn.h>
#elif defined(_WIN32) || defined(_WIN64)
#define IMP_HAS_WINDOWS
#elif defined(__APPLE__)
#define IMP_HAS_MACOS
#else
#error "Unsupported OS for dynamic backend"
#endif

b32 imp_dynamic_backend_init(const char* path) {
#ifdef IMP_HAS_LINUX
    
#endif
    return true;
}

b32 imp_dynamic_backend_set_canvas(imp_Canvas canvas, const char* title) {
    return true;
}

b32 imp_dynamic_backend_set_camera(imp_Camera camera) {
    return true;
}

b32 imp_dynamic_backend_get_inputs(imp_Inputs* inputs) {
    return true;
}

b32 imp_dynamic_backend_run_commands(imp_CommandList command_list) {
    return true;
}

b32 imp_dynamic_backend_deinit() {
    return true;
}
