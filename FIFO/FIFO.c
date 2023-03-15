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

struct Node
{
	int pid;
    int JobSize;
};

struct queue{
    struct Node list[30];
    int qLen;
};

typedef int buffer_t;
struct CSem {
    sem_t mutex;
    sem_t wait; 
    int val;
};

struct queue * head, *head1;
struct CSem *empty_sem, *full_sem, *empty_sem1, *full_sem1;
struct Node *processid, *processid1;
pthread_mutex_t *buffer_mutex, *buffer_mutex1;
struct Node *buffer, *buffer1, *buffer2;
int *buffer_index, *buffer_index1, *counter, *count1, *kill_flag;
struct timeval begin, end, start1, end1, end3, start3, end4, start4;
int shmid, shmid2, shmid3, shmid4, shmid5, shmid6, shmid7, shmid8, shmid9, shmid10, shmid11;
float *timeresul, *timeresult;
float *timetotal, *timetotal1;
int ThreadSizeProc[20] = {0};
int ThreadJobProc[20] ={0};
int childpid[100];
pthread_t threadid[100];
int num_proc, num_thread, JobsComplete = 0;
key_t key = 4311;
key_t key2 = 4312;
key_t key3 = 4313;
key_t key4 = 4314;
key_t key5 = 4315;
key_t key6 = 4316;
key_t key7 = 4317;
key_t key8 = 4318;
key_t key9 = 4319;
key_t key10 = 4320;
key_t key11 = 4321;

void insert(struct Node In);
void removeNode(int ThreadNum);
void printList();
void initiateQueue();

void initiateQueue(){
    int tempschmid;
    struct queue *Head;
    tempschmid = shmget(key8, sizeof(head1), 0666);
	Head = shmat(shmid8, NULL, 0);
    for(int i = 0; i < 30; i++){
        Head->list[i].pid = -1;
        Head->list[i].JobSize = -1;
    }
    Head->qLen = 0;
}

void insert(struct Node In){
    int tempschmid;
    struct queue *Head;
    tempschmid = shmget(key8, sizeof(head1), 0666);
	Head = shmat(shmid8, NULL, 0);
    if(Head->qLen != 30){
        Head->list[Head->qLen].pid = In.pid;
        Head->list[Head->qLen].JobSize = In.JobSize;
        Head->qLen = Head->qLen + 1;
		printf("Producer %d added job size %d to buffer\n", In.pid, In.JobSize);
  
    }
    else{
        printf("\nThe queue is full!\n");
    }

}

void removeNode(int ThreadNum){
    int tempschmid;
    struct queue *Head;
    struct Node swap;
    tempschmid = shmget(key8, sizeof(head1), 0666);
	Head = shmat(shmid8, NULL, 0);
    if(Head->qLen != 0){
        printf("Consumer %d dequeued process %d, job size %d from buffer\n", ThreadNum, Head->list[0].pid, Head->list[0].JobSize);
        for(int i = 0; i < 29; i++){
            Head->list[i].pid = Head->list[i+1].pid;
            Head->list[i].JobSize = Head->list[i+1].JobSize;
        }
        Head->list[29].pid = -1;
        Head->list[29].JobSize = -1;
        Head->qLen = Head->qLen - 1;
    }
}


void printList()
{
	int tempschmid;
    struct queue *Head;
    tempschmid = shmget(key8, sizeof(head1), 0666);
	Head = shmat(shmid8, NULL, 0);
    printf("\n");
    for(int i = 0; i < 30; i++){
        printf("%d:%d\t", Head->list[i].pid, Head->list[i].JobSize);
    }
    printf("\n");
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

	 buffer = buffer1;
	 buffer_index = buffer_index1;
	 buffer_mutex = buffer_mutex1;
	 full_sem = full_sem1;
	 counter = count1;
	 empty_sem = empty_sem1;
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
		 processid->pid = ThreadNum;
		 processid->JobSize = val;
		 gettimeofday(&start3, NULL);
		 Pc(full_sem);
		 gettimeofday(&end3, NULL);
		 float timevall = ((double)end3.tv_sec+0.000001*(double)end4.tv_usec) - ((double)start3.tv_sec+0.000001*(double)start4.tv_usec);
		 pthread_mutex_lock(buffer_mutex); /* protecting critical section */
		 insert(*processid);
		 *timeresult = timevall + *timeresult;
		 gettimeofday(&start1, NULL);
		 pthread_mutex_unlock(buffer_mutex);
		 Vc(empty_sem);
    }
}

