#include "default_backend.h"

imp_DefaultBackendContext* default_ctx = NULL;

b32 imp_default_backend_init() {
    assert(default_ctx == NULL && "Default backend already initialized!");

    // Allocate context for this backend
    default_ctx = (imp_DefaultBackendContext*)malloc(sizeof(imp_DefaultBackendContext));

    (*default_ctx) = (imp_DefaultBackendContext){0};

    return true;
}

b32 imp_default_backend_get_inputs(imp_Inputs* inputs) {
    return true;
}

b32 imp_default_backend_set_canvas(imp_Canvas canvas, const char* title) {
    assert(default_ctx != NULL && "Need a context in order to make a canvas!");

    // If already open, make a new one
    if (default_ctx->window_open)
        CloseWindow();

    InitWindow(canvas.size.x, canvas.size.y, title);

    return true;
}

b32 imp_default_backend_run_commands(imp_CommandList command_list) {
    for (s32 i = 0; i < command_list.num_commands; i++) {
        imp_Command command = command_list.commands[i];

        switch (command.type) {
        case IMP_COMMAND_DRAW_ARRAY:

            break;
        default:
            break;
        }
    }

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
