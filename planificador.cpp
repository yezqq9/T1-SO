#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>


using namespace std;

//bandera para atrapar ctrl c
volatile sig_atomic_t cancelado=0;

void cortar_programa(int sig){
    (void)sig;
    cancelado=1;
}

struct tarea{
    int id;
    string nombre;
    int tiempo_ms;               
    vector<int> dependencias;  
    int deps_restantes; //para saber cuantas dependencias le faltan para ejecutarse
    string estado; //"PENDIENTE", "EN_PROCESO", "TERMINADA", "FALLIDA", "CANCELADA"
    pid_t pid;
    int fd[2]; //pipe para comunicar hijo con padre
};


//le manda el mensaje al papa por el pipe
void mensaje_al_padre(int fd_escritura, string nombre_tarea){
    string texto="Insumo listo: "+nombre_tarea;
    char buffer[100];
    for(size_t k=0; k<texto.length(); k++){
        buffer[k]=texto[k];
    }
    buffer[texto.length()]='\0';
    write(fd_escritura, buffer, 100);
}


//el papa lee lo que mando el hijo
void leer_mensaje_hijo(int fd_lectura){
    char buffer[100];
    read(fd_lectura, buffer, 100);
    cout<<"   [PIPE] "<<buffer<<"\n";
}


void ejecutador_de_procesos(vector<tarea>& DAG, int limite_K){
    int procesos_ejecutandose=0;
    int tareas_terminadas=0;
    int tareas_totales=DAG.size();

    while(tareas_terminadas<tareas_totales){

        //si apretamos ctrl c matamos a los hijos activos
        if(cancelado){
            cout<<"\nprograma interrumpido, cerrando procesos...\n";
            for(int i=0; i<tareas_totales; i++){
                if(DAG[i].estado=="EN_PROCESO"){
                    kill(DAG[i].pid, SIGKILL);
                    close(DAG[i].fd[0]);
                }
            }
            while(wait(NULL)>0);
            exit(0);
        }

        bool hijo_activo=false;

        //si hay cupo lanzamos tareas que no tengan dependencias pendientes
        if(procesos_ejecutandose<limite_K){
            for(int i=0; i<tareas_totales; i++){
                if(DAG[i].estado=="PENDIENTE" && DAG[i].deps_restantes==0){

                    if(pipe(DAG[i].fd)==-1){
                        cerr<<"error al crear pipe\n";
                        exit(1);
                    }

                    pid_t pid=fork();

                    if(pid<0){
                        cerr<<"error al crear el proceso\n";
                        exit(1);
                    }
                    else if(pid==0){
                        //el hijo cierra la lectura
                        close(DAG[i].fd[0]);

                        cout<<"iniciando tarea "<<DAG[i].id<<": "<<DAG[i].nombre<<"\n";
                        usleep(DAG[i].tiempo_ms*1000); //simulamos los ms del archivo

                        mensaje_al_padre(DAG[i].fd[1], DAG[i].nombre);
                        close(DAG[i].fd[1]);

                        exit(0);
                    }
                    else{
                        //el papa cierra la escritura
                        close(DAG[i].fd[1]);

                        DAG[i].pid=pid;
                        DAG[i].estado="EN_PROCESO";
                        procesos_ejecutandose++;
                        hijo_activo=true;

                        if(procesos_ejecutandose==limite_K){
                            break;
                        }
                    }
                }
            }
        }

        //usamos wait para no meter busy-waiting
        if(procesos_ejecutandose==limite_K || (hijo_activo==false && procesos_ejecutandose>0)){
            int estado_hijo;
            pid_t pid_muerto=wait(&estado_hijo);

            //si salto ctrl c volvemos al inicio del while
            if(cancelado) continue;

            for(int i=0; i<tareas_totales; i++){
                if(DAG[i].pid==pid_muerto){
                    procesos_ejecutandose--;
                    tareas_terminadas++;

                    //revisamos si termino con error o bien
                    if(WIFEXITED(estado_hijo) && WEXITSTATUS(estado_hijo)==0){
                        DAG[i].estado="TERMINADA";
                        leer_mensaje_hijo(DAG[i].fd[0]);
                        close(DAG[i].fd[0]);

                        //descontamos la dependencia lista a las tareas que la esperan
                        int id_terminado=DAG[i].id;
                        for(int j=0; j<tareas_totales; j++){
                            for(size_t d=0; d<DAG[j].dependencias.size(); d++){
                                if(DAG[j].dependencias[d]==id_terminado){
                                    DAG[j].deps_restantes--;
                                }
                            }
                        }
                    }
                    else{
                        //si fallo cerramos pipe y cancelamos a las que dependen de ella
                        DAG[i].estado="FALLIDA";
                        close(DAG[i].fd[0]);
                        cout<<"tarea "<<DAG[i].id<<": "<<DAG[i].nombre<<" fallo\n";

                        int id_fallo=DAG[i].id;
                        for(int j=0; j<tareas_totales; j++){
                            if(DAG[j].estado=="PENDIENTE"){
                                for(size_t d=0; d<DAG[j].dependencias.size(); d++){
                                    if(DAG[j].dependencias[d]==id_fallo){
                                        DAG[j].estado="CANCELADA";
                                        tareas_terminadas++;
                                        cout<<"tarea "<<DAG[j].id<<": "<<DAG[j].nombre<<" cancelada\n";
                                        break;
                                    }
                                }
                            }
                        }
                    }
                    break;
                }
            }
        }
    }
}


int main(int argc, char* argv[]){
if(argc<3){ //dado que solo nos piden "./planificador plan.txt K" solo tomamos estos argumentos, cualquier valor fuera de los solicitado no lo trabajamos. 
cerr<<"Uso: "<<argv[0]<<" <archivo> <K>\n";
return 1;
}

//capturamos ctrl c
signal(SIGINT, cortar_programa);

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
        t.id=stoi(id);
        t.nombre=nombre;
        t.estado="PENDIENTE";


        // si el tiempo viene vacio o solo con un espacio, asignamos aleatorio entre 100 y 5000
        if(tiempo=="" || tiempo==" "){
        t.tiempo_ms=rand() % 4901+100;
        }
        else{ 
        t.tiempo_ms=stoi(tiempo);
        }

        
        //separamos las dependencias por comas al igual que en linea con ":"
        stringstream flujo_deps(dependencias);
        string dep;

        while(getline(flujo_deps, dep, ',')){
            if(dep!="" && dep!=" "){
                t.dependencias.push_back(stoi(dep));
            }
        }

        t.deps_restantes=t.dependencias.size();
        
        lista_tareas.push_back(t);

        cout<<"ID: "<<t.id<<"\n";
        cout<<"Nombre: "<<t.nombre<<"\n";
        cout<<"Tiempo: "<<t.tiempo_ms<<"\n";
        cout<<"Dependencias: ";
        for(size_t i=0; i<t.dependencias.size(); i++){
            cout<<t.dependencias[i]<<" ";
        }   
        cout<<"\n\n";
    }

ejecutador_de_procesos(lista_tareas, K);

return 0;
}
