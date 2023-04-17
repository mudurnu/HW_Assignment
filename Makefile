CC = gcc
CFLAGS = -Wall
LIBS = -lmosquitto

mqtt_publisher: ./C/main.c
	$(CC) $(CFLAGS) ./C/main.c -o main $(LIBS)

clean:
	rm -f main
