all: attack

attack: framework.c libenc.so
	gcc framework.c -o attack  -g -lm -lpthread -ldl -lenc -L.

libenc.so: libenc.c libenc.h aes.c
	gcc -shared -fpic libenc.c aes.c -falign-functions=4096 -o libenc.so
	
run: attack libenc.so
	LD_LIBRARY_PATH=. ./attack

clean:
	rm -f attack
