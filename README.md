# Tarea 1: El Planificador Dieciochero

Integrantes:
- Bastian Ampuero
- Guliano Bardi

## Descripción del Proyecto

El proyecto consiste en un planificador de procesos en Linux para coordinar las actividades de una fonda dieciochera modeladas como un grafo dirigido acíclico (DAG). El sistema respeta dependencias entre tareas, limita la concurrencia a un máximo de K procesos simultáneos sin generar espera activa, comunica insumos mediante pipes, aísla fallos en ramas dependientes y responde a interrupciones de la Seremi (SIGINT).

Elegimos C++ (C++17) principalmente por dos razones:
1. Permite acceso directo a las llamadas del sistema POSIX (fork, pipe, wait, signal) con el mismo rendimiento y control de bajo nivel que C.
2. La biblioteca estándar (std::vector, std::string, std::stringstream) facilita el parseo del archivo y el manejo dinámico del grafo sin la complejidad ni los riesgos de memoria de los punteros manuales.

## El formato de plan.txt

Cada línea describe una actividad:

ID : nombre : tiempo_ms : dep1, dep2, ...

- Si tiempo_ms viene vacío, el programa le asigna un valor aleatorio entre 100 y 5000 ms.
- Si no tiene dependencias, ese campo queda vacío.

## Compilación y Ejecución

El Makefile corre g++ -Wall -Wextra -std=c++17 ... -lpthread, tal como pide el enunciado. make clean borra los ejecutables.

Para compilar:
make

Para correrlo:
./planificador plan.txt K

Donde plan.txt es el archivo con las actividades y K es cuántos procesos pueden estar vivos al mismo tiempo.

Para limpiar:
make clean

## Decisiones de Diseño

- DAG y Dependencias: Leemos el archivo línea por línea guardando las tareas en un vector. Cada una tiene su tiempo y la cantidad de dependencias que le faltan (deps_restantes). Al terminar una tarea, se le descuenta a las que estaban esperando.
- Concurrencia: No se lanzan más de K procesos a la vez. Usamos wait() bloqueante para esperar que termine alguno y liberar el cupo, evitando consumir CPU sin hacer nada (busy-waiting).
- Pipes: Cada proceso hijo manda por su pipe el mensaje de texto avisando su insumo listo. El padre lo lee, lo muestra en consola y cierra los descriptores para no acumular archivos abiertos.
- Aislamiento de fallos: Revisamos el retorno con WIFEXITED y WEXITSTATUS. Si una tarea termina con error, se marca cancelada toda la rama que dependía de ella y el resto del plan sigue normal.
- Manejo de Ctrl+C (SIGINT): Se atrapa la señal, se le envía SIGKILL a los hijos que estén corriendo y se limpian los procesos antes de salir para no dejar procesos zombi.

## Carga de Estrés

Incluimos generador_estres.cpp para armar carga_estres.txt con 10.000 actividades encadenadas y probar el sistema con volumen alto.

Para correr la prueba:
make
./generador_estres
./planificador carga_estres.txt 5
