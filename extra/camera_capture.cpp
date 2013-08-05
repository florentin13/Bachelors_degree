
#include "highgui.h"

int main( int argc, char** argv ) {
	
	cvNamedWindow("Cockpit stream", CV_WINDOW_NORMAL );
	cvSetWindowProperty("Cockpit stream", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);


	CvCapture* capture = cvCreateCameraCapture(0) ;
	IplImage* frame;

	while(1) {
		frame = cvQueryFrame( capture );
		if( !frame ) 
			break;

		// draw a green line of width 1 between (100,100) and (200,200)
		cvLine(frame, cvPoint(-1000, 300), cvPoint(10000,300), cvScalar(0,255,0), 30);

		cvShowImage( "Cockpit stream", frame );

		char c = cvWaitKey(33);
		if( c == 27 ) 
			break;
	}

	cvReleaseCapture( &capture );
	cvDestroyWindow( "Cockpit stream" );

}

