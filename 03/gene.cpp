#include<iostream>
#include<fstream>
#include<ctime>
#include <cstdlib>

using namespace std;

int main(){
	int num=0;
	cout << "Please input the number:" ;
	cin >> num;
	
	fstream fp;
	fp.open("text",ios::out | ios::trunc);
	fp << num << " " << endl;

	srand(time(NULL));
	for(int i=0; i<num; ++i){
		int x=rand()%20000;
		fp << x << " ";
	}
	fp.close();
	return 0;
}

