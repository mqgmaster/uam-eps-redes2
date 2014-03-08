SOURCE = src
OBJECTS = obj

build: 
	gcc -c $(SOURCE)/server.c -o $(OBJECTS)/server.o
	gcc $(OBJECTS)/server.o -o server

clean:
	rm -f server
	rm -f $(OBJECTS)/*.o

rebuild:	clean	build