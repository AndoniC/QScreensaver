#include "args.h"

#include <stdio.h>
#include <stdlib.h>
#include <codecvt>

#ifndef WIN32
#include <stdexcept>  // for runtime_error
#endif

#include "ext/ext_windows.h"
#include "ext/ext_sys.h"
#include "ext/ext_fs.h"
#include "ext/ext_io.h"
/*!
 * ArgsBaseJSON Methods
 */
Args::Args() {
	display_mode = "not_initialized";
    std::vector<ext::ocv::FONT_t> l = ext::sys::getFontList();
}


int Args::read(const rapidjson::Document& doc) {
  
	if (doc.HasMember("display_mode")) {
		display_mode = doc["display_mode"].GetString();
		
	}

	if (doc.HasMember("background")) {
		
		if (doc["background"].HasMember("type"))
		{
			background_type = doc["background"]["type"].GetInt();
		}
		if (doc["background"].HasMember("image"))
		{
			if (doc["background"]["image"].HasMember("file"))
			{
				background = doc["background"]["image"]["file"].GetString();
			}

		}
		if (doc["background"].HasMember("gradient"))
		{
			apply_gradient_color = true;
			if (doc["background"]["gradient"].HasMember("from"))
			{
				if (doc["background"]["gradient"]["from"].HasMember("R"))
				{
					Ro = doc["background"]["gradient"]["from"]["R"].GetInt();
				}
				if (doc["background"]["gradient"]["from"].HasMember("G"))
				{
					Go = doc["background"]["gradient"]["from"]["G"].GetInt();
				}
				if (doc["background"]["gradient"]["from"].HasMember("B"))
				{
					Bo = doc["background"]["gradient"]["from"]["B"].GetInt();
				}
				if (doc["background"]["gradient"]["from"].HasMember("A"))
				{
					Ao = doc["background"]["gradient"]["from"]["A"].GetInt();
				}
			}
			if (doc["background"]["gradient"].HasMember("to"))
			{
				if (doc["background"]["gradient"]["to"].HasMember("R"))
				{
					Rd = doc["background"]["gradient"]["to"]["R"].GetInt();
				}
				if (doc["background"]["gradient"]["to"].HasMember("G"))
				{
					Gd = doc["background"]["gradient"]["to"]["G"].GetInt();
				}
				if (doc["background"]["gradient"]["to"].HasMember("B"))
				{
					Bd = doc["background"]["gradient"]["to"]["B"].GetInt();
				}
				if (doc["background"]["gradient"]["to"].HasMember("A"))
				{
					Ad = doc["background"]["gradient"]["to"]["A"].GetInt();
				}
			}
			if (doc["background"]["gradient"].HasMember("type"))
			{
				gradient_type = doc["background"]["gradient"]["type"].GetInt();
			}

		}

	}

	if (doc.HasMember("text"))
	{
		if (doc["text"].HasMember("display")) {
			texto = ext::string::utf8_to_wstring(m_doc["text"]["display"].GetString());
			//std::cout << texto.c_str() << std::endl;
		}
		if (doc["text"].HasMember("text_color")) {
			if (doc["text"]["text_color"].HasMember("R"))
			{
				Rt = doc["text"]["text_color"]["R"].GetInt();
			}
			if (doc["text"]["text_color"].HasMember("G"))
			{
				Gt = doc["text"]["text_color"]["G"].GetInt();
			}
			if (doc["text"]["text_color"].HasMember("B"))
			{
				Bt = doc["text"]["text_color"]["B"].GetInt();
			}
			if (doc["text"]["text_color"].HasMember("A"))
			{
				At = doc["text"]["text_color"]["A"].GetInt();
			}
		}
		if (doc["text"].HasMember("font"))
		{
			if (doc["text"]["font"].HasMember("name"))
			{
				text_font.name = doc["text"]["font"]["name"].GetString();
			}
			if (doc["text"]["font"].HasMember("size"))
			{
				text_font.size = doc["text"]["font"]["size"].GetInt();
			}
			if (doc["text"]["font"].HasMember("bold"))
			{
				text_font.bold = doc["text"]["font"]["bold"].GetBool();
			}


		}
	}
	if (doc.HasMember("clock")) {
		if (doc["clock"].HasMember("color")) {
			if (doc["clock"]["color"].HasMember("R"))
			{
				Rc = doc["clock"]["color"]["R"].GetInt();
			}
			if (doc["clock"]["color"].HasMember("G"))
			{
				Gc = doc["clock"]["color"]["G"].GetInt();
			}
			if (doc["clock"]["color"].HasMember("B"))
			{
				Bc = doc["clock"]["color"]["B"].GetInt();
			}
			if (doc["clock"]["color"].HasMember("A"))
			{
				Ac = doc["clock"]["color"]["A"].GetInt();
			}
		}
		if (doc["clock"].HasMember("format")){
			format = doc["clock"]["format"].GetString();
		}

		if (doc["clock"].HasMember("font"))
		{
			if (doc["clock"]["font"].HasMember("name"))
			{
				timer_font.name = doc["clock"]["font"]["name"].GetString();
			}
			if (doc["clock"]["font"].HasMember("size"))
			{
				timer_font.size = doc["clock"]["font"]["size"].GetInt();
			}
			if (doc["clock"]["font"].HasMember("bold"))
			{
				timer_font.bold = doc["text"]["font"]["bold"].GetBool();
			}


		}
	}

   
    return 1;
}
int Args::load(const char* file) {
	// check if the file exists
	if (!ext::fs::fexists(file)) {
	 //   throw std::runtime_error("Could't load the JSON configuration file...");
		ext::io::cout_message(ext::io::FG_RED, "Config file does not exist.\n");
		this->texto = L"Config file does not exist";
			return -1;
	}

	

	// read from json file
	std::string json_txt, line_aux;
	std::ifstream myfile(file);
	if (myfile.is_open()) {
	    while (getline(myfile, line_aux)) {
	        json_txt += line_aux;
	    }
	    myfile.close();
	}

	// parse parameters from file
	rapidjson::StringStream ss(json_txt.c_str());
	m_doc.ParseStream(ss);
	rapidjson::ParseErrorCode error = m_doc.GetParseError();
	
	//ArgsBaseJSON::read(m_doc);
	if (error != rapidjson::kParseErrorNone)
	{
		size_t idx = m_doc.GetErrorOffset();
		//std::cout << "Error parsing json (" << idx << ")"<< std::endl;
		this->texto = L"Error parsing config.json (" + std::to_wstring(error) + L" - " + std::to_wstring(idx) + L")";
	}
	else
		read(m_doc);
	return 1;
}
int Args::save(const char* file) {
    // set MODBUS parameters
    m_writer->Key("display_mode");
	m_writer->String(display_mode.c_str());

    // write base arguments
	//ArgsBaseJSON::save(file);
	
	return 1;
}
