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
#include <component/bjt.hpp>
#include <component/mosfet.hpp>
#include <component/voltageControlledVoltageSource.hpp>
#include <component/currentControlledVoltageSource.hpp>
#include <component/voltageControlledCurrentSource.hpp>
#include <component/currentControlledCurrentSource.hpp>
#include <component/opamp.hpp>

#include "input.hpp"

ModelStatement::ModelStatement(vector<string> args){
    if(args.size() < 2){
        cerr << "Invalid model statement: A model statement must at least contain a name and a component type." << endl;
        exit(1);
    }

    componentId = args[0];
    string componentStr = args[1];

    //convert componentStr to uppercase (as netlist case insensitive)
    for_each(componentStr.begin(), componentStr.end(), [](char &c){
        c = toupper(c);
    });

    auto it = componentTable.find(componentStr);
    if(it == componentTable.end()){ //if not found
        cerr << "This component is not supported. Component: " << componentStr <<endl;
        exit(1);
    }
    componentName = it->second;

    const string delimiter = "="; //separates paramName and paramValue
    string param{}, paramName{}, paramValueStr{};
    float paramValue{};

    // checks for PMOS
    if(componentName == component::PMOS){
        params.emplace(static_cast<int>(mosfetParamType::TYPE), 0);
    }else if(componentName == component::PNP){
        params.emplace(static_cast<int>(bjtParamType::TYPE), 0);
    }

    for(int i{2}; i<args.size(); i++){
        param = args[i];

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
            if(it1 == mosfetParamTable.end()){ //if not found
                cerr << componentStr << " doesn't support the parameter " << paramName <<endl;
                exit(1);
            }
            paramId = static_cast<int>(it1->second);
        }else if(componentName == component::NPN || componentName == component::PNP){
            auto it1 = bjtParamTable.find(paramName);
            if(it1 == bjtParamTable.end()){ //if not found
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

    vector<int> nodes;
    string lineString{};
    int maxNode = 0;
    bool containsEnd = false; //A valid netlist must contain a .end statement
    bool containsAnalysisType = false; //A valid netlist must contain a simulation type statement e.g. .tran
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

        //special inputs (not circuit components)
        if(compTypeC == "."){
            //remove "."
            name.erase(0, 1);

            //convert name to uppercase (as netlist case insensitive)
            for_each(name.begin(), name.end(), [](char &c){
                c = toupper(c);
            });

            if(name == "MODEL"){
                modelStatements.emplace_back(args);
            }else if(name == "TRAN"){
                if(args.size() < 2){
                    cerr << "Not enough arguments supplied for .tran" <<endl;
                    exit(1);
                }else if(Component::getValue(args[0]) <= 0 || Component::getValue(args[1]) <= 0){
                    cerr << "TSPEP and TSTOP must be greater than 0" <<endl;
                    exit(1);
                }else if(args.size() == 2){
                    c.setTStep(Component::getValue(args[0])); //TSTEP
                    c.setSimulationTime(Component::getValue(args[1])); //TSTOP
                    c.setTStart(0); //TSTART
                    c.setMaxTimeStep((c.getSimulationTime()/50 < 1) ? c.getSimulationTime()/50 : 1); //TMAX
                }else if(args.size() == 3){
                    if(Component::getValue(args[2]) < 0){
                        cerr << "TSTART cannot be smaller than zero" <<endl;
                        exit(1);
                    }
                    c.setTStep(Component::getValue(args[0])); //TSTEP
                    c.setSimulationTime(Component::getValue(args[1])); //TSTOP
                    c.setTStart(Component::getValue(args[2])); //TSTART
                    c.setMaxTimeStep((c.getSimulationTime()/50 < 1) ? c.getSimulationTime()/50 : 1); //TMAX
                }else{
                    if(Component::getValue(args[2]) < 0 || Component::getValue(args[3]) <= 0){
                        cerr << "START cannot be smaller than zero and TMAX must be greater than zero" <<endl;
                        exit(1);
                    }
                    c.setTStep(Component::getValue(args[0])); //TSTEP
                    c.setSimulationTime(Component::getValue(args[1])); //TSTOP
                    c.setTStart(Component::getValue(args[2])); //TSTART
                    c.setMaxTimeStep(Component::getValue(args[3])); //TMAX
                }
                containsAnalysisType = true;
            }else if(name == "END"){
                containsEnd = true;
                break; //ignore anything that comes after a .end statement
            }else if(name == "OPTIONS"){
                setupOptions(c, args);
            }else{
                cerr << "Unsuported netlist statement. Statement: " << compTypeC + name <<endl;
                exit(1);
            }

            continue; //don't execute the rest (not relevant for .model statements)
        }else if(compTypeC == "*"){
            continue; //ignore comments
        }

        // expected inputs are in comments, anything after the -> is optional
        if(compTypeC == "R" || compTypeC == "r"){
            // + - resistance
			c.addComponent<Resistor>(name, args);
		}else if(compTypeC == "V" || compTypeC == "v"){
            // + - voltage
			c.addComponent<VoltageSource>(name, args);
		}else if(compTypeC == "I" || compTypeC == "i"){
            // + - current
			c.addComponent<CurrentSource>(name, args);
		}else if(compTypeC == "L" || compTypeC == "l"){
            // + - inductance
			c.addComponent<Inductor>(name, args);
            c.incrementInductorNumber(); //Used for DC operating point analysis
		}else if(compTypeC == "C" || compTypeC == "c"){
            // + - capacitance
			c.addComponent<Capacitor>(name,args);
		}else if(compTypeC =="D" || compTypeC == "d"){
            // + - -> IS N VT
			c.addComponent<Diode>(name,args);
		}else if(compTypeC =="Q" || compTypeC == "q"){
			c.addComponent<BJT>(name,args);
		}else if(compTypeC =="M" || compTypeC == "m"){
            // D G S modelName
			c.addComponent<Mosfet>(name,args);
		}else if(compTypeC == "E" || compTypeC == "e"){
            c.addComponent<VoltageControlledVoltageSource>(name, args);
        }else if(compTypeC == "H" || compTypeC == "h"){
            c.addComponent<CurrentControlledVoltageSource>(name, args);
        }else if(compTypeC == "G" || compTypeC == "g"){
            c.addComponent<VoltageControlledCurrentSource>(name, args);
        }else if(compTypeC == "F" || compTypeC == "f"){
            c.addComponent<CurrentControlledCurrentSource>(name, args);
        }else if(compTypeC == "X" || compTypeC == "x"){
            c.addComponent<OpAmp>(name, args);
        }else{
            cerr << "Unsuported netlist statement. Statement: " << compTypeC <<endl;
            exit(1);
        }
		
        nodes = c.getLastComponent()->getNodes();
        for(int n : nodes){
            maxNode = n>maxNode?n:maxNode;
        } 
    }

    //Terminate with error message when the netlist is not terminated properly by an .end statement
    if(!containsEnd){
        cerr << "Netlist invalid: A netlist must be terminated with a '.end' statement" <<endl;
        exit(1);
    }
    //Terminate with error message when the netlist does not specify a simulation type
    if(!containsAnalysisType){
        cerr << "Netlist invalid: A nestlist must contain a simulation type e.g. '.tran'" <<endl;
        exit(1);
    }

    //add model params to components
    auto nonLinears = c.getNonLinearsRef();
    for(const auto &model : modelStatements){
        for(const auto &nL : nonLinears){
            if(nL->getModelName() == model.componentId){
                for(const auto &el : model.params){
                    nL->addParam(el.first, el.second);
                }
            }
        }
    }

    //add timestep from tran params to capacitor/inductor
    auto vcUpdatables = c.getVCUpdatablesRef();
    for(const auto &comp : vcUpdatables){
        comp->setTimeStep(c.getTStep()); //CAREFULL: Currently we are using the printing interval (tStep) as the static timestep. This is likely going to change for a dynamic timestep
    }

    c.setHighestNodeNumber(maxNode);
}

void setupOptions(Circuit& c, vector<string>& args){
    if(args.size() == 0){
        cerr << "Invalid options statement. An options statement must have at least one parameter." <<endl;
        exit(1);
    }

    const string delimiter = "="; //separates paramName and paramValue

    string paramName{}, paramValueStr{};
    float paramValue{};

    for(auto arg : args){
        paramName = arg.substr(0, arg.find(delimiter));
        paramValueStr = arg.substr(arg.find(delimiter)+1);

        //convert paramName to uppercase (as netlist case insensitive)
        for_each(paramName.begin(), paramName.end(), [](char &c){
	        c = toupper(c);
        });

        //convert value to float
        paramValue = Component::getValue(paramValueStr);

        //add any new option here
        if(paramName == "GMIN"){
            c.setMinPNConductance(paramValue);
        }else if(paramName == "ABSTOL"){
            c.setAbstol(paramValue);
        }else if(paramName == "IMAX"){
            c.setMaxNewtonRaphsonCount(paramValue);
        }else{
            cerr << "Invalid OPTIONS command. Unsupported option: " << paramName << endl;
            exit(1);
        }
    }
}