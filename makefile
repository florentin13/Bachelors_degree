
#run example
#make build CODE="001white_rectangle"

CODE = "pni"


all: clear build run

build:
	g++ $(CODE).cpp -o $(CODE) -lX11 -lGL -lGLU -lc -lm -lglut -lGLU `pkg-config --cflags --libs opencv` `pkg-config --libs --cflags libusb-1.0`

run:
	./$(CODE)

clear:
	rm -f $(CODE)

pni:

