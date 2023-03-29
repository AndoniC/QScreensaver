#ifndef DIR_H
#define DIR_H
#include "string_ex.h"
#include <vector>
#include <iostream>
#ifdef TTS_SUPPORT
	#include <sapi.h>           // SAPI includes
	//#include <sphelper.h>
	#include <spuihelp.h>  // para el tts
#endif
#include "conio.h"
#include <time.h>

#ifdef WIN32
	#include <windows.h>
#else
	#include <stdio.h>
	#include <sys/time.h>
	#include <time.h>
	#include <unistd.h>
	#include <termios.h>
	#include <X11/Xlib.h>
	#include <dirent.h>
#endif

#include <stdarg.h>
#include <fstream>


namespace file_system
{
	bool isImage(std::string f);
	bool isVideo(std::string f);
	std::string getAppDirectory();
	std::string extension(std::string _file);
	std::string name(std::string _file);
	std::string path(std::string _file);

	bool deletefile (std::string file);
	int removedir (std::string &_dir,bool _remove_subdirs);
	int createdir (std::string &dir,bool remove_content);
	int get_dir_files (std::string dir, std::vector<std::string> &files,std::string filter=std::string(),bool recursive=true,bool clear=false);


	class dir 
	{
	public:
		dir(std::string _dir){m_dir = _dir;};

		
		int getfiles (std::vector<std::string> &files,std::string filter=std::string(),bool recursive=true,bool clear=false);
		int create (bool _remove_content);
		int del (bool _remove_subdirs);
		
		bool exists();
	
	


	private:
		std::string m_dir;

	};

	class file
	{
	public:
		
		file(std::string _file){m_complete = _file;
		m_name = _file;
		m_path = path();};
		bool exists();
		std::string extension();
		std::string name();
		std::string path();
		bool del();

	private:
		std::string m_name;
		std::string m_dir;
		std::string m_path;
		std::string m_complete;
	};

}

#endif