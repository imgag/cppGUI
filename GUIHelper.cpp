#include "GUIHelper.h"
#include <QDialog>
#include <QApplication>
#include <QLabel>
#include <QDialogButtonBox>
#include <QBarSet>
#include <QBarSeries>
#include <QBarCategoryAxis>
#include <QValueAxis>
#include <QDebug>
#include <QMessageBox>
#include <QSortFilterProxyModel>
#include <QStringListModel>
#include <QLayout>
#include <QHeaderView>
#include <QScrollBar>
#include <QClipboard>
#include <QMimeData>
#include <QtCharts/QChartView>

QMainWindow* GUIHelper::mainWindow()
{
	QMainWindow* mw = nullptr;
	foreach(QWidget* widget, QApplication::topLevelWidgets())
	{
		if (!widget->isWindow()) continue;

		QMainWindow* tmp = qobject_cast<QMainWindow*>(widget);
		if (tmp!=nullptr) mw = tmp;
	}
	return mw;
}

void GUIHelper::showMessage(QString title, QString message, QMap<QString, QString> add_info)
{
	//create dialog
	QDialog* dialog = new QDialog(mainWindow());
	dialog->window()->setWindowTitle(title);
	QGridLayout* layout = new QGridLayout();
	dialog->window()->setLayout(layout);

	//add content
	int row = 0;
	layout->addWidget(new QLabel("message:"),row ,0, Qt::AlignLeft|Qt::AlignTop);
	layout->addWidget(new QLabel(message),row ,1);
	foreach(QString key, add_info.keys())
	{
		++row;
		layout->addWidget(new QLabel(key + ":"),row ,0, Qt::AlignLeft|Qt::AlignTop);
		layout->addWidget(new QLabel(add_info[key]),row ,1);
	}

	//show
	dialog->exec();
}

QSharedPointer<QDialog> GUIHelper::createDialog(QWidget* widget, QString title, QString label, bool add_buttons)
{
	QSharedPointer<QDialog> dialog = QSharedPointer<QDialog>(new QDialog(mainWindow()));
	dialog->setWindowFlags(Qt::Window);
	dialog->setWindowTitle(title);

	dialog->setLayout(new QBoxLayout(QBoxLayout::TopToBottom));
    dialog->layout()->setContentsMargins(3, 3, 3, 3);
	if (!label.isEmpty())
	{
		QLabel* label_widget = new QLabel(label);
		label_widget->setTextFormat(Qt::RichText);
		label_widget->setTextInteractionFlags(Qt::TextSelectableByMouse|Qt::LinksAccessibleByMouse);
		dialog->layout()->addWidget(label_widget);
	}
	dialog->layout()->addWidget(widget);

	//add buttons
	if (add_buttons)
	{
		QDialogButtonBox* button_box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
		dialog->connect(button_box, SIGNAL(accepted()), dialog.data(), SLOT(accept()));
		dialog->connect(button_box, SIGNAL(rejected()), dialog.data(), SLOT(reject()));
		dialog->layout()->addWidget(button_box);
	}

	return dialog;
}

void GUIHelper::styleSplitter(QSplitter* splitter)
{
	splitter->setHandleWidth(5);
	if (splitter->orientation()==Qt::Vertical)
	{
		splitter->setStyleSheet("QSplitter::handle:vertical { image: url(:/Icons/Splitter_vertical.png); } ");
	}
	else
	{
		splitter->setStyleSheet("QSplitter::handle:horizontal { image: url(:/Icons/Splitter_horizontal.png); } ");
	}
}

void GUIHelper::resizeTableCellWidths(QTableWidget* widget, int max_col_width, int max_used)
{
	widget->horizontalHeader()->setResizeContentsPrecision(max_used);
	widget->resizeColumnsToContents();

	//restrict width
	if (max_col_width>0)
	{
		for (int i=0; i<widget->columnCount(); ++i)
		{
			if (widget->columnWidth(i)>max_col_width)
			{
				widget->setColumnWidth(i, max_col_width);
			}
		}
	}
}

void GUIHelper::resizeTableCellHeightsToMinimum(QTableWidget* widget, int max_used)
{
	//determine min height
	int used = 0;
	int height = -1;
	for (int i=0; i<widget->rowCount(); ++i)
    {
		widget->resizeRowToContents(i);
		int row_height = widget->rowHeight(i);
		if (row_height>0)
		{
			if (height==-1) height = row_height;
			else height = std::min(row_height, height);

			++used;
		}

		if (used>=max_used) break;
	}

	//set row height
    if (height!=-1)
	{
        widget->verticalHeader()->setDefaultSectionSize(height);

		for (int i=0; i<widget->rowCount(); ++i)
		{
			widget->setRowHeight(i, height);
		}
	}
}

void GUIHelper::resizeTableCellHeightsToFirst(QTableWidget* widget, bool only_visible)
{
	//determine max height
	int height = -1;
	for (int i=0; i<widget->rowCount(); ++i)
	{
		if (only_visible && widget->isRowHidden(i)) continue;

		widget->resizeRowToContents(i);
		height = widget->rowHeight(i);
		break;
	}

	//set row height
	if (height!=-1)
    {
        widget->verticalHeader()->setDefaultSectionSize(height);

		for (int i=0; i<widget->rowCount(); ++i)
		{
			widget->setRowHeight(i, height);
		}
	}
}

