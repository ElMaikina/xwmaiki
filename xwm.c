/* See LICENSE file for license details. */
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <xcb/xcb.h>
#include <xcb/xcb_keysyms.h>
#include "xwm.h"
#include "config.h"

#define UNUSED(x) (void)(x)

/* Current display */
static xcb_connection_t * dpy;

/* Current focused screen */
static xcb_screen_t * scre;

/* Current focused window */
static xcb_drawable_t win;
static uint32_t values[3];

/* Kills a window */
static void killclient(char **com) {
	UNUSED(com);
	xcb_kill_client(dpy, win);
}

/* Kills the entire desktop */
static void closewm(char **com) {
	UNUSED(com);
	if (dpy != NULL) {		
		xcb_disconnect(dpy);
	}
}

/* Spawns a window by recieving a command */
static void spawn(char **com) {
	if (fork() == 0) {
		setsid();
		if (fork() != 0) {
			_exit(0);	
		}
		/* child thread executes the command */
		execvp((char*)com[0], (char**)com);
		_exit(0);
	}
	wait(NULL);
}

/* Makes a window fullscreen */
static void moveandresizeclient(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
	uint32_t vals[4];

	/* x, y, width and height of new window */
	vals[0] = x;
	vals[1] = y;
	vals[2] = width;
	vals[3] = height;

	/* applies changes to the desired window */
	xcb_configure_window(
			dpy, 				/* display */
			win, 				/* window  */

			/* arguments to modify the window */
			XCB_CONFIG_WINDOW_X	|	
			XCB_CONFIG_WINDOW_Y |	
			XCB_CONFIG_WINDOW_WIDTH |	
			XCB_CONFIG_WINDOW_HEIGHT,

			/* the values corresponding to the arguments */
			vals
	);

	/* writes to buffer */
	xcb_flush(dpy);
}

/* Makes a window fullscreen */
static void fullclient(char **com) {
	UNUSED(com);
	uint32_t x		= 0 - BORDER_WIDTH;
	uint32_t y		= 0 - BORDER_WIDTH;
	uint32_t width	= scre->width_in_pixels;
	uint32_t height = scre->height_in_pixels;
	moveandresizeclient(x, y, width, height);
}

/* Maximizes a window */
static void maximizeclient(char **com) {
	UNUSED(com);
	uint32_t x		= 0;
	uint32_t y		= 0;
	uint32_t width	= scre->width_in_pixels - BORDER_WIDTH*2;
	uint32_t height = scre->height_in_pixels - BORDER_WIDTH*2 - PANEL_HEIGHT;
	moveandresizeclient(x, y, width, height);
}

/* Tiles a window to the left half of the screen */
static void tileclientleft(char **com) {
	UNUSED(com);
	if (win != 0) {
  		xcb_get_geometry_reply_t *geom;
		geom = xcb_get_geometry_reply(dpy, xcb_get_geometry (dpy, win), NULL);

		uint32_t x		= 0;
		uint32_t y		= 0;
		uint32_t width	= scre->width_in_pixels/2 - BORDER_WIDTH*2;
		uint32_t height = scre->height_in_pixels - BORDER_WIDTH*2 - PANEL_HEIGHT;
		
		if (width == geom->width) {
			width = scre->width_in_pixels - BORDER_WIDTH*2;
		}
		if (scre->width_in_pixels/2 == geom->x) { 
			x = 0;
			width =	scre->width_in_pixels/2 - BORDER_WIDTH*2;
		}
		moveandresizeclient(x, y, width, height);
		free(geom); 
	}
}

