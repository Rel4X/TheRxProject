#include			"RxFileSystem.h"

const char*				RxFileSystem::driveTab[32] =
{"A:\\", "B:\\", "C:\\", "D:\\", "E:\\", "F:\\", "G:\\", "H:\\", "I:\\", "J:\\", "K:\\", "L:\\", "M:\\", "N:\\"
, "O:\\", "P:\\", "Q:\\", "R:\\", "S:\\", "T:\\", "U:\\", "V:\\", "W:\\", "X:\\", "Y:\\", "Z:\\", "err", "err"
, "err", "err", "err", 0};

RxFileSystem::RxFileSystem(void)
	: _bufferizedFile(0), _bufferizedFileSize(0), _bufferizedFileName("")
{
}

RxFileSystem::~RxFileSystem(void)
{
	if (this->_bufferizedFile > 0)
		delete (this->_bufferizedFile);
}

void					RxFileSystem::Flush()
{
	if (this->_bufferizedFile > 0)
		delete (this->_bufferizedFile);
	this->_bufferizedFile = 0;
	this->_bufferizedFileName = "";
	this->_bufferizedFileSize = 0;
}

std::string				RxFileSystem::GetCurrentDir() const
{
	char				buffer[MAX_PATH_LEN];

	memset(buffer, 0, MAX_PATH_LEN);
	if (GetCurrentDirectoryA(MAX_PATH_LEN - 1, buffer) == 0)
		return ("");
	else
		return (buffer);
}

