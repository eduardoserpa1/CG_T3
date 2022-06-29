// **********************************************************************
// PUCRS/Escola Politecnica
// COMPUTACAO GRAFICA
//
// Programa criado como solucao ao trabalho 3 da disciplina
//
// Marcio Sarroglia Pinho
// pinho@pucrs.br
// **********************************************************************

// 	Integrantes:
// 	Leonardo Gibrowski Faé - 20280524
// 	José Eduardo Rdorigues Serpa - 20200311
//	Turma 030

#include <iostream>
#include <cmath>
#include <ctime>
#include <fstream>
#include <utility>
#include <vector>
#include <cstdlib>

using namespace std;

#ifdef WIN32
#include <windows.h>
#include <glut.h>
#else
#include <sys/time.h>
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#endif

#ifdef __linux__
#include <GL/glut.h>
#endif

#include "Ponto.h"
#include "Instancia.h"

#include "Temporizador.h"
#include "ListaDeCoresRGB.h"

#include "Actors.h"

#include "Texturas.h"

Modelo monta_arquivo(string arquivo);
Mapa monta_arquivo_mapa(string arquivo);

Temporizador T;
double AccumDeltaT = 0;
const float fps = 30;

//LUZ
GLfloat pos[] = { 2.0f, 2.0f, 1.0f };
GLfloat ambiente[] = { 0.4f, 0.4f, 0.4f };
GLfloat difusa[] = { 0.7f, 0.7f, 0.7f };

// MODELOS:
Modelo playerMod = monta_arquivo("ferrari.tri");
Modelo capsulaMod = monta_arquivo("capsula.tri");

Player player = Player(&playerMod);
Inimigo capsula = Inimigo(Ponto(), 0, &capsulaMod);

//CIDADE
Mapa cidade = monta_arquivo_mapa("cidade.txt");
float escala = 10.0f;
vector<int> cores_dos_predios;

//CAPSULAS
vector<Ponto> ruas;
vector<Inimigo> inimigos;

//CAMERA E PLAYER
int acelera = 0;
int rotacaoc = -180;
float z_terceira_pessoa = 0.0f;
int tipo_camera = 2;
float distancia_camera = 2.5f;
float altura_camera = 1.5f;

//LIMITES
float min_x, min_y;
float max_x, max_y;

Ponto Min, Max;

vector<int> le_linhas(ifstream *f, int n_linha)
{
	vector<int> v;
	string line;

	while (n_linha > 0) {
		getline(*f, line);

		if (line.at(0) == '#')
			continue;

		size_t pos = 0;
		string token;
		while ((pos = line.find(" ")) != string::npos) {
			token = line.substr(0, pos);
			v.push_back(std::stoi(token));
			line.erase(0, pos + 1);
		}

		if (line != " " && !line.empty())
			v.push_back(std::stoi(line));

		--n_linha;
	}
	return v;
}

std::string ReplaceAll(std::string str, const std::string &from,
					   const std::string &to)
{
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos +=
			to.length(); // Handles case where 'to' is a substring of 'from'
	}
	return str;
}

vector<string> le_linhas_tri_to_string(ifstream *f, int n_linha)
{
	vector<string> v;
	string line;

	while (n_linha > 0) {
		getline(*f, line);

		if (line.at(0) == '#')
			continue;

		size_t pos = 0;
		string token;
		line = ReplaceAll(line, "  ", " ");
		while ((pos = line.find(" ")) != string::npos) {
			token = line.substr(0, pos);
			v.push_back(token);
			line.erase(0, pos + 1);
		}

		if (line != " " && !line.empty())
			v.push_back(line);

		--n_linha;
	}
	return v;
}

Mapa monta_arquivo_mapa(string arquivo)
{
	ifstream myfile;
	myfile.open(arquivo);
	if (!myfile.is_open()) {
		cout << "Unable to open file";
		exit(-1);
	}

	Mapa cidade;
	int n_lin = le_linhas(&myfile, 1).at(0);

	for (int i = 0; i < n_lin; i++) {
		vector<int> linha = le_linhas(&myfile, 1);
		cidade.push_back(linha);
	}

	myfile.close();
	return cidade;
}

