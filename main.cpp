#include "stdafx.hpp"

#include "FileUploader.hpp"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	ChooseFileDlg dlg;
	return dlg.exec();
}
