#Makefile
#Riesenie IOS_project2
#Datum odovzdania: 28.4.2022
#Autor: Matej Keznikl
#Fakulta: Fakulta informačných technológií VUT v Brne (FIT VUT)
#Prelozene: GCC 7.5.0
#Testované na zariadeniach s operačnými systémami: Ubuntu 20.04, Debian 10,...
#Popis: Makefile pre IOS projekt_2

CC=gcc
CFLAGS= -std=gnu99 -Wall -Wextra -Werror -pedantic 
LDFLAGS= -lrt -lpthread

.PHONY: all clean zip
all: proj2

proj2: proj2.o 
	$(CC) $(CFLAGS)  proj2.o -o proj2 $(LDFLAGS)

proj2.o: proj2.c proj2.h
	$(CC) $(CFLAGS) -c proj2.c -o proj2.o $(LDFLAGS)

zip:
	zip proj2.zip *.c *.h Makefile

clean:
	rm -f *.o 
	rm -f proj2
	rm -f proj2.out
	rm -f proj2.out.strace
	rm -f proj2.zip
