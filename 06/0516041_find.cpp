#include <dirent.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <bits/stdc++.h>
#define MB 1048576 

using namespace std;

int cond[4] = {0};
long long int inode = 0, size_min = 0, size_max = 0;
char file_name[100] = "";

int search(char* path){
	DIR* data_path = opendir(path);
	if(data_path == NULL) return -1;
	struct dirent 	*block;
	struct stat 	buf;

	while((block = readdir(data_path))!=NULL){

		if(!strcmp(block->d_name, ".") || !strcmp(block->d_name, "..")) continue;
		else{
			char path_str[1000];
			strcpy(path_str, path);
			strcat(path_str,"/");
		    strcat(path_str, block->d_name);  
			
			stat(path_str, &buf);

			if(block->d_type==4) search(path_str);	

			if((cond[0] && inode != buf.st_ino) 			   	||
			   (cond[1] && strcmp(block->d_name, file_name)) 	||	
			   (cond[2] && buf.st_size < size_min * MB) 	   	||
			   (cond[3] && buf.st_size > size_max * MB)) continue;

			cout << path_str << " ";
			cout << buf.st_ino << " ";
			cout << buf.st_nlink << " ";
			cout << buf.st_size / pow(2.,20.) << " MB ";
			cout << ctime(&buf.st_ctime);			
		}
	}
	return 0;
}

int main(int argc,char** argv){	
    for(int i=2; i<argc; ++i){
        if(!strcmp(argv[i],"-inode")){
            inode = atoi(argv[i+1]);
            cond[0]++;
        }
        else if(!strcmp(argv[i],"-name")){
        	strcpy(file_name, argv[i+1]);
            // file_name = argv[i+1];
            cond[1]++;
        }
        else if(!strcmp(argv[i],"-size_min")){
            size_min = atoi(argv[i+1]);
            cond[2]++;
        }
        else if(!strcmp(argv[i],"-size_max")){
            size_max = atoi(argv[i+1]);
            cond[3]++;
        }
    }
    // cout << "inode: " << inode << endl;
    // cout << "name: " << file_name << endl;
    // cout << "size_min: " << size_min << endl;
    // cout << "size_max: " << size_max << endl;
    
    search(argv[1]);
	return 0;
}