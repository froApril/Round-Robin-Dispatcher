rrd: pcb.c rrd.c sigtrap.c
	gcc -o process sigtrap.c
	gcc -o rrd pcb.c rrd.c 
	./rrd data.txt
random:
	gcc -o random random.c
	./random data.txt

clean:
	rm process random rrd
