
/*M///////////////////////////////////////////////////////////////////////////////////////
//  App:  Qt Screen Saver for Windows
//  Author: Andoni Cortés Vidal
//  email: andonicor@gmail.com
//  license: 
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                           License Agreement
//                For Free Qt Screen Saver for Windows
//
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/


#include <QObject>
#include <QApplication>
#include <QtGui>
#include <QMessageBox>
#include <QInputDialog>
#include "ScreenSaverWidget.h"
#include "parameters/args.h"

enum Mode
{
	FullScreenMode,
	ConfigurationMode,
	PreviewMode,
	DesktopMode
};
void printhelp()
{
	std::cout << " qscreensaver args" << std::endl;
	std::cout << "[-/][Cc]  or no arguments for configuration mode" << std::endl;
	std::cout << "[-/][Ss]  for fullscreen mode" << std::endl;
	std::cout << "[-/][Pp][ :][0-9]+ for preview mode" << std::endl;
	std::cout <<  "[-/][Dd] for desktop mode" << std::endl;
	
}
int main(int argc, char* argv[])
{
	// ocultar la consola
	/*#ifdef WIN32
		HWND hWnd = GetConsoleWindow();
		ShowWindow( hWnd, SW_SHOW );
	#endif
	*/

	QApplication app(argc, argv);
	QStringList arguments = app.arguments();
	
	Args &args = Args::instance();
	args.load("config.json");
	
	
	/* parse the commandline:
	 * "[-/][Cc]" or no arguments for configuration mode
	 * "[-/][Ss]", for fullscreen mode
	 * "[-/][Pp][ :][0-9]+" for preview mode
	 * "[-/][Dd]" for desktop mode
	*/
	Mode mode = ConfigurationMode;
	WId parent = 0;
	bool OK = arguments.size()<=1;
	for(int i=1;i<arguments.size();i++)
	{
		if(arguments[i].size()<2)
			break;
		switch(arguments[i][1].toLatin1())
		{
		case 'c': case 'C':	
			mode = ConfigurationMode;	
			OK = true;
			break;

		case 's': case 'S':	
			mode = FullScreenMode;	
			OK = true;	
			break;

		case 'p': case 'P':
			if(arguments[i].size()>=4 && arguments[i][2]==':')
				parent = (WId)arguments[i].mid(3).toUInt(&OK);
			else if(arguments.size()>i)
				parent = (WId)arguments[i+1].toUInt(&OK);

			//if (parent==0) QMessageBox::information(NULL, "Hi!","Parent Null");
			//else 
			//{
			//	std::stringstream ss;
			//	ss << "parent: " << parent;
				//QMessageBox::information(NULL, "Hi!", ss.str().c_str());
			//}
			mode = PreviewMode;
			break;
		case 'd': case 'D':
			//args.display_mode = "mix";
			
			mode = DesktopMode;
			OK = true;
			break;
		default:
			printhelp();
			break;
		}
	}
	if(!OK){
		QMessageBox::critical(0, "ScreenSaver error", "Couldn't parse command line:\n" + arguments.join("\n"));
		return -1;
	}

	//QWidget* widget_parent = new QWidget;
	ScreenSaverWidget* widget;
	

	
	switch (mode)
	{
	case ConfigurationMode:
	{
		QSettings settings("QT", "Screen Saver");
		int animationSpeed = settings.value("animationSpeed", 10).toInt();
		animationSpeed = QInputDialog::getInt(0, "Configure Screen Saver", "Set Animation speed (0-100%)", animationSpeed, 0, 100);
		settings.setValue("animationSpeed", animationSpeed);

	}
	return 0;

	case FullScreenMode:
		widget = new ScreenSaverWidget();
		widget->setAttribute(Qt::WA_DeleteOnClose);
		QObject::connect(widget, SIGNAL(destroyed()), &app, SLOT(quit()));

		//-- Hide the mouse --//
#ifdef Q_WS_QWS
		QWSServer::setCursorVisible(false);
#endif
		app.setOverrideCursor(QCursor(Qt::BlankCursor));
		//-- Show Screen Saver --//
		//widget_parent->showFullScreen();
		widget->show();
		widget->showFullScreen();
		return app.exec();

	case PreviewMode:
	{
		widget = new ScreenSaverWidget( parent);
		widget->setAttribute(Qt::WA_DeleteOnClose);
		//QObject::connect(widget, SIGNAL(destroyed()), &app, SLOT(quit()));
		//QObject::connect(&app, SIGNAL(quit()), widget, SLOT(close()));
		// static HDC hDC;
		//widget->hide();
		//widget->setWindowFlags(Qt::FramelessWindowHint|Qt::SubWindow);
		// InitGL( (WId)parent, hDC, hRC );
		//::SetParent(widget->winId(), (WId)parent);


		//	widget->move(0,0);
		//	widget->resize(parentRect.right, parentRect.bottom);
		//	widget->show();
		//	widget->update();
		break;
	}
	case DesktopMode:
	{
		widget = new ScreenSaverWidget( NULL);
		widget->setAttribute(Qt::WA_DeleteOnClose);
		widget->show();
		break;
	}
	}

	int res = app.exec();
	if (mode == PreviewMode) widget->close();
	return res;
}
