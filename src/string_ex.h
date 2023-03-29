#ifndef STRING_EX_H
#define STRING_EX_H


#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
#include <string>

namespace string_ext
{
// trim from start
inline std::string &ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
}

// trim from end
inline std::string &rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
}

// trim from both ends
inline std::string &trim(std::string &s) {
        return ltrim(rtrim(s));
}

static inline std::string &makeUpper(std::string &s) {
	
	std::transform(s.begin(), s.end(),s.begin(), ::toupper);
	return s;
}
static inline std::string &makeLower(std::string &s) {
        std::transform(s.begin(), s.end(),s.begin(), ::tolower);
	return s;
}
static inline std::string &tolinuxmode(std::string &s) {

	if (!s.empty()) 
	{
		
		if (s.at(0) != '\\' && s.at(0) != '/') 
			std::replace( s.begin(), s.end(), '\\', '/');
		else
		// remote directory
			std::replace( s.begin(), s.end(), '/', '\\');
	}
	return s;
}

static inline std::string timestamp(std::string s) {
       	std::string ts;
		time_t rawtime;
		struct tm * timeinfo;

		char buffer [150];

		time ( &rawtime );
		timeinfo = localtime ( &rawtime );
	
		strftime (buffer,sizeof(buffer),s.c_str(),timeinfo);
	
		s = buffer;

		return s;
}


}
#endif