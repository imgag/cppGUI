#ifndef COLORNAMES_H
#define COLORNAMES_H

#include "cppGUI_global.h"
#include <QColor>
#include <QHash>
#include <QString>
#include <QStringList>

//Named colors for use in UI
class CPPGUISHARED_EXPORT ColorNames
{
public:
	//Returns the sorted list of all lower-case color names
	static QStringList names();
	//Returns a color by name (case-insensitive). Throws ArgumentException for unknown names.
	static QColor color(const QString& name);
	//Returns color string as RGBA usable in HTML/CSS. Throws ArgumentException for unknown names.
	static QString stringRGBA(const QString& name, int alpha);


protected:
	ColorNames() = delete;
	static const QHash<QString, QColor>& colorMap();
};

#endif