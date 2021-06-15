#include "nfc_i.h"
#include "api-hal-nfc.h"

osMessageQueueId_t message_queue = NULL;

uint32_t nfc_view_stop(void* context) {
    furi_assert(message_queue);
    NfcMessage message;
    message.type = NfcMessageTypeStop;
    furi_check(osMessageQueuePut(message_queue, &message, 0, osWaitForever) == osOK);
    return NfcViewMenu;
}

uint32_t nfc_view_exit(void* context) {
    furi_assert(message_queue);
    NfcMessage message;
    message.type = NfcMessageTypeExit;
    furi_check(osMessageQueuePut(message_queue, &message, 0, osWaitForever) == osOK);
    return VIEW_NONE;
}

void nfc_menu_callback(void* context, uint32_t index) {
    furi_assert(message_queue);
    NfcMessage message;
    message.type = index;
    furi_check(osMessageQueuePut(message_queue, &message, 0, osWaitForever) == osOK);
}

Nfc* nfc_alloc() {
    Nfc* nfc = furi_alloc(sizeof(Nfc));

    message_queue = osMessageQueueNew(8, sizeof(NfcMessage), NULL);

    nfc->worker = nfc_worker_alloc(message_queue);

    // Open GUI record
    nfc->gui = furi_record_open("gui");

    // View Dispatcher
    nfc->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_attach_to_gui(nfc->view_dispatcher, nfc->gui, ViewDispatcherTypeFullscreen);

    // Menu
    nfc->submenu = submenu_alloc();
    submenu_add_item(nfc->submenu, "Detect", NfcMessageTypeDetect, nfc_menu_callback, nfc);
    submenu_add_item(nfc->submenu, "Read EMV", NfcMessageTypeReadEMV, nfc_menu_callback, nfc);
    submenu_add_item(
        nfc->submenu, "Emulate EMV", NfcMessageTypeEmulateEMV, nfc_menu_callback, nfc);
    submenu_add_item(nfc->submenu, "Emulate", NfcMessageTypeEmulate, nfc_menu_callback, nfc);
    submenu_add_item(nfc->submenu, "Field", NfcMessageTypeField, nfc_menu_callback, nfc);
    submenu_add_item(
        nfc->submenu, "Read MfUltralight", NfcMessageTypeReadMfUltralight, nfc_menu_callback, nfc);

    View* submenu_view = submenu_get_view(nfc->submenu);
    view_set_previous_callback(submenu_view, nfc_view_exit);
    view_dispatcher_add_view(nfc->view_dispatcher, NfcViewMenu, submenu_view);

    // Detect
    nfc->view_detect = view_alloc();
    view_set_context(nfc->view_detect, nfc);
    view_set_draw_callback(nfc->view_detect, nfc_view_read_draw);
    view_set_previous_callback(nfc->view_detect, nfc_view_stop);
    view_allocate_model(nfc->view_detect, ViewModelTypeLocking, sizeof(NfcViewReadModel));
    view_dispatcher_add_view(nfc->view_dispatcher, NfcViewRead, nfc->view_detect);

    // Read EMV
    nfc->view_read_emv = view_alloc();
    view_set_context(nfc->view_read_emv, nfc);
    view_set_draw_callback(nfc->view_read_emv, nfc_view_read_emv_draw);
    view_set_previous_callback(nfc->view_read_emv, nfc_view_stop);
    view_allocate_model(nfc->view_read_emv, ViewModelTypeLocking, sizeof(NfcViewReadModel));
    view_dispatcher_add_view(nfc->view_dispatcher, NfcViewReadEmv, nfc->view_read_emv);

    // Emulate EMV
    nfc->view_emulate_emv = view_alloc();
    view_set_context(nfc->view_emulate_emv, nfc);
    view_set_draw_callback(nfc->view_emulate_emv, nfc_view_emulate_emv_draw);
    view_set_previous_callback(nfc->view_emulate_emv, nfc_view_stop);
    view_dispatcher_add_view(nfc->view_dispatcher, NfcViewEmulateEMV, nfc->view_emulate_emv);

    // Emulate
    nfc->view_emulate = view_alloc();
    view_set_context(nfc->view_emulate, nfc);
    view_set_draw_callback(nfc->view_emulate, nfc_view_emulate_draw);
    view_set_previous_callback(nfc->view_emulate, nfc_view_stop);
    view_dispatcher_add_view(nfc->view_dispatcher, NfcViewEmulate, nfc->view_emulate);

    // Field
    nfc->view_field = view_alloc();
    view_set_context(nfc->view_field, nfc);
    view_set_draw_callback(nfc->view_field, nfc_view_field_draw);
    view_set_previous_callback(nfc->view_field, nfc_view_stop);
    view_dispatcher_add_view(nfc->view_dispatcher, NfcViewField, nfc->view_field);

    // Read Mifare Ultralight
    nfc->view_read_mf_ultralight = view_alloc();
    view_set_context(nfc->view_read_mf_ultralight, nfc);
    view_set_draw_callback(nfc->view_read_mf_ultralight, nfc_view_read_mf_ultralight_draw);
    view_set_previous_callback(nfc->view_read_mf_ultralight, nfc_view_stop);
    view_allocate_model(
        nfc->view_read_mf_ultralight, ViewModelTypeLocking, sizeof(NfcViewReadModel));
    view_dispatcher_add_view(
        nfc->view_dispatcher, NfcViewReadMfUltralight, nfc->view_read_mf_ultralight);

    // Error
    nfc->view_error = view_alloc();
    view_set_context(nfc->view_error, nfc);
    view_set_draw_callback(nfc->view_error, nfc_view_error_draw);
    view_set_previous_callback(nfc->view_error, nfc_view_stop);
    view_allocate_model(nfc->view_error, ViewModelTypeLockFree, sizeof(NfcViewErrorModel));
    view_dispatcher_add_view(nfc->view_dispatcher, NfcViewError, nfc->view_error);

    // Switch to menu
    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewMenu);

    return nfc;
}

