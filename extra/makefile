



run:
	./camera_capture

build:
	g++ camera_capture.cpp -o camera_capture `pkg-config --cflags --libs opencv`

build2:
	g++ optical_flow_demo.cpp -o opflow `pkg-config --cflags --libs opencv`

clean:
	rm -f camera_capture usb glCam

build_usb:
	g++ usb.cpp -o usb `pkg-config --libs --cflags libusb-1.0`


usb_ls:
	g++ lsusb.c -o lsusb `pkg-config --libs --cflags libusb-1.0`

usbtest:
	g++ usb-test.c -o usb-test `pkg-config --libs --cflags libusb-1.0`

pni:
	g++ pni.c -o expni `pkg-config --libs --cflags libusb-1.0`

gl:
	g++ -o gl gl.cpp -lX11 -lGL -lGLU

all: clean_glCam build_glCam run_glCam

clean_glCam:
	rm -f glCam

build_glCam:
	g++ glCam.cpp -o glCam -lX11 -lGL -lGLU -lc -lm -lglut -lGLU `pkg-config --cflags --libs opencv`

run_glCam:
	./glCam



