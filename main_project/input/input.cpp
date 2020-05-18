#include <string>
#include <iostream>
#include <circuit/circuit.hpp>
#include <component/resistor.hpp>
#include <component/capacitor.hpp>
#include <component/inductor.hpp>
#include <component/voltageSource.hpp>
#include <component/currentSource.hpp>
#include <component/diode.hpp>
#include <component/voltageControlledVoltageSource.hpp>
#include <component/currentControlledVoltageSource.hpp>
#include <component/voltageControlledCurrentSource.hpp>
#include <component/currentControlledCurrentSource.hpp>

#include "input.hpp"

void readSpice(Circuit& c, istream& file){
    string title;
    getline(file, title);
    c.setTitle(title);

    string lineString{};
    int maxNode = 0;

    while(getline(file, lineString)){
        stringstream line;
        string compTypeC{};
        string name{};
        string arg{};
        vector<string> args{};

        line << lineString;
        line >> name;
        compTypeC = name[0];

        while(line >> arg){
            args.push_back(arg);
        }

        // for now this script will assume knowledge of components to get largest node values
        // will only work for components with two inputs, will fix this later
        int n1 = stoi(args[0]);
        int n2 = stoi(args[1]);
        
        if(n1 > maxNode) maxNode = n1;
        if(n2 > maxNode) maxNode = n2;

        if(compTypeC == "R" || compTypeC == "r"){
			c.addComponent<Resistor>(name, args);
		}else if(compTypeC == "V" || compTypeC == "v"){
			c.addComponent<VoltageSource>(name, args);
		}else if(compTypeC == "I" || compTypeC == "i"){
			c.addComponent<CurrentSource>(name, args);
		}else if(compTypeC == "L" || compTypeC == "l"){
			c.addComponent<Inductor>(name, args);
		}else if(compTypeC == "C" || compTypeC == "c"){
			c.addComponent<Capacitor>(name,args);
		}else if(compTypeC == "D" || compTypeC == "d"){
            c.addComponent<Diode>(name, args);
        }else if(compTypeC == "E" || compTypeC == "e"){
            c.addComponent<VoltageControlledVoltageSource>(name, args);
        }else if(compTypeC == "H" || compTypeC == "h"){
            c.addComponent<CurrentControlledVoltageSource>(name, args);
        }else if(compTypeC == "G" || compTypeC == "g"){
            c.addComponent<VoltageControlledCurrentSource>(name, args);
        }else if(compTypeC == "F" || compTypeC == "f"){
            c.addComponent<CurrentControlledCurrentSource>(name, args);
        }else{
            cerr << "Unknown component type" <<endl;
            exit(1); //change to throwing error
        }
		
    }
    c.setHighestNodeNumber(maxNode);
}

void setupBasic(Circuit& c, double timeStep){
    c.setTimeStep(timeStep);
}
