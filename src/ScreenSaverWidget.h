#ifndef ScreenSaverWidget_H
#define ScreenSaverWidget_H

#include <QGLWidget>
#include <QMutex>
#include <QTimer>
#include <QSettings>
#include <QFont>
#include <QEvent>
#include <QTimer>
#include <string>
#include <sstream>
#include <QMessageBox>
#include <QPushButton>
#include <QRadialGradient>
#include <QPainterPath>
#include <QColor>
#include <QLinearGradient>
#include <iostream>     
#include <fstream> 
#include <string>
#include <algorithm>
#include <QMessageBox>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <ratio>
#include <chrono>

#include "file_system.h"
#include "string_ex.h"
#include "debug_ex.h"
#include "parameters/args.h"
#include "ext/ext.h"
#include "QImageButton.h"
#include "StatusManager.h"
#include "ext/ext.h"
#include "ext/ext_sys.h"

static std::string fixedLength(int value, int digits = 3) {
    unsigned int uvalue = value;
    if (value < 0) {
        uvalue = -uvalue;
    }
    std::string result;
    while (digits-- > 0) {
        result += ('0' + uvalue % 10);
        uvalue /= 10;
    }
    if (value < 0) {
        result += '-';
    }
    std::reverse(result.begin(), result.end());
    return result;
};

static std::string time_stamp(std::string format,time_t &_rawtime)
{
	std::string ts;
	
	struct tm * timeinfo;

	char buffer [150];

	time ( &_rawtime );
	timeinfo = localtime ( &_rawtime );
	
	strftime (buffer,sizeof(buffer),format.c_str(),timeinfo);
	
	ts = buffer;

	return ts;

}


class ScreenSaverWidget : public QWidget
{
	Q_OBJECT

public:
	ScreenSaverWidget( WId parent=0): QWidget() // QWidget *parent =NULL ): QWidget( parent )
	{
		StatusManager::instance().setState(StatusManager::ST_PAUSE);
		m_parent= parent;
		if(parent)	// It is necessary to use an existing window?
        {
		  //  create(parent,true);		// We initialize a window.
		    //w = width();
		    //h = height();
			RECT parentRect;
			POINT p;
			setWindowFlags(Qt::FramelessWindowHint|Qt::SubWindow|Qt::WindowStaysOnTopHint);
			::GetClientRect((HWND)parent, &parentRect);
			p.x=0;
			p.y=0;			
			::ClientToScreen((HWND)parent,&p);
			std::stringstream ss;
			ss << "parent: " << parentRect.left << "," << parentRect.top << "," << parentRect.right << "," << parentRect.bottom;
			//QMessageBox::information(NULL, "Hi!", ss.str().c_str());
			move(p.x,p.y);
			resize(parentRect.right, parentRect.bottom);
			show();
		}
		//else setFixedSize(w,h);		// Set window's size as 600x420.

		//this->hide();
		Args &args = Args::instance();

		if (!args.display_mode.compare("mix") || !args.display_mode.compare("timer"))
		{
			m_starting_time = clock();
			m_starting_tp = std::chrono::high_resolution_clock::now();
		}

		QSettings settings("QT", "Screen Saver");
		
		//this->show();
		setMouseTracking(true); // activar para que se produzcan eventos de MouseMove sin tener presionado ningun boton del ratón.
		
		m_seconds=m_minutes=m_hour=m_msecs=0;	

		//connect(&m_timer, SIGNAL(timeout()), this, SLOT(sltTimeOut()));
		m_timer.singleShot(1,this,SLOT(sltTimeOut()));

		//std::ifstream file ("c:/message.txt", std::ifstream::in);
		//std::stringstream buffer;
		//buffer << file.rdbuf();
		//m_message = buffer.str();

		//file.close();
		m_message = args.texto;

		std::string extension = file_system::file(args.background.c_str()).extension();

		QString str = "loading "+ QString (args.background.c_str());
		//QMessageBox::information(NULL,"info.", str);

		//if (!file_system::file(m_args.background.c_str()).exists())
		//	QMessageBox::information(NULL,"info.", "Background file doesn't exist.");
		 //debug_ex::error("ScreenSaverWidget","Background file doesn't exist.");
		//else 
		//	QMessageBox::information(NULL,"info.", "Background loaded.");
		//debug_ex::message(TEXT_COLOR_GREEN,"Background loaded: %s\n",m_args.background.c_str());

		extension = string_ext::makeUpper(extension);
		if (!extension.compare("JPG"))
			m_background.load(args.background.c_str(),"JPG");
		else if (!extension.compare("PNG"))
			m_background.load(args.background.c_str(), "PNG");
		else if (!extension.compare("BMP"))
			m_background.load(args.background.c_str(), "BMP");
		else if (!extension.compare("GIF"))
			m_background.load(args.background.c_str(), "GIF");
		else 
			m_background.load(args.background.c_str(), "BMP");

		if (m_background.isNull())
				m_background.load(":/backgroundbmp");
		//m_background.load("E:/Projects/QTScreenSaver/screenSaverWidgetPlata/background.bmp","BMP");
		showButtons = true;
		if (showButtons)
		{
			bPlayStop = new QImageButton(":play", this);
			connect(bPlayStop, SIGNAL(siReleased()), this, SLOT(sltChangeState()));
			bReinit = new QImageButton(":reinit", this);
			connect(bReinit, SIGNAL(siReleased()), this, SLOT(sltRestart()));
			bPlayStop->setPos(0, 0);
			bPlayStop->setSize(50, 50);
			bReinit->setPos(0, 50);
			bReinit->setSize(50, 50);


		}
		int c, r;
		ext::sys::getScreenSize(c,r);
		resize(QSize(c*0.5, r*0.5));
		//this->showFullScreen();
	}
	
