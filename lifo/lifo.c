#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
 
#define SIZE 30


int shmid, shmid2, shmid3, shmid4, shmid5, shmid6, shmid7, shmid8, shmid9, shmid10, shmid11;
float *timeresul, *timeresult;
float *timetotal, *timetotal1;



typedef int buffer_t;
struct CSem {
    sem_t mutex;
    sem_t wait; 
    int val;
};
struct printReq {
	int pid;
	int JobSize;
	float beginnig;
	float finis;
};
struct CSem *empty_sem, *full_sem, *empty_sem1, *full_sem1;
struct printReq processid;
pthread_mutex_t *buffer_mutex, *buffer_mutex1;
struct printReq *buffer, *buffer1, *buffer2;
int *buffer_index, *buffer_index1, *counter, *count1, *kill_flag, k = 0;
struct timeval start, end, *start1, *start2, *end1, *end2, end3, start3, end4, start4;


int ThreadSizeProc[20] = {0};
int ThreadJobProc[20] = {0};
int childpid[100];
pthread_t threadid[100];
int num_proc, num_thread, JobsComplete = 0;


key_t key = 5311;
key_t key2 = 5312;
key_t key3 = 5313;
key_t key4 = 5314;
key_t key5 = 5315;
key_t key6 = 5316;
key_t key7 = 5317;
key_t key8 = 5318;
key_t key9 = 5319;
key_t key10 = 5320;
key_t key11 = 5321;



/* when 0, buffer is empty. */
void insertbuffer(struct printReq cs) {
	buffer = buffer1;
	buffer_index = buffer_index1;
	buffer_mutex = buffer_mutex1;
	full_sem = full_sem1;
	empty_sem = empty_sem1;
    if (*buffer_index < SIZE) {
        buffer = buffer + *buffer_index;
		(*buffer).JobSize = cs.JobSize;
		(*buffer).pid = cs.pid;
		(*buffer).beginnig = cs.beginnig;
		*buffer_index1 = *buffer_index1 + 1;} 
    else {
        printf("Buffer overflow\n");}
}
 
struct printReq dequeuebuffer() {
	 buffer = buffer1;
	 buffer_index = buffer_index1;
	 buffer_mutex = buffer_mutex1;
	 full_sem = full_sem1;
	 empty_sem = empty_sem1;
	 
    if (*buffer_index > 0) {
		if (*buffer_index > 1) {
		for (int s = 1; s < *buffer_index; s++) {
		buffer++;
		}
		buffer2 = buffer;
		for (int s = 1; s < *buffer_index; s++) {
		buffer--;
		}
		*buffer_index = *buffer_index - 1;
        return *buffer2;
		
		}
		*buffer_index = *buffer_index - 1;
		buffer = buffer - *buffer_index1;
        return *buffer; 
    }
    else {
        printf("Buffer underflow\n");
    }
}
void deathtoall(){
	
    for (int i=0; i < num_proc; i++){
        kill(childpid[i],SIGKILL);
    }    
	for (int j = 0; j < num_thread; j++) {
		pthread_cancel(threadid[j]);
	}
} 
void initiate_sem(){
	 buffer = buffer1;
	 buffer_index = buffer_index1;
	 buffer_mutex = buffer_mutex1;
	 full_sem = full_sem1;
	 empty_sem = empty_sem1;
    empty_sem->val = 0;
    full_sem->val = SIZE;
    sem_init(&empty_sem->mutex, 1, 1);
    sem_init(&full_sem->mutex, 1, 1);
    if(empty_sem->val > 0) {sem_init(&empty_sem->wait, 1, 1);}
    else {sem_init(&empty_sem->wait, 1, 0);}

    if(full_sem->val > 0) {sem_init(&full_sem->wait, 1, 1);}
    else {sem_init(&full_sem->wait, 1, 0);}
}

void Pc(struct CSem *semaphore){
    sem_wait(&semaphore->wait);
    sem_wait(&semaphore->mutex);
    semaphore->val--;
    if(semaphore->val > 0){
        sem_post(&semaphore->wait);
    }
    sem_post(&semaphore->mutex);
}
void Vc(struct CSem *semaphore){
    sem_wait(&semaphore->mutex);
    semaphore->val++;
    if(semaphore->val == 1){
        sem_post(&semaphore->wait);
    }
    sem_post(&semaphore->mutex);
}

