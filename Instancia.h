//
//  Instancia.hpp
//  OpenGLTest
//
//  Created by Márcio Sarroglia Pinho on 22/09/20.
//  Copyright © 2020 Márcio Sarroglia Pinho. All rights reserved.
//

#ifndef Instancia_hpp
#define Instancia_hpp

#ifdef WIN32
#include <windows.h>
#include <glut.h>
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#endif

#ifdef __linux__
#include <GL/glut.h>
#endif

#include "Ponto.h"
#include <vector>

typedef struct Color {
	double red = 0.0f;
	double green = 0.0f;
	double blue = 0.0f;
} Color;

typedef struct TrianguloTRI {
	Ponto p1 = Ponto(0,0,0);
	Ponto p2 = Ponto(0,0,0);
	Ponto p3 = Ponto(0,0,0);
	Color cor;
} TrianguloTRI;

typedef vector<TrianguloTRI> Modelo;
typedef vector<vector<int> > Mapa;

class Instancia {
    private:
	Modelo *modelo;

    public:
	int vida;
	Ponto posicao, escala;
	float rotacao;
	float raio_colisao;

	Instancia(Modelo *mod, int vida);
	void (*desenha_modelo)(Modelo const *);
	void desenha();
	bool colide(Instancia const &i);
};

#endif /* Instancia_hpp */
