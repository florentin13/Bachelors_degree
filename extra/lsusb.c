
#define PNI_IDVENDOR    0x20ff
#define PNI_IDPRODUCT   0x0100

#include <stdio.h>
#include <sys/types.h>

#include <libusb.h>


int is_PNI(libusb_device *dev) {
	struct libusb_device_descriptor desc;
	int rc = libusb_get_device_descriptor(dev, &desc);

	if(desc.idVendor == PNI_IDVENDOR && desc.idProduct == PNI_IDPRODUCT) {
		printf("%04x:%04x (bus %d, device %d)\n", 
				desc.idVendor, desc.idProduct,
				libusb_get_bus_number(dev), libusb_get_device_address(dev));
		return 1;
	}
	return 0;
}


void print_devs(libusb_device **devs, ssize_t cnt) {

	libusb_device_handle *handle;
	libusb_device *found = NULL;
	libusb_device *dev;

	int rc;
	int i = 0;
	
	// find our device
	for(i = 0; i < cnt; i++){
		libusb_device *device = devs[i];
		if( is_PNI(device) ){
			found = device;
			break;
		}
	}
		
	rc = libusb_open(found, &handle);
	if(rc < 0) {
		fprintf(stderr, "Failed to get device descriptor\n");
		return;
	}

	printf("Success! \n");

}

int main(void) {

      libusb_device **devs;
      int rc;
      ssize_t cnt;
		libusb_context *ctx = NULL;

      rc = libusb_init(&ctx);
      if (rc < 0)
            return rc;
		libusb_set_debug(ctx, 3);

      cnt = libusb_get_device_list(ctx, &devs);
      if (cnt < 0)
            return (int) cnt;

      print_devs(devs, cnt);

      libusb_free_device_list(devs, 1);
      libusb_exit(ctx);

      return 0;

}



