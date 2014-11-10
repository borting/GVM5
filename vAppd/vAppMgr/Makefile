CC=gcc
FLAG=
CFLAGS=-Wall $(FLAG)

TARGETS=vAppMgr
CTRLSIM=ctrlSim
VAPPDSIM=vAppdSim
OBJECTS=appList.o portPool.o
SOCKOP=sockop.o

all:$(OBJECTS) $(SOCKOP) vAppMgr.c ctrlSim.c vAppdSim.c
	$(CC) $(CFLAGS) -o $(TARGETS) vAppMgr.c $(OBJECTS) $(SOCKOP) -lpthread
	$(CC) $(CFLAGS) -o $(CTRLSIM) ctrlSim.c $(SOCKOP)
	$(CC) $(CFLAGS) -o $(VAPPDSIM) vAppdSim.c $(SOCKOP)

appList.o:appList.c
	$(CC) $(CFLAGS) -c appList.c

sockop.o:sockop.c
	$(CC) $(CFLAGS) -c sockop.c

portPool.o:portPool.c
	$(CC) $(CFLAGS) -c portPool.c

clean:
	rm -rf $(TARGETS) $(CTRLSIM) $(VAPPDSIM) $(OBJECTS) $(SOCKOP)

