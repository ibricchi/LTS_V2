#ifndef CONTROLLEDSOURCE_HPP
#define CONTROLLEDSOURCE_HPP

#include <vector>
#include <string>

#include "component.hpp"

/*
    Acts as interface for dependent sources
*/
class ControlledSource: public Component{
protected:
    double gain;

public:
    ControlledSource(string _name, vector<string> args, vector<double> extraInfo);
    // ControlledSource(string _name, double _voltage, int n1, int n2, int nc1, int nc2);
    virtual ~ControlledSource() = 0;

    void setupBasic(int n1, int n2, int nc1, int nc2);

    double getGain() const;

    //returns all 4 nodes (N+, N-, NC+, NC-)
    vector<int> getNodes() const override;
};

#endif