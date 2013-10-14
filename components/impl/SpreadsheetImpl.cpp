/*
 * This file is part of the KDE project
 * Copyright (C) 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "SpreadsheetImpl.h"

#include <QtWidgets/QGraphicsWidget>

#include <KoZoomController.h>
#include <sheets/part/Part.h>
#include <sheets/part/Doc.h>
#include <sheets/part/CanvasItem.h>
#include <sheets/Map.h>
#include <sheets/Sheet.h>

using namespace Calligra::Components;

class SpreadsheetImpl::Private
{
public:
    Private() : part{nullptr}, document{nullptr}
    { }

    Calligra::Sheets::Part* part;
    Calligra::Sheets::Doc* document;
};

SpreadsheetImpl::SpreadsheetImpl(QObject* parent)
    : DocumentImpl{parent}, d{new Private}
{
    setDocumentType(Global::SpreadsheetType);
}

SpreadsheetImpl::~SpreadsheetImpl()
{
    delete d;
}

bool SpreadsheetImpl::load(const QUrl& url)
{
    if(d->part) {
        delete d->part;
        delete d->document;
    }

    d->part = new Calligra::Sheets::Part{this};
    d->document = new Calligra::Sheets::Doc{d->part};
    d->part->setDocument(d->document);

    bool retval = d->document->openUrl(url);

    Calligra::Sheets::CanvasItem* canvas = static_cast<Calligra::Sheets::CanvasItem*>(d->part->canvasItem(d->document));

    createAndSetCanvasController(canvas);
    createAndSetZoomController(canvas);
    connect(canvas, &Calligra::Sheets::CanvasItem::documentSizeChanged, this, &SpreadsheetImpl::updateDocumentSize);

    Calligra::Sheets::Sheet *sheet = d->document->map()->sheet(0);
    if(sheet) {
        updateDocumentSize(sheet->documentSize().toSize());
    }

    setCanvas(canvas);

    return retval;
}

void SpreadsheetImpl::updateDocumentSize(const QSize& size)
{
    zoomController()->setDocumentSize(size, false);
}
