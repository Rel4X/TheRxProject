#ifndef			_RXNETOUT_H_
# define		_RXNETOUT_H_

#include				<Windows.h>
#include				<list>

#include				"RxWSocket.h"
#include				"RxProtocol.h"

#define		IP_SERVER	"127.0.0.1"

extern HANDLE					hNetMutex;
extern HANDLE					hUDSockMtx;
extern std::list<RxPacket*>		toSend;
extern RxWSocket				sock;

void							AddToSendList(RxPacket*);
void							SendingLoop();

#endif			// _RXNETOUT//