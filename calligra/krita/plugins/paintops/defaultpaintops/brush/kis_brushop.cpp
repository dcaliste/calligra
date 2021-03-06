/*
 *  Copyright (c) 2002 Patrick Julien <freak@codepimps.org>
 *  Copyright (c) 2004-2008 Boudewijn Rempt <boud@valdyas.org>
 *  Copyright (c) 2004 Clarence Dang <dang@kde.org>
 *  Copyright (c) 2004 Adrian Page <adrian@pagenet.plus.com>
 *  Copyright (c) 2004 Cyrille Berger <cberger@cberger.net>
 *  Copyright (c) 2010 Lukáš Tvrdý <lukast.dev@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "kis_brushop.h"

#include <QRect>

#include <kis_image.h>
#include <kis_vec.h>
#include <kis_debug.h>

#include <KoColorTransformation.h>
#include <KoColor.h>

#include <kis_brush.h>
#include <kis_global.h>
#include <kis_paint_device.h>
#include <kis_painter.h>
#include <kis_brush_based_paintop_settings.h>
#include <kis_color_source.h>
#include <kis_pressure_sharpness_option.h>
#include <KoColorSpaceRegistry.h>
#include <kis_fixed_paint_device.h>

KisBrushOp::KisBrushOp(const KisBrushBasedPaintOpSettings *settings, KisPainter *painter, KisImageWSP image)
    : KisBrushBasedPaintOp(settings, painter), m_opacityOption(settings->node()), m_hsvTransformation(0)
{
    Q_UNUSED(image);
    Q_ASSERT(settings);

    KisColorSourceOption colorSourceOption;
    colorSourceOption.readOptionSetting(settings);
    m_colorSource = colorSourceOption.createColorSource(painter);

    m_hsvOptions.append(KisPressureHSVOption::createHueOption());
    m_hsvOptions.append(KisPressureHSVOption::createSaturationOption());
    m_hsvOptions.append(KisPressureHSVOption::createValueOption());

    foreach(KisPressureHSVOption* option, m_hsvOptions)
    {
        option->readOptionSetting(settings);
        option->sensor()->reset();
        if(option->isChecked() && !m_hsvTransformation)
        {
            m_hsvTransformation = painter->backgroundColor().colorSpace()->createColorTransformation("hsv_adjustment", QHash<QString, QVariant>());
        }
    }

    m_opacityOption.readOptionSetting(settings);
    m_sizeOption.readOptionSetting(settings);
    m_spacingOption.readOptionSetting(settings);
    m_softnessOption.readOptionSetting(settings);
    m_sharpnessOption.readOptionSetting(settings);
    m_darkenOption.readOptionSetting(settings);
    m_rotationOption.readOptionSetting(settings);
    m_mixOption.readOptionSetting(settings);
    m_scatterOption.readOptionSetting(settings);

    m_opacityOption.sensor()->reset();
    m_sizeOption.sensor()->reset();
    m_softnessOption.sensor()->reset();
    m_sharpnessOption.sensor()->reset();
    m_darkenOption.sensor()->reset();
    m_rotationOption.sensor()->reset();
    m_scatterOption.sensor()->reset();

    m_dabCache->setSharpnessPostprocessing(&m_sharpnessOption);
    m_rotationOption.applyFanCornersInfo(this);
}

KisBrushOp::~KisBrushOp()
{
    qDeleteAll(m_hsvOptions);
    delete m_colorSource;
    delete m_hsvTransformation;
}

KisSpacingInformation KisBrushOp::paintAt(const KisPaintInformation& info)
{
    if (!painter()->device()) return 1.0;

    KisBrushSP brush = m_brush;
    Q_ASSERT(brush);
    if (!brush)
        return 1.0;

    if (!brush->canPaintFor(info))
        return 1.0;

    qreal scale = m_sizeOption.apply(info);
    if (checkSizeTooSmall(scale)) return KisSpacingInformation();


    KisPaintDeviceSP device = painter()->device();

    qreal rotation = m_rotationOption.apply(info);

    setCurrentScale(scale);
    setCurrentRotation(rotation);

    QPointF hotSpot = brush->hotSpot(scale, scale, rotation, info);
    // return info.pos() if sensor is not enabled
    QPointF pos = m_scatterOption.apply(info, qMax(brush->width(), brush->height()) * scale);
    QPointF pt = pos - hotSpot;

    // Split the coordinates into integer plus fractional parts. The integer
    // is where the dab will be positioned and the fractional part determines
    // the sub-pixel positioning.
    qint32 x;
    qreal xFraction;
    qint32 y;
    qreal yFraction;

    m_sharpnessOption.apply(info, pt, x, y, xFraction, yFraction);

    quint8 origOpacity = painter()->opacity();
    quint8 origFlow    = painter()->flow();

    m_opacityOption.apply(painter(), info);
    m_colorSource->selectColor(m_mixOption.apply(info));
    m_darkenOption.apply(m_colorSource, info);

    if (m_hsvTransformation) {
        foreach(KisPressureHSVOption* option, m_hsvOptions) {
            option->apply(m_hsvTransformation, info);
        }
        m_colorSource->applyColorTransformation(m_hsvTransformation);
    }

    KisFixedPaintDeviceSP dab = m_dabCache->fetchDab(device->compositionSourceColorSpace(),
                                                     m_colorSource,
                                                     scale, scale,
                                                     rotation,
                                                     info,
                                                     xFraction, yFraction,
                                                     m_softnessOption.apply(info));

    painter()->bltFixed(QPoint(x, y), dab, dab->bounds());

    painter()->renderMirrorMaskSafe(QRect(QPoint(x,y), QSize(dab->bounds().width(),dab->bounds().height())),
                                    dab,
                                    !m_dabCache->needSeparateOriginal());
    painter()->setOpacity(origOpacity);
    painter()->setFlow(origFlow);

    return effectiveSpacing(dab->bounds().width(),
                            dab->bounds().height(),
                            m_spacingOption, info);
}

void KisBrushOp::paintLine(const KisPaintInformation& pi1, const KisPaintInformation& pi2, KisDistanceInformation *currentDistance)
{
    if(m_sharpnessOption.isChecked() && m_brush && (m_brush->width() == 1) && (m_brush->height() == 1)) {

        if (!m_lineCacheDevice) {
            m_lineCacheDevice = source()->createCompositionSourceDevice();
        } else {
            m_lineCacheDevice->clear();
        }

        KisPainter p(m_lineCacheDevice);
        p.setPaintColor(painter()->paintColor());
        p.drawDDALine(pi1.pos(), pi2.pos());

        QRect rc = m_lineCacheDevice->extent();
        painter()->bitBlt(rc.x(), rc.y(), m_lineCacheDevice, rc.x(), rc.y(), rc.width(), rc.height());
    } else {
        KisPaintOp::paintLine(pi1, pi2, currentDistance);
    }
}
