#include ble_server.h

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
//
#include "FreeRTOS.h"
#include "task.h"
//
#include "pico_error_str.h"


#define HEARTBEAT_PERIOD_MS 1000
 
static btstack_packet_callback_registration_t hci_event_callback_registration;
 
static void callbackSendData(void) { /* callback is called every second */
  static uint32_t counter = 0;
  counter++;
  if ((counter%10) == 0) { /* Update the temperature every 10s */
    poll_temperature();
    if (le_notification_enabled) {
      att_server_request_can_send_now_event(con_handle);
    }
  }
}
 
static void callbackToggleLED(void) { /* called every second */
  /* Invert the led */
  static int led_on = true;
  led_on = !led_on;
  cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_on);
}
 
#if PL_CONFIG_USE_WIFI
  static void heartbeat_handler(async_context_t *context, async_at_time_worker_t *worker);
  static async_at_time_worker_t heartbeat_worker = { .do_work = heartbeat_handler };
 
  static void heartbeat_handler(async_context_t *context, async_at_time_worker_t *worker) {
    callbackSendData();
    callbackToggleLED();
    /* restart timer */
    async_context_add_at_time_worker_in_ms(context, &heartbeat_worker, HEARTBEAT_PERIOD_MS);
  }
#else
  static btstack_timer_source_t heartbeat;
 
  static void heartbeat_handler(struct btstack_timer_source *ts) {
    callbackSendData();
    callbackToggleLED();
    /* restart timer */
    btstack_run_loop_set_timer(ts, HEARTBEAT_PERIOD_MS);
    btstack_run_loop_add_timer(ts);
  }
#endif
 
void BleServer_SetupBLE(void) {
  l2cap_init(); /* Set up L2CAP and register L2CAP with HCI layer */
  sm_init(); /* setup security manager */
  att_server_init(profile_data, att_read_callback, att_write_callback); /* setup attribute callbacks */
 
  /* inform about BTstack state */
  hci_event_callback_registration.callback = &packet_handler; /* setup callback for events */
  hci_add_event_handler(&hci_event_callback_registration); /* register callback handler */
 
  /* register for ATT event */
  att_server_register_packet_handler(packet_handler); /* register packet handler */
 
  /* setup timer */
#if PL_CONFIG_USE_WIFI /* use cyw43 timer */
  async_context_add_at_time_worker_in_ms(cyw43_arch_async_context(), &heartbeat_worker, HEARTBEAT_PERIOD_MS);
#else /* use BTStack timer */
  heartbeat.process = &heartbeat_handler;
  btstack_run_loop_set_timer(&heartbeat, HEARTBEAT_PERIOD_MS);
  btstack_run_loop_add_timer(&heartbeat);
#endif
  hci_power_control(HCI_POWER_ON); /* turn BLE on */
}
 
static void SetupTemperatureSensor(void) {
  /* Initialize adc for the temperature sensor */
  adc_init();
  adc_select_input(ADC_CHANNEL_TEMPSENSOR);
  adc_set_temp_sensor_enabled(true);
}
 
#if !PL_CONFIG_USE_WIFI /* if using WiFi, will do the BLE stuff from the WiFi task */
static void serverTask(void *pv) {
  /* initialize CYW43 driver architecture (will enable BT if/because CYW43_ENABLE_BLUETOOTH == 1) */
  if (cyw43_arch_init()) {
    McuLog_fatal("failed to initialize cyw43_arch");
    for(;;) {}
  }
  BleServer_SetupBLE();
  for(;;) {
    btstack_run_loop_execute(); /* does not return */
  }
}
#endif /* !PL_CONFIG_USE_WIFI */
 
void BleServer_Init(void) {
  SetupTemperatureSensor();
#if !PL_CONFIG_USE_WIFI /* if using WiFi, will do the BLE stuff from the WiFi task */
  if (xTaskCreate(
      serverTask,  /* pointer to the task */
      "BLEserver", /* task name for kernel awareness debugging */
      1200/sizeof(StackType_t), /* task stack size */
      (void*)NULL, /* optional task startup argument */
      tskIDLE_PRIORITY+2,  /* initial priority */
      (TaskHandle_t*)NULL /* optional task handle to create */
    ) != pdPASS)
  {
    McuLog_fatal("failed creating task");
    for(;;){} /* error! probably out of memory */
  }
#endif
}