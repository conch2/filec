#include <string>
#include <fstream>
#include <unistd.h>
#include <iostream>

int main( int argc, char *argv[] )
{
	std::string comm = "g++ ",  // 存储编译指令
		copyFile = "cp ",       // 备份指令
		barName = argv[1];
	if( argc < 2 )
	{
		std::cout << "请加入要编译的文件名" << std::endl;
		return 1;
	} 
	comm += argv[1];
	if( argc ==  2 )
	{
		std::cout << "编译生成的可执行文件将使用默认名称：a.out" << std::endl;
	} else 
	{
		std::cout << "编译生成的可执行文件将使用该名称："
			<< argv[2] << std::endl;
	}

	// 程序暂停时间
	unsigned int s_time = 1500000;

	bool fileOpen = false,
		fileOE = false;
	short i = 4;
	char in, brck;

	std::ifstream file_a, bar;

	// insert( 插入的位置, 要插入的字符或字符串 )
	copyFile.insert( 3, argv[1] );
	copyFile += ' ';
	barName += ".bar";
	copyFile += barName;

	//std::cout << copyFile.c_str() << '\n';

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
			} else
			{
				if( !access( barName.c_str(), F_OK ) )
				{
					bar.open( barName.c_str(), std::ios::in );
					if( !bar )
					{
						std::cout << "bar file Error" << '\n';
					}

					bar.seekg( 0, std::ios::end );
					file_a.seekg( 0, std::ios::end );
					std::cout << "file_a size:" << file_a.tellg()
						<< "bar size:" << bar.tellg() << std::endl;

					if( file_a.tellg() != bar.tellg() )
					{
						system( copyFile.c_str() );
						std::cout << "成功创建备份文件" << std::endl;
					} else 
					{
						bar.seekg( 0, std::ios::beg );
						file_a.seekg( 0, std::ios::beg );
						while( !bar.eof() || !file_a.eof() )
						{
							if( bar.get() != file_a.get() ||
									bar.eof() ^ file_a.eof() )
							{
								system( copyFile.c_str() );
								std::cout << "b成功创建备份文件"
									<< std::endl;
								break;
							} 
						}
					}
					bar.close();
				} else 
				{
					system( copyFile.c_str() );
					std::cout << "成功创建备份文件" << std::endl;
				}

				file_a.close();
			}

			system( comm.c_str() );
			system( "./a.out" );

			usleep( s_time );
		} else 
		{
			std::cout << "无法检查到该文件，请检查文件名称是否正确\n" 
				<< "输入任意字符回车继续" << std::endl;
			std::cin >> in;
		}
	}

	// std::cout << i << '\n';

	file_a.close();
	bar.close();

	return 0;
}
