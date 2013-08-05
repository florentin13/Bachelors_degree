
//--------------------------------
// Standard includes
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "utils.h"

#define CHIP_VENDOR_ID    0x20ff
#define CHIP_PRODUCT_ID   0x0100

#define KEY_ESCAPE 27

//--------------------------------
// OpenCV video stream
#include <cv.h>
#include <highgui.h>

//--------------------------------
// OpenGL/Glut includes
#include <GL/glut.h>

//--------------------------------
//		libusb
#include <unistd.h>
#include <../libusb-1.0/libusb.h>
//--------------------------------


libusb_device_handle *handle;	
int intfNb = 1; // 0 - Raw data / 1 - Qaternions
int nEndpoint = 0x81 + intfNb;
int nTimeout  = 16;	//in milliseconds
int bytesRead = 0;

static float quaternion[4];

//----------------------------------------------------------------------------------
// OpenCV Video stream functions
CvCapture* g_Capture;
GLint g_hWindow;

// Frame size
int frame_width  = 640;
int frame_height = 480;
//----------------------------------------------------------------------------------
// 	OpenCV
void processQuaternionData(unsigned char output[65]) {
	int i;
	for(i = 6; i < 14; i += 2)
		quaternion[(i-6)/2] = (int) output[i] + 256*(int)output[i+1];
	for(i = 0; i < 4; i++)
		quaternion[i] = 3.0518e-005f*(quaternion[i]-32768);
}

int is_usbdevblock( libusb_device *dev ) {
	struct libusb_device_descriptor desc;
	int r = libusb_get_device_descriptor( dev, &desc );
	DIE(r < 0, "libusb_get_device_descriptor error");
	if( (desc.idVendor == CHIP_VENDOR_ID) && (desc.idProduct == CHIP_PRODUCT_ID) )
		return 1;
	return 0;
}

void pni_read() {
	unsigned char output[65];
	libusb_interrupt_transfer(handle, nEndpoint, output, 64, &bytesRead, nTimeout);
	processQuaternionData(output);

	int i;

	// Capture next frame, this will almost always be the limiting factor in the
	// framerate, my webcam only gets ~15 fps
	IplImage * image = cvQueryFrame(g_Capture);

	cvFlip(image, NULL, 0);

	// Image is memory aligned which means we there may be extra space at the end
	// of each row. gluBuild2DMipmaps needs contiguous data, so we buffer it here
	char *buffer = new char[image->width*image->height*image->nChannels];
	int step     = image->widthStep;
	int height   = image->height;
	int width    = image->width;
	int channels = image->nChannels;
	char * data  = (char *)image->imageData;
	// memcpy version below seems slightly faster
	for(i = 0; i < height; i++)
		memcpy(&buffer[i*width*channels],&(data[i*step]),width*channels);

	// Create Texture
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGB,
		image->width,
		image->height,
		0,
		GL_BGR,
		GL_UNSIGNED_BYTE,
		buffer);

	// Clean up buffer
	delete[] buffer;

	// Update display
	glutPostRedisplay();

}
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//		OpenGL functions
void init(void) {
   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel (GL_FLAT);
}

void display(void) {


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	// These are necessary if using glTexImage2D instead of gluBuild2DMipmaps
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	// Set Projection Matrix
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, frame_width, frame_height, 0);

	// Switch to Model View Matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Draw a textured quad
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(100, 100, 1);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(frame_width-100, 100, -1);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(frame_width-100, frame_height-100, -1);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(100, frame_height-100, 1);
	glEnd();


   glColor3f (1.0, 1.0, 1.0);
   glPushMatrix();
	glRotatef ((GLfloat) quaternion[0]*100, (GLfloat) quaternion[1]*100, (GLfloat) quaternion[2]*100, (GLfloat) quaternion[3]*100);
   glutWireSphere(1.0, 20, 16);   /* draw sun */
   glPopMatrix();

	glFlush();
   glutSwapBuffers();
}

void reshape (int w, int h) {
   glViewport (0, 0, (GLsizei) w, (GLsizei) h); 
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();
   gluPerspective(60.0, (GLfloat) w/(GLfloat) h, 1.0, 20.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
	gluLookAt (0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

void keyboard(unsigned char ch, int x, int y){
	switch(ch){
		case KEY_ESCAPE:
			cvReleaseCapture(&g_Capture);
			glutDestroyWindow(g_hWindow);
			exit(0);
			break;
	}
}
//----------------------------------------------------------------------------------

int main(int argc, char** argv) {
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
	// Create GLUT Window
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(frame_width, frame_height);
   glutInitWindowPosition (100, 100);

	g_hWindow = glutCreateWindow("Video Texture");

	// Create OpenCV camera capture
	// If multiple cameras are installed, this takes "first" one
	g_Capture = cvCaptureFromCAM(0);
	assert(g_Capture);
	// capture properties
	frame_height = (int)cvGetCaptureProperty(g_Capture, CV_CAP_PROP_FRAME_HEIGHT);
	frame_width  = (int)cvGetCaptureProperty(g_Capture, CV_CAP_PROP_FRAME_WIDTH);
	
	init ();
   glutDisplayFunc(display); 
   glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard); // Only for Esc key
	glutIdleFunc(pni_read);
   glutMainLoop();
	
	libusb_close(handle);
	libusb_free_device_list(list, 1);
	libusb_exit(ctx);

   return 0;
}

