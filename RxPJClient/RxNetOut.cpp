#include			"RxNetOut.h"

HANDLE					hNetMutex = CreateMutex(NULL, false, NULL);
HANDLE					hUDSockMtx = CreateMutex(NULL, false, NULL);
std::list<RxPacket*>	toSend;
RxWSocket				sock;

void				AddToSendList(RxPacket* newPacket)
{
	WaitForSingleObject(hNetMutex, INFINITE);
	toSend.push_back(newPacket);
	ReleaseMutex(hNetMutex);
}

void				SendingLoop()
{
	int				ret;
	// a revoir peut etre 
	WaitForSingleObject(hNetMutex, INFINITE);
	while (toSend.size() > 0)
	{
		ret = sock.Send((char*)toSend.front(), sizeof(RxPacket));
		if (ret < 0)
			break;
		delete (toSend.front());
		toSend.pop_front();
	}
	ReleaseMutex(hNetMutex);
}
