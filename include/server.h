#ifndef _MCW_SERVER_H
#define _MCW_SERVER_H
#include <wayland-server.h>
#include <wlr/backend.h>
#include <wlr/types/wlr_compositor.h>

struct mcw_server {
	struct wl_display *wl_display;
	struct wl_event_loop *wl_event_loop;

	struct wlr_backend *backend;
	struct wlr_compositor *compositor;

	struct wl_listener new_output;

	struct wl_list outputs; // mcw_output::link
};

struct mcw_output {
	struct wlr_output *wlr_output;
	struct mcw_server *server;
	struct timespec last_frame;

	struct wl_listener destroy;
	struct wl_listener frame;

	struct wl_list link;
};

void mcw_server_init(struct mcw_server *server);

#endif
