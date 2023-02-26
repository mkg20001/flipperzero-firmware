#include "../nfc_i.h"
#include <dolphin/dolphin.h>

enum SubmenuIndex {
    SubmenuIndexSave,
    SubmenuIndexEmulate,
    SubmenuIndexInfo,
};

void nfc_scene_emv_menu_submenu_callback(void* context, uint32_t index) {
    Nfc* nfc = context;

    view_dispatcher_send_custom_event(nfc->view_dispatcher, index);
}

void nfc_scene_emv_menu_on_enter(void* context) {
    Nfc* nfc = context;
    Submenu* submenu = nfc->submenu;

    submenu_add_item(submenu, "Save", SubmenuIndexSave, nfc_scene_emv_menu_submenu_callback, nfc);
    submenu_add_item(submenu, "Emulate", SubmenuIndexEmulate, nfc_scene_emv_menu_submenu_callback, nfc);
    submenu_add_item(submenu, "Info", SubmenuIndexInfo, nfc_scene_emv_menu_submenu_callback, nfc);

    submenu_set_selected_item(
        nfc->submenu, scene_manager_get_scene_state(nfc->scene_manager, NfcSceneEmvMenu));

    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewMenu);
}

bool nfc_scene_emv_menu_on_event(void* context, SceneManagerEvent event) {
    Nfc* nfc = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(nfc->scene_manager, NfcSceneEmvMenu, event.event);
        if(event.event == SubmenuIndexSave) {
            nfc->dev->format = NfcDeviceSaveFormatEMV;
            scene_manager_next_scene(nfc->scene_manager, NfcSceneSaveName);
            consumed = true;
        } else if(event.event == SubmenuIndexEmulate) {
            scene_manager_next_scene(nfc->scene_manager, NfcSceneEmvEmulate);
            if(scene_manager_has_previous_scene(nfc->scene_manager, NfcSceneSetType)) {
                DOLPHIN_DEED(DolphinDeedNfcAddEmulate);
            } else {
                DOLPHIN_DEED(DolphinDeedNfcEmulate);
            }
            consumed = true;
        } else if(event.event == SubmenuIndexInfo) {
            scene_manager_next_scene(nfc->scene_manager, NfcSceneNfcDataInfo);
            consumed = true;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        consumed = scene_manager_previous_scene(nfc->scene_manager);
    }

    return consumed;
}

void nfc_scene_emv_menu_on_exit(void* context) {
    Nfc* nfc = context;

    // Clear view
    submenu_reset(nfc->submenu);
}
