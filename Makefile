# Makefile para Linux e macOS

PROG = main
FONTES = $(shell find . -name '*.cpp')
OBJETOS = $(patsubst %.cpp,%.o,$(FONTES))
CPPFLAGS = -O3 -DGL_SILENCE_DEPRECATION -Wall -Wextra -g

UNAME = `uname`

all: $(TARGET)
	-@make $(UNAME)

%.o: %.cpp
	g++ $(CPPFLAGS) -c $< -o $@

Darwin: $(OBJETOS)
	g++ $(OBJETOS) $(CPPFLAGS) -framework OpenGL -framework Cocoa -framework GLUT -lm -o $(PROG)

Linux: $(OBJETOS)
	g++ $(CPPFLAGS) $(OBJETOS) -lGL -lGLU -lglut -lm -o $(PROG)

clean:
	$(RM) $(OBJETOS) $(PROG)

run:
	./main

.PHONY: all clean run
