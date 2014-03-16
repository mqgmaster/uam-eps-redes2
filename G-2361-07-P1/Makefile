FILES = src/*.c includes/*.h
OBJECTS = obj/*.o
INPUT = src/*.c
OUTPUT = server

build: $(FILES)
	gcc -pthread -o $(OUTPUT) $(INPUT)

clean:
	rm -f $(OUTPUT)
	rm -f $(OBJECTS)

rebuild:	clean	build
