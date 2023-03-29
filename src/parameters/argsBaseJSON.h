/**
 * \defgroup  argsBaseJSON
 * argsBaseJSON (Automatic Docking Assistance)
 *
 * Copyright (C)
 * (Spain) all rights reserved.
 */
#ifndef MODULES_PARAMETERS_ARGSBASEJSON_H_
#define MODULES_PARAMETERS_ARGSBASEJSON_H_

// Fingerguide Project
#include "parameters/argsBase.h"
#include "ext/ext.h"

// Rapidjson Library
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/prettywriter.h"

/*!
 * \brief This class represents a interface to manage JSON input parameters
 */
class ArgsBaseJSON : public ArgsBase {
 protected:
    ArgsBaseJSON();

 public:
    ~ArgsBaseJSON();
    virtual int load(const char* file);
    virtual int save(const char* file);
    virtual int read(const rapidjson::Document& doc);
    virtual int write();

 public:
	rapidjson::Document m_doc;
	rapidjson::StringBuffer m_str_buffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer>* m_writer;
};
#endif  // MODULES_PARAMETERS_ARGSBASEJSON_H_
