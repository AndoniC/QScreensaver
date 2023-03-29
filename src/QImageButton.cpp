//#include "math.h"
#include "QImageButton.h"
#include <QPainter>
#include <QPaintEvent>
#include <QBitmap>


QImageButton::QImageButton(QWidget *parent)
	: QWidget(parent)
{
	this->setWindowFlags(Qt::Window |  Qt::FramelessWindowHint | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint );//| Qt::WindowTitleHint);
	this->setWindowOpacity( 0.5 );
	//this->setAttribute( Qt::WA_TransparentForMouseEvents );
	//cursorWidget.setEnabled( false );
	//this->setFocusPolicy( Qt::NoFocus );
	this->show();

}
QImageButton::QImageButton(std::string _image,QWidget *parent): QWidget(parent)
{
	setImage(_image);
	m_mouse_pressed = false;
	//this->setWindowFlags(Qt::Window);// | Qt::FramelessWindowHint | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint );//| Qt::WindowTitleHint);
	//this->setWindowOpacity( 0.5 );
	//this->setParent(parent);
	//this->setAttribute( Qt::WA_TransparentForMouseEvents );
	//cursorWidget.setEnabled( false );
	//this->setFocusPolicy( Qt::NoFocus );
	this->setMouseTracking(true);
	
 
}

QImageButton::~QImageButton()
{


}
void QImageButton::paintEvent(QPaintEvent *event)
{
	if (m_image_file.empty()) return;
   
   QPixmap scaled_pixmap = m_image.scaled(m_size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

   QRect frameRect = scaled_pixmap.rect();
	
	

   // Only redraw when the frame is in the invalidated area
  // frameRect.moveCenter(rect().center());
   if (frameRect.intersects(event->rect()))
   {
      QPainter painter(this);
	  this->setMask( scaled_pixmap.mask() );
	  painter.setRenderHint(QPainter::SmoothPixmapTransform);
     
	  if (this->underMouse())
	  {
		  QColor color;
		  QPixmap input = scaled_pixmap;
		  QImage image(input.size(), QImage::Format_ARGB32_Premultiplied);
		  if (m_mouse_pressed)
		  {
			 
			 
			  color = Qt::yellow;
			  image.fill(color);
			  QPainter p(&image);
			  p.setOpacity(0.5);
			  p.drawPixmap(0, 0, input);
			  p.end();
		  }
		  else
		  {
			 
			  color = Qt::red;
			  image.fill(color);
			  QPainter p(&image);
			  p.setOpacity(0.5);
			  p.drawPixmap(0, 0, input);
			  p.end();
			
		  }
		  QPixmap output = QPixmap::fromImage(image);
		  output.setMask(input.mask());
		  painter.drawPixmap(
			  frameRect.left(),
			  frameRect.top(),
			  output);
	  }
	  else
	  {
		  painter.drawPixmap(
			  frameRect.left(),
			  frameRect.top(),
			  scaled_pixmap);
	  }
	
   }

   
}
void QImageButton::keyPressEvent( QKeyEvent *event )
{
	event->ignore();
}

void QImageButton::setPos(int _x, int _y)
{
	this->setGeometry(_x,_y,m_size.width(),m_size.height());
}
