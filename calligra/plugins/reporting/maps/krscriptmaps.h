/*
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)
 * Copyright (C) 2011 by Radoslaw Wicik (rockford@wicik.pl)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef SCRIPTINGKRSCRIPTMAPS_H
#define SCRIPTINGKRSCRIPTMAPS_H

#include <QObject>
#include <QPointF>
#include <QSizeF>

class KoReportItemMaps;

namespace Scripting
{

/**
 @author Adam Pigg <adam@piggz.co.uk>
 @author Radoslaw Wicik <rockford@wicik.pl>
*/
class Maps : public QObject
{
    Q_OBJECT
public:
    explicit Maps(KoReportItemMaps *);

    ~Maps();
public slots:


    /**
    * Get the position of the map
    * @return position in points
     */
    QPointF position();


    /**
     * Sets the position of the map in points
     * @param Position
     */
    void setPosition(const QPointF&);

    /**
     * Get the size of the map
     * @return size in points
     */
    QSizeF size();

    /**
     * Set the size of the map in points
     * @param Size
     */
    void setSize(const QSizeF&);

    /**
     * Get the resize mode for the image
     * @return resizeMode Clip or Stretch
     */
    QString resizeMode();

    /**
     * Sets the resize mode for the image
     * @param ResizeMode "Stretch" or "Clip" default is to clip
     */
    void setResizeMode(const QString &);

    /**
     * Sets the data for the static image
     * the data should be base64 encoded
     * @param RawImageData
     */
    void setInlineImage(const QByteArray&);

    /**
     * Get the data from a file (expected to be an image)
     * the returned data will be base64 encoded
     * @param Path location of file
     * @return File data enoded in base64
     */
    void loadFromFile(const QVariant &);
private:
    KoReportItemMaps *m_map;

};

}

#endif //SCRIPTINGKRSCRIPTMAPS_H
