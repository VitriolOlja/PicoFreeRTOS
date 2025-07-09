#include "service_implementation.h"

#include "stdio.h"
#include "led_control.h"




// Create a callback registration object, and an att service handler object
att_service_handler_t 	service_handler ;
custom_service_t 		service_object ;
char characteristic_a[] = "LED Status and Control" ;

/* Binary Semaphore to handle waiting for ble RX.  */
//SemaphoreHandle_t  BLUETOOTH_READY; 

// Read callback (no client configuration handles on characteristics without Notify)
uint16_t custom_service_read_callback(hci_con_handle_t con_handle, uint16_t attribute_handle, uint16_t offset, uint8_t * buffer, uint16_t buffer_size){
	UNUSED(con_handle);
	printf("READ callback triggered: attr_handle = 0x%04X\n", attribute_handle);

	size_t free_heap = xPortGetFreeHeapSize();
  printf("Free heap memory: %u bytes\n", (unsigned)free_heap);
  UBaseType_t free_stack_words = uxTaskGetStackHighWaterMark(NULL);
  printf("Free stack space: %u words\n", (unsigned)free_stack_words);
	

	if (attribute_handle == service_object.characteristic_a_handle){
		printf("Reading characteristic_a_value: %s\n", service_object.characteristic_a_value);
		return att_read_callback_handle_blob(service_object.characteristic_a_value, strlen(service_object.characteristic_a_value), offset, buffer, buffer_size);
	}
	if (attribute_handle == service_object.characteristic_a_client_configuration_handle){
		printf("Reading client config value: %d\n", service_object.characteristic_a_client_configuration);
		return att_read_callback_handle_little_endian_16(service_object.characteristic_a_client_configuration, offset, buffer, buffer_size);
	}
	if (attribute_handle == service_object.characteristic_a_user_description_handle) {
		printf("Reading user description: %s\n", service_object.characteristic_a_user_description);
		return att_read_callback_handle_blob(service_object.characteristic_a_user_description, strlen(service_object.characteristic_a_user_description), offset, buffer, buffer_size);
	}
	
	printf("Read callback: Unknown attribute handle\n");
	return 0;
}


// #define xSemaphoreGive(sem) my_debug_xSemaphoreGive(sem, __FILE__, __LINE__)
// BaseType_t my_debug_xSemaphoreGive(SemaphoreHandle_t sem, const char* file, int line) {
//     printf("Semaphore give called from %s:%d\n", file, line);
//     printf("Giving semaphore at %p\n", sem);
//     return xSemaphoreGive(sem);
// }

// Write callback
int custom_service_write_callback(hci_con_handle_t con_handle, uint16_t attribute_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size){
	UNUSED(transaction_mode);
	UNUSED(offset);
	UNUSED(buffer_size);

	printf("WRITE callback triggered: attr_handle = 0x%04X, buffer = %.*s\n", attribute_handle, buffer_size, buffer);
	
	// Enable/disable notifications
	if (attribute_handle == service_object.characteristic_a_client_configuration_handle){
		service_object.characteristic_a_client_configuration = little_endian_read_16(buffer, 0);
		service_object.con_handle = con_handle;

		printf("Client configuration updated to: %d\n", service_object.characteristic_a_client_configuration);
	}
	
	// Write characteristic value
	if (attribute_handle == service_object.characteristic_a_handle) {
		custom_service_t * instance = &service_object ;
		
		memset(instance->characteristic_a_value, 0, sizeof(instance->characteristic_a_value));

    // Copy the exact number of bytes written, up to the size of the destination buffer
    size_t copy_size = buffer_size < sizeof(instance->characteristic_a_value) - 1
                         ? buffer_size
                         : sizeof(instance->characteristic_a_value) - 1;

    memcpy(instance->characteristic_a_value, buffer, copy_size);

    printf("Characteristic A updated to: %s\n", instance->characteristic_a_value);

		//buffer[buffer_size] = 0 ;
		//memset(service_object.characteristic_a_value, 0, strlen(service_object.characteristic_a_value)) ;
		//memcpy(service_object.characteristic_a_value, buffer, strlen(buffer)) ;

		//printf("Characteristic A updated to: %s\n", service_object.characteristic_a_value);


		// If client has enabled notifications, register a callback
		if (instance->characteristic_a_client_configuration) {

			printf("Notifications enabled; registering callback.\n");

			instance->callback_a.callback = &characteristic_a_callback ;
			instance->callback_a.context = (void*) instance ;
			att_server_register_can_send_now_callback(&instance->callback_a, instance->con_handle) ;
		}

    /*
    pt is defined in: 
    it refers to the current protothread (lightweight cooperative thread used in freertos systems)

    This is a wakeupcall to another thread that a characteristic changed value. 
    */
		// Alert the application of a bluetooth RX
		//printf("giving bluetoot semaphore!\n");
		//xSemaphoreGive(BLUETOOTH_READY);
		//printf("gave bluetooth semaphore\n");
	}
	
	return 0;
}

