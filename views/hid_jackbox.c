#include "hid_jackbox.h"
#include <gui/elements.h>
#include "../hid.h"

#include "hid_icons.h"

#define TAG "HidJackbox"

struct HidJackbox {
    View* view;
    Hid* hid;
};

typedef struct {
    bool left_pressed;
    bool up_pressed;
    bool right_pressed;
    bool down_pressed;
    bool ok_pressed;
    bool back_pressed;
    bool connected;
} HidJackboxModel;

static void hid_jackbox_draw_arrow(Canvas* canvas, uint8_t x, uint8_t y, CanvasDirection dir) {
    canvas_draw_triangle(canvas, x, y, 5, 3, dir);
    if(dir == CanvasDirectionBottomToTop) {
        canvas_draw_dot(canvas, x, y - 1);
    } else if(dir == CanvasDirectionTopToBottom) {
        canvas_draw_dot(canvas, x, y + 1);
    } else if(dir == CanvasDirectionRightToLeft) {
        canvas_draw_dot(canvas, x - 1, y);
    } else if(dir == CanvasDirectionLeftToRight) {
        canvas_draw_dot(canvas, x + 1, y);
    }
}

static void hid_jackbox_draw_callback(Canvas* canvas, void* context) {
    furi_assert(context);
    HidJackboxModel* model = context;

    // Header
    if(model->connected) {
        canvas_draw_icon(canvas, 0, 0, &I_Ble_connected_15x15);
    } else {
        canvas_draw_icon(canvas, 0, 0, &I_Ble_disconnected_15x15);
    }

    canvas_set_font(canvas, FontPrimary);
    elements_multiline_text_aligned(canvas, 17, 3, AlignLeft, AlignTop, "Jackbox");

    // Keypad circles
    canvas_draw_icon(canvas, 58, 8, &I_Circles_47x47);
    canvas_draw_icon(canvas, 105, 42, &I_Circle_17x17);

    // Up
    if(model->up_pressed) {
        canvas_set_bitmap_mode(canvas, 1);
        canvas_draw_icon(canvas, 75, 9, &I_Pressed_Button_13x13);
        canvas_set_bitmap_mode(canvas, 0);
        canvas_set_color(canvas, ColorWhite);
    }
    hid_jackbox_draw_arrow(canvas, 81, 14, CanvasDirectionBottomToTop);
    canvas_set_color(canvas, ColorBlack);

    // Down
    if(model->down_pressed) {
        canvas_set_bitmap_mode(canvas, 1);
        canvas_draw_icon(canvas, 75, 41, &I_Pressed_Button_13x13);
        canvas_set_bitmap_mode(canvas, 0);
        canvas_set_color(canvas, ColorWhite);
    }
    hid_jackbox_draw_arrow(canvas, 81, 48, CanvasDirectionTopToBottom);
    canvas_set_color(canvas, ColorBlack);

    // Left
    if(model->left_pressed) {
        canvas_set_bitmap_mode(canvas, 1);
        canvas_draw_icon(canvas, 59, 25, &I_Pressed_Button_13x13);
        canvas_set_bitmap_mode(canvas, 0);
        canvas_set_color(canvas, ColorWhite);
    }
    hid_jackbox_draw_arrow(canvas, 64, 31, CanvasDirectionRightToLeft);
    canvas_set_color(canvas, ColorBlack);

    // Right
    if(model->right_pressed) {
        canvas_set_bitmap_mode(canvas, 1);
        canvas_draw_icon(canvas, 91, 25, &I_Pressed_Button_13x13);
        canvas_set_bitmap_mode(canvas, 0);
        canvas_set_color(canvas, ColorWhite);
    }
    hid_jackbox_draw_arrow(canvas, 98, 31, CanvasDirectionLeftToRight);
    canvas_set_color(canvas, ColorBlack);

    // Enter
    if(model->ok_pressed) {
        canvas_draw_icon(canvas, 75, 25, &I_Pressed_Button_13x13);
        canvas_set_color(canvas, ColorWhite);
    }
    hid_jackbox_draw_arrow(canvas, 80, 32, CanvasDirectionRightToLeft);
    canvas_draw_line(canvas, 81, 32, 83, 32);
    canvas_draw_line(canvas, 83, 31, 83, 28);
    canvas_set_color(canvas, ColorBlack);

    // Esc
    if(model->back_pressed) {
        canvas_draw_icon(canvas, 108, 45, &I_Pressed_Button_13x13);
        canvas_set_color(canvas, ColorWhite);
    }
    canvas_set_font(canvas, FontSecondary);
    elements_multiline_text_aligned(canvas, 108, 51, AlignLeft, AlignCenter, "esc");
    canvas_set_color(canvas, ColorBlack);

    // Exit
    canvas_draw_icon(canvas, 0, 54, &I_Pin_back_arrow_10x8);
    elements_multiline_text_aligned(canvas, 13, 62, AlignLeft, AlignBottom, "Hold to exit");
}

