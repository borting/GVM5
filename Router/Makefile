CC=gcc
CFLAGS=-Wall

TARGETS=dnatd dnatd_manuall ctrl_sim
OBJECTS=iplist.o iptables_ctrl.o portarray.o
SOCKET=sockop.o

all:$(TARGETS)

dnatd:$(OBJECTS) $(SOCKET) dnatd_test.c
	$(CC) $(CFLAGS) -o dnatd dnatd.c $(OBJECTS) $(SOCKET)

dnatd_manuall:$(OBJECTS) dnatd_test.c
	$(CC) $(CFLAGS) -o dnatd_manuall dnatd_test.c $(OBJECTS)

ctrl_sim:$(SOCKET) ctrl_sim.c
	$(CC) $(CFLAGS) -o ctrl_sim ctrl_sim.c $(SOCKET)

iplist.o:iplist.c
	$(CC) $(CFLAGS) -c iplist.c

iptables_ctrl.o:iptables_ctrl.c
	$(CC) $(CFLAGS) -c iptables_ctrl.c

portarray.o:portarray.c
	$(CC) $(CFLAGS) -c portarray.c

sockop.o:sockop.c
	$(CC) $(CFLAGS) -c sockop.c

clean:
	rm -rf $(TARGETS) $(OBJECTS) $(SOCKET)

