#pragma once

#include		<QThread>
#include		<string>
#include		"RxWSocket.h"
#include		"RxProtocol.h"
#include		"RxFileSystem.h"

#define			THREADTESTTIMEOUT	1000

#define			MOD_IDLE			1
#define			MOD_DWNL			2
#define			MOD_UPLD			3

#define			DL_OUTPUT_DIR		"\\Downloaded-Files\\"

class			RxUpAndDown :
	public QThread
{
	Q_OBJECT

private:
	RxFileSystem	_fs;
	RxWSocket		_sock;
	HANDLE			_sockMtx;
	char			_mod;
	std::string		_filename;
	int				_sockN;
	std::string		_downloadDir;

public:
	RxUpAndDown(void);
	~RxUpAndDown(void);

public:
	bool			DownloadFileFrom(int, std::string);
	bool			UploadFileFrom(int, std::string);
	void			run();

private:
	void			RunForDown();
	void			RunForUp();

signals:
	void			UpdnIncident(std::string);
	void			UpdnNewPartRec(int, int);
	void			UpdnDlFinished();
	void			UpdnNewPartSnd(int, int);
	void			UpdnUpFinished();
};
