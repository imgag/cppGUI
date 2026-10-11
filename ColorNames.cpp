#include "ColorNames.h"
#include "Exceptions.h"
#include <QHash>

const QHash<QString, QColor> &ColorNames::colorMap()
{
	static const QHash<QString, QColor> colors = {
		{ "black",       QColor::fromRgb(0x222222) },
		{ "blue",        QColor::fromRgb(0x4285F4) },
		{ "brown",       QColor::fromRgb(0x795548) },
		{ "dark blue",   QColor::fromRgb(0x174EA6) },
		{ "dark gray",   QColor::fromRgb(0x424242) },
		{ "dark green",  QColor::fromRgb(0x137333) },
		{ "dark red",    QColor::fromRgb(0xA61C24) },
		{ "gray",        QColor::fromRgb(0x9E9E9E) },
		{ "green",       QColor::fromRgb(0x34A853) },
		{ "light blue",  QColor::fromRgb(0x90CAF9) },
		{ "light gray",  QColor::fromRgb(0xE0E0E0) },
		{ "light green", QColor::fromRgb(0xA5D6A7) },
		{ "light red",   QColor::fromRgb(0xF28B82) },
		{ "orange",      QColor::fromRgb(0xFB8C00) },
		{ "pink",        QColor::fromRgb(0xF48FB1) },
		{ "purple",      QColor::fromRgb(0xAB47BC) },
		{ "red",         QColor::fromRgb(0xE53935) },
		{ "white",       QColor::fromRgb(0xFFFFFF) },
		{ "yellow",      QColor::fromRgb(0xFDD835) }
	};
	return colors;
}

QColor ColorNames::color(const QString &name)
{
	const auto& colors = colorMap();
	auto it = colors.constFind(name.toLower());
	if (it == colors.cend()) THROW(ArgumentException, "Unknown color name '" + name + "'!");
	return it.value();
}

QStringList ColorNames::names()
{
	QStringList result = colorMap().keys();
	result.sort(Qt::CaseInsensitive);
	return result;
}
