#include			"RxUpAndDown.h"

RxUpAndDown::RxUpAndDown(void)
	: _mod(MOD_IDLE), _filename(""), _sockN(-1)
{
	this->_sockMtx = CreateMutex(NULL, false, NULL);
	this->_downloadDir = this->_fs.GetCurrentDir() + DL_OUTPUT_DIR;
	this->_fs.CreateADir(this->_downloadDir);
}

RxUpAndDown::~RxUpAndDown(void)
{
}

bool				RxUpAndDown::DownloadFileFrom(int sockN, std::string filename)
{
	if (filename.size() > 0 && sockN > 0)
	{
		if (this->_fs.DoesThisFileExist(this->_downloadDir + filename) == false)
		{
			if (WaitForSingleObject(this->_sockMtx, THREADTESTTIMEOUT) == WAIT_OBJECT_0)
			{
				if (this->_sock.ServerInit(SV_UPDN_PORT) == true)
				{
					this->_mod = MOD_DWNL;
					this->_filename = filename;
					this->_sockN = sockN;
					this->start();
					return (true);
				}
			}
		}
	}
	return (false);
}

bool				RxUpAndDown::UploadFileFrom(int sockN, std::string pathname)
{
	if (pathname.size() > 0 && sockN > 0)
	{
		if (this->_fs.DoesThisFileExist(pathname) == true)
		{
			if (WaitForSingleObject(this->_sockMtx, THREADTESTTIMEOUT) == WAIT_OBJECT_0)
			{
				if (this->_fs.BufferizeFile(pathname) == true)
				{
					if (this->_sock.ServerInit(SV_UPDN_PORT) == true)
					{
						this->_mod = MOD_UPLD;
						this->_filename = pathname;
						this->_sockN = sockN;
						this->start();
						return (true);
					}
				}
			}
		}
	}
	return (false);
}

void				RxUpAndDown::run()
{
	if (this->_mod == MOD_DWNL)
		this->RunForDown();
	else if (this->_mod == MOD_UPLD)
		this->RunForUp();
	this->_mod = MOD_IDLE;
	this->_filename = "";
	this->_sockN = -1;
	this->_sock.Disconnect();
	ReleaseMutex(this->_sockMtx);
}

void				RxUpAndDown::RunForDown()
{
	bool			finished;
	bool			connCheck;
	bool			dlOk;
	RxWSocket*		tmpSock;
	fd_set			fdread;
	struct timeval	timer;
	int				rdRet;
	int				offset;
	int				lastPartReceived;
	RxUpDownPacket	toRead;
	bool			firstPack;

	finished = false;
	connCheck = false;
	dlOk = false;
	firstPack = false;
	timer.tv_sec = SELECT_TIMEOUT;
	timer.tv_usec = 0;
	lastPartReceived = 0;
	offset = 0;
	tmpSock = 0;
	while (finished == false)
	{
		FD_ZERO(&fdread);
		if (connCheck == false)
			FD_SET(this->_sock.GetSocket(), &fdread);
		else if (tmpSock > 0)
			FD_SET(tmpSock->GetSocket(), &fdread);
		else
		{
			emit this->UpdnIncident("UPDN Socket error. (download).");
			break;
		}
		if (select(42, &fdread, NULL, NULL, &timer) > 0)
		{
			if (connCheck == false && FD_ISSET(this->_sock.GetSocket(), &fdread))
			{
				tmpSock = this->_sock.Accept();
				if (tmpSock == NULL)
				{
					emit this->UpdnIncident("UPDN Accept : NULL returned. (download).");
					break ;
				}
				emit this->UpdnIncident("UPDN - Connection established. (download).");
				connCheck = true;
			}
			else if (connCheck == true)
			{
				if (FD_ISSET(tmpSock->GetSocket(), &fdread))
				{
					if (offset == 0)
						memset(&toRead, 0, sizeof(toRead));
					rdRet = tmpSock->Receive((char*)&toRead + offset, sizeof(toRead) - offset);
					if (rdRet <= 0)
					{
						emit this->UpdnIncident("UPDN Read failed. Aborting download.");
						break ;
					}
					if (rdRet + offset == sizeof(RxUpDownPacket))
					{
						if (firstPack == false)
						{
							firstPack = true;
							emit this->UpdnIncident("UPDN Transfer : " + this->_filename);
						}
						offset = 0;
						if (toRead.sockN == this->_sockN && toRead.part == lastPartReceived + 1 && toRead.type == T_UPDN_TRAN)
						{
							if (this->_fs.AppendToAFile(this->_downloadDir + this->_filename, toRead.data, toRead.partSize) == true)
							{
								lastPartReceived += 1;
								emit this->UpdnNewPartRec(lastPartReceived, toRead.totalParts);
								if (lastPartReceived == toRead.totalParts)
								{
									emit this->UpdnDlFinished();
									dlOk = true;
									break ;
								}
							}
							else
							{
								emit this->UpdnIncident("UPDN Append : Could not append data.");
								break ;
							}
						}
						else if (toRead.sockN == this->_sockN && toRead.type == T_UPDN_DL_BUFF_FAIL)
						{
							emit this->UpdnIncident("UPDN Client can not bufferize.");
							break ;
						}
						else
						{
							emit this->UpdnIncident("UPDN Integrity compromised. Aborting.");
							break ;
						}
					}
					else
						offset = offset + rdRet;
				}
			}
			else
			{
				emit this->UpdnIncident("UPDN Select timed-out. (download).");
				break;
			}
		}
		else
		{
			emit this->UpdnIncident("UPDN Select failed. Aborting. (download).");
			break ;
		}
	}
	if (dlOk == false)
		this->_fs.DeleteAFile(this->_downloadDir + this->_filename);
	if (tmpSock > 0)
	{
		delete (tmpSock);
		tmpSock = 0;
	}
}