void *consumer(void *thread_n) {

	 buffer = buffer1;
	 buffer_index = buffer_index1;
	 buffer_mutex = buffer_mutex1;
	 full_sem = full_sem1;
	 empty_sem = empty_sem1;
     counter = count1;
    int ThreadNum = *(int *)thread_n;
	threadid[ThreadNum] = pthread_self();
    while (1) {
        usleep(800000);
        gettimeofday(&start4, NULL);
        Pc(empty_sem);
		gettimeofday(&end4, NULL);
		float timeee = ((double)end4.tv_sec+0.000001*(double)end4.tv_usec) - ((double)start4.tv_sec+0.000001*(double)start4.tv_usec);
		*timetotal = *timetotal + timeee;
        /* there could be race condition here, that could cause
           buffer underflow error */
        pthread_mutex_lock(buffer_mutex);
        removeNode(ThreadNum);
        // printList();
        ThreadSizeProc[ThreadNum] += processid->JobSize;
        ThreadJobProc[ThreadNum] += 1;
        JobsComplete += 1;
        pthread_mutex_unlock(buffer_mutex);
        Vc(full_sem); // post (increment) fullbuffer semaphore
        // printf("Total jobs completed so far: %d\n", JobsComplete);
        if (JobsComplete == *counter){deathtoall();}
    }
}

void signal_handler(int sig){
    printf("\nRecieved SIGINT signal!\n");
    *kill_flag = 1;
    shmctl(shmid, IPC_RMID, NULL);
    shmctl(shmid2, IPC_RMID, NULL);
    shmctl(shmid3, IPC_RMID, NULL);
    shmctl(shmid4, IPC_RMID, NULL);
    shmctl(shmid5, IPC_RMID, NULL);
    shmctl(shmid6, IPC_RMID, NULL);
    shmctl(shmid7, IPC_RMID, NULL);
	shmctl(shmid8, IPC_RMID, NULL);
    shmctl(shmid9, IPC_RMID, NULL);
    shmctl(shmid10, IPC_RMID, NULL);
	shmctl(shmid11, IPC_RMID, NULL);
    deathtoall();
    exit(sig);
}

int main(int argc, char *argv[])
{
	
    int i;
    if(argc != 3){
        printf("incorrect format...\n");
        printf("Please enter two In parameters for desired processes and threads. \n");
        return 0;
    }
    else{
        num_proc = atoi(argv[1]);
	    num_thread = atoi(argv[2]);
    }
	
    //Code to redirect ^C signal to customer defined sig handler
	signal(SIGINT, signal_handler);

    //The code below creates shared memory
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
    if ((shmid8 = shmget(key8, sizeof(head1), IPC_CREAT | 0666)) < 0){
         perror("shmget");
         exit(1);}
	 if( (head1 = shmat(shmid8, NULL, 0)) == (void *) -1 ){
         perror("shmat");
         exit(1);}
	 if ((shmid9 = shmget(key9, sizeof(processid1), IPC_CREAT | 0666)) < 0){
         perror("shmget");
         exit(1);}
	 if( (processid1 = shmat(shmid9, NULL, 0)) == (void *) -1 ){
         perror("shmat");
         exit(1);}
	 if ((shmid10 = shmget(key10, sizeof(timeresul), IPC_CREAT | 0666)) < 0){
         perror("shmget");
         exit(1);}
	 if( (timeresul = shmat(shmid10, NULL, 0)) == (void *) -1 ){
         perror("shmat");
         exit(1);}
	 if ((shmid11 = shmget(key11, sizeof(timetotal1), IPC_CREAT | 0666)) < 0){
         perror("shmget");
         exit(1);}
	 if( (timetotal1 = shmat(shmid11, NULL, 0)) == (void *) -1 ){
         perror("shmat");
         exit(1);}
	 
     initiateQueue();
	 timetotal = timetotal1;
	 timeresult = timeresul;
	 buffer = buffer1;
	 buffer_index = buffer_index1;
	 buffer_mutex = buffer_mutex1;
	 full_sem = full_sem1;
	 empty_sem = empty_sem1;
	 processid = processid1;
	 head = head1;
     counter = count1;
	 *counter = 0;     
     *buffer_index = 0;

     gettimeofday(&begin, NULL);
	 pthread_mutex_init(buffer_mutex, NULL);
	 initiate_sem();

     pthread_t thread[num_thread];
	 int ThreadNum[num_thread];
	 
	 
	 
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
    printf ("Total execution time = %f seconds\n",(((double) (end.tv_usec - begin.tv_usec) / 1000000 )+ (double) (end.tv_sec - begin.tv_sec)));
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
	shmctl(shmid8, IPC_RMID, NULL);
    shmctl(shmid9, IPC_RMID, NULL);
    shmctl(shmid10, IPC_RMID, NULL);
	shmctl(shmid11, IPC_RMID, NULL);
	return 0;
}
