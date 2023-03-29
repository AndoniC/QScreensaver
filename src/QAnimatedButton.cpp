//#include "math.h"
#include "QAnimatedButton.h"
#include <QPainter>
#include <QPaintEvent>
#include <QBitmap>
#include <QMessageBox>
#include <QApplication>
#include <QDesktopWidget>
#include <iostream>


using namespace std;

static QMutex m_mut;
static int m_number_of_processes_painting=0;
static int m_stop=0;

syncronization::syncronization() 
{

}
syncronization::~syncronization()
{
}

bool syncronization::add() 
{
		QMutexLocker lock(&m_mut); 
		if (!m_stop) 
		{
			m_number_of_processes_painting++; 
			//std::cout << "processPainting " <<m_number_of_processes_painting<<std::endl; 
			return true;
		} 
		else return false;
}
void syncronization::sub() 
{
	m_number_of_processes_painting--;
//	std::cout << "processPainting " <<m_number_of_processes_painting<<std::endl; 
}
void syncronization::setStop(int val) 
{
	QMutexLocker lock(&m_mut); 
	//std::cout << "Stop a " << val << "  processPainting "<< m_number_of_processes_painting <<std::endl; 
	m_stop=val;
}
bool syncronization::wait() 
{	//std::cout << "m_number_of_processes_painting "<< m_number_of_processes_painting <<std::endl; 
	return (m_number_of_processes_painting==0);
}

QAnimatedButton::QAnimatedButton(QWidget *parent)
	: QWidget(parent)
{
	create("default");
	timed=false;
	enabled=true;

}
QAnimatedButton::QAnimatedButton(std::string id,QWidget *parent)
	: QWidget(parent)
{
	create(id);
	timed=false;
	enabled=true;

}

QAnimatedButton::QAnimatedButton(std::string id,QString fover, QString fout, QString fpush,QWidget *parent): QWidget(parent)
	//: __movie(fgif.toStdString().c_str())
{
	m_active=0;
	create (id,fover.toStdString(),fout.toStdString(),fpush.toStdString());
	timed=false;
	enabled=true;
		
	

}

QAnimatedButton::~QAnimatedButton()
{
	/*if (barTimer) {
		barTimer->stop();
		delete barTimer;
		barTimer=0;
	}
	if (timer)
	{
			timer->stop();
			delete timer;
			timer=0;
	}*/

}

