
#include <stdio.h>
#include <unistd.h>
#include <libusb-1.0/libusb.h>

#include "utils.h"

#define CHIP_VENDOR_ID    0x20ff
#define CHIP_PRODUCT_ID   0x0100

//Check For Our VID & PID

void processRawData(unsigned char output[65]) {

	int MagX, MagY, MagZ;
	int AccX, AccY, AccZ;
	int GyroR, GyroP, GyroY;
	int butL, butR;

	char charAux[3];

	//--------------------------------------------------------------------------
	sprintf(charAux, "%02X%02X", output[1], output[0]);
	sscanf(charAux, "%04x", &MagX);
	sprintf(charAux, "%02X%02X", output[3], output[2]);
	sscanf(charAux, "%04x", &MagY);
	sprintf(charAux, "%02X%02X", output[5], output[4]);
	sscanf(charAux, "%04x", &MagZ);
	//--------------------------------------------------------------------------

	//--------------------------------------------------------------------------
	sprintf(charAux, "%02X%02X", output[7], output[8]);
	sscanf(charAux, "%04x", &AccX);
	sprintf(charAux, "%02X%02X", output[9], output[8]);
	sscanf(charAux, "%04x", &AccY);
	sprintf(charAux, "%02X%02X", output[11], output[10]);
	sscanf(charAux, "%04x", &AccZ);
	//--------------------------------------------------------------------------

	//--------------------------------------------------------------------------
	sprintf(charAux, "%02X%02X", output[13], output[12]);
	sscanf(charAux, "%04x", &GyroR);
	sprintf(charAux, "%02X%02X", output[15], output[14]);
	sscanf(charAux, "%04x", &GyroP);
	sprintf(charAux, "%02X%02X", output[17], output[16]);
	sscanf(charAux, "%04x", &GyroY);
	//--------------------------------------------------------------------------

	//--------------------------------------------------------------------------
	sprintf(charAux, "%02X%02X", output[18], output[19]);
	sscanf(charAux, "%04x", &GyroR);
	butL = output[18] & 1;
	butR = (output[18]>>1) & 1;
	//--------------------------------------------------------------------------

	printf("Mag(X:Y:Z)  : %d:%d:%d \n", MagX-32768, MagY-32768, MagZ-32768);
	printf("Acc(X:Y:Z)  : %d:%d:%d \n", AccX-32768, AccY-32768, AccZ-32768);
	printf("Gyro(R:P:Y) : %d:%d:%d \n", GyroR-32768, GyroP-32768, GyroY-32768);
	printf("butL = %d  | butR = %d \n", butL, butR);

}


void processQuaternionData(unsigned char output[65]) {
	
	int i;

	int AccX, AccY, AccZ;
	float quaternion[4];
	int butL, butR;
	
	for(i = 6; i < 14; i += 2)
		quaternion[(i-6)/2] = (int) output[i] + 256*(int)output[i+1];

	for(i = 0; i < 4; i++)
		quaternion[i] = 3.0518e-005f*(quaternion[i]-32768);

	butL = output[14] & 1;
	butR = (output[14]>>1) & 1;

	printf("q0 = %7.4f | q1 = %7.4f | q2 = %7.4f | q3 = %7.4f | butL = %d | butR = %d\n", quaternion[0], quaternion[1], quaternion[2], quaternion[3], butL, butR);
	
}


int is_usbdevblock( libusb_device *dev ) {

	struct libusb_device_descriptor desc;
	int r = libusb_get_device_descriptor( dev, &desc );
	DIE(r < 0, "libusb_get_device_descriptor error");

	if( (desc.idVendor == CHIP_VENDOR_ID) && (desc.idProduct == CHIP_PRODUCT_ID) )
		return 1;

	return 0;

}


int main( int argc, char **argv) {

	// discover devices
	libusb_device **list;
	libusb_device *found = NULL;
	libusb_context *ctx = NULL;
	int attached = 0;

	libusb_init(&ctx);
	libusb_set_debug(ctx, 3); 
	ssize_t i = 0;
	int err = 0;
	int intfNb = 1; // 0 - Raw data / 1 - Qaternions
	ssize_t cnt = libusb_get_device_list(ctx, &list);
	DIE(cnt < 0, "no usb devices found\n" );

	// find our device
	for(i = 0; i < cnt; i++){
		libusb_device *device = list[i];
		if( is_usbdevblock(device) ){
			found = device;
			break;
		}
	}

	DIE(found == 0, "Device not connected !");
	if(found) {
		printf("Device connected ! \n");
		
		libusb_device_handle *handle;	
		err = libusb_open(found, &handle);
		DIE(err < 0, "Unable to open usb device\n");

		if(libusb_kernel_driver_active(handle, intfNb)) {
			printf("Device busy...detaching...\n");
			err = libusb_detach_kernel_driver(handle, intfNb);
			DIE(err < 0, "libusb_detach_kernel_driver error");
			attached = 1;
		}
		else
			printf("Device free from kernel\n");

		err = libusb_claim_interface(handle, intfNb);
		if(err) {
			printf("Failed to claim interface %d \n", intfNb);
			
			switch(err) {
				case LIBUSB_ERROR_NOT_FOUND:  printf("not found \n");  break;
				case LIBUSB_ERROR_BUSY:       printf("busy \n");       break;
				case LIBUSB_ERROR_NO_DEVICE:  printf("no device \n");  break;
				default:                      printf("other \n");      break;
			}

			return 0;
		}

		int nEndpoint = 0x81 + intfNb;
		int nTimeout  = 16;	//in milliseconds
		int bytesRead = 0;
		unsigned char output[65];

		while(1) {

			libusb_interrupt_transfer(handle, nEndpoint, output, 64, &bytesRead, nTimeout);
			//printf("Read %d bytes from endpoint address 0x%X\n", bytesRead, nEndpoint );

			if(intfNb == 0)
				processRawData(output);
			else
				processQuaternionData(output);

			usleep(10000);
			//sleep(1);
			//printf("--------------------------------------------------------\n");
		}

		//if we detached kernel driver, reattach.
		if(attached == 1) {
			libusb_attach_kernel_driver(handle, 0);
		}

		libusb_close(handle);

	} 

	libusb_free_device_list(list, 1);
	libusb_exit( ctx );

}





