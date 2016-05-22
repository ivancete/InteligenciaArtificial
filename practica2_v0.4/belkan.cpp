#include "belkan.h"
#include "conexion.h"
#include "environment.h"
#include <iostream>
#include <cstdlib>
#include <vector>
#include <utility>
#include <cstring>

using namespace std;



// -----------------------------------------------------------
void PasarVectoraMapaCaracteres(int fila, int columna, char m[200][200], char *v, int brujula){
  m[fila][columna]=v[0];

    switch(brujula){
        case 0: // Orientacion Norte

		m[fila-1][columna]=v[1];
		for (int i=0; i<3; i++){
		  m[fila-2][columna+i-1]=v[2+i];
		}
		for (int j=0; j<5; j++){
		  m[fila-3][columna+j-2]=v[5+j];
		}
	        break;
	case 1: // Orientacion Este
		m[fila][columna+1]=v[1];
		for (int i=0; i<3; i++){
		  m[fila+i-1][columna+2]=v[2+i];
		}
		for (int j=0; j<5; j++){
		  m[fila+j-2][columna+3]=v[5+j];
		}
	        break;
        case 2: // Orientacion Sur
		m[fila+1][columna]=v[1];
		for (int i=0; i<3; i++){
		  m[fila+2][columna+1-i]=v[2+i];
		}
		for (int j=0; j<5; j++){
		  m[fila+3][columna+2-j]=v[5+j];
		}
		                break;
        case 3: // Orientacion Oeste
		m[fila][columna-1]=v[1];
		for (int i=0; i<3; i++){
		  m[fila+1-i][columna-2]=v[2+i];
		}
		for (int j=0; j<5; j++){
		  m[fila+2-j][columna-3]=v[5+j];
		}

                break;
    }

}


// -----------------------------------------------------------
void Agent::Perceive(Environment &env)
{
	env.SenSorStatus(VISTA_, SURFACE_, MENSAJE_, REINICIADO_, EN_USO_, MOCHILLA_, PUNTUACION_, FIN_JUEGO_, SALUD_, false);

}


bool Agent::Perceive_Remote(conexion_client &Cliente, Environment &env)
{
	bool actualizado=false;


	actualizado = env.Perceive_Remote(Cliente);
	if (actualizado)
		env.SenSorStatus(VISTA_, SURFACE_, MENSAJE_, REINICIADO_, EN_USO_, MOCHILLA_, PUNTUACION_, FIN_JUEGO_, SALUD_, true);

    return actualizado;
}


// -----------------------------------------------------------
string ActionStr(Agent::ActionType accion)
{
	switch (accion)
	{
	case Agent::actFORWARD: return "FORWARD";
	case Agent::actTURN_L: return "TURN LEFT";
	case Agent::actTURN_R: return "TURN RIGHT";
	case Agent::actIDLE: return "IDLE";
	case Agent::actPICKUP: return "PICK UP";
	case Agent::actPUTDOWN: return "PUT DOWN";
	case Agent::actPUSH: return "PUSH";
	case Agent::actPOP: return "POP";
	case Agent::actGIVE: return "GIVE";
	case Agent::actTHROW: return "THROW";
	default: return "????";
	}
}

// -----------------------------------------------------------
void Agent::ActualizarInformacion(Environment *env){
	// Actualizar mi informacion interna
	if (REINICIADO_){

        for(int i = 0; i < 200; i++)
            for(int j = 0; j < 200; j++){
                mapa_entorno_[i][j] = '?';
                mapa_objetos_[i][j] = '?';
                recorrido[i][j] = 0;
            }
        orientarme = true;
		y_1 = -1;
		x_1 = -1;
		y_2 = -1;
		x_2 = -1;
        x_ = 99;
        y_ = 99;
        vistoPK = false;
		primero_dch = false;
		segundo_dch = false;
		primero_izd = false;
		segundo_izd = false;
		avances = 0;
		pos = -1;
        orientacion_ = 3;
        iteracion = 0;
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
	}

	switch(last_accion_){
	  case 0: //avanzar
	  switch(orientacion_){
	    case 0: // norte
		    y_--;
		    break;
	    case 1: // este
		    x_++;
		    break;
	    case 2: // sur
		    y_++;
		    break;
	    case 3: // este
		    x_--;
		    break;
	  }
	  break;
	  case 1: // girar izq
		  orientacion_=(orientacion_+3)%4;
		  break;
	  case 2: // girar dch
		  orientacion_=(orientacion_+1)%4;
		  break;
	}

	// Comprobacion para no salirme del rango del mapa
	bool algo_va_mal=false;
	if (y_<0){
		y_=0;
		algo_va_mal=true;
	}
	else if (y_>199){
		y_=199;
		algo_va_mal=true;
	}
	if (x_<0){
		x_=0;
		algo_va_mal=true;
	}
	else if (x_>199){
		x_=199;
		algo_va_mal=true;
	}

	if (algo_va_mal){
		cout << "CUIDADO: NO ESTAS CONTROLANDO BIEN LA UBICACION DE TU AGENTE\n";
	}


	PasarVectoraMapaCaracteres(y_,x_,mapa_entorno_,VISTA_,orientacion_);
	PasarVectoraMapaCaracteres(y_,x_,mapa_objetos_,SURFACE_,orientacion_);

	env->ActualizarMatrizUsuario(mapa_entorno_);

}

