#---------------------------------------------------#
#          CMake Project Configuration              #
#---------------------------------------------------#
# CMake will issue a warning if you don't set this
CMAKE_MINIMUM_REQUIRED(VERSION 3.1)


#@ Carpeta de modulos
set (MODULES_folder "./modules" CACHE PATH "Path to folder with modules (ext, openssl, exif, matlablib, ... )" ) 

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${MODULES_folder}/CMakeModules/")

#@ check architecture
include("${MODULES_folder}/CMakeModules/TargetArch.cmake")
target_architecture (TARGET_COMPILATION_ARCH)
#check : IF(TARGET_COMPILATION_ARCH MATCHES x86_64)
message(STATUS "** Compilation Target is ${TARGET_COMPILATION_ARCH} bits **")




if(COMMAND cmake_policy)
     cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

PROJECT( QTScreenSaver )


SET_PROPERTY( GLOBAL PROPERTY USE_FOLDERS ON ) # to organize in folders

# Version
SET( APP_VERSION_MAJOR 1 )
SET( APP_MINOR_VERSION 3 )
SET( APP_PATCH_VERSION 0 )
SET( APP_STATUS_VERSION "dev") #1.3.0 : first version


#@ Set additional functions library files
include(${MODULES_folder}/ext/Ext.cmake)

#@ Set capture library files
#include(${MODULES_folder}/capture/Capture.cmake )


#---------------------------------------------------#
#              QT Configuration                     #
#---------------------------------------------------#

FIND_PACKAGE( Qt5Core QUIET)
#FIND_PACKAGE(Qt5Widgets)
# Qt5CoreConfig.cmake  can be found in C:/Qt/Qt5.4.1/5.4/msvc2013_opengl/lib/cmake/Qt5Core

if(  NOT Qt5Core_FOUND )
	message(STATUS "Using Qt4...")
	SET( QT_USE_QTOPENGL 1 )
	SET( QT_USE_QTNETWORK 1 )
	FIND_PACKAGE( Qt4 REQUIRED )

	IF ( QT_FOUND)
		INCLUDE( ${QT_USE_FILE} )
		SET(QT_VERSION "Qt4")
	ELSE ( QT_FOUND)
		MESSAGE( FATAL_ERROR "This application requires Qt. One of these components is missing. Please verify configuration")
	ENDIF ( QT_FOUND)
else(   NOT Qt5Core_FOUND )
	message(STATUS "Using Qt5...")
	FIND_PACKAGE( Qt5OpenGL )
	FIND_PACKAGE( Qt5Widgets )
	FIND_PACKAGE( Qt5Network )

	# Tell CMake to run moc when necessary:
    #set(CMAKE_AUTOMOC ON)

ENDIF(   NOT Qt5Core_FOUND )


#---------------------------------------------------#
#               Project Configuration               #
#---------------------------------------------------#
SET ( EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/ CACHE PATH "Single output directory for building all executables." FORCE )
SET ( CMAKE_DEBUG_POSTFIX d )
SET ( LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/ CACHE PATH "Single output directory for building all libraries." FORCE )
SET ( EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/ CACHE PATH "Single output directory for building all executables." FORCE )

#---------------------------------------------------#
#           Include directories                     #
#---------------------------------------------------#

SET (include_dirs
	${CMAKE_CURRENT_BINARY_DIR} #para que pille los UIS_H
	${INCLUDE_PATHS} # para viulib
	"src"
)

INCLUDE_DIRECTORIES( ${include_dirs} )
LINK_DIRECTORIES(${VideoMan_LIB_DIR})
#---------------------------------------------------#
#    Files of the project .cpp & .h & moc files     #
#---------------------------------------------------#

# Sources

FILE( GLOB PROJECT_SRCS
    "src/*.cpp"
)

FILE( GLOB PROJECT_HEADERS
    "src/*.h"
)


FILE( GLOB PARAMETERS_SRCS
    "src/parameters/*.cpp"
)
FILE( GLOB PARAMETERS_HEADERS
    "src/parameters/*.h"
)

SET( MOC_FILES
src/ScreenSaverWidget.h
src/QAnimatedButton.h
src/QAnimatedClip.h
src/QImageButton.h
)

#---------------------------------------------------#
#   					 GROUPS				        #
#---------------------------------------------------#
SOURCE_GROUP(UIs FILES ${UIS})

