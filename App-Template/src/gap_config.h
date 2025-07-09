#ifndef GAP_CONFIG_H
#define GAP_CONFIG_H

/**
 * Copyright (c) 2023 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "btstack.h"
#include "advertising.h"
#include "btstack_defines.h"
#include "btstack_event.h"

// Flag for general discoverability
#define APP_AD_FLAGS 0x06

// GAP data packet (must not exceed 32 bytes)
static uint8_t adv_data[] = {
    // Flags general discoverable
    0x02,
    BLUETOOTH_DATA_TYPE_FLAGS,
    APP_AD_FLAGS,
    // Name
    0x08,
    BLUETOOTH_DATA_TYPE_COMPLETE_LOCAL_NAME,
    'P',
    'i',
    'c',
    'o',
    'L',
    'E',
    'D',
    // Custom Service UUID
    0x03,
    BLUETOOTH_DATA_TYPE_COMPLETE_LIST_OF_16_BIT_SERVICE_CLASS_UUIDS,
    0x10,
    0xFF,
};

static const uint8_t adv_data_len = sizeof(adv_data);

// HCI packet handler
void packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size)
{
  //Previous error was too little stack memory allocated for this task. 
  // good debug check is to check how much memory you have. 
  // It should trigger a callback in freertos, yet it didn't. 

  //size_t free_heap = xPortGetFreeHeapSize();
  //printf("Free heap memory: %u bytes\n", (unsigned)free_heap);

  //UBaseType_t free_stack_words = uxTaskGetStackHighWaterMark(NULL);
  //printf("Free stack space: %u words\n", (unsigned)free_stack_words);

  if (packet == NULL) {
    printf("packet is NULL!\n");
    return;
  }

  UNUSED(size);         
  UNUSED(channel);      
  bd_addr_t local_addr; 
  
  if (packet_type != HCI_EVENT_PACKET){
    return;
  }
  // Retrieve event type from HCI 
  
  //printf("packet size = %d\n", size);
  //
  //if (packet && size > 0) {
  //  printf("Packet data bytes: ");
  //  for (int i = 0; i < size; i++) {
  //      printf("%02X ", packet[i]);
  //  }
  //  printf("\n");
  //} else {
  //    printf("Invalid packet or size\n");
  //}

  //printf("packet pointer = %p\n", (void*)packet);
  uint8_t event_type = hci_event_packet_get_type(packet); 

  //printf("with event type: %d \n", event_type);

  // Switch on event type . . .
  switch (event_type)
  {
  // Setup GAP advertisement
  case BTSTACK_EVENT_STATE:
    uint8_t state = btstack_event_state_get_state(packet);
    printf("BTSTACK_EVENT_STATE state = %d\n", state);
    if (state != HCI_STATE_WORKING)
    {
      return;
    }
    printf("valid bstack event state.");
    gap_local_bd_addr(local_addr);
    printf("BTstack up and running on %s.\n", bd_addr_to_str(local_addr));
    // setup advertisements
    // https://stackoverflow.com/questions/76337421/why-does-ble-advertising-interval-need-to-be-multiple-of-0-625msec
    // this seems to be 800 * 0.625ms which is 500 ms, so advertise for half a second.
    // it should also be in the range of 20 ms to 10.24 seconds. so a value between 32 and 16384 is fine.
    uint16_t adv_int_min = 800;
    uint16_t adv_int_max = 800;

    uint8_t adv_type = ADV_TYPE_IND;

    bd_addr_t null_addr; // mac address

    memset(null_addr, 0, 6);
    gap_advertisements_set_params(adv_int_min, adv_int_max, adv_type, 0, null_addr, 0x07, 0x00);
    assert(adv_data_len <= 31); // ble limitation
    gap_advertisements_set_data(adv_data_len, (uint8_t *)adv_data);
    gap_advertisements_enable(1);

    break;
  // Disconnected from a client
  case HCI_EVENT_DISCONNECTION_COMPLETE:
    //printf("Disconnections completed\n");
    break;
  // Ready to send ATT
  case ATT_EVENT_CAN_SEND_NOW:
    //printf("ATT can send now.\n");
    break;
  case HCI_EVENT_COMMAND_COMPLETE:
  {
    /**
     * Each command is assigned a 2 byte Opcode used to uniquely identify different types of commands.
     * The Opcode parameter is divided into two fields, 
     * called the Opcode Group Field (OGF) and Opcode Command Field (OCF). 
     * The OGF occupies the upper 6 bits of the Opcode, while the OCF occupies the remaining 10 bits. 
     * OGF
     * Link Control COmmands 0x01
     * Link Policy commands, the OGF is defined as 0x02.
     * For the HCI Control and Baseband commands, the OGF is defined as 0x03.
     * Informational Parameters commands, the OGF is defined as 0x04.
     * status parameters commands, the OGF is defined as 0x05.
     * Testing commands, the OGF is defined as 0x06.
     * LE Controller commands, the OGF code is defined as 0x08.
     */
    uint16_t opcode = hci_event_command_complete_get_command_opcode(packet);

    const uint8_t *return_params = hci_event_command_complete_get_return_parameters(packet);
    uint8_t status = return_params[0];
    //printf("Command Complete: opcode 0x%04x, status %d\n", opcode, status);
    break;
  }
  case HCI_EVENT_COMMAND_STATUS:
  {
    uint8_t status = hci_event_command_status_get_status(packet);
    uint16_t opcode = hci_event_command_status_get_command_opcode(packet);
    //printf("Command Status: opcode 0x%04x, status %d\n", opcode, status);
    break;
  }
  case HCI_EVENT_TRANSPORT_PACKET_SENT:
  {
    //printf("transport packet sent.\n");
    break;
  }
  default:
    //printf("Unhandled event type %d, raw data:", event_type);
    //for (int i = 0; i < size; i++)
    //{
    //  printf(" %02x", packet[i]);
    //}
    //printf("\n");
    break;
  }
}

#endif /* GAP_CONFIG_H */