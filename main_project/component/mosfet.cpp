#include <string>
#include <vector>
#include <cmath>

#include "mosfet.hpp"

Mosfet::Mosfet(string name, vector<string> args, vector<double> extraInfo)
    :Component{name}
{
    int nd = stoi(args[0]); //drain
    int ng = stoi(args[1]); //gate
	int ns = stoi(args[2]); //source
    int nb = stoi(args[3]); //body

	subComponents = 3;
	nodes.push_back(nd);
	nodes.push_back(ng);	
	nodes.push_back(ns);
	nodes.push_back(nb);
	
	//initialize conductances/current with an initial guess of vgs, vds = 0
	updateVals(0, 0);

	types.push_back(componentType::conductanceSource);
	types.push_back(componentType::currentSource); //both because of the current source and the voltage controlled current source in the companion model
	types.push_back(componentType::nonLinear);
}

double Mosfet::getConductance() const{
	return compConductance;
}

double Mosfet::getGm() const{
	return gm;
}

double Mosfet::getCurrent() const{
	 //nMOS: direction of current source is opposite to direction of the mosfet
	 //(D = top node)
	//  if(type == mosfetType::NMOS){
	// 	 return -compCurrent;
	//  }else{
	// 	 return compCurrent;
	//  }

	//how will we get current from the dependent source (used same method so far)?
	//probably have to use type checking...
}

double Mosfet::getGain() const{
    return gain;
}

double Mosfet::getTotalCurrent(double voltage, int order){
	return NAN; //not yet implemented
}

void Mosfet::updateVals(double vgs, double vds){
	operatingRegion region = getOperatingRegion(vgs, vds);

	if(type == mosfetType::NMOS){
		if(region == operatingRegion::CUTOFF){
			compConductance = 0;
			gm = 0;
			compCurrent = 0;
		}else if(region == operatingRegion::SATURATION){
			double id = k * pow(vgs-vt, 2) * (1 + vds/va);

			compConductance = id/va;
			gm = sqrt(2 * k * id);

			compCurrent = id - gm*vgs - compConductance*vds;
		}else{
			double id = k * (2 * (vgs - vt) * vds - pow(vds, 2));

			compConductance = k * ((vgs - vt) - vds);
			gm = k * vds;

			compCurrent = id - gm*vgs - compConductance*vds;
		}
	}else{
		double vsg = -vgs;
		double vsd = -vds;
		if(region == operatingRegion::CUTOFF){
			compConductance = 0;
			gm = 0;
			compCurrent = 0;
		}else if(region == operatingRegion::SATURATION){
			double is = k * pow(vsg-vt, 2) * (1 + vsd/va);

			compConductance = is/va;
			gm = sqrt(2 * k * is);

			compCurrent = is - gm*vsg - compConductance*vsd;
		}else{
			double is = k * (2 * (vgs - vt) * vds - pow(vds, 2));
			
			compConductance = k * ((vsg - vt) - vsd);
			gm = k * vsd;

			compCurrent = is - gm*vsg - compConductance*vsd;
		}
	}
}

vector<int> Mosfet::getNodes() const{
    return nodes;
}

operatingRegion Mosfet::getOperatingRegion(double vgs, double vds) const{
	if(type == mosfetType::NMOS){
		if(vgs <= vt){
			return operatingRegion::CUTOFF;
		}else if(vds >= vgs-vt && vgs > vt){
			return operatingRegion::SATURATION;
		}else{
			return operatingRegion::TRIODE;
		}
	}else{
		if(vgs >= vt){
			return operatingRegion::CUTOFF;
		}else if(vds <= vgs-vt && vgs < vt){
			return operatingRegion::SATURATION;
		}else{
			return operatingRegion::TRIODE;
		}
	}
}