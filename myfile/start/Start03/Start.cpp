/*
 * @Author:
 * @Date: 2020-12-23 12:53
 * @Last Modified time: 2020-12-23 13:10
 * @Description:
 * */
#include <iostream>
#include "myHeader.h"

int main( int argc, char *argv[] )
{
	if( argc < 2 )
	{
		std::cout << "请以文件名为参数！" << std::endl;
		return Base::ONE;
	}

	Base base( argv[Base::ONE] );

	while( base.run() );

	return Base::ZERO;
}

