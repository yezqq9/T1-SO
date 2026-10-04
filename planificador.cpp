#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <ctime>


using namespace std;

struct tarea{
    int id;
    string nombre;
    int tiempo_ms;              
    vector<int> dependencias;  
};


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


srand(time(NULL)); //usamos la hora actual como semilla para que rand() no repita los mismos numeros al ejecutar
    
vector<tarea> lista_tareas; // vector donde guardaremos todas las tareas leidas
string linea;
    while(getline(archivo, linea)){ //leemos el archivo linea por linea
        stringstream flujo_linea(linea); //convertimos la linea en flujo para separar sus campos
        string id, nombre, tiempo, dependencias;

        getline(flujo_linea, id, ':');
        getline(flujo_linea, nombre, ':'); //separamos por ":"
        getline(flujo_linea, tiempo, ':');
        getline(flujo_linea, dependencias);


        tarea t;
        t.id = stoi(id);
        t.nombre = nombre;


        // si el tiempo viene vacio o solo con un espacio, asignamos aleatorio entre 100 y 5000
        if(tiempo=="" || tiempo==" "){
        t.tiempo_ms = rand() % 4901 + 100;
        }
        else{ 
        t.tiempo_ms = stoi(tiempo);
        }

        
        //separamos las dependencias por comas
        stringstream flujo_deps(dependencias);
        string dep;

        while(getline(flujo_deps, dep, ',')){
            if(dep!="" && dep!=" "){
                t.dependencias.push_back(stoi(dep));
            }
        }

        
        lista_tareas.push_back(t);

        cout<<"ID: "<<t.id<<"\n";
        cout<<"Nombre: "<<t.nombre<<"\n";
        cout<<"Tiempo: "<<t.tiempo_ms<<"\n";
        cout<<"Dependencias: ";
        for(size_t i = 0; i<t.dependencias.size(); i++){
            cout<<t.dependencias[i]<<" ";
        }   
        cout<<"\n\n";
    }
        
(void)K;
        
return 0;
}
