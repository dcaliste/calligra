/* This file is part of the KDE project
   Copyright (C) 2003-2007 Jarosław Staniek <staniek@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
  */

#include "KexiStartupDialog.h"
#include "KexiStartupDialogTemplatesPage.h"
#include "kexi.h"
#include <widget/KexiProjectSelectorWidget.h>
#include <widget/KexiConnectionSelectorWidget.h>
#include <widget/KexiFileWidget.h>
#include <kexiutils/utils.h>
#include <db/utils.h>
#include <kexi_global.h>

#include <KoIcon.h>

#include <QLayout>
#include <QTabWidget>
#include <QComboBox>
#include <QCheckBox>
#include <QPoint>
#include <QObject>
#include <QApplication>

#include <QPixmap>
#include <QLabel>
#include <QKeyEvent>
#include <QEvent>
#include <QListView>

#include <klocale.h>

#include <kcomponentdata.h>
#include <kdebug.h>
#include <kpushbutton.h>
#include <kglobalsettings.h>
#include <ktextedit.h>
#include <kmessagebox.h>
#include <QApplication>
#include <kmimetype.h>
#include <ktextbrowser.h>
#include <kconfig.h>
#include <kiconloader.h>
#include <kurl.h>

#ifdef KEXI_SHOW_UNIMPLEMENTED
#define KEXI_STARTUP_SHOW_RECENT
#endif

//! @internal
class KexiStartupDialog::Private
{
public:
    Private()
            : pageTemplates(0), pageOpenExisting(0), pageOpenRecent(0)
            , templPageWidgetItem_BlankDatabase(0)
            , templPageWidgetItem_ImportExisting(0)
            , templPageWidgetItem_CreateFromTemplate(0)
//  , pageTemplatesID(-1)
//  , pageOpenExistingID(-1)
//  , pageOpenRecentID(-1)
    {
        result = -1;
        QString iconname(KexiDB::defaultFileBasedDriverIconName());
        kexi_sqlite_icon = KIcon(iconname);
        const char shortcutMimeTypeName[] = "application/x-kexiproject-shortcut";
        KMimeType::Ptr mime(KMimeType::mimeType(shortcutMimeTypeName));
        if (mime.isNull()) {
            KexiDBWarn << QString("'%1' mimetype not installed!").arg(shortcutMimeTypeName);
            iconname.clear();
        } else
            iconname = mime->iconName();
        kexi_shortcut_icon = KIcon(iconname); // TODO: no longer used?
        prj_selector = 0;
        chkDoNotShow = 0;
        openExistingConnWidget = 0;
        templatesWidget = 0;
        templatesWidget_IconListView = 0;
    }
    ~Private() {
    }

    int dialogType, dialogOptions;

    KFakePageWidgetItem *pageTemplates, *pageOpenExisting, *pageOpenRecent;

    // subpages within "templates" page
    KFakePageWidgetItem *templPageWidgetItem_BlankDatabase,
    *templPageWidgetItem_ImportExisting, *templPageWidgetItem_CreateFromTemplate;
    //int pageTemplatesID;
    //int pageOpenExistingID, pageOpenRecentID;
    //int templatesSectionID_blank, templatesSectionID_import;
//#ifdef KEXI_PROJECT_TEMPLATES
// int templatesSectionID_templates; //, templatesSectionID_custom2;
//#endif
    QCheckBox *chkDoNotShow;

    //widgets for template tab:
    KPageWidget* templatesWidget;
    QListView *templatesWidget_IconListView;//helper

    KexiStartupDialogTemplatesPage *viewTemplates;
    //TemplatesPage *viewBusinessTempl;

    int result;

    KIcon kexi_sqlite_icon;
    KIcon kexi_shortcut_icon;

// //! Key string of selected database template. \sa selectedTemplateKey()
// QString selectedTemplateKey;

    //! used for "open existing"
    KexiDBConnectionSet *connSet;
    KexiFileWidget *openExistingFileWidget; //! embedded file widget
    KexiConnectionSelectorWidget *openExistingConnWidget;
// KUrl existingUrlToOpen; //! helper for returning a file name to open
    KexiDB::ConnectionData* selectedExistingConnection; //! helper for returning selected connection

