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

#ifndef CALLIGRA_COMPONENTS_DOCUMENTIMPL_H
#define CALLIGRA_COMPONENTS_DOCUMENTIMPL_H

#include <QtCore/QObject>
#include "Global.h"

class KoDocument;
class QGraphicsWidget;
class KoFindBase;
class KoCanvasController;
class KoZoomController;
class KoDocument;
class KoCanvasBase;

namespace Calligra {
namespace Components {

/**
 * \brief Defines an interface for handling specific documents in Document.
 *
 */

class DocumentImpl : public QObject
{
    Q_OBJECT
public:
    explicit DocumentImpl(QObject* parent = 0);
    virtual ~DocumentImpl();

    virtual bool load(const QUrl& url) = 0;
    virtual int currentIndex() = 0;
    virtual void setCurrentIndex(int newValue) = 0;

    DocumentType::Type documentType() const;
    KoFindBase* finder() const;
    QGraphicsWidget* canvas() const;
    KoCanvasController* canvasController() const;
    KoZoomController* zoomController() const;
    QSize documentSize() const;
    KoDocument* koDocument() const;

Q_SIGNALS:
    void documentSizeChanged();
    void currentIndexChanged();

protected:
    void setDocumentType(DocumentType::Type type);
    void setKoDocument(KoDocument* document);
    void setCanvas(QGraphicsWidget* newCanvas);
    void setFinder(KoFindBase* newFinder);

    void createAndSetCanvasController(KoCanvasBase* canvas);
    void createAndSetZoomController(KoCanvasBase* canvas);

private Q_SLOTS:
    void setDocumentSize(const QSize& size);

private:
    class Private;
    Private* const d;
};

} // Namespace Components
} // Namespace Calligra

#endif // CALLIGRA_COMPONENTS_DOCUMENTIMPL_H
