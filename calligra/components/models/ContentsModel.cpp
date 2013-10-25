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

#include "ContentsModel.h"

#include <QSize>
#include <QGraphicsWidget>

#include <KoCanvasBase.h>

#include "Document.h"
#include "PresentationContentsModelImpl.h"
#include "SpreadsheetContentsModelImpl.h"
#include "TextContentsModelImpl.h"

using namespace Calligra::Components;

class ContentsModel::Private
{
public:
    Private() : impl{nullptr}, document{nullptr}, thumbnailSize{128, 128}
    { }

    ContentsModelImpl* impl;
    Document* document;
    QSize thumbnailSize;
};

ContentsModel::ContentsModel(QObject* parent)
    : QAbstractListModel{parent}, d{new Private}
{
    QHash<int, QByteArray> roleNames;
    roleNames.insert(TitleRole, "title");
    roleNames.insert(LevelRole, "level");
    roleNames.insert(ThumbnailRole, "thumbnail");
    roleNames.insert(ContentIndexRole, "contentIndex");
    setRoleNames(roleNames);
}

ContentsModel::~ContentsModel()
{
    delete d;
}

QVariant ContentsModel::data(const QModelIndex& index, int role) const
{
    if(!d->impl || !index.isValid())
        return QVariant();

    return d->impl->data(index.row(), static_cast<Role>(role));
}

int ContentsModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    if(d->impl) {
        return d->impl->rowCount();
    }

    return 0;
}

Document* ContentsModel::document() const
{
    return d->document;
}

void ContentsModel::setDocument(Document* newDocument)
{
    if(newDocument != d->document) {
        if(d->document) {
            disconnect(d->document, &Document::statusChanged, this, &ContentsModel::updateImpl);
        }

        d->document = newDocument;
        connect(d->document, &Document::statusChanged, this, &ContentsModel::updateImpl);

        updateImpl();

        emit documentChanged();
    }
}

QSize ContentsModel::thumbnailSize() const
{
    return d->thumbnailSize;
}

void ContentsModel::setThumbnailSize(QSize newValue)
{
    if(newValue != d->thumbnailSize) {
        d->thumbnailSize = newValue;

        if(d->impl) {
            d->impl->setThumbnailSize(newValue);
            emit dataChanged(index(0), index(d->impl->rowCount() - 1), QVector<int>{} << ThumbnailRole);
        }

        emit thumbnailSizeChanged();
    }
}

void ContentsModel::updateImpl()
{
    beginResetModel();

    if(d->impl) {
        delete d->impl;
    }

    if(d->document && d->document->status() == DocumentStatus::Loaded) {
        switch(d->document->documentType()) {
            case DocumentType::TextDocument: {
                auto textImpl = new TextContentsModelImpl{d->document->koDocument(), dynamic_cast<KoCanvasBase*>(d->document->canvas())};
                d->impl = textImpl;
                connect(textImpl, &TextContentsModelImpl::listContentsCompleted, this, &ContentsModel::reset);
                break;
            }
            case DocumentType::Spreadsheet:
                d->impl = new SpreadsheetContentsModelImpl{d->document->koDocument()};
                break;
            case DocumentType::Presentation:
                d->impl = new PresentationContentsModelImpl{d->document->koDocument()};
                break;
            default:
                d->impl = nullptr;
                break;
        }
    } else {
        d->impl = nullptr;
    }

    if(d->impl) {
        d->impl->setThumbnailSize(d->thumbnailSize);
    }

    endResetModel();
}
