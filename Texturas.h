#include <iostream>
#include "TextureClass.h"

using namespace std;

// Estruturas para armazenar as texturas de uma cidade

// Constantes para indexar as texturas 
enum PISOS {
    None,
    GRASS,
    FLOOR,
    LAST_IMG
};

// Lista de nomes das texturas
string nomeTexturas[] = {
    "None.png",
    "grass.png",
    "floor.png"};

GLuint idTexturaRua[LAST_IMG];  // vetor com os identificadores das texturas


// **********************************************************************
// void CarregaTexturas()
// **********************************************************************
void CarregaTexturas()
{
    for(int i=0;i<LAST_IMG;i++)
        idTexturaRua[i] = LoadTexture(nomeTexturas[i].c_str());
}