void QAnimatedButton::setID(std::string id)
{

	__id = id;
	if (timed && enabled)
	{
		bar=new QProgressBar(this);
		
		//bar->resize(200,10);
		bar->resize(this->width()*0.9, this->height()*0.1);
		bar->setOrientation(Qt::Horizontal);
		bar->setRange(0,99);
		bar->setValue(0);
		
		barTimer = new QTimer(this);
		connect(barTimer, SIGNAL(timeout()), this, SLOT(plusOne()));
		connect(bar, SIGNAL(valueChanged(int)), this, SLOT(timedValue(int)));
		bar->hide();
		bar->setTextVisible(false);
		timer=0;
		//timer = new QTimer(this); // timer before appearing bar
		//cout<<"Creo el timer "<<this->__id<<"\n";
	}
}
void QAnimatedButton::create(std::string id,std::string fout, std::string fover, std::string fpush)
{

	timer=NULL;

	labelText = new QLabel(this);
	this->setWindowFlags(Qt::Window |  Qt::FramelessWindowHint | Qt::CustomizeWindowHint );//| Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint );//| Qt::WindowTitleHint);
	//this->setWindowOpacity( 0.5 );
	//this->setAttribute( Qt::WA_TransparentForMouseEvents );
	//cursorWidget.setEnabled( false );
	//this->setFocusPolicy( Qt::NoFocus );
	this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
	__state=M_OUT;
	__m1=__m2=10;
	__id = id;


	if (!fout.empty()) 
	{
		__vstate[M_OUT].file_name= fout.c_str();
		__vstate[M_OUT].type = filetype(__vstate[M_OUT].file_name);
	
		__state = M_OUT;

		if (__vstate[M_OUT].type == GIF)
		{
			__vstate[M_OUT].mov.setFileName(__vstate[M_OUT].file_name);
			//	QSize s = __movie.pixma ().size();	
			//	this->resize(s.width(),s.height());
			//if (parent!=NULL) this->setParent(parent);
			connect(&__vstate[M_OUT].mov,SIGNAL(frameChanged(int)),this,SLOT(paintNewFrame(int)));
		}
		else
		{
			__vstate[M_OUT].img.load(__vstate[M_OUT].file_name);
			this->setMask(__vstate[M_OUT].img.mask());
		}
	}

	

	if (!fover.empty()) 
	{
		__vstate[M_OVER].file_name = fover.c_str();
		__vstate[M_OVER].type = filetype(__vstate[M_OVER].file_name);
		if (__vstate[M_OVER].type == GIF)
		{
			__vstate[M_OVER].mov.setFileName(__vstate[M_OVER].file_name);
			//	QSize s = __movie.pixma ().size();	
			//	this->resize(s.width(),s.height());
			//if (parent!=NULL) this->setParent(parent);
		//	connect(&__vstate[OVER].mov,SIGNAL(frameChanged(int)),this,SLOT(paintNewFrame(int)));
		}
		else 
			__vstate[M_OVER].img.load(__vstate[M_OVER].file_name);
	}		
	if (!fpush.empty()) 
	{
		__vstate[M_PRESSED].file_name= fpush.c_str();
		__vstate[M_PRESSED].type = filetype(__vstate[M_PRESSED].file_name);
		if (__vstate[M_PRESSED].type == GIF)
		{
			__vstate[M_PRESSED].mov.setFileName(__vstate[M_PRESSED].file_name);
			//	QSize s = __movie.pixma ().size();	
			//	this->resize(s.width(),s.height());
			//if (parent!=NULL) this->setParent(parent);
		//	connect(&__vstate[PRESSED].mov,SIGNAL(frameChanged(int)),this,SLOT(paintNewFrame(int)));
		}
		else 
			__vstate[M_PRESSED].img.load(__vstate[M_PRESSED].file_name);

	}

	this->setMouseTracking(true);


	show();
	
	//__size.setWidth(100);
	//__size.setHeight(100);
}
void QAnimatedButton::plusOne()
{
	//std::cout << "bar :"<<__text << std::endl;

	if (!this->m_active) 
	{
		if(timed) eliminarTimers();
		return; 
	}
	if(!enabled || !this->isVisible())
	{
		if (timer)
		{
			disconnect(timer, SIGNAL(timeout()), this, SLOT(visibleBar()) );
			delete timer;
			timer=0;
		}
		return;
	}
	int valor=bar->value();
	bar->setValue(valor+1);

	std::cout << "99" << ":" << valor << "\r";
	if (valor == 99)
	{
		std::cout << "fin barra \n" << std::endl;	
		barTimer->stop();
		bar->setValue(0);
		bar->hide();
		bar->update();

		QMouseEvent *event=NULL;
		
		if (!timer)
		{
			std::cout << "Start timer again inside bar timer" <<std::endl;

			timer = new QTimer();
			timer->stop();
			connect(timer, SIGNAL(timeout()), this, SLOT(visibleBar()) );
			timer->start(timeBarDelay);
		}

		else std::cout << "ERROR: \n    Fin de la barra y timer activo." <<std::endl;
		
		//emit mousePressEvent(event);
		press(event);

	}
	else bar->update();
	//cout<<"Start timer\n";

}
void QAnimatedButton::addText(QString text,QFont font,STATE st)
{
	__text=text.toLatin1().constData();
	addText(__text,font,st);
	
}	
void QAnimatedButton::addText(std::string text,QFont font,STATE st)
{
	__text=text;
	if(!__type.compare("display"))
	{
		if (__vstate[st].text.size() > text.size())
			font.setPointSize(__vstate[st].font.pointSize()+10);
		else if (__vstate[st].text.isEmpty())
		{
			font.setPointSize(0.25*this->width());

		}
		else 
			font.setPointSize(__vstate[st].font.pointSize());

		__vstate[st].text=text.c_str();
		__m1=0.03*this->width();
		__m2=0.04*this->height();
		//	__vstate[__state].font.setPointSize(0.25*w);
		//adjustTextSize(__vstate[__state].font);
		__vstate[st].font.setPointSize(fitSize(std::string(__vstate[st].text.toLatin1()),font, QRect(__m1,__m2,this->width()-__m1 *2,this->height()-__m2*2)));
		return ;
	}
	
	__vstate[st].text=text.c_str();

	int w = this->width();
	int h = this->height();

	/*cout <<"Width "<<w<<"\n";
	cout <<"Height "<<h<<"\n";
*/
	//labelText->setText(text);
#ifndef WIN32
	QTextCodec *linuxCodec = QTextCodec::codecForName("UTF-8");
	QTextCodec::setCodecForTr(linuxCodec);
	QTextCodec::setCodecForCStrings(linuxCodec);
	QTextCodec::setCodecForLocale(linuxCodec);
#endif 
	labelText->setText(__text.c_str());
	labelText->setFont(font);
	if (!__id.compare("YES") || !__id.compare("NO") || !__id.compare("SPEAK") || !__id.compare("ALARM") || !__id.compare("DELETE"))
	{
		labelText->setStyleSheet("QLabel { color : white; }");
	}


//	cout<<" "<<__m1<<"\n";
	//labelText->setGeometry(0,__size.height()/3,__size.width(),__size.height());
	labelText->setFixedHeight(__size.height()/6);
	labelText->setFixedWidth(__size.width()-__size.width()/10);
	labelText->move(__size.width()/20,__size.height()-__size.height()/5);

	//labelText->move(10,__size.height()-35);
	labelText->setAlignment(Qt::AlignCenter);
	labelText->show();
	labelText->raise();
	labelText->update();
	/*cout<<"Raise "<<labelText->text().toUtf8().constData()<<"\n";
	cout<<"Raise "<<labelText->isVisible()<<"\n";*/
	/*cout<<"Pinto en "<< __size.height()<<"\n";
	cout<<"Pinto en "<<__size.width()<<"\n";*/


}
QString QAnimatedButton::getText(STATE st)
{
	return __vstate[st].text;
}

