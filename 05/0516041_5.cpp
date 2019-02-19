/*
Student No.: 0516041
Student Name: YuSheng Su (蘇裕盛)
Email: kingjasonsu.cs05@g2.nctu.edu.tw
SE tag: xnxcxtxuxoxsx
Statement: I am fully aware that this program is not supposed to be posted to a public server, such as a public GitHub repository or a public web page. 
*/

#include <bits/stdc++.h>
#include <sys/time.h>
 
using namespace std;

struct LRU_PAGE{
	int index;
	int time;
	bool operator<(const LRU_PAGE &right)const{
        return right.time < time;    
    }
}lru_page[8200];

struct LFU_PAGE{
	int index;
	int freq;
	int time;
	bool operator<(const LFU_PAGE &right)const{
        if(right.freq != freq) return right.freq < freq;
		else return right.time < time;    
    }
}lfu_page[8200];

void LFU(string file){
	int size = 128;
	set<LFU_PAGE> LFU_set;
	cout << "LFU policy:" << endl;
	cout << "frame\thit\t\tmiss\t\tpage fault ratio" << endl;
	for(int i=0; i<4; ++i){
		LFU_set.clear();
		for(int j=0; j<8200; ++j){
			lfu_page[j].index = j;
			lfu_page[j].freq = 0;
			lfu_page[j].time = 2e9;
		}
		fstream fin;
		fin.open(file.c_str(), ios::in);
		int now = 0;
		int num;
		int full = 0;
		int hit = 0, miss = 0;
		int exist[8200] = {0};
		while(fin >> num){
			now++;
			if(exist[num]){
				hit++;
				LFU_set.erase(LFU_set.find(lfu_page[num]));
				lfu_page[num].freq++;
				LFU_set.insert(lfu_page[num]);
			}
			else{
				miss++;
				lfu_page[num].freq = 1;
				lfu_page[num].time = now;
				if(!full) if((int)LFU_set.size() == size) full++;
				if(full){
					exist[(*(--LFU_set.end())).index] --;
					LFU_set.erase(--LFU_set.end());
				}
				LFU_set.insert(lfu_page[num]);
				exist[num] ++;
			}
		}
		printf("%d\t%d\t\t%d\t\t%.10f\n", size, hit, miss, (float)miss/(float)(miss+hit));
		size *= 2;
	}
}


void LRU(string file){
	int size = 128;
	set<LRU_PAGE> LRU_set;
	cout << "LRU policy:" << endl;
	cout << "frame\thit\t\tmiss\t\tpage fault ratio" << endl;
	for(int i=0; i<4; ++i){
		int now = 0;
		LRU_set.clear();
		for(int j=0; j<8200; ++j){
			lru_page[j].index = j;
			lru_page[j].time = 2e9;
		}
		fstream fin;
		fin.open(file.c_str(), ios::in);
		int num;
		int full = 0;
		int hit = 0, miss = 0;
		int exist[8200] = {0};
		while(fin >> num){
			++now;
			if(exist[num]){
				hit++;
				LRU_set.erase(LRU_set.find(lru_page[num]));
				lru_page[num].time = now;
				LRU_set.insert(lru_page[num]);	
			}
			else{
				miss++;
				lru_page[num].time = now;
				if(!full) if((int)LRU_set.size() == size) full++;
				if(full){
					exist[(*(--LRU_set.end())).index] --;
					LRU_set.erase(--LRU_set.end());
				}
				LRU_set.insert(lru_page[num]);
				exist[num] ++;
			}
		}
		printf("%d\t%d\t\t%d\t\t%.10f\n", size, hit, miss, (float)miss/(float)(miss+hit));
		size *= 2;
	}
}

int main(int argc, char** argv){
	struct timeval start, end;
	int sec, usec;
	
	gettimeofday(&start, 0);
	LFU(argv[argc-1]);
	gettimeofday(&end, 0);
	sec = end.tv_sec - start.tv_sec;
    usec = end.tv_usec - start.tv_usec;
    printf("Total elapsed time: %.4f sec\n", sec+(usec / 1000000.0));
    cout << endl;
    
	gettimeofday(&start, 0);
	LRU(argv[argc-1]);
	gettimeofday(&end, 0);
	sec = end.tv_sec - start.tv_sec;
    usec = end.tv_usec - start.tv_usec;
    printf("Total elapsed time: %.4f sec\n", sec+(usec / 1000000.0));
    
    return 0;
}
