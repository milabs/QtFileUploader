#pragma once

#include "stdafx.hpp"

class ChooseFileDlg : public QDialog
{
	Q_OBJECT
public:
	ChooseFileDlg(QWidget *parent = 0);

public slots:
	void onFileSelect(bool);
};
