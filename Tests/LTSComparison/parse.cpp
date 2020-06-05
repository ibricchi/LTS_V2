#include <vector>
#include <iostream>
#include <string>
#include <sstream>

using namespace std;

int main(){
vector<string> varNames;
string input;
string s;
string garbage;
//int nOfVars =0;

//for(int a=0;a<18;a++){
while(!cin.fail()){
	getline(cin,s);
	s.pop_back();

	if(s=="Variables:"){
		while(true){
			getline(cin,input);
			input.pop_back();

			if(input=="Values:"){
				break;
			}

			stringstream stream(input);

			stream >> garbage; //only interested in middle value of value line
			stream >> s;
			varNames.push_back(s);
		}
		s.clear();
		getline(cin,s);
		s.pop_back();
		for (int n=0;n<varNames.size();n++){
			cout << varNames[n]  << ",";
		}
		cout << endl;
		while(!cin.fail()){
			for(int n=0;n<varNames.size();n++){
				if(s[s.size()-23] == '-'){				
					s.erase(0,s.size()-23); //22 is the number of characters in each value if positive		
				}else{s.erase(0,s.size()-22);}
				cout << s << ",";
				getline(cin,s);
				s.pop_back();
			}
			cout << endl;
		}
	}

}


}

