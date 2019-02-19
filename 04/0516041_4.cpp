/*
Student No.: 0516041
Student Name: YuSheng Su (蘇裕盛)
Email: kingjasonsu.cs05@g2.nctu.edu.tw
SE tag: xnxcxtxuxoxsx
Statement: I am fully aware that this program is not supposed to be posted to a public server, such as a public GitHub repository or a public web page. 
*/
#include <iostream>
#include <bits/stdc++.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>
#include <unistd.h>

using namespace std;

struct Job{
    void (*fptr)();
};
sem_t sem, sem2, bsort_wait, finish, busy_wait;
int array_num, array_divide, array_1[1000005], array_2[1000005];
bool done[15];
queue<Job> Job_queue;

void Read(char *input);
void Write(char *output);
void mergeArrays(int start1, int start2, int end1, int end2);
void bsort(int start, int end);
void *Dispatch(void *);
void *job_wait(void *);
void job_0();
void job_1();
void job_2();
void job_3();
void job_4();
void job_5();
void job_6();
void job_7();
void job_8();
void job_9();
void job_10();
void job_11();
void job_12();
void job_13();
void job_14();
void checker();

int main(){
    struct timeval start, end;
    char input[10] = "input.txt";
    char output[13] = "output_1.txt";
    
    Read(input);
    for(int i=0; i<15; ++i) done[i] = 0;
    array_divide = array_num/8;

    sem_init(&bsort_wait, 0, 0);
    sem_init(&sem, 0, 0);
    sem_init(&sem2, 0, 0);
    sem_init(&finish, 0, 0);

    pthread_t pt[8];
    for(int n=1; n<=8; ++n){
        memcpy(array_2, array_1, sizeof(array_1)); // reset
        pthread_t pt_dispatcher;

        while(!Job_queue.empty()) Job_queue.pop();
        pthread_create(&pt[n-1], NULL, job_wait, NULL);
        
        gettimeofday(&start, 0);
        pthread_create(&pt_dispatcher, NULL, Dispatch, NULL);
        sem_wait(&finish);
        gettimeofday(&end, 0);

        int sec = end.tv_sec - start.tv_sec;
        int usec = end.tv_usec - start.tv_usec;
        printf("%d sorting time: %f sec\n", n, sec+(usec/1000000.0));
        
        for(int i=0; i<15; ++i)done[i]=0;
        // checker();
        Write(output);
    }
    
    sem_destroy(&bsort_wait);
    sem_destroy(&sem);
    sem_destroy(&finish);
}

void Read(char *input){
    FILE *f;
    f = fopen(input, "r");
    if(f == NULL) printf("Read Error.");
    else{
        fscanf(f, "%d", &array_num);
        for(int i=0; i<array_num; ++i){
            fscanf(f, "%d", &array_1[i]);
        } 
    }
    fclose(f);
}

void Write(char *output){
    FILE *f;
    f = fopen(output, "w");
    for(int i=0; i<array_num; ++i)
        fprintf(f,"%d ",array_2[i]);
    fclose(f);
    output[7]++;
}

void mergeArrays(int start1, int start2, int end1, int end2){ 
    int i = start1, j = start2, k = 0; 
    int merge_len = (end1 - start1) + (end2 - start2);
    int temp[merge_len];

    while(i < end1 && j < end2) { 
        if (array_2[i] < array_2[j]) temp[k++] = array_2[i++]; 
        else temp[k++] = array_2[j++]; 
    }
    while(i < end1) temp[k++] = array_2[i++]; 
    while(j < end2) temp[k++] = array_2[j++];
    for(int i=start1, j=0; i<start1+merge_len, j<merge_len; ++i,++j) array_2[i]=temp[j];
}

void bsort(int start, int end){
    for(int i=start; i<end-1; ++i){
        for(int j=end-1; j>i; --j){
            if(array_2[j] < array_2[j-1]){
                int temp = array_2[j];
                array_2[j] = array_2[j-1];
                array_2[j-1] = temp;
            }
        }
    }

}

