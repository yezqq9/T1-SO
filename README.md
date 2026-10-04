# Tarea 1: El Planificador Dieciochero

Integrantes:
- Bastian Ampuero
- Guliano Bardi

## Compilacion y uso

Para compilar usamos los flags de la pauta:
g++ -Wall -Wextra -std=c++17 -lpthread planificador.cpp -o planificador

Para correr el programa:
./planificador plan.txt K

Donde K es la cantidad máxima de procesos que pueden correr al mismo tiempo.

## Cómo funciona el código

- Manejo de tareas y dependencias: Leemos el archivo línea por línea y guardamos cada tarea en un vector. A cada una le guardamos sus datos, el tiempo de ejecución (si no venía definido le pusimos un número al azar entre 100 y 5000 ms) y la cantidad de dependencias que le faltan para poder partir. Cada vez que una tarea termina bien, recorremos el arreglo y le restamos 1 a las tareas que la estaban esperando. Si la cuenta llega a 0, la tarea queda lista.

- Límite de procesos (K) y CPU: Nos aseguramos de no pasar nunca el tope de K procesos ejecutándose a la vez. Cuando se llega a K o cuando no hay tareas disponibles para lanzar, usamos wait() para pausar al padre. Esto hace que el proceso padre quede esperando que termine algún hijo sin gastar CPU en un ciclo vacío (evitando el busy-waiting).

- Comunicación con pipes: Cada hijo tiene su propio pipe. Al terminar su simulación con usleep(), el hijo manda por la tubería el mensaje de texto avisando que su insumo está listo y cierra su extremo. El padre lee el mensaje, lo muestra en la consola y cierra los descriptores que ya no se usan para no acumular archivos abiertos.

- Manejo de fallos: Revisamos la salida del proceso con WIFEXITED y WEXITSTATUS. Si una tarea se cae, se marca como fallida y se cancelan todas las tareas que dependían de ella para que no se tranque el flujo. El resto de las ramas independientes del plan siguen corriendo de forma normal.

- Interrupción con Ctrl+C: Atrapamos la señal SIGINT. Si se interrumpe la ejecución, el padre le manda SIGKILL a los hijos que todavía sigan corriendo, limpia los procesos con wait() para que no queden procesos zombi y cierra el programa de forma limpia.

## Prueba de estrés

Hicimos el programa generador_estres.cpp para armar un archivo de prueba con 10.000 tareas encadenadas (carga_estres.txt) y probar que el sistema soporte harto volumen sin quedarse pegado ni botar errores de pipes.

Pasos para probarlo:
g++ generador_estres.cpp -o generador_estres
./generador_estres
./planificador carga_estres.txt 5
