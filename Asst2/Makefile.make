all:multiThreadSorter

multiThreadSorter: multiThreadSorter.c multiThreadSorter.h mergesort.c
	gcc -Wall -Werror -fsanitize=address multiThreadSorter.c -o multiThreadSorter -lm
	gcc -Wall -Werror -fsanitize=address mergesort.c -o mergesort -lm

clean:
	rm -rf multiThreadSorter 
	rm -rf mergesort
