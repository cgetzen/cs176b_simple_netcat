snc: server.c tcp.c udp.c
	gcc server.c -o snc -lpthread

clean:
	rm -f snc