void GUIHelper::resizeTableHeight(QTableWidget* widget)
{
	int height = 0;

	//rows
	for (int i = 0; i < widget->verticalHeader()->count(); ++i)
	{
		if (!widget->verticalHeader()->isSectionHidden(i))
		{
			height += widget->verticalHeader()->sectionSize(i);
		}
	}

	//scroll bar
	if (!widget->horizontalScrollBar()->isHidden())
	{
		height += widget->horizontalScrollBar()->height();
	}

	//header
	if (!widget->horizontalHeader()->isHidden())
	{
		height += widget->horizontalHeader()->height();
	}

	widget->setMinimumHeight(height);
}

QTableWidgetItem* GUIHelper::createTableItem(const QString& text, Qt::Alignment alignment, Qt::ItemFlags flags)
{
	QTableWidgetItem* item = new QTableWidgetItem(text);
	item->setTextAlignment(alignment);
	item->setFlags(flags);
	return item;
}

QTableWidgetItem* GUIHelper::createTableItem(int value, Qt::Alignment alignment, Qt::ItemFlags flags)
{
	QTableWidgetItem* item = new QTableWidgetItem;
	item->setData(Qt::EditRole, value);
	item->setTextAlignment(alignment);
	item->setFlags(flags);
	return item;
}

QTableWidgetItem* GUIHelper::createTableItem(double value, int prec, Qt::Alignment alignment, Qt::ItemFlags flags)
{
	QTableWidgetItem* item = new QTableWidgetItem;
	if (prec == -1)
	{
		item->setData(Qt::EditRole, value);
	}
	else
	{
		item->setData(Qt::EditRole, QString::number(value, 'f', prec).toDouble());
	}
	item->setTextAlignment(alignment);
	item->setFlags(flags);
	return item;
}

QTableWidgetItem* GUIHelper::createTableItem(const QByteArray& text, Qt::Alignment alignment, Qt::ItemFlags flags)
{
	return createTableItem(QString(text), alignment, flags);
}

QTableWidgetItem* GUIHelper::createTableItem(const char* text, Qt::Alignment alignment, Qt::ItemFlags flags)
{
	return createTableItem(QString(text), alignment, flags);
}

QLabel* GUIHelper::createLinkLabel(const QString& text, bool external_link)
{
	QLabel* label = new QLabel();

    label->setContentsMargins(2, 2, 2, 2);
	label->setText(text);
	label->setAlignment(Qt::AlignLeft|Qt::AlignTop);
	if (external_link) label->setOpenExternalLinks(true);

	return label;
}

QList<int> GUIHelper::selectedTableRows(const QTableWidget* table, bool skip_hidden)
{
	QList<int> output;

	QList<QTableWidgetSelectionRange> ranges = table->selectedRanges();
	foreach(const QTableWidgetSelectionRange& range, ranges)
	{
		for (int r=range.topRow(); r<=range.bottomRow(); ++r)
		{
			if (skip_hidden && table->isRowHidden(r)) continue;
			output << r;
		}
	}

	std::sort(output.begin(), output.end());

	return output;
}

QList<int> GUIHelper::selectedTableColumns(const QTableWidget* table, bool skip_hidden)
{
	QList<int> output;

	foreach(const QTableWidgetSelectionRange& range, table->selectedRanges())
	{
		for (int c=range.leftColumn(); c<=range.rightColumn(); ++c)
		{
			if (skip_hidden && table->isColumnHidden(c)) continue;
			output << c;
		}
	}

	std::sort(output.begin(), output.end());

	return output;
}

void GUIHelper::copyToClipboard(const QTableWidget* table, bool selected_rows_only, bool convert_numbers_to_locale, const QStringList& comments)
{
	//get selected rows
	QList<int> all_selected_rows = selectedTableRows(table);
	QSet<int> selected_rows = QSet<int>(all_selected_rows.begin(), all_selected_rows.end());

	//comments
	QString output;
	if (comments.size() > 0)
	{
		output += "##";
		output += comments.join("\n##");
		output += "\n";
	}

	//header
	output += "#";
	for (int col=0; col<table->columnCount(); ++col)
	{
		if (col!=0) output += '\t';
		output += table->horizontalHeaderItem(col)->text().replace('\t', ' ').replace('\n', ' ').replace('\r', ' ').trimmed();
	}
	output += '\n';

	//rows
	for (int row=0; row<table->rowCount(); ++row)
	{
		if (table->isRowHidden(row)) continue;

		if (selected_rows_only && !selected_rows.contains(row)) continue;

		for (int col=0; col<table->columnCount(); ++col)
		{
			if (col!=0) output += '\t';

			QString text = "";
			QTableWidgetItem* item  = table->item(row, col);
			if (item!=nullptr)
			{
				if (convert_numbers_to_locale)
				{
					QMetaType type = item->data(Qt::EditRole).metaType();
					if(type==QMetaType::fromType<int>())
					{
						text = QLocale::system().toString(item->data(Qt::EditRole).toInt());
					}
					else if(type==QMetaType::fromType<double>())
					{
						text = QLocale::system().toString(item->data(Qt::EditRole).toDouble());
					}
					else
					{
						text = item->text();
					}
				}
				else
				{
					text = item->text();
				}
			}
			else
			{
				QWidget* cell_widget = table->cellWidget(row, col);
				if (cell_widget!=nullptr)
				{
					if (cell_widget->inherits("QLabel"))
					{
						text = qobject_cast<QLabel*>(cell_widget)->text();
					}
					else
					{
						qDebug() << "Unhandled table item type " + QString(cell_widget->metaObject()->className()) + " in copyToClipboard!";
					}
				}
			}
			output += text.replace('\t', ' ').replace('\n', ' ').replace('\r', ' ').trimmed();
		}
		output += '\n';
	}

	QApplication::clipboard()->setText(output);
}

