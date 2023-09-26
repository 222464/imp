#pragma once

#include "constructs.h"
#include "commands.h"

#include <stdarg.h>
#include <stdio.h>

#define IMP_CHAR_BUFFER_SIZE 0x1000

typedef struct {
    imp_Vec2f mouse;
    f32 mouse_scroll;
    s32 mouse_down;
    imp_Vec2f last_mouse;
    s32 mouse_pressed;
} imp_Inputs;

typedef struct {
    imp_Vec2i size;
} imp_Canvas;

typedef enum {
    IMP_BACKEND_TYPE_DEFAULT,
    IMP_BACKEND_TYPE_DYNAMIC
} imp_BackendType;

typedef struct {
    // pointers to the backend functions
    b32 (*backend_run_commands)(imp_CommandList);
    b32 (*backend_get_inputs)(imp_Inputs* inputs);

    imp_Inputs inputs;
    imp_Inputs inputs_prev; // last frame of input

    imp_Palette palette;

    char char_buffer[IMP_CHAR_BUFFER_SIZE];
    s32 char_pos;

    imp_CommandList command_list;

    imp_Canvas canvas;
} imp_Context;

imp_Str imp_set_str(char* s);

imp_Str imp_strfv(imp_Context* ctx, char* fmt, va_list args);
imp_Str imp_strf(imp_Context* ctx, char* fmt, ...);

// Main immediate mode interface
void imp_init(imp_Context* ctx, b32 (*backend_run_commands)(imp_CommandList), b32 (*backend_get_inputs)(imp_Inputs* inputs));
void imp_init_default(imp_Context* ctx);
void imp_init_dynamic(imp_Context* ctx);

void imp_begin(imp_Context* ctx);
void imp_end(imp_Context* ctx);

void imp_begin_plot();
