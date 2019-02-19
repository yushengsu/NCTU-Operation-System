#include<iostream>
#include<fstream>
#include<ctime>

using namespace std;

int main(){
	int num=0;
	cout << "Please input the number:" ;
	cin >> num;
	
	fstream fp;
	fp.open("input.txt",ios::out | ios::trunc);
	fp << num << " " << endl;

	srand(time(NULL));
	for(int i=num-1; i>=0; --i){
		
		fp << i << " ";
	}
	fp.close();
	return 0;
}