int QAnimatedButton::fitSize(std::string text,QFont &f, QRect size)
{
	int fit = false;
		
	QFont myFont = f;	

	while (!fit && myFont.pointSize() > 6)
	{
		QFontMetrics fm( myFont );		
		QRect bound = fm.boundingRect(size,/*Qt::TextWrapAnywhere*/Qt::TextWordWrap|Qt::AlignLeft |Qt::AlignTop, text.c_str());
		
		if (bound.width() <= size.width() &&
		    bound.height() <= size.height())
		    fit = true;
		else
			myFont.setPointSize(myFont.pointSize() - 1);
	}
	
	return myFont.pointSize();
}

void QAnimatedButton::adjustTextSize(QFont &f)
{


	int w = this->width();
	int h = this->height();
	//cout <<"width "<<w<<" x height "<<h<<"\n";
	w -= 2*__m1;
	h -= 2*__m2;
//	QRect rwindow(100,100,w,h);
	QRect rwindow(__m1,__m2,w,h);
	QFontMetrics fm(f);
	QRect r = fm.boundingRect ( rwindow,Qt::TextWordWrap|Qt::AlignLeft , __vstate[__state].text);

/**
 * Tendría que tocar esto?
 */

	if (r.height()>h)
	{
		QFont faux = __vstate[__state].font;
		for (int i=0;i< 30;i++)
		{
			faux.setPointSize(faux.pointSize() - 1);
			QFontMetrics fm(faux);
			QRect r = fm.boundingRect ( rwindow,Qt::TextWordWrap|Qt::AlignLeft |Qt::AlignTop , __vstate[__state].text);
			
			if (r.height()<h) break;
			__vstate[__state].font.setPointSize(faux.pointSize());
			//cout << "Cambio el tamaño del texto a 1 "<<faux.pointSize()<<" \n";
		}
		
		
	}
	else 
	{
		QFont faux = __vstate[__state].font;
		for (int i=0;i< 30;i++)
		{
			faux.setPointSize(faux.pointSize() + 1);
			QFontMetrics fm(faux);
			QRect r = fm.boundingRect ( rwindow,Qt::TextWordWrap|Qt::AlignLeft , __vstate[__state].text);
			if (r.height()>h) break;
			__vstate[__state].font.setPointSize(faux.pointSize());
			//cout << "Cambio el tamaño del texto a 2 "<<faux.pointSize()<<" "<<r.height()<<" \n";
			
			
		}

	}


}