void				RxUpAndDown::RunForUp()
{
	bool			finished;
	fd_set			fdread;
	fd_set			fdwrite;
	struct timeval	timer;
	bool			connCheck;
	RxWSocket*		tmpSock;
	int				offset;
	RxUpDownPacket	udp;
	int				part;
	int				nParts;

	finished = false;
	connCheck = false;
	tmpSock = 0;
	offset = 0;
	part = 1;
	timer.tv_sec = SELECT_TIMEOUT;
	timer.tv_usec = 0;
	nParts = this->_fs.GetBufferizedFileSize() / SIZE_UPDN_DATA;
	if (this->_fs.GetBufferizedFileSize() % SIZE_UPDN_DATA > 0)
		nParts += 1;
	while (finished == false)
	{
		FD_ZERO(&fdread);
		FD_ZERO(&fdwrite);
		if (connCheck == false)
			FD_SET(this->_sock.GetSocket(), &fdread);
		else if (tmpSock > 0)
			FD_SET(tmpSock->GetSocket(), &fdwrite);
		else
		{
			emit this->UpdnIncident("UPDN Socket error. (Upload).");
			break;
		}
		if (select(42, &fdread, &fdwrite, NULL, &timer) > 0)
		{
			if (connCheck == false && FD_ISSET(this->_sock.GetSocket(), &fdread))
			{
				tmpSock = this->_sock.Accept();
				if (tmpSock == NULL)
				{
					emit this->UpdnIncident("UPDN Accept : NULL returned. (Upload).");
					break ;
				}
				emit this->UpdnIncident("UPDN - Connection established. (Upload).");
				connCheck = true;
			}
			else if (connCheck == true)
			{
				if (FD_ISSET(tmpSock->GetSocket(), &fdwrite))
				{
					memset(&udp, 0, sizeof(udp));
					udp.type = T_UPDN_TRAN;
					udp.part = part;
					udp.totalParts = nParts;
					udp.sockN = this->_sockN;
					if (part < nParts)
					{
						memcpy(udp.data, this->_fs.GetBufferizedFile() + offset, SIZE_UPDN_DATA);
						udp.partSize = SIZE_UPDN_DATA;
						part = part + 1;
						offset = offset + SIZE_UPDN_DATA;
					}
					else
					{
						memcpy(udp.data, this->_fs.GetBufferizedFile() + offset, this->_fs.GetBufferizedFileSize() % SIZE_UPDN_DATA);
						udp.partSize = this->_fs.GetBufferizedFileSize() % SIZE_UPDN_DATA;
						if (udp.partSize == 0 && offset < this->_fs.GetBufferizedFileSize())
						{
							udp.partSize = SIZE_UPDN_DATA;
							offset = offset + SIZE_UPDN_DATA;
						}
						else
							offset = offset + (this->_fs.GetBufferizedFileSize() % SIZE_UPDN_DATA);
						finished = true;
						emit this->UpdnUpFinished();
					}
					if (tmpSock->Send((char*)&udp, sizeof(udp)) <= 0)
					{
						emit this->UpdnIncident("UPDN Send : Failure. Aborting. (Upload).");
						break;
					}
					if (finished == false)
						emit this->UpdnNewPartSnd(udp.part, udp.totalParts);
					Sleep(1);
				}
			}
			else
			{
				emit this->UpdnIncident("UPDN Select timed-out. (Upload).");
				break;
			}
		}
		else
		{
			emit this->UpdnIncident("UPDN Select failed. Aborting. (Upload).");
			break ;
		}
	}
	this->_fs.Flush();
}
