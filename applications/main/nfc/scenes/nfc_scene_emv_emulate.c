#include "../nfc_i.h"
#include "../helpers/nfc_emv_parser.h"

#define NFC_MF_UL_DATA_NOT_CHANGED (0UL)
#define NFC_MF_UL_DATA_CHANGED (1UL)

bool nfc_emv_emulate_worker_callback(NfcWorkerEvent event, void* context) {
    UNUSED(event);
    Nfc* nfc = context;

    scene_manager_set_scene_state(
        nfc->scene_manager, NfcSceneEmvEmulate, NFC_MF_UL_DATA_CHANGED);
    return true;
}

void nfc_scene_emv_emulate_on_enter(void* context) {
    Nfc* nfc = context;

    // Setup view
    Popup* popup = nfc->popup;
    EmvData* emv_data = &nfc->dev->dev_data.emv_data;

    popup_set_header(popup, "Hacking ATM", 56, 13, AlignLeft, AlignTop);
    if(strcmp(nfc->dev->dev_name, "") != 0) {
        nfc_text_store_set(nfc, "%s", nfc->dev->dev_name);
    } else {
        FuriString *temp_str;
        if (emv_data->name[0] != '\0') {
            temp_str = furi_string_alloc_printf("\e%s\n", emv_data->name);
        } else {
            temp_str = furi_string_alloc_printf("\eUnknown Bank Card\n");
        }
        if (emv_data->number_len) {
            furi_string_cat(temp_str, "#");
            for (uint8_t i = 0; i < emv_data->number_len; i += 2) {
                furi_string_cat_printf(
                        temp_str, "%02X%02X ", emv_data->number[i], emv_data->number[i + 1]);
            }
            furi_string_trim(temp_str);
        } else if (emv_data->aid_len) {
            furi_string_cat_printf(temp_str, "Can't parse data from app\n");
            // Parse AID name
            FuriString *aid_name;
            aid_name = furi_string_alloc();
            if (nfc_emv_parser_get_aid_name(
                    nfc->dev->storage, emv_data->aid, emv_data->aid_len, aid_name)) {
                furi_string_cat_printf(temp_str, "AID: %s", furi_string_get_cstr(aid_name));
            } else {
                furi_string_cat_printf(temp_str, "AID: ");
                for (uint8_t i = 0; i < emv_data->aid_len; i++) {
                    furi_string_cat_printf(temp_str, "%02X", emv_data->aid[i]);
                }
            }
            furi_string_free(aid_name);
        }

        nfc_text_store_set(nfc, furi_string_get_cstr(temp_str));
    }
    popup_set_icon(popup, 0, 3, &I_NFC_dolphin_emulation_47x61);
    popup_set_text(popup, nfc->text_store, 90, 28, AlignCenter, AlignTop);

    // Setup and start worker
    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewPopup);
    nfc_worker_start(
        nfc->worker,
        NfcWorkerStateEMVEmulate,
        &nfc->dev->dev_data,
        nfc_emv_emulate_worker_callback,
        nfc);
    nfc_blink_emulate_start(nfc);
}

bool nfc_scene_emv_emulate_on_event(void* context, SceneManagerEvent event) {
    Nfc* nfc = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeBack) {
        // Stop worker
        nfc_worker_stop(nfc->worker);
        // Check if data changed and save in shadow file
        if(scene_manager_get_scene_state(nfc->scene_manager, NfcSceneEmvEmulate) ==
           NFC_MF_UL_DATA_CHANGED) {
            scene_manager_set_scene_state(
                nfc->scene_manager, NfcSceneEmvEmulate, NFC_MF_UL_DATA_NOT_CHANGED);
            // Save shadow file
            if(furi_string_size(nfc->dev->load_path)) {
                nfc_device_save_shadow(nfc->dev, furi_string_get_cstr(nfc->dev->load_path));
            }
        }
        consumed = false;
    }
    return consumed;
}

void nfc_scene_emv_emulate_on_exit(void* context) {
    Nfc* nfc = context;

    // Clear view
    popup_reset(nfc->popup);

    nfc_blink_stop(nfc);
}
