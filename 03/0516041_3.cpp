/*
Student No.: 0516041
Student Name: YuSheng Su (蘇裕盛)
Email: kingjasonsu.cs05@g2.nctu.edu.tw
SE tag: xnxcxtxuxoxsx
Statement: I am fully aware that this program is not supposed to be posted to a public server, such as a public GitHub repository or a public web page. 
*/
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>
#include <unistd.h>

using namespace std;

sem_t sem[15],sem2[15],sem_end;
int array_num, array_divide, array[1000005], array2[1000005];

void Read(int *array_num,char *file_name);
void Write(int *array,int array_num,char *file_name);

void bsort(int start, int end, int check);
void mergeArrays(int start1, int start2, int end1, int end2, int ans[]); 
void *multiple_merge(void *arg);
void *multiple_bubble(void *arg);
void *master_thread(void *arg);
void single_bubble();
void single_merge();
void *single_thread(void *);

// void check(){
// 	int flag1 = 0;
// 	int flag2 = 0;
// 	for(int i=0; i < array_num-1; ++i){
// 		if(array[i] > array[i+1]) flag1 = 1;
// 		if(array2[i] > array2[i+1]) flag2 = 1;	
// 	}
// 	if(flag1) cout << "array1 error" << endl;
// 	else if(flag2) cout << "array2 error" << endl;
// 	else cout << "correct!" << endl;
// }

int main(){
	int sec, usec;
	struct timeval start, end;

	printf("Please input file name:");
	char file_name[1000]="text";
	scanf("%s", file_name);

	Read(&array_num, file_name);
	array_divide = array_num/8;
	
	for(int i=0; i<15; ++i) sem_init(&sem[i], 0, 0);
	for(int i=0; i<15; ++i) sem_init(&sem2[i], 0, 0);
	sem_init(&sem_end, 0, 0);
	
	pthread_t pt[15];
	pthread_create(&pt[0], NULL, master_thread, NULL); //master process

	for(int i=1; i<15; ++i){
		int *p = (int *)malloc(sizeof(int));
		*p = i;
		if(i <= 6) pthread_create(&pt[i], NULL, multiple_merge, (void *)(p));
		else pthread_create(&pt[i], NULL, multiple_bubble, (void *)(p));
	}

	//multi-thread
	gettimeofday(&start, 0);
	sem_post(&sem[0]); 
	sem_wait(&sem2[0]);
	gettimeofday(&end, 0);
	Write(array, array_num, (char*)"output1.txt");
	sec = end.tv_sec - start.tv_sec;
	usec = end.tv_usec - start.tv_usec;
	printf("Multi-thread sorting time: %f sec \n", sec+(usec/1000000.0));
	
	//single-thread
	pthread_t st;
	gettimeofday(&start, 0);
	pthread_create(&st, NULL, single_thread, NULL);
	sem_wait(&sem_end);
	gettimeofday(&end, 0);
	sec = end.tv_sec - start.tv_sec;
	usec = end.tv_usec - start.tv_usec;
	printf("Single-thread sorting time: %f sec \n", sec+(usec/1000000.0));
	
	for(int i=0; i<15; ++i) sem_destroy(&sem[i]);
	for(int i=0; i<15; ++i) sem_destroy(&sem2[i]);
	sem_destroy(&sem_end);
	// check();
}

void Read(int *array_num, char *file_name){
	FILE *f;
	f = fopen(file_name,"r");
	if(f==NULL)	printf("Read Error.");
	else{
		fscanf(f,"%d",array_num);
		for(int i=0; i<*array_num; ++i){
			fscanf(f,"%d",&array[i]);
			array2[i] = array[i];
		} 
	}
	fclose(f);
}

void Write(int *array, int elements, char *filen_Lme){
	FILE *f;
	f = fopen(filen_Lme,"w");
	for(int i=0; i<elements; ++i)
		fprintf(f,"%d ",array[i]);
	fclose(f);
}

void bsort(int start, int end, int check){
	for(int i=start; i<end; ++i){
		for(int j=end; j>i; --j){
			if(!check){
				if(array[j] < array[j-1]){
					int temp = array[j];
					array[j] = array[j-1];
					array[j-1] = temp;
				}
			}
			else{
				if(array2[j] < array2[j-1]){
					int temp = array2[j];
					array2[j] = array2[j-1];
					array2[j-1] = temp;
				}	
			}
		}
	}
}

