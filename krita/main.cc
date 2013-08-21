/*
 *  main.cc - part of KImageShop
 *
 *  Copyright (c) 1999 Matthias Elter <me@kde.org>
 *  Copyright (c) 2002 Patrick Julien <freak@codepimps.org>
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
 */

#include <stdlib.h>

#include <QString>
#include <QPixmap>
#include <QDebug>
#include <QProcess>
#include <QProcessEnvironment>
#include <QDesktopServices>
#include <QDir>

#include <kglobal.h>
#include <kcmdlineargs.h>
#include <ksplashscreen.h>

#include <KoApplication.h>

#include <krita_export.h>

#include "data/splash/splash_screen.xpm"
#include "ui/kis_aboutdata.h"
#include "ui/kis_doc2.h"

#ifdef Q_OS_WIN
#include "stdlib.h"
#endif

extern "C" KDE_EXPORT int kdemain(int argc, char **argv)
{
#if HAVE_X11
    setenv("QT_NO_GLIB", "1", true);
#endif

    int state;
    QScopedPointer<KAboutData> about(newKritaAboutData());

    KCmdLineArgs::init(argc, argv, about.data());

    KCmdLineOptions options;
    options.add("+[file(s)]", ki18n("File(s) or URL(s) to open"));
    KCmdLineArgs::addCmdLineOptions(options);
    KoApplication::addCommonCommandLineOptions();
    // first create the application so we can create a  pixmap
    KoApplication app(argc, argv, KIS_MIME_TYPE, about.data());

    // then create the pixmap from an xpm: we cannot get the
    // location of our datadir before we've started our components,
    // so use an xpm.
    QSplashScreen *splash = new KSplashScreen(QPixmap(splash_screen_xpm));
    app.setSplashScreen(splash);


    if (!app.start()) {
        return 1;
    }

    // now save some memory.
    app.setSplashScreen(0);
    delete splash;

    state = app.exec();

    delete aboutData;

    return state;
}

