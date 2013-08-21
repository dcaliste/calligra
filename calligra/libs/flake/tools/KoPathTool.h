/* This file is part of the KDE project
 * Copyright (C) 2006-2012 Jan Hambrecht <jaham@gmx.net>
 * Copyright (C) 2006,2007 Thorsten Zachmann <zachmann@kde.org>
 * Copyright (C) 2007 Thomas Zander <zander@kde.org>
 * Copyright (C) 2007 Boudewijn Rempt <boud@valdyas.org>
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

#ifndef KOPATHTOOL_H
#define KOPATHTOOL_H

#include "KoPathShape.h"
#include "KoToolBase.h"
#include "KoPathToolSelection.h"
#include <QList>

class QButtonGroup;
class KoCanvasBase;
class KoInteractionStrategy;
class KoPathToolHandle;
class KoParameterShape;

class KAction;

/// The tool for editing a KoPathShape or a KoParameterShape
class FLAKE_TEST_EXPORT KoPathTool : public KoToolBase
{
    Q_OBJECT
public:
    explicit KoPathTool(KoCanvasBase *canvas);
    ~KoPathTool();

    /// reimplemented
    virtual void paint(QPainter &painter, const KoViewConverter &converter);

    /// reimplemented
    virtual void repaintDecorations();

    /// reimplemented
    virtual void mousePressEvent(KoPointerEvent *event);
    /// reimplemented
    virtual void mouseMoveEvent(KoPointerEvent *event);
    /// reimplemented
    virtual void mouseReleaseEvent(KoPointerEvent *event);
    /// reimplemented
    virtual void keyPressEvent(QKeyEvent *event);
    /// reimplemented
    virtual void keyReleaseEvent(QKeyEvent *event);
    /// reimplemented
    virtual void mouseDoubleClickEvent(KoPointerEvent *event);
    /// reimplemented
    virtual void activate(ToolActivation toolActivation, const QSet<KoShape*> &shapes);
    /// reimplemented
    virtual void deactivate();

    /// reimplemented
    virtual void deleteSelection();

    /// reimplemented
    virtual KoToolSelection* selection();

    /// repaints the specified rect
    void repaint(const QRectF &repaintRect);

public slots:
    void documentResourceChanged(int key, const QVariant & res);

signals:
    void typeChanged(int types);
    void pathChanged(KoPathShape* path); // TODO this is unused, can we remove this one?
protected:
    /// reimplemented
    virtual QList<QWidget *>  createOptionWidgets();

private:
    struct PathSegment;

    void updateOptionsWidget();
    PathSegment* segmentAtPoint(const QPointF &point);

private slots:
    void pointTypeChanged(QAction *type);
    void insertPoints();
    void removePoints();
    void segmentToLine();
    void segmentToCurve();
    void convertToPath();
    void joinPoints();
    void mergePoints();
    void breakAtPoint();
    void breakAtSegment();
    void pointSelectionChanged();
    void updateActions();
    void pointToLine();
    void pointToCurve();
    void activate();

protected:
    KoPathToolSelection m_pointSelection; ///< the point selection
    QCursor m_selectCursor;

private:

    KoPathToolHandle * m_activeHandle;       ///< the currently active handle
    int m_handleRadius;    ///< the radius of the control point handles
    uint m_grabSensitivity; ///< the grab sensitivity
    QPointF m_lastPoint; ///< needed for interaction strategy
    PathSegment *m_activeSegment;

    // make a frind so that it can test private member/methods
    friend class TestPathTool;

    KoInteractionStrategy *m_currentStrategy; ///< the rubber selection strategy

    QButtonGroup *m_pointTypeGroup;

    KAction *m_actionPathPointCorner;
    KAction *m_actionPathPointSmooth;
    KAction *m_actionPathPointSymmetric;
    KAction *m_actionCurvePoint;
    KAction *m_actionLinePoint;
    KAction *m_actionLineSegment;
    KAction *m_actionCurveSegment;
    KAction *m_actionAddPoint;
    KAction *m_actionRemovePoint;
    KAction *m_actionBreakPoint;
    KAction *m_actionBreakSegment;
    KAction *m_actionJoinSegment;
    KAction *m_actionMergePoints;
    KAction *m_actionConvertToPath;
    QCursor m_moveCursor;

    Q_DECLARE_PRIVATE(KoToolBase)
};

#endif
