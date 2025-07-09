#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/gpio.h"
#include "btstack.h"
#include "btstack_run_loop.h"

#include "lwipopts.h"
//
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
//
#include "pico_error_str.h"
#include "gap_config.h"
#include "service_implementation.h"

static btstack_packet_callback_registration_t hci_event_callback_registration;

/* for storing the value for the a characteristic */
char characteristic_a_value[100] = { 0x30, 0x00 }; // '0' + '/0'
uint16_t characteristic_a_length;
#define LED_PIN 15

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
     */
    // att_server_init(profile_data, att_read_callback, att_write_callback);

    custom_service_server_init(characteristic_a_value);

    att_server_register_service_handler(&service_handler);

    /* inform about BTstack state */
    hci_event_callback_registration.callback = &packet_handler; /* setup callback for events */
    hci_add_event_handler(&hci_event_callback_registration);    /* register callback handler */

    ///* register for ATT event */
    att_server_register_packet_handler(packet_handler); /* register packet handler */


    //printf("Before hci_power_control\n");
    
    int success = hci_power_control(HCI_POWER_ON); /* turn BLE on called to start processing.  */

    //printf("return of hci_power_control: %d", success);
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

void ledTask(void *pv)
{
    //if (BLUETOOTH_READY == NULL) {
    //    printf("Semaphore not initialized! Task exiting.\n");
    //    vTaskDelete(NULL);
    //}

    char last_value = '\0';
    printf("ledTask waiting for BLE signal...\n");

    // Wait until BLE signals ready
    while (1) {
        // wait indefinitely for the semaphore to become available.
        //printf("ledTask: taking semaphore at %p\n", BLUETOOTH_READY);
        //if (xSemaphoreTake(BLUETOOTH_READY, portMAX_DELAY) == pdTRUE) {
        //    // Assuming characteristic_a_value is a null-terminated string
        //    if (characteristic_a_value != NULL && characteristic_a_value[0] != '\0') {
        //        if (characteristic_a_value[0] == '0') {
        //            // Turn LED OFF
        //            //led_off();
        //            printf("LED OFF\n");
        //        } else {
        //            // Turn LED ON
        //            //led_on();
        //            printf("LED ON\n");
        //        }
        //    } else {
        //        printf("characteristic_a_value is empty or NULL\n");
        //    }
        //}

        if (characteristic_a_value != NULL && characteristic_a_value[0] != '\0') {
            if (characteristic_a_value[0] != last_value) {
                last_value = characteristic_a_value[0];

                if (last_value == '0') {
                    gpio_put(LED_PIN, 0);
                    printf("LED OFF\n");
                } else {
                    gpio_put(LED_PIN, 1);
                    printf("LED ON\n");
                }
            }
        } else {
            printf("characteristic_a_value is empty or NULL\n");
        }

        vTaskDelay(pdMS_TO_TICKS(1000)); // wait 1 second
    }
}

// void overflow_task(void *pvParameters) {
//     printf("overflow!!!!");
//     volatile char dummy[160]; // Force stack usage
//     dummy[0] = 'x'; // Prevent optimizing out
//     for (;;) {}
// }




int main()
{
    stdio_init_all();
    sleep_ms(1000);

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);


    // countdown to connect to have time to connect serial output
    for (int i = 5; i >= 0; i--)
    {
        printf("Countdown! %d \n", i); // print "Countdown! i" every second. until we attempt to connect
        sleep_ms(1000);
    }

    //BLUETOOTH_READY = xSemaphoreCreateBinary();
    //if (BLUETOOTH_READY == NULL) {
    //printf("Failed to create BLUETOOTH_READY semaphore\n");
    //} else {
    //    printf("Created BLUETOOTH_READY: %p\n", BLUETOOTH_READY);
    //}

    //for now lets leave trying to crash and trigger the 
    //xTaskCreate(overflow_task, "overflow", 128, NULL, tskIDLE_PRIORITY + 4, NULL);


    if (xTaskCreate(
            ledTask,                 /* pointer to the task */
            "ledTask",                /* task name for kernel awareness debugging */
            2400 / sizeof(StackType_t), /* task stack size */
            (void *)NULL,               /* optional task startup argument */
            tskIDLE_PRIORITY + 3,       /* initial priority */
            (TaskHandle_t *)NULL        /* optional task handle to create */
            ) != pdPASS)
    {
        printf("failed creating led task");
        for (;;)
        {
        } /* error! probably out of memory */
    }

    BleServer_Init();


    vTaskStartScheduler();
}