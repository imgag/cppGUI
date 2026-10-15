#ifndef PARAMETEREDITOR_H
#define PARAMETEREDITOR_H

#include "cppGUI_global.h"
#include "ParameterList.h"
#include <QPointer>
#include <QWidget>

class QLabel;

namespace Ui {
class ParameterEditor;
}

//Edits a referenced ParameterList. Loading captures the values used by Reset.
class CPPGUISHARED_EXPORT ParameterEditor : public QWidget
{
	Q_OBJECT

public:
	ParameterEditor(ParameterList& parameters, QWidget* parent = nullptr);
	~ParameterEditor() override;
	//Disable and hide auto-apply checkbox
	void disableAutoApply();
	//Sets the name of the parameter owner. An empty title hides it.
	void setName(const QString& title);
	//Binds the list and rebuilds the controls. Also called when its constraints change.
	void load(ParameterList& parameters);
	//Stores all fields if valid; returns false without changing the list otherwise.
	bool store();

public slots:
	//Restores the last loaded values. Auto-apply also writes valid restored values.
	void reset();

signals:
	//Emitted after one or more parameter values have been applied.
	void settingsApplied();

private:
	struct Field
	{
		Parameter parameter;
		QWidget* editor;
		QLabel* error;
		QVariant loaded_value;
		QVariant displayed_value;
		QString displayed_text;
	};

	QPointer<ParameterList> parameters_;
	QList<Field> fields_;
	Ui::ParameterEditor* ui_;
	bool updating_ = false;
	bool applying_ = false;

	QWidget* createEditor(const Parameter& parameter);
	QVariant editorValue(const Field& field) const;
	void setEditorValue(Field& field, const QVariant& value);
	QHash<QByteArray, QVariant> validValues(bool& all_valid);
	bool applyValues(bool require_all_valid);
	void edited();
};

#endif // PARAMETEREDITOR_H
