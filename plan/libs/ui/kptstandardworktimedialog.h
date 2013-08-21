/* This file is part of the KDE project
   Copyright (C) 2004 - 2007 Dag Andersen <danders@get2net.dk>

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
 * Boston, MA 02110-1301, USA.
*/

#ifndef KPTSTANDARDWORKTIMEDIALOG_H
#define KPTSTANDARDWORKTIMEDIALOG_H

#include "kplatoui_export.h"

#include "ui_standardworktimedialogbase.h"

#include "kptcalendar.h"

#include <kdialog.h>

#include <QWidget>


namespace KPlato
{

class Project;
class MacroCommand;

class StandardWorktimeDialogImpl : public QWidget, public Ui::StandardWorktimeDialogBase {
    Q_OBJECT
public:
    StandardWorktimeDialogImpl ( StandardWorktime *std, QWidget *parent);

    StandardWorktime *standardWorktime() { return m_std; }
    double inYear() const { return m_year; }
    double inMonth() const { return m_month; }
    double inWeek() const { return m_week; }
    double inDay() const { return m_day; }
    
private slots:
    void slotCheckAllFieldsFilled();
    void slotEnableButtonOk(bool on);

    void slotYearChanged(double);
    void slotMonthChanged(double);
    void slotWeekChanged(double);
    void slotDayChanged(double);

signals:
    void obligatedFieldsFilled(bool yes);
    void enableButtonOk(bool);

private:
    StandardWorktime *m_std;
    double m_year;
    double m_month;
    double m_week;
    double m_day;
};

class KPLATOUI_EXPORT StandardWorktimeDialog : public KDialog {
    Q_OBJECT
public:
    explicit StandardWorktimeDialog(Project &project, QWidget *parent=0);
    
    MacroCommand *buildCommand();

protected slots:
    void slotOk();
    
private:
    Project &project;
    StandardWorktimeDialogImpl *dia;
    StandardWorktime *m_original;
};

} //KPlato namespace

#endif // STANDARDWORKTIMEDIALOG_H