Modelo monta_arquivo(string arquivo)
{
	ifstream myfile;
	myfile.open(arquivo);
	if (!myfile.is_open()) {
		cout << "Unable to open file";
		exit(-1);
	}

	Modelo modelo;

	int n_lin = le_linhas(&myfile, 1).at(0);

	for (int i = 0; i < n_lin; i++) {
		TrianguloTRI face;

		vector<string> line_strip = le_linhas_tri_to_string(&myfile, 1);

		face.p1 = Ponto(stof(line_strip.at(0)), stof(line_strip.at(1)),
						stof(line_strip.at(2)));
		face.p2 = Ponto(stof(line_strip.at(3)), stof(line_strip.at(4)),
						stof(line_strip.at(5)));
		face.p3 = Ponto(stof(line_strip.at(6)), stof(line_strip.at(7)),
						stof(line_strip.at(8)));

		if (line_strip.at(9).length() > 7) {
			face.cor.red =
				stoul(line_strip.at(9).substr(2, 2), nullptr, 16) / 255.0f;
			face.cor.green =
				stoul(line_strip.at(9).substr(4, 2), nullptr, 16) / 255.0f;
			face.cor.blue =
				stoul(line_strip.at(9).substr(6, 2), nullptr, 16) / 255.0f;
		} else {
			face.cor.red = 0.7f;
			face.cor.green = 0.7f;
			face.cor.blue = 0.7f;
		}

		modelo.push_back(face);
	}

	myfile.close();
	return modelo;
}

void spawn_capsula()
{
	int qtd_tile_rua = ruas.size();

	int r = rand() % qtd_tile_rua;

	Inimigo capsula = Inimigo(ruas.at(r), 0, &capsulaMod);

	inimigos.push_back(capsula);
}

void init()
{
	glEnable(GL_DEPTH_TEST);

	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	//glShadeModel(GL_SMOOTH);

	glEnable(GL_COLOR_MATERIAL);

	glEnable(GL_LIGHTING);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambiente);

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambiente);

	glLightfv(GL_LIGHT0, GL_DIFFUSE, difusa);

	glLightfv(GL_LIGHT0, GL_POSITION, pos);

	glEnable(GL_LIGHT0);

	// Habilitar o uso de texturas
	glEnable(GL_TEXTURE_2D);

	CarregaTexturas();

	float player_pos_inicial_x = 0;
	float player_pos_inicial_y = 0;

	int xlen = cidade.size();
	int ylen = cidade.at(0).size();

	for (int i = 0; i < xlen; i++) {
		for (int j = 0; j < ylen; j++) {
			if (cidade.at(i).at(j) > 3) {
				int r = rand() % 100;
				cores_dos_predios.push_back(r);
			} else if (cidade.at(i).at(j) == 3) {
				player_pos_inicial_x =
					((i * escala) + escala / 2) - (xlen * escala) / 2;
				player_pos_inicial_y =
					((j * escala) + escala / 2) - (ylen * escala) / 2;
			}
			if (cidade.at(i).at(j) == 1 || cidade.at(i).at(j) == 3) {
				float x = ((i * escala) + escala / 2) - (xlen * escala) / 2;
				float y = ((j * escala) + escala / 2) - (ylen * escala) / 2;
				Ponto p = Ponto(x, y, 0.2f);
				ruas.push_back(p);
			}
		}
	}

	unsigned long max = cidade.size();
	if (cidade.at(0).size() > max)
		max = cidade.at(0).size();

	float d = (max * escala) / 2;

	cout << "d: " << d << endl;

	min_x = -d;
	min_y = -d;

	max_x = d;
	max_y = d;

	Min = Ponto(-d, -d);
	Max = Ponto(d, d);

	player.max = Max;
	player.min = Min;

	player.posicao = Ponto(player_pos_inicial_x, player_pos_inicial_y, 0.1f);

	player.combustivel = 500;
}

double nFrames = 0;
double TempoTotal = 0;

void animate()
{
	AccumDeltaT += T.getDeltaT();
	if (AccumDeltaT > 1.0 / fps) {
		AccumDeltaT = 0;
		glutPostRedisplay();
	}
}

