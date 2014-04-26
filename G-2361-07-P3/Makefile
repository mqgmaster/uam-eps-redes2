CLIENT_OUT = client
SERVER_OUT = server
OBJECTS = *.o

client: client.c
	gcc client.c -o $(CLIENT_OUT) -lssl -lcrypto

server: server.c
	gcc server.c -o $(SERVER_OUT) -lssl -lcrypto

clean:
	rm -f $(CLIENT_OUT)
	rm -f $(SERVER_OUT)
	rm -f $(OBJECTS)