void QAnimatedButton::load(QString f,STATE st)
{
	
		QRect screenResolution=QApplication::desktop()->screenGeometry();



		/*cout<<"W "<<screenResolution.width()<<"\n";
		cout<<"H "<<screenResolution.height()<<"\n";*/

		/*userInterface.setFixedWidth(screenResolution.width());
		userInterface.setFixedHeight(screenResolution.height());*/

		/*cout<<"Width "<<this->width()<<"\n";
		cout<<"Height "<<this->height()<<"\n";*/
		//cout<<"Hago set id "<<id<<"enabled "<<enabled<<" timed "<<timed<<"\n";

		//Si no está fullscreen hay que sacar el widht height del padre del padre del padre
		__size.setHeight(screenResolution.height()/4);
		__size.setWidth(screenResolution.width()/5);


	if (__vstate[st].type == GIF && __state == st)
	{
		
			__vstate[st].mov.stop();
			 disconnect( &__vstate[st].mov,SIGNAL(frameChanged(int)),this,SLOT(paintNewFrame(int)));
	}

	__vstate[st].file_name = f;

	

	TYPE tipo = filetype(__vstate[st].file_name);
	
	QByteArray ba = f.toLocal8Bit();
	char *sz = ba.data();
	std::string b=sz;

		
	
	if (tipo == GIF)
	{
	
		/*mov1.setFileName(__vstate[st].file_name);
		bool b = connect(&mov1,SIGNAL(frameChanged(int)),this,SLOT(paintNewFrame(int)));
		mov1.start();*/
		
		QList<QByteArray> lista = __vstate[st].mov.supportedFormats();
		std::cout << "Numero de formatos soportados" << lista.size() << std::endl;
		for (int i=0;i< lista.size();i++)
			std::cout << "format "<< (char*)lista[i].data() << std::endl;

		__vstate[st].mov.setFileName(__vstate[st].file_name);
		bool Valid = __vstate[st].mov.isValid();
		if (__state==st) 
		{
		
			
			bool b = connect(&__vstate[st].mov,SIGNAL(frameChanged(int)),this,SLOT(paintNewFrame(int)));
			
			__vstate[st].mov.start();
		}
	}
/*	else if (tipo == VIDEO )
	{
		if (__vstate[st].vid) delete __vstate[st].vid;
		__vstate[st].vid = new QVideo();

		__vstate[st].vid->addVideo(__vstate[st].file_name);
		if (__state==st) 
		{
			connect(__vstate[st].vid,SIGNAL(siNewFrame(int)),this,SLOT(paintNewFrame(int)));
			__vstate[st].vid->play();
		}

	}*/
	else if (tipo == IMAGE)
	{
		__vstate[st].img.load(__vstate[st].file_name);


	

		//@@-- mask to filter mouse clicks --@@//
		//if (st == M_OVER)
		//	this->setMask(__vstate[M_OVER].img.mask());
		/*if (!__vstate[st].img.isNull())
		{
			QImage image(__vstate[st].img.toImage());
			QImage *im = new QImage(image.width(), image.height(), QImage::Format_Grayscale8);
			for (int x = 0; x < image.width(); x++)
			{
				for (int y = 0; y < image.height(); y++)
				{
					QColor color(image.pixel(x, y));
					if (color.alpha() > 0)
						im->setPixel(x, y, 255);
					else
						im->setPixel(x, y, 0);
				}
			}
			__vstate[st].mask = QPixmap::fromImage(*im);
			__vstate[st].mask.save("e:\\im.jpg");
		}*/
	}
/*	else if (tipo==CAM)
	{
		if (__vstate[st].vid) delete __vstate[st].vid;
		__vstate[st].vid = new QVideo();

		__vstate[st].vid->addCamera(0);
		if (__state==st) 
		{
			connect(__vstate[st].vid,SIGNAL(siNewFrame(int)),this,SLOT(paintNewFrame(int)));
			__vstate[st].vid->play();
		}

	}*/

	__vstate[st].type = tipo;
	if (st==M_OUT)
	{
		__image=f.toUtf8().constData();
	}
	else if (st==M_PRESSED)
	{
		__onpress_image=f.toUtf8().constData();
	}
	else if (st==M_OVER)
	{
		__over_image=f.toUtf8().constData();
	}


	this->update();
}
void QAnimatedButton::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	int x = 0;
	int y = 0;
	int w = this->width();
	int h = this->height();

	while (!m_sync.add());
	if (__vstate[__state].file_name.isEmpty() && __vstate[__state].type!=CAM)   
	{
		m_sync.sub();

		/*A?ADIR TEXTO AL BOTON*/
		if (!__vstate[__state].text.isEmpty())
		{
			//__m1=0.03*w;
			//__m2=0.04*h;
			//__vstate[__state].font.setPointSize(0.25*w);
			//adjustTextSize(__vstate[__state].font);
			//fitSize(std::string(__vstate[__state].text.toLatin1()),__vstate[__state].font, QRect(0,0,this->width(),this->height()));
			painter.setFont(__vstate[__state].font);
			painter.drawText(__m1, __m2 - 7, w - __m1, h - __m2, Qt::TextWordWrap | Qt::AlignLeft | Qt::AlignTop, __vstate[__state].text);

		}
		return;
	}
   
	QPixmap *currentFrame=0;
	QPixmap pix;
	if (__vstate[__state].type==GIF)
		currentFrame = &(__vstate[__state].mov.currentPixmap());
	else if (__vstate[__state].type==IMAGE)
		currentFrame = &(__vstate[__state].img);
	/*else if (__vstate[__state].type==VIDEO || __vstate[__state].type==CAM)
	{
		// currentFrame = &(__vstate[__state].vid.currentPixmap());
		 // currentFrame =&(__vstate[__state].vid.currentPixmap());
		 pix=__vstate[__state].vid->currentPixmap();
		 currentFrame = &pix;
   	}*/
	QRect frameRect = QRect(0,0,0,0);
	if (currentFrame)
		frameRect = currentFrame->rect();
	else 
		std::cout << "Botón " << this->__name <<" no tiene currentframe "<< this->__image << std::endl;

	if (frameRect.x()==0 && frameRect.y()==0 && frameRect.width()==0 && frameRect.height()==0) 
	{
		std::cout << "Error : No hay currentFrame. Fuente " << __vstate[__state].type << std::endl;
		m_sync.sub();
		return;
	}

	/*if (__type.compare("INFO")==0) 
	{
		std::cout << "background info " << __background << std::endl;
		std::cout << "state " << __state << std::endl;
		std::cout << "type " << __vstate[__state].type << std::endl;
		std::cout << "Rect " << frameRect.x()<< "," << frameRect.y()<<","<< frameRect.width()<<"," << frameRect.height() << std::endl;
	}*/
   // Only redraw when the frame is in the invalidated area
  // frameRect.moveCenter(rect().center());
     
		//this->setMask( currentFrame->mask() );
		//  painter.drawPixmap(    frameRect.left(), frameRect.top(), *currentFrame);



	QRectF target(0,0,w,h);

	QRectF source(frameRect.left(), frameRect.top(),frameRect.width(),frameRect.height());

	//QBitmap b("../../../extras/rc/userview graphical elements/mascara.png");
	/*MASCARA*/
	/*
	QBitmap b(currentFrame->size());
	b.fill(Qt::white);
	QPainter paintermask(&b);
	paintermask.setBrush(QColor(0x00, 0x00, 0x00));
	paintermask.drawRoundedRect(QRectF(0.2*frameRect.width(),0.2*frameRect.height(),frameRect.width()-0.2*frameRect.width(),frameRect.height()-0.2*frameRect.height()),25.0,25.0);
	currentFrame->setMask(b);
	*/
	if (!__background.empty())
	{
		
		QImage backgroundImage(__background.c_str());
		//cout<<"Pinto "<<__background<<" en "<<__id<<"\n";
		painter.drawImage(target, backgroundImage);
	}
		//painter.drawPixmap(target,*currentFrame,source);

	if(!__type.compare("fullsized") || !__type.compare("display"))
	{
		
		QRectF target_(0,0,w,h);
		painter.drawPixmap(target_,*currentFrame,source);
	}
	else if (!__type.compare("centered"))
	{
		QRectF target_(w/10,h/10,w*7/10,h*7/10);
		painter.drawPixmap(target_,*currentFrame,source);
	}
	else if (!__type.compare("proportional") || !__type.compare("proportional&text"))
	{
		QPixmap paux = currentFrame->scaled(w, h, Qt::KeepAspectRatio);
		int dx = w*0.5 - paux.width()*0.5;
		int dy = h*0.5 - paux.height()*0.5;

		QRectF target_(dx, dy, paux.width(), paux.height());
		QRectF source(0, 0, paux.width(), paux.height());
		painter.drawPixmap(target_, paux, source);
	}
	else
	{
		QRectF target_(w/6,h/12,w*7/10,h*7/10);
		painter.drawPixmap(target_,*currentFrame,source);


	}
	//labelText->raise();
	/*cout<<"Raise "<<labelText->text().toUtf8().constData()<<"\n";
	cout<<"Raisev "<<labelText->isVisible()<<"\n";
	cout<<"posx "<<labelText->pos().x()<<"\n";
	cout<<"posy "<<labelText->pos().y()<<"\n";
	cout<<"width "<<labelText->width()<<"\n";
	cout<<"height "<<labelText->height()<<"\n";*/

	if (!__type.compare("display") || !__type.compare("proportional&text"))
	{		// display es para el teclado


	/*A?ADIR TEXTO AL BOTON*/
	if (!__vstate[__state].text.isEmpty())
	{
		//__m1=0.03*w;
		//__m2=0.04*h;
		//__vstate[__state].font.setPointSize(0.25*w);
		//adjustTextSize(__vstate[__state].font);
		//fitSize(std::string(__vstate[__state].text.toLatin1()),__vstate[__state].font, QRect(0,0,this->width(),this->height()));

		if (!__type.compare("proportional&text"))
		{
			QFontMetrics fm(__vstate[__state].font);
			int textWidthInPixels = fm.width(__vstate[__state].text);
			int textHeightInPixels = fm.height();


			//QPainterPath path;
			//float rounded_rect_margin = 1.3;
			//QPen pen(QBrush(QColor(255, 255, 255, 100)), 1);
			//painter->setPen(pen);
			//ainter->fillPath(path, QBrush(QColor(255, 255, 255, 100)));
			//painter->drawPath(path);
			//painter->setPen(Qt::black);
			QRect r = QRect(__m1, __m2, w - __m1, h - __m2);
			painter.drawText(r, Qt::TextWordWrap | Qt::AlignHCenter | Qt::AlignVCenter, __vstate[__state].text);
			
		}
		else
		{
			painter.setFont(__vstate[__state].font);
			painter.drawText(__m1, __m2 - 7, w - __m1, h - __m2, Qt::TextWordWrap | Qt::AlignLeft | Qt::AlignTop, __vstate[__state].text);
		}
	}
	}
	//cout<<"Texto "<<labelText->text().toUtf8().constData()<<"\n";

	/*A?ADIR MARCO AL BOTON*/
	  m_sync.sub();

}

