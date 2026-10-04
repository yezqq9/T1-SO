all: planificador generador_estres

planificador: planificador.cpp
	g++ -Wall -Wextra -std=c++17 -lpthread planificador.cpp -o planificador

generador_estres: generador_estres.cpp
	g++ -Wall -Wextra -std=c++17 generador_estres.cpp -o generador_estres

clean:
	rm -f planificador generador_estres carga_estres.txt
