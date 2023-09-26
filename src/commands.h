#pragma once

#include "constructs.h"

#define IMP_MAX_COMMANDS 1024

// ----------------------------------- Commands -----------------------------------

typedef struct {
    imp_Vec2i size;
    imp_Camera camera;
    b32 visible;
} imp_CommandSetCanvas;

typedef enum {
    IMP_ARRAY_STYLE_CURVE = 0,
    IMP_ARRAY_STYLE_POINTS
} imp_ArrayStyle;

typedef struct {
    imp_Vec3f* curve;
    s32 num_elements;

    imp_ArrayStyle style;
} imp_CommandDrawArray;

typedef struct {
    imp_Vec3f start;
    imp_Vec3f end;
    f32 thickness;
    
    imp_Vec2f range;
    s32 num_ticks;
    f32 tick_thickness;
} imp_CommandDrawAxis;

typedef struct {
    void* user_data;
} imp_CommandCustom;

typedef enum {
    IMP_COMMAND_DRAW_ARRAY,
    IMP_COMMAND_DRAW_AXIS,
    IMP_COMMAND_CUSTOM
} imp_CommandType;

typedef struct {
    imp_CommandType type;

    union {
        imp_CommandDrawArray array;
        imp_CommandDrawAxis axis;
        imp_CommandCustom custom;
    };
} imp_Command;

typedef struct {
    imp_Command commands[IMP_MAX_COMMANDS];
    s32 num_commands;
} imp_CommandList;

void imp_command_list_clear(imp_CommandList command_list);
b32 imp_command_list_add(imp_CommandList command_list, imp_Command command);