/* Tiles a window to the right half of the screen */
static void tileclientright(char **com) {
	UNUSED(com);
	if (win != 0) {
  		xcb_get_geometry_reply_t *geom;
		geom = xcb_get_geometry_reply(dpy, xcb_get_geometry (dpy, win), NULL);

		uint32_t x		= scre->width_in_pixels/2;
		uint32_t y		= 0;
		uint32_t width	= scre->width_in_pixels/2 - BORDER_WIDTH*2;
		uint32_t height = scre->height_in_pixels - BORDER_WIDTH*2 - PANEL_HEIGHT;

		if (width == geom->width) {
			x = 0;
			width = scre->width_in_pixels - BORDER_WIDTH*2;
		}
		if (0 == geom->x) { 
			x = scre->width_in_pixels/2;
			width =	scre->width_in_pixels/2 - BORDER_WIDTH*2;
		}
		moveandresizeclient(x, y, width, height);
		free(geom); 
	}
}

/* Tiles a window to the top half of the screen */
static void tileclienttop(char **com) {
	UNUSED(com);
	if (win != 0) {
  		xcb_get_geometry_reply_t *geom;
		geom = xcb_get_geometry_reply(dpy, xcb_get_geometry (dpy, win), NULL);

		uint32_t x		= 0;
		uint32_t y		= 0;
		uint32_t width	= scre->width_in_pixels - BORDER_WIDTH*2;
		uint32_t height = (scre->height_in_pixels - PANEL_HEIGHT)/2 - BORDER_WIDTH*2;
		
		if (height == geom->height) {
			height = scre->height_in_pixels - PANEL_HEIGHT - BORDER_WIDTH*2;
		}
		if ((scre->height_in_pixels - PANEL_HEIGHT)/2 == geom->y) { 
			y = 0;
			height = (scre->height_in_pixels - PANEL_HEIGHT)/2 - BORDER_WIDTH*2;
		}
		if (scre->width_in_pixels/2 - BORDER_WIDTH*2 == geom->width) {
			x = geom->x;
			width = geom->width;
		}
		moveandresizeclient(x, y, width, height);
		free(geom); 
	}
}


/* Tiles a window to the bottom half of the screen */
static void tileclientbottom(char **com) {
	UNUSED(com);
	if (win != 0) {
  		xcb_get_geometry_reply_t *geom;
		geom = xcb_get_geometry_reply(dpy, xcb_get_geometry (dpy, win), NULL);

		uint32_t x		= 0;
		uint32_t y		= (scre->height_in_pixels - PANEL_HEIGHT)/2;
		uint32_t width	= scre->width_in_pixels - BORDER_WIDTH*2;
		uint32_t height = (scre->height_in_pixels - PANEL_HEIGHT)/2 - BORDER_WIDTH*2;
		
		if (height == geom->height) {
			y = 0;
			height = scre->height_in_pixels - PANEL_HEIGHT - BORDER_WIDTH*2;
		}
		if (0 == geom->y) { 
			y = (scre->height_in_pixels - PANEL_HEIGHT)/2;
			height = (scre->height_in_pixels - PANEL_HEIGHT)/2 - BORDER_WIDTH*2;
		}
		if (scre->width_in_pixels/2 - BORDER_WIDTH*2 == geom->width) {
			x = geom->x;
			width = geom->width;
		}
		moveandresizeclient(x, y, width, height);
		free(geom); 
	}
}

/* Handles button presses */
static void handleButtonPress(xcb_generic_event_t *ev) {
	xcb_button_press_event_t  *e = (xcb_button_press_event_t *) ev;
	win = e->child;
	values[0] = XCB_STACK_MODE_ABOVE;
	xcb_configure_window(dpy, win, XCB_CONFIG_WINDOW_STACK_MODE, values);

	values[2] = ((1 == e->detail) ? 1 : ((win != 0) ? 3 : 0 ));

	xcb_grab_pointer(
			dpy, 
			0, 
			scre->root, 
			XCB_EVENT_MASK_BUTTON_RELEASE | 
			XCB_EVENT_MASK_BUTTON_MOTION | 
			XCB_EVENT_MASK_POINTER_MOTION_HINT,
			XCB_GRAB_MODE_ASYNC, 
			XCB_GRAB_MODE_ASYNC,
			scre->root, 
			XCB_NONE, 
			XCB_CURRENT_TIME
	);
}

