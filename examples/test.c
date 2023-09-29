#include "../src/imp.h"

int main() {
    imp_Context ctx;

    imp_init_default(&ctx);

    const s32 num_elements = 0x1 << 15;

    imp_Vec3f* data = (imp_Vec3f*)malloc(num_elements * sizeof(imp_Vec3f));

    for (int i = 0; i < num_elements; i++) {
        f32 r = (f32)i / (num_elements - 1) * HMM_PI * 2.0f;

        data[i] = (imp_Vec3f){ .X = sin(r * 3.0f) * sin(r * 5.0f - 0.3f), .Y = cos(r * 1.0f), .Z = sin(r * 2.0f + 0.5f) };
    }

    imp_canvas(&ctx, (imp_Canvas){ .size = { .X = 1280, .Y = 720 }, .clear_color = IMP_DARK_GRAY }, "Test");

    imp_Camera camera = IMP_DEFAULT_CAMERA3D;

    while (!ctx.inputs.exit) {
        imp_begin(&ctx);

        imp_camera(&ctx, camera);

        imp_point_list(&ctx, data, num_elements, IMP_POINT_LIST_STYLE_CURVE);

        imp_axes(&ctx, 10);

        imp_end(&ctx);
    }

    free(data);

    imp_deinit(&ctx);

    return 0;
}
