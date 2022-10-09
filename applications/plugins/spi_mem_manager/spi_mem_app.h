#pragma once

#include <furi.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/submenu.h>
#include <gui/modules/dialog_ex.h>
#include <gui/modules/popup.h>
#include <notification/notification_messages.h>
#include "scenes/spi_mem_scene.h"
#include "lib/spi/spi_mem_worker.h"
#include "spi_mem_manager_icons.h"

typedef struct {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;
    Submenu* submenu;
    DialogEx* dialog_ex;
    Popup* popup;
    NotificationApp* notifications;
    SPIMemWorker* worker;
} SPIMemApp;

typedef enum { SPIMemViewSubmenu, SPIMemViewDialogEx, SPIMemViewPopup } SPIMemView;
