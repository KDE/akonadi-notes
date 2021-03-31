/*  This file is part of the KDE project
    SPDX-FileCopyrightText: 2011 Christian Mollekopf <chrigi_1@fastmail.fm>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "akonadi-notes_export.h"

#include <QMap>
#include <QUrl>
class QDateTime;
class QString;

template<typename T> class QSharedPointer;

namespace KMime
{
class Message;
typedef QSharedPointer<Message> MessagePtr;
}
namespace Akonadi
{
namespace NoteUtils
{
/**
 * @return mimetype for notes
 * @since 4.8
 */
AKONADI_NOTES_EXPORT QString noteMimeType();

/**
 * @return icon for notes
 * @since 4.8
 */
AKONADI_NOTES_EXPORT QString noteIconName();

/**
 * An attachment for a note
 * @since 4.9
 */
class AKONADI_NOTES_EXPORT Attachment
{
public:
    /**
     * Create an attachment referencing a url only
     */
    Attachment();
    Attachment(const QUrl &url, const QString &mimetype);
    /**
     * Create an attachment with the content stored inline
     */
    Attachment(const QByteArray &data, const QString &mimetype);
    Attachment(const Attachment &other);
    ~Attachment();

    bool operator==(const Attachment &a) const;
    void operator=(const Attachment &a);

    /**
     * Returns the url for url-only attachments
     */
    QUrl url() const;

    /**
     * Returns the date for inline attachments
     */
    QByteArray data() const;

    /**
     * Sets the unique identifier of the attachment
     *
     * It can be used to refer to attachment from the note itself
     *
     * @since 5.15
     */
    void setContentID(const QString &contentID);

    /**
     * Returns the unique identifier for inline attachment
     *
     * @since 5.15
     */
    QString contentID() const;

    /**
     * Set this to true if inline data provided via ctor
     * is alredy base64 encoded. Default value is false.
     *
     * @since 5.15
     */
    void setDataBase64Encoded(bool encoded);

    /**
     * Returns true if data is already base64-encoded
     *
     * @since 5.15
     */
    bool dataBase64Encoded() const;

    /**
     * Returns the mimetype
     */
    QString mimetype() const;

    /**
     * Sets the label to be presented to the user
     */
    void setLabel(const QString &label);

    /**
     * Returns the label of the attachment
     */
    QString label() const;

private:
    //@cond PRIVATE
    class AttachmentPrivate;
    AttachmentPrivate *const d_ptr;
    Q_DECLARE_PRIVATE(Attachment)
    //@endcond
};

/**
 * A convenience wrapper around KMime::MessagePtr for notes
 *
 * This is the format used by the Akonotes Resource
 *
 * A note has the following properties:
 * uid: globally unique identifier (generated if empty)
 * creationDate: timestamp when the note was created (generated if empty)
 * lastModified: lastModified (generated if empty)
 * classification: one of private, confidential, public. This is only meant as an indication to the user.
 * title: title of the note
 * text: textual content
 * from: author (generated if empty)
 * attachments: inline or url only
 * custom: key value pair for custom values
 *
 * Reading a note from an Akonotes akonadi item:
 * @code
 * if ( item.hasPayload<KMime::MessagePtr>() ) {
 *   NoteUtils::NoteMessageWrapper note(item.payload<KMime::MessagePtr>());
 *   qCDebug(AKONADINOTES_LOG) << note.text();
 *   textIsRich = messageWrapper.textFormat() == Qt::RichText;
 * }
 * @endcode
 *
 * Setting the note as payload of an akonadi Item
 * @code
 * item.setMimeType(NoteUtils::noteMimeType());
 * NoteUtils::NoteMessageWrapper note;
 * note.setTitle( "title" );
 * note.setText( "text" );
 * note.setFrom( QString::fromLatin1( "MyApplication@kde4" ) );
 * item.setPayload( note.message() );
 * @endcode
 *
 * @author Christian Mollekopf <chrigi_1@fastmail.fm>
 * @since 4.8
 */
class AKONADI_NOTES_EXPORT NoteMessageWrapper
{
public:
    NoteMessageWrapper();
    explicit NoteMessageWrapper(const KMime::MessagePtr &msg);
    ~NoteMessageWrapper();

    /**
     * Set the uid of the note
     * @param uid should be globally unique
     */
    void setUid(const QString &uid);

    /**
     * Returns the uid of the note
     */
    QString uid() const;

    enum Classification { Public, Private, Confidential };

    /**
     * Set the classification of the note
     */
    void setClassification(Classification);

    /**
     * Returns the classification of the note
     */
    Classification classification() const;

    /**
     * Set the title of the note
     */
    void setTitle(const QString &title);

    /**
     * Returns the title of the note
     */
    QString title() const;

    /**
     * Set the text of the note
     *
     * @param format only Qt::PlainText and Qt::RichText is supported
     */
    void setText(const QString &text, Qt::TextFormat format = Qt::PlainText);

    /**
     * Returns the text of the note
     */
    QString text() const;

    /**
     * @return Qt::PlainText or Qt::RichText
     */
    Qt::TextFormat textFormat() const;

    /**
     * @return plaintext version of the text (if richtext)
     */
    QString toPlainText() const;

    /**
     * Set the creation date of the note (stored in the mime header)
     */
    void setCreationDate(const QDateTime &creationDate);

    /**
     * Returns the creation date of the note
     */
    QDateTime creationDate() const;

    /**
     * Set the lastModified-date of the note
     */
    void setLastModifiedDate(const QDateTime &lastModifiedDate);

    /**
     * Returns the lastModified-date of the note
     */
    QDateTime lastModifiedDate() const;

    /**
     * Set the origin (creator) of the note (stored in the mime header)
     * This is usually the application creating the note.
     * @param from must be an address in the style of foo@kde.org.
     */
    void setFrom(const QString &from);

    /**
     * Returns the origin (creator) of the note
     */
    QString from() const;

    /**
     * Returns a reference to the list of attachments of the note
     */
    QVector<Attachment> &attachments();

    /**
     * Returns a reference to the custom-value map
     * @return key-value map containing all custom values
     */
    QMap<QString, QString> &custom();

    /**
     * Assemble a KMime message with the given values
     *
     * The message can then i.e. be stored inside an akonadi item
     */
    KMime::MessagePtr message() const;

private:
    //@cond PRIVATE
    Q_DISABLE_COPY(NoteMessageWrapper)
    class NoteMessageWrapperPrivate;
    NoteMessageWrapperPrivate *const d_ptr;
    Q_DECLARE_PRIVATE(NoteMessageWrapper)
    //@endcond
};

}
}

Q_DECLARE_TYPEINFO(Akonadi::NoteUtils::Attachment, Q_MOVABLE_TYPE);
