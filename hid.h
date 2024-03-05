#pragma once

#include <furi.h>
#include <furi_hal_bt.h>
#include <furi_hal_bt_hid.h>
#include <furi_hal_usb_hid.h>

#include <bt/bt_service/bt.h>
#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <notification/notification.h>

#include <gui/modules/dialog_ex.h>
#include <gui/modules/popup.h>
#include "views/hid_jackbox.h"

typedef enum {
    HidTransportBle,
} HidTransport;

typedef struct Hid Hid;

struct Hid {
    Bt* bt;
    Gui* gui;
    NotificationApp* notifications;
    ViewDispatcher* view_dispatcher;
    DialogEx* dialog;
    HidJackbox* hid_jackbox;

    HidTransport transport;
    uint32_t view_id;
};

void hid_hal_keyboard_press(Hid* instance, uint16_t event);
void hid_hal_keyboard_release(Hid* instance, uint16_t event);
void hid_hal_keyboard_release_all(Hid* instance);
