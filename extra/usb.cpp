
#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#include <libusb.h>

using namespace std;

int main() {

	libusb_device **devs; //pointer to pointer of device, used to retrieve a list of devices
	libusb_context *ctx = NULL; //a libusb session
	int r; //for return values

	r = libusb_init(&ctx); //initialize a library session
	printf("1\n");
	libusb_device_handle *handle;
	libusb_device *dev;
	
	handle = libusb_open_device_with_vid_pid(NULL, 1, 1);
	printf("2\n");

	dev = libusb_get_device(handle);
	printf("3\n");
	
	//while(r >= 0) {
		unsigned char buf[255];
		r = libusb_get_string_descriptor(handle, 0, 0, buf, sizeof(buf));
		printf("buf = %s", buf);
	//}
	
	libusb_free_device_list(devs, 1); //free the list, unref the devices in it
	libusb_exit(ctx); //close the session

	return 0;


}

