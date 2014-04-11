#ifndef			_RXCLIENTUPDOWN_H_
# define		_RXCLIENTUPDOWN_H_

#include		"RxNetOut.h"
#include		"RxFileSystem.h"

extern			RxWSocket		upDownSock;

void			DownloadRoutin(RxPacket*);
void			AnsAskDwl(RxWSocket&, RxPacket&);

void			UploadRoutine(RxPacket*);
void			AnsAskUpl(RxWSocket&, RxPacket&);

#endif			// _RXCLIENTUPDOWN_H_