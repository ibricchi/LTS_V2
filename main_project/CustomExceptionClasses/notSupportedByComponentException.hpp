#ifndef NOTSUPPORTEDBYCOMPONENTEXCEPTION
#define NOTSUPPORTEDBYCOMPONENTEXCEPTION

#include <exception>
#include <string>

using namespace std;

class NotSupportedByComponentException : public exception
{
private:
    const string exceptionType = "This method is not supported by the component.";
    string exceptionLocation;
public:
    NotSupportedByComponentException(string location = "Unknown")
        : exceptionLocation{location}
    {}

    const char * what () const throw () override
    {
        string message = exceptionType + "Location: " + exceptionLocation;
    	return message.c_str();
    }
};

#endif