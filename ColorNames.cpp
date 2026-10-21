#include "ColorNames.h"
#include "Exceptions.h"
#include <QHash>

const QHash<QString, QColor> &ColorNames::colorMap()
{
	static const QHash<QString, QColor> colors = {
		{ "black",       QColor::fromRgb(0x000000) },
		{ "blue",        QColor::fromRgb(0x2B55FF) },
		{ "brown",       QColor::fromRgb(0x795548) },
		{ "dark blue",   QColor::fromRgb(0x001EB7) },
		{ "dark gray",   QColor::fromRgb(0x424242) },
		{ "dark green",  QColor::fromRgb(0x137333) },
		{ "dark red",    QColor::fromRgb(0xA61C24) },
		{ "gray",        QColor::fromRgb(0x9E9E9E) },
		{ "green",       QColor::fromRgb(0x00FF00) },
		{ "light blue",  QColor::fromRgb(0x75BEFF) },
		{ "light gray",  QColor::fromRgb(0xE0E0E0) },
		{ "light green", QColor::fromRgb(0xA5D6A7) },
		{ "light red",   QColor::fromRgb(0xFF5656) },
		{ "orange",      QColor::fromRgb(0xFF6600) },
		{ "pink",        QColor::fromRgb(0xF48FB1) },
		{ "purple",      QColor::fromRgb(0xAB47BC) },
		{ "red",         QColor::fromRgb(0xFF0000) },
		{ "white",       QColor::fromRgb(0xFFFFFF) },
		{ "yellow",      QColor::fromRgb(0xFFDF0F) }
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

QString ColorNames::stringRGBA(const QString &name, int alpha)
{
	QColor c = color(name);
	c.setAlpha(alpha);
	return QString("rgba(%1, %2, %3, %4)").arg(c.red()).arg(c.green()).arg(c.blue()).arg(c.alphaF());
}

QStringList ColorNames::names()
{
	QStringList result = colorMap().keys();
	result.sort(Qt::CaseInsensitive);
	return result;
}
