/*
* argsBaseJSON.cpp:
* argsBaseJSON
*
* Copyright (C)
* (Spain) all rights reserved.
*/

// Fingerguide Project
#include "parameters/argsBaseJSON.h"

// C Standard Library
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#ifndef WIN32
#include <stdexcept>  // for runtime_error
#include <typeinfo>
#endif

/*!
 * ArgsBaseJSON Methods
 */
ArgsBaseJSON::ArgsBaseJSON() {
    m_writer = new rapidjson::PrettyWriter<rapidjson::StringBuffer>(m_str_buffer);
    m_writer->StartObject();
}
ArgsBaseJSON::~ArgsBaseJSON() {
    delete m_writer;
}
int ArgsBaseJSON::read(const rapidjson::Document& doc) {
    // general Parameters
	
    return 1;
}
int ArgsBaseJSON::load(const char* file) {
    // read  from json file
    std::string json_txt, line_aux;
    printf("reading json file...\n");
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
    read(m_doc);
	
	return 1;
}
int ArgsBaseJSON::write() {
  
	return 1;
}
int ArgsBaseJSON::save(const char* file) {
	write();
	m_writer->EndObject();
	std::ofstream of(file);
	of << m_str_buffer.GetString();
	if (!of.good())
		throw std::runtime_error("Can't write the JSON string to the file!");
	of.close();

	return 1;
}
