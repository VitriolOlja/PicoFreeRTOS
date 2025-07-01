#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "btstack.h"
#include "btstack_run_loop.h"

#include "lwipopts.h"
//
#include "FreeRTOS.h"
#include "task.h"
//
#include "pico_error_str.h"
#include "led_control.h"
#include "gap_config.h"

static btstack_packet_callback_registration_t hci_event_callback_registration;

void BleServer_SetupBLE(void)
{
    // what is l2cap and what is hci layer.

    /**
     * l2cap : Logical link control and adaptation protocol
     * Host controller interface HCI
     *
     * l2cap - multiplexing data between different higher layer protocols
     *  - segmentation and reassembly of packets.
     *  - providing one way transmission management of multicast data to a group of bt devices.
     * - QOS management.
     *
     */
    l2cap_init(); /* Set up L2CAP and register L2CAP with HCI layer */

    sm_init(); /* setup security manager */

    /**
     * profile_data : byte array of att/gatt database generated from the gatt file by the cmake
     *
     * att_read_callback :
     *       called whenever a pear reads on characteristic
     *
     * att_write_callback : ble_server.c
     *       called on writes (when a client enables notifications or writes to a characteristic)
     */
    // att_server_init(profile_data, att_read_callback, att_write_callback); /* setup attribute callbacks */
    att_server_init(profile_data, NULL, NULL); /* setup attribute callbacks */

    /* inform about BTstack state */
    hci_event_callback_registration.callback = &packet_handler; /* setup callback for events */
    hci_add_event_handler(&hci_event_callback_registration);    /* register callback handler */

    ///* register for ATT event */
    att_server_register_packet_handler(packet_handler); /* register packet handler */


    printf("Before hci_power_control\n");
    
    int success = hci_power_control(HCI_POWER_ON); /* turn BLE on called to start processing.  */

    printf("return of hci_power_control: %d", success);
}

static void serverTask(void *pv)
{
    /* initialize CYW43 driver architecture (will enable BT if/because CYW43_ENABLE_BLUETOOTH == 1) */
    if (cyw43_arch_init())
    {
        printf("failed to initialize cyw43_arch");
        for (;;)
        {
        }
    }
    BleServer_SetupBLE();
    printf("Ble setup complete.");

    btstack_run_loop_execute(); /* does not return */
    printf("Error exited btstack run loop.");
    vTaskDelete(NULL);
}

void BleServer_Init(void)
{
    if (xTaskCreate(
            serverTask,                 /* pointer to the task */
            "BLEserver",                /* task name for kernel awareness debugging */
            2400 / sizeof(StackType_t), /* task stack size */
            (void *)NULL,               /* optional task startup argument */
            tskIDLE_PRIORITY + 2,       /* initial priority */
            (TaskHandle_t *)NULL        /* optional task handle to create */
            ) != pdPASS)
    {
        printf("failed creating task");
        for (;;)
        {
        } /* error! probably out of memory */
    }
}

int main()
{
    stdio_init_all();
    sleep_ms(1000);

    // countdown to connect to have time to connect serial output
    for (int i = 5; i >= 0; i--)
    {

        printf("Countdown! %d \n", i); // print "Countdown! i" every second. until we attempt to connect
        sleep_ms(1000);
    }

    BleServer_Init();

    // xTaskCreate(bleServerTask, "BLE Server", 4096, NULL, tskIDLE_PRIORITY + 1, NULL);

    vTaskStartScheduler();
}