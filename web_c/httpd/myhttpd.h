
/***********************************************
#
#      Filename: myhttpd.h
#
#        Author: 
#   Description: ---
#        Create: 2021-01-05 03:12:35
# Last Modified: 2021-01-05 03:12:35
***********************************************/

#ifndef MYHTTPD_H
#define MYHTTPD_H
#include<netinet/in.h>

namespace myhtp
{

typedef unsigned int UINT;

class Httpd
{
public:
	Httpd(const UINT post);
	virtual ~Httpd();
	int startup(void);
	void run(void);
	int get_len(void);

protected:

private:
	UINT port;
	int  server_sock;
	int  client_sock;
	socklen_t client_len;
	socklen_t server_len;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	friend void *accept_request(void *);

};

}      // end myhtp

#endif // MYHTTPD_H
