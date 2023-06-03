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
#include <math.h>

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

void readPipe(int* pd,int pipeEnd, int pipe)
{
	if(!timeout)
  {
		close(pd[WRITE_END]);

		struct timeval curr_time;
        gettimeofday(&curr_time, NULL);

		int current_read_time_sec = (int)(curr_time.tv_sec - start_t.tv_sec);
		float current_read_time_msec = (float)(curr_time.tv_usec - start_t.tv_usec);

		if((current_read_time_msec < 0)) {
			current_read_time_sec -= 1;
		}
        
        read(pipeEnd, buffer, BUFFER_SIZE);
        if (pipe == 4)
        {
			    fprintf(output, "%d:%06.3f %s", current_read_time_sec, fabs(current_read_time_msec/1000.), buffer);
        } else
        {
       		fprintf(output, "%d:%06.3f %s\n", current_read_time_sec, fabs(current_read_time_msec/1000.), buffer);
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

	// Terminate
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

			struct timeval curr_time_ch;
			gettimeofday(&curr_time_ch, NULL);

			int current_read_time_sec_ch = (int)(curr_time_ch.tv_sec - start_t.tv_sec);
			float current_read_time_msec_ch = (float)(curr_time_ch.tv_usec - start_t.tv_usec);

			if((current_read_time_msec_ch < 0)) {
				current_read_time_sec_ch -= 1;
			}

			if(i == 4) 
      {
        	printf("Child 5 => ");
				fgets(terminal_input, TERMINAL_BUFFER_SIZE, stdin);
				snprintf(buffer, BUFFER_SIZE, "Child 5: %d:%06.3f %s", current_read_time_sec_ch, fabs(current_read_time_msec_ch/1000.), terminal_input);
				writePipe(fd[i]);
			}
			else 
      { 
				snprintf(buffer, BUFFER_SIZE, "Child %d: %d:%06.3f %d", i+1, current_read_time_sec_ch, fabs(current_read_time_msec_ch/1000.), message_count++);
				writePipe(fd[i]);
				sleep(rand() % 3);
				
			}
		} 

	}
   fclose(output);
exit(0);
}
