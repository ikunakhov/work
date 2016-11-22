#include "one_converter.h"
#include <ui_one_converter.h>
#include <stdio.h>


void Ui_ONE_conv::calc()
{
	QString Filepth2;
	QString Fname;
	QString Filepth1;
	QString formatconv;
    int width, height;
    QImage *im1;

	Filepth1 = lineEdit->text();
	formatconv = comboBox->currentText();

	im1 =  new QImage(Filepth1);

	//fix his
	if (checkBox_5->isChecked() == true)
	{
 		QClipboard *clipboard = QApplication::clipboard();
		*im1 = clipboard->image(QClipboard::Clipboard);
	}
	//
	if (checkBox_4->isChecked() == true)
		im1->invertPixels();
	if (checkBox_3->isChecked() == true)
		*im1 = im1->mirrored(true);
	if (checkBox_2->isChecked() == true)
		*im1 = im1->rgbSwapped();
    if (radioButton->isChecked() == true) 
    {
        width = lineEdit_23->text().toInt();
        height = lineEdit_22->text().toInt();
    }
    if (radioButton_2->isChecked() == true)
    {
        width = lineEdit_6->text().toInt();
        height = lineEdit_3->text().toInt();
        int byte_size_new = lineEdit_24->text().toInt();
        if (comboBox_4->currentText() == "Kb") byte_size_new *= 1000;
        if (comboBox_4->currentText() == "Mb") byte_size_new *= 1000000;

        int byte_size_old = byte_size_img;

        double ratio = (double)byte_size_new / (double)byte_size_old;
        double coef_ratio = sqrt(ratio);
        width = (int)(coef_ratio*width);
        height = (int)(coef_ratio*height);
    }

    *im1 = im1->scaled(height, width);
	Filepth2 = lineEdit_2->text();
	QString tmp = Filepth2;
	Fname = lineEdit_4->text();
	Filepth2.append("/"); Filepth2.append(Fname); Filepth2.append("."); Filepth2.append(formatconv);
	if (Fname.isEmpty())
	{
		QDateTime time = QDateTime::currentDateTime();
		Filepth2 = time.toString();
		Filepth2.append("d");
		Filepth2.append(".");
		Filepth2.append(formatconv);
	}

	if (formatconv == "bmp")
		im1->save(Filepth2, "BMP");
	if (formatconv == "jpg")
		im1->save(Filepth2, "JPG");
	if (formatconv == "jpeg")
		im1->save(Filepth2, "JPEG");
	if (formatconv == "png")
		im1->save(Filepth2, "PNG");
	if (formatconv == "ppm")
		im1->save(Filepth2, "PPM");
	if (formatconv == "xbm")
		im1->save(Filepth2, "XBM");
	if (formatconv == "xpm")
		im1->save(Filepth2, "XPM");   

}

void Ui_ONE_conv::open_dialog()
{
	QString lFileName = QFileDialog::getOpenFileName(this, " open file...", "C:/work/windows/Qt_pojects/ONE_converter/Tests", " All files ( *.* ) ");
	if (lFileName.isEmpty())
	{
		return;
	}
	lineEdit->setText(lFileName);
	QFile lFile(lFileName);
	QImage pixmap1 = QImage(lFileName);
    double num_byte = pixmap1.byteCount();
	int width = pixmap1.width();
	int height = pixmap1.height();
	pixmap1 = pixmap1.scaled(611, 461);
	label_8->setPixmap(QPixmap::fromImage(pixmap1));
    
	QString w = QString::number(width); QString h = QString::number(height);
	lineEdit_3->setText(w);
	lineEdit_6->setText(h);

    QString postfix = " b";
    byte_size_img = num_byte;
    if (((int)num_byte/1000))
    {
        num_byte /= 1000;
        postfix = " Kb";
        if (((int)num_byte / 1000))
        {
            num_byte /= 1000;
            postfix = " Mb";
        }
    }
    lineEdit_21->setText(QString::number(num_byte).append(postfix));

	if (lFile.open(QIODevice::ReadOnly)){}
	else
		QMessageBox::warning(this, " Error ", QString(" Could not open File %1 for reading " ).arg ( lFile.fileName() ), QMessageBox::Ok);
}

void Ui_ONE_conv::open_dialog_2()
{
	QString lFileName = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "C:/work/windows/Qt_pojects/ONE_converter/Tests", QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
	if (lFileName.isEmpty())
	{
		return;
	}
	lineEdit_2->setText(lFileName);
	QFile lFile(lFileName);
}
void  Ui_ONE_conv::cb4_on()
{
	QPixmap pixmap2 = QPixmap(lineEdit->text());
}

