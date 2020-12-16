/*
 * @Author:
 * @Date: 2020-12-15 23:36
 * @Last Modified time: 2020-12-16 13:40
 * @Description:
 * */

#include <string>
#include <thread>
#include <fstream>
#include <unistd.h>
#include <iostream>

class Base
{
public:
	bool newfile = true;
	// 刷新间隔，单位微秒
	unsigned long s_time;
	// 文件类型，如C++、C、Java
	std::string file_type;
	// 文件名，有后缀
	std::string file_name;
	char backupSuffix[0x10] = ".bar";

	enum _NUM{ ZERO, ONE, TWO };

	// 如使用该构造器刷新间隔默认设置为1秒
	Base( std::string file_name );
	Base( std::string file_name, unsigned long s_time );
	// 析构器
	~Base();
	// 开始进行
	void run( void );
	void create_bar( void );
	// 返回值为true时代表文件是一个新的文件
	bool latest_file( void );
	void child_thread( void );

protected:

private:

};

Base::Base( std::string file_name )
{
	// 设置时间默认为1秒
	this->s_time = 0xF4240;
	this->file_type = "C++";
	this->file_name = file_name;
}

Base::Base( std::string file_name, unsigned long s_time )
{
	this->s_time = s_time;
	this->file_type = "C++";
	this->file_name = file_name;
}

Base::~Base(){}

void Base::run( void )
{
	std::cout << "开始操作..." << std::endl;
	/* F_OK: 文件是否存在？ W_OK: 文件是否可写？
	 * R_OK: 文件是否可读？ X_OK: 文件是否可执行？*/
	if( !access( file_name.c_str(), F_OK ) )
	{
		if( (newfile = this->latest_file()) )
			this->create_bar();
	} else 
	{
		std::cout << "无法检查到该文件，请检查文件名称是否正确" << '\n'
			<< "输入任意字符继续" << std::endl;
		system( "stty -icanon" );
		std::cin.get();
		system( "stty icanon" );
	}

	// 使用了lambda表达式
	std::thread c_thread([&](){
			this->child_thread();
			});
	c_thread.join();
}

/* 子线程
 * */
void Base::child_thread( void )
{
	char ch;

	if( system( ("g++ "+this->file_name
					+" -o "+this->file_name).c_str() ) )
	{
		std::cout << "\n是否继续？（Y/N）";
		system( "stty -icanon" );
		std::cin >> ch;
		system( "stty icanon" );
		switch( ch )
		{
		case '\n':
		case 'Y':
		case 'y':
			break;
		case 'N':
		case 'n':
			break;
		default:
			std::cout << "\nWhat Word?";
			break;
		}
		return ;
	}
}

void Base::create_bar( void )
{
	if( system( ("cp "+file_name+' '+file_name+backupSuffix).c_str() ) )
	{
		std::cout << "无法创建备份文件！\n";
	}
}

/* 判断备份文件是否存在或者备份文件是否与原文件一致。
 *
 * Return:
 *     true: 原文件与备份文件不一致。
 *     false: 原文件与备份文件一致。
 * */
bool Base::latest_file( void )
{
	// 如果文件不存在或不能访问access( filename, F_OK )返回-1，否则返回0
	if( access( (file_name+backupSuffix).c_str(), F_OK ) )
	{
		return true;
	}

	std::ifstream bar( (file_name+backupSuffix).c_str(), std::ios::in );
	if( !bar )
	{
		system( ("rm "+file_name+backupSuffix).c_str() );
		return true;
	}

	std::ifstream in( file_name.c_str(), std::ios::in );
	if( !in )
	{
		std::cout << "文件无法打开！" << std::endl;
		bar.close();
		return false;
	}

	//设置文件输出流指针指向文件尾
	in.seekg( 0, std::ios::end );
	bar.seekg( 0, std::ios::end );

	if( in.tellg() != bar.tellg() )
	{
		in.close();
		bar.close();
		return true;
	}

	//设置文件输出流指针指向文件头
	in.seekg( 0, std::ios::beg );
	bar.seekg( 0, std::ios::beg );

	std::string bar_len, in_len;

	do {
		getline( in, in_len );
		getline( bar, bar_len );
		if( in.eof() || bar.eof() )
		{
			in.close();
			bar.close();
			return true;
		}
	} while( !in.eof() && !bar.eof() && in_len == bar_len );

	if( in.eof() && bar.eof() )
	{
		in.close();
		bar.close();
		return false;
	}

	in.close();
	bar.close();

	return true;
}

int main( int argc, char *argv[] )
{
	if( argc < Base::TWO )
	{
		std::cout << "请以文件名为参数！" << std::endl;
		return Base::ONE;
	}

	Base base( argv[Base::ONE] );

	base.run();

	return Base::ZERO;
}