int GUIHelper::columnIndex(const QTableWidget* table, QString column, bool throw_if_not_found)
{
	for (int c=0; c<table->columnCount(); ++c)
	{
		if (table->horizontalHeaderItem(c)->text()==column)
		{
			return c;
		}
	}

	if (throw_if_not_found)	THROW(ArgumentException, "Could not find column with name '" + column + "' in table!");

	return -1;
}

QFrame* GUIHelper::horizontalLine()
{
	QFrame* line = new QFrame();
	line->setObjectName("line");
	line->setFrameShape(QFrame::HLine);
	line->setFrameShadow(QFrame::Sunken);
	line->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	line->setMinimumHeight(3);
	return line;
}

QChartView* GUIHelper::histogramChart(const Histogram& hist, QString title, int highlight_bin)
{
	QBarSet* set = new QBarSet(title);
	QBarSet* highlight_set = new QBarSet(title);
	highlight_set->setColor(QColor("red"));
	for(int bin=0; bin<hist.binCount(); ++bin)
	{
		if(bin == highlight_bin)
		{
			highlight_set->append(hist.binValue(bin, true));
			set->append(0.0);
			continue;
		}
		highlight_set->append(0.0);
		set->append(hist.binValue(bin, true));

	}

	QBarSeries* series = new QBarSeries();
	series->append(set);
	if (highlight_bin >= 0)	series->append(highlight_set);
	QChart* chart = new QChart();
	chart->addSeries(series);
	chart->legend()->setVisible(false);
	chart->createDefaultAxes();

	QValueAxis* y_axis = qobject_cast<QValueAxis*>(chart->axes(Qt::Vertical).at(0));
	y_axis->setTitleText("%");
	y_axis->setTickCount(8);

	QBarCategoryAxis* x_axis = qobject_cast<QBarCategoryAxis*>(chart->axes(Qt::Horizontal).at(0));
	QStringList categories;
	for(int bin=0; bin<hist.binCount(); ++bin)
	{
		double start = hist.startOfBin(bin);
		categories << QString::number(start, 'f', 2) + "-" + QString::number(start+hist.binSize(), 'f', 2);
	}
	x_axis->setCategories(categories);
	x_axis->setTitleText(title);
	x_axis->setLabelsAngle(90);

	QChartView* view = new QChartView(chart);
	view->setRenderHint(QPainter::Antialiasing);
	view->setMinimumSize(1000, 800);

	return view;
}

QCompleter* GUIHelper::completer(QObject* parent, const QStringList& items)
{
    QStringListModel *model = new QStringListModel(items, parent);

    QSortFilterProxyModel *proxy_model = new QSortFilterProxyModel(parent);
    proxy_model->setSourceModel(model);
    proxy_model->setFilterCaseSensitivity(Qt::CaseInsensitive);

    QCompleter *completer = new QCompleter(proxy_model, parent);
    completer->setCompletionMode(QCompleter::PopupCompletion);
    completer->setFilterMode(Qt::MatchContains);
    completer->setCompletionRole(Qt::DisplayRole);
	completer->setCaseSensitivity(Qt::CaseInsensitive);

	return completer;
}

QString GUIHelper::colorToQssFormat(const QColor& color)
{
	return QString("rgba(%1, %2, %3, %4)")
			.arg(color.red())
			.arg(color.green())
			.arg(color.blue())
			.arg(color.alpha());
}

void GUIHelper::showException(QWidget* parent, const Exception& e, QString title)
{
	//reset override cursor
	while(QApplication::overrideCursor()!=nullptr)
	{
		QApplication::restoreOverrideCursor();
	}

	//show dialog to user
	if (e.type()==ExceptionType::CRITIAL)
	{
		QMessageBox::critical(parent, title, "Critical error - this should probably not happen:\n" + e.message());
	}
	else if (e.type()==ExceptionType::WARNING)
	{
		QMessageBox::warning(parent, title, e.message());
	}
	else
	{
		QMessageBox::information(parent, title, e.message());
	}
}

