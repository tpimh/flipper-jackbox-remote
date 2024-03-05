#include "hid.h"
#include "views.h"
#include <notification/notification_messages.h>
#include <dolphin/dolphin.h>

#define TAG "HidApp"

static void bt_hid_connection_status_changed_callback(BtStatus status, void* context) {
    furi_assert(context);
    Hid* hid = context;
    bool connected = (status == BtStatusConnected);
    if(connected) {
        notification_internal_message(hid->notifications, &sequence_set_blue_255);
    } else {
        notification_internal_message(hid->notifications, &sequence_reset_blue);
    }
    hid_jackbox_set_connected_status(hid->hid_jackbox, connected);
}

static void hid_dialog_callback(DialogExResult result, void* context) {
    furi_assert(context);
    Hid* app = context;
    if(result == DialogExResultLeft) {
        view_dispatcher_stop(app->view_dispatcher);
    } else if(result == DialogExResultRight) {
        view_dispatcher_switch_to_view(app->view_dispatcher, app->view_id); // Show last view
    }
}

static uint32_t hid_exit_confirm_view(void* context) {
    UNUSED(context);
    return HidViewExitConfirm;
}

Hid* hid_alloc(HidTransport transport) {
    Hid* app = malloc(sizeof(Hid));
    app->transport = transport;

    // Gui
    app->gui = furi_record_open(RECORD_GUI);

    // Bt
    app->bt = furi_record_open(RECORD_BT);

    // Notifications
    app->notifications = furi_record_open(RECORD_NOTIFICATION);

    // View dispatcher
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    return app;
}

Hid* hid_app_alloc_view(void* context) {
    furi_assert(context);
    Hid* app = context;
    // Dialog view
    app->dialog = dialog_ex_alloc();
    dialog_ex_set_result_callback(app->dialog, hid_dialog_callback);
    dialog_ex_set_context(app->dialog, app);
    dialog_ex_set_left_button_text(app->dialog, "Exit");
    dialog_ex_set_right_button_text(app->dialog, "Stay");
    dialog_ex_set_header(app->dialog, "Close Current App?", 16, 12, AlignLeft, AlignTop);
    view_dispatcher_add_view(
        app->view_dispatcher, HidViewExitConfirm, dialog_ex_get_view(app->dialog));

    // Jackbox view
    app->hid_jackbox = hid_jackbox_alloc(app);
    view_set_previous_callback(hid_jackbox_get_view(app->hid_jackbox), hid_exit_confirm_view);
    view_dispatcher_add_view(
        app->view_dispatcher, HidViewJackbox, hid_jackbox_get_view(app->hid_jackbox));

    app->view_id = HidViewJackbox;
    view_dispatcher_switch_to_view(app->view_dispatcher, app->view_id);

    return app;
}

void hid_free(Hid* app) {
    furi_assert(app);

    // Reset notification
    notification_internal_message(app->notifications, &sequence_reset_blue);

    // Free views
    view_dispatcher_remove_view(app->view_dispatcher, HidViewExitConfirm);
    dialog_ex_free(app->dialog);
    view_dispatcher_remove_view(app->view_dispatcher, HidViewJackbox);
    hid_jackbox_free(app->hid_jackbox);
    view_dispatcher_free(app->view_dispatcher);

    // Close records
    furi_record_close(RECORD_GUI);
    app->gui = NULL;
    furi_record_close(RECORD_NOTIFICATION);
    app->notifications = NULL;
    furi_record_close(RECORD_BT);
    app->bt = NULL;

    // Free rest
    free(app);
}

void hid_hal_keyboard_press(Hid* instance, uint16_t event) {
    furi_assert(instance);
    if(instance->transport == HidTransportBle) {
        furi_hal_bt_hid_kb_press(event);
    } else {
        furi_crash();
    }
}

void hid_hal_keyboard_release(Hid* instance, uint16_t event) {
    furi_assert(instance);
    if(instance->transport == HidTransportBle) {
        furi_hal_bt_hid_kb_release(event);
    } else {
        furi_crash();
    }
}

void hid_hal_keyboard_release_all(Hid* instance) {
    furi_assert(instance);
    if(instance->transport == HidTransportBle) {
        furi_hal_bt_hid_kb_release_all();
    } else {
        furi_crash();
    }
}

int32_t hid_ble_jackbox_app(void* p) {
    UNUSED(p);
    Hid* app = hid_alloc(HidTransportBle);
    app = hid_app_alloc_view(app);

    bt_disconnect(app->bt);

    furi_check(bt_set_profile(app->bt, BtProfileHidKeyboard));

    furi_hal_bt_start_advertising();
    bt_set_status_changed_callback(app->bt, bt_hid_connection_status_changed_callback, app);

    dolphin_deed(DolphinDeedPluginStart);

    view_dispatcher_run(app->view_dispatcher);

    bt_set_status_changed_callback(app->bt, NULL, NULL);

    bt_disconnect(app->bt);

    furi_check(bt_set_profile(app->bt, BtProfileSerial));

    hid_free(app);

    return 0;
}
