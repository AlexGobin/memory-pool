#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
//#include <iostream>
#include "ringbuffer.h"

#define	  FIFO_SIZE	16

//using namespace std;

void * consumer_proc(void *arg)
{
	unsigned int cnt;
	struct ringbuffer *ring_buf = (struct ringbuffer *)arg;

	cnt = 0;

	while(1)
	{
		sleep(2);
		printf("------------------------------------------\n");
		printf("get data from ring buffer.\n");
			
		{
			char i;
			
			if (ringbuffer_is_empty(ring_buf)) {
				printf("buffer is empty !\n");
				sleep(1);
				continue;
			}

			if (cnt !=0 && !(cnt % 16))
				printf("\n");

			ringbuffer_get(ring_buf, &i, sizeof(i));

			printf("data is: %d \n", i);

			cnt++;
		}


		printf("ring buffer length: %u\n", ringbuffer_len(ring_buf));
		printf("------------------------------------------\n");
	}
}

void * producer_proc(void *arg)
{
	struct ringbuffer *ring_buf = (struct ringbuffer *)arg;
	char i;

	i = 0;
	while(1)
	{
		printf("******************************************\n");
		printf("put datas to ring buffer.\n");
		

		if (ringbuffer_is_full(ring_buf)) {
			printf("buffer is full !\n");
			sleep(1);
			continue;
		}
		ringbuffer_put(ring_buf, &i, sizeof(i));
		i++;

		printf("ring buffer length: %u\n", ringbuffer_len(ring_buf));
		printf("******************************************\n");
		sleep(1);
	}
}


pthread_t consumer_thread(void *arg)
{
	int err;
	pthread_t tid;
	err = pthread_create(&tid, NULL, consumer_proc, arg);
	if (err != 0)
	{
		fprintf(stderr, "Failed to create consumer thread.errno:%u, reason:%s\n",
				errno, strerror(errno));
		return -1;
	}
	return tid;
}
pthread_t producer_thread(void *arg)
{
	int err;
	pthread_t tid;
	err = pthread_create(&tid, NULL, producer_proc, arg);
	if (err != 0)
	{
		fprintf(stderr, "Failed to create consumer thread.errno:%u, reason:%s\n",
				errno, strerror(errno));
		return -1;
	}
	return tid;
}



int main(void)
{
	struct ringbuffer *ring_buf;
	pthread_t produce_pid, consume_pid; 

	ring_buf = ringbuffer_create(FIFO_SIZE);
	if (!ring_buf) {
		perror("ringbuffer_create()");
		exit(1);
	}

	printf("multi thread test.......\n");

	produce_pid  = producer_thread((void*)ring_buf);
	consume_pid  = consumer_thread((void*)ring_buf);

	int i_result = pthread_join(produce_pid, NULL);
    if(i_result == EDEADLK){
        printf(" A deadlock was detected \n");
    }else if(i_result==EINVAL){
        printf(" thread is not a joinable thread or Another thread is already waiting to join with this thread\n");
    }else{
        printf(" No thread with the ID thread could be found \n");
    }
	pthread_join(consume_pid, NULL);

	ringbuffer_destroy(ring_buf);

	return 0;
}
