#include "Actors.h"

void Desenha(Modelo const *coords) {

	int x = coords->size();
	int y = coords->at(0).size();

	float x_mag = (x/2.0) * 0.1f;
	float y_mag = (y/2.0) * 0.1f;

	glPushMatrix();
	for (int i = 0; i < x; i++) {
		for (int j = 0; j < y; j++) {

			float x_len = i * 0.1f;
			float y_len = j * 0.1f;

			double r = coords->at(i).at(j).red;
			double g = coords->at(i).at(j).green;
			double b = coords->at(i).at(j).blue;

			glColor3f(r,g,b);

			glBegin(GL_QUADS);
				glVertex2f((0.0f + x_len) - x_mag, (0.0f + y_len) - y_mag);
				glVertex2f((0.1f + x_len) - x_mag, (0.0f + y_len) - y_mag);
				glVertex2f((0.1f + x_len) - x_mag, (0.1f + y_len) - y_mag);
				glVertex2f((0.0f + x_len) - x_mag, (0.1f + y_len) - y_mag);
			glEnd();
		}
	}
	glPopMatrix();
}

Tiro::Tiro(Ponto pos, float angulo, Modelo *mod) : Instancia(mod, 1) {
	this->posicao = pos;
	this->rotacao = angulo;
	this->escala = Ponto(0.5, 0.5, 1.0);
	this->desenha_modelo = Desenha;
}

void Tiro::anda() {
	float oposto = sin(this->rotacao * M_PI / 180) * 0.2;
	float adjacente = cos(this->rotacao * M_PI / 180) * 0.2;
	this->posicao = this->posicao - Ponto(adjacente, oposto);
}

bool Tiro::foraDaAreaDeDesenho(Ponto max, Ponto min) {
	return this->posicao.x > max.x || this->posicao.y > max.y
		|| this->posicao.x < min.x || this->posicao.y < min.y;
}


Player::Player(Modelo *mod) : Instancia(mod, 300) {
	this->desenha_modelo = Desenha;
}

Texto::Texto(Ponto pos, Ponto esc, Modelo *mod) : Instancia(mod, 1) {
	this->posicao = pos;
	this->rotacao = -90;
    this->escala = esc;
	this->desenha_modelo = Desenha;
}

void Player::atira(std::vector<Tiro> *tiros, Modelo *tiroMod) {
	if (tiros->size() < 10)
		tiros->push_back(Tiro(this->posicao, this->rotacao, tiroMod));
}


Inimigo::Inimigo(Ponto pos, float angulo, Modelo *mod)
	: Instancia(mod, 1), curva(Bezier(pos, pos, pos)) {
	this->posicao = pos;
	this->rotacao = angulo > 0 ? angulo : 360.0 + angulo;
	this->desenha_modelo = Desenha;
}

void Inimigo::atira(std::vector<Tiro> *tiros, Modelo *tiroMod) {
	if (this->delay <= 0) {
		tiros->push_back(Tiro(this->posicao, this->rotacao, tiroMod));
		int r = rand() % 100;
		this->delay = r > 0 ? r: -r;
	}
}

void Inimigo::anda(Ponto playerPos, Ponto max, Ponto min) {
	if (this->deltaT >= 1.0) {
		this->curva = Bezier(
			this->posicao,
			 Ponto(
				min.x + static_cast <float> (rand()) / ((float) RAND_MAX/(max.x - min.x)),
				min.y + static_cast <float> (rand()) / ((float) RAND_MAX/(max.y - min.y))
			),
			playerPos);
		this->deltaT = 0.0;
	}
	this->curva.Traca();
	this->deltaT += this->curva.CalculaT(0.1);
	this->posicao = this->curva.Calcula(this->deltaT);
	--this->delay;
}
