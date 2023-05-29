/*
// COEN 383 Project 6
// File: pipe.c
*/

//Importing Libraries
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <assert.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>

// Setting the limits
#define BUFFER_SIZE		100
#define TERMINAL_BUFFER_SIZE    80
#define READ_END		0
#define WRITE_END		1
#define NUM_PIPES		5
#define PIPE_DURAITON	30

fd_set input, input_fd;
FILE* output;
char buffer[BUFFER_SIZE];
char terminal_input[TERMINAL_BUFFER_SIZE];
int fd[NUM_PIPES][2];
int timeout = 0;
int message_count = 1;
struct timeval start_t;
struct itimerval timer;
time_t start;

int timeval_subtract (struct timeval *result, struct timeval *x,struct timeval  *y)  
{  
	if (x->tv_usec < y->tv_usec) {  
		int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;  
		y->tv_usec -= 1000000 * nsec;  
		y->tv_sec += nsec;  
	}  
	if (x->tv_usec - y->tv_usec > 1000000) {  
		int nsec = (y->tv_usec - x->tv_usec) / 1000000;  
		y->tv_usec += 1000000 * nsec;  
		y->tv_sec -= nsec;  
	}  
	 
	result->tv_sec = x->tv_sec - y->tv_sec;  
	result->tv_usec = x->tv_usec - y->tv_usec;  
	
	return x->tv_sec < y->tv_sec;  
}

void readPipe(int* pd,int pipeEnd, int pipe)
{
	if(!timeout)
  {
		close(pd[WRITE_END]);

		struct timeval curr_time;
        gettimeofday(&curr_time, NULL);

		struct timeval diff;
		int result;
		result = timeval_subtract(&diff, &curr_time, &start_t);

		int current_read_time_sec = (int)(diff.tv_sec);
		float current_read_time_msec = (float)(diff.tv_usec/1000.);
        
        read(pipeEnd, buffer, BUFFER_SIZE);
        if (pipe == 4)
        {
			    fprintf(output, "%d:%06.3f %s", current_read_time_sec, current_read_time_msec, buffer);
        } else
        {
       		fprintf(output, "%d:%06.3f %s\n", current_read_time_sec, current_read_time_msec, buffer);
        }
	}
}

void writePipe(int* pd)
{
	if(!timeout)
  {
		close(pd[READ_END]);
		write(pd[WRITE_END], buffer, BUFFER_SIZE);
	}
}

void interruptHandler(int signal)
{
	assert(signal == SIGALRM);
	timeout = 1;
	exit(0);
}

int main()
{
	output = fopen("output.txt", "w");

	time(&start); //get start time
	timer.it_value.tv_sec = PIPE_DURAITON;	//30 second timeout
	timer.it_value.tv_usec = 0;
	setitimer(ITIMER_REAL, &timer, NULL);
	gettimeofday(&start_t, NULL);
	signal(SIGALRM, interruptHandler);
	srand(time(NULL));
  int seed;

	FD_ZERO(&input_fd);

	int i, pipeNumber;
	pid_t pid;

	for(i = 0; i < NUM_PIPES; i++)
  {
    seed = rand();
		if(pipe(fd[i]) == -1) 
    {
			perror("Pipe error");
			exit(1);
		}
		
		FD_SET(fd[i][READ_END], &input_fd);
		
		pid = fork();
		if(pid  == -1) 
    {
			perror("Fork error");
			exit(1);
		}

		if(pid == 0) 
    {
			fflush(stdout);
            // different child now has different random seed
            srand(seed);
			break;
		}

	}

	while(!timeout)
  {

	//Parent Process
	if(pid > 0)
    {
			input = input_fd;

			pipeNumber = select(FD_SETSIZE, &input, NULL, NULL, NULL);
			if(pipeNumber < 0)
      {
				perror("Pipe Number error");
				exit(1);
			} else if (pipeNumber == 0)
      {
                // should not be here, since the timeout parameter is NULL
				perror("Nothing to read");
			} else
      {
				for(i = 0; i < NUM_PIPES; i++)
        {
					if(FD_ISSET(fd[i][READ_END], &input)) 
          {
						readPipe(fd[i],fd[i][READ_END], i);
					}
				}
			}
		} else
    { // Child Process
			input = input_fd;

			if(i == 4) 
      {
        	printf("Child 5 => ");
				fgets(terminal_input, TERMINAL_BUFFER_SIZE, stdin);
				snprintf(buffer, BUFFER_SIZE, "Child 5: %s", terminal_input);
				writePipe(fd[i]);
			}
			else 
      { 
				snprintf(buffer, BUFFER_SIZE, "Child %d: %d", i+1, message_count++);
				writePipe(fd[i]);
				sleep(rand() % 3);
				
			}
		} 

	}
   fclose(output);
exit(0);
}
