#ifndef FAKE_KINPUTDIALOG_H
#define FAKE_KINPUTDIALOG_H

#include <kdialog.h>
#include <QInputDialog>
#include <QIntValidator>

#include <kofake_export.h>

class KOFAKE_EXPORT KInputDialog : public QInputDialog
{
public:
    KInputDialog(QWidget *widget = 0) : QInputDialog(widget) {}

    static QString getText(const QString &title, const QString &label = QString(), const QString &value=QString(), bool *ok=0, QWidget *parent=0, QValidator *validator=0, const QString &mask=QString(), const QString& whatsThis=QString(), const QStringList &completionList=QStringList())
    {
#if QT_VERSION < 0x050000
        KInputDialog dlg(parent);
        dlg.setWindowTitle(title);
        dlg.setLabelText(label);
        dlg.setTextValue(value);
        bool accpted = dlg.exec() == QDialog::Accepted;
        if (ok)
            *ok = accpted;
        if (!accpted)
            return QString();
        return dlg.textValue();
#else
        return QInputDialog::getText(parent, title, label, QLineEdit::Normal, value, ok);
#endif
    }

};

#endif
