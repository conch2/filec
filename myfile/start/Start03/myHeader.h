
#ifndef MYHEADER_H
#define MYHEADER_H

class Base
{
public:
	enum _NUM{ ZERO, ONE, TWO, THREE };

	// 如使用该构造器刷新间隔默认设置为1秒
	Base( char const* fp );
	Base( const std::string file_name );
	Base( const std::string file_name, const unsigned long s_time );
	// 析构器
	virtual ~Base();
	// 开始进行
	bool run( void );
	inline void create_bar( void );
	// 返回值为true时代表文件是一个新的文件
	bool latest_file( void );
	bool child_thread( void );

protected:

private:
	short apprun;
	bool newfile;
	// 文件数量
	short fileNum;
	// 刷新间隔，单位微秒
	unsigned long s_time;
	// 文件类型，如C++、C、Java
	std::string file_type;
	// 文件名
	std::string file_name;
	// 文件后缀
	std::string fileSuffix;
	// 备份文件后缀
	const char backupSuffix[0x10] = ".bar";

};

/************
# 该类是一个数据类型
# 它遵循先进先出的原则
*************/
template <typename T>
class Fifo
{
public:
	Fifo( const unsigned int size )
	{
		this->fp = 0x0;
		this->size = size;
		this->dataSize = 0x0;
		this->data = new T[this->size];
	}

	~Fifo()
	{
		delete[] this->data;
	}

	void push( const T ch )
	{
		unsigned int num = this->size - (this->fp + this->dataSize);

		if( this->fp + this->dataSize < this->size )
		{
			data[this->fp+this->dataSize] = ch;
			this->dataSize++;
		}
		else if( this->fp > num )
		{
			this->data[num] = ch;
			this->dataSize++;
		}
		else
		{
			throw 0x01;
		}
	}

	T pop( void )
	{
		if( !this->dataSize )
		{
			throw 0x02;
		}
		if( this->fp < this->size-1 )
		{
			this->dataSize--;
			return this->data[fp++];
		}
		else 
		{
			this->dataSize--;
			this->fp = 0x0;
			return this->data[this->size-1];
		}
	}

protected:

private:
	T *data;
	unsigned int fp;
	unsigned int size;
	unsigned int dataSize;

};

#endif
