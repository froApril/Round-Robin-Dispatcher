#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/times.h>
#include <limits.h>
#include <sys/resource.h>


int main() {
	char*array[2];
	array[0] = "./process";
	array[1] = NULL;

    pid_t pid;
    pid = fork();
    if(pid  == 0) //返回子进程
    {	
    	execvp(array[0],array);
    } else {
    	sleep(10);

    	if(kill(pid,SIGINT)){
        	fprintf(stderr, "terminate of %d failed",(int)pid );
        }
    }
}