    //! used for "open recent"
    KexiProjectSet *recentProjects;
    KexiProjectSelectorWidget* prj_selector;

    //! true if the dialog contain single page, not tabs
    bool singlePage;
};

static QString captionForDialogType(int type)
{
    if (type == KexiStartupDialog::Templates)
        return i18n("Create Project");
    else if (type == KexiStartupDialog::OpenExisting)
        return i18n("Open Existing Project");
    else if (type == KexiStartupDialog::OpenRecent)
        return i18n("Open Recent Project");

    return i18n("Choose Project");
}

/*================================================================*/

KexiStartupDialog::KexiStartupDialog(
    int dialogType, int dialogOptions,
    KexiDBConnectionSet& connSet, KexiProjectSet& recentProjects,
    QWidget *parent)
        : KoPageDialog(parent)
        , d(new Private())
{
    d->singlePage = dialogType == KexiStartupDialog::Templates
                    || dialogType == KexiStartupDialog::OpenExisting || dialogType == KexiStartupDialog::OpenRecent;
    setFaceType(d->singlePage ? Plain : Tabbed);
    setCaption(captionForDialogType(dialogType));
    setButtons(Help | Ok | Cancel);
    d->recentProjects = &recentProjects;
    d->connSet = &connSet;
    d->dialogType = dialogType;
    d->dialogOptions = dialogOptions;

    if (dialogType == OpenExisting) {//this dialog has "open" tab only!
        setWindowIcon(koIcon("document-open"));
    } else {
        setWindowIcon(d->kexi_sqlite_icon);
    }

    setSizeGripEnabled(true);
// int id=0;
    KFakePageWidgetItem *firstPage = 0;
    if (d->dialogType & Templates) {
        setupPageTemplates();
        //d->pageTemplatesID = id++;
        d->templatesWidget->setFocus();
        if (!firstPage)
            firstPage = d->pageTemplates;
    }
    if (d->dialogType & OpenExisting) {
        setupPageOpenExisting();
//  d->pageOpenExistingID = id++;
        if (d->singlePage)
            d->openExistingConnWidget->setFocus();
        if (!firstPage)
            firstPage = d->pageOpenExisting;
    }
#ifdef KEXI_STARTUP_SHOW_RECENT
    if (d->dialogType & OpenRecent) {
        setupPageOpenRecent();
//  d->pageOpenRecentID = id++;
        if (d->singlePage)
            d->prj_selector->setFocus();
        if (!firstPage)
            firstPage = d->pageOpenRecent;
    }
#endif

    if (!d->singlePage) {
        connect(this, SIGNAL(currentPageChanged(KFakePageWidgetItem*,KFakePageWidgetItem*)),
                this, SLOT(slotCurrentPageChanged(KFakePageWidgetItem*,KFakePageWidgetItem*)));
        d->templatesWidget->setFocus();
    }
    connect(this, SIGNAL(okClicked()), this, SLOT(slotOk()));
    setCurrentPage(firstPage);
    updateDialogOKButton(firstPage);
    adjustSize();
}

KexiStartupDialog::~KexiStartupDialog()
{
    delete d;
}

bool KexiStartupDialog::shouldBeShown()
{
    KConfigGroup group = KGlobal::config()->group("Startup");
    return group.readEntry("ShowStartupDialog", true);
}

void KexiStartupDialog::showEvent(QShowEvent *e)
{
    KoPageDialog::showEvent(e);
    //just some cleanup
    //d->existingUrlToOpen = KUrl();
    d->result = -1;

    KDialog::centerOnScreen(this);
}

int KexiStartupDialog::result() const
{
    return d->result;
}

