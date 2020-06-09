#include "UAVSearch.h"
#include <qwebengineview.h>
#include <Qstring>
#include <qstringlist.h>
#include <qdir.h>
#include <qdebug.h>
#include <qfiledialog.h>
#include <qmenu.h>

UAVSearch::UAVSearch(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	QString str = QString(u8"多无人机系统");
	this->setWindowTitle(str);//设置窗口标题

	connect(ui.btnSearch, SIGNAL(clicked()), this, SLOT(OnSettingSearch())); //图片数据
	connect(ui.btnStart, SIGNAL(clicked()), this, SLOT(OnSettingStart()));//开始搜索
	connect(ui.btnKey, SIGNAL(clicked()), this, SLOT(OnSettingKeyArea()));//区域选择
	connect(ui.btnNo, SIGNAL(clicked()), this, SLOT(OnSettingEndSearch()));//结束搜索
	connect(ui.btnRestart, SIGNAL(clicked()), this, SLOT(OnSettingReStart()));//重新搜索
	connect(ui.AddBase, SIGNAL(clicked()), this, SLOT(OnSettingBase()));//基地
	connect(ui.AddUAV, SIGNAL(clicked()), this, SLOT(OnAddUAV()));//添加无人机
	connect(ui.AddEnd,&QPushButton::clicked,this,&UAVSearch::OnAddEnd);
	connect(ui.DeleteUAV, SIGNAL(clicked()), this, SLOT(OnDeleteUAV()));//删除无人机


	connect(ui.tableWidgetUAV, SIGNAL(cellChanged(int, int)), this, SLOT(OnSettingUAV(int, int)));//无人机参数表格
	connect(ui.tableWidgetUAV, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(UAVLineStatus()));

	//设置无人机机库的tablewidget数据
	ui.tableWidgetUAV->setShowGrid(true);
	ui.tableWidgetUAV->setSortingEnabled(true);
	ui.tableWidgetUAV->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.tableWidgetUAV->horizontalHeader()->setStyleSheet("QHeaderView::section{background:skyblue;}");
	ui.tableWidgetUAV->setColumnCount(12);//12格
	ui.tableWidgetUAV->resizeColumnsToContents();
	ui.tableWidgetUAV->resizeRowsToContents();
	ui.tableWidgetUAV->verticalHeader()->setDefaultSectionSize(70);
	ui.tableWidgetUAV->horizontalHeader()->setDefaultSectionSize(140);

	QStringList headersUAV;//标题组
	headersUAV.append(QString(u8"型号"));
	headersUAV.append(QString(u8"时间: h"));
	headersUAV.append(QString(u8"高度: km"));
	headersUAV.append(QString(u8"速度: km/h"));
	headersUAV.append(QString(u8"机号"));
	headersUAV.append(QString(u8"相机焦距: mm"));
	headersUAV.append(QString(u8"相机光圈: mm"));
	headersUAV.append(QString(u8"最小转弯角度: o"));
	headersUAV.append(QString(u8"最小转弯半径: km"));
	headersUAV.append(QString(u8"转弯延长公里: km"));
	headersUAV.append(QString(u8"重叠率: %"));
	headersUAV.append(QString(u8"航线颜色标识"));
	ui.tableWidgetUAV->setHorizontalHeaderLabels(headersUAV);//防止标题

	connect(ui.btn_scale, SIGNAL(clicked()), this, SLOT(OnSettingScale()));//比例尺
	connect(ui.btn_dynamic, SIGNAL(clicked()), this, SLOT(OnSettingDynamicUAV()));//动态
	connect(ui.btn_recover, SIGNAL(clicked()), this, SLOT(OnSettingDynamicRecover()));//恢复
	connect(ui.M_model, SIGNAL(clicked()), this, SLOT(OnMillitary_model()));//军用模式
	connect(ui.HL_model, SIGNAL(clicked()), this, SLOT(OnLowToHighModel()));//低精度模式

	//地图窗口
	connect(ui.frame, SIGNAL(movepoint(QString, QString, QString, QString)), this, SLOT(OnMouseTrace(QString, QString, QString, QString)));         //屏幕显示的像素点
	connect(ui.frame, SIGNAL(tablepoint(QString, QString)), this, SLOT(OnTableWidget(QString, QString)));
	connect(ui.frame, SIGNAL(savepoint(QVector<QPointF>, int)), this, SLOT(OnSavePoint(QVector<QPointF>, int)));
	connect(ui.frame, SIGNAL(sent(QPointF)), this, SLOT(datafromweb(QPointF)));

	//设置获取经纬度的tablewidget数据
	ui.tableWidget->setShowGrid(true);
	ui.tableWidget->setSortingEnabled(true);
	ui.tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	ui.tableWidget->setColumnCount(2);
	ui.tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	// 水平方向的标题
	QStringList headers;
	headers.append(QString(u8"经度"));  // 经度
	headers.append(QString(u8"纬度")); // 维度
	ui.tableWidget->setHorizontalHeaderLabels(headers);

	connect(ui.tableWidget, SIGNAL(customContextMenuRequested(const QPoint&)),
		this, SLOT(OnListContextMenu(const QPoint&)));
	connect(ui.actionDelete, SIGNAL(triggered()),
		this, SLOT(OnMenuRemove()));
}

