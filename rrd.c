#include "rrd.h"

int main(int argc, char*argv[]){

	FILE * input_list_stream = NULL;
	PcbPtr rrd_queue = NULL;
	PcbPtr job_queue = NULL;
	PcbPtr current_process = NULL;
	PcbPtr process = NULL;
	int timer = 0;
	int time_quantum = 0;
	int quantum = 1;
	int current_running_time =0;

	if (argc <= 0)
    {
        fprintf(stderr, "FATAL: Bad arguments array\n");
        exit(EXIT_FAILURE);
    }
    else if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <TESTFILE>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (!(input_list_stream = fopen(argv[1], "r")))
    {
        fprintf(stderr, "ERROR: Could not open \"%s\"\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    // fill job dispatch from file
    while(!feof(input_list_stream)){
    	process = createnullPcb();
        if (fscanf(input_list_stream,"%d, %d",
             &(process->arrival_time), 
             &(process->service_time)) != 2) {
            free(process);
            continue;
        }
        process->remaining_cpu_time = process->service_time;
        process->status = PCB_INITIALIZED;
        job_queue = enqPcb(job_queue,process);
    }
    // enter an integer for time quantum
    printf("Please enter the value of time_quantum: ");
    scanf("%d", &time_quantum);
	//While there is a currently running process or either queue is not empty
    while(current_process || rrd_queue || job_queue){
    	// i.Unload any arrived pending processes from the Job Dispatch queue
		// dequeue process from Job Dispatch queue and enqueue on Round Robin
		// queue;
		PcbPtr temp = job_queue;
		while(job_queue){
			if(job_queue->arrival_time <= timer){
				rrd_queue = enqPcb(rrd_queue,deqPcb(&job_queue));
			}
			else{
				break;	
			}
		}

		// ii. If a process is currently running
		if(current_process){
			//a. Decrease process remaining_cpu_time by quantum;
			current_process->remaining_cpu_time--;
			current_running_time++;
			// printf("current_running_time: %d\n",current_running_time );
			// printf("remaining_cpu_time: %d\n",current_process->remaining_cpu_time);
			//b. If times up
			if(current_process->remaining_cpu_time<1){
				// A. terminate the process
				terminatePcb(current_process);
				// B. Deallocate the PCB
				free(current_process);
				current_process = NULL;
				current_running_time=0;

			}
			// out of the time allowed for one process
			else if(current_running_time>=time_quantum && rrd_queue){
				printf("here");
			
				suspendPcb(current_process);
				rrd_queue = enqPcb(rrd_queue,current_process);
				current_process = NULL;
				current_running_time = 0;
				
			}
			
		}
		// iii. If no process is currently running and Round Robin queue is not empty
		if(!current_process && rrd_queue){
			current_process = deqPcb(&rrd_queue);
			if(current_process->status == PCB_SUSPENDED){
				kill(current_process->pid, SIGCONT);
			}
			else{
				startPcb(current_process);
			}
			current_running_time=0;

		}
		sleep(quantum);
		timer++;
    }

    exit(EXIT_SUCCESS);












}