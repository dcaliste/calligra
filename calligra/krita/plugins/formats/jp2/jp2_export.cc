/*
 *  Copyright (c) 2009 Cyrille Berger <cberger@cberger.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "jp2_export.h"

#include <QCheckBox>
#include <QSlider>

#include <QApplication>
#include <kdialog.h>
#include <kpluginfactory.h>

#include <KoColorSpaceConstants.h>
#include <KoFilterChain.h>
#include <KoFilterManager.h>

#include <kis_properties_configuration.h>
#include <kis_config.h>
#include <kis_doc2.h>
#include <kis_image.h>
#include <kis_group_layer.h>
#include <kis_paint_layer.h>
#include <kis_paint_device.h>

#include "jp2_converter.h"

#include "ui_kis_wdg_options_jp2.h"

class KisExternalLayer;

K_PLUGIN_FACTORY_WITH_JSON(ExportFactory, "krita_jp2_export.json", registerPlugin<jp2Export>();)
//K_EXPORT_PLUGIN(ExportFactory("calligrafilters"))

jp2Export::jp2Export(QObject *parent, const QVariantList &) : KoFilter(parent)
{
}

jp2Export::~jp2Export()
{
}

KoFilter::ConversionStatus jp2Export::convert(const QByteArray& from, const QByteArray& to)
{
    dbgFile << "JP2 export! From:" << from << ", To:" << to << "";

    if (from != "application/x-krita")
        return KoFilter::NotImplemented;

    KisDoc2 *output = dynamic_cast<KisDoc2*>(m_chain->inputDocument());
    QString filename = m_chain->outputFile();

    if (!output)
        return KoFilter::NoDocumentCreated;


    if (filename.isEmpty()) return KoFilter::FileNotFound;

    KDialog* kdb = new KDialog(0);
    kdb->setWindowTitle(i18n("JPEG 2000 Export Options"));
    kdb->setButtons(KDialog::Ok | KDialog::Cancel);

    Ui::WdgOptionsJP2 optionsJP2;

    QWidget* wdg = new QWidget(kdb);
    optionsJP2.setupUi(wdg);


    QString filterConfig = KisConfig().exportConfiguration("JP2");
    KisPropertiesConfiguration cfg;
    cfg.fromXML(filterConfig);
    optionsJP2.numberResolutions->setValue(cfg.getInt("number_resolutions", 6));
    optionsJP2.qualityLevel->setValue(cfg.getInt("quality", 100));
    
    kdb->setMainWidget(wdg);
    qApp->restoreOverrideCursor();

    if (!m_chain->manager()->getBatchMode()) {
        if (kdb->exec() == QDialog::Rejected) {
            return KoFilter::OK; // FIXME Cancel doesn't exist :(
        }
    }
    
    JP2ConvertOptions options;
    options.numberresolution = optionsJP2.numberResolutions->value();
    cfg.setProperty("number_resolutions", options.numberresolution);
    options.rate = optionsJP2.qualityLevel->value();
    cfg.setProperty("quality", options.rate);

    KisConfig().setExportConfiguration("JP2", cfg);

    KUrl url;
    url.setPath(filename);

    KisImageWSP image = output->image();
    Q_CHECK_PTR(image);
    image->refreshGraph();
    image->lock();

    jp2Converter kpc(output);

    KisPaintDeviceSP pd = new KisPaintDevice(*image->projection());
    KisPaintLayerSP l = new KisPaintLayer(image, "projection", OPACITY_OPAQUE_U8, pd);
    image->unlock();

    KisImageBuilder_Result res;

    if ((res = kpc.buildFile(url, l, options)) == KisImageBuilder_RESULT_OK) {
        dbgFile << "success !";
        return KoFilter::OK;
    }
    dbgFile << " Result =" << res;
    return KoFilter::InternalError;
}

#include <jp2_export.moc>

