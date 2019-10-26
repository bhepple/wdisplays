/* SPDX-License-Identifier: MIT */
/* Copyright (C) 2019 cyclopsian
 * Copyright (C) 2017-2019 emersion */

/*
 * Parts of this file are taken from emersion/kanshi:
 * https://github.com/emersion/kanshi/blob/38d27474b686fcc8324cc5e454741a49577c0988/include/kanshi.h
 * https://github.com/emersion/kanshi/blob/38d27474b686fcc8324cc5e454741a49577c0988/include/config.h
 */

#ifndef WDISPLAY_WDISPLAY_H
#define WDISPLAY_WDISPLAY_H

#define HEADS_MAX 64
#define HOVER_USECS (100 * 1000)

#include <stdbool.h>
#include <wayland-client.h>

struct zxdg_output_v1;
struct zxdg_output_manager_v1;
struct zwlr_output_mode_v1;
struct zwlr_output_head_v1;
struct zwlr_output_manager_v1;
struct zwlr_export_dmabuf_manager_v1;
struct zwlr_export_dmabuf_frame_v1;
struct zwlr_screencopy_manager_v1;
struct zwlr_screencopy_frame_v1;
struct zwlr_layer_shell_v1;
struct zwlr_layer_surface_v1;

struct _GtkWidget;
typedef struct _GtkWidget GtkWidget;
struct _GtkBuilder;
typedef struct _GtkBuilder GtkBuilder;
struct _GdkCursor;
typedef struct _GdkCursor GdkCursor;
struct _cairo_surface;
typedef struct _cairo_surface cairo_surface_t;

typedef void *EGLImageKHR;

enum wd_head_fields {
  WD_FIELD_NAME           = 1 << 0,
  WD_FIELD_ENABLED        = 1 << 1,
  WD_FIELD_DESCRIPTION    = 1 << 2,
  WD_FIELD_PHYSICAL_SIZE  = 1 << 3,
  WD_FIELD_SCALE          = 1 << 4,
  WD_FIELD_POSITION       = 1 << 5,
  WD_FIELD_MODE           = 1 << 6,
  WD_FIELD_TRANSFORM      = 1 << 7,
  WD_FIELDS_ALL           = (1 << 8) - 1
};

struct wd_output {
  struct wd_state *state;
  struct zxdg_output_v1 *xdg_output;
  struct wl_output *wl_output;
  struct wl_list link;

  char *name;
  struct wl_list frames;
  GtkWidget *overlay_window;
  struct zwlr_layer_surface_v1 *overlay_layer_surface;
};

#define WD_MAX_PLANES 4

struct wd_frame {
  struct wd_output *output;
  struct zwlr_export_dmabuf_frame_v1 *export_frame;
  struct zwlr_screencopy_frame_v1 *copy_frame;

  struct wl_list link;

  unsigned width;
  unsigned height;
  uint64_t tick;

  uint32_t format;
  uint32_t flags;
  uint64_t modifier;
  uint32_t n_planes;
  int fds[WD_MAX_PLANES];
  uint32_t offsets[WD_MAX_PLANES];
  uint32_t strides[WD_MAX_PLANES];
  struct wl_shm_pool *pool;
  struct wl_buffer *buffer;
  uint8_t *pixels;
  bool ready;
};

struct wd_head_config {
  struct wl_list link;

  struct wd_head *head;
  bool enabled;
  int32_t width;
  int32_t height;
  int32_t refresh; // mHz
  int32_t x;
  int32_t y;
  double scale;
  enum wl_output_transform transform;
};

struct wd_mode {
  struct wd_head *head;
  struct zwlr_output_mode_v1 *wlr_mode;
  struct wl_list link;

  int32_t width, height;
  int32_t refresh; // mHz
  bool preferred;
};

struct wd_head {
  struct wd_state *state;
  struct zwlr_output_head_v1 *wlr_head;
  struct wl_list link;

  struct wd_output *output;
  struct wd_render_head_data *render;
  cairo_surface_t *surface;

  uint32_t id;
  char *name, *description;
  int32_t phys_width, phys_height; // mm
  struct wl_list modes;

  bool enabled;
  struct wd_mode *mode;
  struct {
    int32_t width, height;
    int32_t refresh;
  } custom_mode;
  int32_t x, y;
  enum wl_output_transform transform;
  double scale;
};

struct wd_gl_data;

struct wd_render_head_flags {
  uint8_t rotation;
  bool x_invert;
};

struct wd_render_head_data {
  struct wl_list link;
  uint64_t updated_at;
  uint64_t hover_begin;
  uint64_t click_begin;

  float x1;
  float y1;
  float x2;
  float y2;

  struct wd_render_head_flags queued;
  struct wd_render_head_flags active;

  uint8_t *pixels;
  EGLImageKHR image;
  unsigned tex_stride;
  unsigned tex_width;
  unsigned tex_height;

  uint32_t preview : 1,
           y_invert : 1,
           swap_rgb : 1,
           has_alpha : 1,
           hovered : 1,
           clicked : 1;
};