void Ui_ONE_conv::init()
{

}
void Ui_ONE_conv::find_audio()
{
    QString lFileName = QFileDialog::getOpenFileName(this, " open file...", "C:/work/windows/Vidio_player/Tests", " All files ( *.* ) ");

    if (lFileName.isEmpty())
    {
        return;
    }
    lineEdit_109->setText(lFileName);

    Audio_inf_err info_audio;
    info_audio = get_audio_info_qt(lFileName);
    if (info_audio.err == -1)
    {
        QMessageBox::warning(this, " Error ", info_audio.txt_err, QMessageBox::Ok);
        return;
    }

    lineEdit_111->setText(info_audio.info.frequency);
    lineEdit_115->setText(info_audio.info.frequency);
    lineEdit_113->setText(info_audio.info.chanels);
    lineEdit_117->setText(info_audio.info.chanels);
    lineEdit_110->setText(info_audio.info.bitrate);
    lineEdit_116->setText(info_audio.info.bitrate);
    lineEdit_112->setText(info_audio.info.codec);
    lineEdit_114->setText(info_audio.info.duration);
}
void Ui_ONE_conv::find_audio_folder()
{
    QString lFileName = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "C:/work/windows/Qt_pojects/ONE_converter/Tests", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (lFileName.isEmpty())
    {
        return;
    }
    lineEdit_107->setText(lFileName);

}

void Ui_ONE_conv::start_play_audio()
{
    QString in_file = lineEdit_109->text();
    QByteArray ba;
    ba = in_file.toLatin1();
    const char* filename; filename = ba.data();


    play(filename, AUDIO_PREVIEW);
    
}
void Ui_ONE_conv::find_player_info()
{
    QString lFileName = QFileDialog::getOpenFileName(this, " open file...", "C:/work/windows/Vidio_player/Tests", " All files ( *.* ) ");
    if (lFileName.isEmpty())
    {
        return;
    }
    lineEdit_7->setText(lFileName);
    Audio_inf_err info_audio;
    Video_inf_err info_video;

    info_video = get_video_info_qt(lFileName);
    if (info_video.err == -1)
    {
        QMessageBox::warning(this, " Error ", info_video.txt_err, QMessageBox::Ok);
        lineEdit_15->setText("");
        lineEdit_16->setText("");
        lineEdit_17->setText("");
        lineEdit_19->setText("");
        lineEdit_25->setText("");
        lineEdit_18->setText("");
        lineEdit_20->setText("");
        lineEdit_10->setText("");
        lineEdit_11->setText("");
        return;
    }
    lineEdit_15->setText(info_video.info.width);
    lineEdit_16->setText(info_video.info.height);
    lineEdit_17->setText(info_video.info.bitrate);
    lineEdit_19->setText(info_video.info.codec);
    lineEdit_25->setText(info_video.info.duration);


    info_audio = get_audio_info_qt(lFileName);

    if (info_audio.err == -1)
    {
        QMessageBox::warning(this, " Error ", info_audio.txt_err, QMessageBox::Ok);
        lineEdit_18->setText("");
        lineEdit_20->setText("");
        lineEdit_10->setText("");
        lineEdit_11->setText("");
        return;
    }

    lineEdit_18->setText(info_audio.info.frequency);
    lineEdit_20->setText(info_audio.info.codec);
    lineEdit_10->setText(info_audio.info.bitrate);
    lineEdit_11->setText(info_audio.info.chanels);

}


void Ui_ONE_conv::find_video()
{   
	QString lFileName = QFileDialog::getOpenFileName(this, " open file...", "C:/work/windows/Vidio_player/Tests", " All files ( *.* ) ");
	if (lFileName.isEmpty())
	{
		return;
	}
	lineEdit_5->setText(lFileName);
	
    Video_inf_err info_video;
    info_video = get_video_info_qt(lFileName);
    if (info_video.err == -1)
    {
        QMessageBox::warning(this, " Error ", info_video.txt_err, QMessageBox::Ok);
        return;
    }
	lineEdit_27->setText(info_video.info.width);
	lineEdit_29->setText(info_video.info.height);
	lineEdit_26->setText(info_video.info.bitrate);
	lineEdit_28->setText(info_video.info.codec);
    lineEdit_30->setText(info_video.info.duration);
}




