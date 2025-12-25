CC = g++

all:
	$(CC) main.cpp -framework opengl -framework ApplicationServices -framework Cocoa -o test -g -I/Users/macbook2015/Desktop/brew/include -framework GLUT
clean:
	@echo Cleaning up...
	@rm test
	@echo Done.

