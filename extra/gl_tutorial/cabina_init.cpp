

#include <stdio.h>
#include <unistd.h>
#include <../libusb-1.0/libusb.h>

#include "utils.h"

#define CHIP_VENDOR_ID    0x20ff
#define CHIP_PRODUCT_ID   0x0100

float quaternion[4];

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

static float rotX = 0.0, rotY = 0.0;

libusb_device_handle *handle;	
int nEndpoint = 0x81 + intfNb;
int nTimeout  = 16;	//in milliseconds
int bytesRead = 0;
unsigned char output[65];

//----------------------------------------------------------------------------------
// 	OpenCV
void pni_read() {

	libusb_interrupt_transfer(handle, nEndpoint, output, 64, &bytesRead, nTimeout);
	processQuaternionData(output);

}

void processQuaternionData(unsigned char output[65]) {
	
	int i;

	int AccX, AccY, AccZ;
	int butL, butR;
	
	for(i = 6; i < 14; i += 2)
		quaternion[(i-6)/2] = (int) output[i] + 256*(int)output[i+1];

	for(i = 0; i < 4; i++)
		quaternion[i] = 3.0518e-005f*(quaternion[i]-32768);

	butL = output[14] & 1;
	butR = (output[14]>>1) & 1;

//	printf("q0 = %7.4f | q1 = %7.4f | q2 = %7.4f | q3 = %7.4f | butL = %d | butR = %d\n", quaternion[0], quaternion[1], quaternion[2], quaternion[3], butL, butR);
	
}

int is_usbdevblock( libusb_device *dev ) {

	struct libusb_device_descriptor desc;
	int r = libusb_get_device_descriptor( dev, &desc );
	DIE(r < 0, "libusb_get_device_descriptor error");

	if( (desc.idVendor == CHIP_VENDOR_ID) && (desc.idProduct == CHIP_PRODUCT_ID) )
		return 1;

	return 0;

}
//----------------------------------------------------------------------------------

void init(void) 
{
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel (GL_FLAT);
}

void display(void)
{
   glClear (GL_COLOR_BUFFER_BIT);
   glColor3f (1.0, 1.0, 1.0);

	pni_read();

   glPushMatrix();
	glRotatef ((GLfloat) rotX, (GLfloat) rotY, 1.0, 0.0);
   //glTranslatef (0.0, 0.0, (GLfloat) distZ);
   glutWireSphere(1.0, 20, 16);   /* draw sun */
   glPopMatrix();
   glutSwapBuffers();
}

void reshape (int w, int h)
{
   glViewport (0, 0, (GLsizei) w, (GLsizei) h); 
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();
   gluPerspective(60.0, (GLfloat) w/(GLfloat) h, 1.0, 20.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
	gluLookAt (0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

int main(int argc, char** argv)
{
	//----------------------------------------------------------------------------------
	//		OpenCV part
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
	}

	//----------------------------------------------------------------------------------

   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
   glutInitWindowSize (500, 500); 
   glutInitWindowPosition (100, 100);
   glutCreateWindow (argv[0]);
   init ();
   glutDisplayFunc(display); 
   glutReshapeFunc(reshape);
   glutKeyboardFunc(keyboard);
   glutMainLoop();
   return 0;
}

