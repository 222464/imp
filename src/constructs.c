#include "constructs.h"

void imp_mat_set_zero(imp_Mat4* mat) {
    memset(mat->data, 0, sizeof(mat->data));
}

void imp_mat_set_identity(imp_Mat4* mat) {
    imp_mat_set_zero(mat);
    
    for (int i = 0; i < 4; i++)
        mat->data[i * 5] = 1.0f;
}

void imp_mat_set_ortho(imp_Mat4* mat, f32 left, f32 right, f32 bottom, f32 top, f32 z_near, f32 z_far) {
    f32 right_minus_left_inv = 1.0f / (right - left);
    f32 top_minus_bottom_inv = 1.0f / (top - bottom);
    f32 far_minus_near_inv = 1.0f / (z_far - z_near);

    mat->data[0] = 2.0f * right_minus_left_inv;
    mat->data[1] = 0.0f;
    mat->data[2] = 0.0f;
    mat->data[3] = 0.0f;
    mat->data[4] = 0.0f;
    mat->data[5] = 2.0f * top_minus_bottom_inv;
    mat->data[6] = 0.0f;
    mat->data[7] = 0.0f;
    mat->data[8] = 0.0f;
    mat->data[9] = 0.0f;
    mat->data[10] = -2.0f * far_minus_near_inv;
    mat->data[11] = 0.0f;
    mat->data[12] = -(right + left) * right_minus_left_inv;
    mat->data[13] = -(top + bottom) * top_minus_bottom_inv;
    mat->data[14] = -(z_far + z_near) * far_minus_near_inv;
    mat->data[15] = 1.0f;
}

void imp_mat_set_perspective(imp_Mat4* mat, f32 fov_y, f32 aspect, f32 z_near, f32 z_far) {
    f32 f = 1.0f / tanf(0.5f * fov_y);

    f32 near_minus_far = z_near - z_far;

    mat->data[0] = f / aspect;
    mat->data[1] = 0.0f;
    mat->data[2] = 0.0f;
    mat->data[3] = 0.0f;
    mat->data[4] = 0.0f;
    mat->data[5] = f;
    mat->data[6] = 0.0f;
    mat->data[7] = 0.0f;
    mat->data[8] = 0.0f;
    mat->data[9] = 0.0f;
    mat->data[10] = (z_near + z_far) / near_minus_far;
    mat->data[11] = -1.0f;
    mat->data[12] = 0.0f;
    mat->data[13] = 0.0f;
    mat->data[14] = (2.0f * z_near * z_far) / near_minus_far;
    mat->data[15] = 0.0f;
}

void imp_mat_set_translation(imp_Mat4* mat, imp_Vec3f position) {
    imp_mat_set_identity(mat);

    mat->data[12] = position.x;
    mat->data[13] = position.y;
    mat->data[14] = position.z;
}

void imp_mat_set_rotation_x(imp_Mat4* mat, f32 angle) {
    f32 s = sin(angle);
    f32 c = cos(angle);

    imp_mat_set_identity(mat);

    mat->data[5] = c;
    mat->data[9] = s;
    mat->data[6] = -s;
    mat->data[10] = c;
}

void imp_mat_set_rotation_y(imp_Mat4* mat, f32 angle) {
    f32 s = sin(angle);
    f32 c = cos(angle);

    imp_mat_set_identity(mat);

    mat->data[0] = c;
    mat->data[8] = -s;
    mat->data[2] = s;
    mat->data[10] = c;
}

void imp_mat_set_rotation_z(imp_Mat4* mat, f32 angle) {
    f32 s = sin(angle);
    f32 c = cos(angle);

    imp_mat_set_identity(mat);

    mat->data[0] = c;
    mat->data[4] = s;
    mat->data[1] = -s;
    mat->data[5] = c;
}

void imp_mat_mul_vec3(imp_Vec3f* result, const imp_Mat4* mat, imp_Vec3f vec) {
    for (int i = 0; i < 4; i++) {
        f32 s = 0.0f;

        for (int k = 0; k < 4; k++)
            s += mat->data[i + 4 * k] * vec.data[k];

        result->data[i] = s;
    }
}

void imp_mat_mul_mat(imp_Mat4* result, const imp_Mat4* left, const imp_Mat4* right) {
    for (int j = 0; j < 4; j++) {
        for (int i = 0; i < 4; i++) {
            f32 s = 0.0f;

            for (int k = 0; k < 4; k++)
                s += left->data[i + 4 * k] * right->data[k + 4 * j];

            result->data[i + 4 * j] = s;
        }
    }
}
