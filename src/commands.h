#pragma once

#include "constructs.h"

#define IMP_MAX_COMMANDS 1024

// ----------------------------------- Commands -----------------------------------

typedef imp_Camera imp_CommandSetCamera;

typedef enum {
    IMP_POINT_LIST_STYLE_CURVE = 0,
    IMP_POINT_LIST_STYLE_POINTS
} imp_PointListStyle;

typedef struct {
    imp_Vec3f* data;
    s32 num_elements;

    imp_PointListStyle style;

    f32 thickness;

    imp_Color color;
} imp_CommandDrawPointList;

typedef struct {
    imp_Vec3f start;
    imp_Vec3f end;
    f32 thickness;
    
    imp_Vec2f range;
    s32 num_ticks;
    f32 tick_thickness;

    imp_Color color;
} imp_CommandDrawAxis;

typedef struct {
    imp_Vec3f position;
    f32 size;
    imp_Str text;
    
    imp_Color color;
} imp_CommandDrawAnnotation;

typedef struct {
    void* user_data;
} imp_CommandCustom;

typedef enum {
    IMP_COMMAND_SET_CAMERA,
    IMP_COMMAND_DRAW_POINT_LIST,
    IMP_COMMAND_DRAW_AXIS,
    IMP_COMMAND_DRAW_ANNOTATION,
    IMP_COMMAND_CUSTOM
} imp_CommandType;

typedef struct {
    imp_CommandType type;

    union {
        imp_CommandSetCamera camera;
        imp_CommandDrawPointList point_list;
        imp_CommandDrawAxis axis;
        imp_CommandDrawAnnotation annotation;
        imp_CommandCustom custom;
    };
} imp_Command;

typedef struct {
    imp_Command commands[IMP_MAX_COMMANDS];
    s32 num_commands;
} imp_CommandList;

void imp_command_list_clear(imp_CommandList command_list);
b32 imp_command_list_add(imp_CommandList command_list, imp_Command command);
