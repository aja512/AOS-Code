/*
// COEN 383 AOS Project 3
// File: main.c
*/

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "buyer.h"

typedef struct {
    int occupied;
    char seller_type;
    int seller_id;
    int customer_id;
    int priority;
    int turn_around;
    int response_time;
    int arrival_time;
} chart_struct;

typedef struct {
    customer_queue_t *queue;
    priority_t priority;
    char name[3];
    int id;
    int seller_id;
} seller_args_t;

chart_struct seats[10][10];

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_cond = PTHREAD_MUTEX_INITIALIZER;

int seat_count = 0;
int allocateSeat(seller_args_t *args, customer_t *current, int j, int k, char seller_type,
                 int startTime, int startTenTime, int startHour, int serviceTime) {
    int finish_sale;

    printf("[%d:%d%d] %c%d Serving customer %c%d%02d \n", startHour, startTenTime, startTime, seller_type, (*args).seller_id,
           seller_type, (*args).seller_id, (*current).id);
    seats[j][k].occupied = 1;
    seats[j][k].seller_type = seller_type;
    seats[j][k].customer_id = (*current).id;
    seats[j][k].seller_id = (*args).seller_id;
    seats[j][k].priority = (*args).priority;
    int completeStartTime  = startHour*60 + startTenTime*10 + startTime;
    seat_count++;
    //printf("completeStartTime: %d\n", completeStartTime);
    seats[j][k].turn_around = (completeStartTime + (serviceTime)) - current->arrival_time;
    seats[j][k].response_time = completeStartTime - current->arrival_time;
    seats[j][k].arrival_time = current->arrival_time;
    finish_sale = 1;
    int endHour = ((completeStartTime + serviceTime))/60;
    int endTime = (((completeStartTime + serviceTime))%60)%10;
    int endTenTime = (((completeStartTime + serviceTime))%60)/10;
    // print seat assignment of customer
    printf("[%d:%d%d] %c%d%02d customer assigned seat %d in row %d, service time of %d minutes\n\n", endHour, endTenTime, endTime,
           seller_type, (*args).seller_id, (*current).id, k, j, serviceTime);

    return finish_sale;
}

void print_seat_chart(chart_struct chart[][10]) {
    printf("\nSeating chart: \n");
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            if (chart[i][j].occupied == 0)
                printf("- ");
            else
                printf("%c%d%02d ", chart[i][j].seller_type, chart[i][j].seller_id, chart[i][j].customer_id);   // print each seat in matrix chart
        }
        printf("\n");
    }
    printf("\n");
}

