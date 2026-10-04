# Tarea 1: El Planificador Dieciochero

Integrantes:
- Bastian Ampuero
- Guliano Bardi

## Compilacion y uso

Para compilar usamos los flags exigidos por la pauta:
g++ -Wall -Wextra -std=c++17 -lpthread planificador.cpp -o planificador

Para correr el programa:
./planificador plan.txt K

Donde K es la cantidad máxima de procesos que pueden correr en simultáneo.

## Cómo funciona el código

- Manejo de tareas y dependencias: Leemos el archivo línea por línea y guardamos cada tarea en un vector. A cada una le guardamos sus datos, el tiempo de ejecución (si venía vacío le pusimos un número al azar entre 100 y 5000 ms) y la cantidad de dependencias que le faltan para poder partir (`deps_restantes`). Cada vez que una tarea termina bien, recorremos el arreglo y le restamos 1 a las tareas que la estaban esperando. Si la cuenta llega a 0, la tarea queda lista.

- Límite de procesos (K) y CPU: Nos aseguramos de no pasar nunca el tope de K procesos ejecutándose a la vez. Cuando se llega a K o cuando no hay tareas disponibles para lanzar, usamos la llamada bloqueante:
  wait(&estado_hijo);
  Esto duerme al proceso padre a nivel de sistema operativo hasta que algún hijo cambie de estado, liberando el procesador y evitando busy-waiting.

- Comunicación con pipes: Cada hijo tiene su propio pipe creado antes del fork. El hijo duerme con usleep() y escribe el mensaje de texto al padre por el extremo de escritura:
  write(t.fd[1], mensaje.c_str(), mensaje.size());
  El padre lee el mensaje, lo muestra en consola y cierra los descriptores que ya no se usan para evitar fugas de archivos abiertos.

- Manejo de fallos: Revisamos el retorno del proceso con las macros del sistema:
  if (!WIFEXITED(estado_hijo) || WEXITSTATUS(estado_hijo) != 0) { ... }
  Si una tarea se cae o sale con error, se marca como fallida y se cancelan todas las tareas que dependían de ella para que no se tranque el flujo. Las demás ramas independientes siguen corriendo normal.

- Interrupción con Ctrl+C: Atrapamos la señal SIGINT con signal(SIGINT, manejador). Si se interrumpe la ejecución, el padre le manda SIGKILL a los procesos hijos activos, limpia los procesos zombies con wait(NULL) y cierra el programa limpiamente.

## Prueba de estrés

Hicimos el programa generador_estres.cpp para armar un archivo de prueba con 10.000 tareas encadenadas (carga_estres.txt) y probar que el sistema soporte volumen alto sin trabarse ni saturar los descriptores de archivo.

Pasos para probarlo:
g++ generador_estres.cpp -o generador_estres
./generador_estres
./planificador carga_estres.txt 5