// -----------------------------------------------------------
Agent::ActionType Agent::Think()
{

    Agent::ActionType accion = actFORWARD; // Por defecto avanza

    if(vistoPK){

        if(VISTA_[1] == 'A' || VISTA_[1] == 'B' || VISTA_[1] == 'P' || VISTA_[1] == 'M' || VISTA_[1] == 'D' ||
           (SURFACE_[1] >= 'e' && SURFACE_[1] <= 'z')){
            primero_izd = false;
            segundo_izd = false;
            primero_dch = false;
            segundo_dch = false;
            pos = -1;
            vistoPK = false;
        }

        //IR AL 5(A)
        else if(primero_izd && segundo_izd && pos == 5){
            //cout << "AL 5" << endl;
            if(avances == 1){
                accion = actTURN_L;
            }
            else if(avances == 3){
                accion = actTURN_R;
                avances++;
            }
            else if(avances == 5){
                accion = actTURN_L;
                primero_izd = false;
                segundo_izd = false;
                avances = 0;
                vistoPK = false;
                pos = -1;
            }
            else
                avances++;
        }
        //IR AL 5(B)
        else if(!primero_izd && segundo_izd && pos == 5){
            if(avances == 2){
                accion = actTURN_L;
                avances++;
            }
            else if(avances == 4){
                segundo_izd = false;
                avances = 0;
                vistoPK = false;
                pos = -1;
            }
            else
                avances++;
        }
        //IR AL 9(A)
        else if(primero_dch && segundo_dch && pos == 9){
            //cout << "AL 9" << endl;
            if(avances == 1){
                accion = actTURN_R;
                avances++;
            }
            else if(avances == 3){
                accion = actTURN_L;
                avances++;
            }
            else if(avances == 5){
                accion = actTURN_R;
                primero_dch = false;
                segundo_dch = false;
                avances = 0;
                vistoPK = false;
                pos = -1;
            }
            else
                avances++;
        }

        //IR AL 9(B)
        else if(!primero_dch && segundo_dch && pos == 9){
            if(avances == 2){
                accion = actTURN_R;
                avances++;
            }
            else if(avances == 5){
                primero_dch = false;
                segundo_dch = false;
                avances = 0;
                vistoPK = false;
                pos = -1;
            }
            else
                avances++;
        }

        //IR AL 6(A)
        else if(primero_izd && segundo_dch && pos == 6){
            //cout << "AL 6" << endl;
            if(avances == 1){
                accion = actTURN_L;
                avances++;
            }
            else if(avances == 3){
                accion = actTURN_R;
                avances++;
            }
            else if(avances == 5){
                primero_izd = false;
                segundo_dch = false;
                avances = 0;
                vistoPK = false;
                pos = -1;
            }
            else
                avances++;
        }

        //IR AL 6(B)
        else if(segundo_izd && pos == 6){

            if(avances == 2){
                accion = actTURN_L;
                avances++;
            }
            else if(avances == 4){
                segundo_izd = false;
                avances = 0;
                vistoPK = false;
                pos = -1;
            }
            else
                avances++;
        }

        //IR AL 8(A)
        else if(primero_dch && pos == 8){
            //cout << "AL 8" << endl;
            if(avances == 1){
                accion = actTURN_R;
                avances++;
            }
            else if(avances == 3){
                accion = actTURN_L;
                avances++;
            }
            else if(avances == 5){
                primero_dch = false;
                avances = 0;
                vistoPK = false;
                pos = -1;
            }
            else
                avances++;
        }

        //IR AL 8(B)
        else if(segundo_dch && pos == 8){
            //cout << "SEGUNDO" << endl;
            if(avances == 2){
                accion = actTURN_R;
                avances++;
            }
            else if(avances == 4){
                segundo_dch = false;
                avances = 0;
                vistoPK = false;
                pos = -1;
            }
            else
                avances++;
        }

        //IR AL 2
        else if(primero_izd && pos == 2){
            //cout << "AL 2" << endl;
            if(avances == 1){
                accion = actTURN_L;
                avances++;
            }

            else if(avances == 3){
                primero_izd = false;
                avances = 0;
                vistoPK = false;
                pos = -1;
            }
            else
                avances++;
        }

        //IR AL 4
        else if(primero_dch && pos == 4){
            //cout << "AL 4" << endl;
            if(avances == 1){
                accion = actTURN_R;
                avances++;
            }

            else if(avances == 3){
                primero_dch = false;
                avances = 0;
                vistoPK = false;
                pos = -1;
            }
            else
                avances++;
        }
    }

    //REGLAS DE ORIENTACION
    if(orientarme){

        if(VISTA_[0] == 'K'){
            string cad;
            if(x_1 == -1 && y_1 == -1){
                fil = y_;
                col = x_;
                CapturaFilaColumnaPK(MENSAJE_,y_1, x_1);

                cout << "PRIMER PK ENCONTRADO " << fil << col << y_1 << x_1 << endl;

            }
            else if(y_ != fil || x_ != col){

                fil_2 = y_;
                col_2 = x_;

                CapturaFilaColumnaPK(MENSAJE_, y_2, x_2);

                orientarme = false;

                cout << "SEGUNDO PK ENCONTRADO " << fil_2 << col_2 << y_2 << x_2 << endl;

                int F1,F2,C1,C2;

                F1 = y_1 - y_2;
                F2 = fil - fil_2;
                C1 = x_1 - x_2;
                C2 = col - col_2;

                if(F1 == F2){
                    cout << "Estamos orientados !!!!!!" << endl;
                }
                else if(F1 == -F2){
                    cout << "Estamos bocaAbajooooo !!!!!!" << endl;
                    girar_180();
                    orientacion_ = abs(orientacion_ - 2);
                }
                else if(F1 == C2){
                    cout << "Giramos 90º a la derecha !!!!!!" << endl;
                    gira_derecha();
                    if(orientacion_ == 3)
                        orientacion_ = 0;
                    else
                        orientacion_++;
                }
                else if(F1 == -C2){
                    cout << "Giramos 90º a la izquierda !!!!!!" << endl;
                    gira_izquierda();
                    if(orientacion_ == 0)
                        orientacion_ = 3;
                    else
                        orientacion_--;
                }

                pasarASolucion();
            }
        }

        if(!vistoPK && orientarme){

            //cout << "BUSCA PKKKKKK" << endl;

            if(VISTA_[2] == 'K' && VISTA_[1] != 'B' && VISTA_[1] != 'A' && VISTA_[3] != 'B' && VISTA_[3] != 'A'){
                primero_izd = true;
                segundo_izd = false;
                primero_dch = false;
                segundo_dch = false;
                vistoPK = true;
                pos = 2;
            }
            else if(VISTA_[4] == 'K' && VISTA_[1] != 'B' && VISTA_[1] != 'A' && VISTA_[3] != 'B' && VISTA_[3] != 'A'){
                primero_dch = true;
                segundo_dch = false;
                primero_izd = false;
                segundo_izd = false;
                vistoPK = true;
                pos = 4;
            }
            else if(VISTA_[5] == 'K' && VISTA_[1] != 'B' && VISTA_[1] != 'A' && VISTA_[3] != 'B' && VISTA_[3] != 'A' && VISTA_[6] != 'B' && VISTA_[6] != 'A'){
                if(VISTA_[7] != 'B' && VISTA_[7] != 'A'){
                    primero_izd = false;
                    segundo_izd = true;
                    primero_dch = false;
                    segundo_dch = false;
                    vistoPK = true;
                    pos = 5;
                }
                else if(VISTA_[2] != 'B' && VISTA_[2] != 'A'){
                    primero_izd = true;
                    segundo_izd = true;
                    primero_dch = false;
                    segundo_dch = false;
                    vistoPK = true;
                    pos = 5;
                }
            }
            else if(VISTA_[6] == 'K' && VISTA_[1] != 'B' && VISTA_[1] != 'A' && VISTA_[3] != 'B' && VISTA_[3] != 'A'){
                if(VISTA_[2] != 'B' && VISTA_[2] != 'B'){
                    primero_izd = true;
                    segundo_izd = false;
                    primero_dch = false;
                    segundo_dch = true;
                    vistoPK = true;
                    pos = 6;
                }
                else if(VISTA_[7] != 'B' && VISTA_[7] != 'A'){
                    primero_izd = false;
                    segundo_izd = true;
                    primero_dch = false;
                    segundo_dch = false;
                    vistoPK = true;
                    pos = 6;
                }
            }
            else if(VISTA_[8] == 'K' && VISTA_[1] != 'B' && VISTA_[1] != 'A' && VISTA_[3] != 'B' && VISTA_[3] != 'A'){
                if(VISTA_[4] != 'B' && VISTA_[4] != 'A'){
                    primero_izd = false;
                    segundo_izd = false;
                    primero_dch = true;
                    segundo_dch = false;
                    vistoPK = true;
                    pos = 8;
                }
                else if(VISTA_[7] != 'B' && VISTA_[7] != 'A'){
                    primero_izd = false;
                    segundo_izd = false;
                    primero_dch = false;
                    segundo_dch = false;
                    vistoPK = true;
                    pos = 8;
                }
            }
            else if(VISTA_[9] == 'K' && VISTA_[1] != 'B' && VISTA_[1] != 'A' && VISTA_[3] != 'B' && VISTA_[3] != 'A' && VISTA_[8] != 'B' && VISTA_[8] != 'A'){
                if(VISTA_[4] != 'B' && VISTA_[4] != 'A'){
                    primero_izd = false;
                    segundo_izd = false;
                    primero_dch = true;
                    segundo_dch = true;
                    vistoPK = true;
                    pos = 9;
                }
                else if(VISTA_[7] != 'B' && VISTA_[7] != 'A'){
                    primero_izd = false;
                    segundo_izd = false;
                    primero_dch = false;
                    segundo_dch = true;
                    vistoPK = true;
                    pos = 9;
                }
            }
        }
    }

    if(!vistoPK && !llevoBotas){

        if(cogerImpor){
            accion = actPICKUP;
            cogerImpor = false;
            llevoBotas = true;
        }

        else if(VISTA_[1] == 'B' || VISTA_[1] == 'A' || VISTA_[1] == 'P' || VISTA_[1] == 'M' || VISTA_[1] == 'D'
           || (SURFACE_[1] >= 'e' && SURFACE_[1] <= 'z')){

            if(VISTA_[1] == 'B' && tengoBotas){
                if(EN_USO_ == '6'){
                    llevoBotas = true;
                    accion = actFORWARD;
                }
                else{
                    darCambiarObjeto(accion,'6');
                }
            }
            else if((SURFACE_[1] == 'g' || SURFACE_[1] == 'h') && tengoOro && !entregadoOro){
                cout << "Es la princesa" << endl;
                if(EN_USO_ == '4'){
                    accion = actGIVE;
                    entregadoOro = true;
                    capacidad++;
                }
                else{
                    darCambiarObjeto(accion, '4');
                }
            }
            else if((SURFACE_[1] == 'm' || SURFACE_[1] == 'n') && tengoMan && !entregadoMan){
                cout << "Es la bruja" << endl;
                if(EN_USO_ == '8'){
                    accion = actGIVE;
                    entregadoMan = true;
                    capacidad++;
                }
                else{
                    darCambiarObjeto(accion, '8');
                }
            }
            else if((SURFACE_[1] == 'i' || SURFACE_[1] == 'j') && tengoOscar && !entregadoOscar){
                cout << "Es DiCaprio" << endl;
                if(EN_USO_ == '5'){
                    accion = actGIVE;
                    entregadoOscar = true;
                    capacidad++;
                }
                else{
                    darCambiarObjeto(accion, '5');
                }
            }
            else if((SURFACE_[1] == 'o' || SURFACE_[1] == 'p') && tengoAlg && !entregadoAlg){
                cout << "Es el profe de IA" << endl;
                if(EN_USO_ == '9'){
                    accion = actGIVE;
                    entregadoAlg = true;
                    capacidad++;
                }
                else{
                    darCambiarObjeto(accion, '9');
                }
            }
            else if((SURFACE_[1] == 'k' || SURFACE_[1] == 'l') && tengoOro && !entregadoOro){
                cout << "Es el princeso" << endl;
                if(EN_USO_ == '4'){
                    accion = actGIVE;
                    entregadoOro = true;
                    capacidad++;
                }
                else{
                    darCambiarObjeto(accion, '4');
                }
            }
            else{
                int sigP = mejorGiro(y_, x_, recorrido,orientacion_);

                if(sigP == 2){
                    accion = actTURN_L;
                }
                else if(sigP == 3){
                    accion = actTURN_R;
                }
            }
        }

        else if(SURFACE_[0] >= '0' && SURFACE_[0] <= '9' && capacidad != 0){

            if(EN_USO_ >= '0' && EN_USO_ <= '9'){
                accion = actPUSH;

                if(MENSAJE_ == "Has encontrado un Oscar, no sabia que se puediera encontrar esto!!!. "){
                    tengoOscar = true;
                    cout << "Tengo un oscaaaaar" << endl;
                }
                else if(MENSAJE_ == "Has encontrado una manzana, y por poco no te dio en la cabeza Einstein. "){
                    tengoMan = true;
                    cout << "Tengo una manzana" << endl;
                }
                else if(MENSAJE_ == "Has encontrado un algoritmo, UN ALGORITMO!!!, COMO UN ALGORITMO?. "){
                    tengoAlg = true;
                    cout << "Tengo un algoritmoo" << endl;
                }
                else if(MENSAJE_ == "Has encontrado unas zapatillas, con sus cordones y todo!!!. "){
                    llevoBotas = true;
                    tengoBotas = true;
                    cout << "Tengo unas botaaass" << endl;
                }
                else if(MENSAJE_ == "Has encontrado un lingote de oro, bien bien BIEN!!!. "){
                    tengoOro = true;
                    cout << "Tengo un lingote de oro" << endl;
                }
            }
            else{
                accion = actPICKUP;
                capacidad--;
            }
        }

        else if(SURFACE_[0] >= '0' && SURFACE_[0] <= '9' && MENSAJE_ == "Has encontrado unas zapatillas, con sus cordones y todo!!!. " && capacidad == 0){
            accion = actTHROW;
            cogerImpor = true;
        }
        else{
            //cout << "Mejor direccion " << endl;

            int sigP = mejorDir(y_, x_, recorrido,orientacion_);

            if(sigP == 1){
                accion = actFORWARD;
            }
            else if(sigP == 2){
                accion = actTURN_L;
            }
            else if(sigP == 3){
                accion = actTURN_R;
            }
        }
    }
    else if(!vistoPK && llevoBotas){

        cout << "LLevo puestas unas botaaaaas" << endl;

        if(VISTA_[0] == 'S' || VISTA_[0] == 'T'){
            llevoBotas = false;
            accion = actPUSH;

            cout << "Botas quitadas" << endl;
        }

        else if(VISTA_[1] == 'A' || VISTA_[1] == 'P' || VISTA_[1] == 'M' || VISTA_[1] == 'D'
           || (SURFACE_[1] >= 'e' && SURFACE_[1] <= 'z')){

            int sigP = mejorGiro(y_, x_, recorrido,orientacion_);

            if(sigP == 2){
                accion = actTURN_L;
            }
            else if(sigP == 3){
                accion = actTURN_R;
            }
        }
        else{
            int sigP = mejorDir(y_, x_, recorrido,orientacion_);

            if(sigP == 1){
                accion = actFORWARD;
            }
            else if(sigP == 2){
                accion = actTURN_L;
            }
            else if(sigP == 3){
                accion = actTURN_R;
            }
        }
    }

    if(!orientarme){
        pasarASolucion();
    }

    iteracion++;
    cout << iteracion << endl;
    actualizarPaso(recorrido,y_,x_,iteracion);

	// recuerdo la ultima accion realizada
	last_accion_ = accion;

	return accion;

}
//_____________________________________________________________________________________________________\\

