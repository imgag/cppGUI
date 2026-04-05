#include "MarkdownEditor.h"
#include "ui_MarkdownEditor.h"
#include "Helper.h"
#include <QDesktopServices>
#include <QFileInfo>
#include <QMessageBox>
#include <QScrollBar>
#include <QUrl>

MarkdownEditor::MarkdownEditor(QWidget* parent)
	: QWidget(parent)
	, ui_(new Ui::MarkdownEditor())
{
	ui_->setupUi(this);
	connect(ui_->html, SIGNAL(anchorClicked(QUrl)), this, SLOT(openExternalLink(QUrl)));
	connect(ui_->plain, SIGNAL(textChanged()), this, SLOT(textChanged()));
}

MarkdownEditor::~MarkdownEditor()
{
	delete ui_;
}

void MarkdownEditor::loadFile(QString filename)
{
	askIfFileShouldBeStored();

	QFileInfo info(filename);
	if (!info.exists())
	{
		clear();
	}

	//store file name and paths
	file_ = info.canonicalFilePath();
	file_folder_ = info.canonicalPath() + "/";

	//load file data
	ui_->plain->setPlainText(Helper::fileText(file_));
	ui_->plain->setEnabled(true);
	is_modified_ = false;
}

void MarkdownEditor::storeFile()
{
	Helper::storeTextFile(file_, ui_->plain->toPlainText().split("\n"));
}

void MarkdownEditor::clear()
{
	askIfFileShouldBeStored();

	file_.clear();
	file_folder_.clear();
	is_modified_ = false;

	ui_->plain->clear();
	ui_->plain->setEnabled(false);
	ui_->html->clear();
}

void MarkdownEditor::setHighlightStrings(QStringList strings)
{
	highlight_ = strings;

	updateHTML();
}

void MarkdownEditor::toggleEditArea()
{
	ui_->plain->setVisible(!ui_->plain->isVisible());
}

void MarkdownEditor::toggleEditingEnabled()
{
	ui_->plain->setReadOnly(!ui_->plain->isReadOnly());
}

void MarkdownEditor::setTabStopWidth(qreal tab_dist)
{
	ui_->plain->setTabStopDistance(tab_dist);
}

void MarkdownEditor::setFont(QFont font)
{
	ui_->plain->setFont(font);
}

void MarkdownEditor::setBaseFolder(QString folder)
{
	folder = folder.trimmed();
	if (folder.isEmpty())
	{
		base_folder_.clear();
	}
	else
	{
		base_folder_ = QFileInfo(folder).canonicalFilePath() + "/";
	}
}

void MarkdownEditor::openExternalLink(QUrl url)
{
	QString url_str = url.toString().trimmed();

	//web link
	if (url_str.startsWith("http://") || url_str.startsWith("https://"))
	{
		QDesktopServices::openUrl(url);
		return;
	}

	//link relative to notes folder
	if (file_folder_.startsWith(base_folder_) && QFile::exists(base_folder_ + url_str))
	{
		loadFile(base_folder_+ url_str);
		return;
	}

	//link relative to current file
	if (QFile::exists(file_folder_ + url_str))
	{
		loadFile(file_folder_ + url_str);
		return;
	}

	QMessageBox::warning(this, "Link error", "Could not open link to file: " + url_str);
}

void MarkdownEditor::textChanged()
{
	if (!is_modified_) emit modificationStateChanged();
	is_modified_ = true;

	updateHTML();
}

void MarkdownEditor::updateHTML()
{
	QString text = markdownToHtml(ui_->plain->toPlainText());

	//highlight search terms
	const QString start_tag = "<span style=\"background-color: yellow;\">";
	const QString end_tag = "</span>";
	int index = -1;
	foreach(const QString& term, highlight_)
	{
		int from_index = 0;
		while((index = text.indexOf(term, from_index, Qt::CaseInsensitive))!=-1)
		{
			text.insert(index, start_tag);
			text.insert(index+start_tag.length()+term.length(), end_tag);
			from_index =index+start_tag.length()+term.length()+end_tag.length();
		}
	}

	//store scrollbar position
	int scroll_pos = ui_->html->verticalScrollBar()->value();

	//update
	ui_->html->setSearchPaths(QStringList() << file_folder_);
	ui_->html->setHtml(text);

	//restore scrollbar position
	ui_->html->verticalScrollBar()->setValue(scroll_pos);
}

struct ElementPos {
	QString tag;
	qsizetype start;
	qsizetype end;
};

QList<ElementPos> findHtmlElements(QString html, QStringList tags)
{
	QList<ElementPos> result;
	for (const QString& tag : tags)
	{
		int pos = 0;
		while (pos < html.size()) {
			int startTag = html.indexOf("<" + tag, pos, Qt::CaseInsensitive);
			if (startTag == -1)
				break;

			int endStartTag = html.indexOf(">", startTag);
			if (endStartTag == -1)
				break;

			int endTag = html.indexOf("</" + tag + ">", endStartTag, Qt::CaseInsensitive);
			if (endTag == -1)
				break;

			result.append({tag, startTag, endTag + tag.length() + 3});
			// +3 for "</" and ">"
			pos = endTag + tag.length() + 3;
		}
	}
	return result;
}

QString MarkdownEditor::markdownToHtml(QString in)
{
	if(in.size()==0) return "";

	//convert to HTML
	QTextDocument doc;
	doc.setMarkdown(in);
	QString html = doc.toHtml();

	//style html
	QList<ElementPos> elements = findHtmlElements(html, QStringList{"h1", "h2", "h3", "td"});
	for (int i=elements.count()-1; i>=0; --i) //reverse order to make the positions correct
	{
		const ElementPos& e = elements[i];
		QString text = html.mid(e.start, e.end - e.start);
		if (e.tag=="h1")
		{
			text = text.replace("margin-top:0px;", "margin-top:20px;");
			text = text.replace("margin-bottom:0px;", "margin-bottom:5px;");
		}
		else if (e.tag=="h2")
		{
			text = text.replace("margin-top:0px;", "text-decoration:underline; margin-top:10px;");
			text = text.replace("margin-bottom:0px;", "margin-bottom:5px;");
		}
		else if (e.tag=="h3")
		{
			text = text.replace("margin-top:0px;", "margin-top:10px;");
			text = text.replace("margin-bottom:0px;", "margin-bottom:5px;");
		}
		else if (e.tag=="td")
		{
			text = text.replace("<td>", "<td style=\"border: 1px solid #aaa;\">");
		}
		html.replace(e.start, e.end-e.start, text);
	}
	//Helper::storeTextFile("C:\\Users\\sturm\\Desktop\\test.html", QStringList() << html);
	return html;
}

void MarkdownEditor::askIfFileShouldBeStored()
{
	if (file_.isEmpty() || !is_modified_) return;

	QMessageBox box(this);
	box.setWindowTitle(QApplication::applicationName());
	box.setText("Save changes to '" + file_ + "'?");
	box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	box.setDefaultButton(QMessageBox::No);
	if (box.exec() == QMessageBox::Yes)
	{
		storeFile();
	}
}
