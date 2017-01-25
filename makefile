all: u_c u_s t_c t_s

snc: test.c
	gcc test.c -o snc -lpthread

u_c: client.c udp.c tcp.c
	gcc client.c -o u_c -lpthread

u_s: server.c udp.c
	gcc server.c -o u_s -lpthread

t_c: client_tcp.c
	gcc client_tcp.c -o t_c -lpthread

t_s: server_tcp.c
	gcc server_tcp.c -o t_s -lpthread

clean:
	rm -f snc
