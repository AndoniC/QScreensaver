#include "file_system.h"


bool file_system::isImage(std::string f)
{
	std::string ext = file_system::file(f).extension();
	

	if (ext.compare("bmp")==0 ||
		ext.compare("dib")==0 ||
		ext.compare("jpg")==0 ||
		ext.compare("jpe")==0 ||
		ext.compare("jpeg")==0 ||
		ext.compare("png")==0 ||
		ext.compare("pbm")==0 ||
		ext.compare("pgm")==0 ||
		ext.compare("ppm")==0 ||
		ext.compare("sr")==0 ||
		ext.compare("ras")==0 ||
		ext.compare("tiff")==0 ||
		ext.compare("tif")==0 ||
		ext.compare("gif")==0)
		return true;
	else return false;
}
bool file_system::isVideo(std::string f)
{

	std::string ext = file_system::file(f).extension();


	if (ext.compare("avi")==0 ||
		ext.compare("mpeg")==0 ||
		ext.compare("mpg")==0 ||
		ext.compare("asf")==0 ||
		ext.compare("wmv")==0 ||
		ext.compare("avs")==0 ||
		ext.compare("mp4")==0)
		return true;
	else return false;

}
std::string file_system::extension(std::string _f)
{
	int pos = _f.find_last_of('.');
	std::string ext = _f.substr(pos+1,_f.length());
	std::transform(ext.begin(), ext.end(), ext.begin(),::tolower);
	return ext;

}
std::string file_system::name(std::string _f)
{
	int pos = _f.find_last_of('\\');
	int pos1 = _f.find_last_of('/');
	pos = max (pos,pos1);

	std::string name = _f.substr(pos+1,_f.length());
	//std::transform(ext.begin(), ext.end(), ext.begin(),::tolower);
	return name;
}
std::string file_system::path(std::string _f)
{
	int pos = _f.find_last_of('\\');
	int pos1 = _f.find_last_of('/');
	pos = max (pos,pos1);

	std::string path = _f.substr(0,pos+1);
	//std::transform(ext.begin(), ext.end(), ext.begin(),::tolower);
	return path;
}

int file_system::get_dir_files (std::string dir, std::vector<std::string> &files,std::string filter,bool recursive,bool clear)
{
	if (dir.empty()) return -1;
	if (clear) files.clear();

	if (dir.at(dir.length()-1) != '\\' && dir.at(dir.length()-1) != '/' ) dir = dir + "/";
	
	WIN32_FIND_DATA data;

	std::string completefilter;

	//-- Si queremos los videos o las imagenes seleccionamos todos y luego eliminamos por extension --//
	if (filter.empty()) filter="*.*";
	else if (!filter.compare("images") || !filter.compare("videos") ) completefilter=dir + std::string("*");
	
	if (filter.compare("images") && filter.compare("videos"))  completefilter = dir + filter;


	HANDLE h = FindFirstFile(completefilter.c_str(),&data);
	
	if( h!=INVALID_HANDLE_VALUE ) 
	{
		do
		{
			std::string fichero = data.cFileName;
			if (fichero == "." || fichero == "..") continue;

			DWORD dwAttrib = GetFileAttributes((dir + fichero).c_str());
			if (dwAttrib == 0xffffffff ) continue;
			DWORD isdir = dwAttrib & FILE_ATTRIBUTE_DIRECTORY;
			if ( isdir ) 
			{
				std::string dirpath = dir+fichero+"/";
				std::replace( dirpath.begin(), dirpath.end(), '\\', '/');
				//-- Si el filtro es * queremos los directorios --//
				if (!filter.compare("*.")) files.push_back(dirpath);

				if (recursive)	get_dir_files(dirpath,files,filter,recursive);
			}
			else 
			{
				if (!filter.compare("images")) if (!file_system::isImage(fichero)) continue;
				if (!filter.compare("videos")) if (!file_system::isVideo(fichero)) continue;

				std::string filepath = dir+fichero;
				std::replace( filepath.begin(), filepath.end(), '\\', '/');
				files.push_back(filepath);
			}
			

		} while(FindNextFile(h,&data));

	
	} 
	else 
		std::cout << "Error: No hay ficheros " << completefilter << " en el directorio."<< std::endl;

	FindClose(h);

	return 0;
}


