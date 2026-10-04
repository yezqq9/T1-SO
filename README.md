# Tarea 1: El Planificador Dieciochero

Integrantes:
- Bastian Ampuero
- Guliano Bardi

## Compilacion y Ejecucion

Compilacion con banderas estrictas:
g++ -Wall -Wextra -std=c++17 -lpthread planificador.cpp -o planificador

Ejecucion:
./planificador plan.txt K

Donde plan.txt es el archivo con las tareas y K es el limite de procesos concurrentes.

## Decisiones de Diseno y Funcionamiento

1. Representacion del DAG y Dependencias
Guardamos las tareas en un vector<tarea>. Cada actividad almacena su tiempo (si viene vacio o con espacio, usamos rand() % 4901 + 100 para darle entre 100 y 5000 ms) y un contador deps_restantes. Cada vez que un proceso hijo termina con exito, el padre busca en el vector y le descuenta 1 a todas las actividades que dependian de esa tarea terminada. Cuando el contador llega a 0, la tarea queda lista para lanzarse.

2. Control de Concurrencia y Sincronizacion (K)
Para respetar el limite de K procesos activos y evitar consumo innecesario de procesador (busy-waiting), usamos la llamada bloqueante wait(&estado_hijo). Cuando se alcanza el tope de K o cuando no hay mas tareas listas para ejecutar, el padre se duerme en el wait. De esta forma el sistema operativo no gasta ciclos de CPU en un bucle vacio y solo despierta al padre cuando un hijo realmente cambia de estado.

3. Comunicacion con Tuberias (Pipes)
Cada proceso hijo tiene su propia tuberia creada con pipe(). 
- El hijo cierra su extremo de lectura (fd[0]), simula el trabajo con usleep(), escribe por fd[1] el mensaje acotado "Insumo listo: [nombre]" y cierra el descriptor antes de salir con exit(0).
- El padre cierra de inmediato el extremo de escritura (fd[1]) tras el fork(). Al recibir la muerte del hijo en el wait(), lee el mensaje por fd[0], lo imprime en pantalla y cierra ese descriptor para evitar fugas de recursos en el sistema.

4. Aislamiento de Errores
Analizamos la salida del hijo usando WIFEXITED(estado_hijo) y WEXITSTATUS(estado_hijo). Si el proceso termina con error (codigo distinto de 0), marcamos la tarea como FALLIDA y recorremos el vector para marcar como CANCELADA a todas las tareas que dependian de ella directa o indirectamente. De esta manera la rama rota se descarta de inmediato, se suman a las tareas finalizadas para no trabar el ciclo, y las demas ramas independientes del plan continuan ejecutandose normalmente.

5. Inspeccion de la Seremi (Ctrl+C / SIGINT)
Capturamos SIGINT con la funcion signal(). Al presionar Ctrl+C, una bandera atomica interrumpe el ciclo principal. El padre recorre las tareas activas (EN_PROCESO), les envia la senal SIGKILL para terminarlas al instante, cierra sus descriptores y hace una limpieza final con wait(NULL) para no dejar ningun proceso huerfano ni zombie en el sistema.

## Pruebas de Estres
Incluimos el archivo auxiliar generador_estres.cpp para generar un plan masivo de 10.000 tareas encadenadas (carga_estres.txt) y comprobar la estabilidad del planificador ante alto volumen.

Para compilar, generar el archivo y ejecutar la prueba:
g++ generador_estres.cpp -o generador_estres
./generador_estres
./planificador carga_estres.txt 5
