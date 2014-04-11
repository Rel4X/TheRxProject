#include		<string>
#include		"RxWSocket.h"

int				RxWSocket::sockCount = 0;

RxWSocket::RxWSocket(void) :
__connected(false), __socket(0)
{
	memset(&this->__infos, 0, sizeof(this->__infos));
}

RxWSocket::~RxWSocket(void)
{
	if (this->__connected == true)
		this->Reset();
}

void			RxWSocket::Disconnect()
{
	this->Reset();
}

void			RxWSocket::Reset()
{
	if (this->__connected == true)
		closesocket(this->__socket);
	this->__connected = false;
	this->__socket = 0;
	memset(&this->__infos, 0, sizeof(this->__infos));
	if (RxWSocket::sockCount == 1)
	{
		RxWSocket::sockCount = 0;
		WSACleanup();
	}
	else if (RxWSocket::sockCount > 1)
		RxWSocket::sockCount -= 1;
}

int				RxWSocket::WinInit()
{
	WSADATA		WSAData;

	if (RxWSocket::sockCount == 0)
	{
		if (WSAStartup(MAKEWORD(2, 0), &WSAData) == 0)
		{
			RxWSocket::sockCount = 1;
			return (0);
		}
		return (-1);
	}
	RxWSocket::sockCount += 1;
	return (0);
}

bool			RxWSocket::ServerInit(const std::string& port)
{
	if (this->__connected == false)
	{
		this->WinInit();
		this->__infos.sin_family = AF_INET;
		this->__infos.sin_port = htons(atoi(port.c_str()));
		this->__infos.sin_addr.s_addr = INADDR_ANY;
		this->__socket = socket(AF_INET, SOCK_STREAM, 0);
		if (this->__socket == -1
			|| (bind(this->__socket, (const sockaddr*)&this->__infos, sizeof(struct sockaddr_in)) == -1)
			|| (listen(this->__socket, 42) == -1))
		{
			this->Reset();
			return (false);
		}
		this->__connected = true;
		return (true);
	}
	return (false);
}

RxWSocket*		RxWSocket::Accept()
{
	RxWSocket*	newSocket;
	int			size;

	if (this->__connected == true)
	{
		newSocket = new RxWSocket();
		size = sizeof(newSocket->__infos);
		newSocket->__socket = accept(this->__socket, (struct sockaddr*)&newSocket->__infos, &size);
		if (newSocket > 0)
		{
			newSocket->__connected = true;
			newSocket->WinInit();
			return (newSocket);
		}
	}
	this->Reset();
	return (0);
}

bool			RxWSocket::Connect(const std::string& ip, const std::string& port)
{
	if (this->__connected == false)
	{
		this->WinInit();
		this->__infos.sin_family = AF_INET;
		this->__infos.sin_port = htons(atoi(port.c_str()));
		this->__infos.sin_addr.s_addr = inet_addr(ip.c_str());
		this->__socket = socket(AF_INET, SOCK_STREAM, 0);
		if (connect(this->__socket, (const sockaddr*)&this->__infos, sizeof(struct sockaddr_in)) == 0)
		{
			this->__connected = true;
			return (true);
		}
		this->Reset();
		return (false);
	}
	return (false);
}

int				RxWSocket::Receive(char* buffer, int size)
{
	int			received;

	if (this->__connected == true && buffer != 0 && size > 0)
	{
		received = recv(this->__socket, buffer, size, 0);
		return (received);
	}
	return (-1);
}

int				RxWSocket::Send(char* buffer, int size)
{
	int			sent;

	if (this->__connected == true && buffer != 0 && size > 0)
	{
		sent = send(this->__socket, buffer, size, 0);
		if (sent == SOCKET_ERROR)
			return (-1);
		return (sent);
	}
	return (-2);
}
