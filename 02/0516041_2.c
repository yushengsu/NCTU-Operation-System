/*
Student No.: 0516041
Student Name: YuSheng Su (蘇裕盛)
Email: kingjasonsu.cs05@g2.nctu.edu.tw
SE tag: xnxcxtxuxoxsx
Statement: I am fully aware that this program is not supposed to be posted to a public server, such as a public GitHub repository or a public web page.
*/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/shm.h>
#include<sys/ipc.h>
#include<sys/types.h>
#include<sys/time.h>

struct timeval time1, time2;
double t1, t2;
int shmId[3];
int *matrix[3];

void initial(int size){

	for(int i=0; i<3; i++){
		shmId[i] = shmget(0, sizeof(int[size][size]), 0666|IPC_CREAT); //shmget(key, size, shmflg)

		if(shmId[i] == -1){
			printf("ERROR while get shm\n");
			exit(-1);
		}

		matrix[i] = (int *)shmat(shmId[i], 0, 0); //shmat(shmId, shmaddr, shmflg)

		if(*matrix[i] == -1){

			printf("ERROR while attach shm");
			exit(-1);
		}
	}

	for(int i=0; i<size*size; i++){
		*(matrix[0]+i) = i;
		*(matrix[1]+i) = i;
	}
}

void multiply(int size, int n){

	gettimeofday(&time1, NULL);
	t1 = time1.tv_sec + time1.tv_usec/1000000.0;
	
	int divide[n];
	int remainder = size % n;
	for(int i=0; i<n; ++i){
		divide[i] = (int)(size/n);
		if(i < remainder) divide[i]++;
		//printf("%d ", divide[i]);
	}
	//printf("\n");

	int seg[n+1];
	for(int i=0; i<=n; i++){
		if(!i) seg[i] = 0;
		else seg[i] = seg[i-1] + divide[i-1];
		//printf("%d ", seg[i]);
	}
	//printf("\n");

	pid_t pid[n];
	for(int i=0; i<n; i++){

		pid[i]=fork();
		if(pid[i]<0){

			printf("{ERROR while fork}");
			exit(-1);
		}
		else if(pid[i]==0){
			for(int j=seg[i]; j<seg[i+1]; j++){
				// printf("%d ", j);
				for(int k=0; k<size; k++){
					
					*(matrix[2]+(j*size+k))=0;
					for(int l=0; l<size; l++){
						*(matrix[2]+(j*size+k)) += *(matrix[0]+(j*size+l)) * *(matrix[1]+(l*size+k));
					}
				}
			}
			exit(0);
		}
		else{
			//Parent
			//printf("{ps:%d created}\n",i);
		}
	}

	int status;
	for(int i=0; i<n; i++) waitpid(pid[i], &status, 0);

	unsigned int sum=0;
	for(int i=0; i<size*size; i++) sum += *(matrix[2]+i);
	
	gettimeofday(&time2, NULL);
	t2 = time2.tv_sec+time2.tv_usec/1000000.0;
	printf("Multiplying matrices using %d process", n);

	if(n==1) printf("\n");
	else printf("es\n");
	printf("Elapsed time: %lf sec, Checksum: %u\n", t2-t1, sum);

}

void clean(){
	for(int i=0; i<3; i++) shmctl(shmId[i], IPC_RMID, NULL);
	return ;
}

int main(){
	int size=0;
	printf("Input the matrix dimension: ");
	scanf("%d", &size);
	printf("\n");

	for(int i=1; i<=16; i++){
		initial(size);	
		multiply(size, i);
		clean();
	} 

	return 0;
}