void Ui_ONE_conv::start_wind_video()
{
    QString in_file = lineEdit_5->text();
    QByteArray ba;
    ba = in_file.toLatin1();
    const char* filename; filename = ba.data();
    play(filename, VIDEO_PREVIEW);

}

void Ui_ONE_conv::player_run()
{
    QString in_file = lineEdit_7->text();
    QByteArray ba;
    ba = in_file.toLatin1();
    const char* filename; filename = ba.data();
    play(filename, VIDEO_PREVIEW);
}



void Ui_ONE_conv::inf_codec_video(const QString & info_par)
{
   
}
void Ui_ONE_conv::inf_format_video(const QString & info_par)
{

}
void Ui_ONE_conv::inf_format_image(const QString & info_par)
{

    if (info_par == "bmp")
        textEdit_9->setText("BMP - bitmap picture format.");
    if (info_par == "jpg" || info_par == "jpeg")
        textEdit_9->setText("JPEG(JPG) - joint photographic experts group.");
    if (info_par == "png")
        textEdit_9->setText("PNG - portable network graphics.");
    if (info_par == "tiff")
        textEdit_9->setText("TIFF - tagged image file format");
    if (info_par == "ppm")
        textEdit_9->setText("PPM - portable pixmap");
    if (info_par == "xbm")
        textEdit_9->setText("XBM - X11 bitmap");
    if (info_par == "xpm")
        textEdit_9->setText("XPM - X11 pixmap");
}
void Ui_ONE_conv::inf_format_audio(const QString & info_par)
{

}

void Ui_ONE_conv::byte_size_rb()
{
    lineEdit_24->setEnabled(true);
    radioButton->setChecked(false);
    lineEdit_22->setText("");
    lineEdit_23->setText("");
    lineEdit_22->setEnabled(false);
    lineEdit_23->setEnabled(false);
    comboBox_4->setEnabled(true);
}
void Ui_ONE_conv::pix_size_rb()
{
    lineEdit_22->setEnabled(true);
    lineEdit_23->setEnabled(true);
    comboBox_4->setEnabled(false);
    radioButton_2->setChecked(false);
    lineEdit_24->setText("");
    lineEdit_24->setEnabled(false);
}
void Ui_ONE_conv::audio_convert()
{
    Audio_c_err output;
    Transcoding_context recode_inform;
    QString outfile = lineEdit_107->text();
    outfile.append("/");
    outfile.append(lineEdit_108->text());
    outfile.append(".");
    QStringList codec_format_list = (QString(comboBox_15->currentText())).split('+');
    outfile.append(codec_format_list[1]);

    if ((recode_inform.audio_context.codec_id = qstring_to_sup_codec_id(codec_format_list[0])) == AV_CODEC_ID_NONE)
    {
        QMessageBox::warning(this, " Error ", " Unsupported codec ", QMessageBox::Ok);
        return;
    }
    
    recode_inform.audio_context.bit_rate            = lineEdit_116->text().toInt();
    recode_inform.audio_context.chanel_layout       = lineEdit_117->text().toInt();
    recode_inform.audio_context.sample_rate         = lineEdit_115->text().toInt();

    output = transcoding_audio(lineEdit_109->text(), outfile, recode_inform);
    Loading_widget->hide();
 
    if (output.err == -1) {
        QMessageBox::warning(this, " Error ", output.txt_err, QMessageBox::Ok);
        return;
    }
}

























