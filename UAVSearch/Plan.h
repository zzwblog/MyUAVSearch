#pragma once

#include <QDialog>
#include "ui_Plan.h"

class Plan : public QDialog
{
	Q_OBJECT

public:
	Plan(QWidget *parent = Q_NULLPTR);
	~Plan();

private slots:
	int OnBtnOk();
	int OnBtnCancel();

public:
	QString B_X;
	QString B_Y;
	QString BASE;

private:
	Ui::Plan ui;
};
