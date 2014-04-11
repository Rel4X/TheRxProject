#ifndef				_RXUTILITY_H_
# define			_RXUTILITY_H_

#include				<string>
#include				<Windows.h>
#include				<Psapi.h>

#define					TITLE_MAX	1024

const std::string		RxGetComputerName();
const std::string		RxGetWindowName(const HWND&);	// Unused atm
const std::string		RxGetProcessName(const HWND&);	// Unused atm

#endif				// _RXUTILITY_H_