Win::Win(QWidget *parent) :QWidget(parent)
{
	setWindowTitle("Image converter");

	frame = new QFrame(this);
	frame->setFrameShadow(QFrame::Plain);
	frame->setFrameShape(QFrame::Panel);

	Lbl1 = new QLabel("Input Image for conversion", this);
	fileline1 = new QLineEdit("", this);

	QStringList formats;
	formats << "bmp" << "jpg" << "jpeg" << "png" << "ppm" << "xbm" << "xpm";

	Lblselect = new QLabel("Select format for comvetion", this);
	selectformat = new QComboBox(this);
	selectformat->addItems(formats);

	prtscreen  = new QCheckBox("Take the Print Screen image", this);
	invertpix  = new QCheckBox("Invert pixeles", this);
	mirror     = new QCheckBox("Mirrored the image vertically", this);
	rgbswap    = new QCheckBox("Swap RGB to BRG", this);

	lstsession = new QPushButton("Use last settings", this);

	LblDpi = new QLabel("Dots per inch", this);
	DpiEdit = new QLineEdit("", this);

	Lbl2 = new QLabel("Path for output file", this);
	fileline2 = new QLineEdit("", this);

	lblName = new QLabel("Name of outputfile", this);
	nameEdit = new QLineEdit("", this);

	Calculate = new QPushButton("CONVERS", this);
	setStyleSheet("QPushButton { background-color: #119844; }");
	setStyleSheet("QPushButton:hover { background-color: white }");

	QVBoxLayout *layout = new QVBoxLayout(frame);

	layout->addWidget(Lblselect);
	layout->addWidget(selectformat);

	layout->addWidget(Lbl1);
	layout->addWidget(fileline1);

	layout->addWidget(Lbl2);
	layout->addWidget(fileline2);

	layout->addWidget(lblName);
	layout->addWidget(nameEdit);

	layout->addWidget(Calculate);
	layout->addWidget(frame);

	layout->addStretch();

	QVBoxLayout *layout1 = new QVBoxLayout();

	layout1->addWidget(prtscreen);
	layout1->addWidget(invertpix);
	layout1->addWidget(mirror);
	layout1->addWidget(rgbswap);
	layout1->addWidget(lstsession);

	layout1->addWidget(Lblselect);
	layout1->addWidget(selectformat);

	layout1->addWidget(LblDpi);
	layout1->addWidget(DpiEdit);

	layout->addStretch();

	QHBoxLayout *hLayout = new QHBoxLayout(this);
	hLayout->addWidget(frame);
	hLayout->addLayout(layout1);

	connect(lstsession, SIGNAL(clicked(bool)), this, SLOT( init()));
	connect(Calculate,  SIGNAL(clicked(bool)), this, SLOT( calc()));

}

void Win::calc()
{
	QString Filepth2;
	QString Fname;
	QString Dpi;
	QString Filepth1;
	QString formatconv;
	int dpm;

	Filepth1 = fileline1->text();
	formatconv = selectformat->currentText();
	Dpi = DpiEdit->text();
	dpm = int(Dpi.toInt() / 0.0254);

	QImage Image(Filepth1);

	if (prtscreen->isChecked() == true)
	{
		QClipboard *clipboard = QApplication::clipboard();
		Image = clipboard->image(QClipboard::Clipboard);
	}
	if (invertpix->isChecked() == true)
		Image.invertPixels();
	if (mirror->isChecked() == true)
		Image = Image.mirrored(true);
	if (rgbswap->isChecked() == true)
		Image = Image.rgbSwapped();
	Image.setDotsPerMeterX(dpm);
	Image.setDotsPerMeterY(dpm);

	Filepth2 = fileline2->text();
	QString tmp = Filepth2;
	Fname = nameEdit->text();
	Filepth2.append("/"); Filepth2.append(Fname); Filepth2.append("."); Filepth2.append(formatconv);
	if (tmp == "")
	{
		QDateTime time = QDateTime::currentDateTime();
		Filepth2 = time.toString();
		Filepth2.append("d");
		Filepth2.append(".");
		Filepth2.append(formatconv);
	}
	if (formatconv == "bmp")
		Image.save(Filepth2, "BMP");
	if (formatconv == "jpg")
		Image.save(Filepth2, "JPG");
	if (formatconv == "jpeg")
		Image.save(Filepth2, "JPEG");
	if (formatconv == "png")
		Image.save(Filepth2, "PNG");
	if (formatconv == "ppm")
		Image.save(Filepth2, "PPM");
	if (formatconv == "xbm")
		Image.save(Filepth2, "XBM");
	if (formatconv == "xpm")
		Image.save(Filepth2, "XPM");
	QFile  fp("C:/work/windows/Qt_pojects/ONE_converter/ONE_converter/default_propertiesUser.txt");
	fp.remove();
	QFile  fp1("C:/work/windows/Qt_pojects/ONE_converter/ONE_converter/default_propertiesUser.txt");
	fp1.open(QIODevice::WriteOnly);
	QTextStream out(&fp1);
	out << Filepth1; out << "\n";
	out << tmp;      out << "\n";
	out << Fname;    out << "\n";
	out << Dpi;
	fp1.close();

}

void Win::init()
{
	QFile fp("C:/work/windows/Qt_pojects/ONE_converter/ONE_converter/default_propertiesUser.txt");
	fp.open(QIODevice::ReadOnly);
	QTextStream in(&fp);
	QString fl1;
	in >> fl1;
	fileline1->setText(fl1);
	QString fl2;
	in >> fl2;
	fileline2->setText(fl2);
	QString oi;
	in >> oi;
	nameEdit->setText(oi);
	QString num;
	in >> num;
	DpiEdit->setText(num);
	fp.close();
}
