#pragma once

#include "utils.h"

#include "third_party/HandmadeMath.h"
#include <math.h>
#include <memory.h>

// ----------------------------------- String -----------------------------------

typedef struct {
    char* str;
    s32 len;
} imp_Str;


// ----------------------------------- Vector -----------------------------------

// For integer vectors, make simple structs
typedef union {
    struct{ s32 X, Y; };
    s32 data[2];
} imp_Vec2i;

typedef struct {
    struct{ s32 X, Y, Z, W; };
    s32 data[4];
} imp_Vec4i;

typedef imp_Vec4i imp_Vec3i;

// cleaning up the naming
typedef HMM_Vec2 imp_Vec2f;
typedef HMM_Vec3 imp_Vec3f;
typedef HMM_Vec4 imp_Vec4f;

// ----------------------------------- Matrix -----------------------------------

typedef HMM_Mat4 imp_Mat4;

// ----------------------------------- Color -----------------------------------

typedef union {
    struct { u8 R, G, B, A; };
    u8 data[4];
    u32 raw;
} imp_Color;

#define IMP_WHITE ((imp_Color){ 255, 255, 255, 255 })
#define IMP_BLACK ((imp_Color){ 0, 0, 0, 255 })
#define IMP_RED ((imp_Color){ 255, 0, 0, 255 })
#define IMP_GREEN ((imp_Color){ 0, 255, 0, 255 })
#define IMP_BLUE ((imp_Color){ 0, 0, 255, 255 })

#define IMP_DARK_GRAY ((imp_Color){ 20, 20, 20, 255 })
#define IMP_LIGHT_GRAY ((imp_Color){ 60, 60, 60, 255 })
#define IMP_OFF_WHITE ((imp_Color){ 220, 220, 220, 255 })

// Colors (or default colors) of various elements for some basic themeing
typedef struct {
    imp_Color background;
    imp_Color grid;
    imp_Color point_list;
    imp_Color axis;
} imp_Palette;

#define IMP_PALETTE_DARK_MODE \
((imp_Palette){ \
    .background = IMP_DARK_GRAY, \
    .grid = IMP_LIGHT_GRAY, \
    .point_list = IMP_RED, \
    .axis = IMP_OFF_WHITE \
})


// ----------------------------------- Camera -----------------------------------

typedef enum {
    IMP_CAMERA_ORTHO,
    IMP_CAMERA_PERSPECTIVE
} imp_CameraMode;

typedef struct {
    imp_Vec3f position;
    imp_Vec3f target;
    imp_Vec3f up;
    f32 fov_y;
    f32 zoom;
    f32 rotation;
    imp_CameraMode mode;
} imp_Camera;

#define IMP_DEFAULT_CAMERA2D  \
(imp_Camera imp_default_camera { \
    .position = { 0.0f, 0.0f, 1.0f }, \
    .target = { 0.0f, 0.0f, 0.0f }, \
    .up = { 0.0f, 1.0f, 0.0f }, \
    .fov_y = 0.0f, \
    .zoom = 1.0f, \
    .rotation = 0.0f, \
    .mode = IMP_CAMERA_ORTHO \
})

#define IMP_DEFAULT_CAMERA3D  \
(imp_Camera imp_default_camera { \
    .position = { 1.0f, 1.0f, 1.0f }, \
    .target = { 0.0f, 0.0f, 0.0f }, \
    .up = { 0.0f, 1.0f, 0.0f }, \
    .fov_y = HMM_PI * 0.5f, \
    .zoom = 0.0f, \
    .rotation = 0.0f, \
    .mode = IMP_CAMERA_PERSPECTIVE \
})
