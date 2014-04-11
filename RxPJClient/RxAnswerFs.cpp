#include			"RxAnswerFs.h"

void				AnsAskFs(RxWSocket& sock, RxFileSystem& fs)
{
	bool							first = true;
	std::list<FileInfo>				fileList;
	std::list<FileInfo>::iterator	is;
	std::list<FileInfo>::iterator	ie;
	RxFileTransfer					ft;
	RxPacket*						p;

	fileList = fs.GetCurDirContent();
	if (fileList.size() <= 0)
		; // faire un truc
	is = fileList.begin();
	ie = fileList.end();
	while (is != ie)
	{
		p = new RxPacket();
		memset(p, 0, sizeof(*p));
		p->type = T_ANS_FS_NEXT;
		if (first == true)
		{
			p->type = T_ANS_FS;
			first = false;
		}
		memset(&ft, 0, sizeof(ft));
		ft.type = (*is).type;
		// verifier que la size depasse pas la taille de la macro !
		memcpy(ft.filename, (*is).name.c_str(), (*is).name.size());
		memcpy(p->data, (char*)&ft, sizeof(ft));
		AddToSendList(p);
		++is;
	}
}

void				AnsAskCd(RxWSocket& sock, RxFileSystem& fs, RxPacket& p)
{
	RxPacket*		np;
	std::string		tmp;
	
	tmp = p.data;
	np = new RxPacket();
	memset(np, 0, sizeof(*np));
	if (tmp.size() > 0)
	{
		if (fs.ChangeDirectory(tmp) == true)
		{
			np->type = T_ANS_CD_OK;
			AddToSendList(np);
			return ;
		}
	}
	np->type = T_ANS_CD_FAILED;
	AddToSendList(np);
}

void				AnsAskDel(RxWSocket& sock, RxFileSystem& fs, RxPacket& p)
{
	RxPacket*		np;
	std::string		tmp;
	
	tmp = p.data;
	np = new RxPacket();
	memset(np, 0, sizeof(*np));
	if (tmp.size() > 0)
	{
		if (fs.DeleteAFile(tmp) == true)
		{
			np->type = T_ANS_DELETE_OK;
			AddToSendList(np);
			return ;
		}
	}
	np->type = T_ANS_DELETE_FAILED;
	AddToSendList(np);
}

void				AnsAskCpy(RxWSocket& sock, RxFileSystem& fs, RxPacket& p)
{
	RxPacket*		np;
	std::string		tmp;
	
	tmp = p.data;
	np = new RxPacket();
	memset(np, 0, sizeof(*np));
	if (tmp.size() > 0)
	{
		if (fs.BufferizeFile(tmp) == true)
		{
			np->type = T_ANS_COPY_OK;
			AddToSendList(np);
			return ;
		}
	}
	np->type = T_ANS_COPY_FAILED;
	AddToSendList(np);
}

void				AnsAskPst(RxWSocket& sock, RxFileSystem& fs, RxPacket& p)
{
	RxPacket*		np;
	
	np = new RxPacket();
	memset(np, 0, sizeof(*np));
	if (fs.CreateAFile(fs.GetBufferizedFileName(), fs.GetBufferizedFile(), fs.GetBufferizedFileSize()) == true)
	{
		np->type = T_ANS_PASTE_OK;
		AddToSendList(np);
		return ;
	}
	np->type = T_ANS_PASTE_FAILED;
	AddToSendList(np);
}

void				AnsAskRnm(RxWSocket& sock, RxFileSystem& fs, RxPacket& p)
{
	RxPacket*		np;
	RxRenameData	rnm;
	
	np = new RxPacket();
	memset(np, 0, sizeof(*np));
	memset(&rnm, 0, sizeof(rnm));
	memcpy(&rnm, p.data, sizeof(rnm));
	if (fs.RenameAFile(rnm.fileName, rnm.newName) == true)
	{
		np->type = T_ANS_RENAME_OK;
		AddToSendList(np);
		return ;
	}
	np->type = T_ANS_RENAME_FAILED;
	AddToSendList(np);
}

void				AnsAskCrt(RxWSocket& sock, RxFileSystem& fs, RxPacket& p)
{
	RxPacket*		np;
	RxCreateFile	crt;
	
	np = new RxPacket();
	memset(np, 0, sizeof(*np));
	memset(&crt, 0, sizeof(crt));
	memcpy(&crt, p.data, sizeof(crt));
	if (fs.CreateAFile(crt.title, crt.content, strlen(crt.content)) == true)
	{
		np->type = T_ANS_CRT_FILE_OK;
		AddToSendList(np);
		return ;
	}
	np->type = T_ANS_CRT_FILE_FAILED;
	AddToSendList(np);
}

void				AnsAskExe(RxWSocket& sock, RxFileSystem& fs, RxPacket& p)
{
	RxPacket*		np;
	std::string		tmp;
	
	tmp = p.data;
	np = new RxPacket();
	memset(np, 0, sizeof(*np));
	if (tmp.size() > 0)
	{
		if (fs.ExecDotExe(tmp) == true)
		{
			np->type = T_ANS_EXEC_OK;
			AddToSendList(np);
			return ;
		}
	}
	np->type = T_ANS_EXEC_FAILED;
	AddToSendList(np);
}
