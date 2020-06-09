#include "Plan.h"
#include <qmessagebox.h>

Plan::Plan(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	connect(ui.btnok, SIGNAL(clicked()), this, SLOT(OnBtnOk()));
	connect(ui.btncancel, SIGNAL(clicked()), this, SLOT(OnBtnCancel()));
}

Plan::~Plan()
{
}


int Plan::OnBtnOk()
{
	B_X = ui.BaseX->text();
	B_Y = ui.BaseY->text();
	BASE = ui.Base->text();
	if (BASE == "")
	{
		return 0;
	}
	if (BASE.toInt() > 3 || BASE.toInt() < 0)
	{
		QMessageBox::information(NULL, "title", "wrong base");
		return 0;
	}

	// 	kind = ui.uavkind->text();
	// 	speed = ui.uavspeed->text();
	// 	high = ui.uavh->text();
	// 	fuel = ui.uavf->text();
	// 	r = ui.uavr->text();
	return 0;
}
int Plan::OnBtnCancel()
{
	//reject();
	return 0;
}