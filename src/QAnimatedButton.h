#pragma once
#ifndef QANIMATEDBUTTON_H
#define QANIMATEDBUTTON_H

#include <QWidget>
#include <QString>
#include <QMovie>
#include <QImage>
#include <QKeyEvent>
#include <QMutex>
#include <QTimer>
#include <QProgressBar>
#include <QMetaEnum>
#include <QHash>
#include <QLabel>
 #include <QTextCodec>
 #include <QMutexLocker>
#include <QMutex>

class syncronization
{ private:
	

public:
	syncronization();
	~syncronization();

	bool add(); 
	void sub();
	void setStop(int val=1);
	bool wait();
	
};

class QAnimatedButton : public QWidget
{
	Q_OBJECT

public:
	typedef enum STATE { M_OUT = 0, M_OVER = 1, M_PRESSED = 2, M_DISABLED = 3 };
	typedef enum TYPE { GIF = 0, IMAGE = 1, VIDEO = 2, CAM = 3, NONE = 4 };

	typedef struct _INFO
	{
		//-- image and background for each state --//
		std::vector<std::string> img_state;
		std::vector<std::string> img_background_state;
		std::vector < std::string > text;


		std::string id;
		std::string name;
		std::string type;
		bool active;
		int level;

		int parent;
		int self_index;

	}INFO;

	QAnimatedButton(QWidget *parent = 0);
	QAnimatedButton(std::string id,QWidget *parent=0);
	QAnimatedButton(std::string id,QString fover, QString fout, QString fpush,QWidget *parent=0);
	~QAnimatedButton();

	void create(std::string id,std::string fout=std::string(""), std::string fover=std::string(""), std::string fpush=std::string(""));
	void setID(std::string id);
	/*{__id = id; 	if (timed && enabled)
	{
		bar=new QProgressBar(this);
		bar->resize(200,10);
		bar->setOrientation(Qt::Horizontal);
		bar->setRange(0,99);
		bar->setValue(0);
		barTimer = new QTimer(this);
		connect(barTimer, SIGNAL(timeout()), this, SLOT(plusOne()));
		connect(bar, SIGNAL(valueChanged(int)), this, SLOT(timedValue(int)));
		bar->hide();
		bar->setTextVisible(false);
		timer = new QTimer(this);
		cout<<"Creo el timer "<<this->__id<<"\n";
	}};*/
	std::string getID() {return __id;};

	void 	enterEvent ( QEvent * event );
	void 	leaveEvent ( QEvent * event );
	void 	mouseMoveEvent ( QMouseEvent * event );
	void 	mousePressEvent ( QMouseEvent * event );
	void 	mouseReleaseEvent ( QMouseEvent * event );

	

	virtual void paintEvent(QPaintEvent *event);
	void keyPressEvent( QKeyEvent *event );
	void resizeEvent(QResizeEvent * /* event */);

	void load(QString f,STATE st);
	//QString getImage(STATE st);
	void addText(QString text,QFont font,STATE st);
	void addText(std::string text,QFont font,STATE st);
	QString getText(STATE st);
	void adjustTextSize(QFont &f);
	int fitSize(std::string text,QFont &f, QRect size);
	void press( QMouseEvent *event);

	QSize size() {return __size;};
	
	TYPE filetype(QString t);

	void setEnabled();
	void setDisabled();
	void setName(QString name);
	void setText(QString text);
	void setType(QString text);

	void initializeVideoMan(const std::string &path_fichero, const std::string &_friendlyName, const std::string &_devicePath, bool showConfigDlg,STATE st);
	void loadBackground(std::string background);
	void setPushedBackground(std::string background);
	void setTimed(bool timed);
	void setTimeBarDuration(int duration);
	void setTimeBarDelay(int delay);

	void eliminarTimers ( );
	void updateMask();

	enum INPUT_TYPE
	{
		CAMERA=0,IMAGE_FILE=1,VIDEO_FILE=2
	};

	struct T_STATE_ELEMENT
	{
		QMovie	mov;
		QPixmap img;
		QPixmap mask;
		//QVideo	*vid;
		TYPE	type;
		QString file_name;
		QString	text;
		QFont	font;
		T_STATE_ELEMENT() {type=NONE;text.clear();/*vid=NULL;*/}
	};
	
	INFO m_info;

	syncronization m_sync;
	//TODO esto es temporal, tengo que poner una función que devuelva este elemento
	std::string __image;
	std::string __onpress_image;
	std::string __over_image;
	std::string __id;
	std::string __name;
	std::string __text;
	std::string __background;
	std::string __pushedBackground;
	std::string __type;
	std::string __tempBackground;
	bool m_active;

private:

	QString state( int st);
	QString type( int tp);
	bool enabled;
	bool timed;
	int timeBarDuration;
	int timeBarDelay;
	QLabel *labelText;

	

	QSize __size;
	int __m1,__m2; // margenes para el texto

	
	T_STATE_ELEMENT __vstate[3];
	STATE __state;

	QMovie	mov1;

	QProgressBar *bar;
	QTimer *barTimer;
	QTimer *timer;


public slots:
	void plusOne();
private slots:
   void paintNewFrame(int);
   void timedValue(int value);
   void visibleBar();

signals:
	void siRelease(QMouseEvent * event,std::string ID);
	void siPress(QMouseEvent * event,std::string ID,std::string NAME,std::string TEXT);
	
};
#endif
