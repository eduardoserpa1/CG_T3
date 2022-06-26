#ifndef _ACTORS_H_
#define _ACTORS_H_

#include <cstdlib>

#include "Instancia.h"
#include "Ponto.h"

class Tiro : public Instancia {
    public:
	Tiro(Ponto pos, float angulo, Modelo *mod);
	void anda();
};

class Player : public Instancia {
    public:
	int delay = 0;
	const float speed = 1;
	int combustivel = 100;
	Ponto max;
	Ponto min;
	Player(Modelo *mod);

	bool foraDaAreaDeDesenho(Ponto max, Ponto min, Ponto p);
	void atira(std::vector<Tiro> *tiro, Modelo *tiroMods);
	void anda();
};

class Inimigo : public Instancia {
    public:
	Inimigo(Ponto pos, float angulo, Modelo *mod);
	void atira(std::vector<Tiro> *tiros, Modelo *tiroMod);
	void anda(Ponto playerPos, Ponto max, Ponto min);

    private:
	int delay = 0;
	float deltaT = 1.0;
};

class Texto : public Instancia {
    public:
	Texto(Ponto pos, Ponto esc, Modelo *mod);
};

#endif //_ACTORS_H_
