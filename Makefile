CC := gcc -O3
DD := gcc -g

all: server.c client.c
	$(CC) server.c -o server
	$(CC) client.c client_handle.c -o client
	$(CC) version_server.c -o version_server
	$(CC) table_data.c -o table_data

tar:
	tar -cJf ap_ste.tar.xz client.c client.h client_handle.c Makefile server.c server.h version_server.c

debug: server.c client.c
	$(DD) server.c -o server
	$(DD) client_handle.c client.c -o client
	$(DD) version_server.c -o version_server
	$(CC) table_data.c -o table_data

clean:
	rm -f client server version_server client_handle table_data
	rm -f *\~
	rm -f .*.swp
