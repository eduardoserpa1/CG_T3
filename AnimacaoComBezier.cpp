// **********************************************************************
// PUCRS/Escola Polit�cnica
// COMPUTA��O GR�FICA
//
// Programa basico para criar aplicacoes 2D em OpenGL
//
// Marcio Sarroglia Pinho
// pinho@pucrs.br
// **********************************************************************

// Para uso no Xcode:
// Abra o menu Product -> Scheme -> Edit Scheme -> Use custom working directory
// Selecione a pasta onde voce descompactou o ZIP que continha este arquivo.
//


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

Modelo monta_arquivo(string arquivo);
void CriaInimigos();

Temporizador T;
double AccumDeltaT=0;

// MODELOS:
Modelo playerMod = monta_arquivo("player.txt");
Modelo heartMod = monta_arquivo("heart.txt");

Modelo inimigo1Mod =  monta_arquivo("inimigo.txt");
Modelo inimigo2Mod =  monta_arquivo("inimigo2.txt");
Modelo inimigo3Mod =  monta_arquivo("inimigo3.txt");
Modelo inimigo4Mod =  monta_arquivo("inimigo4.txt");

Modelo tiroMod = monta_arquivo("tiro.txt");

Modelo winMod = monta_arquivo("win.txt");
Modelo loseMod = monta_arquivo("lose.txt");

Player player = Player(&playerMod);

Texto win = Texto(Ponto(0,0), Ponto(5,5,5), &winMod);
Texto lose = Texto(Ponto(0,0), Ponto(5,5,5), &loseMod);

vector<Inimigo> inimigos;
vector<Tiro> tirosPlayer;
vector<Tiro> tirosInimigos;

float min_x,min_y;

Ponto Min, Max;