/* Manages window moving and resizing with the mouse */
static void handleMotionNotify(xcb_generic_event_t *ev) {
	UNUSED(ev);
	uint32_t vals[4];
	xcb_query_pointer_cookie_t coord = xcb_query_pointer(dpy, scre->root);
	xcb_query_pointer_reply_t *poin = xcb_query_pointer_reply(dpy, coord, 0);

	/* Moving window with the mouse */
	if ((values[2] == (uint32_t)(1)) && (win != 0)) {
		xcb_get_geometry_cookie_t geom_now = xcb_get_geometry(dpy, win);
		xcb_get_geometry_reply_t *geom = xcb_get_geometry_reply(dpy, geom_now, NULL);

		uint16_t width = geom->width/2 + (2 * BORDER_WIDTH);
		uint16_t height = geom->height/2 + (2 * BORDER_WIDTH);

		vals[0] = poin->root_x - width;
		vals[1] = poin->root_y - height;
		vals[2] = WINDOW_WIDTH;
		vals[3] = WINDOW_HEIGHT;

		/* manages mouse tiling */
		if (poin->root_y < TILE_BORDER) {
			vals[0] = 0;
			vals[1] = 0;
			vals[2] = scre->width_in_pixels - BORDER_WIDTH*2;
			vals[3] = (scre->height_in_pixels - PANEL_HEIGHT)/2 - BORDER_WIDTH*2;
		}
		if (poin->root_y > scre->height_in_pixels - TILE_BORDER) {
			vals[0] = 0;
			vals[1] = (scre->height_in_pixels - PANEL_HEIGHT)/2;
			vals[2] = scre->width_in_pixels - BORDER_WIDTH*2;
			vals[3] = (scre->height_in_pixels - PANEL_HEIGHT)/2 - BORDER_WIDTH*2;
		}
		if (poin->root_x < TILE_BORDER) {
			vals[0] = 0;
			vals[1] = 0;
			vals[2] = scre->width_in_pixels/2 - BORDER_WIDTH*2;
			vals[3] = scre->height_in_pixels - BORDER_WIDTH*2 - PANEL_HEIGHT;
		}
		if (poin->root_x > scre->width_in_pixels - TILE_BORDER) {
			vals[0] = scre->width_in_pixels/2;
			vals[1] = 0;
			vals[2] = scre->width_in_pixels/2 - BORDER_WIDTH*2;
			vals[3] = scre->height_in_pixels - BORDER_WIDTH*2 - PANEL_HEIGHT;
		}

		/* applies changes to the desired window */
		xcb_configure_window(
				dpy, 				/* display */
				win, 				/* window  */
	
				/* arguments to modify the window */
				XCB_CONFIG_WINDOW_X	|	
				XCB_CONFIG_WINDOW_Y |	
				XCB_CONFIG_WINDOW_WIDTH |	
				XCB_CONFIG_WINDOW_HEIGHT,
	
				/* the values corresponding to the arguments */
				vals
		);
	}

	/* Resizing window with the mouse */
	else if ((values[2] == (uint32_t)(3)) && (win != 0)) {
		xcb_get_geometry_cookie_t geom_now = xcb_get_geometry(dpy, win);
		xcb_get_geometry_reply_t* geom = xcb_get_geometry_reply(dpy, geom_now, NULL);
		if (!((poin->root_x <= geom->x) || (poin->root_y <= geom->y))) 
		{
			values[0] = poin->root_x - geom->x - BORDER_WIDTH;
			values[1] = poin->root_y - geom->y - BORDER_WIDTH;
			if ((values[0] >= (uint32_t)(WINDOW_MIN_WIDTH)) &&
				(values[1] >= (uint32_t)(WINDOW_MIN_HEIGHT))) {
				xcb_configure_window(
						dpy, 
						win, 
						XCB_CONFIG_WINDOW_WIDTH | 
						XCB_CONFIG_WINDOW_HEIGHT, 
						values
				);
			}
		}
	} else {}
}

