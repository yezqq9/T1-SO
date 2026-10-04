# Tarea 1: El Planificador Dieciochero

Integrantes:
- Bastian Ampuero
- Guliano Bardi

## Compilación y Ejecución

Compilación con Makefile:
make

O compilación manual:
g++ -Wall -Wextra -std=c++17 -lpthread planificador.cpp -o planificador

Para limpiar los ejecutables:
make clean

Ejecución:
./planificador plan.txt K

Donde K es el límite de procesos simultáneos.

## Decisiones de Diseño

- DAG y Dependencias: Leemos el archivo línea por línea guardando las tareas en un vector. Cada una tiene su tiempo (si venía vacío le pusimos rand entre 100 y 5000 ms) y la cantidad de dependencias que le faltan. Al terminar una tarea, se le descuenta a las que estaban esperando.

- Concurrencia: No se lanzan más de K procesos a la vez. Usamos wait() bloqueante para esperar que termine alguno y liberar el cupo, evitando consumir CPU sin hacer nada (busy-waiting).

- Pipes: Cada proceso hijo manda por su pipe el mensaje de texto avisando su insumo listo. El padre lo lee, lo muestra en consola y cierra los descriptores.

- Aislamiento de fallos: Revisamos el retorno con WIFEXITED y WEXITSTATUS. Si una tarea termina con error, se marca cancelada toda la rama que dependía de ella y el resto del plan sigue normal.

- Manejo de Ctrl+C (SIGINT): Se atrapa la señal, se le envía SIGKILL a los hijos que estén corriendo y se limpian los procesos antes de salir.

## Carga de Estrés

Incluimos generador_estres.cpp para armar carga_estres.txt con 10000 actividades y probar el sistema con volumen alto.

Para correr la prueba:
make
./generador_estres
./planificador carga_estres.txt 5
