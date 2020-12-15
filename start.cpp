#include <fstream>
#include <unistd.h>
#include <iostream>

int main( int argc, char *argv[] )
{
	if(argc < 2)
	{
		std::cout << "请加入要编译的文件名\n";
		return 1;
	}

	// 程序暂停时间
	unsigned int s_time = 500000;

	bool fileOpen = false,
		fileOE = false;
	short i = 5;
	char in, brck;

	std::string comm = "g++ ",  // 存储编译指令
		copyFile = "cp ";       // 备份指令

	std::ifstream file_a;

	comm += argv[1];
	// insert( 插入的位置, 要插入的字符或字符串 )
	copyFile.insert( 3, argv[1] );
	copyFile += ' ';
	copyFile += argv[1];
	copyFile += ".bar";

	std::cout << copyFile.c_str() << '\n';

	while( i-- )
	{
		// F_OK: 文件是否存在？ R_OK: 可读？ W_OK: 可写？ X_OK: 可执行？
		if( !access( argv[1], F_OK ))
		{
			std::cout << "File Ok" << '\n';
			file_a.open( argv[1], std::ios::in );
			if( !file_a )
			{
				std::cout << "File Error" << '\n';
			}
			else
			{
				file_a.close();
			}
		}

		usleep( s_time );
	}

	// std::cout << i << '\n';

	file_a.close();

	return 0;
}
