sfml-app: main.o
	g++ $< -o sfml-app -lsfml-graphics -lsfml-window -lsfml-system

main.o: main.cpp simulation.h
	g++ -c $<
