#ifndef				_RXPROTOCOL_H_
# define			_RXPROTOCOL_H_

# define			SV_BASE_PORT		"4242"
# define			SV_UPDN_PORT		"4243"

# define			SELECT_TIMEOUT		30	// Seconds

# define			SIZE_VERSION		6
# define			SIZE_DATA			1024
# define			SIZE_FILE_NAME		SIZE_DATA - 24
# define			SIZE_UPDN_DATA		16384


# define			T_DISCONNECT			9999
/* Client */
# define			T_IDENTIFICATION		1
# define			T_ANS_VERSION			2
# define			T_ANS_FS				3
# define			T_ANS_FS_NEXT			4
# define			T_ANS_FS_FAILED			5
# define			T_ANS_CD_OK				6
# define			T_ANS_CD_FAILED			7
# define			T_ANS_DELETE_OK			8
# define			T_ANS_DELETE_FAILED		9
# define			T_ANS_COPY_OK			10
# define			T_ANS_COPY_FAILED		11
# define			T_ANS_PASTE_OK			12
# define			T_ANS_PASTE_FAILED		13
# define			T_ANS_RENAME_OK			14
# define			T_ANS_RENAME_FAILED		15
# define			T_ANS_CRT_FILE_OK		16
# define			T_ANS_CRT_FILE_FAILED	17
# define			T_ANS_EXEC_OK			18
# define			T_ANS_EXEC_FAILED		19
# define			T_ANS_KEYLOG_OK			20
# define			T_ANS_KEYLOG_FAILED		21
# define			T_ANS_KEY_PRESSED		22
# define			T_ANS_KEYLOG_STOP_OK	23
# define			T_ANS_KEYLOG_STOP_FAIL	24
# define			T_ANS_DOWNLOAD_OK		25
# define			T_ANS_DOWNLOAD_FAIL		26
# define			T_ANS_UPLOAD_OK			27
# define			T_ANS_UPLOAD_FAIL		28
/* Serveur */
# define			T_ASK_VERSION			100
# define			T_ASK_FS				101
# define			T_ASK_CD				102
# define			T_ASK_DELETE			103
# define			T_ASK_COPY				104
# define			T_ASK_PASTE				105
# define			T_ASK_RENAME			106
# define			T_ASK_CRT_FILE			107
# define			T_ASK_EXEC				108
# define			T_ASK_KEYLOG			109
# define			T_ASK_KEYLOG_STOP		110
# define			T_ASK_DOWNLOAD			111
# define			T_ASK_UPLOAD			112
/* UpDown Protocol */
# define			T_UPDN_TRAN				200
# define			T_UPDN_DL_BUFF_FAIL		201

/* Base structs */
struct				RxPacket
{
	unsigned int	type;
	char			data[SIZE_DATA];
};

struct				RxFileTransfer
{
	char			type;
	char			filename[SIZE_FILE_NAME];
};

struct				RxRenameData
{
	char			fileName[SIZE_DATA / 2];
	char			newName[SIZE_DATA / 2];
};

struct				RxCreateFile
{
	char			title[SIZE_DATA / 2];
	char			content[SIZE_DATA / 2];
};

struct				RxKeyPressed
{
	unsigned long	vkcode;
};

struct				RxInitUpDown
{
	int				sock;
	char			data[SIZE_DATA - sizeof(int)];
};

/* UpDown structs */
struct				RxUpDownPacket
{
	int				type;
	int				sockN;
	int				part;
	int				totalParts;
	int				partSize;
	char			data[SIZE_UPDN_DATA];
};

#endif				// _RXPROTOCOL_H_