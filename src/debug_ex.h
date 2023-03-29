#ifndef DEBUG_EX_H
#define DEBUG_EX_H


#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
#include <string>

#define TEXT_COLOR_RED 0x0C
#define TEXT_COLOR_GREEN 0x0A
#define TEXT_COLOR_CYAN 0x03


namespace debug_ex
{
	inline void trace(const char *text, ...);
	inline void trace(int write, const char *text,...);
	inline void trace(const char *text, ...);
	inline void warning(const char *loc, const char *text, ...);
	inline void error(const char *loc, const char *text, ...);
	inline void message(unsigned char color, const char *text, ...);
	inline void errorENH(const char *loc, const char *text, ...);
	inline std::string getEnvironmentVariable(std::string variable);



	inline void trace(const char *text, ...)
	{
		va_list arg;
		int done;
		va_start (arg, text);
		done = vfprintf (stdout, text, arg);
		fflush(stdout);
		va_end (arg);
	
	}


	void trace(int write, const char *text, ...)
	{
		if (write)
		{
			va_list arg;
			int done;
			va_start (arg, text);
			done = vfprintf (stdout, text, arg);
			fflush(stdout);
			va_end (arg);
		 }
	}


		
	void warning(const char *loc, const char *text, ...)
	{

		char cBuffer[3000] = { 0 };
	
		va_list arg;
		int done;
		va_start (arg, text);
		vsprintf(cBuffer, text, arg);


	#ifdef WIN32
		HANDLE hOut;
		hOut = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hOut, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		std::cout <<"warning (" << loc <<"): "<< cBuffer<<std::endl;
		SetConsoleTextAttribute(hOut, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);

	#else
		//"\033[1;31mbold RED TEXT \033[0m\n"
		//printf(MAKE_GREEN "warning %s: %s" RESET_COLOR "\n",loc,cBuffer);
		std::cout <<MAKE_GREEN << "warning " << loc <<": " <<cBuffer << RESET_COLOR << std::endl;
	#endif
		fflush(stdout);
		va_end (arg);
	}

	void errorENH(const char *loc, const char *text, ...)
	{
		char cBuffer[3000] = { 0 };
	
		va_list arg;
		int done;
		va_start (arg, text);
		vsprintf(cBuffer, text, arg);

		#ifdef WIN32
			HANDLE hOut;
			hOut = GetStdHandle(STD_OUTPUT_HANDLE);
			SetConsoleTextAttribute(hOut, FOREGROUND_RED | FOREGROUND_INTENSITY);
			std::cout <<std::endl << "error (" << loc <<"): "<< cBuffer<<std::endl<<std::endl;
			SetConsoleTextAttribute(hOut, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
			//printf("error (%s): %s\n",loc,cBuffer);
		#else
			//"\033[1;31mbold RED TEXT \033[0m\n"
			std::cout <<MAKE_GREEN << "error " << loc <<": " <<cBuffer << RESET_COLOR << std::endl;

			//printf(MAKE_GREEN "warning %s: %s" RESET_COLOR "\n",loc,cBuffer);
	
		#endif

	

		fflush(stdout);
		va_end (arg);

	}


	void error(const char *loc, const char *text, ...)
	{
		char cBuffer[3000] = { 0 };
	
		va_list arg;
		int done;
		va_start (arg, text);
		vsprintf(cBuffer, text, arg);

		#ifdef WIN32
			HANDLE hOut;
			hOut = GetStdHandle(STD_OUTPUT_HANDLE);
			SetConsoleTextAttribute(hOut, FOREGROUND_RED | FOREGROUND_INTENSITY);
			std::cout << "error (" << loc <<"): "<< cBuffer<<std::endl;
			SetConsoleTextAttribute(hOut, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
			//printf("error (%s): %s\n",loc,cBuffer);
		#else
			//"\033[1;31mbold RED TEXT \033[0m\n"
			std::cout <<MAKE_GREEN << "error " << loc <<": " <<cBuffer << RESET_COLOR << std::endl;

			//printf(MAKE_GREEN "warning %s: %s" RESET_COLOR "\n",loc,cBuffer);
	
		#endif

	

		fflush(stdout);
		va_end (arg);
	}
	
	

	void message(unsigned char color, const char *text, ...)
	{
			va_list arg;
			int done;
			char cBuffer[3000] = { 0 };
			va_start (arg, text);
	//		done = vfprintf (stdout, text, arg);
			vsprintf(cBuffer, text, arg);
			#ifdef WIN32
				HANDLE hOut;
				hOut = GetStdHandle(STD_OUTPUT_HANDLE);
				SetConsoleTextAttribute(hOut, color);
				std::cout << cBuffer<<std::endl;
				SetConsoleTextAttribute(hOut, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
				//printf("error (%s): %s\n",loc,cBuffer);
			#else
				//"\033[1;31mbold RED TEXT \033[0m\n"
				//std::cout <<MAKE_GREEN << "error " << loc <<": " <<cBuffer << RESET_COLOR << std::endl;
				std::cout <<MAKE_GREEN << "error: " <<cBuffer << RESET_COLOR << std::endl;

				//printf(MAKE_GREEN "warning %s: %s" RESET_COLOR "\n",loc,cBuffer);
	
			#endif

			fflush(stdout);
			va_end (arg);

	}

	std::string getEnvironmentVariable(std::string variable)
	{
		char value[1024];
		GetEnvironmentVariable(variable.c_str(), value, 1024);
		std::string staux = value;
		return staux;
	}

}

#endif