void QAnimatedButton::paintNewFrame(int)
{
	std::cout << "repaint " << std::endl;
   repaint();
}
void QAnimatedButton::keyPressEvent( QKeyEvent *event )
{
	event->ignore();
}
QAnimatedButton::TYPE QAnimatedButton::filetype(QString t)
{
	TYPE tipo;
	int pos;
	if (t.isEmpty()) tipo=CAM;
	else
	{
		pos= t.lastIndexOf(".");
	
		QString ext = t.right(t.length()-pos - 1);
		ext=ext.toLower();
		(ext=="gif")? tipo=GIF : 
		(ext=="avi" || ext=="wmv" || ext =="mpg" || ext =="mp4" || ext=="mpeg" || ext=="avs" || ext =="asf")? tipo = VIDEO:
		(ext=="bmp" || ext=="dib" || ext =="jpg" || ext =="jpe" || ext=="jpeg" || ext=="png" || ext =="pbm" || ext=="pgm" || ext == "ppm" || ext=="sr" || ext=="ras" || ext=="tiff" || ext=="tif")? tipo=IMAGE : tipo=NONE;
	}
	return tipo;
}
QString QAnimatedButton::state( int st)
{
	QString text;
	switch (st)
	{
		case M_OUT :
			text = "M_OUT";
			break;
		case M_OVER:
			text = "M_OVER";
			break;
		case M_PRESSED:
			text = "M_PRESSED";
			break;
	}
	return text;
}

