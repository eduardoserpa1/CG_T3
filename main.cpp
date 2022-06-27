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
void CriaInimigos();

Temporizador T;
double AccumDeltaT = 0;

// MODELOS:
Modelo playerMod = monta_arquivo("player.txt");
Modelo capsulaMod = monta_arquivo("capsula.txt");

Player player = Player(&playerMod);
Inimigo capsula = Inimigo(Ponto(),0,&capsulaMod);

float min_x, min_y;
float max_x, max_y;

Ponto Min, Max;

Mapa cidade = monta_arquivo_mapa("cidade.txt");
float escala = 1.0f;
vector<int> cores_dos_predios;

vector<Ponto> ruas;

vector<Inimigo> inimigos;

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

std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
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
	
	for (int i = 0; i < n_lin; i++){
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
	vector<int> matriz_cores = le_linhas(&myfile, n_lin);

	vector<Color> cores;
	for (int i = 0; i < n_lin * 4; i += 4) {
		Color c;
		c.red = matriz_cores.at(i + 1) / 255.0;
		c.green = matriz_cores.at(i + 2) / 255.0;
		c.blue = matriz_cores.at(i + 3) / 255.0;
		cores.push_back(c);
	}

	vector<int> receita_tamanho = le_linhas(&myfile, 1);
	n_lin = receita_tamanho.at(0);
	int n_col = receita_tamanho.size() > 1 ? receita_tamanho.at(1) : n_lin;

	vector<int> matriz_receita = le_linhas(&myfile, n_lin);

	for (int i = 0; i < n_lin; ++i) {
		vector<Color> linha_de_cores;
		for (int j = 0; j < n_col; ++j) {
			linha_de_cores.push_back(
				cores.at(matriz_receita.at(i * n_col + j) - 1));
		}
		modelo.push_back(linha_de_cores);
	}

	myfile.close();
	return modelo;
}

GLfloat pos[] = {2.0f,2.0f,1.0f};
GLfloat ambiente[] = {0.4f,0.4f,0.4f};
GLfloat difusa[] = {0.7f,0.7f,0.7f};
GLfloat especular[] = {1.0f,1.0f,1.0f};	


void spawn_capsula(){
	int qtd_tile_rua = ruas.size();
	
	int r = rand() % qtd_tile_rua;

	Inimigo capsula = Inimigo(ruas.at(r),0,&capsulaMod);

	inimigos.push_back(capsula);
}

void init()
{	
	glEnable(GL_DEPTH_TEST);	

	glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );
	
	//glShadeModel(GL_SMOOTH);

	glEnable(GL_COLOR_MATERIAL);

	glEnable(GL_LIGHTING);
	
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambiente);

	glLightfv(GL_LIGHT0,GL_AMBIENT,ambiente);

	glLightfv(GL_LIGHT0,GL_DIFFUSE,difusa);

	glLightfv(GL_LIGHT0,GL_POSITION,pos);

	glEnable(GL_LIGHT0);


	// Habilitar o uso de texturas
	glEnable ( GL_TEXTURE_2D );

	CarregaTexturas();


	escala = 10.0f;

	float player_pos_inicial_x = 0;
	float player_pos_inicial_y = 0;

	int xlen = cidade.size();
	int ylen = cidade.at(0).size();

	for (int i = 0; i < xlen; i++){
		for (int j = 0; j < ylen; j++){
			if(cidade.at(i).at(j) > 3){
				int r = rand() % 100;
				cores_dos_predios.push_back(r);
			}else
			if(cidade.at(i).at(j) == 3){
				player_pos_inicial_x = ((i * escala) + escala/2) - (xlen * escala)/2;
				player_pos_inicial_y = ((j * escala) + escala/2) - (ylen * escala)/2;
			}	
			if(cidade.at(i).at(j) == 1){
				float x = ((i * escala) + escala/2) - (xlen * escala)/2;
				float y = ((j * escala) + escala/2) - (ylen * escala)/2;
				Ponto p = Ponto(x,y,0.2f);
				ruas.push_back(p);
			}	
		}
	}
	
	int max = cidade.size();
	if(cidade.at(0).size() > max)
		max = cidade.at(0).size();

	float d = (max * escala)/2;

	cout << "d: " << d << endl;

	min_x = -d;
	min_y = -d;

	max_x = d;
	max_y = d;

	Min = Ponto(-d, -d);
	Max = Ponto(d, d);

	player.max = Max;
	player.min = Min;

	player.posicao = Ponto(player_pos_inicial_x,player_pos_inicial_y, 0.1f);

	player.combustivel = 999;

	spawn_capsula();
	spawn_capsula();
	spawn_capsula();
	spawn_capsula();
	spawn_capsula();
	spawn_capsula();

}


