#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
 
#define SIZE 5
#define NUMB_THREADS 6
#define PRODUCER_LOOPS 2

//stuct for Couting semaphore
struct myStruct{
    int val;  //value for counting myStruct
    sem_t wait;   //BSem
    sem_t mutex;  //Bsem
};

struct myStruct myStruct; 

typedef int buffer_t;
buffer_t buffer[SIZE];
int buffer_index;
 
pthread_mutex_t buffer_mutex;

struct myStruct full_sem;  
struct myStruct empty_sem;


//WAIT FUNCTION
void P(struct myStruct *myStruct){
    sem_wait(&(myStruct->mutex));
    myStruct->val--;
    if(myStruct->val < 0){ 
        sem_post(&(myStruct->mutex));
        sem_wait(&(myStruct->wait));    
    }
    else
        sem_post(&(myStruct->mutex));
}


//function to initilize, takes pointer of myStruct and int
void initCSem(struct myStruct *myStruct, int k){
    myStruct->val=k;  //set value
    sem_init(&(myStruct->wait), 0, 0);
    sem_init(&(myStruct->mutex), 0, 1);
}   

//POST FUNCTION
void V(struct myStruct *myStruct){
    
    sem_wait(&(myStruct->mutex));
    myStruct->val++;
    if(myStruct->val <= 0) 
        sem_post(&(myStruct->wait));   
    sem_post(&(myStruct->mutex));
}

 
void insertbuffer(buffer_t value) {
    if (buffer_index < SIZE) {
        buffer[buffer_index++] = value;
    } else {
        printf("Buffer overflow\n");
    }
}
 
buffer_t dequeuebuffer() {
    if (buffer_index > 0) {
        return buffer[--buffer_index]; 
    } else {
        printf("Buffer underflow\n");
    }
    return 0;
}
 
 
void *producer(void *thread_n) {
    int thread_numb = *(int *)thread_n;
    buffer_t value;
    int i=0;
    while (i++ < PRODUCER_LOOPS) {
        sleep(rand() % 10);
        value = rand() % 100;
        P(&full_sem);
        pthread_mutex_lock(&buffer_mutex);
        insertbuffer(value);
        pthread_mutex_unlock(&buffer_mutex);
        V(&empty_sem);
        printf("Producer %d added %d to buffer\n", thread_numb, value);
    }
    pthread_exit(0);
}
 
void *consumer(void *thread_n) {
    int thread_numb = *(int *)thread_n;
    buffer_t value;
    int i=0;
    while (i++ < PRODUCER_LOOPS) {
        P(&empty_sem);
        pthread_mutex_lock(&buffer_mutex);
        value = dequeuebuffer(value);
        pthread_mutex_unlock(&buffer_mutex);
        V(&full_sem);
        printf("Consumer %d dequeue %d from buffer\n", thread_numb, value);
   }
    pthread_exit(0);
}
 
int main(int argc, int **argv) {
    buffer_index = 0;
   
 
    pthread_mutex_init(&buffer_mutex, NULL);
    pthread_mutex_init(&buffer_mutex, NULL);
    
    initCSem(&full_sem, 5);
    initCSem(&empty_sem, 0);

    pthread_t thread[NUMB_THREADS];
    int thread_numb[NUMB_THREADS];
    int i;
    for (i = 0; i < NUMB_THREADS; ) {
        thread_numb[i] = i;
        pthread_create(thread + i, // pthread_t *t
                       NULL, // const pthread_attr_t *attr
                       producer, // void *(*start_routine) (void *)
                       thread_numb + i);  // void *arg
        i++;
        thread_numb[i] = i;
        // playing a bit with thread and thread_numb pointers...
        pthread_create(&thread[i], // pthread_t *t
                       NULL, // const pthread_attr_t *attr
                       consumer, // void *(*start_routine) (void *)
                       &thread_numb[i]);  // void *arg
        i++;
    }
 
    for (i = 0; i < NUMB_THREADS; i++)
        pthread_join(thread[i], NULL);
 
    pthread_mutex_destroy(&buffer_mutex);
    sem_destroy(&(myStruct.wait));
    sem_destroy(&(myStruct.mutex));
 
    return 0;
}