void nfc_free(Nfc* nfc) {
    // Free nfc worker
    nfc_worker_free(nfc->worker);
    // Free allocated queue
    osMessageQueueDelete(message_queue);
    message_queue = NULL;

    // Free allocated views
    // Menu
    view_dispatcher_remove_view(nfc->view_dispatcher, NfcViewMenu);
    submenu_free(nfc->submenu);

    // Detect
    view_dispatcher_remove_view(nfc->view_dispatcher, NfcViewRead);
    view_free(nfc->view_detect);

    // Read EMV
    view_dispatcher_remove_view(nfc->view_dispatcher, NfcViewReadEmv);
    view_free(nfc->view_read_emv);

    // Emulate EMV
    view_dispatcher_remove_view(nfc->view_dispatcher, NfcViewEmulateEMV);
    view_free(nfc->view_emulate_emv);

    // Emulate
    view_dispatcher_remove_view(nfc->view_dispatcher, NfcViewEmulate);
    view_free(nfc->view_emulate);

    // Field
    view_dispatcher_remove_view(nfc->view_dispatcher, NfcViewField);
    view_free(nfc->view_field);

    // Read Mifare Ultralight
    view_dispatcher_remove_view(nfc->view_dispatcher, NfcViewReadMfUltralight);
    view_free(nfc->view_read_mf_ultralight);

    // Error
    view_dispatcher_remove_view(nfc->view_dispatcher, NfcViewError);
    view_free(nfc->view_error);

    // Free View Dispatcher
    view_dispatcher_free(nfc->view_dispatcher);

    // Close all opened records
    furi_record_close("gui");
    nfc->gui = NULL;

    // Free nfc object
    free(nfc);
}

void nfc_start(Nfc* nfc, NfcView view_id, NfcWorkerState worker_state) {
    NfcWorkerState state = nfc_worker_get_state(nfc->worker);
    if(state == NfcWorkerStateBroken) {
        with_view_model(
            nfc->view_error, (NfcViewErrorModel * model) {
                model->error = nfc_worker_get_error(nfc->worker);
                return true;
            });
        view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewError);
    } else if(state == NfcWorkerStateReady) {
        view_dispatcher_switch_to_view(nfc->view_dispatcher, view_id);
        nfc_worker_start(nfc->worker, worker_state);
    }
}

int32_t nfc_task(void* p) {
    Nfc* nfc = nfc_alloc();

    NfcMessage message;
    while(1) {
        furi_check(osMessageQueueGet(message_queue, &message, NULL, osWaitForever) == osOK);

        if(message.type == NfcMessageTypeDetect) {
            with_view_model(
                nfc->view_detect, (NfcViewReadModel * model) {
                    model->found = false;
                    return true;
                });
            nfc_start(nfc, NfcViewRead, NfcWorkerStatePoll);
        } else if(message.type == NfcMessageTypeReadEMV) {
            with_view_model(
                nfc->view_read_emv, (NfcViewReadModel * model) {
                    model->found = false;
                    return true;
                });
            nfc_start(nfc, NfcViewReadEmv, NfcWorkerStateReadEMV);
        } else if(message.type == NfcMessageTypeEmulateEMV) {
            nfc_start(nfc, NfcViewEmulateEMV, NfcWorkerStateEmulateEMV);
        } else if(message.type == NfcMessageTypeEmulate) {
            nfc_start(nfc, NfcViewEmulate, NfcWorkerStateEmulate);
        } else if(message.type == NfcMessageTypeField) {
            nfc_start(nfc, NfcViewField, NfcWorkerStateField);
        } else if(message.type == NfcMessageTypeReadMfUltralight) {
            nfc_start(nfc, NfcViewReadMfUltralight, NfcWorkerStateReadMfUltralight);
        } else if(message.type == NfcMessageTypeStop) {
            nfc_worker_stop(nfc->worker);
        } else if(message.type == NfcMessageTypeDeviceFound) {
            with_view_model(
                nfc->view_detect, (NfcViewReadModel * model) {
                    model->found = true;
                    model->device = message.device;
                    return true;
                });
        } else if(message.type == NfcMessageTypeDeviceNotFound) {
            with_view_model(
                nfc->view_detect, (NfcViewReadModel * model) {
                    model->found = false;
                    return true;
                });
        } else if(message.type == NfcMessageTypeEMVFound) {
            with_view_model(
                nfc->view_read_emv, (NfcViewReadModel * model) {
                    model->found = true;
                    model->device = message.device;
                    return true;
                });
        } else if(message.type == NfcMessageTypeEMVNotFound) {
            with_view_model(
                nfc->view_read_emv, (NfcViewReadModel * model) {
                    model->found = false;
                    return true;
                });
        } else if(message.type == NfcMessageTypeMfUlFound) {
            with_view_model(
                nfc->view_read_mf_ultralight, (NfcViewReadModel * model) {
                    model->found = true;
                    model->device = message.device;
                    return true;
                });
        } else if(message.type == NfcMessageTypeMfUlNotFound) {
            with_view_model(
                nfc->view_read_mf_ultralight, (NfcViewReadModel * model) {
                    model->found = false;
                    return true;
                });
        } else if(message.type == NfcMessageTypeExit) {
            nfc_free(nfc);
            break;
        }
    }

    return 0;
}
