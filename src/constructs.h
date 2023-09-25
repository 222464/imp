#pragma once

#include "utils.h"

#include <math.h>
#include <memory.h>

// ----------------------------------- Vector -----------------------------------

typedef union {
    struct{ s32 x, y; };
    s32 data[2];
} imp_Vec2i;

typedef struct {
    struct{ s32 x, y, z, w; };
    s32 data[4];
} imp_Vec4i;

typedef imp_Vec4i imp_Vec3i;

typedef struct {
    struct{ f32 x, y; };
    f32 data[2];
} imp_Vec2f;

typedef struct {
    struct{ f32 x, y, z, w; };
    f32 data[4];
} imp_Vec4f;

typedef imp_Vec4f imp_Vec3f;

// ----------------------------------- Matrix -----------------------------------

// Basic matrix type, column major
typedef struct {
    f32 data[16];
} imp_Mat4;

void imp_mat_set_zero(imp_Mat4* mat);
void imp_mat_set_identity(imp_Mat4* mat);
void imp_mat_set_ortho(imp_Mat4* mat, f32 left, f32 right, f32 bottom, f32 top, f32 z_near, f32 z_far);
void imp_mat_set_perspective(imp_Mat4* mat, f32 fov_y, f32 aspect, f32 z_near, f32 z_far);
void imp_mat_set_translation(imp_Mat4* mat, imp_Vec3f position);
void imp_mat_set_rotation_x(imp_Mat4* mat, f32 angle);
void imp_mat_set_rotation_y(imp_Mat4* mat, f32 angle);
void imp_mat_set_rotation_z(imp_Mat4* mat, f32 angle);
void imp_mat_mul_vec3(imp_Vec3f* result, const imp_Mat4* mat, imp_Vec3f vec);
void imp_mat_mul_mat(imp_Mat4* result, const imp_Mat4* left, const imp_Mat4* right);

// ----------------------------------- Color -----------------------------------

typedef union {
    struct { u8 r, g, b, a; };
    u8 data[4];
    u32 raw;
} imp_Color;

// ----------------------------------- Camera -----------------------------------

typedef struct {
    imp_Mat4 projection_view;
} imp_Camera;
