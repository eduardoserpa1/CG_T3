#include "Actors.h"

void Desenha(Modelo const *coords)
{
	glRotatef(90, 1, 0, 0);
	glRotatef(180, 0, 1, 0);

	glScalef(0.5f, 0.5f, 0.5f);

	for (size_t i = 0; i < coords->size(); i++) {
		TrianguloTRI t = coords->at(i);

		glColor3f(t.cor.red, t.cor.green, t.cor.blue);

		Ponto vetor1 = t.p2 - t.p1;
		Ponto vetor2 = t.p3 - t.p1;

		Ponto normal;

		ProdVetorial(vetor1, vetor2, normal);

		glBegin(GL_POLYGON);

		glNormal3f(normal.x, normal.y, normal.z);
		glVertex3f(t.p1.x, t.p1.y, t.p1.z);
		glVertex3f(t.p2.x, t.p2.y, t.p2.z);
		glVertex3f(t.p3.x, t.p3.y, t.p3.z);

		glEnd();
	}
}

//uma lista de minimos e maximos de cada area de grama
//metodo para verificar se o centro do personagem esta dentro desse minimo e maximo
//caso a posicao atual mais o ponto de movimentacao esteja dentro do minimo e maximo, ele nao efetua a movimentação

bool Player::foraDaAreaDeDesenho(Ponto max, Ponto min, Ponto p)
{
	return this->posicao.x - p.x > max.x || this->posicao.y - p.y > max.y ||
		   this->posicao.x - p.x < min.x || this->posicao.y - p.y < min.y;
}

Player::Player(Modelo *mod)
	: Instancia(mod, 300)
{
	this->posicao = Ponto(0, 0, 0);
	this->desenha_modelo = Desenha;
}

void Player::anda(vector<Ponto> espaco, float escala)
{
	float oposto = sin(this->rotacao * M_PI / 180) * 0.2;
	float adjacente = cos(this->rotacao * M_PI / 180) * 0.2;
	Ponto movimentacao = Ponto(adjacente, oposto) * escala;
	if (!foraDaAreaDeDesenho(this->max, this->min, movimentacao) &&
		limites_do_espaco(movimentacao, espaco, escala))
		this->posicao = this->posicao - movimentacao;

	this->combustivel -= 1;

	//cout << limites_do_espaco(Ponto(adjacente, oposto), espaco, escala) << endl;
}

bool Player::limites_do_espaco(Ponto movimentacao, vector<Ponto> espaco,
							   float escala)
{
	Ponto posicao_apos_movimentacao = this->posicao - movimentacao;

	for (size_t i = 0; i < espaco.size(); i++) {
		Ponto min = Ponto(espaco.at(i).x - escala / 2,
						  espaco.at(i).y - escala / 2,
						  espaco.at(i).z - escala / 2);
		Ponto max = Ponto(espaco.at(i).x + escala / 2,
						  espaco.at(i).y + escala / 2,
						  espaco.at(i).z + escala / 2);

		if (posicao_apos_movimentacao >= min &&
			posicao_apos_movimentacao <= max) {
			return 1;
		}
	}

	return 0;
}

Inimigo::Inimigo(Ponto pos, float angulo, Modelo *mod)
	: Instancia(mod, 1)
{
	this->posicao = pos;
	this->rotacao = angulo > 0 ? angulo : 360.0 + angulo;
	this->desenha_modelo = Desenha;
}
