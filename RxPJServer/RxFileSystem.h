#pragma once

#include			<Windows.h>
#include			<iostream>
#include			<fstream>
#include			<string>
#include			<list>

#define				MAX_PATH_LEN	4096

#define				DRIVE_HARD_DISK	1
#define				DRIVE_OTHER		9

#define				FILE_DIRECTORY	1
#define				FILE_FILE		2

struct				DriveInfo
{
	const char*		letter;
	char			type;

	DriveInfo() { }
	DriveInfo(const char* l, char t) : letter(l), type(t) { }
	DriveInfo(const DriveInfo& d) : letter(d.letter), type(d.type) { } 
	~DriveInfo() { }
};

struct				FileInfo
{
	std::string		name;
	char			type;

	FileInfo() { }
	FileInfo(std::string n, char t) : name(n), type(t) { }
	~FileInfo() { }
};

class				RxFileSystem
{
private:
	static const char*	driveTab[32];

private:
	std::string		_bufferizedFileName;
	char*			_bufferizedFile;
	int				_bufferizedFileSize;

public:
	RxFileSystem(void);
	~RxFileSystem(void);

public:
	const std::string&		GetBufferizedFileName() const	{ return (this->_bufferizedFileName); }
	char*					GetBufferizedFile() const		{ return (this->_bufferizedFile); }
	int						GetBufferizedFileSize() const	{ return (this->_bufferizedFileSize); }

public:
	void					Flush();
	std::string				GetCurrentDir() const;
	std::list<FileInfo>		GetCurDirContent();
	std::list<DriveInfo>	GetDrives() const;
	bool					DoesThisFileExist(std::string);
	bool					ChangeDirectory(const std::string&);
	void					PrintCurDirContentToConsole();
	bool					CreateADir(const std::string&, bool hidden = false, bool system = false);
	bool					CreateAFile(const std::string&, const char*, int, bool hidden = false, bool system = false);
	bool					AppendToAFile(const std::string&, const char*, int);
	bool					DeleteADir(const std::string&);
	bool					DeleteAFile(const std::string&);
	bool					BufferizeFile(const std::string&);
	bool					RenameAFile(const std::string&, const std::string&);
	bool					ExecDotExe(const std::string&);
};