#---------------------------------------------------#
#   			 configure QT				        #
#---------------------------------------------------#
set(RC_FILES src/imagenes.qrc )




if(Qt5Core_FOUND)
	
	message(STATUS "Creating MOC Qt5 Files ....")
	# Configure Qt
	SET( QT_USE_QTOPENGL TRUE )
	
	QT5_WRAP_UI( SOURCES_UIS_H ${UIS} )
	QT5_WRAP_CPP( MOC_SOURCE_FILES ${MOC_FILES} )
	QT5_ADD_RESOURCES( RC_SRC_FILES ${RC_FILES} )

	# Use the compile definitions defined in the Qt 5 Widgets module
	add_definitions(${Qt5Widgets_DEFINITIONS})
	add_definitions(${Qt5OpenGL_DEFINITIONS})
	add_definitions(${Qt5Core_DEFINITIONS})
	add_definitions(${Qt5Network_DEFINITIONS})
	
	# Add the include directories for the Qt 5 Widgets module to
	# the compile lines.
	SET( QT_INCLUDE_DIR ${QT_INCLUDE_DIR} ${Qt5OpenGL_INCLUDE_DIRS} )
	SET( QT_INCLUDE_DIR ${QT_INCLUDE_DIR} ${Qt5Core_INCLUDE_DIRS} )
	SET( QT_INCLUDE_DIR ${QT_INCLUDE_DIR} ${Qt5Widgets_INCLUDE_DIRS} )
	SET( QT_INCLUDE_DIR ${QT_INCLUDE_DIR} ${Qt5Network_INCLUDE_DIRS} )
	
	# Add compiler flags for building executables (-fPIE)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
	
ELSE(Qt5Core_FOUND)
	message(STATUS "Creating MOC Qt4 Files ....")
	
	QT4_WRAP_UI( SOURCES_UIS_H ${UIS} )
	QT4_WRAP_CPP(MOC_SOURCE_FILES ${MOC_FILES}  )
	QT4_ADD_RESOURCES( RC_SRC_FILES ${RC_FILES} )
	SET( QT_INCLUDE_DIR ${QT_INCLUDE_DIRS} )
SOURCE_GROUP( "Generated Files" FILES ${MOC_SOURCE_FILES} )# ${SOURCES_UIS_H} )

ENDIF(Qt5Core_FOUND)

SOURCE_GROUP( "Generated Files" FILES ${MOC_SOURCE_FILES}  ${SOURCES_UIS_H}  ${RC_SRC_FILES})
SOURCE_GROUP("params" FILES ${PARAMETERS_SRCS} ${PARAMETERS_HEADERS})
SOURCE_GROUP("ext" FILES ${EXT_SRCS})

include_directories(${QT_INCLUDE_DIR})

#---------------------------------------------------#
#              Control version                 #
#---------------------------------------------------#
ADD_DEFINITIONS(-DAPP_VERSION_MAJOR=${APP_VERSION_MAJOR} 
						    -DAPP_VERSION_MINOR=${APP_VERSION_MINOR}
							-DAPP_VERSION_PATCH=${APP_VERSION_PATCH}
							-DAPP_VERSION_STATUS="${APP_VERSION_STATUS}" )

#---------------------------------------------------#
#   			 EXECUTABLE					        #
#---------------------------------------------------#
ADD_EXECUTABLE( QTScreenSaver WIN32 ${PROJECT_SRCS}  ${EXT_SRCS} ${PROJECT_HEADERS} ${PARAMETERS_SRCS} ${PARAMETERS_HEADERS} ${EXT_SRCS} ${MOC_SOURCE_FILES} ${SOURCES_UIS_H} ${RC_SRC_FILES} )


#---------------------------------------------------#
#               Link Libraries                      #
#---------------------------------------------------#

#TARGET_LINK_LIBRARIES( QTScreenSaver ${OpenCV_LIBS} ${viulib_LIBS} ${VideoMan_LIBS})
TARGET_LINK_LIBRARIES( QTScreenSaver  ${OpenCV_LIBS} ${VideoMan_LIBS} ${OPENGL_LIBRARIES})