vector<int> le_linhas(ifstream *f, int n_linha) {
	vector<int> v;
    string line;

	while (n_linha > 0) {
		getline(*f, line); 

		if(line.at(0) == '#')
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

Modelo monta_arquivo(string arquivo){

    ifstream myfile; 
	myfile.open(arquivo);
    if (!myfile.is_open()){
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

	for (int i = 0; i < n_lin; ++i){
		vector<Color> linha_de_cores;
		for (int j = 0; j < n_col; ++j) {
			linha_de_cores.push_back(cores.at(matriz_receita.at(i * n_col + j) - 1));
		}
		modelo.push_back(linha_de_cores);
	}

	myfile.close();
	return modelo;
}

// **************************************************************
void init()
{
    CriaInimigos();

    float d = 15;

    min_x = -d;
    min_y = -d;

    Min = Ponto(-d,-d);
    Max = Ponto(d,d);
    
}

double nFrames=0;
double TempoTotal=0;

void animate()
{
    AccumDeltaT += T.getDeltaT();
    if (AccumDeltaT > 1.0 / 30)
    {
        AccumDeltaT = 0;
        glutPostRedisplay();
    }
}

// **************************************************************
//  void reshape( int w, int h )
//  trata o redimensionamento da janela OpenGL
// **************************************************************
void reshape( int w, int h )
{
    // Reset the coordinate system before modifying
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Define a area a ser ocupada pela area OpenGL dentro da Janela
    glViewport(0, 0, w, h);
    // Define os limites logicos da area OpenGL dentro da Janela
    glOrtho(Min.x,Max.x, Min.y,Max.y, -10,+10);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
// ****************************************************************

// ****************************************************************
void CriaInimigos()
{

	inimigos.push_back(Inimigo(Ponto(-8, 12), 45, &inimigo1Mod));
	inimigos.push_back(Inimigo(Ponto(8, 12), 90, &inimigo2Mod));
    inimigos.push_back(Inimigo(Ponto(-8, -12), 90, &inimigo3Mod));
    inimigos.push_back(Inimigo(Ponto(8, -12), 90, &inimigo4Mod));

}

void update_tiros(vector<Tiro> &tiros) {
	for (auto tiro = tiros.begin(); tiro < tiros.end();) {
		if (tiro->foraDaAreaDeDesenho(Max, Min) || tiro->vida == 0) {
			tiros.erase(tiro);
			continue;
		}
		tiro->desenha();
		tiro->anda();
		++tiro;
	}
}

// ****************************************************************
//  void display( void )
// ****************************************************************
void display( void )
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

    // Define os limites l�gicos da �rea OpenGL dentro da Janela
	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	// Coloque aqui as chamadas das rotinas que desenham os objetos
	// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	//

	if(player.vida <= 0) {
        lose.desenha();
		goto end;
	} else if(inimigos.empty()) {
        win.desenha();
		goto end;
	}

	for (int i = 1; i < player.vida + 1; i++)
		Texto(Ponto(min_x + (i * 1.3), min_y + 1.5, 0), Ponto(1,1,1), &heartMod).desenha();
	
	for (auto &tiro : tirosPlayer) {
		for (auto &inimigo : inimigos) {
			if (tiro.colide(inimigo)) {
				--tiro.vida;
				--inimigo.vida;
			}
		}
	}
	update_tiros(tirosPlayer);

	for (auto &tiro : tirosInimigos) {
		if (tiro.colide(player)) {
			--tiro.vida;
			--player.vida;
		}
	}
	update_tiros(tirosInimigos);

	for (auto inim = inimigos.begin(); inim < inimigos.end();) {
		if (inim->vida <= 0) {
			inimigos.erase(inim);
			continue;
		}
		inim->desenha();
		inim->anda(player.posicao, Max, Min);
		inim->atira(&tirosInimigos, &tiroMod);
		++inim;
	}

	player.desenha();
	--player.delay;

	for (auto inimigo : inimigos) {
		if (player.colide(inimigo) && player.delay <= 0){
            --player.vida;
            player.delay = 60;
        }
	}

end:
	glutSwapBuffers();
}

// ****************************************************************
//  void keyboard ( unsigned char key, int x, int y )
// ****************************************************************
void keyboard ( unsigned char key, int, int) {
	switch (key) {
		case 27: exit(0); // ESC = termina o programa
        case ' ': player.atira(&tirosPlayer, &tiroMod);
	}
}

// ****************************************************************
//  void arrow_keys ( int a_keys, int x, int y )
// ****************************************************************
void arrow_keys ( int a_keys, int, int)
{
	// OBSERVACAO: As rotacoes presumem que o 'normal' eh
	// o objeto apontando para cima
	switch ( a_keys )
	{
        case GLUT_KEY_LEFT:
			player.rotacao = 0;
			if (player.posicao.x > Min.x + player.raio_colisao)
				player.posicao.x -= 0.2;
            break;
        case GLUT_KEY_RIGHT:
			player.rotacao = 180;
			if (player.posicao.x < Max.x - player.raio_colisao)
				player.posicao.x += 0.2;
            break;
		case GLUT_KEY_UP:
			player.rotacao = 270;
			if (player.posicao.y < Max.y - player.raio_colisao)
				player.posicao.y += 0.2;
			break;
	    case GLUT_KEY_DOWN:
			player.rotacao = 90;
			if (player.posicao.y > Min.y + player.raio_colisao)
				player.posicao.y -= 0.2;
			break;
		default:
			break;
	}
}

// ****************************************************************
//  void main ( int argc, char** argv )
// ****************************************************************
int  main ( int argc, char** argv )
{
    cout << "Programa OpenGL" << endl;

	srand (static_cast <unsigned> (time(0)));

    glutInit            ( &argc, argv );
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB );
    glutInitWindowPosition (0,0);

    // Define o tamanho inicial da janela grafica do programa
    glutInitWindowSize  ( 650, 500);

    // Cria a janela na tela, definindo o nome da
    // que aparecera na barra de t�tulo da janela.
    glutCreateWindow    ( "Transformacoes Geometricas em OpenGL" );

    // executa algumas inicializa��es
    init ();

    // Define que o tratador de evento para
    // o redesenho da tela. A funcao "display"
    // ser� chamada automaticamente quando
    // for necess�rio redesenhar a janela
    glutDisplayFunc ( display );

    // Define que o tratador de evento para
    // o invalida��o da tela. A funcao "display"
    // ser� chamada automaticamente sempre que a
    // m�quina estiver ociosa (idle)
    glutIdleFunc(animate);

    // Define que o tratador de evento para
    // o redimensionamento da janela. A funcao "reshape"
    // ser� chamada automaticamente quando
    // o usu�rio alterar o tamanho da janela
    glutReshapeFunc ( reshape );

    // Define que o tratador de evento para
    // as teclas. A funcao "keyboard"
    // ser� chamada automaticamente sempre
    // o usu�rio pressionar uma tecla comum
    glutKeyboardFunc ( keyboard );

    // Define que o tratador de evento para
    // as teclas especiais(F1, F2,... ALT-A,
    // ALT-B, Teclas de Seta, ...).
    // A funcao "arrow_keys" ser� chamada
    // automaticamente sempre o usu�rio
    // pressionar uma tecla especial
    glutSpecialFunc ( arrow_keys );

    // inicia o tratamento dos eventos
    glutMainLoop ( );

    return 0;
}