void KexiStartupDialog::done(int r)
{
    if (d->result != -1) //already done!
        return;

// kDebug() << "KexiStartupDialog::done(" << r << ")";
// updateSelectedTemplateKeyInfo();

    if (r == QDialog::Rejected) {
        d->result = CancelResult;
    } else {
        KFakePageWidgetItem *currentPageWidgetItem = currentPage();

        if (currentPageWidgetItem == d->pageTemplates) {
            KFakePageWidgetItem *currenTemplatesPageWidgetItem = d->templatesWidget->currentPage();
            if (currenTemplatesPageWidgetItem == d->templPageWidgetItem_BlankDatabase)
                d->result = CreateBlankResult;
#ifdef KEXI_PROJECT_TEMPLATES
            else if (currenTemplatesPageWidgetItem == d->templPageWidgetItem_CreateFromTemplate)
                d->result = CreateFromTemplateResult;
#endif
            else if (currenTemplatesPageWidgetItem == d->templPageWidgetItem_ImportExisting)
                d->result = ImportResult;
        } else if (currentPageWidgetItem == d->pageOpenExisting) {
            // return file or connection:
            if (d->openExistingConnWidget->selectedConnectionType()
                    == KexiConnectionSelectorWidget::FileBased) {
                if (!d->openExistingFileWidget->checkSelectedFile())
                    return;
//kde4    d->existingFileToOpen = d->openExistingFileWidget->selectedFile();
                d->openExistingFileWidget->accept();
                d->selectedExistingConnection = 0;
            } else {
                //d->existingUrlToOpen = KUrl();
                d->selectedExistingConnection
                = d->openExistingConnWidget->selectedConnectionData();
            }
            d->result = OpenExistingResult;
        } else if (currentPageWidgetItem == d->pageOpenRecent) {
            d->result = OpenRecentResult;
        } else
            return;
    }

    //save settings
    KConfigGroup group = KGlobal::config()->group("Startup");
    if (d->openExistingConnWidget)
        group.writeEntry("OpenExistingType",
                         (d->openExistingConnWidget->selectedConnectionType() == KexiConnectionSelectorWidget::FileBased)
                         ? "File" : "Server");
    if (d->chkDoNotShow)
        group.writeEntry("ShowStartupDialog", !d->chkDoNotShow->isChecked());

    group.sync();

    KoPageDialog::done(r);
}

void KexiStartupDialog::reject()
{
// d->result = CancelResult;
    KoPageDialog::reject();
}

