#pragma once

#include <QFrame>
#include <QMouseEvent>
#include <QVector>
#include <QLabel>
#include <QListWidget>
#include <qwebengineview.h>
#include <qwebchannel.h>
#include <iostream>
#include <algorithm>
#include <qmenu.h>
#include <qfile.h>
#include <qdialog.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <iostream>
#include <fstream> 
#include "engine.h"
#include "matrix.h"
#include "tmwtypes.h"

//记得将lib中的lib文件添加库中
using namespace std;


class Map : public QFrame
{
	Q_OBJECT

public:
	Map(QWidget *parent);
	~Map();

signals:
	void movepoint(QString lng, QString lat, QString containerX, QString containerY);
	void tablepoint(QString lng, QString lat);
	void savepoint(QVector<QPointF>, int);
	void sent(QPointF);

public:
	void SearchArea(bool millitary_model);
	void StartSearch(QVector<QString>matlab_uav_data, double scale, double minflight, double maxflight, QMap<int, QColor>LineColor);
	void StartKeyArea();
	void EndSearch();
	void ReStart();
	void dynamic_Rocover();
	void Dynanic_index(double index);
	void DeletePoint(int row);           //删除选择的经纬度点数据
	bool WatchUAVLine(int line);  //观察航线函数
	void LowModel();          //低精度和高进度模式切换
	void HighModel();

private:
	void paintEvent(QPaintEvent * event);
	void timerEvent(QTimerEvent *event);
	// 鼠标支持
	void mouseMoveEvent(QMouseEvent * event);
	void mousePressEvent(QMouseEvent * event);
	void mouseReleaseEvent(QMouseEvent * event);
	void mouseDoubleClickEvent(QMouseEvent *event);
	void resizeEvent(QResizeEvent *event);
	void wheelEvent(QWheelEvent* event);
	void eventFilter(QEvent* event);
	bool event(QEvent * event);

public slots:
	int radar();        //设置雷达
	int actiondanger_begin();		//设置危险区域
	int actiondanger_end();
	//航迹点对应修改，添加删除的函数
	int actionpoint_change();
	int actionpoint_add();
	int actionpoint_delete();
	bool eventFilter(QObject *, QEvent *);          //安装installEventFilter形成穿透效果
	void slotPopulateJavaScriptWindowObject();    //qt给js注册对象的函数，标准写法
	void onCall(QString lng, QString lat);         //js回传数据到qt的函数，注意是槽函数
	void container(QString x, QString y);
	void datatommap(QString x_lng, QString y_lat);               //从地图中获取到对应的像素点坐标
	void mousemovedata(QString lng, QString lat, QString x, QString y);
	void sentbase(QString lng, QString lat, QString base);             //s手动输入基地
	void GetBaseData(QString lng, QString lat, QString BaseX, QString BaseY, QString Base);             //获取基地信息
	void GetPointData(QString lng, QString lat, QString BaseX, QString BaseY);                  //获取目标点的信息从地图中
	void ShowMapPointLnglat(QString lng, QString lat, QString containerX, QString containerY);

public:
	bool m_pressedFlag;
	bool m_matlabFlag;
	bool m_keyFlag;
	bool m_EndFlag;
	bool m_pathFlag;
	bool dynamic_status;   //设置无人机动态飞行
	QVector<QPoint> m_points;
	QVector<QPoint> m_keyPoints;
	QVector<QPointF>m_breakArea;   //画出分割框;
	QVector<QPointF>m_UAV;
	QPolygonF polygon;
	QVector<int>m_number;
	QVector<int>m_length;
	QPoint  m_curPos; //  鼠标当前位置
	QPoint  m_start, m_end;
	int m_timeId;
	int dynamic_length = 100;
	int total_sum = 0;
	QVector<QPointF>dynamic_UAV;
	QVector<QPointF>temp_UAV;
	QVector<int>BaseData;           //基地数据

	float ratio;       //图片缩放比例大小
	int action;          //动作(放大,缩小,移动...)
	QPointF offset;              //一次的图片偏移值
	QPointF Alloffset;           //总偏移
	//底图
	QPixmap pixmap;  //加载图片
	int pixW;            //图片宽
	int pixH;            //图片高
	//军事基地
	QPixmap millitary_base;         //加载军事基地
	int militaryW;
	int militaryH;

	QPixmap radar_base;               //加载雷达
	int radarW;
	int radarH;

	bool radar_status = false;     //雷达状态
	QVector<float> radar_r;


	QVector<QVector<QPointF>>danger_area;    //加载威胁区域个数
	bool danger_status = false;          //威胁状态关闭
	int danger_number = 0;                //计数危险区域

	QMap<int, QColor>GetLineColor;            //获取航线颜色标识
	bool GetLineColorStatus = false;                    //是否获取航线
	int CurChooseLine = 0;       //设置获取读取航线的状态
	int preChooseLine = -1;

	//用户航迹点修改矩阵
	QVector<QVector<QPointF>>ChoosePoint;
	bool pointchange_status = false;    //打开选点
	bool pointBeginChange_status = false;    //z找到要修改航迹并启动修改
	QPointF pointchange;            //获取修改航迹点
	//用户航迹点添加
	bool pointadd_status = false;        //航迹点添加状态
	bool pointaddbegin_status = false;    //启动添加选点
	QPoint pointadd;
	//用户删除航路点
	bool pointdelete_status = false;
	bool pointdeletebegin_status = false;
	QPointF pointdelete;
	QVector<QString>Getlnglat;

	bool picture_model = false;
	QString picture_path;
	QVector<double>picture_lnglat;
	QString currentlng;
	QString currentlat;
	//获取滚轮状态
public:
	enum  Type {
		None = 0,
		Amplification,
		Shrink,
		Lift,
		Right,
		Up,
		Down,
		Move
	};

private:
	QWebEngineView *view;   //加载地图

};
