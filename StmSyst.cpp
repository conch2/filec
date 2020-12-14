#include<fstream>
#include<iostream>

struct Student
{
	char sex;
	std::string id;
	std::string name;
};

const char * FILE_NAME = "test.txt";

bool print();
bool addData();

int main( int argc, char *argv[] )
{
	short num = 0;

	while(num != 3)
	{
		std::cout << "1: 加载信息" << '\n'
			<< "2: 添加信息" << '\n'
			<< "3: 退出" << '\n';
		std::cin >> num;

		switch(num)
		{
		case 1:
			print();
			break;
		case 2:
			addData();
			break;
		case 3:
			break;
		default:
			std::cout << "请规范输入" << '\n';
			break;
		}
	}

	return 0;
}

bool print()
{
	char ch;
	std::fstream data( FILE_NAME, std::ios::in |
		std::ios::app | std::ios::binary );

	if( !data )
	{
		std::cout << "文件打开失败！" << '\n';
		return false;
	}

	data.seekg(std::ios::beg);

	std::cout << '\n'
		<< "NAME     ID     SEX" << '\n';

	while( (ch = data.get()) != EOF )
	{
		std::cout << ch;
	}

	std::cout << '\n';
	data.close();

	return true;
}

bool addData()
{
	bool i = true;
	std::string chv;
	std::fstream data( FILE_NAME, std::ios::app );
	if( !data )
	{
		std::cerr << "文件打开失败！";
		return false;
	}

	std::cout << "请输入姓名：";
	std::cin >> chv;

	while( i )
	{
		if( chv.size() < 3 )
		{
			std::cout << "名字太短了！" << '\n';
			std::cin >> chv;
			i = true;
		} 
		else if( chv.size() > 12)
		{
			std::cout << "名字太长了！" << '\n';
			std::cin >> chv;
			i = true;
		}
		else 
		{
			i = false;
		}
	}

	data << chv << ' ';

	std::cout << "请输入ID：";
	std::cin >> chv;

	if( chv.size() != 8 )
	{
		do {
			std::cout << "ID应为8个字符！" << '\n'
				<< "请重新输入：";
				std::cin >> chv;
		} while( chv.size() < 6 );
	}

	data << chv << ' ';


	char c;
	i = 1;
	std::cout << "请输入性别：";
	std::cin >> c;

	while( i )
	{
		switch( c )
		{
		case 'F':
		case 'f':
			c = 'F';
			i = 0;
			break;
		case 'M':
		case 'm':
			c = 'M';
			i = 0;
			break;
		default:
			std::cout << "请规范输入（F/N）：";
			std::cin >> c;
			break;
		}
	}

	data << c;

	std::cout << '\n';

	data << '\n';
	data.close();

	return true;
}
