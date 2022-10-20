#include "../spi_mem_app.h"

static void spi_mem_chip_detect_callback(void* context, SPIMemWorkerChipDetectResult result) {
    SPIMemApp* app = context;
    SPIMemCustomEventWorker event;
    if(result == SPIMemWorkerChipDetectResultSuccess) {
        event = SPIMemCustomEventWorkerChipDetectSuccess;
    } else if(result == SPIMemWorkerChipDetectResultFail) {
        event = SPIMemCustomEventWorkerChipDetectFail;
    } else {
        return;
    }
    view_dispatcher_send_custom_event(app->view_dispatcher, event);
}

void spi_mem_scene_chip_detect_on_enter(void* context) {
    SPIMemApp* app = context;
    popup_set_text(app->popup, "Looking\nfor SPI chip ...", 95, 25, AlignCenter, AlignTop);
    notification_message(app->notifications, &sequence_blink_start_cyan);
    view_dispatcher_switch_to_view(app->view_dispatcher, SPIMemViewPopup);
    spi_mem_worker_start_thread(app->worker);
    spi_mem_worker_chip_detect_start(
        app->chip_info, app->worker, spi_mem_chip_detect_callback, app);
}

bool spi_mem_scene_chip_detect_on_event(void* context, SceneManagerEvent event) {
    SPIMemApp* app = context;
    UNUSED(app);
    bool consumed = false;
    if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;
        if(event.event == SPIMemCustomEventWorkerChipDetectSuccess) {
            scene_manager_next_scene(app->scene_manager, SPIMemSceneChipDetected);
        } else if(event.event == SPIMemCustomEventWorkerChipDetectFail) {
            scene_manager_next_scene(app->scene_manager, SPIMemSceneChipDetectFailed);
        }
    }
    return consumed;
}

void spi_mem_scene_chip_detect_on_exit(void* context) {
    SPIMemApp* app = context;
    spi_mem_worker_stop(app->worker);
    spi_mem_worker_stop_thread(app->worker);
    notification_message(app->notifications, &sequence_blink_stop);
    popup_reset(app->popup);
}