void mergeArrays(int start1, int start2, int end1, int end2, int ans[]){ 
    int i = start1, j = start2, k = 0; 
    while(i < end1 && j < end2) { 
        if (array[i] < array[j]) ans[k++] = array[i++]; 
        else ans[k++] = array[j++]; 
    }
  	while(i < end1) ans[k++] = array[i++]; 
 	while(j < end2) ans[k++] = array[j++];
}

void *multiple_merge(void *arg){
	int index = *((int *) arg);

	sem_wait(&sem[index]);
	sem_post(&sem[index*2+1]);
	sem_post(&sem[index*2+2]);
	sem_wait(&sem2[index*2+1]);
	sem_wait(&sem2[index*2+2]);

	int start_L, end_L;
	int start_R, end_R;
	int merge_len;

	if(index >= 3 && index <= 6){
		start_L = (index-3) * 2 * array_divide;
		start_R = ((index-3)*2+1) * array_divide;
		end_L = ((index-3)*2+1) * array_divide;
		end_R = ((index-3)*2+2) * array_divide;
		if(index == 6) end_R = array_num;
	}
	else if(index == 2){
		start_L = 4 * array_divide;
		start_R = 6 * array_divide;
		end_L = 6 * array_divide;
		end_R = array_num;
	}
	else if(index == 1){
		start_L = 0;
		start_R = 2 * array_divide;
		end_L = 2 * array_divide;
		end_R = 4 * array_divide;
	}
	merge_len = (end_L - start_L) + (end_R - start_R);
	int array_multiple[merge_len];
	mergeArrays(start_L, start_R, end_L, end_R, array_multiple);

	for(int i=start_L, j=0; i<start_L+merge_len, j<merge_len; ++i, ++j) array[i]=array_multiple[j];
	sem_post(&sem2[index]);
	pthread_exit(NULL);
}

void *multiple_bubble(void *arg){
	int index = *((int *) arg);
	sem_wait(&sem[index]);

	if(index == 14) bsort(7*array_divide, array_num-1, 0);
	else bsort((index-7)*array_divide, (index-6)*array_divide-1, 0);

	sem_post(&sem2[index]);
	pthread_exit(NULL);
}

void *master_thread(void *arg){
	
	sem_wait(&sem[0]);
	sem_post(&sem[1]);
	sem_post(&sem[2]);
	sem_wait(&sem2[1]);
	sem_wait(&sem2[2]);

	int start_L = 0, end_L = 4*array_divide;
	int start_R = 4*array_divide, end_R = array_num;
	int merge_len = (end_L - start_L) + (end_R - start_R);
	int array_multiple[array_num];

	mergeArrays(start_L, start_R, end_L, end_R, array_multiple);
	for(int i=start_L, j=0; i<start_L+merge_len, j<merge_len; ++i, ++j) array[i]=array_multiple[j];
	sem_post(&sem2[0]);
	pthread_exit(NULL);
}

void single_bubble(){
	for(int i=7; i<15; ++i){
		if(i == 14) bsort(7*array_divide, array_num-1, 1);
		else bsort((i-7)*array_divide, (i-6)*array_divide-1, 1);
	}
}

void single_merge(){
	for(int index=6; index>0; --index){
		
		int start_L, end_L;
		int start_R, end_R;
		int merge_len;

		if (index >= 3 && index <= 6){
			start_L = (index-3)*2 * array_divide;
			start_R = ((index-3)*2+1) * array_divide;
			end_L = ((index-3)*2+1) * array_divide;
			end_R = ((index-3)*2+2) * array_divide;
			if(index == 6) end_R = array_num;
		}
		if (index == 2){
			start_L = 4 * array_divide;
			start_R = 6 * array_divide;
			end_L = 6 * array_divide;
			end_R = array_num;
		}
		if (index == 1){
			start_L = 0;
			start_R = 2 * array_divide;
			end_L = 2 * array_divide;
			end_R = 4 * array_divide;
		}
		merge_len = (end_L - start_L) + (end_R - start_R);
		int array_single[merge_len];
		mergeArrays(start_L, start_R, end_L, end_R, array_single);
		for(int i=start_L, j=0; i<start_L+merge_len, j<merge_len; ++i,++j) array2[i]=array_single[j];
	}
	// last single merge
	int start_L = 0, end_L = 4 * array_divide;
	int start_R = 4 * array_divide, end_R = array_num;
	int merge_len = end_R;
	
	int array_single[array_num];
	mergeArrays(start_L, start_R, end_L, end_R, array_single);
	Write(array_single, array_num, (char*)"output2.txt");
}

void *single_thread(void *){
	single_bubble();
	single_merge();
	sem_post(&sem_end);
}