QString QAnimatedButton::type( int tp)
{
	
	QString text;
	switch (tp)
	{
		case GIF :
			text = "GIF";
			break;
		case IMAGE:
			text = "IMAGE";
			break;
		case VIDEO:
			text = "VIDEO";
			break;
		case CAM:
			text = "CAM";
			break;
		case NONE:
			text = "NONE";
			break;
	}
	return text;

}
void QAnimatedButton::enterEvent(QEvent *event)
{
	//if (__type.compare("INFO")==0) return;
		

	m_active = true;

	if(!enabled)
	{
		//cout<<"Deshabilitado\n";
		return;
	}

	
	__background=__pushedBackground;
	if(timed)
	{
		if(!timer && !bar->isVisible())
		{
			timer = new QTimer();
			// Acabo de entrar en el botón
			//timer->setSingleShot(true);
			connect(timer, SIGNAL(timeout()), this, SLOT(visibleBar()) );
			timer->start(timeBarDelay);
		}
	}
	//bar->hide();
}

void QAnimatedButton::eliminarTimers ( )
{
	if(timer)
	{	
		std::cout << "Eliminar timer \n" << std::endl;		
		timer->stop();
		disconnect(timer, SIGNAL(timeout()), this, SLOT(visibleBar()));
		delete timer;
		timer = 0;
	}
	if (barTimer->isActive())
	{
		barTimer->stop();
		bar->hide();
		bar->setValue(0);
	}
	

}

void QAnimatedButton::leaveEvent ( QEvent * event )
{
	//if (__type.compare("INFO")==0) return;

	m_active = false;
	if(timed) eliminarTimers();
	//cout<<"Dejo el boton "<<__id<<"\n";
	__background=__tempBackground;
	update();
	if(!enabled)
	{
		return;
	}
	if(timed)
	{
		eliminarTimers();
	}
	if (__state != M_OUT )
	{
		if (__vstate[__state].type == GIF)
		{
			
			__vstate[__state].mov.stop();
			 disconnect( &__vstate[__state].mov,SIGNAL(frameChanged(int)),this,SLOT(paintNewFrame(int)));
		}
	/*	if (__vstate[__state].type == VIDEO || __vstate[__state].type == CAM)
		{
			__vstate[__state].vid->stop();
			 disconnect( __vstate[__state].vid,SIGNAL(siNewFrame(int)),this,SLOT(paintNewFrame(int)));
		}
	*/	
		__state=M_OUT;

		if (__vstate[__state].type == GIF)
		{
			connect(&__vstate[__state].mov,SIGNAL(frameChanged(int)),this,SLOT(paintNewFrame(int)));
			__vstate[__state].mov.start();
		
		}
/*		if (__vstate[__state].type == VIDEO || __vstate[__state].type == CAM)
		{
			connect(__vstate[__state].vid,SIGNAL(siNewFrame(int)),this,SLOT(paintNewFrame(int)));
			__vstate[__state].vid->play();
		
		}
*/	}


	update();
	//cout<<"Salgo\n";

}
void QAnimatedButton::mouseMoveEvent ( QMouseEvent * event )
{
	//if (__type.compare("INFO")==0) return;

	if (!m_active) return;

	if(!enabled)
	{
		return;
	}

	//cout<<"Movimiento "<<this->__id<<"\n";
	if(enabled==true)
	{
		__background=__pushedBackground;

	
		if (__state != M_OVER  )
		{
			if (__vstate[__state].type == GIF)
			{
			
				__vstate[__state].mov.stop();
				 disconnect( &__vstate[__state].mov,SIGNAL(frameChanged(int)),this,SLOT(paintNewFrame(int)));
			}	
	/*		if (__vstate[__state].type == VIDEO || __vstate[__state].type == CAM)
			{
				__vstate[__state].vid->stop();
				 disconnect( __vstate[__state].vid,SIGNAL(siNewFrame(int)),this,SLOT(paintNewFrame(int)));
			}
	*/	
			__state=M_OVER;

		//	printf("st:::%s type:::%s\n", state(__state).toStdString().c_str(),type(__vstate[__state].type).toStdString().c_str());
			if (__vstate[__state].type == GIF)
			{
				connect(&__vstate[__state].mov,SIGNAL(frameChanged(int)),this,SLOT(paintNewFrame(int)));
				__vstate[__state].mov.start();
			
			}
	/*		if (__vstate[__state].type == VIDEO || __vstate[__state].type == CAM)
			{
				connect(__vstate[__state].vid,SIGNAL(siNewFrame(int)),this,SLOT(paintNewFrame(int)));
				__vstate[__state].vid->play();
			
			}
	*/		update();
		}
	}
	//cout<<"Salgo movimiento\n";

}

