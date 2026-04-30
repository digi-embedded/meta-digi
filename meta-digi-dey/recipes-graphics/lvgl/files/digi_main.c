#include "lvgl/lvgl.h"
#include "lvgl/demos/lv_demos.h"
#include "lvgl/src/core/lv_global.h"
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

static const char *getenv_default(const char *name, const char *dflt)
{
    return getenv(name) ? : dflt;
}

#if LV_USE_LINUX_FBDEV || LV_USE_LINUX_DRM
static void indev_deleted_cb(lv_event_t * e)
{
    if(LV_GLOBAL_DEFAULT()->deinit_in_progress)
        return;
    lv_obj_t * cursor_obj = lv_event_get_user_data(e);
    lv_obj_delete(cursor_obj);
}

static void discovery_cb(lv_indev_t * indev, lv_evdev_type_t type, void * user_data)
{
    LV_LOG_USER("new '%s' device discovered", type == LV_EVDEV_TYPE_REL ? "REL" :
                                              type == LV_EVDEV_TYPE_ABS ? "ABS" :
                                              type == LV_EVDEV_TYPE_KEY ? "KEY" :
                                              "unknown");
    /* if input device is mouse add cursor */
    if(type == LV_EVDEV_TYPE_REL) {
        LV_IMAGE_DECLARE(mouse_cursor_icon);
        lv_obj_t * cursor_obj = lv_image_create(lv_screen_active());
        lv_image_set_src(cursor_obj, &mouse_cursor_icon);
        lv_indev_set_cursor(indev, cursor_obj);
        lv_indev_add_event_cb(indev, indev_deleted_cb, LV_EVENT_DELETE, cursor_obj);
    }
}
#endif

#if LV_USE_LINUX_FBDEV
static void lv_linux_disp_init(void)
{
    const char *device = getenv_default("LV_LINUX_FBDEV_DEVICE", "/dev/fb0");
    lv_display_t * disp = lv_linux_fbdev_create();

    lv_linux_fbdev_set_file(disp, device);
    /* search for input devices */
    lv_evdev_discovery_start(discovery_cb, NULL);
}
#elif LV_USE_LINUX_DRM
static void lv_linux_disp_init(void)
{
    const char *device = getenv_default("LV_LINUX_DRM_CARD", "/dev/dri/card0");
    lv_display_t * disp = lv_linux_drm_create();

    lv_linux_drm_set_file(disp, device, -1);
    /* search for input devices */
    lv_evdev_discovery_start(discovery_cb, NULL);
}
#elif LV_USE_WAYLAND
static void lv_linux_disp_init(void)
{
    /*
     * Initial resolution doesn't really matter if we're changing to fullscreen
     * immediately after. It should be fine as long as it's big enough to
     * display all widgets correctly.
     */
    lv_display_t * disp = lv_wayland_window_create(1280, 800, "LVGL Demo", NULL);
    lv_wayland_window_set_fullscreen(disp, true);
    if(LV_COLOR_DEPTH == 32) {
        lv_display_set_color_format(disp, LV_COLOR_FORMAT_ARGB8888);
    }
}
#else
#error Unsupported configuration
#endif

int main(void)
{
    lv_init();

    /*Linux display device init*/
    lv_linux_disp_init();

    /*Create a Demo*/
    lv_demo_widgets();

#if LV_USE_WAYLAND
    while (1) {
        lv_wayland_timer_handler();

        usleep(5000);

        /* Run until the last window closes */
        if (!lv_wayland_window_is_open(NULL)) {
            break;
        }
    }
#else
    /*Handle LVGL tasks*/
    while(1) {
        lv_timer_handler();
        usleep(5000);
    }
#endif

    return 0;
}
