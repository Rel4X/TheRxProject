#include		"RxClientUpDown.h"

RxWSocket				upDownSock;

/**** DOWNLOAD ****/
void				DownloadRoutine(RxPacket* p)
{
	RxFileSystem	fs;
	RxInitUpDown	iud;
	RxUpDownPacket	udp;

	memcpy(&iud, p->data, sizeof(iud));
	if (fs.BufferizeFile(iud.data) == true)
	{
		int			part;
		int			nParts;
		int			offset;
		char*		fContent;

		part = 1;
		nParts = fs.GetBufferizedFileSize() / SIZE_UPDN_DATA;
		if (fs.GetBufferizedFileSize() % SIZE_UPDN_DATA > 0)
			nParts += 1;
		fContent = fs.GetBufferizedFile();
		offset = 0;
		while (offset < fs.GetBufferizedFileSize())
		{
			memset(&udp, 0, sizeof(udp));
			udp.type = T_UPDN_TRAN;
			udp.part = part;
			udp.totalParts = nParts;
			udp.sockN = iud.sock;
			if (part < nParts)
			{
				memcpy(udp.data, fContent + offset, SIZE_UPDN_DATA);
				udp.partSize = SIZE_UPDN_DATA;
				part = part + 1;
				offset = offset + SIZE_UPDN_DATA;
			}
			else
			{
				memcpy(udp.data, fContent + offset, fs.GetBufferizedFileSize() % SIZE_UPDN_DATA);
				udp.partSize = fs.GetBufferizedFileSize() % SIZE_UPDN_DATA;
				if (udp.partSize == 0 && offset < fs.GetBufferizedFileSize())
				{
					udp.partSize = SIZE_UPDN_DATA;
					offset = offset + SIZE_UPDN_DATA;
				}
				else
					offset = offset + (fs.GetBufferizedFileSize() % SIZE_UPDN_DATA);
			}
			if (upDownSock.Send((char*)&udp, sizeof(udp)) <= 0)
				break;
			Sleep(1);
		}
	}
	else
	{
		udp.type = T_UPDN_DL_BUFF_FAIL;
		udp.sockN = iud.sock;
		upDownSock.Send((char*)&udp, sizeof(udp));
	}
	delete (p);
	upDownSock.Disconnect();
	ReleaseMutex(hUDSockMtx);
}

void				AnsAskDwl(RxWSocket& sock, RxPacket& p)
{
	RxPacket*		np;
	RxPacket*		cpy;

	np = new RxPacket();
	memset(np, 0, sizeof(*np));
	if (WaitForSingleObject(hUDSockMtx, 10) == WAIT_OBJECT_0)
	{
		if (upDownSock.Connect(IP_SERVER, SV_UPDN_PORT) == true)
		{
			np->type = T_ANS_DOWNLOAD_OK;
			cpy = new RxPacket();
			memcpy(cpy, &p, sizeof(p));
			if (CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)DownloadRoutine, (LPVOID)cpy, 0, NULL) > 0)
			{
				AddToSendList(np);
				return ;
			}
		}
	}
	np->type = T_ANS_DOWNLOAD_FAIL;
	AddToSendList(np);
}
/**** FIN DOWNLOAD ****/

/**** UPLOAD ****/
void				UploadRoutine(RxPacket* p)
{
	fd_set			fdread;
	RxUpDownPacket	toRead;
	int				offset;
	int				lastPartReceived;
	bool			finished;
	struct timeval	timer;
	int				rdRet;
	RxFileSystem	fs;
	RxInitUpDown	iud;
	std::string		filename;
	bool			dlOk;

	dlOk = false;
	offset = 0;
	lastPartReceived = 0;
	finished = false;
	timer.tv_sec = SELECT_TIMEOUT;
	timer.tv_usec = 0;
	memcpy((char*)&iud, p->data, sizeof(iud));
	filename = iud.data;
	if (fs.DoesThisFileExist(filename))
	{
		finished = true;
		dlOk = true;
	}
	while (finished == false)
	{
		FD_ZERO(&fdread);
		FD_SET(upDownSock.GetSocket(), &fdread);
		if (select(42, &fdread, NULL, NULL, &timer) > 0)
		{
			if (FD_ISSET(upDownSock.GetSocket(), &fdread))
			{
				if (offset == 0)
					memset(&toRead, 0, sizeof(toRead));
				rdRet = upDownSock.Receive((char*)&toRead + offset, sizeof(toRead) - offset);
				if (rdRet <= 0)
					break;
				if (rdRet + offset == sizeof(RxUpDownPacket))
				{
					offset = 0;
					if (toRead.part == lastPartReceived + 1 && toRead.type == T_UPDN_TRAN)
					{
						if (fs.AppendToAFile(filename, toRead.data, toRead.partSize) == true)
						{
							lastPartReceived += 1;
							if (lastPartReceived == toRead.totalParts)
							{
								dlOk = true;
								break;
							}
							std::cout << toRead.part << " / " << toRead.totalParts << std::endl;
						}
					}
					else
						break;
				}
				else
					offset = offset + rdRet;
			}
			else
				break;
		}
		else
			break;
	}
	if (dlOk == false)
		fs.DeleteAFile(filename);
	delete (p);
	upDownSock.Disconnect();
	ReleaseMutex(hUDSockMtx);
}

void				AnsAskUpl(RxWSocket& sock, RxPacket& p)
{
	RxPacket*		np;
	RxPacket*		cpy;

	np = new RxPacket();
	memset(np, 0, sizeof(*np));
	if (WaitForSingleObject(hUDSockMtx, 10) == WAIT_OBJECT_0)
	{
		if (upDownSock.Connect(IP_SERVER, SV_UPDN_PORT) == true)
		{
			np->type = T_ANS_UPLOAD_OK;
			cpy = new RxPacket();
			memcpy(cpy, &p, sizeof(p));
			if (CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)UploadRoutine, (LPVOID)cpy, 0, NULL) > 0)
			{
				AddToSendList(np);
				return ;
			}
		}
	}
	np->type = T_ANS_UPLOAD_FAIL;
	AddToSendList(np);
}
/**** FIN UPLOAD ****/
