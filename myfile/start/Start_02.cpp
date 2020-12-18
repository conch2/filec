#include <string>
#include <fstream>
#include <unistd.h>
#include <iostream>

class Base
{
public:
	std::string file_name;
	unsigned int s_time = 1500000;

	Base( std::string file_name );
	~Base();

protected:

private:

};

Base::Base( std::string file_name )
{
	this->file_name = file_name;
}

Base::~Base(){}

int main( int argc, char *argv[] )
{
	return 0;
}