
#ifndef AGENT__
#define AGENT__

#include <string>
#include <iostream>
#include <cstdlib>
#include "conexion.h"
using namespace std;

// -----------------------------------------------------------
//				class Agent
// -----------------------------------------------------------
class Environment;
class Agent
{
public:
	Agent(){
		x_= 99;
		y_= 99;
		orientacion_=3;
		role_="PlYR";
		last_accion_=3;
		REINICIADO_=false;
		size_=200;
		//giros = 0;
		orientarme = true;
		y_1 = -1;
		x_1 = -1;
		y_2 = -1;
		x_2 = -1;
		vistoPK = false;
		primero_dch = false;
		segundo_dch = false;
		primero_izd = false;
		segundo_izd = false;
		avances = 0;
		pos = -1;
		iteracion = 0;
        capacidad = 4;
        llevoBotas = false;
        tengoBotas = false;
        ponerme = false;
        tengoAlg = false;
        tengoOscar = false;
        tengoMan = false;
        cogerImpor = false;
        entregadoAlg = false;
        entregadoOscar = false;
        entregadoMan = false;
        entregadoOro = false;
        tengoOro = false;

		for (int i=0;i<200;i++)
		    for(int j=0;j<200;j++){
		      mapa_entorno_[i][j]='?';
		      mapa_objetos_[i][j]='?';
		      recorrido[i][j] = 0;
		    }

		for (int i=0;i<100;i++)
		    for(int j=0;j<100;j++)
		      mapa_solucion_[i][j]='?';

	}

	enum ActionType
	{
	    actFORWARD, // avanzar
	    actTURN_L,  // Girar Izquierda
	    actTURN_R,  // Girar Derecha
	    actIDLE,    // No hacer nada
	    actPICKUP,  // Recoger un objeto
	    actPUTDOWN, // Soltar un objeto
	    actPUSH,    // Meter en la mochila
	    actPOP,     // Sacar de la mochila
	    actGIVE,	// Dar un objeto a un personaje
	    actTHROW	// Lanzar un objeto

	};

	void Perceive(Environment &env);
	bool Perceive_Remote(conexion_client &Cliente, Environment &env);
	//______________________________________________________________________________
	void ActualizarInformacion(Environment *env);
	ActionType Think();

	void actualizarPaso(int m[200][200],int fila,int columna,int iter);
	int mejorGiro(int fila, int columna, int z[200][200], int brujula);
	void gira_derecha();
	void gira_izquierda();
	void cargarMatrix(char mAux [200][200], char mAux2[200][200], char mAux3[200][200]);
	void pasarASolucion();
	void girar_180();
	void CapturaFilaColumnaPK (string mensaje, int &fila, int &columna);
	void VectoraSolucion(int fila, int columna, char m[100][100], char *v, int brujula);
	int mejorDir(int fila, int columna, int z[200][200], int brujula);
	void darCambiarObjeto(Agent::ActionType & ac, char cosa);
    //______________________________________________________________________________
	void FixLastAction(Agent::ActionType x){last_accion_=x;};

	char mapa_entorno_[200][200]; // mapa que muestra el tipo de terreno
	char mapa_objetos_[200][200]; // mapa que muestra los objetos que estan encima del terreno
	char mapa_solucion_[100][100]; // Mapa que almacena la solucion que el alumno propone
	int recorrido[200][200]; // Mapa que almacena en que iteracion pase por esa coordenada
	//char mapa_auxiliar[200][200];
	int iteracion; // Indica por la iteracion que va el jugador
	// Funciones de acceso a los datos
	void GetCoord(int &fila, int &columna, int &brujula){fila=y_;columna=x_;brujula=orientacion_;};


private:
	//Variables de interaccion con el entorno grafico
	int size_;

	//SENSORES
	char VISTA_[10];
	char SURFACE_[10];
	bool REINICIADO_;
	string MENSAJE_;
	char EN_USO_;
	char MOCHILLA_[5];
	char PUNTUACION_[9];
	bool FIN_JUEGO_;
	char SALUD_;
	int capacidad;
	bool orientarme;
	int x_1,y_1,fil,col;
	int x_2,y_2,fil_2,col_2;
	bool vistoPK;
	bool primero_izd, segundo_izd;
	bool primero_dch, segundo_dch;
	int avances;
	int pos;
	bool llevoBotas, tengoBotas,ponerme,tengoOscar, tengoAlg, tengoMan, cogerImpor, tengoOro;
	bool entregadoMan, entregadoOscar, entregadoAlg, entregadoOro;

	//Variables de estado
	int x_,y_, orientacion_;
	int last_accion_;
	string role_;

};

string ActionStr(Agent::ActionType);

#endif
