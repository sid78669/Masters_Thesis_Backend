# I am a comment, and I want to say that the variable CC will be
# the compiler to use.
CC=g++
C=gcc
# Hey!, I am comment number 2. I want to say that CFLAGS will be the
# options I'll pass to the compiler.
CFLAGS=-c -fmessage-length=0 -std=c++0x -librt -Ofast
CFLAGSALT=-c -fmessage-length=0 -Ofast
DFLAGS=-c -fmessage-length=0 -std=c++0x -librt -g
DFLAGSALT=-c -fmessage-length=0 -g

all: scheduler

debug : Scheduler.d Population.d Chromosome.d Gene.d Helper.d Utility.d getCPUTime.d
	$(CC) Scheduler.o Population.o Chromosome.o Gene.o Helper.o Utility.o getCPUTime.o -lrt -o research_scheduling_backend

scheduler: Scheduler.o Population.o Chromosome.o Gene.o Helper.o Utility.o getCPUTime.o
	$(CC) Scheduler.o Population.o Chromosome.o Gene.o Helper.o Utility.o getCPUTime.o -lrt -o research_scheduling_backend

Scheduler.o: Scheduler.cpp
	$(CC) $(CFLAGS) Scheduler.cpp

Population.o: Population.cpp
	$(CC) $(CFLAGS) Population.cpp
    
Chromosome.o: Chromosome.cpp
	$(CC) $(CFLAGS) Chromosome.cpp

Gene.o: Gene.cpp
	$(CC) $(CFLAGS) Gene.cpp

Helper.o: Helper.cpp
	$(CC) $(CFLAGS) Helper.cpp

Utility.o: Utility.cpp
	$(CC) $(CFLAGS) Utility.cpp

getCPUTime.o: getCPUTime.c
	$(C) $(CFLAGSALT) getCPUTime.c

Scheduler.d: Scheduler.cpp
	$(CC) $(DFLAGS) Scheduler.cpp

Population.d: Population.cpp
	$(CC) $(DFLAGS) Population.cpp
    
Chromosome.d: Chromosome.cpp
	$(CC) $(DFLAGS) Chromosome.cpp

Gene.d: Gene.cpp
	$(CC) $(DFLAGS) Gene.cpp

Helper.d: Helper.cpp
	$(CC) $(DFLAGS) Helper.cpp

Utility.d: Utility.cpp
	$(CC) $(DFLAGS) Utility.cpp

getCPUTime.d: getCPUTime.c
	$(C) $(DFLAGSALT) getCPUTime.c


clean:
	rm *o research_scheduling_backend