void Agent::darCambiarObjeto(Agent::ActionType & ac, char cosa){

    cout << "Intenta dar algo" << endl;

    if(EN_USO_ == '-'){
        ac = actPOP;
    }
    else if(EN_USO_ != cosa )
        ac = actPUSH;
}

/*void Agent::meterSolucion(){

    //NORTE
    if(orientacion_ == 0){
        mapa_solucion_[y_][x_] = VISTA_[0]; //0
        mapa_solucion_[y_+1][x_] = VISTA_[1]; //1
        mapa_solucion_[y_+2][x_-1] = VISTA_[2]; //2
        mapa_solucion_[y_+2][x_] = VISTA_[3]; //3
        mapa_solucion_[y_+2][x_+1] = VISTA_[4]; //4
        mapa_solucion_[y_+3][x_-2] = VISTA_[5]; //5
        mapa_solucion_[y_+3][x_-1] = VISTA_[6]; //6
        mapa_solucion_[y_+3][x_] = VISTA_[7]; //7
        mapa_solucion_[y_+3][x_+1] = VISTA_[8]; //8
        mapa_solucion_[y_+3][x_+2] = VISTA_[9]; //9
    }
    //ESTE
    else if(orientacion_ == 1){
        mapa_solucion_[y_][x_] = VISTA_[0];
        mapa_solucion_[y_][x_+1] = VISTA_[1];
        mapa_solucion_[y_+1][x_+2] = VISTA_[2];
        mapa_solucion_[y_][x_+2] = VISTA_[3];
        mapa_solucion_[y_-1][x_+2] = VISTA_[4];
        mapa_solucion_[y_+2][x_+3] = VISTA_[5];
        mapa_solucion_[y_+1][x_+3] = VISTA_[6];
        mapa_solucion_[y_][x_+3] = VISTA_[7];
        mapa_solucion_[y_+1][x_+3] = VISTA_[8];
        mapa_solucion_[y_+2][x_+3] = VISTA_[9];
    }
    //SUR
    else if(orientacion_ == 2){
        mapa_solucion_[y_][x_] = VISTA_[0];
        mapa_solucion_[y_-1][x_] = VISTA_[1];
        mapa_solucion_[y_-2][x_+1] = VISTA_[2];
        mapa_solucion_[y_-2][x_] = VISTA_[3];
        mapa_solucion_[y_-2][x_-1] = VISTA_[4];
        mapa_solucion_[y_-3][x_+2] = VISTA_[5];
        mapa_solucion_[y_-3][x_+1] = VISTA_[6];
        mapa_solucion_[y_-3][x_] = VISTA_[7];
        mapa_solucion_[y_-3][x_-1] = VISTA_[8];
        mapa_solucion_[y_-3][x_-2] = VISTA_[9];
    }
    //OESTE
    else if(orientacion_ == 3){
        mapa_solucion_[y_][x_] = VISTA_[0];
        mapa_solucion_[y_][x_-1] = VISTA_[1];
        mapa_solucion_[y_-1][x_-2] = VISTA_[2];
        mapa_solucion_[y_][x_-2] = VISTA_[3];
        mapa_solucion_[y_+1][x_-2] = VISTA_[4];
        mapa_solucion_[y_-2][x_-3] = VISTA_[5];
        mapa_solucion_[y_-1][x_-3] = VISTA_[6];
        mapa_solucion_[y_][x_-3] = VISTA_[7];
        mapa_solucion_[y_+1][x_-3] = VISTA_[8];
        mapa_solucion_[y_+2][x_-3] = VISTA_[9];
    }

}*/

