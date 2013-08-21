/* This file is part of the KDE project
 * Copyright (C) 2007 Thomas Zander <zander@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#ifndef PARAGRAPHSETTINGSDIALOG_H
#define PARAGRAPHSETTINGSDIALOG_H

#include <KoUnit.h>
#include <KoTextEditor.h>

#include <kdialog.h>

class TextTool;
class ParagraphGeneral;
class KoImageCollection;

/// A dialog to show the settings for a paragraph
class ParagraphSettingsDialog : public KDialog
{
    Q_OBJECT
public:
    explicit ParagraphSettingsDialog(TextTool *tool, KoTextEditor *editor, QWidget* parent = 0);
    ~ParagraphSettingsDialog();

    void setUnit(const KoUnit &unit);

    void setImageCollection(KoImageCollection *imageCollection);

protected slots:
    void styleChanged(bool state = true);

    void slotApply();
    void slotOk();

private:
    void initTabs();

    ParagraphGeneral *m_paragraphGeneral;
    TextTool *m_tool;
    KoTextEditor *m_editor;
    bool m_uniqueFormat;
    bool m_styleChanged;
};

#endif
