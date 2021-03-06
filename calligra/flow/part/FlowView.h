/* This file is part of the KDE project
   Copyright (C)  2006 Peter Simonsson <peter.simonsson@gmail.com>

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

#ifndef FLOWVIEW_H
#define FLOWVIEW_H

#include <KoPAView.h>

class QAction;

class FlowDocument;
class FlowPart;

class FlowView : public KoPAView
{
  Q_OBJECT

  public:
    FlowView(FlowPart *part, FlowDocument *document, QWidget *parent);
    ~FlowView();

    /// Returns the document
    FlowDocument* document() const;

  protected slots:
    /// Called when the doc emits updateGui
    void updateGui();

  protected:
    /// Creates and initializes the GUI.
    void initializeGUI();
    /// Initializes all the actions
    void initializeActions();

  private:
    FlowDocument* m_document;
};

#endif
