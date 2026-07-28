#ifndef GUIHELPER_H
#define GUIHELPER_H

#include "cppGUI_global.h"
#include "Histogram.h"
#include "Exceptions.h"
#include <QString>
#include <QMap>
#include <QWidget>
#include <QSplitter>
#include <QTableWidget>
#include <QCompleter>
#include <QMainWindow>
#include <QLabel>
#include <QtCharts/QChartView>

///Auxilary helper functions for the GUI.
class CPPGUISHARED_EXPORT GUIHelper
{
public:
	///Returns the main window of the application;
	static QMainWindow* mainWindow();

	///Shows a message dialog with title, message and additional information.
	static void showMessage(QString title, QString message, QMap<QString, QString> add_info=QMap<QString, QString>());

	///Creates a dialog from a widget. The dialog takes ownership of the widget.
	static QSharedPointer<QDialog> createDialog(QWidget* widget, QString title, QString label="", bool add_buttons=false);

	///Custom splitter styler
	static void styleSplitter(QSplitter* splitter);

	///Resize column width and hight
	static void resizeTableCellWidths(QTableWidget* widget, int max_col_width=-1, int max_used=200);
	static void resizeTableCellHeightsToMinimum(QTableWidget* widget, int max_used=200);
	static void resizeTableCellHeightsToFirst(QTableWidget* widget, bool only_visible=true);
	///Resizes the table to height to show all rows
	static void resizeTableHeight(QTableWidget* widget);

	///Creates a table cell item
	static QTableWidgetItem* createTableItem(const QString& text, Qt::Alignment alignment=Qt::AlignLeft|Qt::AlignTop, Qt::ItemFlags flags =(Qt::ItemIsSelectable|Qt::ItemIsUserCheckable|Qt::ItemIsEnabled));
	static QTableWidgetItem* createTableItem(int value, Qt::Alignment alignment=Qt::AlignRight|Qt::AlignTop, Qt::ItemFlags flags =(Qt::ItemIsSelectable|Qt::ItemIsUserCheckable|Qt::ItemIsEnabled));
	///use prec = -1 to prevend rounding
	static QTableWidgetItem* createTableItem(double value, int prec = 6, Qt::Alignment alignment=Qt::AlignRight|Qt::AlignTop, Qt::ItemFlags flags =(Qt::ItemIsSelectable|Qt::ItemIsUserCheckable|Qt::ItemIsEnabled));
	static QTableWidgetItem* createTableItem(const QByteArray& text, Qt::Alignment alignment=Qt::AlignLeft|Qt::AlignTop, Qt::ItemFlags flags =(Qt::ItemIsSelectable|Qt::ItemIsUserCheckable|Qt::ItemIsEnabled));
	static QTableWidgetItem* createTableItem(const char* text, Qt::Alignment alignment=Qt::AlignLeft|Qt::AlignTop, Qt::ItemFlags flags =(Qt::ItemIsSelectable|Qt::ItemIsUserCheckable|Qt::ItemIsEnabled));


	///Creates a label that can open links in the text.
	static QLabel* createLinkLabel(const QString& text, bool external_link=true);

	///Returns the selected rows of a table (sorted ascending)
	static QList<int> selectedTableRows(const QTableWidget* table, bool skip_hidden=true);

	///Returns the selected columns of a table (sorted ascending)
	static QList<int> selectedTableColumns(const QTableWidget* table, bool skip_hidden=true);

	///Copy all content of a table widget to the clipboard
	static void copyToClipboard(const QTableWidget* table, bool selected_rows_only=false, bool convert_numbers_to_locale=false, const QStringList& comments = QStringList());

	///Returns the index of the given column. If the column is not found, a ArgumentException is thrown or -1 is returned (if throw_if_not_found is set to false).
	static int columnIndex(const QTableWidget* table, QString column, bool throw_if_not_found=true);

	///Creates a horizontal line
	static QFrame* horizontalLine();

    ///Creates a chart with a histogram    
    static QChartView* histogramChart(const Histogram& hist, QString title, int highlight_bin=-1);   

	///Creates a list-based auto-completer
	static QCompleter* completer(QObject* parent, const QStringList& items);

	///Returns a color in Qt StyleSheet format
	static QString colorToQssFormat(const QColor &color);

	///Shows an error message to the user.
	///Depending on the ExceptionType, a critical, warning or notice dialog is shown.
	///If a override cursor is set, it is removed before showing the dialog.
	static void showException(QWidget* parent, const Exception& e, QString title);

protected:
	///Constructor declared away.
	GUIHelper() = delete;
};

#endif