#  * to include DEBUG or RELEASE libraries:
 FOREACH(lib ${EXT_DEBUG_LIBRARIES})
		target_link_libraries(QTScreenSaver debug ${lib} )
 ENDFOREACH()
 FOREACH(lib ${EXT_RELEASE_LIBRARIES})
		target_link_libraries(QTScreenSaver optimized ${lib} )
 ENDFOREACH()
#  * to include DEFS:
 FOREACH(edef ${EXT_DEFS})
		target_compile_definitions( QTScreenSaver PUBLIC  ${edef})
 ENDFOREACH()
 target_link_directories(QTScreenSaver PRIVATE ${EXT_LIB_DIRS})
 target_include_directories(QTScreenSaver PRIVATE ${EXT_INCLUDE_DIRS})
 target_include_directories(QTScreenSaver PRIVATE ${MODULES_folder})
 


if(Qt5Core_FOUND)
		target_link_libraries(QTScreenSaver ${Qt5Core_QTMAIN_LIBRARIES} ${Qt5Network_QTMAIN_LIBRARIES}  )# <--  Qt5::WinMain ${Qt5Widgets_LIBRARIES} ${Qt5OpenGL_LIBRARIES})
		qt5_use_modules( QTScreenSaver Widgets OpenGL Core Network ) 
	
else (Qt5Core_FOUND)
	target_link_libraries( QTScreenSaver  ${QT_LIBRARIES})
	SET_TARGET_PROPERTIES( QTScreenSaver PROPERTIES  VS_KEYWORD Qt4VSv1.0 )
endif(Qt5Core_FOUND)

#---------------------------------------------------#
#    Para que el integration del VS funcione	    #
#---------------------------------------------------#
SET_TARGET_PROPERTIES(QTScreenSaver PROPERTIES  VS_KEYWORD Qt4VSv1.0 )
#set_target_properties(QTScreenSaver PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup")
set_target_properties(QTScreenSaver PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup ") # /NOENTRY")
set_target_properties(QTScreenSaver PROPERTIES SUFFIX  ".scr")


#-------------------------------------------#
#   			 INSTALLER			        #
#-------------------------------------------#
if (WIN32)
	
	IF(TARGET_COMPILATION_ARCH MATCHES x86)
		
		SET ( INSTALLER_QT_DLLS_DIR "C:/Qt/Qt5.4.1/5.4/msvc2013_opengl" CACHE PATH "OPENCV DLLS DIR")
		
		
	ELSE(TARGET_COMPILATION_ARCH MATCHES x86)
		SET ( INSTALLER_QT_DLLS_DIR "C:/Qt/Qt5.4.1/5.4/msvc2013_opengl" CACHE PATH "OPENCV DLLS DIR")
		
		
	ENDIF(TARGET_COMPILATION_ARCH MATCHES x86)
else (WIN32)
		SET ( INSTALLER_QT_DLLS_DIR "C:/Qt/Qt5.4.1/5.4/msvc2013_opengl" CACHE PATH "OPENCV DLLS DIR")
		
endif (WIN32)

Message("Adding QT DLLS from: " ${INSTALLER_QT_DLLS_DIR})

#file( GLOB QT_DLL_FILES  ${INSTALLER_QT_DLLS_DIR}/bin/*.dll
#						 ${INSTALLER_QT_DLLS_DIR}/plugins/imageformats/*.dll)

SET(QT_DLL_FILES 
${INSTALLER_QT_DLLS_DIR}/bin/Qt5Core.dll
${INSTALLER_QT_DLLS_DIR}/bin/Qt5Gui.dll
${INSTALLER_QT_DLLS_DIR}/bin/Qt5gui.dll
)
SET ( QT_PLATFORM_FILES 
${INSTALLER_QT_DLLS_DIR}/plugins/platforms/qwindows.dll
)

SET ( DLL_FILES 
${QT_DLL_FILES}
)


#SET ( QT_FILES 
#${QT_DIR}/plugins/imageformats/qgif.dll
#${QT_DIR}/plugins/imageformats/qico.dll
#${QT_DIR}/plugins/imageformats/qjpeg.dll
#${QT_DIR}/plugins/imageformats/qmng.dll
#${QT_DIR}/plugins/imageformats/qsvg.dll
#${QT_DIR}/plugins/imageformats/qtiff.dll
#)