void producer(void *thread_n) {
     timeresult = timeresul;
	 buffer = buffer1;
	 buffer_index = buffer_index1;
	 buffer_mutex = buffer_mutex1;
	 full_sem = full_sem1;  
	 counter = count1;
	 empty_sem = empty_sem1;
	 start2 = start1;
	 end1 = end2;
	 int ThreadNum = *(int *)thread_n;
	 buffer_t val;
	 int i = 0;
     srand(time(NULL));
	 int num_jobs = (rand() % (30 - 1 + 1)) + 1;
	 *counter = *counter + num_jobs;
     while ((i++ < num_jobs))
	 {
		 val = (rand() % (1000 - 100 + 1)) + 100;
         usleep(val*1000);
		 processid.pid = ThreadNum;
		 processid.JobSize = val;
		 gettimeofday(&start3, NULL);
		 Pc(full_sem);
		 gettimeofday(&end3, NULL);
		 processid.beginnig = ((double)end3.tv_sec+0.000001*(double)end3.tv_usec) - ((double)start3.tv_sec+0.000001*(double)start3.tv_usec);
		 pthread_mutex_lock(buffer_mutex); /* protecting critical section */
		 insertbuffer(processid);
		 *timeresult = processid.beginnig + *timeresult;
		 printf("Producer %d added job size %d to buffer\n", ThreadNum, val);
		 pthread_mutex_unlock(buffer_mutex);
		 Vc(empty_sem);
    }
}

void *consumer(void *thread_n) {
	 timetotal = timetotal1;
	 buffer = buffer1;
	 buffer_index = buffer_index1;
	 buffer_mutex = buffer_mutex1;
	 full_sem = full_sem1;
	 empty_sem = empty_sem1;
	 start2 = start1;
	 end1 = end2;
     counter = count1;
    int ThreadNum = *(int *)thread_n;
	threadid[ThreadNum] = pthread_self();
    while (1) {
        usleep(100000);
		gettimeofday(&start4, NULL);
        Pc(empty_sem);
		gettimeofday(&end4, NULL);
        
        pthread_mutex_lock(buffer_mutex);
        processid = dequeuebuffer();
		float timeee = ((double)end4.tv_sec+0.000001*(double)end4.tv_usec) - ((double)start4.tv_sec+0.000001*(double)start4.tv_usec);
		*timetotal = *timetotal + timeee;
        printf("Consumer %d dequeued process %d, job size %d from buffer\n", ThreadNum, processid.pid, processid.JobSize);
       
        ThreadSizeProc[ThreadNum] += processid.JobSize;
        ThreadJobProc[ThreadNum] += 1;
        JobsComplete += 1;
        pthread_mutex_unlock(buffer_mutex);
        Vc(full_sem); // post (increment) fullbuffer semaphore
        if (JobsComplete == *counter){deathtoall();}
    }
}

