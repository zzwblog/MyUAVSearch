#include "UAVSearch.h"
#include <QtWidgets/QApplication>
#include <qsplashscreen.h>
#include <qthread.h>
#include <qlabel.h>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	UAVSearch w;

	QSplashScreen *splash = new QSplashScreen();
	//    splash->setPixmap(QPixmap("D:\\MyFiles\\ITWorld\\MyUAVSearch\\UAVSearch\\back.jpg").scaled(1200, 700, Qt::KeepAspectRatio));
	splash->setPixmap(QPixmap("./image/back.jpg").scaled(1200, 700, Qt::KeepAspectRatio));

	QFont ft;
	ft.setPointSize(15);
	QPalette pa;
	pa.setColor(QPalette::WindowText, Qt::white);

	QLabel *text1 = new QLabel(splash);
	text1->setFont(ft);
	text1->setPalette(pa);
	text1->setText(QString(u8"厦门大学"));

	QLabel *text2 = new QLabel(splash);
	text2->setFont(ft);
	text2->setPalette(pa);
	text2->move(0, splash->height() - 30);
	text2->setText(QString(u8"正在启动..."));

	QLabel *text3 = new QLabel(splash);
	ft.setPointSize(30);
	text3->setFont(ft);
	text3->setPalette(pa);
	text3->move(140, 200);
	text3->setText(QString(u8"无人机航线协同规划软件"));

	splash->show();
	a.processEvents();//去除线程等待的图标
	QThread::sleep(1);//延长时间
   // splash->close();//主动关闭弹窗


	w.resize(1200, 800);
	w.show();
	splash->finish(&w);//主窗口显示后，弹窗立马消失
	return a.exec();
}
