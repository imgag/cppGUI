#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include "cppGUI_global.h"
#include <QDialog>

namespace Ui {
class AboutDialog;
}

//About dialog
class CPPGUISHARED_EXPORT AboutDialog
	: public QDialog
{
	Q_OBJECT

public:
	AboutDialog(QWidget* parent = nullptr);
	~AboutDialog();

	//Set icon
	void setIcon(QPixmap pixmap);
	//Sets description (rich-text).
	void setDescription(QString description);

	//Add a library version line (rich-text).
	void addLibVersionLine(QString line);

	//Set additional information label (rich-text).
	void setAddInfo(QString add_info);

private:
	Ui::AboutDialog* ui_;
};

#endif // ABOUTDIALOG_H