void KexiStartupDialog::setupPageTemplates()
{
    QFrame *pageTemplatesFrame = new QFrame(this);
    d->pageTemplates = addPage(pageTemplatesFrame, i18n("Create Project"));
    QVBoxLayout *lyr = new QVBoxLayout(pageTemplatesFrame);
    lyr->setSpacing(KDialog::spacingHint());
    lyr->setMargin(0);

// d->templatesWidget = new KJanusWidget(
//  d->pageTemplates, "templatesWidget", KJanusWidget::IconList);
    d->templatesWidget = new KPageWidget(pageTemplatesFrame);
    d->templatesWidget->setObjectName("templatesWidget");
    d->templatesWidget->setFaceType(KPageWidget::List);
    {//aaa! dirty hack
//  d->templatesWidget_IconListBox = d->templatesWidget->child(0,"KListBox");
#ifdef __GNUC__
#warning OK for KPageWidget?
#else
#pragma WARNING( OK for KPageWidget? )
#endif
        d->templatesWidget_IconListView
        = KexiUtils::findFirstChild<QListView*>(d->templatesWidget, "QListView");
        if (d->templatesWidget_IconListView)
            d->templatesWidget_IconListView->installEventFilter(this);
    }
    lyr->addWidget(d->templatesWidget);
    connect(d->templatesWidget, SIGNAL(currentPageChanged(KFakePageWidgetItem*,KFakePageWidgetItem*)),
            this, SLOT(slotCurrentTemplatesubpageChanged(KFakePageWidgetItem*,KFakePageWidgetItem*)));

    if (d->dialogOptions & CheckBoxDoNotShowAgain) {
        d->chkDoNotShow = new QCheckBox(i18n("Do not show me this dialog again"), pageTemplatesFrame);
        d->chkDoNotShow->setObjectName("chkDoNotShow");
        lyr->addWidget(d->chkDoNotShow);
    }

    //template groups:
    QFrame *templPageWidget = 0;
    QVBoxLayout *tmplyr;
    //int itemID = 0; //used just to set up templatesSectionID_*

    //- page "blank db"
// d->templatesSectionID_blank = itemID++;
    QString clickMsg("\n\n" + i18n("Click \"OK\" button to proceed."));
    templPageWidget = new QFrame(d->templatesWidget);
    d->templPageWidgetItem_BlankDatabase = d->templatesWidget->addPage(templPageWidget,
                                           i18n("Blank Database"));
    d->templPageWidgetItem_BlankDatabase->setHeader(i18n("New Blank Database Project"));
    d->templPageWidgetItem_BlankDatabase->setIcon(koIcon("x-office-document"));
    tmplyr = new QVBoxLayout(templPageWidget);
    tmplyr->setSpacing(KDialog::spacingHint());
    QLabel *lbl_blank = new QLabel(
        i18n("Kexi will create a new blank database project.") + clickMsg, templPageWidget);
    lbl_blank->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    lbl_blank->setWordWrap(true);
    lbl_blank->setMargin(0);
    tmplyr->addWidget(lbl_blank);
    tmplyr->addStretch(1);

#ifdef KEXI_PROJECT_TEMPLATES
    //- page "templates"
// d->templatesSectionID_templates = itemID++;
    QString none;
    templPageWidget = new QFrame(d->templatesWidget);
    d->templPageWidgetItem_CreateFromTemplate = d->templatesWidget->addPage(templPageWidget,
            i18n("Create From Template"));
    d->templPageWidgetItem_CreateFromTemplate->setHeader(i18n("New Database Project From Template"));
    d->templPageWidgetItem_CreateFromTemplate->setIcon(d->kexi_sqlite_icon);
    tmplyr = new QVBoxLayout(templPageWidget);
    tmplyr->setSpacing(KDialog::spacingHint());
    QLabel *lbl_templ = new QLabel(
        i18n("Kexi will create a new database project using selected template.\n"
             "Select template and click \"OK\" button to proceed."), templPageWidget);
    lbl_templ->setAlignment(Qt::AlignAuto | Qt::AlignTop);
    lbl_templ->setWordWrap(true);
    lbl_templ->setMargin(0);
    tmplyr->addWidget(lbl_templ);

    d->viewTemplates = new KexiStartupDialogTemplatesPage(templPageWidget);
    tmplyr->addWidget(d->viewTemplates);
    connect(d->viewTemplates, SIGNAL(selected(QString)),
            this, SLOT(templateSelected(QString)));
    /* connect(d->viewTemplates->templates,SIGNAL(returnPressed(QIconViewItem*)),
        this,SLOT(templateItemExecuted(QIconViewItem*)));
      connect(d->viewTemplates->templates,SIGNAL(currentChanged(QIconViewItem*)),
        this,SLOT(templateItemSelected(QIconViewItem*)));*/
    /*later
      templPageFrame = d->templatesWidget->addPage (
        i18n("Personal Databases"), i18n("New Personal Database Project Templates"), koDesktopIcon("user-home"));
      tmplyr = new QVBoxLayout(templPageFrame, 0, KDialog::spacingHint());
      d->viewPersonalTempl = new TemplatesPage( Vertical, templPageFrame, "personal_page" );
      tmplyr->addWidget( d->viewPersonalTempl );
      connect(d->viewPersonalTempl->templates,SIGNAL(doubleClicked(QIconViewItem*)),this,SLOT(templateItemExecuted(QIconViewItem*)));
      connect(d->viewPersonalTempl->templates,SIGNAL(returnPressed(QIconViewItem*)),this,SLOT(templateItemExecuted(QIconViewItem*)));
      connect(d->viewPersonalTempl->templates,SIGNAL(currentChanged(QIconViewItem*)),this,SLOT(templateItemSelected(QIconViewItem*)));
    */

    //- page "business db"
    /*later
      d->templatesSectionID_custom2 = itemID++;
      templPageFrame = d->templatesWidget->addPage (
        i18n("Business Databases"), i18n("New Business Database Project Templates"),
        koDesktopIcon("user-identity"));
      tmplyr = new QVBoxLayout(templPageFrame, 0, KDialog::spacingHint());
      d->viewBusinessTempl = new TemplatesPage( Vertical, templPageFrame, "business_page" );
      tmplyr->addWidget( d->viewBusinessTempl );
      connect(d->viewBusinessTempl->templates,SIGNAL(doubleClicked(QIconViewItem*)),this,SLOT(templateItemExecuted(QIconViewItem*)));
      connect(d->viewBusinessTempl->templates,SIGNAL(returnPressed(QIconViewItem*)),this,SLOT(templateItemExecuted(QIconViewItem*)));
      connect(d->viewBusinessTempl->templates,SIGNAL(currentChanged(QIconViewItem*)),this,SLOT(templateItemSelected(QIconViewItem*)));
    */
#endif //KEXI_PROJECT_TEMPLATES

    //- page "import db"
// d->templatesSectionID_import = itemID++;
    templPageWidget = new QFrame(d->templatesWidget);
    d->templPageWidgetItem_ImportExisting = d->templatesWidget->addPage(templPageWidget,
                                            i18n("Import Existing Database"));
    d->templPageWidgetItem_ImportExisting->setHeader(
        i18n("Import Existing Database as New Database Project"));
    d->templPageWidgetItem_ImportExisting->setIcon(koIcon("document_import_database"));
    tmplyr = new QVBoxLayout(templPageWidget);
    tmplyr->setSpacing(KDialog::spacingHint());
    QLabel *lbl_import = new QLabel(
        i18n("Kexi will import the structure and data of an existing database "
             "as a new database project.") + clickMsg, templPageWidget);
    lbl_import->setAlignment(Qt::AlignAuto | Qt::AlignTop);
    lbl_import->setWordWrap(true);
    lbl_import->setMargin(0);
    tmplyr->addWidget(lbl_import);
    tmplyr->addStretch(1);
}