void QAnimatedButton::press( QMouseEvent *event)
{
	//if (__type.compare("INFO")==0) return;

	if (!m_active) return;
/*int evento =event->type();
	cout <<"Tipo de evento "<<evento<<" \n";*/
	if (__state != M_PRESSED )
	{
		if ( __vstate[__state].type == GIF)
		{
			
			__vstate[__state].mov.stop();
			 disconnect( &__vstate[__state].mov,SIGNAL(frameChanged(int)),this,SLOT(paintNewFrame(int)));
		}
/*		if ( __vstate[__state].type == VIDEO || __vstate[__state].type == CAM)
		{
			__vstate[__state].vid->stop();
			 disconnect( __vstate[__state].vid,SIGNAL(siNewFrame(int)),this,SLOT(paintNewFrame(int)));
		}
*/

		__state=M_PRESSED;
		if (__vstate[__state].type == GIF)
		{
			connect(&__vstate[__state].mov,SIGNAL(frameChanged(int)),this,SLOT(paintNewFrame(int)));
			__vstate[__state].mov.start();
		
		}
/*		if (__vstate[__state].type == VIDEO || __vstate[__state].type == CAM)
		{
			connect(__vstate[__state].vid,SIGNAL(siNewFrame(int)),this,SLOT(paintNewFrame(int)));
			__vstate[__state].vid->play();
		
		}

*/
	}
	
	//update();
	
	//std::cout << "  Press " << __id << "  " << __text << std::endl;
	emit siPress(event,__image,__id,__text);
}
void QAnimatedButton::mousePressEvent ( QMouseEvent * event )
{
	//if (__type.compare("INFO")==0) return;

	if(timed)
		eliminarTimers();

	if (!m_active) return;
	//ºcout<<"MousePressEvent\n";
	if(!enabled)
	{
		return;
	}
	
	press(event);
	//cout <<"Press\n";
}
void QAnimatedButton::mouseReleaseEvent ( QMouseEvent * event )
{
	//if (__type.compare("INFO")==0) return;

	if (!m_active) return;
	//cout<<"Release\n";
	if(!enabled)
	{
		return;
	}
	if (__state != M_OVER )
	{
		if (__vstate[__state].type == GIF)
		{
			
			__vstate[__state].mov.stop();
			 disconnect( &__vstate[__state].mov,SIGNAL(frameChanged(int)),this,SLOT(paintNewFrame(int)));
		}
/*		if (__vstate[__state].type == VIDEO || __vstate[__state].type == CAM)
		{
			__vstate[__state].vid->stop();
			 disconnect( __vstate[__state].vid,SIGNAL(siNewFrame(int)),this,SLOT(paintNewFrame(int)));
		}
*/		__state=M_OVER;
		if (__vstate[__state].type == GIF)
		{
			connect(&__vstate[__state].mov,SIGNAL(frameChanged(int)),this,SLOT(paintNewFrame(int)));
			__vstate[__state].mov.start();
		
		}
/*		if (__vstate[__state].type == VIDEO || __vstate[__state].type == CAM)
		{
			connect(__vstate[__state].vid,SIGNAL(siNewFrame(int)),this,SLOT(paintNewFrame(int)));
			__vstate[__state].vid->play();
		
		}
*/	}
	update();

	emit siRelease(event,__id);

}
void QAnimatedButton::timedValue(int value)
{
	if(!enabled)
	{
		return;
	}
	//cout <<"Valor "<<value<<"\n";
	//QMouseEvent *event=NULL;
	//if (value==bar->width()-1)
	//{
	//	emit this->mousePressEvent(event);
	//	bar->hide();
	//	bar->setValue(0);
	//}

}
void QAnimatedButton::setEnabled()
{
	enabled=true;
}
void QAnimatedButton::setDisabled()
{
	enabled=false;
}
void QAnimatedButton::setName(QString name)
{
	__name=name.toUtf8().constData();
}
void QAnimatedButton::setType(QString type)
{
	QByteArray ba = type.toLocal8Bit();
	char *sz = ba.data();
	__type = sz;
	//__type=type.toUtf8().constData();

}
void QAnimatedButton::setText(QString text)
{
	__text=text.toUtf8().constData();
	
	
}
void QAnimatedButton::setPushedBackground(std::string background)
{
	__pushedBackground=background;
}
void QAnimatedButton::loadBackground(std::string background)
{
	__background = background;
	__tempBackground=background;
}
void QAnimatedButton::visibleBar()
{
	if (timer)
	{
		std::cout << "Timer eliminado" << std::endl;
		timer->stop();
		disconnect(timer, SIGNAL(timeout()), this, SLOT(visibleBar()));
		delete timer;
		timer=0;
	
		//cout<<"VisibleBar\n";
		if(timed)
		{
			if (!bar->isVisible())
			{
			
				//bar->setFixedHeight(__size.height()/6);
				//bar->setFixedWidth(__size.width()-__size.width()/10);
				if (__type.compare("fullscreen"))
				{
						bar->setFixedHeight(this->height()*0.15);
						bar->setFixedWidth(this->width()*0.8);
						bar->setGeometry(this->width()*0.1,this->height()*0.8,bar->width(),bar->height());
				}
				else
				{
						bar->setFixedHeight(__size.height()*0.15);
						bar->setFixedWidth(__size.width()*0.8);
					//bar->move(__size.width()/20,__size.height()-__size.height()/5);
						bar->setGeometry(__size.width()*0.1,__size.height()*0.8,bar->width(),bar->height());
				}

				bar->setStyleSheet(" QProgressBar { border: 1px solid grey; border-radius:1px ;  background: transparent; } QProgressBar::chunk {   background-color: #05B8CC; width: 1px;  } ");
				
				double time = (1.0/99.0) * (double)timeBarDuration;
				//if (time<2.0) std::cout << "Timer de actualización de la barra de progreso (menor que 2 ms) "<< time << "ms" << std::endl;
				//else std::cout << "starting Timer " << time << " ms" << std::endl;
				barTimer->start((int)time);
				
				//http://doc.qt.nokia.com/4.7-snapshot/stylesheet-examples.html#customizing-qprogressbar
				//http://doc.qt.nokia.com/4.7-snapshot/stylesheet-customizing.html#box-model
				//http://doc.qt.nokia.com/4.7-snapshot/stylesheet-reference.html

				//cout<<"stylesheet "<<bar->styleSheet().toUtf8().constData()<<"\n";
				bar->lower();
				bar->show();
				/*bar->setFont(__vstate[__state].font);
				bar->setTextDirection(Qt::AlignBottom|Qt::AlignCenter);
				bar->setFormat(__vstate[__state].text );
				bar->setTextVisible(true);*/

			}
		}
	}
}
void QAnimatedButton::setTimed(bool time)
{
	timed=time;
}
void QAnimatedButton::setTimeBarDuration(int duration)
{
	timeBarDuration=duration;
}
void QAnimatedButton::setTimeBarDelay(int delay)
{
	timeBarDelay=delay;
}