void reshape(int w, int h)
{
	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// Define a area a ser ocupada pela area OpenGL dentro da Janela
	glViewport(0, 0, w, h);
	// Define os limites logicos da area OpenGL dentro da Janela
	glOrtho(Min.x, Max.x, Min.y, Max.y, -10, +10);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

/*
 * Parâmetros: os pontos p1 e p2 definem o lado do quadrado que toca o chão
 * */
void parede(float x1, float y1, float x2, float y2, float altura,
			Ponto const &normal)
{
	glBegin(GL_QUADS);
	glNormal3f(normal.x, normal.y, normal.z);
	glVertex3f(x1, y1, 0);
	glVertex3f(x1, y1, altura);
	glVertex3f(x2, y2, altura);
	glVertex3f(x2, y2, 0);
	glEnd();
}

void desenha_cidade()
{
	// 0 - vazio
	// 1 - rua
	// 2 - quadra
	// 3 - posicao inicial do jogador
	// n > 3 - predio com n sendo o tamanho do predio

	int x = cidade.size();
	int y = cidade.at(0).size();

	float x_mag = (x / 2.0) * escala;
	float y_mag = (y / 2.0) * escala;

	int index_cor = 0;

	glPushMatrix();
	for (int i = 0; i < x; i++) {
		for (int j = 0; j < y; j++) {
			float x_len = i * escala;
			float y_len = j * escala;

			int id = cidade.at(i).at(j);

			double r = 0.8f;
			double g = 0.8f;
			double b = 0.8f;

			if (id == 1 || id == 3) {
				glBindTexture(GL_TEXTURE_2D, idTexturaRua[None]);
			}

			if (id == 2) {
				glBindTexture(GL_TEXTURE_2D, idTexturaRua[GRASS]);
				r = 0.2f;
				g = 0.6f;
				b = 0.2f;
			}

			if (id > 3) {
				r = 0.5f;
				g = 0.5f;
				b = 0.5f;

				glColor3f(r, g, b);
				glBindTexture(GL_TEXTURE_2D, idTexturaRua[FLOOR]);

				glBegin(GL_QUADS);
				glNormal3f(0, 0, 1);

				glTexCoord2f(0, 0);
				glVertex2f((0.0f + x_len) - x_mag, (0.0f + y_len) - y_mag);
				glTexCoord2f(1, 0);
				glVertex2f((escala + x_len) - x_mag, (0.0f + y_len) - y_mag);
				glTexCoord2f(1, 1);
				glVertex2f((escala + x_len) - x_mag, (escala + y_len) - y_mag);
				glTexCoord2f(0, 1);
				glVertex2f((0.0f + x_len) - x_mag, (escala + y_len) - y_mag);

				glEnd();

				defineCor(cores_dos_predios.at(index_cor));
				++index_cor;

				const float x_small = ((escala / 4.0f) + x_len) - x_mag;
				const float x_large = ((3.0f * escala / 4.0f) + x_len) - x_mag;
				const float y_small = ((escala / 4.0f) + y_len) - y_mag;
				const float y_large = ((3.0f * escala / 4.0f) + y_len) - y_mag;

				// left wall
				parede(x_small, y_small, x_small, y_large, id, Ponto(-1, 0, 0));

				// right wall
				parede(x_large, y_small, x_large, y_large, id, Ponto(1, 0, 0));

				// up wall
				parede(x_small, y_large, x_large, y_large, id, Ponto(0, 1, 0));

				// down wall
				parede(x_small, y_small, x_large, y_small, id, Ponto(0, -1, 0));

				// hoof
				glBegin(GL_QUADS);
				glNormal3f(0, 0, 1);
				glVertex3f(x_small, y_small, id);
				glVertex3f(x_small, y_large, id);
				glVertex3f(x_large, y_large, id);
				glVertex3f(x_small, y_small, id);
				glEnd();

			} else {
				glColor3f(r, g, b);

				glBegin(GL_QUADS);
				glNormal3f(0, 0, 1);

				glTexCoord2f(0, 0);
				glVertex2f((0.0f + x_len) - x_mag, (0.0f + y_len) - y_mag);
				glTexCoord2f(1, 0);
				glVertex2f((escala + x_len) - x_mag, (0.0f + y_len) - y_mag);
				glTexCoord2f(1, 1);
				glVertex2f((escala + x_len) - x_mag, (escala + y_len) - y_mag);
				glTexCoord2f(0, 1);
				glVertex2f((0.0f + x_len) - x_mag, (escala + y_len) - y_mag);

				glEnd();
			}
		}
	}

	glPopMatrix();
}

void camera(float distancia_camera, float altura_camera, float z, int livre)
{
	float oposto = sin(player.rotacao * M_PI / 180) * distancia_camera;
	float adjacente = cos(player.rotacao * M_PI / 180) * distancia_camera;

	if (oposto > distancia_camera || oposto < -distancia_camera)
		oposto = 0;
	if (adjacente > distancia_camera || adjacente < -distancia_camera)
		adjacente = 0;

	float oposto2 = sin(rotacaoc * M_PI / 180) * distancia_camera;
	float adjacente2 = cos(rotacaoc * M_PI / 180) * distancia_camera;

	if (oposto2 > distancia_camera || oposto2 < -distancia_camera)
		oposto2 = 0;
	if (adjacente2 > distancia_camera || adjacente2 < -distancia_camera)
		adjacente2 = 0;

	if (livre) {
		gluLookAt(player.posicao.x + adjacente, player.posicao.y + oposto,
				  altura_camera, //eye
				  player.posicao.x + adjacente + adjacente2,
				  player.posicao.y + oposto + oposto2, z, //center
				  0.0f, 1.0f, GL_HIGH_FLOAT); //up
	} else {
		gluLookAt(player.posicao.x + adjacente, player.posicao.y + oposto,
				  altura_camera, //eye
				  player.posicao.x, player.posicao.y, z, //center
				  0.0f, 1.0f, GL_HIGH_FLOAT); //up
	}
}

int old_delta = 0;
void display(void)
{
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90, 16.0 / 9.0, 0.01, 200);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	int new_delta = glutGet(GLUT_ELAPSED_TIME);
	float delta = (new_delta - old_delta) / 1000.0f;
	old_delta = new_delta;

	if (player.combustivel > 0) {
		if (acelera) {
			player.anda(ruas, escala, delta);
		}
	}

	switch (tipo_camera) {
	case 1:
		//1a pessoa
		camera(0.5f, 1.0f, 0.8f, 0);
		break;
	case 2:
		//3a pessoa travada
		camera(distancia_camera, altura_camera, 0, 0);
		break;
	case 3:
		//3a pessoa destravada
		camera(distancia_camera, altura_camera, z_terceira_pessoa, 1);
		break;

	default:
		break;
	}

	glColor3f(255, 0, 0);

	glBegin(GL_LINES);
	glVertex3fv(pos);
	glVertex3f(0, 0, 0);
	glEnd();

	desenha_cidade();

	if (inimigos.size() < 3)
		spawn_capsula();

	for (auto inim = inimigos.begin(); inim < inimigos.end();) {
		if (inim->colide(player)) {
			inimigos.erase(inim);
			player.combustivel += 300;
			continue;
		}
		inim->desenha();
		++inim;
	}

	// Apaga a tela para limpar o output
	cout << (char)27 << "[2J";

	//cout << "delta: " << delta << endl;
	cout << "gas: " << player.combustivel << endl;

	cout << "x: " << player.posicao.x << endl;
	cout << "y: " << player.posicao.y << endl << endl;

	//cout << "minimo x tile 1: " << ruas.at(0).x + (escala / 2) << endl;
	//cout << "minimo y tile 1: " << ruas.at(0).y + (escala / 2) << endl << endl;

	//player.combustivel = 100;

	player.desenha();

	glutSwapBuffers();
}

