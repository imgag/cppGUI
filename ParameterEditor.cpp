#include "ParameterEditor.h"
#include "ui_ParameterEditor.h"
#include "Exceptions.h"
#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QSignalBlocker>
#include <QVBoxLayout>
#include <limits>

ParameterEditor::ParameterEditor(ParameterList& parameters, QWidget* parent)
	: QWidget(parent)
	, ui_(new Ui::ParameterEditor())
{
	ui_->setupUi(this);
	connect(ui_->apply, &QPushButton::clicked, this, [this]() { store(); });
	connect(ui_->reset, &QPushButton::clicked, this, &ParameterEditor::reset);
	connect(ui_->auto_apply, &QCheckBox::toggled, this, [this]() { edited(); });
	load(parameters);
}

ParameterEditor::~ParameterEditor()
{
	delete ui_;
}

void ParameterEditor::load(ParameterList& parameters)
{
	updating_ = true;
	if (parameters_) disconnect(parameters_, nullptr, this, nullptr);
	parameters_ = &parameters;
	fields_.clear();
	while (ui_->form->rowCount() > 0) ui_->form->removeRow(0);
	for (const Parameter& parameter : parameters.parameters())
	{
		QWidget* editor = createEditor(parameter);
		editor->setObjectName(QString::fromUtf8(parameter.name()));
		QString help = parameter.description();
		for (auto it = parameter.constraints().cbegin(); it != parameter.constraints().cend(); ++it)
		{
			help += "\n" + QString::fromUtf8(Parameter::toString(it.key())) + ": " + it.value().toString();
		}
		editor->setToolTip(help.trimmed());
		auto* row = new QWidget;
		auto* row_layout = new QVBoxLayout(row);
		row_layout->setContentsMargins(0, 0, 0, 0);
		row_layout->addWidget(editor);
		auto* error = new QLabel(row);
		error->setWordWrap(true);
		error->setStyleSheet("color: #b00020;");
		row_layout->addWidget(error);
		auto* label = new QLabel(QString::fromUtf8(parameter.name()), row);
		label->setToolTip(help.trimmed());
		label->setBuddy(editor);
		ui_->form->addRow(label, row);
		fields_.append({parameter, editor, error, parameter.value()});
		setEditorValue(fields_.last(), parameter.value());
	}
	connect(&parameters, &ParameterList::constraintsChanged, this, [this]()
	{
		if (parameters_) load(*parameters_);
	});
	connect(&parameters, &QObject::destroyed, this, [this]() { setEnabled(false); });
	setEnabled(true);
	updating_ = false;
	bool all_valid;
	validValues(all_valid);
	ui_->apply->setEnabled(all_valid);
}

QWidget* ParameterEditor::createEditor(const Parameter& parameter)
{
	if (parameter.type() == ParameterType::BOOL)
	{
		auto* editor = new QCheckBox;
		connect(editor, &QCheckBox::toggled, this, [this]() { edited(); });
		return editor;
	}
	if (parameter.type() == ParameterType::STRING && parameter.constraints().contains(ConstraintType::ALLOWED_VALUES))
	{
		auto* editor = new QComboBox;
		for (const QByteArray& value : parameter.constraints().value(ConstraintType::ALLOWED_VALUES).toByteArray().split('\t'))
		{
			if (value.isEmpty() && parameter.constraints().contains(ConstraintType::NON_EMPTY)) continue;
			editor->addItem(QString::fromUtf8(value), value);
		}
		connect(editor, &QComboBox::currentIndexChanged, this, [this]() { edited(); });
		return editor;
	}
	if (parameter.type() == ParameterType::STRINGLIST)
	{
		auto* editor = new QWidget;
		auto* layout = new QVBoxLayout(editor);
		layout->setContentsMargins(0, 0, 0, 0);
		auto* entries = new QListWidget(editor);
		entries->setObjectName("entries");
		entries->setSelectionMode(QAbstractItemView::ExtendedSelection);
		layout->addWidget(entries);
		auto* actions = new QHBoxLayout;
		auto* add = new QPushButton("Add", editor);
		auto* remove = new QPushButton("Remove selected", editor);
		actions->addWidget(add);
		actions->addWidget(remove);
		actions->addStretch();
		layout->addLayout(actions);
		connect(entries, &QListWidget::itemChanged, this, [this]() { edited(); });
		connect(add, &QPushButton::clicked, this, [this, entries]()
		{
			auto* item = new QListWidgetItem;
			item->setFlags(item->flags() | Qt::ItemIsEditable);
			entries->addItem(item);
			entries->setCurrentItem(item);
			entries->editItem(item);
			edited();
		});
		connect(remove, &QPushButton::clicked, this, [this, entries]()
		{
			for (auto* item : entries->selectedItems()) delete entries->takeItem(entries->row(item));
			edited();
		});
		return editor;
	}
	//Text inputs preserve double precision and allow intermediate invalid input.
	auto* editor = new QLineEdit;
	connect(editor, &QLineEdit::textChanged, this, [this]() { edited(); });
	return editor;
}