void KexiStartupDialog::slotCurrentPageChanged(KFakePageWidgetItem* current,
        KFakePageWidgetItem* before)
{
    Q_UNUSED(before);
    updateDialogOKButton(current);
}

void KexiStartupDialog::slotCurrentTemplatesubpageChanged(KFakePageWidgetItem* current,
        KFakePageWidgetItem* before)
{
    Q_UNUSED(before);
    if (current == d->templPageWidgetItem_BlankDatabase) {//blank
    } else if (current == d->templPageWidgetItem_ImportExisting) {
    }
#ifdef KEXI_PROJECT_TEMPLATES
    else if (current == d->templPageWidgetItem_CreateFromTemplate) {
        d->viewTemplates->populate();
    }
    /*later?  KIconView *templ = d->viewTemplates->templates;
        if (templ->count()==0) {
          //add items (on demand):
          d->viewTemplates->addItem("cd_catalog", i18n("CD Catalog"),
            i18n("Easy-to-use database for storing information about your CD collection."),
            koDesktopIcon("media-optical"));
          d->viewTemplates->addItem("expenses", i18n("Expenses"),
            i18n("A database for managing your personal expenses."),
            koDesktopIcon("accessories-calculator"));
          d->viewTemplates->addItem("image_gallery", i18n("Image Gallery"),
            i18n("A database for archiving your image collection in a form of gallery."),
            koDesktopIcon("folder-image"));
        }
      }
      else if (idx==d->templatesSectionID_custom2) {//business
        templ = d->viewBusinessTempl->templates;
        if (templ->count()==0) {
          //add items (on demand):
          d->viewBusinessTempl->addItem("address_book", i18n("Address Book"),
            i18n("A database that offers you a contact information"),
            koDesktopIcon("help-contents"));
        }
      }*/
#endif
    updateDialogOKButton(d->pageTemplates);
}

