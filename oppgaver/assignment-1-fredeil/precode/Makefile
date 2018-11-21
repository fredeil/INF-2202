all: bpt

bpt:
	gcc bpt.c -o bpt -lpthread -lm

test: bpt
	./bpt -t 1

clean:
	rm -f *~ bpt
