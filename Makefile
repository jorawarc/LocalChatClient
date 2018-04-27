s-talk: main.o List.o s-talk.o threading.o
	gcc -o s-talk main.o List.o s-talk.o threading.o -pthread

List.o: List.c List.h Node.h
	gcc -c List.c

s-talk.o: s-talk.c s-talk.h
	gcc -c s-talk.c

threading.o: threading.c threading.h
	gcc -c threading.c

main.o: main.c List.h s-talk.h threading.h
	gcc -c main.c

clean:
	rm *.o s-talk
