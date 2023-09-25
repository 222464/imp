#include "../../../src/backend.h"

#include <raylib.h>
#include <stdbool.h>

// Backend global state
b32 imp_backend_init() {
    InitWindow(

    return true;
}

b32 imp_backend_get_inputs(imp_Inputs* inputs) {

    return true;
}

b32 imp_backend_run_commands(imp_CommandList command_list) {

    return true;
}

b32 imp_backend_close() {

    return true;
}

