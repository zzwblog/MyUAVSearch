#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_UAVSearch.h"
#include <qpoint.h>
#include <qmap.h>
#include <qcolor.h>
#include <qmessagebox.h>
#include <qcombobox.h>
#include <qtablewidget.h>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include "Plan.h"
using namespace std;

class UAVSearch : public QMainWindow
{
	Q_OBJECT

public:
	UAVSearch(QWidget *parent = Q_NULLPTR);

private:
	Ui::UAVSearchClass ui;

private slots:
	bool OnSettingSearch();     //开启搜索，点击搜索区域
	bool OnSettingStart();
	bool OnSettingKeyArea();
	bool OnSettingEndSearch();
	bool OnSettingReStart();
	bool OnSettingScale();
	bool OnSettingDynamicRecover();

	//void OnSettingUAV(QTableWidgetItem* item);
	void OnSettingUAV(int, int);
	bool OnSettingDynamicUAV();
	int OnMouseTrace(QString lng, QString lat, QString containerX, QString containerY);
	int OnTableWidget(QString, QString);
	bool OnSavePoint(QVector<QPointF> uav, int index);      //保存数据
	bool OnSettingBase();
	void OnAddUAV();
	void OnAddEnd();
	void OnDeleteUAV();

	bool UAVLineStatus();           //获取航线颜色
	int OnListContextMenu(const QPoint& pt);   //添加右键菜单
	int OnMenuRemove();
	void datafromweb(QPointF pppp);
	bool OnMillitary_model();
	void OnLowToHighModel();          //高进度和低精度模式切换

public:
	QVector<QString>matlab_uav_data;
	double current_scale = 0;
	void AddData(const QString& x, const QString& y);
	void setValueFromWeb(const QString &weblng, const QString &weblat);

private:
	QString lng;
	QString lat;
	QString position;
	QVector<QString>uav_data;

	QVector<int>BaseNumber;          //无人机基地
	QVector<int>uavnumber;               //判断无人机是否已满
	QMap<int, QColor>uav_color;           //无人机航迹与颜色匹配
	QPointF fromweb;                  //获取从网页的经纬度数据
	QMap<int, QString> uavtoheight;    //无人机对应的高度

	bool millitary_model = false;
	bool low_to_high = false;
};