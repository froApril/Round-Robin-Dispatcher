#include "rrd.h"

int main(int argc, char*argv[]){

	FILE * input_list_stream = NULL;
	PcbPtr rrd_queue = NULL;
	PcbPtr job_queue = NULL;
	PcbPtr current_process = NULL;
	PcbPtr process = NULL;
	int timer = 0;
	int time_quantum = 0;
	int quantum = 0;
	int current_running_time =0;
	double total_turnaround_time=0;
	double total_waiting_time=0;
	double process_num = 0;

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


    printf("COMP3520 assignment2 part 1.\n");
    printf("SID: 470014208, unikey: yixu5881\n");



    // fill job dispatch from file
    while(!feof(input_list_stream)){
    	process = createnullPcb();
    	process_num++;
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
			current_process->remaining_cpu_time-=quantum;
			//b. If times up
			if(current_process->remaining_cpu_time==0){
				// A. terminate the process
				terminatePcb(current_process);
				// B. record Turnaround time
				total_turnaround_time+=(timer-current_process->arrival_time);
				// C. calculate waiting time
				total_waiting_time += (timer-current_process->arrival_time-current_process->service_time);
				// D. Deallocate the PCB
				free(current_process);
				current_process = NULL;

			}
			// out of the time allowed for one process
			else if(rrd_queue){			
				suspendPcb(current_process);
				rrd_queue = enqPcb(rrd_queue,current_process);
				current_process = NULL;
			}
			
		}
		// iii. If no process is currently running and Round Robin queue is not empty
		if(!current_process && rrd_queue){
			current_process = deqPcb(&rrd_queue);
			if(current_process->status == PCB_SUSPENDED){
				// restartPcb(current_process);
				printPcbHdr();
				current_process->status = PCB_RUNNING;
				printPcb(current_process);
				kill(current_process->pid, SIGCONT);
			}
			else{
				startPcb(current_process);
			}

		}

		if(current_process){
			if(current_process->remaining_cpu_time<=time_quantum){
				timer+=current_process->remaining_cpu_time;
				quantum = current_process->remaining_cpu_time;
			}
			else{
				timer+=time_quantum;
				quantum = time_quantum;
			}
			sleep(quantum);
		}
		else{
			timer++;
		}
		
    }
    printf("The average turnaround time is: %.2f\n", (total_turnaround_time/process_num));
    printf("The average waiting time is: %.2f\n", (total_waiting_time/process_num) );
    exit(EXIT_SUCCESS);












}