void Agent::actualizarPaso(int m[200][200],int fila,int columna,int iter){
    m[fila][columna] = iter;
}
/*void Agent::actualizarPaso(int m[200][200],int brujula,int fila,int columna,int iter){

    //NORTE
    if(brujula == 0){
        m[fila][columna] = iter; //0
        m[fila+1][columna] = iter; //1
        m[fila+2][columna-1] = iter; //2
        m[fila+2][columna] = iter; //3
        m[fila+2][columna+1] = iter; //4
        m[fila+3][columna-2] = iter; //5
        m[fila+3][columna-1] = iter; //6
        m[fila+3][columna] = iter; //7
        m[fila+3][columna+1] = iter; //8
        m[fila+3][columna+2] = iter; //9
    }
    //ESTE
    else if(brujula == 1){
        m[fila][columna] = iter;
        m[fila][columna+1] = iter;
        m[fila+1][columna+2] = iter;
        m[fila][columna+2] = iter;
        m[fila-1][columna+2] = iter;
        m[fila+2][columna+3] = iter;
        m[fila+1][columna+3] = iter;
        m[fila][columna+3] = iter;
        m[fila+1][columna+3] = iter;
        m[fila+2][columna+3] = iter;
    }
    //SUR
    else if(brujula == 2){
        m[fila][columna] = iter;
        m[fila-1][columna] = iter;
        m[fila-2][columna+1] = iter;
        m[fila-2][columna] = iter;
        m[fila-2][columna-1] = iter;
        m[fila-3][columna+2] = iter;
        m[fila-3][columna+1] = iter;
        m[fila-3][columna] = iter;
        m[fila-3][columna-1] = iter;
        m[fila-3][columna-2] = iter;
    }
    //OESTE
    else if(brujula == 3){
        m[fila][columna] = iter;
        m[fila][columna-1] = iter;
        m[fila-1][columna-2] = iter;
        m[fila][columna-2] = iter;
        m[fila+1][columna-2] = iter;
        m[fila-2][columna-3] = iter;
        m[fila-1][columna-3] = iter;
        m[fila][columna-3] = iter;
        m[fila+1][columna-3] = iter;
        m[fila+2][columna-3] = iter;
    }
}*/

