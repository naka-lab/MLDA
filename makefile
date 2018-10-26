CFLAGS= -O4 -Wall
LIBS= -lstdc++ -lm
CC=gcc
OBJS=GibbsMLDATest.o GibbsMLDA.o

mlda: $(OBJS)
	$(CC) $(OBJS) $(CFLAG) $(LIBS) -o mlda 

clean: 
	rm $(OBJS) mlda