#if 0
void KexiStartupDialog::templateItemSelected(Q3IconViewItem *)
{
    updateDialogOKButton(d->pageTemplates);
}

void KexiStartupDialog::templateItemExecuted(Q3IconViewItem *item)
{
    if (!item)
        return;
// updateSelectedTemplateKeyInfo();
#ifdef KEXI_PROJECT_TEMPLATES
    accept();
#endif
}

void KexiStartupDialog::updateSelectedTemplateKeyInfo()
{
    if (activePageIndex() != d->pageTemplatesID) {//not a 'new db' tab is selected
        d->selectedTemplateKey.clear();
        return;
    }
    Q3IconViewItem *item;
    if (d->templatesWidget->activePageIndex() == d->templatesSectionID_blank) {
        d->selectedTemplateKey = "blank";
    } else if (d->templatesWidget->activePageIndex() == d->templatesSectionID_import) {
        d->selectedTemplateKey = "import";
    }
#ifdef KEXI_PROJECT_TEMPLATES
    else if (d->templatesWidget->activePageIndex() == d->templatesSectionID_templates) {
        item = d->viewTemplates->templates->currentItem();
        if (!item) {
            d->selectedTemplateKey.clear();
            return;
        }
        d->selectedTemplateKey = QString("personal/") + static_cast<TemplateItem*>(item)->key;
    }
    /*later?
      else  if (d->templatesWidget->activePageIndex()==d->templatesSectionID_custom2) {
        item = d->viewBusinessTempl->templates->currentItem();
        if (!item) {
          d->selectedTemplateKey.clear();
          return;
        }
        d->selectedTemplateKey=QString("business/")+static_cast<TemplateItem*>(item)->key;
      }*/
#endif
}
#endif // 0

/*
void KexiStartupDialog::tabShown(QWidget *w)
{
  updateDialogOKButton(w);

  if (w==d->pageOpenExisting) {
    d->openExistingConnWidget->setFocus();
  }
}*/

void KexiStartupDialog::updateDialogOKButton(KFakePageWidgetItem *pageWidgetItem)
{
    if (!pageWidgetItem) {
        pageWidgetItem = currentPage();
        /*  int idx = activePageIndex();
            if (idx==d->pageTemplatesID)
              w = d->pageTemplates;
            else if (idx==d->pageOpenExistingID)
              w = d->pageOpenExisting;
            else if (idx==d->pageOpenRecentID)
              w = d->pageOpenRecent;*/

        if (!pageWidgetItem)
            return;
    }
    bool enable = true;
    if (pageWidgetItem == d->pageTemplates) {
        //int t_id = d->templatesWidget->activePageIndex();
#ifdef DB_TEMPLATES
        KFakePageWidgetItem *currenTemplatesPageWidgetItem = d->templatesWidget->currentPage();
#ifdef KEXI_PROJECT_TEMPLATES
        enable =
            currenTemplatesPageWidgetItem == d->templPageWidgetItem_BlankDatabase
            || currenTemplatesPageWidgetItem == d->templPageWidgetItem_ImportExisting
            || (currenTemplatesPageWidgetItem == d->templPageWidgetItem_CreateFromTemplate && !d->viewTemplates->selectedFileName().isEmpty());
#else
        enable = currenTemplatesPageWidgetItem == d->templPageWidgetItem_BlankDatabase
                 || currenTemplatesPageWidgetItem == d->templPageWidgetItem_ImportExisting;
#endif
    } else if (pageWidgetItem == d->pageOpenExisting) {
        kDebug() << "d->openExistingFileWidget->highlightedFile(): " << d->openExistingFileWidget->highlightedFile();
        enable =
            (d->openExistingConnWidget->selectedConnectionType() == KexiConnectionSelectorWidget::FileBased)
//kde4   ? !d->openExistingFileWidget->selectedFile().isEmpty()
            ? !d->openExistingFileWidget->highlightedFile().isEmpty()
            : (bool)d->openExistingConnWidget->selectedConnectionData();
//kDebug() << d->openExistingFileWidget->selectedFile() << "--------------";
    } else if (pageWidgetItem == d->pageOpenRecent) {
        enable = (d->prj_selector->selectedProjectData() != 0);
    }
    enableButton(Ok, enable);
}