/*int Agent::mejorGiro(int fila, int columna, int z[200][200], int brujula){

    int mejoria1 = 0, mejoria2 = 0, devolver = -1;

    //NORTE
    if(brujula == 0){

        if(z[fila][columna+1] < z[fila][columna-1])
            mejoria2++;
        else
            mejoria1++;

        if(z[fila+1][columna+2] < z[fila-1][columna-2])
            mejoria2++;
        else
            mejoria1++;

        if(z[fila][columna+2] < z[fila][columna-2])
            mejoria2++;
        else
            mejoria1++;

        if(z[fila-1][columna+2] < z[fila+1][columna-2])
            mejoria2++;
        else
            mejoria1++;

        if(z[fila+2][columna+3] < z[fila-2][columna-3])
            mejoria2++;
        else
            mejoria1++;

        if(z[fila+1][columna+3] < z[fila-1][columna-3])
            mejoria2++;
        else
            mejoria1++;

        if(z[fila][columna+3] < z[fila][columna-3])
            mejoria2++;
        else
            mejoria1++;

        if(z[fila-1][columna+3] < z[fila+1][columna-3])
            mejoria2++;
        else
            mejoria1++;

        if(z[fila-2][columna+3] < z[fila+2][columna-3])
            mejoria2++;
        else
            mejoria1++;

        cout << "NORTEEEEE" << endl;

    }
    //ESTE
    else if(brujula == 1){

        if(z[fila+1][columna] < z[fila-1][columna])
            mejoria1++;
        else
            mejoria2++;

        if(z[fila+2][columna-1] < z[fila-2][columna+1])
            mejoria1++;
        else
            mejoria2++;

        if(z[fila+2][columna] < z[fila-2][columna])
            mejoria1++;
        else
            mejoria2++;

        if(z[fila+2][columna+1] < z[fila-2][columna-1])
            mejoria1++;
        else
            mejoria2++;

        if(z[fila+3][columna-2] < z[fila-3][columna+2])
            mejoria1++;
        else
            mejoria2++;

        if(z[fila+3][columna-1] < z[fila-3][columna+1])
            mejoria1++;
        else
            mejoria2++;

        if(z[fila+3][columna] < z[fila-3][columna])
            mejoria1++;
        else
            mejoria2++;

        if(z[fila+3][columna+1] < z[fila-3][columna-1])
            mejoria1++;
        else
            mejoria2++;

        if(z[fila+3][columna+2] < z[fila-3][columna-2])
            mejoria1++;
        else
            mejoria2++;

        cout << "ESTEEEEEE" << endl;
    }
    //SUR
    else if(brujula == 2){

        cout << "SUUUUUUUR" << endl;

        if(z[fila][columna+1] < z[fila][columna-1])
            mejoria1++;
        else
            mejoria2++;

        if(z[fila+1][columna+2] < z[fila-1][columna-2])
            mejoria1++;
        else
            mejoria2++;

        if(z[fila][columna+2] < z[fila][columna-2])
            mejoria1++;
        else
            mejoria2++;

        if(z[fila-1][columna+2] < z[fila+1][columna-2])
            mejoria1++;
        else
            mejoria2++;

        if(z[fila+2][columna+3] < z[fila-2][columna-3])
            mejoria1++;
        else
            mejoria2++;

        if(z[fila+1][columna+3] < z[fila-1][columna-3])
            mejoria1++;
        else
            mejoria2++;

        if(z[fila][columna+3] < z[fila][columna-3])
            mejoria1++;
        else
            mejoria2++;

        if(z[fila-1][columna+3] < z[fila+1][columna-3])
            mejoria1++;
        else
            mejoria2++;

        if(z[fila-2][columna+3] < z[fila+2][columna-3])
            mejoria1++;
        else
            mejoria2++;

        cout << "SUUUUUUR" << endl;
    }
    //OESTE
    else if(brujula == 3){

        if(z[fila+1][columna] < z[fila-1][columna])
            mejoria2++;
        else
            mejoria1++;

        if(z[fila+2][columna-1] < z[fila-2][columna+1])
            mejoria2++;
        else
            mejoria1++;

        if(z[fila+2][columna] < z[fila-2][columna])
            mejoria2++;
        else
            mejoria1++;

        if(z[fila+2][columna+1] < z[fila-2][columna-1])
            mejoria2++;
        else
            mejoria1++;

        if(z[fila+3][columna-2] < z[fila-3][columna+2])
            mejoria2++;
        else
            mejoria1++;

        if(z[fila+3][columna-1] < z[fila-3][columna+1])
            mejoria2++;
        else
            mejoria1++;

        if(z[fila+3][columna] < z[fila-3][columna])
            mejoria2++;
        else
            mejoria1++;

        if(z[fila+3][columna+1] < z[fila-3][columna-1])
            mejoria2++;
        else
            mejoria1++;

        if(z[fila+3][columna+2] < z[fila-3][columna-2])
            mejoria2++;
        else
            mejoria1++;

        cout << "OESTEEEEEE" << endl;
    }

    if(mejoria1 > mejoria2)
        devolver = 1; //Giro izquierda

    else
        devolver = 2; //Giro derecha

    if(devolver == 1 && mejoria2 < mejoria3)
        devolver = 4;
    else(mejoria1 < mejoria3)
        devolver = 4;

    cout << "mejoria1" << mejoria1 << endl;
    cout << "mejoria2" << mejoria2 << endl;
    cout << "mejoria3" << mejoria3 << endl;
    cout << "devolver" << devolver << endl;

    return devolver;
}*/
int Agent::mejorGiro(int fila, int columna, int z[200][200], int brujula){
    int izq = 0, der = 0;

    if(brujula == 0){
        izq = z[fila][columna-1];
        der = z[fila][columna+1];
        z[fila-1][columna] = 30000;
    }
    else if(brujula == 1){
        izq = z[fila-1][columna];
        der = z[fila+1][columna];
        z[fila][columna+1] = 30000;
    }
    else if(brujula == 2){
        izq = z[fila][columna+1];
        der = z[fila][columna-1];
        z[fila+1][columna] = 30000;
    }
    else if(brujula == 3){
        izq = z[fila+1][columna];
        der = z[fila-1][columna];
        z[fila][columna-1] = 30000;
    }

    int devolver;

    if(izq <= der){
        devolver = 2;
    }
    else
        devolver = 3;

    return devolver;
}
int Agent::mejorDir(int fila, int columna, int z[200][200], int brujula){
    int izq = 0, der = 0, sigue = 0;

    if(brujula == 0){
        //cout << "ENTRA NORTE" << endl;
        if(mapa_entorno_[fila][columna-1] != 'P')
            izq = z[fila][columna-1];
        else
            izq = 30000;
        if(mapa_entorno_[fila][columna+1] != 'P')
            der = z[fila][columna+1];
        else
            der = 30000;

        sigue = z[fila-1][columna];
    }
    else if(brujula == 1){
        //cout << "ENTRA ESTE" << endl;
        if(mapa_entorno_[fila-1][columna] != 'P')
            izq = z[fila-1][columna];
        else
            izq = 30000;
        if(mapa_entorno_[fila+1][columna] != 'P')
            der = z[fila+1][columna];
        else
            der = 30000;
        sigue = z[fila][columna+1];
    }
    else if(brujula == 2){
        //cout << "ENTRA SUR" << endl;
        if(mapa_entorno_[fila][columna] != 'P')
            izq = z[fila][columna+1];
        else
            izq = 30000;
        if(mapa_entorno_[fila][columna-1] != 'P')
            der = z[fila][columna-1];
        else
            der = 30000;
        sigue = z[fila+1][columna];
    }
    else if(brujula == 3){
        //cout << "ENTRA OESTE" << endl;
        if(mapa_entorno_[fila+1][columna] != 'P')
            izq = z[fila+1][columna];
        else
            izq = 30000;
        if(mapa_entorno_[fila-1][columna] != 'P')
            der = z[fila-1][columna];
        else
            der = 30000;
        sigue = z[fila][columna-1];
    }

    int devolver = -1;

    if(izq < der){
        if(izq >= sigue){
            devolver = 1;
        }
        else
            devolver = 2;
    }
    else if(der < izq){
        if(der >= sigue){
            devolver = 1;
        }
        else
            devolver = 3;
    }
    else
        devolver = 5;

    return devolver;
}

