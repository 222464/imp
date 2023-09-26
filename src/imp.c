#include "imp.h"

#include "backends/default_backend.h"
#include "backends/dynamic_backend.h"

void imp_set_default_palette(imp_Palette* palette) {
    palette->background = (imp_Color){ .r = 0, .g = 0, .b = 0, .a = 255 };
    palette->grid = (imp_Color){ .r = 20, .g = 20, .b = 20, .a = 255 };
    palette->array = (imp_Color){ .r = 255, .g = 0, .b = 0, .a = 255 };
    palette->axis = (imp_Color){ .r = 192, .g = 192, .b = 192, .a = 255 };
}

imp_Str imp_set_str(char* s) {
    imp_Str out = { .str = s, .len = 0 };

    while (*s++ != 0)
        out.len++;

    return out;
}

imp_Str imp_strfv(imp_Context* ctx, char* fmt, va_list args) {
    imp_Str result = (imp_Str){0};

    result.str = ctx->char_buffer + ctx->char_pos;
    result.len = IMP_VSNSPRINTF(ctx->char_buffer + ctx->char_pos, IMP_CHAR_BUFFER_SIZE - ctx->char_pos, fmt, args);

    ctx->char_pos += result.len + 1; // Include null terminator

    return result;
}

imp_Str imp_strf(imp_Context* ctx, char* fmt, ...) {
    va_list args;

    va_start(args, fmt);

    imp_Str result = imp_strfv(ctx, fmt, args);

    va_end(args);

    return result;
}

void imp_init(imp_Context* ctx, b32 (*backend_run_commands)(imp_CommandList), b32 (*backend_get_inputs)(imp_Inputs* inputs)) {
    *ctx = (imp_Context){0};

    ctx->backend_run_commands = backend_run_commands;
    ctx->backend_get_inputs = backend_get_inputs;

    imp_set_default_palette(&ctx->palette);
}

void imp_init_default(imp_Context* ctx) {
    imp_init(ctx, &imp_default_backend_run_commands, &imp_default_backend_get_inputs);
}

void imp_init_dynamic(imp_Context* ctx) {
    imp_init(ctx, &imp_dynamic_backend_run_commands, &imp_dynamic_backend_get_inputs);
}

void imp_begin(imp_Context* ctx) {
    // Get inputs
    (*ctx->backend_get_inputs)(&ctx->inputs);

    ctx->char_pos = 0;
    ctx->command_list.num_commands = 0;
}

void imp_end(imp_Context* ctx) {

}
