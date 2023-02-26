#pragma once

#include <furi_hal_nfc.h>

#define MAX_APDU_LEN 255

// Contains one or more data objects relevant to an application directory entry according to ISO/IEC 7816-5
#define EMV_TAG_APP_TEMPLATE 0x61 // 97
// Identifies the application as described in ISO/IEC 7816-5
#define EMV_TAG_AID 0x4F // 79
#define EMV_TAG_PRIORITY 0x87 // 135
#define EMV_TAG_PDOL 0x9F38 // 40760
#define EMV_TAG_CARD_NAME 0x50 // 80
#define EMV_TAG_FCI 0xBF0C // 48908
#define EMV_TAG_LOG_CTRL 0x9F4D // 40781
#define EMV_TAG_TRACK_1_EQUIV 0x56 // 86
#define EMV_TAG_TRACK_2_EQUIV 0x57 // 87
// Valid cardholder account numbe
#define EMV_TAG_PAN 0x5A // 90
#define EMV_TAG_AFL 0x94 // 148
#define EMV_TAG_EXP_DATE 0x5F24 // 24356
#define EMV_TAG_COUNTRY_CODE 0x5F28 // 24360
#define EMV_TAG_CURRENCY_CODE 0x9F42 // 40770
#define EMV_TAG_CARDHOLDER_NAME 0x5F20 // 24352

// Mnemonic associated with the AID according to ISO/IEC 7816-5"
#define EMV_TAG_ALABEL 0x50
// ISO-7816 Path
#define EMV_TAG_PATH 0x51
// Identifies the FCI template according to ISO/IEC 7816-4
#define EMV_TAG_FCI_TEMPLATE

typedef struct {
    char name[32];
    uint8_t aid[16];
    uint16_t aid_len;
    uint8_t number[10];
    uint8_t number_len;
    uint8_t exp_mon;
    uint8_t exp_year;
    uint16_t country_code;
    uint16_t currency_code;
} EmvData;

typedef struct {
    uint16_t tag;
    uint8_t data[];
} PDOLValue;

typedef struct {
    uint8_t size;
    uint8_t data[MAX_APDU_LEN];
} APDU;

typedef struct {
    uint8_t priority;
    uint8_t aid[16];
    uint8_t aid_len;
    bool app_started;
    char name[32];
    bool name_found;
    uint8_t card_number[10];
    uint8_t card_number_len;
    uint8_t exp_month;
    uint8_t exp_year;
    uint16_t country_code;
    uint16_t currency_code;
    APDU pdol;
    APDU afl;
} EmvApplication;

/** Read bank card data
 * @note Search EMV Application, start it, try to read AID, PAN, card name,
 * expiration date, currency and country codes
 *
 * @param tx_rx     FuriHalNfcTxRxContext instance
 * @param emv_app   EmvApplication instance
 * 
 * @return true on success
 */
bool emv_read_bank_card(FuriHalNfcTxRxContext* tx_rx, EmvApplication* emv_app);

/** Emulate bank card
 * @note Answer to application selection and PDOL
 *
 * @param tx_rx     FuriHalNfcTxRxContext instance
 *
 * @return true on success
 */
bool emv_card_emulation(FuriHalNfcTxRxContext* tx_rx);

struct PPSECmd {
    bool parsing;
    uint8_t length;
    uint8_t expected_length;
    uint8_t data[40];
};

typedef struct {
    EmvData data;
    void* context;
    struct PPSECmd cmd;
    uint8_t state;
} EmvEmulator;

void emv_reset_emulation(EmvEmulator* emulator);

void emv_prepare_emulation(EmvEmulator* emulator, EmvData* data);

bool emv_prepare_emulation_response(
        uint8_t* buff_rx,
        uint16_t buff_rx_len,
        uint8_t* buff_tx,
        uint16_t* buff_tx_len,
        uint32_t* data_type,
        void* context);