//区域选择
bool UAVSearch::OnSettingKeyArea()
{
	ui.btnKey->setStyleSheet("QPushButton{color:red;background:yellow}");//设置按键颜色
	ui.btnKey->setText(QString::fromLocal8Bit("已开始"));
	ui.frame->StartKeyArea();
	return 0;
}
//结束选择
bool UAVSearch::OnSettingEndSearch()
{
	ui.btnKey->setStyleSheet(NULL);
	ui.btnKey->setText(QString::fromLocal8Bit("区域选择"));
	ui.btnKey->setEnabled(false);//选完了就不能再按了
	ui.frame->EndSearch();
	return 0;
}

//图片模式
bool UAVSearch::OnSettingSearch()
{
	low_to_high = true;//地端模式
	ui.HL_model->setText(QString::fromLocal8Bit("低精度模式"));//好像没卵用，控件名字本来就是这个
	int width = this->width();
	int height = this->height();

	ui.frame->SearchArea(millitary_model);  //设置地图的显示大小
	return 0;
}

//手动根据经纬度添加基地
bool UAVSearch::OnSettingBase()
{
	Plan dlg(this);
	int ret = dlg.exec();
	QString baseX;
	QString baseY;
	QString base;
	if (ret == QDialog::Accepted)
	{
		baseX = dlg.B_X;
		baseY = dlg.B_Y;
		base = dlg.BASE;
		bool OneBase = true;   //判断是否有重复基地
		for (int i = 0; i < BaseNumber.size(); i++)
		{
			if (base.toInt() == BaseNumber[i])
			{
				OneBase = false;
				QMessageBox::information(NULL, "title", "Repeat base");
				return 0;
			}
		}
		if (OneBase == true)
		{
			BaseNumber.push_back(base.toInt());
		}
		if (millitary_model == true)
		{
			ui.frame->GetBaseData(baseX, baseY, baseX, baseY, base);
		}
		else
		{
			ui.frame->sentbase(baseX, baseY, base);
		}

	}
	return 0;
}

//开始搜索
bool UAVSearch::OnSettingStart()
{
	matlab_uav_data.clear();//存储MATLAB程序运行的结果
	int row = ui.tableWidgetUAV->rowCount();
	for (int i = 0; i < row; i++)
	{//依次存储要发送的数据
		matlab_uav_data.push_back(ui.tableWidgetUAV->item(i, 4)->text());
		matlab_uav_data.push_back(ui.tableWidgetUAV->item(i, 1)->text());
		matlab_uav_data.push_back(ui.tableWidgetUAV->item(i, 2)->text());
		matlab_uav_data.push_back(ui.tableWidgetUAV->item(i, 3)->text());
		matlab_uav_data.push_back(ui.tableWidgetUAV->item(i, 5)->text());
		matlab_uav_data.push_back(ui.tableWidgetUAV->item(i, 6)->text());
		matlab_uav_data.push_back(ui.tableWidgetUAV->item(i, 7)->text());
		matlab_uav_data.push_back(ui.tableWidgetUAV->item(i, 8)->text());
		matlab_uav_data.push_back(ui.tableWidgetUAV->item(i, 9)->text());
		matlab_uav_data.push_back(ui.tableWidgetUAV->item(i, 10)->text());
		//无人机飞行高度
		uavtoheight[ui.tableWidgetUAV->item(i, 4)->text().toInt()] = ui.tableWidgetUAV->item(i, 2)->text();
		//无人机的标号
		int line = ui.tableWidgetUAV->item(i, 4)->text().toInt();
		//获取颜色选择
		QComboBox *combox = (QComboBox*)(ui.tableWidgetUAV->cellWidget(i, 11));
		int color = combox->currentIndex();
		if (color == 0)
		{
			uav_color[line] = QColor(0xff, 0xb6, 0xc1);
		}
		if (color == 1)
		{
			uav_color[line] = QColor(0x00, 0xdd, 0x00);
		}
		if (color == 2)
		{
			uav_color[line] = QColor(0xff, 0xff, 0x00);
		}
		if (color == 3)
		{
			uav_color[line] = QColor(0xff, 0x00, 0xff);
		}
		if (color == 4)
		{
			uav_color[line] = QColor(0x00, 0x00, 0xff);
		}
		qDebug() << uav_color[line];
	}

	double minflight = ui.minflight->text().toDouble();
	double maxflight = ui.maxfight->text().toDouble();
	ui.frame->StartSearch(matlab_uav_data, current_scale, minflight, maxflight, uav_color);
	return 0;
}