/*QString KexiStartupDialog::selectedTemplateKey() const
{
  return d->selectedTemplateKey;
}*/

void KexiStartupDialog::setupPageOpenExisting()
{
// if (d->singlePage)
    QWidget *pageOpenExistingWidget = new QFrame(this);
    d->pageOpenExisting = addPage(pageOpenExistingWidget, i18n("Open Existing Project"));

    QVBoxLayout *lyr = new QVBoxLayout(pageOpenExistingWidget);
    lyr->setSpacing(KDialog::spacingHint());
    lyr->setMargin(0);
    QString recentDirClass;
    KFileWidget::getStartUrl(KUrl("kfiledialog:///OpenExistingOrCreateNewProject"),
                             recentDirClass);
    kDebug() << recentDirClass;

    d->openExistingConnWidget = new KexiConnectionSelectorWidget(*d->connSet,
            "kfiledialog:///OpenExistingOrCreateNewProject", KAbstractFileWidget::Opening,
            pageOpenExistingWidget);
    d->openExistingConnWidget->setObjectName("KexiConnectionSelectorWidget");
    d->openExistingConnWidget->hideConnectonIcon();
    lyr->addWidget(d->openExistingConnWidget);
    KConfigGroup group = KGlobal::config()->group("Startup");
    if (group.readEntry("OpenExistingType", "File") == "File")
        d->openExistingConnWidget->showSimpleConn();
    else {
        d->openExistingConnWidget->showSimpleConn();
        d->openExistingConnWidget->showAdvancedConn();
    }
    d->openExistingFileWidget = d->openExistingConnWidget->fileWidget;
    connect(d->openExistingFileWidget, SIGNAL(accepted()), this, SLOT(accept()));
    connect(d->openExistingFileWidget, SIGNAL(fileHighlighted()),
            this, SLOT(existingFileHighlighted()));
    connect(d->openExistingConnWidget, SIGNAL(connectionItemExecuted(ConnectionDataLVItem*)),
            this, SLOT(connectionItemForOpenExistingExecuted(ConnectionDataLVItem*)));
    connect(d->openExistingConnWidget, SIGNAL(connectionItemHighlighted(ConnectionDataLVItem*)),
            this, SLOT(connectionItemForOpenExistingHighlighted(ConnectionDataLVItem*)));
}

void KexiStartupDialog::connectionItemForOpenExistingExecuted(ConnectionDataLVItem *item)
{
    if (!item)
        return;
    accept();
}

void KexiStartupDialog::connectionItemForOpenExistingHighlighted(ConnectionDataLVItem *item)
{
    enableButtonOk(item);
}

void KexiStartupDialog::slotOk()
{
// kDebug()<<"KexiStartupDialog::slotOk()";
    if (currentPage() == d->pageOpenExisting) {
#ifdef __GNUC__
#warning UNUSED? KFileWidget  if (d->openExistingFileDlg) {
#else
#pragma WARNING( UNUSED? KFileWidget  if (d->openExistingFileDlg) { )
#endif
#ifdef __GNUC__
#warning UNUSED?    if (d->openExistingFileDlg->okButton())
#else
#pragma WARNING( UNUSED?    if (d->openExistingFileDlg->okButton()) )
#endif
#ifdef __GNUC__
#warning UNUSED?     d->openExistingFileDlg->okButton()->animateClick();
#else
#pragma WARNING( UNUSED?     d->openExistingFileDlg->okButton()->animateClick(); )
#endif
//   return;
#ifdef __GNUC__
#warning UNUSED?  }
#else
#pragma WARNING( UNUSED?  } )
#endif
    }
}

void KexiStartupDialog::showSimpleConnForOpenExisting()
{
// kDebug() << "simple";
    d->openExistingConnWidget->showSimpleConn();
}

