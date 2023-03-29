/**
 * \defgroup  args
 * args (Automatic Docking Assistance)
 *
 * Copyright (C)
 * (Spain) all rights reserved.
 */
#ifndef MODULES_PARAMETERS_ARGS_H_
#define MODULES_PARAMETERS_ARGS_H_

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>


#include "parameters/argsBaseJSON.h"
#include "ext/ext.h"

/*!
 * \brief This class represents a interface to manage project input parameters
 */
class Args: public ArgsBaseJSON {
 public:
    
 protected:
    Args();

 public:

	 struct font_t
	 {
		 std::string name;
		 int size;
		 bool bold;
		 bool cursive;
	 };
    ~Args() {}
    static Args& instance() {
        static Args obj;
        return obj;
    }
    int load(const char* file);
    int load(const std::string& file) { return load(file.c_str()); }
    int save(const char* file);
    int save(const std::string& file) { return save(file.c_str()); }
    int read(const rapidjson::Document& doc);

 public:
	 std::string background;
	 int background_type;

	 bool apply_gradient_color;
	 int Ro, Go, Bo, Ao, Rd, Gd, Bd, Ad;
	 int gradient_type;

	 std::string format;   // clock format

	 std::wstring texto;
	 int Rt, Gt, Bt, At;

	 font_t text_font;
	 font_t timer_font;
	 font_t date_font;

	 int Rc, Gc, Bc, Ac;

	 std::string display_mode;


};
#endif  // MODULES_PARAMETERS_ARGS_H_
