
#ifndef __TcpServer_H__
#define __TcpServer_H__

#define SERVER_PORT 8080
#define LISTEN_MAX	1024
#define DATA_MAX	128

#ifndef false
	#define false	0
#endif
#ifndef true
	#define true 1
#endif
#define SOCKE_ERROR (-1)
#include<string.h>

enum MessageCmd
{
	CMD_LOGIN = 1,
	CMD_LOGIN_RESULT,
	CMD_LOGOUT,
	CMD_LOGOUT_RESULT,
	CMD_ERROR
};

class DataHeader
{
public:
	int datalen;
	int cmd;
};
class MessageData : public DataHeader
{
public:
	MessageData(int cmd, const char *buf)
	{
		datalen = sizeof(MessageData);
		this->cmd = cmd;
		memset(data, 0, sizeof(data));
		strcpy(data,  buf);
	}
	MessageData()
	{

	}
	char data[DATA_MAX];
};
enum TcpStatus
{
	TCP_OK = 0,
	TCP_ERR
};

#endif