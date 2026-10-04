## Integrantes
- Guliano Bardi
- Bastian Ampuero

## Compilacion y Ejecucion

Para compilar usamos g++ con -Wall:

g++ -Wall planificador.cpp -o planificador

Para correr el programa le pasamos el archivo y el K:

./planificador plan.txt 2

Donde:
- plan.txt: archivo con las tareas, tiempos y sus dependencias.
- K: cantidad maxima de procesos que dejamos correr al mismo tiempo.

## Como pensamos la solucion (Diseno)

1. Lectura de datos y guardar las tareas
Guardamos todo en un struct tarea adentro de un vector. Leemos con getline linea por linea y vamos cortando con ':' para sacar el id, nombre, tiempo y las dependencias. Si el tiempo viene vacio le tiramos un rand() entre 100 y 5000 ms. Las dependencias las separamos por coma y las metemos a un vector de enteros, y dejamos una variable deps_restantes con el total de dependencias para saber cuando la tarea queda lista para ejecutarse.

2. Manejo de procesos con fork y el limite K
Metimos un while que corre hasta que todas las tareas pasen a terminadas. Si todavia no llegamos al tope de procesos activos (limite K), buscamos que tareas estan pendientes y con deps_restantes en 0. A esas les tiramos fork(). El hijo duerme con usleep(tiempo_ms * 1000) para simular los milisegundos reales del archivo sin redondear a segundos.

3. Pipes y wait para que no se pegue el pc
Para no meter busy-waiting (que la cpu no se quede pegada al 100% gastando recursos a lo loco), cuando llegamos al limite K o no hay nada mas que tirar, hacemos que el papa espere con wait(). Cada tarea tiene su propio pipe. Cuando el hijo termina su usleep, le escribe al papa por fd[1] un mensaje de texto chico diciendo que termino, y hace exit(0). El papa lo desbloquea con el wait, lee el pipe por fd[0] para confirmar, y le resta 1 al deps_restantes de las demas tareas que estaban esperando a esa.