double nFrames = 0;
double TempoTotal = 0;

void animate()
{
	AccumDeltaT += T.getDeltaT();
	if (AccumDeltaT > 1.0 / 30) {
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


void desenha_cidade(){

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

			if(id == 1){
				glBindTexture(GL_TEXTURE_2D, idTexturaRua[None]);
			}

			if(id == 2){
				glBindTexture(GL_TEXTURE_2D, idTexturaRua[GRASS]);
				r = 0.2f;
				g = 0.6f;
				b = 0.2f;
			}

			if(id > 3){
				r = 0.5f;
				g = 0.5f;
				b = 0.5f;

				glColor3f(r,g,b);
				glBindTexture(GL_TEXTURE_2D, idTexturaRua[FLOOR]);

				glBegin(GL_QUADS);
				glNormal3f(0,0,1);
				
				glTexCoord2f(0,0);
				glVertex2f((0.0f + x_len) - x_mag, (0.0f + y_len) - y_mag);
				glTexCoord2f(1,0);
				glVertex2f((escala + x_len) - x_mag, (0.0f + y_len) - y_mag);
				glTexCoord2f(1,1);
				glVertex2f((escala + x_len) - x_mag, (escala + y_len) - y_mag);
				glTexCoord2f(0,1);
				glVertex2f((0.0f + x_len) - x_mag, (escala + y_len) - y_mag);
				
				glEnd();

				defineCor(cores_dos_predios.at(index_cor));
				++index_cor;

				// left wall
				glBegin(GL_QUADS);
				glNormal3f(-1,0,0);
				glVertex3f(((0.0f + escala/4) + x_len) - x_mag, ((0.0f + escala/4) + y_len) - y_mag, 0);
				glVertex3f(((0.0f + escala/4) + x_len) - x_mag, ((0.0f + escala/4) + y_len) - y_mag, id);
				glVertex3f(((0.0f + escala/4) + x_len) - x_mag, ((escala - escala/4) + y_len) - y_mag, id);
				glVertex3f(((0.0f + escala/4) + x_len) - x_mag, ((escala - escala/4)+ y_len) - y_mag, 0);
				glEnd();


				// right wall
				glBegin(GL_QUADS);
				glNormal3f(1,0,0);
				glVertex3f(((escala - escala/4) + x_len) - x_mag, ((0.0f + escala/4) + y_len) - y_mag, 0);
				glVertex3f(((escala - escala/4) + x_len) - x_mag, ((0.0f + escala/4) + y_len) - y_mag, id);
				glVertex3f(((escala - escala/4) + x_len) - x_mag, ((escala - escala/4) + y_len) - y_mag, id);
				glVertex3f(((escala - escala/4) + x_len) - x_mag, ((escala - escala/4) + y_len) - y_mag, 0);
				glEnd();

				// up wall
				glBegin(GL_QUADS);
				glNormal3f(0,1,0);
				glVertex3f(((0.0f + escala/4) + x_len) - x_mag, ((escala - escala/4) + y_len) - y_mag, 0);
				glVertex3f(((0.0f + escala/4) + x_len) - x_mag, ((escala - escala/4) + y_len) - y_mag, id);
				glVertex3f(((escala - escala/4) + x_len) - x_mag, ((escala - escala/4) + y_len) - y_mag, id);
				glVertex3f(((escala - escala/4) + x_len) - x_mag, ((escala - escala/4) + y_len) - y_mag, 0);
				glEnd();

				// down wall
				glBegin(GL_QUADS);
				glNormal3f(0,-1,0);
				glVertex3f(((0.0f + escala/4) + x_len) - x_mag, ((0.0f + escala/4) + y_len) - y_mag, 0);
				glVertex3f(((0.0f + escala/4) + x_len) - x_mag, ((0.0f + escala/4) + y_len) - y_mag, id);
				glVertex3f(((escala - escala/4) + x_len) - x_mag, ((0.0f + escala/4) + y_len) - y_mag, id);
				glVertex3f(((escala - escala/4) + x_len) - x_mag, ((0.0f + escala/4) + y_len) - y_mag, 0);
				glEnd();

				// hoof
				glBegin(GL_QUADS);
				glNormal3f(0,0,1);
				glVertex3f(((0.0f + escala/4) + x_len) - x_mag, ((0.0f + escala/4) + y_len) - y_mag, id);
				glVertex3f(((0.0f + escala/4) + x_len) - x_mag, ((escala - escala/4) + y_len) - y_mag, id);
				glVertex3f(((escala - escala/4) + x_len) - x_mag, ((escala - escala/4) + y_len) - y_mag, id);
				glVertex3f(((escala - escala/4) + x_len) - x_mag, ((0.0f + escala/4) + y_len) - y_mag, id);
				glEnd();		

			}else{
				
				glColor3f(r,g,b);

				glBegin(GL_QUADS);
				glNormal3f(0,0,1);
				
				glTexCoord2f(0,0);
				glVertex2f((0.0f + x_len) - x_mag, (0.0f + y_len) - y_mag);
				glTexCoord2f(1,0);
				glVertex2f((escala + x_len) - x_mag, (0.0f + y_len) - y_mag);
				glTexCoord2f(1,1);
				glVertex2f((escala + x_len) - x_mag, (escala + y_len) - y_mag);
				glTexCoord2f(0,1);
				glVertex2f((0.0f + x_len) - x_mag, (escala + y_len) - y_mag);
				
				glEnd();
				
			}
		}
	}

	glPopMatrix();
}