SET ( DOC_FILES
copyright.txt
config.json
)
FILE(GLOB IMGS_FILES ./etc/imgs/*.*)

SET (ETC_FILES
background.jpg
Digit.ttf
)

# Establece el directorio de instalación al directorio actual?
set(CPACK_NSIS_EXECUTABLES_DIRECTORY ".")

			
SET(CPACK_PACKAGE_EXECUTABLES QTScreenSaver "CustomScreenSaver") 

#set(CPACK_NSIS_MENU_LINKS
#	"QTScreenSaver.exe" "QTScreenSaver"
#	"copyright.txt" "License"
#)

# COMPONENTES DE LA INSTALACION
INSTALL( TARGETS QTScreenSaver 
            RUNTIME DESTINATION "."
			COMPONENT app)
#INSTALL( FILES ${QT_FILES}
#			DESTINATION "./imageformats"
#			COMPONENT qt_support)
INSTALL( FILES ${DLL_FILES}
			DESTINATION "."
			COMPONENT dlls)

INSTALL( FILES ${QT_PLATFORM_FILES}
			DESTINATION "./platforms"
			COMPONENT dlls
			)

INSTALL( FILES ${DOC_FILES}
            DESTINATION "."
			COMPONENT docs)
INSTALL( FILES ${ETC_FILES}
            DESTINATION "./etc"
			COMPONENT docs)
INSTALL( FILES ${IMGS_FILES}
            DESTINATION "./etc/imgs"
			COMPONENT docs)

			
set(CPACK_RPM_COMPONENT_INSTALL ON)

#GRUPOS A LOS QUE PERTENECEN CADA UNO DE LOS COMPONENTES
set(CPACK_COMPONENT_APP_GROUP "Application")
set(CPACK_COMPONENT_DLLS_GROUP "3rd_Party")
set(CPACK_COMPONENT_QT_SUPPORT_GROUP "3rd_Party")
set(CPACK_COMPONENT_DOCS_GROUP "Application")

set(CPACK_COMPONENT_GROUP_APPLICATION_DESCRIPTION   "Custom Screen Saver")

# set(CPACK_COMPONENT_SAMPLES_DISABLED OFF)
#NOMBRE QUE SE MUESTRA EN LA LISTA PARA CADA UNO DE LOS COMPONENTES
set(CPACK_COMPONENT_APP_DISPLAY_NAME "Aplicación")
set(CPACK_COMPONENT_DLLS_DISPLAY_NAME "Dlls")
set(CPACK_COMPONENT_QT_SUPPORT_DISPLAY_NAME "Qt support")

 #DESCRIPCION QUE SE MUESTRA PARA CADA UNO DE LOS COMPONENTES
set(CPACK_COMPONENT_APP_DESCRIPTION 
   "Instalación de los ficheros de la aplicación")
set(CPACK_COMPONENT_DLLS_DESCRIPTION
   "Dlls")
set(CPACK_COMPONENT_QT_SUPPORT_DESCRIPTION
   "Install only if you don't have qt installed")

#REGISTRAMOS LOS NOMBRES DE LOS COMPONENTES PARA EL CPACK
 set(CPACK_COMPONENTS_ALL app dlls qt_support docs  )

 
#CONFIGURAR CPACK
 set(CPACK_RESOURCE_FILE_LICENSE  ${PROJECT_SOURCE_DIR}/COPYRIGHT.txt)
 set(CPACK_PACKAGE_NAME "qtscreensaver")
 set(CPACK_PACKAGE_VENDOR "customtimer")
 set(CPACK_PACKAGE_DESCRIPTION_SUMMARY qtscreensaver)
 set(CPACK_PACKAGE_VERSION "${APP_VERSION_MAJOR}.${APP_MINOR_VERSION}.${APP_PATCH_VERSION}.${APP_STATUS_VERSION}")
 set(CPACK_PACKAGE_VERSION_MAJOR ${APP_VERSION_MAJOR})
 set(CPACK_PACKAGE_VERSION_MINOR ${APP_VERSION_MINOR})
 set(CPACK_PACKAGE_VERSION_PATCH ${APP_VERSION_PATCH})
 set(CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_VENDOR}\\\\${CPACK_PACKAGE_NAME}${APP_VERSION_MAJOR}.${APP_MINOR_VERSION}.${APP_PATCH_VERSION}.${APP_STATUS_VERSION}")
 
INCLUDE(CPack)

