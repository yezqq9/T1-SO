#include <iostream>
#include <fstream>

using namespace std;

int main(){
    ofstream archivo("carga_estres.txt");

    archivo<<"1:tarea_1:1:\n";

    //10000 actividades como exige el enunciado
    for(int i=2; i<=10000; i++){
        archivo<<i<<":tarea_"<<i<<":1:"<<(i-1)<<"\n";
    }

    archivo.close();
    cout<<"listo carga_estres.txt con 10000 tareas\n";
    return 0;
}
