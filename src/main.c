#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <wayland-server.h>
#include <wlr/backend.h>
#include <wlr/render.h>
#include <wlr/types/wlr_matrix.h>
#include <wlr/types/wlr_compositor.h>
#include <wlr/types/wlr_gamma_control.h>
#include <wlr/types/wlr_idle.h>
#include <wlr/types/wlr_primary_selection.h>
#include <wlr/types/wlr_screenshooter.h>
#include <wlr/types/wlr_xdg_shell_v6.h>
#include "server.h"

int main(int argc, char **argv) {
	struct mcw_server server;
	mcw_server_init(&server);
	wl_display_run(server.wl_display);
	wl_display_destroy(server.wl_display);
	return 0;
}
