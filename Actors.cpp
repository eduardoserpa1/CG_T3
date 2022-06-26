#include "Actors.h"

void Desenha(Modelo const *coords)
{
	int x = coords->size();
	int y = coords->at(0).size();

	float x_mag = (x / 2.0) * 0.1f;
	float y_mag = (y / 2.0) * 0.1f;

	glPushMatrix();
	for (int i = 0; i < x; i++) {
		for (int j = 0; j < y; j++) {
			float x_len = i * 0.1f;
			float y_len = j * 0.1f;

			double r = coords->at(i).at(j).red;
			double g = coords->at(i).at(j).green;
			double b = coords->at(i).at(j).blue;

			glColor3f(r, g, b);

			glBegin(GL_QUADS);
			glVertex3f((0.0f + x_len) - x_mag,
				   (0.0f + y_len) - y_mag, 0.1f);
			glVertex3f((0.1f + x_len) - x_mag,
				   (0.0f + y_len) - y_mag, 0.1f);
			glVertex3f((0.1f + x_len) - x_mag,
				   (0.1f + y_len) - y_mag, 0.1f);
			glVertex3f((0.0f + x_len) - x_mag,
				   (0.1f + y_len) - y_mag, 0.1f);
			glEnd();
		}
	}
	glPopMatrix();
}

Tiro::Tiro(Ponto pos, float angulo, Modelo *mod)
	: Instancia(mod, 1)
{
	this->posicao = pos;
	this->rotacao = angulo;
	this->escala = Ponto(0.5, 0.5, 1.0);
	this->desenha_modelo = Desenha;
}

void Tiro::anda()
{
	float oposto = sin(this->rotacao * M_PI / 180) * 0.2;
	float adjacente = cos(this->rotacao * M_PI / 180) * 0.2;
	this->posicao = this->posicao - Ponto(adjacente, oposto);
}

bool Player::foraDaAreaDeDesenho(Ponto max, Ponto min, Ponto p)
{
	return this->posicao.x - p.x > max.x || this->posicao.y - p.y > max.y ||
	       this->posicao.x - p.x < min.x || this->posicao.y - p.y < min.y;
}

Player::Player(Modelo *mod)
	: Instancia(mod, 300)
{
	this->posicao = Ponto(0,0,0);
	this->desenha_modelo = Desenha;
}

void Player::anda()
{
	float oposto = sin(this->rotacao * M_PI / 180) * 0.2;
	float adjacente = cos(this->rotacao * M_PI / 180) * 0.2;
	if( !foraDaAreaDeDesenho(this->max,this->min,Ponto(adjacente, oposto)) )
		this->posicao = this->posicao - Ponto(adjacente, oposto);

	this->combustivel -= 1;
}

Texto::Texto(Ponto pos, Ponto esc, Modelo *mod)
	: Instancia(mod, 1)
{
	this->posicao = pos;
	this->rotacao = -90;
	this->escala = esc;
	this->desenha_modelo = Desenha;
}

void Player::atira(std::vector<Tiro> *tiros, Modelo *tiroMod)
{
	if (tiros->size() < 10)
		tiros->push_back(Tiro(this->posicao, this->rotacao, tiroMod));
}

Inimigo::Inimigo(Ponto pos, float angulo, Modelo *mod)
	: Instancia(mod, 1) 
{
	this->posicao = pos;
	this->rotacao = angulo > 0 ? angulo : 360.0 + angulo;
	this->desenha_modelo = Desenha;
}

void Inimigo::atira(std::vector<Tiro> *tiros, Modelo *tiroMod)
{
	if (this->delay <= 0) {
		tiros->push_back(Tiro(this->posicao, this->rotacao, tiroMod));
		int r = rand() % 100;
		this->delay = r > 0 ? r : -r;
	}
}
