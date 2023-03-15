#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
 
#define SIZE 5
#define NUMB_THREADS 6
#define buffer_index 2

//stuct for myStruct
struct myStruct{
    int val;  //value for counting myStruct
    sem_t gate;   //BSem
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
    sem_wait(&(myStruct->gate));
    sem_wait(&(myStruct->mutex));
    myStruct->val--;
    if(myStruct->val > 0){ 
        sem_post(&(myStruct->gate));    
    }
     sem_post(&(myStruct->mutex));
}


//function to initilize, takes pointer of myStruct and int
void initCSem(struct myStruct *myStruct, int k){
    myStruct->val=k;  //set value
    int min = 1;
    if(myStruct->val < 1)
    {
	min = myStruct->val;
    }
    sem_init(&(myStruct->gate), 0, min);
    sem_init(&(myStruct->mutex), 0, 1);
}   

//POST FUNCTION
void V(struct myStruct *myStruct){
  
    sem_wait(&(myStruct->mutex));
    myStruct->val++;
    if(myStruct->val == 1) 
        sem_post(&(myStruct->gate));   
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
        return buffer[--buffer_index]; // buffer_index-- would be error!
    } else {
        printf("Buffer underflow\n");
    }
    return 0;
}
 

void *producer(void *thread_n) {
	int thread_numb = *(int *)thread_n;
	     //loop for SIZE
    while(1 && buffer_index<SIZE+1){
       //sleep declared as 1;
        sleep(1);
        //semaphore opeartion when empty
        sem_wait(&empty_sem);
        pthread_mutex_lock(&buffer_mutex);
        //generting item value between 1 to 100
        int item = rand()%100;
        //buffer intialisation and index increment
        buffer[buffer_index++] = item;
        //buffer_index increases to SIZE while reaches SIZE
        buffer_index++;
        printf("Producer %d added %d to buffer\n", thread_numb, item);
        pthread_mutex_unlock(&buffer_mutex);
        sem_post(&full_sem);
    }
    return NULL;

}

 
void *consumer(void *thread_n) {
    int thread_numb = *(int *)thread_n;
    while(1 && buffer_index<SIZE+1){
        sleep(1);
        sem_wait(&full_sem);
        pthread_mutex_lock(&buffer_mutex);
        //decrement index and buffer indexdecrement
        int item = buffer[--buffer_index];
       printf("Consumer %d dequeue %d from buffer\n", thread_numb, item);
        
        pthread_mutex_unlock(&buffer_mutex);
        sem_post(&empty_sem);
    }
    return NULL;

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
    sem_destroy(&(myStruct.gate));
    sem_destroy(&(myStruct.mutex));
 
    return 0;
}
