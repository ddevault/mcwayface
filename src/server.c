#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <wayland-server.h>
#include <wlr/backend.h>
#include <wlr/types/wlr_compositor.h>
#include <wlr/types/wlr_gamma_control.h>
#include <wlr/types/wlr_idle.h>
#include <wlr/types/wlr_primary_selection.h>
#include <wlr/types/wlr_screenshooter.h>
#include <wlr/types/wlr_xdg_shell_v6.h>
#include "server.h"

void new_output_notify(struct wl_listener *listener, void *data);

void mcw_server_init(struct mcw_server *server) {
	server->wl_display = wl_display_create();
	assert(server->wl_display);
	server->wl_event_loop = wl_display_get_event_loop(server->wl_display);
	assert(server->wl_event_loop);

	server->backend = wlr_backend_autocreate(server->wl_display);
	assert(server->backend);

	wl_list_init(&server->outputs);

	server->new_output.notify = new_output_notify;
	wl_signal_add(&server->backend->events.new_output, &server->new_output);

	const char *socket = wl_display_add_socket_auto(server->wl_display);
	assert(socket);

	if (!wlr_backend_start(server->backend)) {
		fprintf(stderr, "Failed to start backend\n");
		wl_display_destroy(server->wl_display);
		exit(1);
	}

	printf("Running compositor on wayland display '%s'\n", socket);
	setenv("WAYLAND_DISPLAY", socket, true);

	wl_display_init_shm(server->wl_display);
	wlr_gamma_control_manager_create(server->wl_display);
	wlr_screenshooter_create(server->wl_display);
	wlr_primary_selection_device_manager_create(server->wl_display);
	wlr_idle_create(server->wl_display);

	server->compositor = wlr_compositor_create(server->wl_display,
			wlr_backend_get_renderer(server->backend));

	wlr_xdg_shell_v6_create(server->wl_display);
}
