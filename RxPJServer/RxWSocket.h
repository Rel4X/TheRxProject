#pragma once

#include		<string>
#include		<Windows.h>
//#include		<WinSock2.h>

#pragma			comment(lib, "ws2_32.lib")

class			RxWSocket
{
private:
	static int			sockCount;
	int					__socket;
	struct sockaddr_in	__infos;
	bool				__connected;

public:
	RxWSocket(void);
	~RxWSocket(void);

public:
	int							GetSocket() const		{ return (this->__socket); }
	const struct sockaddr_in&	GetSockInfos() const	{ return (this->__infos); }
	bool						IsConnected() const		{ return (this->__connected); }

public:
	void		Disconnect();
	bool		ServerInit(const std::string&);
	RxWSocket*	Accept();
	bool		Connect(const std::string&, const std::string&);
	int			Receive(char*, int);
	int			Send(char*, int);

private:
	void		Reset();
	int			WinInit();
};
