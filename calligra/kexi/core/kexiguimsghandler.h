/* This file is part of the KDE project
   Copyright (C) 2004-2013 Jarosław Staniek <staniek@kde.org>

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

#ifndef KEXIGUIMSGHANDLER_H
#define KEXIGUIMSGHANDLER_H

#include <core/kexi.h>
#include <db/msghandler.h>

class KEXICORE_EXPORT KexiGUIMessageHandler : public KexiDB::MessageHandler
{
public:
    explicit KexiGUIMessageHandler(QWidget *parent = 0);
    virtual ~KexiGUIMessageHandler();

    using KexiDB::MessageHandler::showErrorMessage;

    void showErrorMessage(const QString&, const QString&, KexiDB::Object *obj);
    void showErrorMessage(Kexi::ObjectStatus *status);
    void showErrorMessage(const QString &message, Kexi::ObjectStatus *status);

    /*! Displays a "Sorry" message with \a title text and optional \a details. */
    void showSorryMessage(const QString &title, const QString &details = QString());

    /*! Displays a message of a type \a type, with \a title text and optional \a details.
     \a dontShowAgainName can be specified to add "Do not show again" option if \a type is Warning. */
    virtual void showMessage(MessageType type, const QString &title, const QString &details,
                             const QString& dontShowAgainName = QString());

    /*! Displays a Warning message with \a title text and optional \a details
     with "Continue" button instead "OK".
     \a dontShowAgainName can be specified to add "Do not show again" option. */
    virtual void showWarningContinueMessage(const QString &title, const QString &details = QString(),
                                            const QString& dontShowAgainName = QString());

protected:
    using KexiDB::MessageHandler::showErrorMessageInternal;

    virtual void showErrorMessageInternal(const QString &title, const QString &details = QString());
    virtual void showErrorMessageInternal(KexiDB::Object *obj, const QString& msg = QString());

    /*! Interactively asks a question using KMessageBox.
     See KexiDB::MessageHandler::askQuestionInternal() for details. */
    virtual int askQuestionInternal(const QString& message,
                                    KMessageBox::DialogType dlgType, KMessageBox::ButtonCode defaultResult,
                                    const KGuiItem &buttonYes = KStandardGuiItem::yes(),
                                    const KGuiItem &buttonNo = KStandardGuiItem::no(),
                                    const QString &dontShowAskAgainName = QString(),
                                    KMessageBox::Options options = KMessageBox::Notify);
};

#endif
