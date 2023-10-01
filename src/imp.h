#ifndef IMP_HEADER
#define IMP_HEADER

#include "constructs.h"
#include "commands.h"

#include <stdarg.h>
#include <stdio.h>

#define IMP_CHAR_BUFFER_SIZE 0x1000
#define IMP_DEFAULT_THICKNESS 0.005f
#define IMP_DEFAULT_TICK_THICKNESS 0.015f

#define IMP_EPSILON 0.00001f

#ifndef IMP_MALLOC
#include <stdlib.h>
#define IMP_MALLOC(size) (malloc((size)))
#endif
#ifndef IMP_FREE
#define IMP_FREE(ptr) free((ptr))
#endif

typedef struct imp_Inputs {
    b32 exit;
    imp_Vec2f mouse;
    f32 mouse_scroll;
    s32 mouse_down_left;
    s32 mouse_down_right;
} imp_Inputs;

typedef struct imp_Canvas {
    imp_Vec2i size;
    imp_Color clear_color;
} imp_Canvas;

typedef struct imp_Backend {
	// Backend data pointer.
	void *data;
    // pointers to the backend functions
	b32 (*init)(struct imp_Backend *);
	b32 (*deinit)(struct imp_Backend *);
    b32 (*set_canvas)(struct imp_Backend *, imp_Canvas, const char *);
    b32 (*set_camera)(struct imp_Backend *, imp_Camera);
    b32 (*get_inputs)(struct imp_Backend *, imp_Inputs *);
    b32 (*run_commands)(struct imp_Backend *, imp_CommandList);
} imp_Backend;

typedef struct imp_Context {
	imp_Backend *backend;

    imp_Inputs inputs;
    imp_Inputs inputs_prev; // last frame of input

    imp_Palette palette;

    char char_buffer[IMP_CHAR_BUFFER_SIZE];
    s32 char_pos;

    imp_CommandList command_list;

    imp_Canvas canvas;
    imp_Camera camera;
} imp_Context;

imp_Str imp_set_str(char* s);

imp_Str imp_strfv(imp_Context* ctx, char* fmt, va_list args);
imp_Str imp_strf(imp_Context* ctx, char* fmt, ...);

// Main immediate mode interface
void imp_init(imp_Context* ctx, imp_Backend *backend);
void imp_deinit(imp_Context* ctx);

imp_Context *imp_make_context(imp_Backend *backend);
void imp_destroy_context(imp_Context *ctx);

void imp_canvas(imp_Context* ctx, imp_Canvas canvas, const char* title);

void imp_begin(imp_Context* ctx);
void imp_end(imp_Context* ctx);

void imp_camera(imp_Context* ctx, imp_Camera camera);
void imp_orbit_camera(imp_Context* ctx, f32 move_sensitivity, f32 zoom_sensitivity);

void imp_point_list(imp_Context* ctx, imp_Vec3f* data, s32 num_elements, imp_PointListStyle style);
void imp_point_list_ex(imp_Context* ctx, imp_Vec3f* data, s32 num_elements, imp_PointListStyle style, imp_Color color, f32 thickness);
void imp_axis(imp_Context* ctx, imp_Vec3f start, imp_Vec3f end, s32 num_ticks);
void imp_axis_ex(imp_Context* ctx, imp_Vec3f start, imp_Vec3f end, s32 num_ticks, imp_Color color, f32 thickness, f32 tick_thickness);
void imp_axes(imp_Context* ctx, s32 num_ticks);
void imp_axes_ex(imp_Context* ctx, s32 num_ticks, imp_Color color, f32 thickness, f32 tick_thickness);

#endif