int file_system::createdir (std::string &dir,bool remove_content)
{
	if (dir.empty()) return -1;

	std::replace( dir.begin(), dir.end(), '\\', '/');
	if (dir.at(dir.length()-1) != '\\' && dir.at(dir.length()-1) != '/' ) dir = dir + "/";

	
	int pos = dir.find("/",0);
	int pos_last = dir.find_last_of("/");
	int current_pos =  dir.find("/",pos+1);
	while ( current_pos< pos_last)
	{
		pos = current_pos;
		current_pos = dir.find("/",current_pos+1);

		std::string dir_aux = dir.substr(0,pos+1);
		if ( (GetFileAttributes(dir_aux.c_str())==-1))
		{
			CreateDirectory(dir_aux.c_str(),NULL);
		}
	}


	if (GetFileAttributes(dir.c_str())==-1) 
	{
		CreateDirectory(dir.c_str(),NULL);
		DWORD err = GetLastError();
		if (ERROR_PATH_NOT_FOUND == err) 
		{
				LPVOID lpMsgBuf;
				FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
				  FORMAT_MESSAGE_FROM_SYSTEM | 
				  FORMAT_MESSAGE_IGNORE_INSERTS,
				  NULL, 
				  GetLastError(), 
				  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
				  (LPTSTR) &lpMsgBuf, 
				  0, 
				  NULL);
				printf ("ERROR:: %s\n",lpMsgBuf);
				LocalFree( lpMsgBuf );
			  return 0;
		}
		return 1;
	}
	else
	{
		if (ERROR_ALREADY_EXISTS == GetLastError()) 
		{
			//--Delete content --//
			if (remove_content)
			{
				std::vector<std::string> files;
				file_system::get_dir_files(dir,files);
				for (int i=0;i<(int)files.size();i++)
				{
					remove( files[i].c_str() );
				}
			}
			printf("Directorio %s ya existe.\n",dir.c_str());
			return 1;
		}
		else
		{
			printf("Error al intentar crear el directorio %s\n",dir.c_str());
			return 0;
		}
	}
}

int file_system::removedir (std::string &dir,bool remove_subdirs)
{
	bool            bSubdirectory = false;       // Flag, indicating whether
                                               // subdirectories have been found
	HANDLE          hFile;                       // Handle to directory
	std::string     strFilePath;                 // Filepath
	std::string     strPattern;                  // Pattern
	WIN32_FIND_DATA FileInformation;             // File information


	std::replace( dir.begin(), dir.end(), '\\', '/');
	if (dir.at(dir.length()-1) != '\\' && dir.at(dir.length()-1) != '/' ) dir = dir + "/";

	strPattern = dir + "*.*";
	hFile = ::FindFirstFile(strPattern.c_str(), &FileInformation);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		do
		{
		  if(FileInformation.cFileName[0] != '.')
		  {
			strFilePath.erase();
			strFilePath = dir +  FileInformation.cFileName;

			if(FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
			  if(remove_subdirs)
			  {
				// Delete subdirectory
				int iRC = removedir(strFilePath, remove_subdirs);
				if(iRC)
				  return iRC;
			  }
			  else
				bSubdirectory = true;
			}
			else
			{
			  // Set file attributes
			  if(::SetFileAttributes(strFilePath.c_str(),
									 FILE_ATTRIBUTE_NORMAL) == FALSE)
				return ::GetLastError();

			  // Delete file
			  if(::DeleteFile(strFilePath.c_str()) == FALSE)
				return ::GetLastError();
			}
		  }
		} while(::FindNextFile(hFile, &FileInformation) == TRUE);

		// Close handle
		::FindClose(hFile);

		DWORD dwError = ::GetLastError();
		if(dwError != ERROR_NO_MORE_FILES)
		  return dwError;
		else
		{
		  if(!bSubdirectory)
		  {
			// Set directory attributes
			if(::SetFileAttributes(dir.c_str(),
								   FILE_ATTRIBUTE_NORMAL) == FALSE)
			  return ::GetLastError();

			// Delete directory
			if(::RemoveDirectory(dir.c_str()) == FALSE)
			  return ::GetLastError();
		  }
		}
	}
	return 1;

}


std::string file_system::getAppDirectory()
{
	char buffer[MAX_PATH];
    GetModuleFileName( NULL, buffer, MAX_PATH );
	std::string dir = std::string( buffer );
	std::replace( dir.begin(), dir.end(), '\\', '/'); 
	std::string::size_type pos = dir.find_last_of( '/' );

	return dir.substr(0,pos+1);
}


int file_system::dir::getfiles ( std::vector<std::string> &files,std::string filter,bool recursive,bool clear)
{
	get_dir_files(m_dir,files,filter,recursive,clear);
	return 0;
}

int file_system::dir::del (bool _remove_subdirs)
{
	return removedir(m_dir,_remove_subdirs);
}

bool file_system::dir::exists()
{
	int res = GetFileAttributes(m_dir.c_str());
	return (res!=-1);
}
int file_system::dir::create(bool _remove_content)
{
	return createdir(m_dir,_remove_content);
}


bool file_system::file::del()
{
	if (!file(m_complete).exists()) return false;

	remove( m_complete.c_str() );
	return true;
}

bool file_system::file::exists()
{
	std::ifstream ifile(m_complete.c_str());
	return ifile.is_open();
}
std::string file_system::file::extension()
{
	int pos = m_name.find_last_of('.');
	std::string ext = m_name.substr(pos+1,m_name.length());
	std::transform(ext.begin(), ext.end(), ext.begin(),::tolower);
	return ext;
}
std::string file_system::file::name()
{
	int pos = m_name.find_last_of('\\');
	int pos1 = m_name.find_last_of('/');
	pos = max (pos,pos1);

	std::string name = m_name.substr(pos+1,m_name.length());
	//std::transform(ext.begin(), ext.end(), ext.begin(),::tolower);
	return name;

}
std::string file_system::file::path()
{
	int pos = m_complete.find_last_of('\\');
	int pos1 = m_complete.find_last_of('/');
	pos = max (pos,pos1);

	std::string path = m_complete.substr(0,pos+1);
	//std::transform(ext.begin(), ext.end(), ext.begin(),::tolower);
	return path;

}




