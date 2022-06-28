//
//  Instancia.cpp
//  OpenGLTest
//
//  Created by Márcio Sarroglia Pinho on 22/09/20.
//  Copyright © 2020 Márcio Sarroglia Pinho. All rights reserved.
//

#include "Instancia.h"

#include <iostream>
#include <fstream>
#include <string>
#include <utility>

void DesenhaPersonagem();
void DesenhaRetangulo();
// ***********************************************************
//  void InstanciaPonto(Ponto3D *p, Ponto3D *out)
//  Esta funcao calcula as coordenadas de um ponto no
//  sistema de referencia do universo (SRU), ou seja,
//  aplica as rotacoes, escalas e translacoes a um
//  ponto no sistema de referencia do objeto (SRO).
// ***********************************************************
void InstanciaPonto(Ponto &p, Ponto &out)
{
	GLfloat ponto_novo[4];
	GLfloat matriz_gl[4][4];
	int i;

	glGetFloatv(GL_MODELVIEW_MATRIX, &matriz_gl[0][0]);

	for (i = 0; i < 4; i++) {
		ponto_novo[i] = matriz_gl[0][i] * p.x + matriz_gl[1][i] * p.y +
				matriz_gl[2][i] * p.z + matriz_gl[3][i];
	}
	out.x = ponto_novo[0];
	out.y = ponto_novo[1];
	out.z = ponto_novo[2];
}

Ponto InstanciaPonto(Ponto P)
{
	Ponto temp;
	InstanciaPonto(P, temp);
	return temp;
}

float calcula_raio_colisao(Modelo const *receita)
{
	/*
	const auto n_lin = receita->size();
	const auto n_col = receita->at(0).size();

	float largest = 0;
	for (unsigned long i = 0; i < n_lin; ++i) {
		for (unsigned long j = 0; j < n_col; ++j) {
			if (sqrt((i * i) + (j * j)) > largest)
				largest = sqrt((i * i) + (j * j));
		}
	}
	*/
	//return (largest / 2.0f) * 0.1f;
	return 1.0f;
}

Instancia::Instancia(Modelo *mod, int vida)
	: modelo(mod)
	, vida(vida)
	, posicao(Ponto(0, 0, 0))
	, escala(Ponto(1, 1, 1))
	, rotacao(0)
	, raio_colisao(calcula_raio_colisao(mod))
{
}

void Instancia::desenha()
{
	// aplica as transformacoes geometricas no modelo
	glPushMatrix();
	glTranslatef(posicao.x, posicao.y, 0);
	glRotatef(rotacao, 0, 0, 1);
	glScalef(escala.x, escala.y, escala.z);

	Ponto PosicaoDoPersonagem;
	Ponto Origem(0, 0, 0);
	InstanciaPonto(Origem, PosicaoDoPersonagem);

	(*desenha_modelo)(modelo);

	glPopMatrix();
}

bool Instancia::colide(Instancia const &i)
{
	return this->raio_colisao + i.raio_colisao >
	       sqrtf(((this->posicao.x - i.posicao.x) *
		      (this->posicao.x - i.posicao.x)) +
		     (this->posicao.y - i.posicao.y) *
			     (this->posicao.y - i.posicao.y));
}
