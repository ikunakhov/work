#ifndef _ONE_CONVERTER_H
#define _ONE_CONVERTER_H


#include <QtGui>
#include <QtWidgets/QApplication>
#include <QtGui>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QString>
#include <ui_one_converter.h>
#include <SDL.h>
#include <YUV.h>
#include <Audio.h>
#include <Video.h>
#include <Player.h>
#include <Project_init.h>
#include <qtooltip.h>

extern "C" {
#include <libavutil/mem.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/mathematics.h>
}


class Ui_ONE_conv : public QMainWindow, public Ui::ONE_converterClass {
	Q_OBJECT
public:
	Ui_ONE_conv(QMainWindow *parent = 0) :QMainWindow(parent)
	{
		setupUi(this);
        Loading_widget->hide();
        progressBar->setMaximum(100);
        progressBar->setMinimum(0);
        Loading_widget_2->hide();
        connect(pushButton_17, SIGNAL(clicked(bool)), this, SLOT(audio_convert()));
		connect(pushButton_2,  SIGNAL(clicked(bool)), this, SLOT(init()));
		connect(pushButton,    SIGNAL(clicked(bool)), this, SLOT(calc()));
		connect(toolButton,    SIGNAL(clicked(bool)), this, SLOT(open_dialog()));
        connect(toolButton_20, SIGNAL(clicked(bool)), this, SLOT(find_audio_folder()));
		connect(toolButton_2,  SIGNAL(clicked(bool)), this, SLOT(open_dialog_2()));
		connect(toolButton_3,  SIGNAL(clicked(bool)), this, SLOT(find_video()));
		connect(pushButton_6,  SIGNAL(clicked(bool)), this, SLOT(start_wind_video()));
        connect(pushButton_5,  SIGNAL(clicked(bool)), this, SLOT(player_run()));
        connect(toolButton_21, SIGNAL(clicked(bool)), this, SLOT(find_audio()));
        connect(pushButton_18, SIGNAL(clicked(bool)), this, SLOT(start_play_audio()));
        connect(toolButton_4,  SIGNAL(clicked(bool)), this, SLOT(find_player_info()));
        connect(radioButton,   SIGNAL(clicked(bool)), this, SLOT(pix_size_rb()));
        connect(radioButton_2, SIGNAL(clicked(bool)), this, SLOT(byte_size_rb()));
        connect(comboBox,      SIGNAL(currentIndexChanged(const QString &)), this, SLOT(inf_format_image(const QString &)));
        connect(comboBox_15,   SIGNAL(currentIndexChanged(const QString &)), this, SLOT(inf_format_audio(const QString &)));
        connect(comboBox_3,    SIGNAL(currentIndexChanged(const QString &)), this, SLOT(inf_codec_video(const QString &)));

	}
    int byte_size_img;
    Audio_c_err transcoding_audio(QString in_file, QString ou_file, Transcoding_context input_context);



public slots:
    void inf_codec_video(const QString &);
   // void inf_codec_audio(const QString &);
    void inf_format_video(const QString &);
    void inf_format_image(const QString &);
    void inf_format_audio(const QString &);
    void audio_convert();
    void find_audio_folder();
    void find_audio();
    void find_video();
    void find_player_info();
	void start_wind_video();
    void player_run();
    void start_play_audio();
	void open_dialog();
	void open_dialog_2();
    void cb4_on();
    void pix_size_rb();
    void byte_size_rb();
	void calc();
	void init();
};

class Win : public QWidget
{
	Q_OBJECT
protected:
	QFrame *frame;

	QLabel *Lbl1;
	QLabel *Lbl2;
	QLabel *lblName;
	QLabel *Lblselect;
	QLabel *Lblscled1;
	QLabel *Lblscled2;
	QLabel *LblDpi;

	QCheckBox *prtscreen;
	QCheckBox *invertpix;
	QCheckBox *mirror;
	QCheckBox *rgbswap;

	QComboBox *selectformat;
	QLineEdit *fileline1;
	QLineEdit *fileline2;
	QLineEdit *scled1;
	QLineEdit *scled2;
	QLineEdit *DpiEdit;

	QLineEdit *nameEdit;
	QPushButton *Calculate;
	QPushButton *lstsession;
public:
	Win(QWidget *parent = 0);
	public slots:
	void calc();
	void init();
};

#endif
