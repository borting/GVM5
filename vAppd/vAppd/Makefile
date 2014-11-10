CC=gcc
FLAG=
CFLAGS=-Wall $(FLAG)

TARGETS=vAppd
OBJECTS=hive.o procReq.o
OBJ=Obj.o
SOCKOP=sockop.o
CTRLSIM=ctrlSim
MGRSIM=mgrSim

all:$(OBJECTS) $(OBJ) $(SOCKOP) vAppd.c ctrlSim.c mgrSim.c
	$(CC) $(CFLAGS) -o $(TARGETS) vAppd.c $(OBJECTS) $(OBJ) $(SOCKOP)
	$(CC) $(CFLAGS) -o $(CTRLSIM) ctrlSim.c $(OBJ) $(SOCKOP)
	$(CC) $(CFLAGS) -o $(MGRSIM) mgrSim.c $(SOCKOP)

hive.o:hive.c
	$(CC) $(CFLAGS) -c hive.c

Obj.o:Obj.c
	$(CC) $(CFLAGS) -c Obj.c

procReq.o:procReq.c
	$(CC) $(CFLAGS) -c procReq.c

sockop.o:sockop.c
	$(CC) $(CFLAGS) -c sockop.c

clean:
	rm -rf $(TARGETS) $(CTRLSIM) $(MGRSIM) $(OBJECTS) $(OBJ) $(SOCKOP)
