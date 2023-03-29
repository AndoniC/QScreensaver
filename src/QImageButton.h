#pragma once
#ifndef QANIMATEDCLIP_H
#define QANIMATEDCLIP_H

#include <QWidget>
#include <QString>
#include <QMovie>
#include <QKeyEvent>

class QImageButton : public QWidget
{
	Q_OBJECT

public:
	QImageButton(QWidget *parent = 0);
	QImageButton(std::string _image,QWidget *parent = 0);
	~QImageButton();

	virtual void paintEvent(QPaintEvent *event);
	void keyPressEvent( QKeyEvent *event );

	QSize size() {return m_size;};
	QString clipName() {return m_clip_name;};
	void setAlpha(float _value) {
		this->setWindowOpacity(_value);
	}
	void setPos(int _x, int _y);
	void setSize(int _w, int _h) {m_size.setWidth(_w);m_size.setHeight(_h);};
	void setImage(std::string _image) 
	{
		m_image_file = _image; 
		m_image.load(m_image_file.c_str());

		QRect frameRect = m_image.rect();
		resize(frameRect.width(), frameRect.height());
		m_size.setHeight(frameRect.height());
		m_size.setWidth(frameRect.width());

	

	}
	void setImage(std::string _image, int _w, int _h)
	{
		m_image_file = _image;
		m_image.load(m_image_file.c_str());

		QRect frameRect = m_image.rect();
		resize(frameRect.width(), frameRect.height());
		m_size.setHeight(frameRect.height());
		m_size.setWidth(frameRect.width());
		m_size = QSize(_w, _h);
	}

	bool event(QEvent *event)
	{
	
		int etype = event->type();
		

		if (event->type() == QEvent::MouseButtonPress)
		{
			QMouseEvent* e = static_cast<QMouseEvent*>(event);
			m_mouse_pressed = true;
			emit(siPressed());
		}
		else if (event->type() == QEvent::MouseButtonRelease)
		{
			QMouseEvent* e = static_cast<QMouseEvent*>(event);
			m_mouse_pressed = false;
			emit(siReleased());
		}

		return QWidget::event(event);
	}

	std::string getImage() { return m_image_file; }

private:
	 QSize m_size;
	 QPixmap m_image;
	 QString m_clip_name;

	 std::string m_image_file;
	 bool m_mouse_pressed;


signals:
	void siPressed();
	void siReleased();

};
#endif