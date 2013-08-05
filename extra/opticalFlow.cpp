
#include "highgui.h"

int main( int argc, char** argv ) {
	
	CvCapture* input_video = cvCaptureFromFile("nunta.avi");
	
	cvQueryFrame(input_video);

	CvSize frame_size;
	frame_size.height = cvGetCaptureProperty(input_video, CV_CAP_PROP_FRAME_HEIGHT);

	cvNamedWindow("Optical Flow", CV_WINDOW_NORMAL );
	
	cvSetCaptureProperty(input_video, CV_CAP_PROP_POS_FRAMES, N);
	IplImage *frame = cvQueryFrame(input_video);

	IplImage *frame1 = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);

	cvConvertImage(frame, frame1);


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

