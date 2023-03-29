/**
 * \defgroup  argsBase
 * argsBase (Automatic Docking Assistance)
 *
 * Copyright (C)
 * (Spain) all rights reserved.
 */
#ifndef MODULES_PARAMETERS_ARGSBASE_H_
#define MODULES_PARAMETERS_ARGSBASE_H_

// C Standard Library
#include <string>
#include <vector>


/*!
 * \brief This class represents a interface to manage base input parameters
 */
class ArgsBase {
protected:
    ArgsBase(): verbose_stdout(0), show_window(false), show_debug_images(false),
                show_debug_windows(false), analyse_roi_only(false),
                processing_size_fixed(0), 
                maximize_processing_size(0) {}

 public:
    virtual ~ArgsBase()  {}
    virtual int load(const char* file) = 0;
    virtual int load(const std::string &filepath) { return load(filepath.c_str()); }
    virtual int save(const char* file) = 0;
    int save(const std::string &filepath) { return save(filepath.c_str()); }

 public:
    int verbose_stdout;
    bool show_window;
	bool velodyne;
	float framerate;
    bool show_debug_images;
    bool show_debug_windows;
    bool analyse_roi_only;
    int processing_size_fixed;
    int maximize_processing_size;
};
#endif  // MODULES_PARAMETERS_ARGSBASE_H_
