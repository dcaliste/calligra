/* This file is part of the KDE project
   Copyright (C) 2004 Dag Andersen <danders@get2net.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation;
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KPTMAP_H
#define KPTMAP_H

#include "kplatokernel_export.h"
#include "kptcalendar.h"

#include <QMap>
#include <QDateTime>
#include <QString>
#include <QPair>

#include <kdebug.h>
#include <kglobal.h>
#include <klocale.h>

namespace KPlato
{

typedef QMap<QString, int> DateMapType;
class KPLATOKERNEL_EXPORT DateMap : public DateMapType
{
public:
    DateMap() {}
    virtual ~DateMap() {}

    virtual bool contains(QDate date) const { return DateMapType::contains(date.toString(Qt::ISODate)); }

    void insert(QString date, int state=CalendarDay::NonWorking) {
        //kDebug()<<date<<"="<<state;
        if (state == CalendarDay::None)
            DateMapType::remove(date);
        else
            DateMapType::insert(date, state);
    }
    void insert(QDate date, int state=CalendarDay::NonWorking) { insert(date.toString(Qt::ISODate), state); }

    void remove(QDate date) {
        //kDebug()<<date.toString(Qt::ISODate);
        DateMapType::remove(date.toString(Qt::ISODate));
    }

    int state(QString date) {
        DateMapType::iterator it = find(date);
        if (it == end()) return 0;
        else return it.value();
    }
    int state(QDate date) { return state(date.toString(Qt::ISODate)); }

    bool operator==(const DateMap &m) const {
        return keys() == m.keys() && values() == m.values();
    }
    bool operator!=(const DateMap &m) const {
        return keys() != m.keys() || values() != m.values();
    }

    // boolean use
    void toggle(QString date, int state=CalendarDay::NonWorking) {
        //kDebug()<<date<<"="<<state;
        if (DateMapType::contains(date))
            DateMapType::remove(date);
        else
            DateMapType::insert(date, state);
    }
    void toggle(QDate date, int state=CalendarDay::NonWorking) { return toggle(date.toString(Qt::ISODate), state); }
    void toggleClear(QString date, int state=CalendarDay::NonWorking) {
        //kDebug()<<date<<"="<<state;
        bool s = DateMapType::contains(date);
        clear();
        if (!s) insert(date, state);
    }
    void toggleClear(QDate date, int state=CalendarDay::NonWorking) {
        toggleClear(date.toString(Qt::ISODate), state);
    }
};

typedef QMap<int, int> IntMapType;
class KPLATOKERNEL_EXPORT IntMap : public IntMapType
{
public:
    IntMap() {}
    virtual ~IntMap() {}

    void insert(int key, int state=CalendarDay::NonWorking) {
        if (state == CalendarDay::None)
            IntMapType::remove(key);
        else
            IntMapType::insert(key, state); }

    virtual int state(int key) {
        IntMapType::iterator it = IntMapType::find(key);
        if (it == IntMapType::end()) return 0;
        else return it.value();
    }

    bool operator==(const IntMap &m) const {
        return keys() == m.keys() && values() == m.values();
    }
    bool operator!=(const IntMap &m) const {
        return keys() != m.keys() || values() != m.values();
    }

    // boolean use
    void toggle(int key, int state=CalendarDay::NonWorking) {
        if ( IntMapType::contains(key) )
            remove(key);
        else
            insert(key, state);
    }
    void toggleClear(int key, int state=CalendarDay::NonWorking) {
        bool s =contains(key);
        clear();
        if (!s) insert(key, state);
    }
};

class KPLATOKERNEL_EXPORT WeekMap : public IntMap
{
public:
    bool contains(int week, int year) { return IntMap::contains(week*10000 + year); }
    bool contains(QPair<int,int> week) { return contains(week.first,  week.second); }

    void insert(int week, int year, int state=CalendarDay::NonWorking) {
        if (week < 1 || week > 53) { kError()<<"Illegal week number: "<<week<<endl; return; }
        IntMap::insert(week*10000 + year, state);
    }
    void insert(QPair<int,int> week, int state=CalendarDay::NonWorking) { insert(week.first, week.second, state); }

    void insert(WeekMap::iterator it, int state) { insert(week(it.key()), state); }

    void remove(QPair<int,int> week) { IntMap::remove(week.first*10000 + week.second); }

    static QPair<int, int> week(int key) { return QPair<int, int>(key/10000, key%10000); }

    using IntMap::state;
    int state(QPair<int, int> week) { return IntMap::state(week.first*10000 + week.second); }
    int state(int week, int year) { return state(QPair<int, int>(week, year)); }

    void toggle(QPair<int,int> week, int state=CalendarDay::NonWorking) {
        if (week.first < 1 || week.first > 53) { kError()<<"Illegal week number: "<<week.first<<endl; return; }
        IntMap::toggle(week.first*10000 + week.second, state);
    }
    void toggleClear(QPair<int,int> week, int state=CalendarDay::NonWorking) {
        if (week.first < 1 || week.first > 53) { kError()<<"Illegal week number: "<<week.first<<endl; return; }
        IntMap::toggleClear(week.first*10000 + week.second, state);
    }
};

}  //KPlato namespace

#endif
