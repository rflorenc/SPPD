
#include "defs.h"


/////////////////////////////////////////////////////////////////////////////////
//tasks
void print_task(struct task *task)
{
	printf("TASK:%d\t%llu\t%llu\t%llu\t%llu\n",
	task->task_id,
	task->exec,
	task->period,
	task->deadline,
	task->offset);
}
void print_tasks(struct task *tasks, int num)
{
	int i;
	
	for(i=0;i<num;i++){
		print_task(&tasks[i]);
	}
}
unsigned int get_u32(char *s)
{
	int i,d=0;
	unsigned int v=0;
	for(i=0;s[i]>='0' && s[i]<='9';i++){
		d=s[i]-'0';
		v=(v*10)+d;
	}
	return v;
}
unsigned long long get_u64(char *s)
{
	int i,d=0;
	unsigned long long v=0;
	for(i=0;s[i]>='0' && s[i]<='9';i++){
		d=s[i]-'0';
		v=(v*10)+d;
	}
	return v;
}

/////////////////////////////////////////////////////////////////////////////////
// CONFIG
void get_task_info(char * str, struct task *task)
{
	char *s ,*s1;
	int i=0;
	s = s1=str;
	while(i<5){
		if(*s=='\t'){
			*s='\0';
			switch(i){
				case 0:
					
					task->task_id=get_u32(s1);
					s1=s+1;
					i++;
				break;
				case 1:
					task->exec=get_u64(s1);
					s1=s+1;
					i++;
				break;
				case 2:
					task->period=get_u64(s1);
					s1=s+1;
					i++;
					break;
				case 3:
					task->deadline=get_u64(s1);
					s1=s+1;
					i++;
				break;
				case 4:
					task->offset=get_u64(s1);
					s1=s+1;
					i++;
				break;
	
			}
			
		}
		s++;
	}
}



void get_taskset_config(char *file, unsigned int*njobs,unsigned int*ntasks,struct task *tasks)
{
	char buffer[BUF_SIZE];
	unsigned int i=0,n=0,j=0;
	FILE* fd  = fopen(file, "r");
	buffer[0]=0;
	*ntasks=0;
	*njobs=0;
	while( (fgets(buffer, BUF_SIZE, fd))!=NULL) {
		if(buffer[0]>='0' || buffer[0]<='9'){
			switch(n){
				case 0:
					j=get_u32(buffer);
					n++;
				break;
				case 1:
					get_task_info(buffer,&tasks[i]);	
					i++;
				break;
			}
		}
		buffer[0]=0;
	}
	if(j<=0 || i<=0){
		printf("Error: njobs parameter(%d) or ntasks parameter(%d) invalid\n",j,i);
		exit(0);
	}
	(*ntasks)=i;
	(*njobs)=j;
	fclose(fd);

}
unsigned char is_schedulable(struct task *tasks, int ntasks)
{
	double u=0.0;
	int i;
	for(i=0;i<ntasks;i++){
		u+=(double)(1.0*tasks[i].exec)/(1.0*tasks[i].deadline);
	}
	printf("U= %f\n",u);
	if(u>1.0){
		printf("Error: task set is not schedulable: %f\n",u);
		return 0;
	}
	return 1;
}
///////////////////////////////////////////////////////////////	
int main(int argc, char *argv[])
{
	pid_t pid_casio_tasks[NR_TASKS];
	struct task tasks[NR_TASKS];
	unsigned int njobs,ntasks;
	unsigned long long time0;
	char arg[7][30];
	int status,i;
///////////////////////////////////////////////////////////////
	if(argc!=2)
		exit(0);	
	get_taskset_config(argv[1], &njobs,&ntasks,tasks);
	print_tasks(tasks,ntasks);

	if(!is_schedulable(tasks,ntasks))
		exit(0);

	printf("LAUNCH: Cleaning ...\n");
	if(syscall(__NR_casio_task_release_clean)){
		printf("Error: __NR_task_release_clean\n");
		exit(0);
	}
	if(syscall(__NR_casio_task_ready_clean)){
		printf("Error: __NR_task_ready_clean\n");
		exit(0);
	}
	if(syscall(__NR_casio_trace_init)){
		printf("Error: __NR_casio_trace_init\n");
		exit(0);
	}

	if(syscall(__NR_casio_clock,&time0)){
		printf("Error: __NR_casio_clock\n");
		exit(0);
	}
	printf("LAUNCH: Forking tasks\n");
	for(i=0;i<ntasks;i++){
		sprintf(arg[0],"%d",tasks[i].task_id);	
		sprintf(arg[1],"%llu",tasks[i].exec);	
		sprintf(arg[2],"%llu",tasks[i].period);	
		sprintf(arg[3],"%llu",tasks[i].deadline);	
		sprintf(arg[4],"%llu",tasks[i].offset);	
		sprintf(arg[5],"%d",njobs);
		sprintf(arg[6],"%llu",time0);
		pid_casio_tasks[i]=fork();
		if(pid_casio_tasks[i]==0){
			execl("./casio_task","casio_task",arg[0],arg[1],arg[2],arg[3],arg[4],arg[5],arg[6],NULL);
			printf("Error: execv: casio_task\n");
			exit(0);
		}
		
	}
	printf("LAUNCH: Waiting ...\n");
	for(i=0;i<ntasks;i++){
		waitpid(0,&status,0);
		if(WIFEXITED(status)){
			printf("task:%d: finished\n",WEXITSTATUS(status));
		}	
	}
	printf("LAUNCH: Cleaning ...\n");
	if(syscall(__NR_casio_task_release_clean)){
		printf("Error: __NR_task_release_clean\n");
		exit(0);
	}
	if(syscall(__NR_casio_task_ready_clean)){
		printf("Error: __NR_task_ready_clean\n");
		exit(0);
	}
	return 0;
}

