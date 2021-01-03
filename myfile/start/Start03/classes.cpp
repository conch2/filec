#include <string>
#include <thread>
#include <fstream>
#include <unistd.h>
#include <iostream>
#include <sys/time.h>
#include "myHeader.h"

Base::Base( char const* fp ) : Base( std::string(fp), 0xF4240 )
{
	//创建一个新的Base对象覆盖this所在的内存
	//new(this) Base(file_name, 0xF4240);
}

Base::Base( const std::string file_name ) : Base( file_name, 0xF4240 ) {}

Base::Base( const std::string file_name, const unsigned long s_time )
{
	this->apprun = 0;
	this->newfile = true;

	unsigned int len = file_name.length(),
			   wat = file_name.find( '.' );
	this->s_time = s_time;
	this->file_name = file_name;
	this->fileSuffix = this->file_name.substr( wat, len );
	this->file_name.erase( wat, len );
	if( fileSuffix == ".cpp" )
	{
		this->file_type = "C++";
		//std::cout << this->file_type;
	} else if( fileSuffix == ".c")
	{
		this->file_type = "C";
		//std::cout << this->file_type;
	}
	std::cout << "Start " << this->file_name << this->fileSuffix << std::endl;
}

Base::~Base()
{
	if( !access( (file_name+fileSuffix).c_str(), F_OK ) )
	{
#ifdef __linux
		if( system( ("rm "+file_name+fileSuffix+backupSuffix).c_str() ) )
		{
#endif
			std::cout << "备份文件无法删除！" << std::endl;
		}
	}
}

bool Base::run( void )
{
	char ch;

	std::cout << "开始操作...\n" << std::endl;

	while( true )
	{
		/* F_OK: 文件是否存在？ W_OK: 文件是否可写？
		 * * R_OK: 文件是否可读？ X_OK: 文件是否可执行？*/
		if( !access( (file_name+fileSuffix).c_str(), F_OK ) )
		{
			this->create_bar();
			break;
		} else 
		{
			std::cout << "无法检查到该文件，请检查文件名称是否正确" << '\n'
				<< "输入任意字符继续，q退出" << std::endl;
			system( "stty -icanon" );
			ch = std::cin.get();
			system( "stty icanon" );
			if( ch == 'q')
				return false;
		}
	}

	// 使用了lambda表达式
	std::thread c_thread([&]() -> bool {
			return this->child_thread();
			});

	bool i = true;

	while( !(newfile = this->latest_file()) )
	{
		if( (this->apprun == THREE || this->apprun == -1) && i )
		{
			std::cout << "\n程序已退出\n正在继续监视文件" << std::endl;

			i = false;
		}
		usleep( this->s_time );
	}
	this->create_bar();

	if( newfile )
	{
		std::cout << "\n文件发生变化，正在重新编译运行" << std::endl;
	}

	while( !(this->apprun) )
	{
		//std::cout << ONE << std::endl;
		usleep( 0x3e8 );
	}

	if( this->apprun == TWO )
	{
#ifdef __linux
		system( ("ps -ef|grep '**./"+
				file_name+
				"**'|grep -v 'grep'"+
				"|awk '{print $2}'|xargs kill -9").c_str() );
#endif
	}
	this->apprun = 0;

	if( c_thread.joinable() )
	{
		// 堵塞主线程等待子线程结束
		c_thread.join();
	} else 
	{
		std::cout << "子线程无法join" << std::endl;
	}

	fflush( stdin );
	//std::cin.ignore( 0x3e8, '\n' );

	return true;
}

/* 子线程
 * */
bool Base::child_thread( void )
{
	std::string bj_type;
	std::string runfile;
	unsigned long long time;
	struct timeval start_tv, acc_tv;

	if( this->file_type == "C++" )
		bj_type = "g++ ";
	else if( this->file_type == "C" )
		bj_type = "gcc ";

	this->apprun = ONE;

	gettimeofday( &start_tv, NULL );
	if( system( (bj_type+this->file_name+fileSuffix
					+" -o "+this->file_name).c_str() ) )
	{
		gettimeofday( &acc_tv, NULL );
		time = acc_tv.tv_sec * 1000 + acc_tv.tv_usec / 1000;
		time -= start_tv.tv_sec * 1000 + start_tv.tv_usec / 1000;
		this->apprun = -1;
		std::cout << "\n编译错误，请检查源代码！" << "总用时：" <<
			time << "ms" << std::endl;
		return false;
	}

	gettimeofday( &acc_tv, NULL );
	time = acc_tv.tv_sec * 1000 + acc_tv.tv_usec / 1000;
	time -= start_tv.tv_sec * 1000 + start_tv.tv_usec / 1000;
	std::cout << "\n编译完成，开始运行...  编译总用时：" << time << 
		"ms\n" << std::endl;

	runfile = "./" + file_name;

	this->apprun = TWO;
	gettimeofday( &start_tv, NULL );
	system( runfile.c_str() );
	gettimeofday( &acc_tv, NULL );
	time = acc_tv.tv_sec * 1000 + acc_tv.tv_usec / 1000;
	time -= start_tv.tv_sec * 1000 + start_tv.tv_usec / 1000;
	std::cout << "\n程序运行总用时：" << time << 
		"ms" << std::endl;
	this->apprun = THREE;

	return true;
}

void Base::create_bar( void )
{
	if( system( ("cp "+file_name+fileSuffix+' '+
					file_name+fileSuffix+backupSuffix).c_str() ) )
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
	std::string fileName = file_name+fileSuffix;

	// 如果文件不存在或不能访问access( filename, F_OK )返回-1，否则返回0
	if( access( (fileName+backupSuffix).c_str(), F_OK ) )
	{
		return true;
	}

	std::ifstream bar( (fileName+backupSuffix).c_str(), std::ios::in );
	if( !bar )
	{
		system( ("rm "+fileName+backupSuffix).c_str() );
		return true;
	}

	std::ifstream in( fileName.c_str(), std::ios::in );
	if( !in )
	{
		std::cout << "文件无法打开！" << std::endl;
		bar.close();
		return false;
	}

	//设置文件输出流指针指向文件尾
	in.seekg( ZERO, std::ios::end );
	bar.seekg( ZERO, std::ios::end );

	if( in.tellg() != bar.tellg() )
	{
		in.close();
		bar.close();
		return true;
	}

	//设置文件输出流指针指向文件头
	in.seekg( ZERO, std::ios::beg );
	bar.seekg( ZERO, std::ios::beg );

	std::string bar_len, in_len;

	do {
		getline( in, in_len );
		getline( bar, bar_len );
	} while( !in.eof() && !bar.eof() && in_len == bar_len );

	if( in.eof() && bar.eof() )
	{
		in.close();
		bar.close();
		return false;
	}

	in.close();
	bar.close();
	std::cout << "end" << std::endl;

	return true;
}
