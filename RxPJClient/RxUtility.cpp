#include		"RxUtility.h"
#include		<Windows.h>
#include		<string>

const std::string	RxGetComputerName()
{
	std::string		cName;
	char			computer[1024];
	DWORD			size = sizeof(computer);

	GetComputerName(computer, &size);
	cName = computer;
	if (cName.size() <= 0)
		return ("");
	return (cName);
}

const std::string	RxGetWindowName(const HWND& hdl)
{
	std::string			wiName("");
	char				nWindow[TITLE_MAX + 1];
	int					len;

	memset(nWindow, 0, TITLE_MAX + 1);
	len = GetWindowText(hdl, nWindow, TITLE_MAX);
	if (len > 0)
		wiName = wiName + '[' + nWindow + "]\n";
	else
		wiName = "[Empty]\n";
	return (wiName);
}

const std::string	RxGetProcessName(const HWND& hdl)
{
	std::string			pName("\n\n");
	char				nProcess[TITLE_MAX + 1];
	DWORD				threadProcessId;
	HANDLE				pid;

	memset(nProcess, 0, TITLE_MAX + 1);
	GetWindowThreadProcessId(hdl, &threadProcessId);
	pid = OpenProcess(PROCESS_ALL_ACCESS, false, threadProcessId);
	if (pid != NULL)
	{
		GetModuleBaseName(pid, NULL, nProcess, TITLE_MAX);
		pName = pName + '{' + nProcess + '}';
	}
	else
	{
		pName = "\n\n{Inconnu}";
	}
	return (pName);
}
