#include "../src/imp.h"

int main() {
    imp_Context ctx;

    imp_init_default(&ctx);

    imp_Vec3f data[32];

    for (int i = 0; i < 32; i++) {
        data[i] = (imp_Vec3f){ .X = sin(i * 0.1f), .Y = cos(i * 0.1f), .Z = 0.0f };
    }

    imp_canvas(&ctx, (imp_Canvas){ .size = { .X = 1280, .Y = 720 }, .clear_color = IMP_DARK_GRAY }, "Test");

    imp_Camera camera = IMP_DEFAULT_CAMERA3D;

    camera.position = (imp_Vec3f){ 4.0f, 4.0f, 4.0f };

    while (!ctx.inputs.exit) {
        imp_begin(&ctx);

        imp_camera(&ctx, camera);

        imp_point_list(&ctx, data, 32, IMP_POINT_LIST_STYLE_CURVE);

        imp_end(&ctx);
    }

    imp_deinit(&ctx);

    return 0;
}
