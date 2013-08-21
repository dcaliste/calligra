/* This file is part of the KDE project
   Copyright (C) 2006-2007 Dag Andersen <danders@get2net.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KPlato_DateTimeTester_h
#define KPlato_DateTimeTester_h

#include <QtTest/QtTest>

#include "kptdatetime.h"
namespace QTest
{
    template<>
    char *toString(const KPlato::DateTime &dt)
    {
        QString s;
        switch ( dt.timeSpec() ) {
            case Qt::LocalTime: s = " LocalTime"; break;
            case Qt::UTC: s = " UTC"; break;
            case Qt::OffsetFromUTC: s = " OffsetFromUTC"; break;
        }
        return toString( QString( "%1T%2 %3" ).arg( dt.date().toString(Qt::ISODate) ).arg( dt.time().toString( "hh:mm:ss.zzz" ) ).arg( s ) );
    }
}

namespace KPlato
{

class DateTimeTester : public QObject
{
    Q_OBJECT
private slots:
    void subtractDay();
    void subtractHour();
    void subtractMinute();
    void subtractSecond();

#if QT_VERSION  >= 0x040700
    void subtractMillisecond();
#endif

    void addDay();
    void addHour();
    void addMinute();
    void addSecond();
    void addMillisecond();

};

} //namespace KPlato

#endif
