/* 
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include "tusb.h"

#include "pico/stdlib.h"
#include "pico/unique_id.h"

#define USB_VID 0x1209
#define USB_PID 0xACED

//--------------------------------------------------------------------+
// Device Descriptors
//--------------------------------------------------------------------+
tusb_desc_device_t const desc_device =
{
    .bLength            = sizeof(tusb_desc_device_t),
    .bDescriptorType    = TUSB_DESC_DEVICE,
    .bcdUSB             = 0x0200,

    // Use Interface Association Descriptor (IAD) for CDC
    // As required by USB Specs IAD's subclass must be common class (2) and protocol must be IAD (1)
    .bDeviceClass       = TUSB_CLASS_MISC,
    .bDeviceSubClass    = MISC_SUBCLASS_COMMON,
    .bDeviceProtocol    = MISC_PROTOCOL_IAD,

    .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,
    .idVendor           = USB_VID,
    .idProduct          = USB_PID,
    .bcdDevice          = 0x0100,
    .iManufacturer      = 0x01,
    .iProduct           = 0x02,
    .iSerialNumber      = 0x03,
    .bNumConfigurations = 0x01
};

// Invoked when received GET DEVICE DESCRIPTOR
// Application return pointer to descriptor
uint8_t const *tud_descriptor_device_cb(void) {
    return (uint8_t const *) &desc_device;
}

//--------------------------------------------------------------------+
// Configuration Descriptor
//--------------------------------------------------------------------+
enum {
    ITF_NUM_CDC_CMD,
    ITF_NUM_CDC_DATA,
    ITF_NUM_PORT01,
    ITF_NUM_PORT02,
    ITF_NUM_PORT03,
    ITF_NUM_PORT04,
    ITF_NUM_PORT05,
    ITF_NUM_PORT06,
    ITF_NUM_PORT07,
    ITF_NUM_PORT08,
    ITF_NUM_PORT09,
    ITF_NUM_TOTAL
};

#define  CONFIG_TOTAL_LEN  (TUD_CONFIG_DESC_LEN + TUD_CDC_DESC_LEN + CFG_TUD_VENDOR * TUD_VENDOR_DESC_LEN)

// Endpoints 0x00 and 0x80 are for CONTROL transfers => do not use

#define USBD_CDC_EP_CMD          0x81
#define USBD_CDC_EP_OUT          0x02
#define USBD_CDC_EP_IN           0x82
#define USBD_CDC_CMD_MAX_SIZE       8
#define USBD_CDC_IN_OUT_MAX_SIZE   64

#define USBD_PORT00_EP_OUT       0x02
#define USBD_PORT00_EP_IN        0x82


uint8_t const desc_configuration[] =
{
    // Config number, interface count, string index, total length, attribute, power in mA
    TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, CONFIG_TOTAL_LEN,
        TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 500),

    // Interface number, string index, EP notification address and size, EP data address (out, in) and size.
    TUD_CDC_DESCRIPTOR(ITF_NUM_CDC_CMD, 4, USBD_CDC_EP_CMD,
        USBD_CDC_CMD_MAX_SIZE, USBD_CDC_EP_OUT, USBD_CDC_EP_IN,
        USBD_CDC_IN_OUT_MAX_SIZE),

    // Interface number, string index, EP Out & IN address, EP size
    // However, cannot set SubClass and Protocol this way :(
    //TUD_VENDOR_DESCRIPTOR(ITF_NUM_PORT00, 5, USBD_PORT00_EP_OUT, USBD_PORT00_EP_IN, 64),

    9, TUSB_DESC_INTERFACE, ITF_NUM_PORT01, 0, 2, TUSB_CLASS_VENDOR_SPECIFIC, 0xff, 0xff, 1 + 4,\
    /* Endpoint Out */\
    7, TUSB_DESC_ENDPOINT, 1 + USBD_PORT00_EP_OUT, TUSB_XFER_BULK, U16_TO_U8S_LE(64), 0,\
    /* Endpoint In */\
    7, TUSB_DESC_ENDPOINT, 1 + USBD_PORT00_EP_IN, TUSB_XFER_BULK, U16_TO_U8S_LE(64), 0,

    9, TUSB_DESC_INTERFACE, ITF_NUM_PORT02, 0, 2, TUSB_CLASS_VENDOR_SPECIFIC, 0xff, 0xff, 2 + 4,\
    /* Endpoint Out */\
    7, TUSB_DESC_ENDPOINT, 2 + USBD_PORT00_EP_OUT, TUSB_XFER_BULK, U16_TO_U8S_LE(64), 0,\
    /* Endpoint In */\
    7, TUSB_DESC_ENDPOINT, 2 + USBD_PORT00_EP_IN, TUSB_XFER_BULK, U16_TO_U8S_LE(64), 0,

    9, TUSB_DESC_INTERFACE, ITF_NUM_PORT03, 0, 2, TUSB_CLASS_VENDOR_SPECIFIC, 0xff, 0xff, 3 + 4,\
    /* Endpoint Out */\
    7, TUSB_DESC_ENDPOINT, 3 + USBD_PORT00_EP_OUT, TUSB_XFER_BULK, U16_TO_U8S_LE(64), 0,\
    /* Endpoint In */\
    7, TUSB_DESC_ENDPOINT, 3 + USBD_PORT00_EP_IN, TUSB_XFER_BULK, U16_TO_U8S_LE(64), 0,

    9, TUSB_DESC_INTERFACE, ITF_NUM_PORT04, 0, 2, TUSB_CLASS_VENDOR_SPECIFIC, 0xff, 0xff, 4 + 4,\
    /* Endpoint Out */\
    7, TUSB_DESC_ENDPOINT, 4 + USBD_PORT00_EP_OUT, TUSB_XFER_BULK, U16_TO_U8S_LE(64), 0,\
    /* Endpoint In */\
    7, TUSB_DESC_ENDPOINT, 4 + USBD_PORT00_EP_IN, TUSB_XFER_BULK, U16_TO_U8S_LE(64), 0,

    9, TUSB_DESC_INTERFACE, ITF_NUM_PORT05, 0, 2, TUSB_CLASS_VENDOR_SPECIFIC, 0xff, 0xff, 5 + 4,\
    /* Endpoint Out */\
    7, TUSB_DESC_ENDPOINT, 5 + USBD_PORT00_EP_OUT, TUSB_XFER_BULK, U16_TO_U8S_LE(64), 0,\
    /* Endpoint In */\
    7, TUSB_DESC_ENDPOINT, 5 + USBD_PORT00_EP_IN, TUSB_XFER_BULK, U16_TO_U8S_LE(64), 0,

    9, TUSB_DESC_INTERFACE, ITF_NUM_PORT06, 0, 2, TUSB_CLASS_VENDOR_SPECIFIC, 0xff, 0xff, 6 + 4,\
    /* Endpoint Out */\
    7, TUSB_DESC_ENDPOINT, 6 + USBD_PORT00_EP_OUT, TUSB_XFER_BULK, U16_TO_U8S_LE(64), 0,\
    /* Endpoint In */\
    7, TUSB_DESC_ENDPOINT, 6 + USBD_PORT00_EP_IN, TUSB_XFER_BULK, U16_TO_U8S_LE(64), 0,

    9, TUSB_DESC_INTERFACE, ITF_NUM_PORT07, 0, 2, TUSB_CLASS_VENDOR_SPECIFIC, 0xff, 0xff, 7 + 4,\
    /* Endpoint Out */\
    7, TUSB_DESC_ENDPOINT, 7 + USBD_PORT00_EP_OUT, TUSB_XFER_BULK, U16_TO_U8S_LE(64), 0,\
    /* Endpoint In */\
    7, TUSB_DESC_ENDPOINT, 7 + USBD_PORT00_EP_IN, TUSB_XFER_BULK, U16_TO_U8S_LE(64), 0,

    9, TUSB_DESC_INTERFACE, ITF_NUM_PORT08, 0, 2, TUSB_CLASS_VENDOR_SPECIFIC, 0xff, 0xff, 8 + 4,\
    /* Endpoint Out */\
    7, TUSB_DESC_ENDPOINT, 8 + USBD_PORT00_EP_OUT, TUSB_XFER_BULK, U16_TO_U8S_LE(64), 0,\
    /* Endpoint In */\
    7, TUSB_DESC_ENDPOINT, 8 + USBD_PORT00_EP_IN, TUSB_XFER_BULK, U16_TO_U8S_LE(64), 0,

    9, TUSB_DESC_INTERFACE, ITF_NUM_PORT09, 0, 2, TUSB_CLASS_VENDOR_SPECIFIC, 0xff, 0xff, 9 + 4,\
    /* Endpoint Out */\
    7, TUSB_DESC_ENDPOINT, 9 + USBD_PORT00_EP_OUT, TUSB_XFER_BULK, U16_TO_U8S_LE(64), 0,\
    /* Endpoint In */\
    7, TUSB_DESC_ENDPOINT, 9 + USBD_PORT00_EP_IN, TUSB_XFER_BULK, U16_TO_U8S_LE(64), 0,

};

