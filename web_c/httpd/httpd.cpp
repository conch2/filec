
/***********************************************
#
#      Filename: httpd.cpp
#
#        Author: 
#   Description: ---
#        Create: 2021-01-05 03:07:46
# Last Modified: 2021-01-05 03:07:46
***********************************************/
#include <stdio.h>
#include "myhttpd.h"

int main(const int argc, const char *argv[] )
{
	myhtp::Httpd http(6686);

	http.run();

	return 0;
}