// Callback functions for ATT notifications on characteristics
void characteristic_a_callback(void * context){
	// Associate the void pointer input with our custom service object
	custom_service_t * instance = (custom_service_t *) context ;

	// Send a notification
	printf("Sending notification with value: %s\n", instance->characteristic_a_value);
	att_server_notify(instance->con_handle, instance->characteristic_a_handle, instance->characteristic_a_value, strlen(instance->characteristic_a_value)) ;
}



/////////////////////////////////////////////////////////////////////////////
////////////////////////////// USER API /////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

// Initialize our custom service handler
/**
 * custom_service_server_init
 * char * a_ptr: 
 */
void custom_service_server_init(char * a_ptr){
	printf("Initializing custom service server...\n");

	att_server_init(profile_data, NULL, NULL); 

	

	// Initialize the semaphore
  // TODO: implement some semaphore her with freertos maybe instead of protothreads.
	//PT_SEM_SAFE_INIT(&BLUETOOTH_READY, 0) ;

	// Pointer to our service object
	custom_service_t * instance = &service_object ;

	// Assign characteristic value
	instance->characteristic_a_value = a_ptr ;

	// Assign characteristic user description
	instance->characteristic_a_user_description = characteristic_a ;

	// Assign handle values (from generated gatt header)
  //#define ATT_CHARACTERISTIC_0xFFF1_01_VALUE_HANDLE 0x0009
	instance->characteristic_a_handle = ATT_CHARACTERISTIC_0000FF11_0000_1000_8000_00805F9B34FB_01_VALUE_HANDLE ;

  // not added unless explicitly added in the .gatt file. 
	instance->characteristic_a_client_configuration_handle = ATT_CHARACTERISTIC_0000FF11_0000_1000_8000_00805F9B34FB_01_CLIENT_CONFIGURATION_HANDLE ;
	instance->characteristic_a_user_description_handle = ATT_CHARACTERISTIC_0000FF11_0000_1000_8000_00805F9B34FB_01_USER_DESCRIPTION_HANDLE ;

	// Service start and end handles (modeled off heartrate example)
	service_handler.start_handle = 0 ;
	service_handler.end_handle = 0xFFFF ;
	service_handler.read_callback = &custom_service_read_callback ;
	service_handler.write_callback = &custom_service_write_callback ;

	// Register the service handler
	att_server_register_service_handler(&service_handler);

	printf("Custom service initialized. Characteristic A initial value: %s\n", a_ptr);
}

// Update Characteristic A value
void set_characteristic_a_value(int value){

	// Pointer to our service object
	custom_service_t * instance = &service_object ;

	// Update field value
	sprintf(instance->characteristic_a_value, "%d", value) ;

	printf("Characteristic A set to: %s\n", instance->characteristic_a_value);

	// Are notifications enabled? If so, register a callback
	if (instance->characteristic_a_client_configuration){
		printf("Notifications enabled; registering send-now callback.\n");

		instance->callback_a.callback = &characteristic_a_callback;
		instance->callback_a.context  = (void*) instance;
		att_server_register_can_send_now_callback(&instance->callback_a, instance->con_handle);;
	}
}