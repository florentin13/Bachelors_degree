
all: clean build run

run:
	./opticalFlow

build:
	g++ opticalFlow.cpp -o opticalFlow `pkg-config --cflags --libs opencv`

clean:
	rm -f opticalFlow