void *Dispatch(void *){
    Job job[8];
    job[0].fptr = &job_7;
    job[1].fptr = &job_8;
    job[2].fptr = &job_9;
    job[3].fptr = &job_10;
    job[4].fptr = &job_11;
    job[5].fptr = &job_12;
    job[6].fptr = &job_13;
    job[7].fptr = &job_14;
    for(int i=0; i<=7; ++i) Job_queue.push(job[i]);
    sem_post(&sem);
    sem_post(&sem2);
    while(1){   
        sem_wait(&sem);
        if(!Job_queue.empty()) sem_post(&bsort_wait);    
        else{
            while(1){
                sem_wait(&busy_wait);
                if(done[1] && done[2])    {done[1]=0;Job job;job.fptr=&job_0;Job_queue.push(job);sem_post(&sem);break;}
                if(done[3] && done[4])    {done[3]=0;Job job;job.fptr=&job_1;Job_queue.push(job);sem_post(&sem);break;}
                if(done[5] && done[6])    {done[5]=0;Job job;job.fptr=&job_2;Job_queue.push(job);sem_post(&sem);break;}
                if(done[7] && done[8])    {done[7]=0;Job job;job.fptr=&job_3;Job_queue.push(job);sem_post(&sem);break;}
                if(done[9] && done[10])   {done[9]=0;Job job;job.fptr=&job_4;Job_queue.push(job);sem_post(&sem);break;}
                if(done[11] && done[12])  {done[11]=0;Job job;job.fptr=&job_5;Job_queue.push(job);sem_post(&sem);break;}
                if(done[13] && done[14])  {done[13]=0;Job job;job.fptr=&job_6;Job_queue.push(job);sem_post(&sem);break;}
            }
        }
    }
}

void *job_wait(void *){
    while(1){
        Job job;
        sem_wait(&sem2);
        sem_wait(&bsort_wait);
        if(Job_queue.empty()) continue;
        job = Job_queue.front();
        Job_queue.pop();
        sem_post(&sem2);
        sem_post(&sem);
        if(job.fptr == NULL) continue;
        job.fptr();
    }
}

void job_0(){
    mergeArrays(0*array_divide, 4*array_divide, 4*array_divide, array_num);
    done[0]=1;
    sem_post(&finish);
    sem_post(&busy_wait);
}
void job_1(){
    mergeArrays(0*array_divide, 2*array_divide, 2*array_divide, 4*array_divide);
    done[1]=1;
    sem_post(&busy_wait);
}
void job_2(){
    mergeArrays(4*array_divide, 6*array_divide, 6*array_divide, array_num);
    done[2]=1;
    sem_post(&busy_wait);
}
void job_3(){
    mergeArrays(0*array_divide, 1*array_divide, 1*array_divide, 2*array_divide);
    done[3]=1;
    sem_post(&busy_wait);
}
void job_4(){
    mergeArrays(2*array_divide, 3*array_divide, 3*array_divide, 4*array_divide);
    done[4]=1;
    sem_post(&busy_wait);
}
void job_5(){
    mergeArrays(4*array_divide, 5*array_divide, 5*array_divide, 6*array_divide);
    done[5]=1;
    sem_post(&busy_wait);
}
void job_6(){
    mergeArrays(6*array_divide, 7*array_divide, 7*array_divide, array_num);
    done[6]=1;
    sem_post(&busy_wait);
}
void job_7(){
    bsort(0*array_divide, 1*array_divide); 
    done[7]=1;
    sem_post(&busy_wait);
}
void job_8(){
    bsort(1*array_divide, 2*array_divide); 
    done[8]=1;
    sem_post(&busy_wait);
}
void job_9(){
    bsort(2*array_divide, 3*array_divide); 
    done[9]=1;
    sem_post(&busy_wait);
}
void job_10(){
    bsort(3*array_divide, 4*array_divide); 
    done[10]=1;
    sem_post(&busy_wait);
}
void job_11(){
    bsort(4*array_divide, 5*array_divide); 
    done[11]=1;
    sem_post(&busy_wait);
}
void job_12(){
    bsort(5*array_divide, 6*array_divide); 
    done[12]=1;
    sem_post(&busy_wait);
}
void job_13(){
    bsort(6*array_divide, 7*array_divide); 
    done[13]=1;
    sem_post(&busy_wait);
}
void job_14(){
    bsort(7*array_divide, array_num); 
    done[14]=1;
    sem_post(&busy_wait);
}

void checker(){
    int flag=0;
    for(int i=0; i<array_num-1; ++i){
        if(array_2[i] > array_2[i+1]) flag++;
    }
    if(flag)printf("NO~~~%d\n",flag);
    else printf("YESSSS\n");
}