// Invoked when received GET CONFIGURATION DESCRIPTOR
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
uint8_t const *tud_descriptor_configuration_cb(uint8_t index) {
    (void) index; // for multiple configurations
    return desc_configuration;
}

//--------------------------------------------------------------------+
// String Descriptors
//--------------------------------------------------------------------+

// array of pointer to string descriptors
char const *string_desc_arr[] =
{
    (const char[]) {0x09, 0x04}, // 0: is supported language is English (0x0409)
    "DE/FX5/U1 Clone",           // 1: Manufacturer
    "16Tx 00Rx S",               // 2: Product
    "RP2040_0123456789ABCDEF",   // 3: Serial, fallback here, it's dynamically created later
    "16Tx00RxS_000001",          // 4: CDC interface name
    "Port 01",
    "Port 02",
    "Port 03",
    "Port 04",
    "Port 05",
    "Port 06",
    "Port 07",
    "Port 08",
    "Port 09"
};

static uint16_t _desc_str[32];

// Invoked when received GET STRING DESCRIPTOR request
// Application return pointer to descriptor, whose contents must exist long enough for transfer to complete
uint16_t const *tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
    (void) langid;
    char *str = NULL;
    uint8_t chr_count;

    if (index == 3) {
        // Serial number has been requested, construct it from the unique board id
        pico_unique_board_id_t board_id;
        pico_get_unique_board_id(&board_id);

        char serial[33];
        str = serial;

        snprintf(serial, 32, "7a70:");

        for (int i = 0; (i < PICO_UNIQUE_BOARD_ID_SIZE_BYTES) && (i < 4); ++i) {
            snprintf(serial + i*2 + 5, 32, "%02x", board_id.id[i+4]);
        }
    }

    if (index == 0) {
        memcpy(&_desc_str[1], string_desc_arr[0], 2);
        chr_count = 1;
    } else {
        // Convert ASCII string into UTF-16

        if (!(index < sizeof(string_desc_arr) / sizeof(string_desc_arr[0]))) return NULL;

        if (str == NULL) {
            str = string_desc_arr[index];
        }

        // Cap at max char
        chr_count = strlen(str);
        if (chr_count > 31) chr_count = 31;

        for (uint8_t i = 0; i < chr_count; i++) {
            _desc_str[1 + i] = str[i];
        }
    }

    // first byte is length (including header), second byte is string type
    _desc_str[0] = (TUSB_DESC_STRING << 8) | (2 * chr_count + 2);

    return _desc_str;
}
