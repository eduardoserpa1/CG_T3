#ifndef _ACTORS_H_
#define _ACTORS_H_

#include <cstdlib>

#include "Instancia.h"
#include "Ponto.h"

class Player : public Instancia {
public:
	const float speed = 1;
	int combustivel = 100;
	Ponto max;
	Ponto min;
	Player(Modelo *mod);
	void anda(vector<Ponto> espaco, float escala, double delta);

private:
	bool foraDaAreaDeDesenho(Ponto max, Ponto min, Ponto p);
	bool limites_do_espaco(Ponto movimentacao, vector<Ponto> espaco,
						   float escala);
};

class Inimigo : public Instancia {
public:
	Inimigo(Ponto pos, float angulo, Modelo *mod);
};

#endif //_ACTORS_H_
