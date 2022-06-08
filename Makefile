# Makefile para Linux e macOS

PROG = Main
#FONTES = Linha.cpp Ponto.cpp InterseccaoEntreLinhas.cpp Temporizador.cpp
FONTES = Ponto.cpp Temporizador.cpp ListaDeCoresRGB.cpp Bezier.cpp Instancia.cpp Actors.cpp AnimacaoComBezier.cpp

OBJETOS = $(FONTES:.cpp=.o)
CPPFLAGS = -O3 -DGL_SILENCE_DEPRECATION -Wall -Wextra -g  # Todas as warnings, infos de debug

UNAME = `uname`

all: $(TARGET)
	-@make $(UNAME)

Darwin: $(OBJETOS)
#	g++ $(OBJETOS) -O3 -Wno-deprecated -framework OpenGL -framework Cocoa -framework GLUT -lm -o $(PROG)
	g++ $(OBJETOS) $(CPPFLAGS) -framework OpenGL -framework Cocoa -framework GLUT -lm -o $(PROG)

Linux: $(OBJETOS)
	g++ $(OBJETOS) $(CPPFLAGS) -lGL -lGLU -lglut -lm -o $(PROG)

clean:
	-@ rm -f $(OBJETOS) $(PROG)

run:
	./Main