struct wd_render_data {
  float fg_color[4];
  float bg_color[4];
  float border_color[4];
  float selection_color[4];
  unsigned int viewport_width;
  unsigned int viewport_height;
  unsigned int width;
  unsigned int height;
  int scroll_x;
  int scroll_y;
  int x_origin;
  int y_origin;
  uint64_t updated_at;
  bool external_images;

  struct wl_list heads;
};

struct wd_point {
  double x;
  double y;
};

struct wd_state {
  struct zxdg_output_manager_v1 *xdg_output_manager;
  struct zwlr_output_manager_v1 *output_manager;
  struct zwlr_export_dmabuf_manager_v1 *export_manager;
  struct zwlr_screencopy_manager_v1 *copy_manager;
  struct zwlr_layer_shell_v1 *layer_shell;
  struct wl_shm *shm;
  struct wl_list heads;
  struct wl_list outputs;
  uint32_t serial;

  bool apply_pending;
  bool autoapply;
  bool capture;
  bool show_overlay;
  double zoom;

  unsigned int apply_idle;
  unsigned int reset_idle;

  struct wd_render_head_data *clicked;
  /* top left, bottom right */
  struct wd_point click_offset;
  bool panning;
  struct wd_point pan_last;

  GtkWidget *header_stack;
  GtkWidget *stack_switcher;
  GtkWidget *stack;
  GtkWidget *scroller;
  GtkWidget *canvas;
  GtkWidget *spinner;
  GtkWidget *zoom_out;
  GtkWidget *zoom_reset;
  GtkWidget *zoom_in;
  GtkWidget *overlay;
  GtkWidget *info_bar;
  GtkWidget *info_label;
  GtkWidget *menu_button;

  GdkCursor *grab_cursor;
  GdkCursor *grabbing_cursor;
  GdkCursor *move_cursor;

  unsigned int canvas_tick;
  struct wd_gl_data *gl_data;
  struct wd_render_data render;
};


/*
 * Creates the application state structure.
 */
struct wd_state *wd_state_create(void);

/*
 * Frees the application state structure.
 */
void wd_state_destroy(struct wd_state *state);

/*
 * Displays an error message and then exits the program.
 */
void wd_fatal_error(int status, const char *message);

/*
 * Add an output to the list of screen captured outputs.
 */
void wd_add_output(struct wd_state *state, struct wl_output *wl_output, struct wl_display *display);

/*
 * Remove an output from the list of screen captured outputs.
 */
void wd_remove_output(struct wd_state *state, struct wl_output *wl_output, struct wl_display *display);

/*
 * Finds the output associated with a given head. Can return NULL if the head's
 * output is disabled.
 */
struct wd_output *wd_find_output(struct wd_state *state, struct wd_head *head);

/*
 * Finds the head associated with a given output.
 */
struct wd_head *wd_find_head(struct wd_state *state, struct wd_output *output);
/*
 * Starts listening for output management events from the compositor.
 */
void wd_add_output_management_listener(struct wd_state *state, struct wl_display *display);

/*
 * Sends updated display configuration back to the compositor.
 */
void wd_apply_state(struct wd_state *state, struct wl_list *new_outputs, struct wl_display *display);

/*
 * Queues capture of the next frame of all screens.
 */
void wd_capture_frame(struct wd_state *state);

/*
 * Blocks until all captures are finished.
 */
void wd_capture_wait(struct wd_state *state, struct wl_display *display);

/*
 * Updates the UI stack of all heads. Does not update individual head forms.
 * Useful for when a display is plugged/unplugged and we want to add/remove
 * a page, but we don't want to wipe out user's changes on the other pages.
 */
void wd_ui_reset_heads(struct wd_state *state);

/*
 * Updates the UI form for a single head. Useful for when the compositor
 * notifies us of updated configuration caused by another program.
 */
void wd_ui_reset_head(const struct wd_head *head, unsigned int fields);

/*
 * Updates the stack and all forms to the last known server state.
 */
void wd_ui_reset_all(struct wd_state *state);

/*
 * Reactivates the GUI after the display configuration updates.
 */
void wd_ui_apply_done(struct wd_state *state, struct wl_list *outputs);

/*
 * Reactivates the GUI after the display configuration updates.
 */
void wd_ui_show_error(struct wd_state *state, const char *message);

/*
 * Compiles the GL shaders.
 */
struct wd_gl_data *wd_gl_setup(struct wl_display *display);

/*
 * Creates an EGLImage from a given output frame. The frame must have a dmabuf
 * received from a wlr_export_dmabuf_frame.
 */
EGLImageKHR wd_gl_create_dmabuf_texture(struct wd_gl_data *res,
    struct wd_frame *frame);
/*
 * Renders the GL scene.
 */
void wd_gl_render(struct wd_gl_data *res, struct wd_render_data *info, uint64_t tick);

/*
 * Destroys the GL shaders.
 */
void wd_gl_cleanup(struct wd_gl_data *res);

/*
 * Create an overlay on the screen that contains a textual description of the
 * output. This is to help the user identify the outputs visually.
 */
void wd_create_overlay(struct wd_output *output);

/*
 * Forces redrawing of the screen overlay on the given output.
 */
void wd_redraw_overlay(struct wd_output *output);

/*
 * Destroys the screen overlay on the given output.
 */
void wd_destroy_overlay(struct wd_output *output);

#endif
