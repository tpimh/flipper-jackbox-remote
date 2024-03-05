#pragma once

#include <gui/view.h>

typedef struct Hid Hid;
typedef struct HidJackbox HidJackbox;

HidJackbox* hid_jackbox_alloc(Hid* bt_hid);

void hid_jackbox_free(HidJackbox* hid_jackbox);

View* hid_jackbox_get_view(HidJackbox* hid_jackbox);

void hid_jackbox_set_connected_status(HidJackbox* hid_jackbox, bool connected);