static xcb_keycode_t *xcb_get_keycodes(xcb_keysym_t keysym) {
	xcb_key_symbols_t *keysyms = xcb_key_symbols_alloc(dpy);
	xcb_keycode_t *keycode;
	keycode = (!(keysyms) ? NULL : xcb_key_symbols_get_keycode(keysyms, keysym));
	xcb_key_symbols_free(keysyms);
	return keycode;
}

static xcb_keysym_t xcb_get_keysym(xcb_keycode_t keycode) {
	xcb_key_symbols_t *keysyms = xcb_key_symbols_alloc(dpy);
	xcb_keysym_t keysym;
	keysym = (!(keysyms) ? 0 : xcb_key_symbols_get_keysym(keysyms, keycode, 0));
	xcb_key_symbols_free(keysyms);
	return keysym;
}

/* Set focus to a Window */
static void setFocus(xcb_drawable_t window) {
	if ((window != 0) && (window != scre->root)) {
		xcb_set_input_focus(dpy, XCB_INPUT_FOCUS_POINTER_ROOT, window,
			XCB_CURRENT_TIME);
	}
}

/* Applies focus color to a Window */
static void setFocusColor(xcb_window_t window, int focus) {
	if ((BORDER_WIDTH > 0) && (scre->root != window) && (0 != window)) {
		uint32_t vals[1];
		vals[0] = focus ? BORDER_COLOR_FOCUSED : BORDER_COLOR_UNFOCUSED;
		xcb_change_window_attributes(dpy, window, XCB_CW_BORDER_PIXEL, vals);
		xcb_flush(dpy);
	}
}

/* Handle all the events that can be executed via Keyboard commands */
static void handleKeyPress(xcb_generic_event_t *ev) {
	xcb_key_press_event_t *e = ( xcb_key_press_event_t *) ev;
	xcb_keysym_t keysym = xcb_get_keysym(e->detail);
	win = e->child;

	// Kays obtained from the configuration file
	int key_table_size = sizeof(keys) / sizeof(*keys);
	for (int i = 0; i < key_table_size; ++i) {
		if ((keys[i].keysym == keysym) && (keys[i].mod == e->state)) {
			keys[i].func(keys[i].com);
		}
	}
}

static void handleEnterNotify(xcb_generic_event_t *ev) {
	xcb_enter_notify_event_t *e = ( xcb_enter_notify_event_t *) ev;
	setFocus(e->event);
}

static void handleButtonRelease(xcb_generic_event_t *ev) {
	UNUSED(ev);
	xcb_ungrab_pointer(dpy, XCB_CURRENT_TIME);
}

static void handleDestroyNotify(xcb_generic_event_t *ev) {
	xcb_destroy_notify_event_t *e = (xcb_destroy_notify_event_t *) ev;
	xcb_kill_client(dpy, e->window);
}

static void handleFocusIn(xcb_generic_event_t *ev) {
	xcb_focus_in_event_t *e = (xcb_focus_in_event_t *) ev;
	setFocusColor(e->event, 1);
}

static void handleFocusOut(xcb_generic_event_t *ev) {
	xcb_focus_out_event_t *e = (xcb_focus_out_event_t *) ev;
	setFocusColor(e->event, 0);
}

static void handleMapRequest(xcb_generic_event_t *ev) {
	xcb_map_request_event_t *e = (xcb_map_request_event_t *) ev;
	xcb_map_window(dpy, e->window);
	uint32_t vals[5];
	vals[0] = (scre->width_in_pixels / 2) - (WINDOW_WIDTH / 2);
	vals[1] = (scre->height_in_pixels / 2) - (WINDOW_HEIGHT / 2);
	vals[2] = WINDOW_WIDTH;
	vals[3] = WINDOW_HEIGHT;
	vals[4] = BORDER_WIDTH;
	xcb_configure_window(dpy, e->window, XCB_CONFIG_WINDOW_X |
		XCB_CONFIG_WINDOW_Y | XCB_CONFIG_WINDOW_WIDTH |
		XCB_CONFIG_WINDOW_HEIGHT | XCB_CONFIG_WINDOW_BORDER_WIDTH, vals);
	xcb_flush(dpy);
	values[0] = XCB_EVENT_MASK_ENTER_WINDOW | XCB_EVENT_MASK_FOCUS_CHANGE;
	xcb_change_window_attributes_checked(dpy, e->window,
		XCB_CW_EVENT_MASK, values);
	setFocus(e->window);
}

