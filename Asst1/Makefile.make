all:simpleCSVsorter

simpleCSVsorter: simpleCSVsorter.c simpleCSVsorter.h mergesort.c
	gcc -Wall -Werror -fsanitize=address simpleCSVsorter.c -o stringsorter -lm
	gcc -Wall -Werror -fsanitize=address mergesort.c -o mergesort -lm

clean:
	rm -rf simpleCSVsorter 
	rm -rf mergesort