void *seller(void *seller_args) {
    seller_args_t args = *((seller_args_t *) seller_args);
    //printf("{%s} id: %d, priority: %d\n", args.name, args.id, args.priority);

    // initialize seller's internal clock
    int sim_hour = 0;       // simulation clock hour value, 0
    int sim_tenmin = 0;     // simulation clock ten minutes value, 0-5
    int sim_onemin = 0;     // simulation clock one minute value, 0-9

    // Continue for the full hour
    for (int i=0; i<60; i++) {
        pthread_mutex_lock(&mutex_cond);
        pthread_cond_wait(&cond, &mutex_cond);       // make sure all seller threads are synchronized
        pthread_mutex_unlock(&mutex_cond);
        if (args.queue->buf[0].arrival_time <= i)    // handle customer that has arrived at front of queue
        {
            customer_t current = dequeue_customer(args.queue);
            sim_hour = (i / 60);
            int remainder_min = i % 60;
            sim_tenmin = remainder_min / 10;
            sim_onemin = remainder_min % 10;
            if (seat_count >= 100) {
                printf("[%d:%d%d] Concerts tickets sold out and %s%02d customer leaves \n", sim_hour,sim_tenmin, sim_onemin, args.name, current.id);
            } else {
                //high priority seller's functionality
                if (args.priority == HIGH_PRIORITY) {
                    //int time, char seller_type, int sellerId
                    print_customer(current, 'H', (args).seller_id);
                    int finish_sale = 0;
                    int service_time = (rand() % 2) + 1;  // 1-2 minutes to complete
                    //printf("\nservice_time: %d\n", service_time);
                    // look for seats starting from front row
                    for (int j = 0; j < 10; j++) {
                        for (int k = 0; k < 10; k++) {
                            // check if seat position is occupied, if not assign to customer - USE LOCK
                            ;
                            pthread_mutex_lock(&mutex);
                            if (seats[j][k].occupied != 1) {
                                finish_sale = allocateSeat(&args, &current, j, k, 'H', sim_onemin, sim_tenmin, sim_hour,
                                                           service_time);
                            }
                            pthread_mutex_unlock(&mutex);
                            if (finish_sale == 1) {
                                break;
                            }
                        }
                        if (finish_sale == 1) break;
                    }
                    i = i + (service_time - 1); // update timer
                }

                //medium priority seller's functionality
                if (args.priority == MEDIUM_PRIORITY) {
                    print_customer(current, 'M', (args).seller_id);
                    int finish_sale = 0;
                    int service_time = (rand() % 3) + 2;  // 2-4 minutes to complete
                    // printf("\nservice_time: %d\n", service_time);
                    // look for seats starting from middle rows
                    int counter = 1;
                    int assigned = 0;
                    for (int j = 4; j >= 0 && j < 10; counter++) {
                        for (int k = 0; k < 10; k++) {
                            pthread_mutex_lock(&mutex);
                            if (seats[j][k].occupied != 1) {
                                finish_sale = allocateSeat(&args, &current, j, k, 'M', sim_onemin, sim_tenmin, sim_hour,
                                                           service_time);
                            }
                            pthread_mutex_unlock(&mutex);
                            if (finish_sale == 1) {
                                break;
                            }
                        }
                        if (finish_sale == 1) break;

                        if (counter % 2 == 1) j = j + counter;  // 4->5->3->6->2->7->1->8->0->9 [Rows go from 0-9]
                        else j = j - counter;
                    }

                    i = i + (service_time - 1);// update timer
                }

                //low priority sellers's functionality
                if (args.priority == LOW_PRIORITY) {
                    print_customer(current, 'L', (args).seller_id);
                    int finish_sale = 0;
                    int service_time = (rand() % 4) + 4;  // 4-7 minutes to complete
                    //printf("\nservice_time: %d\n", service_time);
                    // look for seats starting from back row
                    for (int j = 9; j >= 0; j--) {
                        for (int k = 0; k < 10; k++) {
                            // check if seat position is occupied, if not assign to customer - USE LOCK
                            pthread_mutex_lock(&mutex);
                            if (seats[j][k].occupied != 1) {
                                finish_sale = allocateSeat(&args, &current, j, k, 'L', sim_onemin, sim_tenmin, sim_hour,
                                                           service_time);
                            }
                            pthread_mutex_unlock(&mutex);
                            if (finish_sale == 1) {
                                break;
                            }
                        }
                        if (finish_sale == 1) break;
                    }
                    i = i + (service_time - 1); // update timer
                }
            }
            if (args.queue->size == 0) break;   // end early if seller finishes handling all customers
          }
    }
    // handle customers that arrive after ticketing has ended
    while(size(args.queue) > 0) {
        customer_t current = dequeue_customer(args.queue);
        int hour = current.arrival_time / 60;
        int ten = current.arrival_time / 10;
        int min = current.arrival_time % 10;
        printf("%s Ticket sale ended, customer %s%02d that arrived at [%d:%d%d] leaves\n",args.name,args.name, current.id, hour,ten, min);
    }
    return NULL;
}

// starts all threads
void wakeup_all_seller_threads()
{
    // 10 broadcast messages are sent to unblock all seller threads
    for(int i = 0; i < 10; i++) {
        pthread_mutex_lock(&mutex_cond);
        pthread_cond_broadcast(&cond);      // signal that all waiting seller threads should continue
        pthread_mutex_unlock(&mutex_cond);
    }
}