//重新开始，清除所有数据
bool UAVSearch::OnSettingReStart()
{
	int width = this->width();
	int height = this->height();

	//经纬度数据清空
	ui.mouseTracelng->clear();
	ui.mouseTracelat->clear();
	BaseNumber.clear();
	int countrow = ui.tableWidget->rowCount();
	for (int i = 0; i < countrow; i++)
	{
		ui.tableWidget->removeRow(0);
	}
	int uavrow = ui.tableWidgetUAV->rowCount();
	for (int i = 0; i < uavrow; i++)
	{
		ui.tableWidgetUAV->removeRow(0);
	}
	uavnumber.clear();
	ui.btnKey->setStyleSheet(NULL);
	ui.btnKey->setText(QString::fromLocal8Bit("区域选择"));
	//清空搜索区域数据
	matlab_uav_data.clear(); //清空传入matilab的无人机数据
	ui.scale_text->clear();
	ui.dynamic_text->clear();
	uav_color.clear();             //无人机航迹和颜色清空
	current_scale = 0;
	ui.frame->ReStart();
	uavtoheight.clear();         //无人机对应的高度
	millitary_model = false;
	low_to_high = false;
	ui.HL_model->setText(QString::fromLocal8Bit("低精度模式"));
	ui.minflight->clear();
	ui.maxfight->clear();
	return 0;
}


///////////////////////////////////////////
//添加无人机
void UAVSearch::OnAddUAV()
{
	ui.tableWidgetUAV->setEditTriggers(QAbstractItemView::DoubleClicked);
	int row = ui.tableWidgetUAV->rowCount();//获取当前行
	if (row > 9)//只能添加10架无人机
	{
		QMessageBox::information(this, "this", QString::fromLocal8Bit("无人机已满"));
	}

	ui.tableWidgetUAV->insertRow(row);//在row后面插入一行
	for (int i = 0; i < 11; i++)//11种元素赋值为空，然后可以填写
	{
		ui.tableWidgetUAV->setItem(row, i, new QTableWidgetItem(""));
	}
	//设置航线颜色
	QStringList list;
	list.append(QString::fromLocal8Bit("粉色"));
	list.append(QString::fromLocal8Bit("绿色"));
	list.append(QString::fromLocal8Bit("黄色"));
	list.append(QString::fromLocal8Bit("紫色"));
	list.append(QString::fromLocal8Bit("蓝色"));
	QComboBox *combox = new QComboBox();//新建下拉框
	combox->addItems(list);
	ui.tableWidgetUAV->setCellWidget(row, 11, combox);
}

