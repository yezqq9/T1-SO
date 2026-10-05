# Tarea 1: El Planificador Dieciochero

Integrantes:
- Bastian Ampuero

- Guliano Bardi


## Resumen del Proyecto

Desarrollamos un planificador de tareas en C++ para simular la preparación de una fonda, modelando las actividades como un grafo dirigido (DAG). El programa se asegura de que no se superen los K procesos en paralelo, gestiona el traspaso de información con pipes, aísla los fallos cancelando solo las ramas afectadas y captura SIGINT (Ctrl+C) para cerrar los procesos de forma limpia y sin dejar procesos zombi.

Optamos por C++ principalmente para utilizar directamente las syscalls de POSIX (fork, pipe, wait, signal) manteniendo un control fino de recursos, apoyándonos en estructuras estándar como std::vector y stringstream para procesar la entrada de datos de manera limpia.

## Instrucciones de Compilación y Uso

Para compilar ambos programas:
make

Para compilar únicamente el planificador de forma manual:
g++ -Wall -Wextra -std=c++17 -lpthread planificador.cpp -o planificador

Para limpiar ejecutables y archivos temporales:
make clean

Para ejecutar el programa:
./planificador plan.txt K

Donde plan.txt corresponde al archivo de entrada con las tareas y K representa la cota máxima de concurrencia.

## Verificación y Pruebas

- Ejecución estándar:

  Correr ./planificador plan.txt K con el archivo de actividades deseado.

- Prueba de interrupción manual (Ctrl+C):

  Lanzar una ejecución y enviar la señal SIGINT mediante Ctrl+C. El proceso padre captura la interrupción, envía SIGKILL a los hijos en ejecución y espera su término con wait() antes de salir.

- Prueba de carga masiva:

  Ejecutar ./generador_estres para crear el archivo carga_estres.txt (10.000 tareas) y luego correr ./planificador carga_estres.txt 5 para comprobar el comportamiento del sistema bajo alto volumen de procesos y descriptores.

## Aspectos de Implementación

- Representación del DAG: Las tareas se almacenan en un vector llevando la cuenta de dependencias pendientes. Cada vez que una tarea finaliza exitosamente, decrementa las dependencias de sus sucesoras; al llegar a cero, la tarea queda en cola para ejecutarse.

- Concurrencia y uso de CPU: Se restringe el número de hijos simultáneos a K. Cuando se alcanza dicho límite o no existen tareas listas, el padre se bloquea invocando wait(), asegurando la liberación de CPU y evitando esperas activas.

- Comunicación vía Pipes: Por cada tarea se crea un pipe antes del fork. El proceso hijo escribe el mensaje de término y el padre lo lee, lo imprime en pantalla y cierra los extremos para no agotar la tabla de descriptores.

- Manejo de fallos: Se evalúa el código de retorno con macros WIFEXITED y WEXITSTATUS. Si una tarea concluye con error, se marca como fallida y se propagan cancelaciones únicamente sobre su descendencia directa e indirecta.

- Control de señales: Se implementó un manejador para SIGINT que localiza los procesos hijos activos, les despacha SIGKILL y realiza la limpieza correspondiente para evitar estados zombie.

