#pragma once

#include "constructs.h"
#include "commands.h"

#include <stdarg.h>
#include <stdio.h>

#define IMP_CHAR_BUFFER_SIZE 0x1000

typedef struct {
    char* str;
    s32 len;
} imp_Str;

typedef struct {
    imp_Vec2f mouse;
    f32 mouse_scroll;
    s32 mouse_down;
    imp_Vec2f last_mouse;
    s32 mouse_pressed;
} imp_Inputs;

// Colors (or default colors) of various elements for some basic themeing
typedef struct {
    imp_Color background;
    imp_Color foreground;
    imp_Color curve;
    imp_Color axis;
} imp_Palette;

typedef u32 imp_ID;

typedef struct {
    imp_Inputs inputs;

    imp_Palette palette;

    char char_buffer[IMP_CHAR_BUFFER_SIZE];
    s32 char_pos;

    imp_CommandList command_list;
} imp_Context;

imp_Str imp_set_str(char* s);

imp_Str imp_strfv(imp_Context* ctx, char* fmt, va_list args);
imp_Str imp_strf(imp_Context* ctx, char *fmt, ...);
