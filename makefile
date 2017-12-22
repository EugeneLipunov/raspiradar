all: raspiradar

raspiradar: raspiradar.o protocol.o serial.o
	gcc -g -Wall -lpthread -lrt raspiradar.o protocol.o serial.o -o raspiradar

raspiradar.o: raspiradar.c
	gcc -g -Wall -c raspiradar.c

protocol.o: protocol.c
	gcc -g -Wall -c protocol.c

serial.o: serial.c
	gcc -g -Wall -c serial.c

clean:
	rm -rf *o raspiradar
