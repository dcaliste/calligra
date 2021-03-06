/*
 * OpenRPT report writer and rendering engine
 * Copyright (C) 2001-2007 by OpenMFG, LLC (info@openmfg.com)
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)
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

#ifndef REPORTSECTIONDETAIL_H
#define REPORTSECTIONDETAIL_H

#include <QWidget>
#include "KoReportDesigner.h"

#include "koreport_export.h"

class QDomNode;
class ReportSectionDetailGroup;
/**
 @author
*/
class KOREPORT_EXPORT ReportSectionDetail : public QWidget
{
    Q_OBJECT
public:
    explicit ReportSectionDetail(KoReportDesigner * rptdes);
    virtual ~ReportSectionDetail();

    enum PageBreak {
        BreakNone = 0,
        BreakAtEnd = 1
    };

    void setPageBreak(int);
    int pageBreak() const;

    ReportSection * detailSection() const;

    void buildXML(QDomDocument & doc, QDomElement & section);
    void initFromXML(QDomNode & node);

    KoReportDesigner * reportDesigner() const;

    int groupSectionCount() const;
    ReportSectionDetailGroup * groupSection(int i) const;
    void insertGroupSection(int idx, ReportSectionDetailGroup * rsd);
    int indexOfGroupSection(const QString & column) const;
    void removeGroupSection(int idx, bool del = false);
    virtual QSize sizeHint() const;

    void setSectionCursor(const QCursor&);
    void unsetSectionCursor();
    
protected:
    QString m_query;

    QString m_name;
    ReportSection * m_detail;
    KoReportDesigner * m_reportDesigner;

    QList<ReportSectionDetailGroup*> groupList;

    QVBoxLayout * m_vboxlayout;

    int m_pageBreak;
};

#endif
