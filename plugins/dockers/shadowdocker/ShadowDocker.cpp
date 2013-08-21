/* This file is part of the KDE project
 * Copyright (C) 2008 Jan Hambrecht <jaham@gmx.net>
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

#include "ShadowDocker.h"
#include <KoShapeShadow.h>
#include <KoCanvasBase.h>
#include <KoShapeManager.h>
#include <KoSelection.h>
#include <KoToolManager.h>
#include <KoCanvasController.h>
#include <KoShadowConfigWidget.h>
#include <KoShapeShadowCommand.h>
#include <klocale.h>
#include <QSpacerItem>
#include <QGridLayout>

class ShadowDocker::Private
{
public:
    Private()
    : widget(0), canvas(0)
    {}
    KoShadowConfigWidget * widget;
    KoCanvasBase * canvas;
    QSpacerItem *spacer;
    QGridLayout *layout;
};

ShadowDocker::ShadowDocker()
: d( new Private() )
{
    setWindowTitle( i18n( "Shadow Properties" ) );

    QWidget * mainWidget = new QWidget(this);
    d->layout = new QGridLayout(mainWidget);

    d->widget = new KoShadowConfigWidget(mainWidget);
    d->widget->setEnabled( false );
    d->layout->addWidget(d->widget, 0, 0);

    d->spacer = new QSpacerItem(0, 0, QSizePolicy::Fixed, QSizePolicy::Fixed);
    d->layout->addItem(d->spacer, 1, 1);

    d->layout->setSizeConstraint(QLayout::SetMinAndMaxSize);

    setWidget( mainWidget );

    connect(this, SIGNAL(dockLocationChanged(Qt::DockWidgetArea )),
             this, SLOT(locationChanged(Qt::DockWidgetArea)));
}

ShadowDocker::~ShadowDocker()
{
    delete d;
}


void ShadowDocker::unsetCanvas()
{
    d->canvas = 0;
}

void ShadowDocker::setCanvas(KoCanvasBase *canvas)
{
    if (d->canvas) {
        d->canvas->disconnectCanvasObserver(this); // "Every connection you make emits a signal, so duplicate connections emit two signals"
    }

    if (canvas) {
        d->widget->setCanvas(canvas);
    }

    d->canvas = canvas;
}


void ShadowDocker::locationChanged(Qt::DockWidgetArea area)
{
    switch(area) {
        case Qt::TopDockWidgetArea:
        case Qt::BottomDockWidgetArea:
            d->spacer->changeSize(0, 0, QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
            break;
        case Qt::LeftDockWidgetArea:
        case Qt::RightDockWidgetArea:
            d->spacer->changeSize(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
            break;
        default:
            break;
    }
    d->layout->setSizeConstraint(QLayout::SetMinAndMaxSize);
    d->layout->invalidate();
}

#include <ShadowDocker.moc>
