#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;
int main(int argc, char* argv[]){
if(argc<3){ //dado que solo nos piden "./planificador plan.txt K" solo tomamos estos argumentos, cualquier valor fuera de los solicitado no lo trabajamos. 
cerr<<"Uso: "<<argv[0]<<" <archivo> <K>\n";
return 1;
}
int K=stoi(argv[2]); //convertimos el argumento K de texto a entero para poder asignarlo
ifstream archivo(argv[1]); //abrimos el archivo del argumento en la posicion 1 
if(!archivo){
cerr<<"El archivo no se pudo abrir\n";
return 1;
}

string linea;
    while(getline(archivo, linea)){
        stringstream flujo(linea);
        string id, nombre, tiempo, dependencias;

        getline(flujo, id, ':');
        getline(flujo, nombre, ':');
        getline(flujo, tiempo, ':');
        getline(flujo, dependencias);

        cout<<"ID: "<<id<<"\n";
        cout<<"Nombre: "<<nombre<<"\n";
        cout<<"Tiempo: "<<tiempo<<"\n";
        cout<<"Dependencia: "<<dependencias<<"\n";
        cout<<"\n";
    }
return 0;
}
