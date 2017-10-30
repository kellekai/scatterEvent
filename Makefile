CC = mpicc

all: reset-config hd.exe run

reset-config: config.fti config/config.fti
	$(shell cp config/config.fti .)

hd.exe: scatterEvent.o reset-config
	$(CC) heatdis.c -o hd.exe -lfti -lm scatterEvent.o

scatterEvent.o: scatterEvent.c scatterEvent.h
	$(CC) -c scatterEvent.c

run:
	mpirun -n 8 ./hd.exe 4 config.fti

clean: 
	rm -rf hd.exe config.fti scatterEvent.o Global/ Local/ Meta/

.PHONY: reset-config run
