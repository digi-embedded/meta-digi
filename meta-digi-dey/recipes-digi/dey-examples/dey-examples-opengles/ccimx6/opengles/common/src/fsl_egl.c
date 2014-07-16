/****************************************************************************
* Copyright (c) 2012 Freescale Semiconductor, Inc.
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*    * Redistributions of source code must retain the above copyright notice,
*		this list of conditions and the following disclaimer.
*
*    * Redistributions in binary form must reproduce the above copyright notice,
*		this list of conditions and the following disclaimer in the documentation
*		and/or other materials provided with the distribution.
*
* 	 * Neither the name of the Freescale Semiconductor, Inc. nor the names of
*		its contributors may be used to endorse or promote products derived from
*		this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Labels parameters

*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <assert.h>
#include <string.h>
#include <EGL/egl.h>

#ifdef EGL_USE_X11
#include <X11/X.h>
#include <X11/Xlib.h>
#elif EGL_API_WL
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <assert.h>
#include <signal.h>

#include <linux/input.h>

#include <wayland-client.h>
#include <wayland-egl.h>
#include <wayland-cursor.h>

#include <GLES2/gl2.h>
#include <EGL/egl.h>

#include <wayland-client.h>
#include <wayland-egl.h>
#include <wayland-cursor.h>

struct geometry {
	int width;
	int height;
};

struct display;

struct window {
	struct display *display;
	struct wl_egl_window *native;
	struct geometry geometry, window_size;
	struct wl_surface *surface;
	struct wl_shell_surface *shell_surface;
	EGLSurface egl_surface;
	struct wl_callback *callback;
	int fullscreen, configured, opaque;
};

struct display {
	struct wl_display *display;
	struct wl_registry *registry;
	struct wl_compositor *compositor;
	struct wl_shell *shell;
	struct wl_seat *seat;
	struct wl_pointer *pointer;
	struct wl_keyboard *keyboard;
	struct wl_shm *shm;
	struct wl_cursor_theme *cursor_theme;
	struct wl_cursor *default_cursor;
	struct wl_surface *cursor_surface;
	struct window *window;
};

struct display sdisplay = { 0 };
struct window swindow = { 0 };

static void
create_wl_surface(VOID_ARGUMENT)
{
	struct display * display = &sdisplay;
	struct window * window = &swindow;

	window->surface = wl_compositor_create_surface(display->compositor);
	window->shell_surface = wl_shell_get_shell_surface(display->shell,
							   window->surface);

	window->native =
		wl_egl_window_create(window->surface,
				     window->window_size.width,
				     window->window_size.height);

	wl_shell_surface_set_title(window->shell_surface, "3DMark for GLES2.0");

	wl_shell_surface_set_fullscreen(window->shell_surface,
						WL_SHELL_SURFACE_FULLSCREEN_METHOD_DEFAULT,
						0, NULL);
}

static void
configure_callback(void *data, struct wl_callback *callback, uint32_t  time)
{
	struct window *window = (struct window *)data;

	wl_callback_destroy(callback);

	window->configured = 1;
}

static struct wl_callback_listener configure_callback_listener = {
	configure_callback,
};

static void
handle_ping(void *data, struct wl_shell_surface *shell_surface,
	    uint32_t serial)
{
	wl_shell_surface_pong(shell_surface, serial);
}

static void
handle_configure(void *data, struct wl_shell_surface *shell_surface,
		 uint32_t edges, int32_t width, int32_t height)
{
	struct window *window = (struct window *)data;

	if (window->native)
		wl_egl_window_resize(window->native, width, height, 0, 0);

	window->geometry.width = width;
	window->geometry.height = height;

	if (!window->fullscreen)
		window->window_size = window->geometry;
}

static void
handle_popup_done(void *data, struct wl_shell_surface *shell_surface)
{
}

static const struct wl_shell_surface_listener shell_surface_listener = {
	handle_ping,
	handle_configure,
	handle_popup_done
};


static void
toggle_fullscreen(struct window *window, int fullscreen)
{
	struct wl_callback *callback;

	window->fullscreen = fullscreen;
	window->configured = 0;

	if (fullscreen) {
		wl_shell_surface_set_fullscreen(window->shell_surface,
						WL_SHELL_SURFACE_FULLSCREEN_METHOD_DEFAULT,
						0, NULL);
	} else {
		wl_shell_surface_set_toplevel(window->shell_surface);
		handle_configure(window, window->shell_surface, 0,
				 window->window_size.width,
				 window->window_size.height);
	}

	callback = wl_display_sync(window->display->display);
	wl_callback_add_listener(callback, &configure_callback_listener,
				 window);
}


static void
pointer_handle_enter(void *data, struct wl_pointer *pointer,
		     uint32_t serial, struct wl_surface *surface,
		     wl_fixed_t sx, wl_fixed_t sy)
{
	struct display *display = (struct display *)data;
	struct wl_buffer *buffer;
	struct wl_cursor *cursor = display->default_cursor;
	struct wl_cursor_image *image;

	if (display->window->fullscreen)
		wl_pointer_set_cursor(pointer, serial, NULL, 0, 0);
	else if (cursor) {
		image = display->default_cursor->images[0];
		buffer = wl_cursor_image_get_buffer(image);
		wl_pointer_set_cursor(pointer, serial,
				      display->cursor_surface,
				      image->hotspot_x,
				      image->hotspot_y);
		wl_surface_attach(display->cursor_surface, buffer, 0, 0);
		wl_surface_damage(display->cursor_surface, 0, 0,
				  image->width, image->height);
		wl_surface_commit(display->cursor_surface);
	}
}

static void
pointer_handle_leave(void *data, struct wl_pointer *pointer,
		     uint32_t serial, struct wl_surface *surface)
{
}

static void
pointer_handle_motion(void *data, struct wl_pointer *pointer,
		      uint32_t time, wl_fixed_t sx, wl_fixed_t sy)
{
}

static void
pointer_handle_button(void *data, struct wl_pointer *wl_pointer,
		      uint32_t serial, uint32_t time, uint32_t button,
		      uint32_t state)
{
	struct display *display = (struct display *)data;

	if (button == BTN_LEFT && state == WL_POINTER_BUTTON_STATE_PRESSED)
		wl_shell_surface_move(display->window->shell_surface,
				      display->seat, serial);
}

static void
pointer_handle_axis(void *data, struct wl_pointer *wl_pointer,
		    uint32_t time, uint32_t axis, wl_fixed_t value)
{
}

static const struct wl_pointer_listener pointer_listener = {
	pointer_handle_enter,
	pointer_handle_leave,
	pointer_handle_motion,
	pointer_handle_button,
	pointer_handle_axis,
};

static void
keyboard_handle_keymap(void *data, struct wl_keyboard *keyboard,
		       uint32_t format, int fd, uint32_t size)
{
}

static void
keyboard_handle_enter(void *data, struct wl_keyboard *keyboard,
		      uint32_t serial, struct wl_surface *surface,
		      struct wl_array *keys)
{
}

static void
keyboard_handle_leave(void *data, struct wl_keyboard *keyboard,
		      uint32_t serial, struct wl_surface *surface)
{
}

static void
keyboard_handle_key(void *data, struct wl_keyboard *keyboard,
		    uint32_t serial, uint32_t time, uint32_t key,
		    uint32_t state)
{
	struct display *d = (struct display *)data;

	if (key == KEY_F11 && state)
		toggle_fullscreen(d->window, d->window->fullscreen ^ 1);
}

static void
keyboard_handle_modifiers(void *data, struct wl_keyboard *keyboard,
			  uint32_t serial, uint32_t mods_depressed,
			  uint32_t mods_latched, uint32_t mods_locked,
			  uint32_t group)
{
}

static const struct wl_keyboard_listener keyboard_listener = {
	keyboard_handle_keymap,
	keyboard_handle_enter,
	keyboard_handle_leave,
	keyboard_handle_key,
	keyboard_handle_modifiers,
};


static void
seat_handle_capabilities(void *data, struct wl_seat *seat,
			 uint32_t caps)
{
	struct display *d = (struct display *)data;

	if ((caps & WL_SEAT_CAPABILITY_POINTER) && !d->pointer) {
		d->pointer = wl_seat_get_pointer(seat);
		wl_pointer_add_listener(d->pointer, &pointer_listener, d);
	} else if (!(caps & WL_SEAT_CAPABILITY_POINTER) && d->pointer) {
		wl_pointer_destroy(d->pointer);
		d->pointer = NULL;
	}

	if ((caps & WL_SEAT_CAPABILITY_KEYBOARD) && !d->keyboard) {
		d->keyboard = wl_seat_get_keyboard(seat);
		wl_keyboard_add_listener(d->keyboard, &keyboard_listener, d);
	} else if (!(caps & WL_SEAT_CAPABILITY_KEYBOARD) && d->keyboard) {
		wl_keyboard_destroy(d->keyboard);
		d->keyboard = NULL;
	}
}

static const struct wl_seat_listener seat_listener = {
	seat_handle_capabilities,
};


static void
registry_handle_global(void *data, struct wl_registry *registry,
		       uint32_t name, const char *interface, uint32_t version)
{
	struct display *d = (struct display *)data;

	if (strcmp(interface, "wl_compositor") == 0) {
		d->compositor =
			(struct wl_compositor*)wl_registry_bind(registry, name,&wl_compositor_interface, 1);
	} else if (strcmp(interface, "wl_shell") == 0) {
		d->shell = (struct wl_shell *)wl_registry_bind(registry, name,
					    &wl_shell_interface, 1);
	} else if (strcmp(interface, "wl_seat") == 0) {
		d->seat = (struct wl_seat *)wl_registry_bind(registry, name,
					   &wl_seat_interface, 1);
		wl_seat_add_listener(d->seat, &seat_listener, d);
	} else if (strcmp(interface, "wl_shm") == 0) {
		d->shm = (struct wl_shm *)wl_registry_bind(registry, name,
					  &wl_shm_interface, 1);
		d->cursor_theme = wl_cursor_theme_load(NULL, 32, d->shm);
		d->default_cursor =
			wl_cursor_theme_get_cursor(d->cursor_theme, "left_ptr");
	}
}

static const struct wl_registry_listener registry_listener = {
	registry_handle_global
};
#endif

EGLNativeDisplayType fsl_getNativeDisplay()
{
	EGLNativeDisplayType eglNativeDisplayType = NULL;
#if (defined EGL_USE_X11)
	eglNativeDisplayType = XOpenDisplay(NULL);
	assert(eglNativeDisplayType != NULL);
#elif EGL_API_WL
	sdisplay.display = wl_display_connect(NULL);
	sdisplay.registry = wl_display_get_registry(sdisplay.display);
	wl_registry_add_listener(sdisplay.registry,
				 &registry_listener, &sdisplay);
	wl_display_dispatch(sdisplay.display);

	return sdisplay.display;
#elif (defined EGL_API_FB)
	eglNativeDisplayType = fbGetDisplayByIndex(0); //Pass the argument as required to show the framebuffer
#else
	display = EGL_DEFAULT_DISPLAY;
#endif
	return eglNativeDisplayType;
}

EGLNativeWindowType fsl_createwindow(EGLDisplay egldisplay, EGLNativeDisplayType eglNativeDisplayType)
{
	EGLNativeWindowType native_window = (EGLNativeWindowType)0;

#if (defined EGL_USE_X11)
	Window window, rootwindow;
	int screen = DefaultScreen(eglNativeDisplayType);
	rootwindow = RootWindow(eglNativeDisplayType,screen);
	window = XCreateSimpleWindow(eglNativeDisplayType, rootwindow, 0, 0, 400, 533, 0, 0, WhitePixel (eglNativeDisplayType, screen));
	XMapWindow(eglNativeDisplayType, window);
	native_window = window;
#elif EGL_API_WL
	swindow.window_size.width  = 800;
	swindow.window_size.height = 480;

    swindow.display = &sdisplay;
	sdisplay.window = &swindow;

	sdisplay.registry = wl_display_get_registry(sdisplay.display);
	wl_registry_add_listener(sdisplay.registry,
				 &registry_listener, &sdisplay);
	wl_display_dispatch(sdisplay.display);
	create_wl_surface();

	native_window = swindow.native;

	sdisplay.cursor_surface =
		wl_compositor_create_surface(sdisplay.compositor);

#else
	const char *vendor = eglQueryString(egldisplay, EGL_VENDOR);
	if (strstr(vendor, "Imagination Technologies"))
		native_window = (EGLNativeWindowType)0;
	else if (strstr(vendor, "AMD"))
		native_window = (EGLNativeWindowType)  open("/dev/fb0", O_RDWR);
	else if (strstr(vendor, "Vivante")) //NEEDS FIX - functs don't exist on other platforms
	{
#if (defined EGL_API_FB)
		native_window = fbCreateWindow(eglNativeDisplayType, 0, 0, 0, 0);
#endif
	}
	else
	{
		printf("Unknown vendor [%s]\n", vendor);
		return 0;
	}
#endif
	return native_window;

}


void fsl_destroywindow(EGLNativeWindowType eglNativeWindowType, EGLNativeDisplayType eglNativeDisplayType)
{
	(void) eglNativeWindowType;
#if (defined EGL_USE_X11)
	//close x display
	XCloseDisplay(eglNativeDisplayType);
#elif EGL_API_WL
	struct display * display = &sdisplay;
	struct window * window = &swindow;

	if(eglNativeWindowType)
	{
		wl_egl_window_destroy(window->native);
		wl_shell_surface_destroy(window->shell_surface);
		wl_surface_destroy(window->surface);
		if (window->callback)
			wl_callback_destroy(window->callback);
	}

	wl_surface_destroy(display->cursor_surface);

	if (display->cursor_theme)
		wl_cursor_theme_destroy(display->cursor_theme);

	if (display->shell)
		wl_shell_destroy(display->shell);

	if (display->compositor)
		wl_compositor_destroy(display->compositor);

	wl_display_flush(display->display);
	wl_display_disconnect(display->display);

#endif
}