int acelera = 0;
int rotacaoc = 0;
int tipo_camera = 2;

void camera(float distancia_camera, float altura_camera, float z, int livre){
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
	if(livre){
		gluLookAt(	player.posicao.x + adjacente, player.posicao.y + oposto , altura_camera, //eye
				player.posicao.x + adjacente + adjacente2, player.posicao.y + oposto + oposto2, z, 	//center
				0.0f, 1.0f, GL_HIGH_FLOAT);	//up
	}else{
		gluLookAt(	player.posicao.x + adjacente, player.posicao.y + oposto , altura_camera, //eye
				player.posicao.x, player.posicao.y, z, 	//center
				0.0f, 1.0f, GL_HIGH_FLOAT);	//up
	}
	
}

void display(void)
{
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90, 16.0 / 9.0, 0.01, 200);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	cout << "gas: " << player.combustivel << endl;

	//cout << "x: " << player.posicao.x << "   y: " << player.posicao.y << endl;

	if(player.combustivel > 0){
		if(acelera){
			player.anda();
		}
	}
	
	float distancia_camera = 3.0f;
	float altura_camera = 2.0f;

	switch (tipo_camera)
	{
	case 1:
		//1a pessoa
		camera(0.5f,0.5f,0.5f,0);
		break;	
	case 2:
		//3a pessoa travada
		camera(distancia_camera,altura_camera,0,0);
		break;
	case 3:
		//3a pessoa destravada
		camera(distancia_camera,altura_camera,0,1);
		break;
	
	default:
		break;
	}

	glColor3f(255,0,0);

	glBegin(GL_LINES);
   	glVertex3fv(pos);
   	glVertex3f(0,0,0);
	glEnd();

	desenha_cidade();
	
	for (auto inim = inimigos.begin(); inim < inimigos.end();) {
		if (inim->colide(player)) {
			inimigos.erase(inim);
			player.combustivel += 100;
			continue;
		}
		inim->desenha();
		++inim;
	}
	
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
		if (rotacaoc <= 0.0f)
			rotacaoc = 360.0f;
		break;
	case 'a':
		rotacaoc += 3.0f;
		if (rotacaoc >= 360.0f)
			rotacaoc = 0.0f;
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
		break;
	}
	
}

void keyUP(unsigned char key, int, int){
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