void UAVSearch::OnAddEnd()
{
	int row = ui.tableWidgetUAV->rowCount();//获取当前行
	for (int i = 0; i < 11; i++)//11种元素赋值为空，然后可以填写
	{
		bool isRightAdd = true;
		ui.tableWidgetUAV->item(row-1, i)->text().toInt(&isRightAdd);
		if (!isRightAdd)
		{
			QMessageBox::warning(this, u8"警告", u8"添加的无人机参数有误，请重新填写！",
				QMessageBox::Ok);
			ui.tableWidgetUAV->removeRow(row);
			return;				
		}
		//QTableWidgetItem* item = new QTableWidgetItem;
		//item->setFlags(item->flags() &= (~Qt::ItemIsEditable));
		//ui.tableWidgetUAV->setItem(row - 1, i, item);
		//delete item;
	}
	QMessageBox::information(this, u8"通知", u8"添加的无人机参数成功！",
		QMessageBox::Ok);
	ui.tableWidgetUAV->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

//删除无人机
void UAVSearch::OnDeleteUAV()
{
	int row = ui.tableWidgetUAV->currentRow();
	QTableWidgetItem* item = ui.tableWidgetUAV->takeItem(row, 0);
	if (item)//查看是否填入了新的无人机信息
	{
		ui.tableWidgetUAV->removeRow(row);
	}
}

/////////////////////////////////////////
//设置像素点和经纬度
int UAVSearch::OnMouseTrace(QString lng, QString lat, QString containerX, QString containerY)
{
	//显示在经度纬度的QLine中
	ui.mouseTracelng->setText(lng);
	ui.mouseTracelat->setText(lat);
	ui.mouseX->setText(containerX);
	ui.mouseY->setText(containerY);
	return 0;
}
//设置比例尺
bool UAVSearch::OnSettingScale()
{
	double index;
	if (ui.scale_text->text() == "")
	{
		index = 0;
	}
	else
	{
		index = ui.scale_text->text().toDouble();
	}
	current_scale = index;
	return 0;
}
//设置动态路线，即画动态路线
bool UAVSearch::OnSettingDynamicUAV()
{
	double index = ui.dynamic_text->text().toDouble();
	ui.frame->Dynanic_index(index);
	return 0;
}
//恢复航迹，抹掉动态航迹
bool UAVSearch::OnSettingDynamicRecover()
{
	ui.frame->dynamic_Rocover();
	return 0;
}
bool UAVSearch::OnMillitary_model()
{
	millitary_model = true;
	return 0;
}

void UAVSearch::OnLowToHighModel()
{
	if (low_to_high == true)
	{
		low_to_high = false;
		ui.HL_model->setText(QString::fromLocal8Bit("高精度模式"));
		ui.frame->HighModel();
	}
	else
	{
		ui.HL_model->setText(QString::fromLocal8Bit("低精度模式"));
		low_to_high = true;
		ui.frame->LowModel();
	}
}
//////////////////////////////////////
//选择了关键点后添加经纬度到table中
void UAVSearch::AddData(const QString &x, const QString &y)
{
	int row = ui.tableWidget->rowCount();
	ui.tableWidget->insertRow(row);//添加一行，row为行号

	// x
	QTableWidgetItem* item = new QTableWidgetItem();
	item->setText(x);
	ui.tableWidget->setItem(row, 0, item);

	// y
	item = new QTableWidgetItem();
	item->setText(y);
	ui.tableWidget->setItem(row, 1, item);
}

//经纬度表格显示菜单
int UAVSearch::OnListContextMenu(const QPoint &pt)
{
	// 当前选中项
	QTableWidgetItem* selected = ui.tableWidget->itemAt(pt);

	// 构造菜单及菜单项
	QMenu menu;
	if (selected)
	{
		menu.addAction(ui.actionDelete);
	}

	// 显示菜单
	menu.exec(ui.tableWidget->mapToGlobal(pt));

	return 0;
}

//无人机参数表格
void UAVSearch::OnSettingUAV(int x, int y)
{
	if (y != 4)
	{
		return;     //只根据第四列来判断基地
	}

	QString tempuav = ui.tableWidgetUAV->item(x, y)->text();
	if (BaseNumber.empty() && ui.frame->BaseData.empty())
	{
		QMessageBox::information(NULL, "this", "no base");
		return;
	}

	//获取从低精度模式下输入的基地
	if (!ui.frame->BaseData.empty())
	{
		BaseNumber.clear();
		for (int i = 0; i < ui.frame->BaseData.size() / 3; i++)
		{
			BaseNumber.push_back(ui.frame->BaseData[i * 3 + 2]);
		}
	}

	int index;
	if (tempuav.isEmpty())
	{
		index = 0;
	}
	index = tempuav.toInt();

	if (index > 10 || index < 0)
	{
		QMessageBox::information(NULL, "title", "wrong base");
		return;
	}
	std::sort(uavnumber.begin(), uavnumber.end());
	for (int i = 0; i < uavnumber.size(); i++)
	{
		if (index == uavnumber[i])
		{
			QMessageBox::information(NULL, "this", QString::fromLocal8Bit("无人机已经存在"));
			return;
		}
	}
	if (uavnumber.size() > 10)
	{
		QMessageBox::information(NULL, "this", QString::fromLocal8Bit("机库已满"));
		return;
	}
	bool basestatus = false;
	std::sort(BaseNumber.begin(), BaseNumber.end());
	for (int i = 0; i < BaseNumber.size(); i++)
	{
		//判断无人机是否有对应的基地
		int baseNu = BaseNumber[i];
		if (index == 0)
		{
			basestatus = true;
			break;
		}
		if ((index == 1 || index == 2 || index == 3) && baseNu == 1)
		{
			basestatus = true;
			break;
		}
		if ((index == 4 || index == 5 || index == 6) && baseNu == 2)
		{
			basestatus = true;
			break;
		}
		if ((index == 7 || index == 8 || index == 9 || index == 10) && baseNu == 3)
		{
			basestatus = true;
			break;
		}
	}
	if (basestatus == false)
	{
		QMessageBox::information(NULL, "this", QString::fromLocal8Bit("无此基地，注意1、2、3对应1号基本"
			"4、5、6对应2号基地，7、8、9、10对应三号基地"));
		return;
	}
	if (index != 0)
	{
		uavnumber.push_back(index);    //保护index为空
	}
}


//根据像素添加经纬度
int UAVSearch::OnTableWidget(QString, QString)
{
	AddData(lng, lat);
	return 0;
}
//保存无人机的路径点
bool UAVSearch::OnSavePoint(QVector<QPointF> uav, int index)
{
	ofstream outFile;
	QString name = "uav" + QString::number(index) + ".txt";
	string Filename;
	Filename = string((const char *)name.toLocal8Bit());
	outFile.open(Filename, ios::out);

	// 	outFile.open("./data.txt",ios::app);
	// 	int i = index;
	// 	string s = "无人机";
	// 	outFile << s << i << "的数据: \n";
	// 	outFile << "起飞点: " << uav[0].x() << " " << uav[0].y()<<"\n";
	// 	outFile << "飞行时间： " << uav.size() << "\n";
	// 	outFile << "飞行轨迹如下" << "\n";
	for (int i = 0; i < uav.size(); i++)
	{
		string lat;
		string lng;
		if (millitary_model == false)
		{
			ui.frame->datatommap(QString::number(uav[i].x()), QString::number(uav[i].y()));
			lng = QString::number(fromweb.x(), 10, 8).toLocal8Bit();
			lat = QString::number(fromweb.y(), 10, 8).toLocal8Bit();
		}
		else
		{
			lng = QString::number(uav[i].x(), 10, 8).toLocal8Bit();
			lat = QString::number(uav[i].y(), 10, 8).toLocal8Bit();
		}
		//转换精度保存模式
		double data_lng = atof(lng.c_str());
		int d_lng = int(data_lng);
		int m_lng = int((data_lng - d_lng) * 60);
		double s_lng = ((data_lng - d_lng) * 60 - m_lng) * 60;
		//转换纬度保存模式
		double data_lat = atof(lat.c_str());
		int d_lat = int(data_lat);
		int m_lat = int((data_lat - d_lat) * 60);
		double s_lat = ((data_lat - d_lat) * 60 - m_lat) * 60;
		outFile << index << "\t";
		outFile << i << "\t";
		outFile << "0" << "\t";
		outFile << d_lng << " " << m_lng << " " << fixed << setprecision(2) << s_lng << "\t";
		outFile << d_lat << " " << m_lat << " " << fixed << setprecision(2) << s_lat << "\t";
		outFile << uavtoheight[index].toInt() * 1000 << "\n";
	}
	outFile.close();
	return 0;
}


//画出航线，按下无人机项目后
bool UAVSearch::UAVLineStatus()
{
	QList<QTableWidgetItem*> items = ui.tableWidgetUAV->selectedItems();
	QTableWidgetItem *item = items.at(4);
	int line = item->text().toInt(); //获取航线
	if (line == 0)
	{
		return 0;
	}
	ui.frame->WatchUAVLine(line);
	return 0;
}


//删除选中的点
int UAVSearch::OnMenuRemove()
{
	int row = ui.tableWidget->currentRow();//当前行
	QTableWidgetItem* item = ui.tableWidget->takeItem(row, 0);//获取当前项目
	ui.frame->DeletePoint(row);//删除该点的标记
	if (item)
	{
		ui.tableWidget->removeRow(row);//表中删除
	}
	return 0;
}

void UAVSearch::datafromweb(QPointF pppp)
{
	fromweb = pppp;
}

void UAVSearch::setValueFromWeb(const QString &weblng, const QString &weblat)
{
	lng = weblng;
	lat = weblat;
}