void KexiStartupDialog::showAdvancedConnForOpenExisting()
{
// kDebug() << "adv";
    d->openExistingConnWidget->showAdvancedConn();
}

QString KexiStartupDialog::selectedFileName() const
{
    if (d->result == OpenExistingResult)
        return d->openExistingFileWidget->highlightedFile();
    else if (d->result == CreateFromTemplateResult && d->viewTemplates)
        return d->viewTemplates->selectedFileName();
    else
        return QString();
}

KexiDB::ConnectionData* KexiStartupDialog::selectedExistingConnection() const
{
    return d->selectedExistingConnection;
}

void KexiStartupDialog::existingFileHighlighted()
{
    kDebug() << "KexiStartupDialog::existingFileHighlighted(): ";
    //d->existingUrlToOpen = KUrl(fileName);
    updateDialogOKButton(0);
}

void KexiStartupDialog::setupPageOpenRecent()
{
#ifdef KEXI_STARTUP_SHOW_RECENT
    QWidget *pageOpenRecentWidget = new QFrame(this);
    d->pageOpenRecent = addPage(pageOpenRecentWidget, i18n("Open Recent Project"));
    QVBoxLayout *lyr = new QVBoxLayout(pageOpenRecentWidget);
    lyr->setSpacing(KDialog::spacingHint());
    lyr->addWidget(d->prj_selector
                   = new KexiProjectSelectorWidget(pageOpenRecentWidget, d->recentProjects)
                  );
    connect(d->prj_selector, SIGNAL(projectExecuted(KexiProjectData*)),
            this, SLOT(recentProjectItemExecuted(KexiProjectData*)));
#endif
}

KexiProjectData* KexiStartupDialog::selectedProjectData() const
{
    if (currentPage() == d->pageOpenRecent) {
        return d->prj_selector->selectedProjectData();
    }
    return 0;
}

void KexiStartupDialog::recentProjectItemExecuted(KexiProjectData *data)
{
    updateDialogOKButton(d->pageOpenRecent);
    if (!data)
        return;
    accept();
}

//! used for accepting templates dialog with just return key press
bool KexiStartupDialog::eventFilter(QObject *o, QEvent *e)
{
    if (o == d->templatesWidget_IconListView && d->templatesWidget_IconListView) {
        bool tryAcept = false;
        if (   e->type() == QEvent::KeyPress
            && (   static_cast<QKeyEvent*>(e)->key() == Qt::Key_Enter
                || static_cast<QKeyEvent*>(e)->key() == Qt::Key_Return)
           )
        {
            tryAcept = true;
        }
        else if (e->type() == QEvent::MouseButtonDblClick) {
            tryAcept = true;
        }

        if (tryAcept) {
            KFakePageWidgetItem *currentTemplatesPageWidgetItem = d->templatesWidget->currentPage();
            if (   currentTemplatesPageWidgetItem == d->templPageWidgetItem_BlankDatabase
                || currentTemplatesPageWidgetItem == d->templPageWidgetItem_ImportExisting)
            {
                accept();
            }
        }
    }
    return KoPageDialog::eventFilter(o, e);
}

// internal reimplementation
/*int KexiStartupDialog::activePageIndex() const
{
  if (!d->singlePage) {
//  kDebug() << "int KexiStartupDialog::activePageIndex()" << KDialog::activePageIndex();
    return KDialog::activePageIndex();
  }
  kDebug() << "int KexiStartupDialog::activePageIndex() == " << 0;
  return 0; //there is always "plain page" #0 selected
}*/

void KexiStartupDialog::templateSelected(const QString& fileName)
{
    if (!fileName.isEmpty())
        accept();
}

const KexiProjectData::AutoOpenObjects& KexiStartupDialog::autoopenObjects() const
{
    if (d->result != CreateFromTemplateResult || !d->viewTemplates)
        KexiProjectData::AutoOpenObjects();

    return d->viewTemplates->autoopenObjectsForSelectedTemplate();
}

#include "KexiStartupDialog.moc"
