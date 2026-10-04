#include <iostream>
#include <fstream>
#include <string>

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
cout<<"La linea obtenida es la siguiente: ["<<linea<<"]\n";
}
return 0;
}
