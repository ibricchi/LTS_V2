#include <vector>
#include <iostream>
#include <string>

using namespace std;

int main(int argc,char** argv){
vector<string> varNames;
string s;
int whichColumn =1;
if(argc!=1){
whichColumn = stoi(argv[1]);
}
//for(int a=0;a<18;a++){
while(!cin.fail()){
	getline(cin,s);
	s.pop_back();
	//cout << s << endl;
	if(s=="Variables:"){
		getline(cin,s);
		//cout << "Woohoo!";
		s.pop_back();
		//cerr << "ERROR "<< s << "ERROR";
		while(s!="Values:"){
			varNames.push_back(s);
			getline(cin,s);
			s.pop_back();
		}
		getline(cin,s);
		s.pop_back();
		for (int n=0;n<varNames.size();n++){
			if(n==0 || n==whichColumn){cout << varNames[n]  << ",";}
		}
		cout << endl;
		while(!cin.fail()){
			for(int n=0;n<varNames.size();n++){
				if(s[s.size()-23] == '-'){				
					s.erase(0,s.size()-23); //22 is the number of characters in each value if positive		
				}else{s.erase(0,s.size()-22);}
				if(n==0 || n==whichColumn){cout << s << ",";}
				getline(cin,s);
				s.pop_back();
			}
			cout << endl;
		}
	}

}


}

