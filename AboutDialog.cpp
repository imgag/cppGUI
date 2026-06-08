#include "AboutDialog.h"
#include <QLibraryInfo>
#include <QSysInfo>
#include "ui_AboutDialog.h"

AboutDialog::AboutDialog(QWidget *parent)
	: QDialog(parent)
	, ui_(new Ui::AboutDialog())
{
	ui_->setupUi(this);

	//title
	setWindowTitle("About "+QApplication::applicationName());

	//name
	ui_->name->setText(QApplication::applicationName());

	//version
	ui_->version->setText(QCoreApplication::applicationVersion());

	//lib versions
	QString lib_versions;
	lib_versions += "Operating system: " + QSysInfo::prettyProductName() + "<br>";
	lib_versions += "Architecture: " + QSysInfo::buildCpuArchitecture() + "<br>";
	lib_versions += "Qt version: " + QLibraryInfo::version().toString();
	ui_->lib_versions->setText(lib_versions);
}

AboutDialog::~AboutDialog()
{
	delete ui_;
}

void AboutDialog::setIcon(QPixmap pixmap)
{
	ui_->icon->setPixmap(pixmap.scaled(96, 96));
}

void AboutDialog::setDescription(QString description)
{
	ui_->description->setText(description);

	adjustSize();
}

void AboutDialog::addLibVersionLine(QString line)
{
	QString lib_versions = ui_->lib_versions->text();
	lib_versions += "<br>" + line.trimmed();
	ui_->lib_versions->setText(lib_versions);

	adjustSize();
}

void AboutDialog::setAddInfo(QString add_info)
{
	ui_->add_info->setText(add_info);

	adjustSize();
}


