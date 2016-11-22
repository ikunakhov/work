#include <QtWidgets/QApplication>
#include <QStyleFactory>
#include "one_converter.h"
#include "ui_one_converter.h"

void nonwindow(QString& Filepth1);
#define _NEW_INTERFACE_

#ifdef _OLD_INTERFACE_
int main(int argc, char *argv[])
{
	if (argc == 2)
	{
		QString  r = argv[1];
		nonwindow(r);
		return 0;
	}
	if (argc == 1)
	{
		QApplication app(argc, argv);
		QApplication::setStyle("macintosh");
		Win win(0);
		win.show();
		return app.exec();

	}

}
#endif

#ifdef _NEW_INTERFACE_

int main(int argc, char *argv[])
{

#ifdef Q_OS_WIN32 || Q_OS_WIN64
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("IBM 866"));
#endif
#ifdef Q_OS_LINUX
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
#endif

#ifdef Q_OS_MAC || Q_OS_MAC32 || Q_OS_MAC64

#endif


	if (argc == 2)
	{
		QString  r = argv[1];
		nonwindow(r);
		return 0;
	}
	if (argc == 1)
	{
		QApplication a(argc, argv);
		QMainWindow *MainWindow = new QMainWindow;
		Ui_ONE_conv ui (MainWindow);
		ui.show();
		return a.exec();
	}
}
#endif





void nonwindow(QString & Filepth1)
{
	QString formatconv = "jpeg";
	QString point = ".";
	int index = Filepth1.lastIndexOf(point);
	QImage Image(Filepth1);
	Filepth1.chop(Filepth1.length() - index - 1);
	QString Filepth2 = Filepth1.append(formatconv);

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
}