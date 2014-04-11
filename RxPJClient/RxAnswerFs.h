#ifndef				_RXANSWERFS_H_
# define			_RXANSWERFS_H_

#include			"RxFileSystem.h"
#include			"RxNetOut.h"

void				AnsAskFs(RxWSocket&, RxFileSystem&);
void				AnsAskCd(RxWSocket&, RxFileSystem&, RxPacket&);
void				AnsAskDel(RxWSocket&, RxFileSystem&, RxPacket&);
void				AnsAskCpy(RxWSocket&, RxFileSystem&, RxPacket&);
void				AnsAskPst(RxWSocket&, RxFileSystem&, RxPacket&);
void				AnsAskRnm(RxWSocket&, RxFileSystem&, RxPacket&);
void				AnsAskCrt(RxWSocket&, RxFileSystem&, RxPacket&);
void				AnsAskExe(RxWSocket&, RxFileSystem&, RxPacket&);

#endif				// _RXANSWERFS_H_
