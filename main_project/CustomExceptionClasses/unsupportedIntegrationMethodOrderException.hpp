#ifndef UNSUPPORTEDINTEGRATIONMETHODORDEREXCEPTION
#define UNSUPPORTEDINTEGRATIONMETHODORDEREXCEPTION

#include <exception>
#include <string>

using namespace std;

class UnsupportedIntegrationMethodOrderException : public exception
{
private:
    const string exceptionType = "Currently no numerical integration method for inductors/capacitors supports this order.";
    string exceptionLocation;
public:
    UnsupportedIntegrationMethodOrderException(string location = "Unknown")
        : exceptionLocation{location}
    {}

    const char * what () const throw () override
    {
        string message = exceptionType + "Location: " + exceptionLocation;
    	return message.c_str();
    }
};

#endif