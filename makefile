snc: test.c
	gcc test.c -o snc -lpthread

clean:
	rm -f snc