void Agent::girar_180(){

    gira_derecha();
    gira_derecha();
}

void Agent::gira_derecha(){
    int t;
    bool entro = false;
    bool entro2 = false;
    char mAux [200][200];
    char mAux2 [200][200];
    char mAux3 [200][200];
    for(int i = 0; i < 200; i++){
        t = 0;
        for(int j = 200-1; j >= 0; --j){
            mAux[i][t] = mapa_entorno_[j][i];
            mAux2[i][t] = mapa_objetos_[j][i];
            mAux3[i][t] = recorrido[j][i];
            if(y_ == j && x_ == i && !entro){
                //cout << y_ << "eeeee " << x_ << endl;
                y_ = i;
                x_ = t;
                fil_2 = i;
                col_2 = t;
                entro = true;
                //cout << y_ << "ooooo " << x_ << endl;
            }
            t++;
        }
    }

    cargarMatrix(mAux, mAux2, mAux3);
}

void Agent:: gira_izquierda(){
    int t = 0;
    char mAux [200][200];
    char mAux2 [200][200];
    char mAux3 [200][200];
    bool entro = false;
    bool entro2 = false;
    for(int i = 200-1; i >= 0; --i){
        for(int j = 0; j < 200; j++){
            mAux[t][j] = mapa_entorno_[j][i];
            mAux2[t][j] = mapa_objetos_[j][i];
            mAux3[t][j] = recorrido[j][i];
            if(y_ == j && x_ == i && !entro){
                //cout << y_ << "izq " << x_ << endl;
                y_ = t;
                x_ = j;
                fil_2 = t;
                col_2 = j;
                //cout << y_ << "izque " << x_ << endl;
                entro = true;
            }
        }
        t++;
    }

    cargarMatrix(mAux, mAux2, mAux3);
}

