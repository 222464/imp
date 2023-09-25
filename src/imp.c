#include "imp.h"

imp_Str imp_set_str(char* s) {
    imp_Str out = { .str = s, .len = 0 };

    while (*s++ != 0)
        out.len++;

    return out;
}

imp_Str imp_strfv(imp_Context* ctx, char* fmt, va_list args) {
    imp_Str result = (imp_Str){0};

    result.str = ctx->char_buffer + ctx->char_pos;
    result.len = vsprintf_s(ctx->char_buffer + ctx->char_pos, IMP_CHAR_BUFFER_SIZE - ctx->char_pos, fmt, args);

    ctx->char_pos += result.len + 1; // Include null terminator

    return result;
}

imp_Str imp_strf(imp_Context* ctx, char *fmt, ...) {
    va_list args;

    va_start(args, fmt);

    imp_Str result = imp_strfv(ctx, fmt, args);

    va_end(args);

    return result;
}
