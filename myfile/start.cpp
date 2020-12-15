#include <fstream>
#include <unistd.h>
#include <iostream>

int main( int argc, char *argv[] )
{
	bool fileOpen = false,
		fileOE = false;
	short i;
	char in, brck;
	std::string comm = "g++ ",
		copyFile = "cp ";

	if(argc < 2)
	{
		std::cout << "请加入要编译的文件名\n";
		return 1;
	}

	std::ifstream file_a;

	comm += argv[1];
	copyFile.insert( 3, argv[1] );
	copyFile += ' ';
	copyFile += argv[1];
	copyFile += ".bar";

	std::cout << copyFile.c_str() << '\n';

	while(true)
	{
		if( !fileOpen )
		{
			file_a.open( argv[1], std::ios::in );
			if( !file_a )
			{
				std::cout << "文件打开失败\n";
			}
			else 
			{
				if( system( copyFile.c_str() ) )
				{}
			}
		}

		usleep(500000);
		if( system( comm.c_str() ) )
		{
			break;
		}
		else
		{
			system( "./a.out" );
		}

		file_a.close();
	}

	file_a.close();

	return 0;
}