void Agent::cargarMatrix(char mAux [200][200], char mAux2[200][200] , char mAux3[200][200]){

    for(int i = 0; i < 200; i++){
        for(int j = 0; j < 200; j++){
            mapa_entorno_[i][j] = mAux[i][j];
            mapa_objetos_[i][j] = mAux2[i][j];
            recorrido[i][j] = mAux3[i][j];
        }
    }
}

void Agent::pasarASolucion(){

    int aux_y = fil_2-y_2;
    int aux_x = col_2-x_2;
    int k = aux_x;

    /*cout << "Y fil_2: " << fil_2 << endl;
    cout << "X col_2: " << col_2 << endl;
    cout << "Y y_2: " << y_2 << endl;
    cout << "X x_2: " << x_2 << endl;
    cout << "Y buena: " << aux_y << endl;
    cout << "X buena: " << aux_x << endl;*/

    for(int i = 0; i< 100 ; i++){
        for(int j = 0; j<100 ; j++){
            mapa_solucion_[i][j] = mapa_entorno_[aux_y][aux_x];

            aux_x = aux_x + 1;
        }
        aux_x = k;
        aux_y = aux_y + 1;
    }
    /*remove("mapa_ent1");
    remove("mapa_sol1");

    fstream fichero ("mapa_ent1");

    fichero.open("mapa_ent1", ios::out);
    if(fichero.is_open()){
        for(int i = 0; i< 200 ; i++){
            for(int j = 0; j<200 ; j++){
                fichero << mapa_entorno_[i][j];
            }
            fichero << endl;
        }
    }
    else{
        cout << "Error primero" << endl;
    }

    fichero.close();

    fstream ficherito ("mapa_sol1");

    ficherito.open("mapa_sol1", ios::out);
    if(ficherito.is_open()){
        for(int i = 0; i< 100 ; i++){
            for(int j = 0; j<100 ; j++){
                ficherito << mapa_solucion_[i][j];
            }
        ficherito << endl;
        }
    }
    else{
        cout << "Error segundo" << endl;
    }

    ficherito.close();*/
}

