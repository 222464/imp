#include "../src/imp.h"
int main() {
    // Bunch of data
    const s32 num_elements = 0x1 << 10;

    imp_Vec3f* data0 = (imp_Vec3f*)malloc(num_elements * sizeof(imp_Vec3f));

    for (int i = 0; i < num_elements; i++) {
        f32 r = (f32)i / (num_elements - 1) * HMM_PI * 2.0f;

        data0[i] = (imp_Vec3f){ .X = sin(r * 3.0f) * sin(r * 5.0f - 0.3f), .Y = sin(r * 1.0f) * sin(r * 8.0f + 1.5f), .Z = sin(r * 2.0f + 0.5f) * sin(r * 7.0f - 0.9f) };
    }

    imp_Vec3f* data1 = (imp_Vec3f*)malloc(num_elements * sizeof(imp_Vec3f));

    for (int i = 0; i < num_elements; i++) {
        f32 r = (f32)i / (num_elements - 1) * HMM_PI * 2.0f;

        data1[i] = (imp_Vec3f){ .X = cos(r * 3.0f) * sin(r * 5.0f - 0.3f), .Y = cos(r * 1.0f) * sin(r * 8.0f + 1.5f), .Z = cos(r * 2.0f + 0.5f) * sin(r * 7.0f - 0.9f) };
    }

    imp_Vec3f* data2 = (imp_Vec3f*)malloc(num_elements * sizeof(imp_Vec3f));

    for (int i = 0; i < num_elements; i++) {
        f32 r = (f32)i / (num_elements - 1) * HMM_PI * 2.0f;

        data2[i] = (imp_Vec3f){ .X = sin(r * 3.0f) * cos(r * 5.0f - 0.3f), .Y = sin(r * 1.0f) * cos(r * 8.0f + 1.5f), .Z = sin(r * 2.0f + 0.5f) * cos(r * 7.0f - 0.9f) };
    }

    // IMP code
    imp_Context ctx;

    imp_init_default(&ctx);

    imp_canvas(&ctx, (imp_Canvas){ .size = { .X = 1280, .Y = 720 }, .clear_color = IMP_DARK_GRAY }, "Test");

    imp_Camera camera = IMP_DEFAULT_CAMERA3D;

    while (!ctx.inputs.exit) {
        imp_orbit_camera(&ctx, 0.01f, 0.1f);

        imp_begin(&ctx);

        imp_point_list_ex(&ctx, data0, num_elements, IMP_POINT_LIST_STYLE_CURVE, IMP_RED, IMP_DEFAULT_THICKNESS);
        imp_point_list_ex(&ctx, data1, num_elements, IMP_POINT_LIST_STYLE_CURVE, IMP_GREEN, IMP_DEFAULT_THICKNESS);
        imp_point_list_ex(&ctx, data2, num_elements, IMP_POINT_LIST_STYLE_CURVE, IMP_BLUE, IMP_DEFAULT_THICKNESS);

        imp_axes(&ctx, 10);

        imp_end(&ctx);
    }

    free(data0);
    free(data1);

    imp_deinit(&ctx);

    return 0;
}
