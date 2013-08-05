#run example
#make build CODE="001white_rectangle"

CODE = "gl"


all: clear build run

build:
	g++ $(CODE).cpp -o $(CODE) -lX11 -lGL -lGLU -lc -lm -lglut -lGLU `pkg-config --cflags --libs opencv`

run:
	./$(CODE)

clear:
	rm -f $(CODE)

