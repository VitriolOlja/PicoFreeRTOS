// btstack_config.h


/*
To use BTstack you need to provide a btstack_config.h file in your source tree and add its location to your include path.
The BTstack configuration macros ENABLE_CLASSIC and ENABLE_BLE are defined for you when you link the
pico_btstack_classic and pico_btstack_ble libraries respectively, so you should not define them yourself.
*/
// https://bluekitchen-gmbh.com/btstack/develop/#how_to/
// https://github.com/raspberrypi/pico-examples#pico-w-bluetooth

/*
This file defines the run time configuration of 
*Bluetooth Chipset
*run loop
*hci transport layer
*provided services
* packet handlers
*/


#ifndef BTSTACK_CONFIG_H
#define BTSTACK_CONFIG_H

#ifndef ENABLE_BLE
#error Please link to pico_btstack_ble
#endif

// BTstack features that can be enabled
#define ENABLE_LE_PERIPHERAL 1
//#define ENABLE_LOG_INFO
//#define ENABLE_LOG_ERROR
#define ENABLE_PRINTF_HEXDUMP

// Enable HCI debug output
#define ENABLE_LOG_INFO      1
#define ENABLE_LOG_DEBUG     1
#define ENABLE_LOG_ERROR     1
#define ENABLE_LOG_HCI       1
#define ENABLE_LOG_PACKET    1

// for the client
#if RUNNING_AS_CLIENT
#define ENABLE_LE_CENTRAL
#define MAX_NR_GATT_CLIENTS 1
#else
#define MAX_NR_GATT_CLIENTS 0
#endif

// BTstack configuration. buffers, sizes, ...
#define HCI_OUTGOING_PRE_BUFFER_SIZE 4
#define HCI_ACL_PAYLOAD_SIZE (255 + 4)
#define HCI_ACL_CHUNK_SIZE_ALIGNMENT 4
#define MAX_NR_HCI_CONNECTIONS 1
#define MAX_NR_SM_LOOKUP_ENTRIES 3
#define MAX_NR_WHITELIST_ENTRIES 16
#define MAX_NR_LE_DEVICE_DB_ENTRIES 16

// Limit number of ACL/SCO Buffer to use by stack to avoid cyw43 shared bus overrun
#define MAX_NR_CONTROLLER_ACL_BUFFERS 3
#define MAX_NR_CONTROLLER_SCO_PACKETS 3

// Enable and configure HCI Controller to Host Flow Control to avoid cyw43 shared bus overrun
#define ENABLE_HCI_CONTROLLER_TO_HOST_FLOW_CONTROL
#define HCI_HOST_ACL_PACKET_LEN (255+4)
#define HCI_HOST_ACL_PACKET_NUM 3
#define HCI_HOST_SCO_PACKET_LEN 120
#define HCI_HOST_SCO_PACKET_NUM 3

// Link Key DB and LE Device DB using TLV on top of Flash Sector interface
#define NVM_NUM_DEVICE_DB_ENTRIES 16
#define NVM_NUM_LINK_KEYS 16

// We don't give btstack a malloc, so use a fixed-size ATT DB.
#define MAX_ATT_DB_SIZE 512

// BTstack HAL configuration
#define HAVE_EMBEDDED_TIME_MS
// map btstack_assert onto Pico SDK assert()
#define HAVE_ASSERT
// Some USB dongles take longer to respond to HCI reset (e.g. BCM20702A).
#define HCI_RESET_RESEND_TIMEOUT_MS 1000
#define ENABLE_SOFTWARE_AES128
#define ENABLE_MICRO_ECC_FOR_LE_SECURE_CONNECTIONS



#endif //BTSTACK_CONFIG_H



/*
/* HAVE_* directives */
/*system properties*/
/*
#define HAVE_AES128 0 // do not use AES128 engine
#define HAVE_BTSTACK_STDIN 0 //stdin available for cli interface 
#define HAVE_LWIP 1 // we want lwip
#define HAVE_MALLOC 1 //use dynamic memory? not sure but i guess.
#define HAVE_MBEDTLS_ECC_P256 1 //mbedTLS proviedes NIST P-256 operations e.g. for LE Sec conns. 
*/
/* Embedded platform properties */
// #define HAVE_EMBEDDED_TIME_MS 1 //System provides time in ms
// #define HAVE_EMBEDDED_TICK 1 //system provides tick interrupt
// #define HAVE_HAL_AUDIO 0 //AUDIO HAL is available? 
// #define HAVE_HAL_AUDIO_SINK_STEREO_ONLY 0 //duplicate samples for mono playback

/* FreeRTOS platform properties */
// #define HAVE_FREERTOS_INCLUDE_PREFIX 0 //Freertos headers and in 'freertos' folder 

/* POSIX platform properties */
// #define HAVE_POSIX_FILE_IO 0
// #define HAVE_POSIX_TIME 0
// #define LINK_KEY_PATH 0
// #define LE_DEVICE_DB_PATH 0

/* Chipset properties */
// #define HAVE_BCM_PCM2 0 //PCM2 is used and requires addition configuration
// #define HAVE_BCM_PCM_NBS_16KHZ 0 //NBS is up/downsampled use 16khz sample rate for NBS


/* ENABLE_* directives */
//#define ENABLE_CLASSIC 0 //linked when we link pico_btstack_classic in cmake
//#define ENABLE_BLE 1 //these are linked for us when we link the  pico_btstack_ble in cmake
//#ifndef ENABLE_BLE
//#error Please link to pico_btstack_ble
//#endif

// #define ENABLE_EHCILL 0 //not sure what this is
// #define ENABLE_H5 0
// #define ENABLE_LOG_DEBUG
// #define ENABLE_LOG_ERROR
// #define ENABLE_LOG_INFO
// #define ENABLE_LOG_BTSTACK_EVENTS
// #define ENABLE_SCO_OVER_HCI
// #define ENABLE_SCO_OVER_PCM
// #define ENABLE_HFP_WIDE_BAND_SPEECH
// #define ENABLE_HFP_AT_MESSAGES
// #define ENABLE_LE_PERIPHERAL 1
// #define ENABLE_LE_CENTRAL 0
// #define ENABLE_LE_SECURE_CONNECTIONS 0
// #define ENABLE_LE_SECURE_CONNECTIONS_DEBUG_KEY 0
// #define ENABLE_LE_PROACTIVE_AUTHENTICATION 0
// #define ENABLE_GATT_CLIENT_PAIRING
// #define ENABLE_GATT_CLIENT_SERVICE_CHANGED
