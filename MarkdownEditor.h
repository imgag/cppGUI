#ifndef MARKDOWNEDITOR_H
#define MARKDOWNEDITOR_H

#include <QWidget>
#include <QUrl>
#include "cppGUI_global.h"


namespace Ui {
class MarkdownEditor  ;
}

//Editot for markdown (left) with HTML view (right)
class CPPGUISHARED_EXPORT MarkdownEditor
	: public QWidget
{
	Q_OBJECT

public:
	MarkdownEditor(QWidget *parent = nullptr);
	~MarkdownEditor();

	//Set tab stop width in pixels
	void setTabStopWidth(qreal tab_dist);
	//Set font
	void setFont(QFont font);
	//Set base folder for links
	void setBaseFolder(QString folder);

	//Returns if the file was modified
	bool isModified() const { return is_modified_;}
	//Returns the currently loaded filename (with path)
	QString file() const { return file_;}
	//Returns the base folder
	QString baseFolder() const { return base_folder_;}

public slots:
	//Load a file and optinally set base folder for links
	void loadFile(QString filename);
	//Store opened file
	void storeFile();
	//Clears the loaded file
	void clear();
	//Set strings to highlight in HTML view
	void setHighlightStrings(QStringList strings);
	//Toggle edit area
	void toggleEditArea();
	//Toggle if editing is enabled
	void toggleEditingEnabled();

signals:
	//Emitted when the file modification state changes
	void modificationStateChanged();

protected slots:
	void openExternalLink(QUrl url);
	void textChanged();
	void updateHTML();
	QString markdownToHtml(QString in, bool prescale_images);
	void askIfFileShouldBeStored();
	void contextMenuHtml(QPoint pos);

private:
	Ui::MarkdownEditor* ui_;
	QString file_; //canonical path
	QString file_folder_; //canonical path of the file folder
	QString base_folder_; //canonocal path of the base folder (links can be relative to this)
	bool is_modified_;
	QStringList highlight_;
};

#endif // MARKDOWNEDITOR_H