void signal_handler(int sig){
    printf("\nRecieved SIGINT signal!\n");
    *kill_flag = 1;
	
    deathtoall();
    shmctl(shmid, IPC_RMID, NULL);
    shmctl(shmid2, IPC_RMID, NULL);
    shmctl(shmid3, IPC_RMID, NULL);
    shmctl(shmid4, IPC_RMID, NULL);
    shmctl(shmid5, IPC_RMID, NULL);
    shmctl(shmid6, IPC_RMID, NULL);
    shmctl(shmid7, IPC_RMID, NULL);
    exit(sig);
}
/*
main function. It takes in command line arguments */
int main(int argc, char *argv[]) {
    
    /*checks command line inputs*/
    if(argc != 3){
        printf("You ran the program in the incorrect format...\n");
        printf("The program requirese two In parameters for desired processes and threads. (i.e. './prog ## ##')\n");
        return 0;
    }
    else{
        num_proc = atoi(argv[1]);
	    num_thread = atoi(argv[2]);
    }
    int i;
	pthread_t thread[num_thread];
	int ThreadNum[num_thread];

    //Code to redirect ^C signal to customer defined sig handler
	signal(SIGINT, signal_handler);

    // creates shared memory
	if ((shmid = shmget(key, sizeof(full_sem1), IPC_CREAT | 0666)) < 0){   
        perror("shmget");
		exit(1);}
	if( (full_sem1 = shmat(shmid, NULL, 0)) == (void *) -1 ){
        perror("shmat");
        exit(1);}
    if ((shmid7 = shmget(key7, sizeof(kill_flag), IPC_CREAT | 0666)) < 0){   
        perror("shmget");
		exit(1);}
	if( (kill_flag = shmat(shmid7, NULL, 0)) == (void *) -1 ){
        perror("shmat");
        exit(1);}
	if ((shmid2 = shmget(key2, sizeof(empty_sem1), IPC_CREAT | 0666)) < 0){
         perror("shmget");
         exit(1);}
	 if( (empty_sem1 = shmat(shmid2, NULL, 0)) == (void *) -1 ){
         perror("shmat");
         exit(1);}
	 if ((shmid3 = shmget(key3, sizeof(buffer_mutex1), IPC_CREAT | 0666)) < 0){
         perror("shmget");
         exit(1);}
	 if( (buffer_mutex1 = shmat(shmid3, NULL, 0)) == (void *) -1 ){
         perror("shmat");
         exit(1);}
	 if ((shmid4 = shmget(key4, sizeof(buffer_index1), IPC_CREAT | 0666)) < 0){
         perror("shmget");
         exit(1);}
	 if( (buffer_index1 = shmat(shmid4, NULL, 0)) == (void *) -1 ){
         perror("shmat");
         exit(1);}
	 if ((shmid5 = shmget(key5, sizeof(buffer1), IPC_CREAT | 0666)) < 0){
         perror("shmget");
         exit(1);}
	 if( (buffer1 = shmat(shmid5, NULL, 0)) == (void *) -1 ){
         perror("shmat");
         exit(1);}
     if ((shmid6 = shmget(key6, sizeof(counter), IPC_CREAT | 0666)) < 0){
         perror("shmget");
         exit(1);}
	 if( (count1 = shmat(shmid6, NULL, 0)) == (void *) -1 ){
         perror("shmat");
         exit(1);}
	 if ((shmid8 = shmget(key8, sizeof(start1), IPC_CREAT | 0666)) < 0){
         perror("shmget");
         exit(1);}
	 if( (start1 = shmat(shmid8, NULL, 0)) == (void *) -1 ){
         perror("shmat");
         exit(1);}
	 if ((shmid9 = shmget(key9, sizeof(end2), IPC_CREAT | 0666)) < 0){
         perror("shmget");
         exit(1);}
	 if( (end2 = shmat(shmid9, NULL, 0)) == (void *) -1 ){
         perror("shmat");
         exit(1);}
	 if ((shmid10 = shmget(key10, sizeof(end2), IPC_CREAT | 0666)) < 0){
         perror("shmget");
         exit(1);}
	 if( (timeresul = shmat(shmid10, NULL, 0)) == (void *) -1 ){
         perror("shmat");
         exit(1);}
	 if ((shmid11 = shmget(key11, sizeof(end2), IPC_CREAT | 0666)) < 0){
         perror("shmget");
         exit(1);}
	 if( (timetotal1 = shmat(shmid11, NULL, 0)) == (void *) -1 ){
         perror("shmat");
         exit(1);}
	
     timetotal = timetotal1;
	 timeresult = timeresul;
	 buffer = buffer1;
	 buffer_index = buffer_index1;
	 buffer_mutex = buffer_mutex1;
	 full_sem = full_sem1;
	 empty_sem = empty_sem1;
	 start2 = start1;
     counter = count1;
	 end1 = end2;
	 *counter = 0;     
     *buffer_index = 0;
	 *timeresult = 0;
	 *timetotal = 0;

     gettimeofday(&start, NULL);
	 pthread_mutex_init(buffer_mutex, NULL);
	 initiate_sem();

     
	 
	 
	 
	 for (i = 0; i < num_thread;){
		 ThreadNum[i] = i;
		 
		 pthread_create(&thread[i],		  // pthread_t *t
						NULL,			  // const pthread_attr_t *attr
						consumer,		  // void *(*start_routine) (void *)
						&ThreadNum[i]); // void *arg
		 i++;
         //sleep(1);
    }
	
	for (i = 0; i < num_proc;i++ ) { 
		if ((childpid[i] = fork()) == 0) {
		    producer(&i);
			exit(0);}
		else {sleep(1);}
	 }
    for (i = 0; i < num_thread; i++){
        pthread_join(thread[i], NULL);
    }
	int condition = 0;
    int tempjobcount;
    while(condition == 0){
        tempjobcount = 0;
        for (i = 0; i < num_thread; i++){
            tempjobcount += ThreadJobProc[i];
        }
        if (tempjobcount == *counter){
            condition = 1;
        }
    }
	
	printf("RESULTS: %f, %f \n", *timeresult, *timetotal);
	float arrayoftimes = 0.0;
	float var1 = *timeresult;
	float var2 = *timetotal;
	arrayoftimes = var1 + var2;
	
	arrayoftimes = (arrayoftimes/JobsComplete) * 1000;
	printf("Average waiting time: %fms\n", arrayoftimes);
	
	gettimeofday(&end, NULL);
    printf ("Total execution time = %f seconds\n",(((double) (end.tv_usec - start.tv_usec) / 1000000 )+ (double) (end.tv_sec - start.tv_sec)));
	printf("Total Jobs Processed: %d\n", *counter);
    for (i = 0; i < num_thread; i++){
        printf("Thread %d processed %d jobs, with a total size of %d.\n", ThreadNum[i], ThreadJobProc[i], ThreadSizeProc[i]);
    }
    shmctl(shmid, IPC_RMID, NULL);
    shmctl(shmid2, IPC_RMID, NULL);
    shmctl(shmid3, IPC_RMID, NULL);
    shmctl(shmid4, IPC_RMID, NULL);
    shmctl(shmid5, IPC_RMID, NULL);
    shmctl(shmid6, IPC_RMID, NULL);
    shmctl(shmid7, IPC_RMID, NULL);
	return 0;
}