QVariant ParameterEditor::editorValue(const Field& field) const
{
	if (auto* editor = qobject_cast<QCheckBox*>(field.editor)) return editor->isChecked();
	if (auto* editor = qobject_cast<QComboBox*>(field.editor)) return editor->currentData();
	if (field.parameter.type() == ParameterType::STRINGLIST)
	{
		QByteArrayList values;
		auto* entries = field.editor->findChild<QListWidget*>("entries");
		for (int i = 0; i < entries->count(); ++i) values.append(entries->item(i)->text().toUtf8());
		return QVariant::fromValue(values);
	}
	const QString text = qobject_cast<QLineEdit*>(field.editor)->text();
	if (field.parameter.type() == ParameterType::STRING) return text.toUtf8();
	return text;
}

void ParameterEditor::setEditorValue(const Field& field, const QVariant& value)
{
	if (auto* editor = qobject_cast<QCheckBox*>(field.editor)) editor->setChecked(value.toBool());
	else if (auto* editor = qobject_cast<QComboBox*>(field.editor)) editor->setCurrentIndex(editor->findData(value));
	else if (field.parameter.type() == ParameterType::STRINGLIST)
	{
		auto* entries = field.editor->findChild<QListWidget*>("entries");
		entries->clear();
		for (const QByteArray& value : value.value<QByteArrayList>())
		{
			auto* item = new QListWidgetItem(QString::fromUtf8(value));
			item->setFlags(item->flags() | Qt::ItemIsEditable);
			entries->addItem(item);
		}
	}
	else
	{
		QString text = field.parameter.type() == ParameterType::DOUBLE
			? QString::number(value.toDouble(), 'g', std::numeric_limits<double>::max_digits10) : value.toString();
		qobject_cast<QLineEdit*>(field.editor)->setText(text);
	}
}

QHash<QByteArray, QVariant> ParameterEditor::validValues(bool& all_valid)
{
	QHash<QByteArray, QVariant> values;
	all_valid = !parameters_.isNull();
	if (!parameters_) return values;
	for (const Field& field : fields_)
	{
		QString error;
		try
		{
			//Use the same validation and normalization as the destination list.
			const Parameter& parameter = parameters_->parameter(field.parameter.name());
			ParameterList validated("editor", {parameter}, {{parameter.name(), editorValue(field)}});
			values.insert(parameter.name(), validated.parameter(parameter.name()).value());
		}
		catch (const Exception& e)
		{
			error = e.message();
			all_valid = false;
		}
		field.error->setText(error);
		field.error->setVisible(!error.isEmpty());
	}
	return values;
}

bool ParameterEditor::applyValues(bool require_all_valid)
{
	bool all_valid;
	const auto values = validValues(all_valid);
	ui_->apply->setEnabled(all_valid);
	if (!parameters_ || (require_all_valid && !all_valid)) return false;
	bool changed = false;
	{
		//Notify observers once, after all valid values have been updated.
		QSignalBlocker blocker(parameters_);
		for (const Field& field : fields_)
		{
			const QByteArray& name = field.parameter.name();
			if (!values.contains(name) || parameters_->parameter(name).value() == values.value(name)) continue;
			const QVariant value = values.value(name);
			switch (field.parameter.type())
			{
				case ParameterType::INT: parameters_->setInt(name, value.toInt()); break;
				case ParameterType::DOUBLE: parameters_->setDouble(name, value.toDouble()); break;
				case ParameterType::BOOL: parameters_->setBool(name, value.toBool()); break;
				case ParameterType::STRING: parameters_->setString(name, value.toByteArray()); break;
				case ParameterType::STRINGLIST: parameters_->setStringList(name, value.value<QByteArrayList>()); break;
			}
			changed = true;
		}
	}
	if (changed)
	{
		emit parameters_->parameterChanged();
		emit settingsApplied();
	}
	return all_valid;
}

bool ParameterEditor::store()
{
	return applyValues(true);
}

void ParameterEditor::reset()
{
	updating_ = true;
	for (const Field& field : fields_) setEditorValue(field, field.loaded_value);
	updating_ = false;
	edited();
}

void ParameterEditor::edited()
{
	if (updating_ || !parameters_) return;
	if (ui_->auto_apply->isChecked()) applyValues(false);
	else
	{
		bool all_valid;
		validValues(all_valid);
		ui_->apply->setEnabled(all_valid);
	}
}