void Agent::CapturaFilaColumnaPK (string mensaje, int &fila, int &columna){
	if (mensaje.substr(0,8)=="PK fila:"){
		int pos = mensaje.find('c');
		string valor = mensaje.substr(9,pos-8);
		fila = atoi(valor.c_str());

		int pos2 = mensaje.find('.');
		pos = pos+8;
		valor = mensaje.substr(pos,pos2-1);
		columna = atoi(valor.c_str());
	}
}

/*void Agent::VectoraSolucion(int fila, int columna, char m[100][100], char *v, int brujula){
  m[fila][columna]=v[0];

    switch(brujula){
        case 0: // Orientacion Norte

		m[fila-1][columna]=v[1];
		for (int i=0; i<3; i++){
		  m[fila-2][columna+i-1]=v[2+i];
		}
		for (int j=0; j<5; j++){
		  m[fila-3][columna+j-2]=v[5+j];
		}
	        break;
	case 1: // Orientacion Este
		m[fila][columna+1]=v[1];
		for (int i=0; i<3; i++){
		  m[fila+i-1][columna+2]=v[2+i];
		}
		for (int j=0; j<5; j++){
		  m[fila+j-2][columna+3]=v[5+j];
		}
	        break;
        case 2: // Orientacion Sur
		m[fila+1][columna]=v[1];
		for (int i=0; i<3; i++){
		  m[fila+2][columna+1-i]=v[2+i];
		}
		for (int j=0; j<5; j++){
		  m[fila+3][columna+2-j]=v[5+j];
		}
		                break;
        case 3: // Orientacion Oeste
		m[fila][columna-1]=v[1];
		for (int i=0; i<3; i++){
		  m[fila+1-i][columna-2]=v[2+i];
		}
		for (int j=0; j<5; j++){
		  m[fila+2-j][columna-3]=v[5+j];
		}

                break;
    }

}*/
