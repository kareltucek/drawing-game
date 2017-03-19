always :
	g++ main.c -o thegame `pkg-config gtkmm-3.0 --cflags --libs` 

clean :
	rm thegame