static int eventHandler(void) {
	int ret = xcb_connection_has_error(dpy);
	if (ret == 0) {
		xcb_generic_event_t *ev = xcb_wait_for_event(dpy);
		if (ev != NULL) {
			handler_func_t *handler;
			for (handler = handler_funs; handler->func != NULL; handler++) {
				if ((ev->response_type & ~0x80) == handler->request) {
					handler->func(ev);
				}
			}
			free(ev);
		}
	}
	xcb_flush(dpy);
	return ret;
}

static void setup(void) {
	values[0] = XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT
		| XCB_EVENT_MASK_STRUCTURE_NOTIFY
		| XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY
		| XCB_EVENT_MASK_PROPERTY_CHANGE;
	xcb_change_window_attributes_checked(dpy, scre->root,
		XCB_CW_EVENT_MASK, values);
	xcb_ungrab_key(dpy, XCB_GRAB_ANY, scre->root, XCB_MOD_MASK_ANY);
	int key_table_size = sizeof(keys) / sizeof(*keys);
	for (int i = 0; i < key_table_size; ++i) {
		xcb_keycode_t *keycode = xcb_get_keycodes(keys[i].keysym);
		if (keycode != NULL) {
			xcb_grab_key(dpy, 1, scre->root, keys[i].mod, *keycode,
				XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC );
		}
	}
	xcb_flush(dpy);
	xcb_grab_button(dpy, 0, scre->root, XCB_EVENT_MASK_BUTTON_PRESS |
		XCB_EVENT_MASK_BUTTON_RELEASE, XCB_GRAB_MODE_ASYNC,
		XCB_GRAB_MODE_ASYNC, scre->root, XCB_NONE, 1, MOD1);
	xcb_grab_button(dpy, 0, scre->root, XCB_EVENT_MASK_BUTTON_PRESS |
		XCB_EVENT_MASK_BUTTON_RELEASE, XCB_GRAB_MODE_ASYNC,
		XCB_GRAB_MODE_ASYNC, scre->root, XCB_NONE, 3, MOD1);
	xcb_flush(dpy);
}

static int die(char *errstr) {
	size_t n = 0;
	char *p = errstr;
	while ((* (p++)) != 0) {
		++n;
	}
	ssize_t o = write(STDERR_FILENO, errstr, n);
	int ret = 1;
	if (o < 0) {
		ret = -1;
	}
	return ret;
}

static int strcmp_c(char *str1, char *str2) {
	char *c1 = str1;
	char *c2 = str2;
	while ((* c1) && ((* c1) == (* c2))) {
		++c1;
		++c2;
	}
	int n = (* c1) - (* c2);
	return n;
}

int main(int argc, char *argv[]) {
	int ret = 0;
	if ((argc == 2) && (strcmp_c("-v", argv[1]) == 0)) {
		ret = die("xwm-0.1.9, Copyright © 2021-2022 Michael Czigler, MIT License\n");
	}
	if ((ret == 0) && (argc != 1)) {
		ret = die("usage: xwm [-v]\n");
	}
	if (ret == 0) {
		dpy = xcb_connect(NULL, NULL);
		ret = xcb_connection_has_error(dpy);
		if (ret > 0) {
			ret = die("xcb_connection_has_error\n");
		}
	}
	if (ret == 0) {
		scre = xcb_setup_roots_iterator(xcb_get_setup(dpy)).data;
		setup();
	}
	while (ret == 0) {
		ret = eventHandler();
	}
	return ret;
}