void QAnimatedButton::updateMask()
{

	
	if (!__vstate[M_OVER].img.isNull())
	{
		this->setMask(__vstate[M_OVER].img.mask());
	}

}

void QAnimatedButton::resizeEvent(QResizeEvent * /* event */)
{

	if (!__type.compare("proportional") || !__type.compare("proportional&text"))
	{

		QImage bmptarget(width(), height(),QImage::Format_Grayscale8);
		bmptarget.fill(Qt::white);
		
		QPainter painter_two(&bmptarget);

		int side = qMin(width(), height());

		QBitmap bmp = __vstate[M_OVER].img.mask();
		bmp = bmp.scaled(this->width(), this->height(), Qt::KeepAspectRatio);
		
		int dx = width()*0.5 - bmp.width()*0.5;
		int dy = height()*0.5 - bmp.height()*0.5;

		QRectF target_(dx, dy, bmp.width(), bmp.height());
		QRectF source(0, 0, bmp.width(), bmp.height());
		painter_two.drawPixmap(target_, bmp, source);

		
		setMask(QBitmap::fromImage(bmptarget));
	}
	else if (!__type.compare("fullsized") || !__type.compare("display"))
	{
		QBitmap bmp = __vstate[M_OVER].img.mask();
		bmp = bmp.scaled(this->width(), this->height());
		setMask(bmp);

	}
}