	void qtPaintGDI()
	{
		QPainter painter( this );
	
		//QGL::setPreferredPaintEngine ( QPaintEngine::OpenGL2 );
		
		int w = this->width();
		int h = this->height();

		QRectF qBoundingRect(0,0,w,h);

		
		//QPointF qCenter;
		//qCenter.setX(((double)width())/2);
		//qCenter.setY(((double)height())/2);

		//QRadialGradient qGradientRadial; //I want to Light Green to Dark Green
		//qGradientRadial.setCenter(qCenter);
		//qGradientRadial.setColorAt(0.0,QColor(0,255,0));
		//qGradientRadial.setColorAt(1.0,QColor(0,120,0));
		// 
		//QPainterPath qPainterPathBones;
		//qPainterPathBones.moveTo(qCenter);
		//qPainterPathBones.arcTo(qBoundingRect,0.0,360); //From 0º to 110º
		//qPainterPathBones.closeSubpath();
		// 

		//painter.setBrush(qGradientRadial);
		//painter.drawPath(qPainterPathBones);
		
		// Draw the QPixmap
		Args &args = Args::instance();
		
		if (!args.display_mode.compare("not_initialized"))
		{
			
			//args.texto = L"Please check config.json file";
			m_message = args.texto;
			args.background_type = 1;
			args.Ro = 125;
			args.Go = 113;
			args.Bo = 134;
			args.Ao =255;
			args.Rd = 10;
			args.Gd = 10;
			args.Bd = 10;
			args.Ad=255;
			args.Rt = 200;
			args.Gt = 215;
			args.Bt = 190;
			args.At = 255;
			args.display_mode = "date";

		}
		
		if (args.background_type == 1)
		{
			painter.drawTiledPixmap ( QRect(0,0,w,h),m_background,QPoint(0,0));
		
		//	painter.drawPixmap(0,0,w,h,m_background);

			if (args.apply_gradient_color)
			{
				// Linear gradient with transparence
				QLinearGradient gradient1(rect().topLeft(), rect().bottomRight());
				QColor color_orig(args.Ro, args.Go, args.Bo, args.Ao);
				QColor color_end(args.Rd, args.Gd, args.Bd, args.Ad);
				//gradient1.setColorAt(0, QColor("#c0bfbf"));
				//gradient1.setColorAt(1, QColor("#abaaaa"));
				gradient1.setColorAt(0, color_orig);
				gradient1.setColorAt(1, color_end);

				QRectF ellipseRect(w*0.25, h*0.25, w*0.5, h*0.5);
				QLinearGradient gradient2(ellipseRect.topLeft(), ellipseRect.bottomRight());
				gradient2.setColorAt(0, QColor("#ccccff"));
				gradient2.setColorAt(1, QColor("#ffffcc"));
		
				// Draw the QLinearGradient
				painter.setRenderHint(QPainter::Antialiasing);
				painter.fillRect(rect(), QBrush(gradient1));
		
				//painter.setBrush(QBrush(gradient2));
				//painter.drawEllipse(ellipseRect);
			}
		}
		else if (args.background_type == 2)
		{
			painter.drawTiledPixmap(QRect(0, 0, w, h), m_background, QPoint(0, 0));

			//	painter.drawPixmap(0,0,w,h,m_background);
						
		}
		else if (args.background_type == 3)
		{
			int r,g,b;
			//int r = ((m_hour >> 16) & 0xff) / 255.0;
			//int g = ((m_minutes >> 16) & 0xff) / 255.0;
			//int b = ((m_seconds >> 16) & 0xff) / 255.0;
			std::stringstream oss;
			oss << "#"<< std::setfill('0') << std::setw(2)<<m_hour << std::setfill('0') <<  std::setw(2)<< m_minutes << std::setfill('0') << std::setw(2)<< m_seconds;
			std::string t = oss.str();

			painter.setPen(QPen(QColor(t.c_str())));
			painter.setBrush(QBrush(QColor(t.c_str())));
			painter.drawRect(QRect(0,0,w,h));
		}
		
	
		/*std::ostringstream oss;
		std::string time_string;
		if (m_args.format.empty())
		{
			oss << fixedLength(m_hour, 2)  << ":" <<  fixedLength(m_minutes, 2) << ":" <<  fixedLength(m_seconds, 2)  << ":" <<  fixedLength(m_msecs, 1) ;
			time_string = oss.str();
		}
		else time_string=time_stamp(m_args.format);*/

		QRect timer_rect, date_rect, message_rect;
		int sw, sh;
		ext::sys::getScreenSize(sw, sh);
		sw = this->width();
		sh = this->height();
		if (!args.display_mode.compare("mix"))
		{
			if (m_message.empty())
			{
				date_rect.setX(sw*0.1);
				date_rect.setY(sh*0.1);
				date_rect.setWidth(sw*0.8);
				date_rect.setHeight(sh*0.4);

				timer_rect.setX(sw*0.1);
				timer_rect.setY(sh*0.6);
				timer_rect.setWidth(sw*0.8);
				timer_rect.setHeight(sh*0.3);
			}
			else
			{
				date_rect.setX(sw*0.1);
				date_rect.setY(sh*0.05);
				date_rect.setWidth(sw*0.8);
				date_rect.setHeight(sh*0.4);

				timer_rect.setX(sw*0.1);
				timer_rect.setY(sh*0.5);
				timer_rect.setWidth(sw*0.8);
				timer_rect.setHeight(sh*0.2);

				message_rect.setX(sw*0.1);
				message_rect.setY(sh*0.85);
				message_rect.setWidth(sw*0.8);
				message_rect.setHeight(sh*0.1);


			}

		}
		else if (!args.display_mode.compare("date"))
		{
			if (m_message.empty())
			{
				date_rect.setX(sw*0.1);
				date_rect.setY(sh*0.1);
				date_rect.setWidth(sw*0.8);
				date_rect.setHeight(sh*0.8);

			}
			else
			{
				date_rect.setX(sw*0.1);
				date_rect.setY(sh*0.1);
				date_rect.setWidth(sw*0.8);
				date_rect.setHeight(sh*0.5);


				message_rect.setX(sw*0.1);
				message_rect.setY(sh*0.7);
				message_rect.setWidth(sw*0.8);
				message_rect.setHeight(sh*0.2);
			}

		}
		else if (!args.display_mode.compare("timer"))
		{
			if (m_message.empty())
			{
				timer_rect.setX(sw*0.1);
				timer_rect.setY(0);
				timer_rect.setWidth(sw*0.8);
				timer_rect.setHeight(sh);

			}
			else
			{
				timer_rect.setX(sw*0.1);
				timer_rect.setY(sh*0.1);
				timer_rect.setWidth(sw*0.8);
				timer_rect.setHeight(sh*0.5);


				message_rect.setX(sw*0.1);
				message_rect.setY(sh*0.7);
				message_rect.setWidth(sw*0.8);
				message_rect.setHeight(sh*0.2);
			}

		}



		QFont font_date = painter.font();
		QFont font_timer = painter.font();
		QFont font_message = painter.font();

		std::string font_name = font_timer.family().toLocal8Bit();
		
		if (!args.timer_font.name.empty())
		{
			if (args.timer_font.name.compare(font_name))
			{
				font_timer.setFamily(args.timer_font.name.c_str());
			}
		}
		font_name = font_message.family().toLocal8Bit();
		if (!args.text_font.name.empty())
		{
			if (args.text_font.name.compare(font_name))
			{
				font_message.setFamily(args.text_font.name.c_str());
			}
		}

		int font_size = h*0.15;
		int fontSizeDate=0, fontSizeTimer=0,fontSizeMessage = 0;
		font_date.setPointSize(font_size);
		font_timer.setPointSize(font_size);
		font_message.setPointSize(font_size);
		
		// calculate the adecuate font size for each text
		if (!m_date_string.empty())
		{
			QFontMetrics metrics_date(font_date);
			int pixelWidth = metrics_date.width(m_date_string.c_str());
			int pixelHeight = metrics_date.height();

			float factor = (float)date_rect.width() / (float)pixelWidth;
			if ((factor < 1) || (factor > 1.25))	fontSizeDate = font_date.pointSizeF()*factor;

			int meanSize = pixelWidth * pixelHeight;
			int meanCanvas = date_rect.width() * date_rect.height();

			font_date.setPointSize(fontSizeDate);
		}
		if (!m_time_string.empty())
		{
			QFontMetrics metrics_timer(font_timer);
			int pixelWidth = metrics_timer.width(m_time_string.c_str());
			int pixelHeight = metrics_timer.height();

			float factor = (float)timer_rect.width() / (float)pixelWidth;
			fontSizeTimer = font_timer.pointSizeF();
			if ((factor < 1) || (factor > 1.25))	fontSizeTimer = fontSizeTimer*factor;

				font_timer.setPointSize(fontSizeTimer);
		}

		if (!m_message.empty())
		{
			QFontMetrics metrics_message(font_message);
			int pixelWidth = metrics_message.width(ext::string::wstring_to_string(m_message.c_str()).c_str());
			int pixelHeight = metrics_message.height();

			float factor = (float)message_rect.width() / (float)pixelWidth;
			if ((factor < 1) || (factor > 1.25))	fontSizeMessage = font_message.pointSizeF()*factor;

			font_message.setPointSize(fontSizeMessage);
		}
		/*
		while (pixelWidth > w)
		{
			font_size = font_size - 10;
			font.setPointSize(font_size);
			QFontMetrics metrics(font);
			pixelWidth = metrics.width(m_time_string.c_str());
		}
		*/
		int y = date_rect.y();
		if (!m_date_string.empty())
		{
			
			QFontMetrics m(font_date);
			int pw = m.width(m_date_string.c_str());
			y += m.height();
			// Draw the text
			painter.setPen(QPen(QColor(args.Rc, args.Gc, args.Bc, args.Ac)));
			//painter.setBrush(QPen(QColor("#a5abbe")));
			painter.setFont(font_date);
			painter.drawText(sw*0.5 - pw*0.5,y , m_date_string.c_str());
			y += m.height();
		}

		// Draw the text
		if (!m_time_string.empty())
		{
			painter.setFont(font_timer);
			QFontMetrics m(font_timer);
			int h = ext::math::round((float)m.height()*0.5);
			int pw = m.width(m_time_string.c_str());
			y = timer_rect.y() + timer_rect.height()*0.5 + h;

			painter.setPen(QPen(QColor(args.Rc, args.Gc, args.Bc, args.Ac)));
			//painter.setBrush(QPen(QColor("#a5abbe")));
			
			//The y-position is used as the baseline of the font
			painter.drawText(sw*0.5 - pw*0.5, y , m_time_string.c_str());
			
		}


		if (!m_message.empty())
		{
			painter.setPen(QPen(QColor(args.Rt, args.Gt, args.Bt, args.At)));
			QFontMetrics m(font_message);
			int pw = m.width(ext::string::wstring_to_string(m_message.c_str()).c_str());
			int h = ext::math::round((float)m.height()*0.5);
			y = message_rect.y() + message_rect.height()*0.5 + h;

			QString qs = QString::fromWCharArray(m_message.c_str());
			painter.setFont(font_message);
			painter.drawText(sw*0.5 - pw*0.5, y, qs);
		}

	

	}

