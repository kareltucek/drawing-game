always :
	g++ -ggdb main.c -o thegame `pkg-config gtkmm-3.0 --cflags --libs` --std=c++11

clean :
	rm thegame