static void hid_jackbox_process(HidJackbox* hid_jackbox, InputEvent* event) {
    with_view_model(
        hid_jackbox->view,
        HidJackboxModel * model,
        {
            if(event->type == InputTypePress) {
                if(event->key == InputKeyUp) {
                    model->up_pressed = true;
                    hid_hal_keyboard_press(hid_jackbox->hid, HID_KEYBOARD_UP_ARROW);
                } else if(event->key == InputKeyDown) {
                    model->down_pressed = true;
                    hid_hal_keyboard_press(hid_jackbox->hid, HID_KEYBOARD_DOWN_ARROW);
                } else if(event->key == InputKeyLeft) {
                    model->left_pressed = true;
                    hid_hal_keyboard_press(hid_jackbox->hid, HID_KEYBOARD_LEFT_ARROW);
                } else if(event->key == InputKeyRight) {
                    model->right_pressed = true;
                    hid_hal_keyboard_press(hid_jackbox->hid, HID_KEYBOARD_RIGHT_ARROW);
                } else if(event->key == InputKeyOk) {
                    model->ok_pressed = true;
                    hid_hal_keyboard_press(hid_jackbox->hid, HID_KEYBOARD_RETURN);
                } else if(event->key == InputKeyBack) {
                    model->back_pressed = true;
                }
            } else if(event->type == InputTypeRelease) {
                if(event->key == InputKeyUp) {
                    model->up_pressed = false;
                    hid_hal_keyboard_release(hid_jackbox->hid, HID_KEYBOARD_UP_ARROW);
                } else if(event->key == InputKeyDown) {
                    model->down_pressed = false;
                    hid_hal_keyboard_release(hid_jackbox->hid, HID_KEYBOARD_DOWN_ARROW);
                } else if(event->key == InputKeyLeft) {
                    model->left_pressed = false;
                    hid_hal_keyboard_release(hid_jackbox->hid, HID_KEYBOARD_LEFT_ARROW);
                } else if(event->key == InputKeyRight) {
                    model->right_pressed = false;
                    hid_hal_keyboard_release(hid_jackbox->hid, HID_KEYBOARD_RIGHT_ARROW);
                } else if(event->key == InputKeyOk) {
                    model->ok_pressed = false;
                    hid_hal_keyboard_release(hid_jackbox->hid, HID_KEYBOARD_RETURN);
                } else if(event->key == InputKeyBack) {
                    model->back_pressed = false;
                }
            } else if(event->type == InputTypeShort) {
                if(event->key == InputKeyBack) {
                    hid_hal_keyboard_press(hid_jackbox->hid, HID_KEYBOARD_ESCAPE);
                    hid_hal_keyboard_release(hid_jackbox->hid, HID_KEYBOARD_ESCAPE);
                }
            }
        },
        true);
}

static bool hid_jackbox_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    HidJackbox* hid_jackbox = context;
    bool consumed = false;

    if(event->type == InputTypeLong && event->key == InputKeyBack) {
        hid_hal_keyboard_release_all(hid_jackbox->hid);
    } else {
        hid_jackbox_process(hid_jackbox, event);
        consumed = true;
    }

    return consumed;
}

HidJackbox* hid_jackbox_alloc(Hid* hid) {
    HidJackbox* hid_jackbox = malloc(sizeof(HidJackbox));
    hid_jackbox->view = view_alloc();
    hid_jackbox->hid = hid;
    view_set_context(hid_jackbox->view, hid_jackbox);
    view_allocate_model(hid_jackbox->view, ViewModelTypeLocking, sizeof(HidJackboxModel));
    view_set_draw_callback(hid_jackbox->view, hid_jackbox_draw_callback);
    view_set_input_callback(hid_jackbox->view, hid_jackbox_input_callback);

    return hid_jackbox;
}

void hid_jackbox_free(HidJackbox* hid_jackbox) {
    furi_assert(hid_jackbox);
    view_free(hid_jackbox->view);
    free(hid_jackbox);
}

View* hid_jackbox_get_view(HidJackbox* hid_jackbox) {
    furi_assert(hid_jackbox);
    return hid_jackbox->view;
}

void hid_jackbox_set_connected_status(HidJackbox* hid_jackbox, bool connected) {
    furi_assert(hid_jackbox);
    with_view_model(
        hid_jackbox->view, HidJackboxModel * model, { model->connected = connected; }, true);
}