	void paintEvent ( QPaintEvent * event )
	{
	//	glClear( GL_COLOR_BUFFER_BIT );
		qtPaintGDI();
		//this->swapBuffers();
	}
	void 	resizeEvent ( QResizeEvent * event )
	{
		//glViewport(0, 0, (GLint)width, (GLint)height);		
	}
	// Gestiona eventos Qt sobre la ventana
	bool event( QEvent *event )
	{
		if (event->type() == QEvent::KeyPress) 
		{
			QKeyEvent *keyEvent = (QKeyEvent *)event;
			
			if (keyEvent->key() == Qt::Key_Escape)
				close();
			else if (keyEvent->key() == Qt::Key_F1)
			{
				if (this->isFullScreen())
				{
					this->showNormal();
				}
				else
				{
					this->showFullScreen();
				}
				event->ignore();
				return false;
			}
			event->ignore();
			return false;
		}
		else if (event->type() == QEvent::KeyRelease) 
		{
			
		}
		else if (event->type() == QEvent::MouseMove) 
		{
			QMouseEvent *mouseEvent = (QMouseEvent *)event;
		
			event->ignore();
			return false;
		}
	
		

		return QWidget::event(event);
	}
	
	long timediff(clock_t t1, clock_t t2) {
		long elapsed;
		elapsed = ((double)t2 - t1) / CLOCKS_PER_SEC * 1000;
		return elapsed;
	}
public slots:
	void sltChangeState()
	{
		if (StatusManager::instance().is(StatusManager::ST_PAUSE))
		{
			bPlayStop->setImage(":pause", 50, 50);
			StatusManager::instance().setState(StatusManager::ST_PLAY);
			StatusManager::instance().unsetState(StatusManager::ST_PAUSE);
		}
		else
		{
			bPlayStop->setImage(":play", 50, 50);
			StatusManager::instance().unsetState(StatusManager::ST_PLAY);
			StatusManager::instance().setState(StatusManager::ST_PAUSE);
		}
	}
	void sltRestart()
	{
		m_starting_time = clock();
		m_starting_tp = std::chrono::high_resolution_clock::now();
        m_msecs = m_seconds = m_hour = m_minutes = 0;
	}

private slots:	
	void sltTimeOut()
	{
		
		if (m_parent)
		{
			if (!::IsWindow((HWND)m_parent)) close();
			if (!::IsWindowVisible((HWND)m_parent)) close();
			//else if (!this->isVisible()) show();
			POINT p;
			p.x=0;
			p.y=0;
			::ClientToScreen((HWND)m_parent,&p);
			move(p.x,p.y);activateWindow();

		}
		Args& args = Args::instance();
		if (!args.display_mode.compare("timer") || !args.display_mode.compare("clock"))
		{
	        //clock_t current;

			//current =  time(0);
			
			//current = clock();
			//m_msecs = timediff(m_starting_time, current);
			//m_seconds = m_msecs / 1000;
			//m_msecs = m_msecs%1000;
			//m_msecs = m_msecs / 10;

			if (StatusManager::instance().is(StatusManager::ST_PLAY))
			{
				// con chrono
				std::chrono::high_resolution_clock::time_point current_tp = std::chrono::high_resolution_clock::now();
				std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(current_tp - m_starting_tp);
				m_msecs = time_span.count()*1000.0;
				m_seconds = m_msecs / 1000;
				m_msecs = m_msecs % 1000;
				//m_msecs = m_msecs/10;

				m_minutes = m_seconds / 60;
				m_seconds = m_seconds % 60;

				m_hour = m_minutes / 60;
				m_minutes = m_minutes % 60;

			}
			
			std::ostringstream oss;
		
			oss << fixedLength(m_hour, 2)  << ":" <<  fixedLength(m_minutes, 2) << ":" <<  fixedLength(m_seconds, 2)  << "." <<  fixedLength(m_msecs, 3) ;
			m_time_string = oss.str();
			
		}
		else if (!args.display_mode.compare("date") || !args.display_mode.compare("time"))
		{
			if (args.format.empty()) args.format = "%Y/%m/%d\nH:%M:%S";
			m_date_string=time_stamp(args.format,m_rawtime);

			struct tm * timeinfo;
			timeinfo = localtime ( &m_rawtime );
			m_hour = timeinfo->tm_hour;
			m_minutes = timeinfo->tm_min;
			m_seconds = timeinfo->tm_sec;
		}
		else if (!args.display_mode.compare("mix"))
		{
			if (args.format.empty()) args.format = "%Y/%m/%d";
			m_date_string = time_stamp(args.format, m_rawtime);

			if (StatusManager::instance().is(StatusManager::ST_PLAY))
			{
				// con chrono
				std::chrono::high_resolution_clock::time_point current_tp = std::chrono::high_resolution_clock::now();
				std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(current_tp - m_starting_tp);
				m_msecs = time_span.count()*1000.0;
				m_seconds = m_msecs / 1000;
				m_msecs = m_msecs % 1000;
				//m_msecs = m_msecs/10;

				m_minutes = m_seconds / 60;
				m_seconds = m_seconds % 60;

				m_hour = m_minutes / 60;
				m_minutes = m_minutes % 60;
			}

			std::ostringstream oss;

			oss << fixedLength(m_hour, 2) << ":" << fixedLength(m_minutes, 2) << ":" << fixedLength(m_seconds, 2) << ":" << fixedLength(m_msecs, 3);
			m_time_string = oss.str();
						
		}



		
		update();
		m_timer.singleShot(1,this,SLOT(sltTimeOut()));
	}

	void sltChangeState(QMouseEvent *event)
	{

		printf("holA");
	}

private:
	QTimer m_timer;

     clock_t m_starting_time;

	 std::chrono::high_resolution_clock::time_point m_starting_tp;
	
	time_t m_rawtime;

	 std::string m_time_string;
	 std::string m_date_string;

	int m_seconds,m_minutes,m_hour,m_msecs;
	WId m_parent;
	std::wstring m_message;
	QPixmap m_background;

	
	bool showButtons;
	QImageButton *bPlayStop;
	QImageButton *bReinit;
	

};

#endif // ReviewViewGL_H
