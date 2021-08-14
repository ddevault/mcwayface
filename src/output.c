#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <wayland-server.h>
#include <wlr/render.h>
#include <wlr/types/wlr_matrix.h>
#include <wlr/types/wlr_surface.h>
#include "server.h"

static void output_frame_notify(struct wl_listener *listener, void *data) {
	struct mcw_output *output = wl_container_of(listener, output, frame);
	struct mcw_server *server = output->server;
	struct wlr_output *wlr_output = data;
	struct wlr_renderer *renderer = wlr_backend_get_renderer(
			wlr_output->backend);

	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);

	wlr_output_make_current(wlr_output, NULL);
	wlr_renderer_begin(renderer, wlr_output);

	float color[4] = { 0.4f, 0.4f, 0.4f, 1.0f };
	wlr_renderer_clear(renderer, color);

	struct wl_resource *_surface;
	wl_resource_for_each(_surface, &server->compositor->surfaces) {
		struct wlr_surface *surface = wlr_surface_from_resource(_surface);
		if (!wlr_surface_has_buffer(surface)) {
			continue;
		}
		struct wlr_box render_box = {
			.x = 20, .y = 20,
			.width = surface->current->width, .height = surface->current->height
		};
		float matrix[16];
		wlr_matrix_project_box(matrix, &render_box,
				surface->current->transform, 0, wlr_output->transform_matrix);
		wlr_render_texture_with_matrix(renderer, surface->texture, matrix, 1.0f);
		wlr_surface_send_frame_done(surface, &now);
	}

	wlr_output_swap_buffers(wlr_output, NULL, NULL);
	wlr_renderer_end(renderer);

	output->last_frame = now;
}

static void output_destroy_notify(struct wl_listener *listener, void *data) {
	struct mcw_output *output = wl_container_of(listener, output, destroy);
	wl_list_remove(&output->link);
	wl_list_remove(&output->destroy.link);
	wl_list_remove(&output->frame.link);
	free(output);
}

void new_output_notify(struct wl_listener *listener, void *data) {
	struct mcw_server *server = wl_container_of(
			listener, server, new_output);
	struct wlr_output *wlr_output = data;

	if (wl_list_length(&wlr_output->modes) > 0) {
		struct wlr_output_mode *mode =
			wl_container_of((&wlr_output->modes)->prev, mode, link);
		wlr_output_set_mode(wlr_output, mode);
	}

	struct mcw_output *output = calloc(1, sizeof(struct mcw_output));
	clock_gettime(CLOCK_MONOTONIC, &output->last_frame);
	output->server = server;
	output->wlr_output = wlr_output;
	wl_list_insert(&server->outputs, &output->link);

	output->destroy.notify = output_destroy_notify;
	wl_signal_add(&wlr_output->events.destroy, &output->destroy);
	output->frame.notify = output_frame_notify;
	wl_signal_add(&wlr_output->events.frame, &output->frame);

	wlr_output_create_global(wlr_output);
}
