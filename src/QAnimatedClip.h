#pragma once
#ifndef QANIMATEDCLIP_H
#define QANIMATEDCLIP_H

#include <QWidget>
#include <QString>
#include <QMovie>
#include <QKeyEvent>

class QAnimatedClip : public QWidget
{
	Q_OBJECT

public:
	QAnimatedClip(QWidget *parent = 0);
	QAnimatedClip(QString fgif,QWidget *parent = 0);
	~QAnimatedClip();

	virtual void paintEvent(QPaintEvent *event);
	void keyPressEvent( QKeyEvent *event );

	void openFile(QString f);
	QSize size() {return m_size;};
	QString clipName() {return m_clip_name;};
	void setAlpha(float _value);
	void setPos(int _x, int _y);
	void setSize(int _w, int _h) {m_size.setWidth(_w);m_size.setHeight(_h);};

private:
	 QSize m_size;
	 QMovie m_movie;
	 QString m_clip_name;


private slots:
   void paintNewFrame(int);

};
#endif