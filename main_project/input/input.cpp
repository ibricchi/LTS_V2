#include <string>
#include <iostream>
#include <algorithm>
#include <circuit/circuit.hpp>
#include <component/resistor.hpp>
#include <component/capacitor.hpp>
#include <component/inductor.hpp>
#include <component/voltageSource.hpp>
#include <component/currentSource.hpp>
#include <component/component.hpp>
#include <component/enums.hpp>
#include <component/diode.hpp>

#include "input.hpp"

ModelStatement::ModelStatement(vector<string> args){
    componentId = args.at(0);
    string componentStr = args.at(1);

    //convert componentStr to uppercase (as netlist case insensitive)
    for_each(componentStr.begin(), componentStr.end(), [](char &c){
        c = toupper(c);
    });

    auto it = componentTable.find(componentStr);
    if(it == componentTable.end()){
        cerr << "This component is not supported. Component: " << componentStr <<endl;
        exit(1);
    }
    componentName = it->second;

    const string delimiter = "=";
    string param{}, paramName{}, paramValueStr{};
    float paramValue{};
    for(int i{2}; i<args.size(); i++){
        param = args.at(i);

        //remove opening and closing parentheses
        if(i == 2){
            param.erase(0, 1);
        }else if(i == args.size()-1){
            param.erase(param.end()-1, param.end());
        }

        paramName = param.substr(0, param.find(delimiter));
        paramValueStr = param.substr(param.find(delimiter)+1, param.size()-1);

        //convert paramName to uppercase (as netlist case insensitive)
        for_each(paramName.begin(), paramName.end(), [](char &c){
	        c = toupper(c);
        });

        paramValue = Component::getValue(paramValueStr);

        //convert paramName to int enum
        int paramId{};
        if(componentName == component::DIODE){
            auto it1 = diodeParamTable.find(paramName);
            if(it1 == diodeParamTable.end()){ //if not found
                cerr << componentStr << " doesn't support the parameter " << paramName <<endl;
                exit(1);
            }
            paramId = static_cast<int>(it1->second);
        }else if(componentName == component::NMOS || componentName == component::PMOS){
            auto it1 = mosfetParamTable.find(paramName);
            if(it1 == mosfetParamTable.end()){
                cerr << componentStr << " doesn't support the parameter " << paramName <<endl;
                exit(1);
            }
            paramId = static_cast<int>(it1->second);
        }else if(componentName == component::NPN || componentName == component::PNP){
            auto it1 = bjtParamTable.find(paramName);
            if(it1 == bjtParamTable.end()){
                cerr << componentStr << " doesn't support the parameter " << paramName <<endl;
                exit(1);
            }
            paramId = static_cast<int>(it1->second);
        }

        if(params.find(paramId) != params.end()){ //if found
            cerr << "Invalid netlist: Cannot contain the same parameter twice in a model statement" <<endl;
            exit(1);
        }
        params.emplace(paramId, paramValue);
    }
}

void readSpice(Circuit& c, istream& file){
    //reading netlist title (must be first line of netlist)
    string title;
    getline(file, title);
    c.setTitle(title);

    string lineString{};
    int maxNode = 0;
    vector<ModelStatement> modelStatements{};

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

        if(name == ".model" || name == ".MODEL"){
			modelStatements.emplace_back(args);

            continue; //don't execute the rest (not relevant for .model statements)
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
			c.addComponent<Diode>(name,args);
		}else{
            cerr << "Unsuported netlist statement. Statement: " << compTypeC <<endl;
            exit(1);
        }
    }

    //add model params to components
    auto nonLinears = c.getNonLinearsRef();
    for(const auto &model : modelStatements){
        for(const auto &nL : nonLinears){
            if(nL->getName() == model.componentId){
                for(const auto &el : model.params){
                    nL->addParam(el.first, el.second);
                }
            }
        }
    }

    c.setHighestNodeNumber(maxNode);
}

void setupBasic(Circuit& c, float timeStep){
    c.setTimeStep(timeStep);
}