int main(int argc, char *argv[]) 
{   
    srand(time(NULL));

    int N = atoi(argv[1]);  // argv[1]s has number customers each seller expects in an hour

    seller_args_t args[10];
    pthread_t threads[10];
       // seats matrix
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            seats[i][j].occupied = 0;   // initialize seats matrix to set all seats empty
        }
    }
    print_seat_chart(seats);

    for (int i = 0; i < 10; i++)    //Initialize arguments for each seller queue first
    {
        if (i == 0) 
        {
            // create customer queues for high-price seller
            customer_queue_t *queue = create_customer_queue(N);
            
            // print queue
            printf("Initial H queue:  ");
            print_customer_queue(queue);

            args[i] = (seller_args_t) {queue, HIGH_PRIORITY, {0}, i, i};
            sprintf(args[i].name, "H");
        } 
        else if (i > 0 && i < 4) 
        {
            // create customer queues for med-price sellers
            customer_queue_t *queue = create_customer_queue(N);

            // print queue
            printf("Initial M%d queue: ", i);
            print_customer_queue(queue);

            args[i] = (seller_args_t) {queue, MEDIUM_PRIORITY, {0}, i, i};
            sprintf(args[i].name, "M%d", i);
        } 
        else 
        {
            // create customer queues for low-price seller
            customer_queue_t *queue = create_customer_queue(N);

            // print queue
            printf("Initial L%d queue: ", i-3);
            print_customer_queue(queue);

            args[i] = (seller_args_t) {queue, LOW_PRIORITY, {0}, i,  i - 3};
            sprintf(args[i].name, "L%d", i - 3);
        }
    }
    printf("\n");

    // initialize thread for all sellers
    for (int i = 0; i < 10; i++) {
        if (pthread_create(&threads[i], NULL, seller, &args[i])) 
        {
            printf("\n Thread Creation Error!!! \n");
        }
    }

    int timerComplete = 0;
    
    // This is the logic to achieve(attempt) closer to synchronized/parallel execution of ticket allocation by seller threads.  
    // 600 is just a upper bound to ensure that we have enough condition broadcast messages. (More broadcast messages than waits should not be a concern)
    // (Enhancement could be to find better tight bound)
    // All broadcast messages are triggered 10 at a time to ensure that we have enough broadcast messages to unblock all 10 seller threads. 
    // All seller threads are expected to start at the same time with 10 broadcast messages
    
    while(timerComplete <= 600)
    {
        wakeup_all_seller_threads();
        usleep(30);         // add delay before trying to wake up threads again (assumption is that sellers can execute a logic to allocate a seat in 30 micro sec)
        timerComplete++;     
    }

    // wait for all seller threads to exit  
    for (int i = 0; i < 10; i++) 
    {
        if (pthread_join(threads[i], NULL)) 
        {
            printf("\n Thread is not joining. Error!!! \n");
        }
    }
    print_seat_chart(seats);

    // calculate statistics
    int hTotalResponseTime = 0;
    int hThroughPut = 0;
    int hTurnAroundInput = 0;

    int mTotalResponseTime = 0;
    int mThroughPut = 0;
    int mTurnAroundInput = 0;

    int lTotalResponseTime = 0;
    int lThroughPut = 0;
    int lTurnAroundInput = 0;

    for(int i =0; i< 10; i++) {
        for (int j = 0; j < 10; j++) {
            if (seats[i][j].occupied == 1 ) {
                if(seats[i][j].seller_type == 'H') {
                    hTotalResponseTime += seats[i][j].response_time;
                    hThroughPut++;
                    hTurnAroundInput += seats[i][j].turn_around;
                } else if(seats[i][j].seller_type == 'M') {
                    mTotalResponseTime += seats[i][j].response_time;
                    mThroughPut++;
                    mTurnAroundInput += seats[i][j].turn_around;
                } else {
                    lTotalResponseTime += seats[i][j].response_time;
                    lThroughPut++;
                    lTurnAroundInput += seats[i][j].turn_around;
                }
            }
        }
    }

    printf("\nStatistics for high-priced sellers:");
    printf("\nTurnaround Time: %d", hTurnAroundInput);
    printf("\nTotal Response Time: %d", hTotalResponseTime);
    printf("\nAverage Response Time: %f", (float) hTotalResponseTime/hThroughPut);
    printf("\nThroughput: %d", (hThroughPut));
    printf("\nAverage Turnaround Time: %f", (float) hTurnAroundInput/hThroughPut);

    printf("\n\nStatistics for medium-priced sellers:");
    printf("\nTurnaround Time: %d", mTurnAroundInput);
    printf("\nTotal Response Time: %d", mTotalResponseTime);
    printf("\nAverage Response Time: %f", (float) mTotalResponseTime/mThroughPut);
    printf("\nThroughput: %d", (mThroughPut));
    printf("\nAverage Turnaround Time: %f", (float) mTurnAroundInput/mThroughPut);

    printf("\n\nStatistics for low-priced sellers:");
    printf("\nTurnaround Time: %d", lTurnAroundInput);
    printf("\nTotal Response Time: %d", lTotalResponseTime);
    printf("\nAverage Response Time: %f", (float) lTotalResponseTime/lThroughPut);
    printf("\nThroughput: %d", (lThroughPut));
    printf("\nAverage Turnaround Time: %f", (float) lTurnAroundInput/lThroughPut);
}
