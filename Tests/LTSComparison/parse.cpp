#include <vector>
#include <iostream>
#include <string>

using namespace std;

int main(){
vector<string> varNames;
string s;
//int nOfVars =0;

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
			cout << varNames[n]  << ",";
		}
		cout << endl;
		while(!cin.fail()){
			s.erase(0,2);
			for(int n=0;n<varNames.size();n++){
				s.erase(0,1);
				cout << s << ",";
				getline(cin,s);
				s.pop_back();
			}
			cout << endl;
		}
	}

}


}

