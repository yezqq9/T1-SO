# Tarea 1: El Planificador Dieciochero

Integrantes:
- Bastian Ampuero
- Guliano Bardi

## Descripción del Proyecto

El programa es un planificador que organiza y ejecuta las actividades de una fonda dieciochera respetando el orden de sus dependencias como un grafo (DAG). Controla que no se superen nunca los K procesos corriendo a la vez sin gastar CPU en esperas vacías, coordina los mensajes de cada actividad mediante pipes, aísla los errores cancelando solo las tareas afectadas y maneja la interrupción por Ctrl+C (SIGINT) cerrando todo de forma limpia.

Elegimos C++ principalmente para usar llamadas del sistema (fork, pipe, wait, signal) de forma directa y a la vez aprovechar la biblioteca estándar (vector, string, stringstream) para parsear el archivo y manejar las tareas sin enredarse con punteros manuales.

## Formato de plan.txt

Cada línea del archivo representa una actividad:

ID : nombre : tiempo_ms : dep1, dep2, ...

- Si tiempo_ms no viene definido, se asigna un valor aleatorio entre 100 y 5000 ms.
- Si una actividad no depende de otra, el campo de dependencias queda en blanco.

## Compilación y Ejecución

Compilar con Makefile:
make

O compilación manual:
g++ -Wall -Wextra -std=c++17 -lpthread planificador.cpp -o planificador

Limpiar ejecutables:
make clean

Ejecutar:
./planificador plan.txt K

Donde plan.txt es la lista de tareas y K el tope de procesos simultáneos.

## Decisiones de Diseño

- DAG y Dependencias: Guardamos las tareas en un vector. Cada una tiene su tiempo y un contador con las dependencias pendientes (deps_restantes). Cuando una tarea termina bien, le resta 1 a las que dependían de ella; al llegar a 0, la tarea queda habilitada para ejecutarse.
- Concurrencia (K) y CPU: Nos aseguramos de no superar el límite K. Cuando se llega al tope o no hay tareas listas para lanzar, el padre se bloquea con wait() esperando a que termine algún hijo, liberando el procesador para evitar busy-waiting.
- Pipes: Cada hijo recibe su propio pipe antes del fork. Al terminar su tiempo, escribe por el pipe avisando que su insumo está listo. El padre lee el mensaje, lo imprime por pantalla y cierra los descriptores para no dejar archivos abiertos.
- Aislamiento de fallos: Se revisa el estado del hijo con WIFEXITED y WEXITSTATUS. Si una tarea falla, se marca como fallida y se cancelan automáticamente solo las tareas que dependían de ella, dejando que las ramas independientes sigan funcionando.
- Manejo de Ctrl+C (SIGINT): Se captura la señal con signal(). Si se interrumpe, el padre envía SIGKILL a los hijos que sigan activos, limpia los procesos con wait() para no dejar zombis y termina la ejecución.

## Prueba de Estrés

Creamos generador_estres.cpp para generar un archivo carga_estres.txt con 10.000 tareas encadenadas y verificar que el planificador aguante alto volumen sin trabarse ni colapsar los pipes.

Para probarlo:
make
./generador_estres
./planificador carga_estres.txt 5
