#include "lwip/apps/httpd.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwipopts.h"
//
#include "FreeRTOS.h"
#include "task.h"
//
#include "pico_error_str.h"


void MainTask(__unused void *params) {
    printf("SSID: %s PASSWORD: %s \n", WIFI_SSID, WIFI_PASSWORD);

    int arch_init_error = cyw43_arch_init(); //PANIC?

    //int arch_init_error = cyw43_arch_init_with_sys_freertos()

    if (arch_init_error != 0) {
        printf("Failed to init with sys_freertos: %s\n", pico_error_str(arch_init_error));
        return;
    }

    printf("Successfully initialized cyw43");

    cyw43_arch_enable_sta_mode();

    printf("Successfully enabled sta mode");

    // Connect to the WiFI network - loop until connected
    int e;
    do {
        printf("Attempting to connect to %s...\n", WIFI_SSID);
        e = cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000);
        if (e != PICO_OK) {
            printf("Failed to connect to %s: %s\n", WIFI_SSID, pico_error_str(e));
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    } while (e != PICO_OK);
    // Print a success message once connected
    printf("Connected! \n");

    // Initialise web server
    cyw43_arch_lwip_begin();
    httpd_init();
    cyw43_arch_lwip_end();
    printf("Http server initialised\n");

    printf("\nListening at %s\n", ip4addr_ntoa(netif_ip4_addr(netif_list)));
    vTaskDelete(NULL);
}

int main() {
    stdio_init_all();
    sleep_ms(1000);

    //endless loop (does prints so this works. )
    //for (;;) {
    //
    //    printf("inside \n");    //print inside every second.
    //    sleep_ms(1000);
    //}

    //countdown to connect to have time to connect serial output
    for (int i = 5; i >= 0; i--) {
    
        printf("Countdown! %d \n", i);    //print "Countdown! i" every second. until we attempt to connect
        sleep_ms(1000);
    }

    
    xTaskCreate(MainTask, "TestMainThread", 4096, NULL, tskIDLE_PRIORITY + 1, NULL);
    vTaskStartScheduler();
}