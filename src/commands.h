#pragma once

#include "constructs.h"

#define MAX_COMMANDS 1024

// ----------------------------------- Commands -----------------------------------

typedef struct {
    imp_Vec2i size;
} imp_CommandSetCanvas;

typedef struct {
    imp_Camera camera;
} imp_CommandSetCamera;

typedef struct {
    imp_Vec3f* curve;
    s32 num_elements;
} imp_CommandDrawCurve;

typedef struct {
    imp_Vec3f* points;
    s32 num_elements;
} imp_CommandDrawPoints;

typedef struct {
    imp_Vec3f start;
    imp_Vec3f end;
    f32 thickness;
    
    imp_Vec2f range;
    int num_ticks;
    f32 tick_thickness;
} imp_CommandDrawAxis;

typedef struct {
    void* user_data;
} imp_CommandCustom;

typedef union {
    imp_CommandSetCanvas canvas;
    imp_CommandSetCamera camera;
    imp_CommandDrawCurve curve;
    imp_CommandDrawPoints points;
    imp_CommandDrawAxis axis;
    imp_CommandCustom custom;
} imp_Command;

typedef struct {
    imp_Command commands[MAX_COMMANDS];
    int num_commands;
} imp_CommandList;

void imp_command_list_clear(imp_CommandList* command_list);
b32 imp_command_list_add(imp_CommandList* command_list, const imp_Command* command);