std::list<FileInfo>		RxFileSystem::GetCurDirContent()
{
	std::list<FileInfo>		contentList;
	std::string				tmpStr;
	FileInfo				temp;
	HANDLE					hf;
	WIN32_FIND_DATAA		wfd;

	tmpStr = this->GetCurrentDir().c_str();
	tmpStr += "\\*";
	hf = FindFirstFileA(tmpStr.c_str(), &wfd);
	if (hf == INVALID_HANDLE_VALUE)
		return (contentList);
	else
	{
		temp.name = wfd.cFileName;
		if (wfd.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
			temp.type = FILE_DIRECTORY;
		else
			temp.type = FILE_FILE;
		contentList.push_back(temp);
		while (FindNextFileA(hf, &wfd))
		{
			temp.name = wfd.cFileName;
			if (wfd.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
				temp.type = FILE_DIRECTORY;
			else
				temp.type = FILE_FILE;
			contentList.push_back(temp);
		}
	}
	return (contentList);
}

std::list<DriveInfo>	RxFileSystem::GetDrives() const
{
	DWORD					drives;
	DWORD					tmp;
	DWORD					mask;
	unsigned int			type;
	std::list<DriveInfo>	drivesList;

	mask = 1;
	drives = GetLogicalDrives();
	for (int i = 0; i < 32; ++i)
	{
		tmp = drives & mask;
		if (tmp > 0)
		{
			type = GetDriveType((LPCTSTR)RxFileSystem::driveTab[i]);
			if (type == DRIVE_FIXED)
				type = DRIVE_HARD_DISK;
			else
				type = DRIVE_OTHER;
			drivesList.push_back(DriveInfo(RxFileSystem::driveTab[i], (char)type));
		}
		mask *= 2;
	}
	return (drivesList);
}

bool					RxFileSystem::DoesThisFileExist(std::string filename)
{
	std::fstream		file(filename.c_str());

	return (!file.fail());
}

bool					RxFileSystem::ChangeDirectory(const std::string& dir)
{
	return (SetCurrentDirectoryA(dir.c_str()));
}

void					RxFileSystem::PrintCurDirContentToConsole()
{
	std::list<FileInfo>				lfi;
	std::list<FileInfo>::iterator	is;
	std::list<FileInfo>::iterator	ie;

	lfi = this->GetCurDirContent();
	is = lfi.begin();
	ie = lfi.end();
	for (; is != ie; ++is)
	{
		if ((*is).type == FILE_DIRECTORY)
			std::cout << "[Dir] ";
		else
			std::cout << "[Fil] ";
		std::cout << (*is).name << std::endl;
	}
}

bool					RxFileSystem::CreateADir(const std::string& newDir, bool hidden, bool system)
{
	bool				ret;

	ret = CreateDirectoryA(newDir.c_str(), NULL);
	if (ret == false)
		return (false);
	if (hidden == true && system == false)
		ret = SetFileAttributes((LPCTSTR)newDir.c_str(), FILE_ATTRIBUTE_HIDDEN);
	else if (hidden == true && system == true)
		ret = SetFileAttributes((LPCTSTR)newDir.c_str(), FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
	else if (hidden == false && system == true)
		ret = SetFileAttributes((LPCTSTR)newDir.c_str(), FILE_ATTRIBUTE_SYSTEM);
	return (ret);
}

bool					RxFileSystem::CreateAFile(const std::string& name, const char* content, int size, bool hidden, bool system)
{
	std::fstream		newFile;

	if (this->DoesThisFileExist(name) == false)
	{
		newFile.open(name, std::ios::out | std::ios::trunc | std::ios_base::binary);
		if (newFile.is_open())
		{
			newFile.write(content, size);
			newFile.close();
			if (hidden == true && system == false)
				SetFileAttributes((LPCTSTR)name.c_str(), FILE_ATTRIBUTE_HIDDEN);
			else if (hidden == true && system == true)
				SetFileAttributes((LPCTSTR)name.c_str(), FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
			else if (hidden == false && system == true)
				SetFileAttributes((LPCTSTR)name.c_str(), FILE_ATTRIBUTE_SYSTEM);
			return (true);
		}
	}
	return (false);
}

bool					RxFileSystem::AppendToAFile(const std::string& name, const char* content, int size)
{
	std::fstream		newFile;

	newFile.open(name, std::ios::out | std::ios::app | std::ios_base::binary);
	if (newFile.is_open())
	{
		newFile.write(content, size);
		newFile.close();
		return (true);
	}
	return (false);
}

bool					RxFileSystem::DeleteADir(const std::string& dir)
{
	return (RemoveDirectoryA(dir.c_str()));
}

bool					RxFileSystem::DeleteAFile(const std::string& file)
{
	return (DeleteFileA(file.c_str()));
}

bool					RxFileSystem::BufferizeFile(const std::string& fName)
{
	std::fstream		file;

	file.open(fName, std::ios::in | std::ios::out | std::ios_base::binary);
	if (file.is_open())
	{
		this->_bufferizedFileName = fName;
		file.seekg(0, std::ios_base::end);
		this->_bufferizedFileSize = file.tellg();
		file.seekg(0, std::ios_base::beg);
		this->_bufferizedFile = new char[this->_bufferizedFileSize]();
		file.read(this->_bufferizedFile, this->_bufferizedFileSize);
		file.close();
		return (true);
	}
	else
	{
		this->Flush();
	}
	return (false);
}

bool					RxFileSystem::RenameAFile(const std::string& oldf, const std::string& newf)
{
	if (this->BufferizeFile(oldf) == true && this->_bufferizedFile != NULL)
	{
		if (this->CreateAFile(newf, this->_bufferizedFile, this->_bufferizedFileSize) == true)
		{
			this->DeleteAFile(oldf);
			return (true);
		}
	}
	return (false);
}

bool					RxFileSystem::ExecDotExe(const std::string& dotExe)
{
	STARTUPINFOA		si;
	PROCESS_INFORMATION	pi;

	memset(&si, 0, sizeof(si));
	memset(&pi, 0, sizeof(pi));
	if (CreateProcessA(NULL, const_cast<char*>(dotExe.c_str()),
		NULL, NULL, false, 0, NULL, NULL, &si, &pi) == false)
	{
		return (false);
	}
	return (true);
}