void keyboard(unsigned char key, int, int)
{
	switch (key) {
	case 27:
		exit(0); // ESC = termina o programa
	case ' ':
		acelera = 1;
		break;
	case 'd':
		rotacaoc -= 3.0f;
		break;
	case 'a':
		rotacaoc += 3.0f;
		break;
	case 'w':
		z_terceira_pessoa += 0.2f;
		break;
	case 's':
		z_terceira_pessoa -= 0.2f;
		break;
	case '1':
		tipo_camera = 1;
		break;
	case '2':
		tipo_camera = 2;
		break;
	case '3':
		tipo_camera = 3;
		rotacaoc = player.rotacao - 180;
		z_terceira_pessoa = 0;
		break;
	}
}

void keyUP(unsigned char key, int, int)
{
	switch (key) {
	case ' ':
		acelera = 0;
		break;
	}
}

void arrow_keys(int a_keys, int, int)
{
	// OBSERVACAO: As rotacoes presumem que o 'normal' eh
	// o objeto apontando para cima
	switch (a_keys) {
	case GLUT_KEY_LEFT:
		player.rotacao += 3.0f;
		if (player.rotacao >= 360.0f)
			player.rotacao = 0.0f;
		break;
	case GLUT_KEY_RIGHT:
		player.rotacao -= 3.0f;
		if (player.rotacao <= 0.0f)
			player.rotacao = 360.0f;
		break;
	}
}

int main(int argc, char **argv)
{
	cout << "CG - T3" << endl;

	srand(static_cast<unsigned>(time(0)));

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(650, 500);
	glutCreateWindow("CG - T3");

	init();

	glutDisplayFunc(display);
	glutIdleFunc(animate);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyUP);
	glutSpecialFunc(arrow_keys);

	glutMainLoop();

	return 0;
}
