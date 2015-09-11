CC=/usr/bin/gcc

all: casio_launch  casio_task
casio_launch: launch.c 
	${CC} -Wall  launch.c -o casio_launch

casio_task: task.c 
	${CC} -Wall task.c -o casio_task

clean:
	rm -f *.o *.a *~ casio_launch  casio_task
