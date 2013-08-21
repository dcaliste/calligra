/*
 *  Copyright (c) 2006 Boudewijn Rempt <boud@valdyas.org>
 *  Copyright (c) 2008 Cyrille Berger <cberger@cberger.net>
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

#include "kis_dlg_adjustment_layer.h"
#include <klocale.h>

#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QGridLayout>
#include <QTimer>

#include <QLineEdit>

#include "filter/kis_filter.h"
#include "kis_config_widget.h"
#include "filter/kis_filter_configuration.h"
#include "kis_paint_device.h"
#include "kis_transaction.h"
#include "kis_node.h"
#include "kis_node_filter_interface.h"
#include <kis_config.h>
#include "kis_view2.h"

KisDlgAdjustmentLayer::KisDlgAdjustmentLayer(KisNodeSP node,
                                             KisNodeFilterInterface* nfi,
                                             KisPaintDeviceSP paintDevice,
                                             const QString &layerName,
                                             const QString &caption,
                                             KisView2 *view)
    : KDialog(view)
    , m_node(node)
    , m_nodeFilterInterface(nfi)
    , m_currentFilter(0)
    , m_freezeName(false)
{
    setCaption(caption);
    setButtons(Ok | Cancel);
    setDefaultButton(Ok);

    QWidget * page = new QWidget(this);
    wdgFilterNodeCreation.setupUi(page);
    setMainWidget(page);
    wdgFilterNodeCreation.filterSelector->setView(view);
    wdgFilterNodeCreation.filterSelector->showFilterGallery(KisConfig().showFilterGalleryLayerMaskDialog());

    connect(wdgFilterNodeCreation.filterSelector, SIGNAL(configurationChanged()), SLOT(slotConfigChanged()));

    wdgFilterNodeCreation.filterSelector->setPaintDevice(paintDevice);
    wdgFilterNodeCreation.layerName->setText(layerName);
    enableButtonOk(0);
}

KisDlgAdjustmentLayer::~KisDlgAdjustmentLayer()
{
    KisConfig().setShowFilterGalleryLayerMaskDialog(wdgFilterNodeCreation.filterSelector->isFilterGalleryVisible());
}

void KisDlgAdjustmentLayer::slotNameChanged(const QString & text)
{
    Q_UNUSED(text);
    if (m_freezeName)
        return;

    enableButtonOk(true);
}

KisFilterConfiguration * KisDlgAdjustmentLayer::filterConfiguration() const
{
    KisFilterConfiguration* config = wdgFilterNodeCreation.filterSelector->configuration();
    return config;
}

QString KisDlgAdjustmentLayer::layerName() const
{
    return wdgFilterNodeCreation.layerName->text();
}

void KisDlgAdjustmentLayer::slotConfigChanged()
{
    enableButtonOk(true);
    KisFilterConfiguration * config = filterConfiguration();
    if (config) {
        m_nodeFilterInterface->setFilter(config);
    }
    m_node->setDirty();
}

#include "kis_dlg_adjustment_layer.moc"
