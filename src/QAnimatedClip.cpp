//#include "math.h"
#include "qanimatedclip.h"
#include <QPainter>
#include <QPaintEvent>
#include <QBitmap>


QAnimatedClip::QAnimatedClip(QWidget *parent)
	: QWidget(parent)
{
	this->setWindowFlags(Qt::Window |  Qt::FramelessWindowHint | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint );//| Qt::WindowTitleHint);
	this->setWindowOpacity( 0.5 );
	this->setAttribute( Qt::WA_TransparentForMouseEvents );
	//cursorWidget.setEnabled( false );
	this->setFocusPolicy( Qt::NoFocus );
	this->show();

}
QAnimatedClip::QAnimatedClip(QString fgif,QWidget *parent): QWidget(parent)
{
	m_movie.setFileName(fgif);
	m_clip_name= fgif;	
//	QSize s = __movie.pixma ().size();	
//	this->resize(s.width(),s.height());
	this->setWindowFlags(Qt::Window |  Qt::FramelessWindowHint | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint );//| Qt::WindowTitleHint);
	this->setWindowOpacity( 0.5 );
	this->setAttribute( Qt::WA_TransparentForMouseEvents );
	//cursorWidget.setEnabled( false );
	this->setFocusPolicy( Qt::NoFocus );

	//if (parent!=NULL) this->setParent(parent);

	connect(
      &m_movie,
      SIGNAL(frameChanged(int)),
      this,
      SLOT(paintNewFrame(int)));
	
   m_movie.start();
   
}

QAnimatedClip::~QAnimatedClip()
{


}
void QAnimatedClip::openFile(QString f)
{
	if (!m_clip_name.isEmpty()) 
	{
		m_movie.stop();
	disconnect(
      &m_movie,
      SIGNAL(frameChanged(int)),
      this,
      SLOT(paintNewFrame(int)));
	}
	m_movie.setFileName(f);
	m_movie.start();
	QPixmap currentFrame = m_movie.currentPixmap();
    QRect frameRect = currentFrame.rect();
	resize(frameRect.width(),frameRect.height());
	m_size.setHeight(frameRect.height());
	m_size.setWidth(frameRect.width());
	m_clip_name= f;
	 connect(
      &m_movie,
      SIGNAL(frameChanged(int)),
      this,
      SLOT(paintNewFrame(int)));

	this->show();
	
}
void QAnimatedClip::paintEvent(QPaintEvent *event)
{
	if (m_clip_name.isEmpty()) return;
   //  First we extract the current frame
   QPixmap currentFrame = m_movie.currentPixmap();
   
   QPixmap scaled_pixmap = currentFrame.scaled(m_size);

   QRect frameRect = scaled_pixmap.rect();
	
	

   // Only redraw when the frame is in the invalidated area
  // frameRect.moveCenter(rect().center());
   if (frameRect.intersects(event->rect()))
   {
      QPainter painter(this);
	  this->setMask( scaled_pixmap.mask() );

      painter.drawPixmap(
         frameRect.left(), 
         frameRect.top(), 
         scaled_pixmap);
   }
}

void QAnimatedClip::paintNewFrame(int)
{
   this->update();
   this->repaint();
}
void QAnimatedClip::keyPressEvent( QKeyEvent *event )
{
	event->ignore();
}

void QAnimatedClip::setPos(int _x, int _y)
{
	this->setGeometry(_x,_y,m_size.width(),m_size.height());
}