SOURCES = src/
OBJECTS = obj/
OUTPUT_CHAT = chat
OUTPUT_SOUND = prsound

all:: chat prsound

chat.o: $(SOURCES)chat.c
	@echo "\033[32;01mCompilando chat.c\033[00m"
	@gcc -Wall $(SOURCES)chat.c -c -o $(OBJECTS)chat.o `pkg-config --cflags gtk+-2.0`

chat_funcs.o: $(SOURCES)chat_funcs.c
	@echo "\033[32;01mCompilando chat_funcs.c\033[00m"
	@gcc -Wall $(SOURCES)chat_funcs.c -c -o $(OBJECTS)chat_funcs.o `pkg-config --cflags gtk+-2.0`

chat: chat.o chat_funcs.o
	@echo "\033[32;01mEnlazando\033[00m"
	@gcc -o $(OUTPUT) $(OBJECTS)chat.o $(OBJECTS)chat_funcs.o `pkg-config --libs gtk+-2.0`

sound.o: $(SOURCES)sound.c
	@echo "\033[32;01mCompilando sound.c\033[00m"
	@gcc -Wall $(SOURCES)sound.c -c

prsound: prsound.c sound.o
	@echo "\033[32;01mEnlazando\033[00m"
	@gcc -o prsound $(SOURCES)prsound.c $(OBJECTS)sound.o `pkg-config --libs libpulse-simple`

clean:
	rm -f $(OUTPUT_CHAT)
	rm -f $(OUTPUT_SOUND)
	rm -f $(OBJECTS)*.*