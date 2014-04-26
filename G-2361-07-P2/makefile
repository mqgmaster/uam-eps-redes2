SOURCES = src/
OBJECTS = obj/
OUTPUT_CHAT = chat
OUTPUT_SOUND = prsound

all:: chat prsound

chat.o: $(SOURCES)chat.c
	@echo "\033[32;01mCompilando chat.c\033[00m"
	@gcc -Wall $(SOURCES)chat.c -c -o $(OBJECTS)chat.o `pkg-config --cflags gtk+-2.0` -lpthread

chat_funcs.o: $(SOURCES)chat_funcs.c
	@echo "\033[32;01mCompilando chat_funcs.c\033[00m"
	@gcc -Wall $(SOURCES)chat_funcs.c -c -o $(OBJECTS)chat_funcs.o `pkg-config --cflags gtk+-2.0` -lpthread

sound.o: $(SOURCES)sound.c
	@echo "\033[32;01mCompilando sound.c\033[00m"
	@gcc -Wall $(SOURCES)sound.c -c -o $(OBJECTS)sound.o

chat: chat.o chat_funcs.o sound.o
	@echo "\033[32;01mEnlazando\033[00m"
	@gcc -o $(OUTPUT_CHAT) $(OBJECTS)chat.o $(OBJECTS)chat_funcs.o $(OBJECTS)sound.o `pkg-config --libs gtk+-2.0 libpulse-simple` -lpthread

prsound: $(SOURCES)prsound.c sound.o
	@echo "\033[32;01mEnlazando\033[00m"
	@gcc -o prsound $(SOURCES)prsound.c $(OBJECTS)sound.o `pkg-config --libs libpulse-simple` -lpthread

clean:
	rm -f $(OUTPUT_CHAT)
	rm -f $(OUTPUT_SOUND)
	rm -f $(OBJECTS)*.*
