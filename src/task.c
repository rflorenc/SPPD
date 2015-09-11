#include "defs.h"

void do_work(unsigned long long exec)
{	
	unsigned long long i,ten_ns;
	ten_ns=exec/10;
	for(i=0; i<ten_ns; i++){
		asm volatile  ("nop" ::); asm volatile  ("nop" ::); asm volatile  ("nop" ::); asm volatile  ("nop" ::); 
		asm volatile  ("nop" ::); asm volatile  ("nop" ::); asm volatile  ("nop" ::); asm volatile  ("nop" ::); 
		asm volatile  ("nop" ::); asm volatile  ("nop" ::); asm volatile  ("nop" ::); asm volatile  ("nop" ::); 
		asm volatile  ("nop" ::); asm volatile  ("nop" ::); asm volatile  ("nop" ::); asm volatile  ("nop" ::); 
		asm volatile  ("nop" ::); asm volatile  ("nop" ::); asm volatile  ("nop" ::); asm volatile  ("nop" ::); 
		asm volatile  ("nop" ::); asm volatile  ("nop" ::); asm volatile  ("nop" ::); asm volatile  ("nop" ::); 
		asm volatile  ("nop" ::); asm volatile  ("nop" ::); asm volatile  ("nop" ::); asm volatile  ("nop" ::); 
		asm volatile  ("nop" ::); asm volatile  ("nop" ::); asm volatile  ("nop" ::); asm volatile  ("nop" ::); 
//32		
	}
}
int main(int argc, char** argv) 
{
	unsigned long long deadline,offset, time0,release,next_release,exec,period;
	unsigned int task_id,njobs,i=0;
	unsigned long l,h;
	task_id=atoi(argv[1]);
	exec=(unsigned long long)atoll(argv[2]);
	period=(unsigned long long)atoll(argv[3]);
	deadline=(unsigned long long)atoll(argv[4]);
	offset=(unsigned long long)atoll(argv[5])+OFFSET;
	njobs=atoi(argv[6]);
	time0=(unsigned long long)atoll(argv[7]);

	printf("Task:%d ready to execute\n",task_id);

	l=(unsigned long) deadline;
	deadline >>=32;
	h=(unsigned long)deadline;
	
	
	if(syscall(__NR_casio_sched_setscheduler,task_id,h,l)){
		printf("Error: __NR_casio_sched_setscheduler\n");
		exit(0);
	}
	release=time0+offset;
	next_release=release;
	while(i<njobs){
		i++;
		printf("Task(%d,%d):%llu\n",task_id,i,release);
		l=(unsigned long) next_release;
		next_release >>=32;
		h=(unsigned long)next_release;
		
		if(syscall(__NR_casio_delay_until,h,l)){
			printf("Error: __NR_casio_delay_until\n");
			exit(0);
		}
		do_work(exec);
		release+=period;
		next_release=release;
	}

	exit(task_id);
	return 0;
}
	
	
	

	
	
	
	

