/*

	V. Hunter Adams (vha3@cornell.edu)
	Custom GATT Service implementation
	Modeled off examples from BTstack

*/
#ifndef SERVICE_IMPLEMENTATION_H
#define SERVICE_IMPLEMENTATION_H


#include "btstack_defines.h"
#include "ble/att_db.h"
#include "ble/att_server.h"
#include "btstack_util.h"
#include "bluetooth_gatt.h"
#include "btstack_debug.h"


//FreeRTOS
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* Binary Semaphore to handle waiting for ble RX.  */
//extern SemaphoreHandle_t BLUETOOTH_READY; 

// Protothreads semaphore
// extern struct pt_sem BLUETOOTH_READY;

// Create a struct for managing this service
typedef struct {
	
	// Connection handle for service
	hci_con_handle_t con_handle ;
	
	// Characteristic A information
	char * 		characteristic_a_value ;
	uint16_t 	characteristic_a_client_configuration ;
	char * 		characteristic_a_user_description ;
	
	// Characteristic A handles
	uint16_t  	characteristic_a_handle ;
	uint16_t 	characteristic_a_client_configuration_handle ;
	uint16_t 	characteristic_a_user_description_handle ;
	
	// Callback functions
	btstack_context_callback_registration_t callback_a ;
	
} custom_service_t ;


extern att_service_handler_t service_handler;
extern custom_service_t service_object;

// Characteristic user descriptions (appear in LightBlue app)
extern char characteristic_a[];



uint16_t custom_service_read_callback(hci_con_handle_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size);
int custom_service_write_callback(hci_con_handle_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size);
void characteristic_a_callback(void * context);


/////////////////////////////////////////////////////////////////////////////
////////////////////////////// USER API /////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

void custom_service_server_init(char * a_ptr);
void set_characteristic_a_value(int value);



#endif /* SERVICE_IMPLEMENTATION_H */