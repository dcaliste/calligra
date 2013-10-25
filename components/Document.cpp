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

#include "Document.h"

#include <QtCore/QDebug>
#include <QtCore/QUrl>
#include <QtCore/QSizeF>

#include "impl/TextDocumentImpl.h"
#include "impl/SpreadsheetImpl.h"
#include "impl/PresentationImpl.h"

using namespace Calligra::Components;

class Document::Private
{
public:
    Private(Document* qq) : q{qq}, impl{nullptr}, status{DocumentStatus::Unloaded}
    { }

    void updateImpl();

    Document* q;

    QUrl source;
    DocumentImpl* impl;
    DocumentStatus::Status status;
};

Document::Document(QObject* parent)
    : QObject{parent}, d{new Private{this}}
{

}

Document::~Document()
{
    delete d;
}

QUrl Document::source() const
{
    return d->source;
}

void Document::setSource(const QUrl& value)
{
    if(value != d->source) {
        d->source = value;
        emit sourceChanged();

        d->status = DocumentStatus::Loading;
        emit statusChanged();

        d->updateImpl();
        emit documentTypeChanged();

        if(d->impl) {
            if(d->impl->load(d->source)) {
                d->status = DocumentStatus::Loaded;
            } else {
                d->status = DocumentStatus::Failed;
            }
        } else {
            d->status = DocumentStatus::Unloaded;
        }

        emit statusChanged();
    }
}

DocumentType::Type Document::documentType() const
{
    if(d->impl) {
        return d->impl->documentType();
    }

    return DocumentType::Unknown;
}

DocumentStatus::Status Document::status() const
{
    return d->status;
}

QSize Document::documentSize() const
{
    if(d->impl) {
        return d->impl->documentSize();
    }

    return QSize{};
}

int Document::currentIndex() const
{
    if(d->impl) {
        return d->impl->currentIndex();
    }

    return -1;
}

void Document::setCurrentIndex(int newValue)
{
    if(d->impl) {
        d->impl->setCurrentIndex(newValue);
    }
}

KoFindBase* Document::finder() const
{
    if(d->impl) {
        return d->impl->finder();
    }

    return nullptr;
}

QGraphicsWidget* Document::canvas() const
{
    if(d->impl) {
        return d->impl->canvas();
    }

    return nullptr;
}

KoCanvasController* Document::canvasController() const
{
    if(d->impl) {
        return d->impl->canvasController();
    }

    return nullptr;
}

KoZoomController* Document::zoomController() const
{
    if(d->impl) {
        return d->impl->zoomController();
    }

    return nullptr;
}

KoDocument* Document::koDocument() const
{
    if(d->impl) {
        return d->impl->koDocument();
    }

    return nullptr;
}

void Document::Private::updateImpl()
{
    if(impl) {
        delete impl;
    }

    if(!source.isEmpty()) {
        auto type = Global::documentType(source);
        switch(type) {
            case DocumentType::TextDocument:
                impl = new TextDocumentImpl{q};
                break;
            case DocumentType::Spreadsheet:
                impl = new SpreadsheetImpl{q};
                break;
            case DocumentType::Presentation:
                impl = new PresentationImpl{q};
                break;
            default:
                impl = nullptr;
                break;
        }
    } else {
        impl = nullptr;
    }

    if(impl) {
        connect(impl, &DocumentImpl::documentSizeChanged, q, &Document::documentSizeChanged);
        connect(impl, &DocumentImpl::currentIndexChanged, q, &Document::currentIndexChanged);
    }
}

#include "moc_Document.cpp"
