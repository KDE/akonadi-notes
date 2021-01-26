/*  This file is part of the KDE project
    SPDX-FileCopyrightText: 2011 Christian Mollekopf <chrigi_1@fastmail.fm>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "noteutils.h"

#include "akonadi_notes_debug.h"
#include <KLocalizedString>
#include <QDateTime>
#include <kmime/kmime_message.h>

#include <QRegularExpression>
#include <QString>
#include <QUuid>
#include <qdom.h>

namespace Akonadi
{
namespace NoteUtils
{
#define X_NOTES_UID_HEADER "X-Akonotes-UID"
#define X_NOTES_LASTMODIFIED_HEADER "X-Akonotes-LastModified"
#define X_NOTES_CLASSIFICATION_HEADER "X-Akonotes-Classification"
#define X_NOTES_CUSTOM_HEADER "X-Akonotes-Custom"

#define CLASSIFICATION_PUBLIC QStringLiteral("Public")
#define CLASSIFICATION_PRIVATE QStringLiteral("Private")
#define CLASSIFICATION_CONFIDENTIAL QStringLiteral("Confidential")

#define X_NOTES_URL_HEADER "X-Akonotes-Url"
#define X_NOTES_LABEL_HEADER "X-Akonotes-Label"
#define X_NOTES_CONTENTTYPE_HEADER "X-Akonotes-Type"
#define CONTENT_TYPE_CUSTOM QStringLiteral("custom")
#define CONTENT_TYPE_ATTACHMENT QStringLiteral("attachment")

#define ENCODING "utf-8"

class Q_DECL_HIDDEN Attachment::AttachmentPrivate
{
public:
    AttachmentPrivate(const QUrl &url, const QString &mimetype)
        : mUrl(url)
        , mMimetype(mimetype)
    {
    }

    AttachmentPrivate(const QByteArray &data, const QString &mimetype)
        : mData(data)
        , mMimetype(mimetype)
    {
    }

    AttachmentPrivate(const AttachmentPrivate &other)
    {
        *this = other;
    }

    QUrl mUrl;
    QByteArray mData;
    bool mDataBase64Encoded = false;
    QString mMimetype;
    QString mLabel;
    QString mContentID;
};

Attachment::Attachment()
    : d_ptr(new Attachment::AttachmentPrivate(QUrl(), QString()))
{
}

Attachment::Attachment(const QUrl &url, const QString &mimetype)
    : d_ptr(new Attachment::AttachmentPrivate(url, mimetype))
{
}

Attachment::Attachment(const QByteArray &data, const QString &mimetype)
    : d_ptr(new Attachment::AttachmentPrivate(data, mimetype))
{
}

Attachment::Attachment(const Attachment &other)
    : d_ptr(new AttachmentPrivate(*other.d_func()))
{
}

Attachment::~Attachment()
{
    delete d_ptr;
}

bool Attachment::operator==(const Attachment &a) const
{
    Q_D(const Attachment);
    if (d->mUrl.isEmpty()) {
        return d->mUrl == a.d_func()->mUrl && d->mDataBase64Encoded == a.d_func()->mDataBase64Encoded && d->mMimetype == a.d_func()->mMimetype
            && d->mContentID == a.d_func()->mContentID && d->mLabel == a.d_func()->mLabel;
    }
    return d->mData == a.d_func()->mData && d->mDataBase64Encoded == a.d_func()->mDataBase64Encoded && d->mMimetype == a.d_func()->mMimetype
        && d->mContentID == a.d_func()->mContentID && d->mLabel == a.d_func()->mLabel;
}

void Attachment::operator=(const Attachment &a)
{
    *d_ptr = *a.d_ptr;
}

QUrl Attachment::url() const
{
    Q_D(const Attachment);
    return d->mUrl;
}

QByteArray Attachment::data() const
{
    Q_D(const Attachment);
    return d->mData;
}

void Attachment::setDataBase64Encoded(bool encoded)
{
    Q_D(Attachment);
    d->mDataBase64Encoded = true;
}

bool Attachment::dataBase64Encoded() const
{
    Q_D(const Attachment);
    return d->mDataBase64Encoded;
}

void Attachment::setContentID(const QString &contentID)
{
    Q_D(Attachment);
    d->mContentID = contentID;
}

QString Attachment::contentID() const
{
    Q_D(const Attachment);
    return d->mContentID;
}

QString Attachment::mimetype() const
{
    Q_D(const Attachment);
    return d->mMimetype;
}

void Attachment::setLabel(const QString &label)
{
    Q_D(Attachment);
    d->mLabel = label;
}

QString Attachment::label() const
{
    Q_D(const Attachment);
    return d->mLabel;
}

class Q_DECL_HIDDEN NoteMessageWrapper::NoteMessageWrapperPrivate
{
public:
    NoteMessageWrapperPrivate()
    {
    }

    NoteMessageWrapperPrivate(const KMime::MessagePtr &msg)
    {
        readMimeMessage(msg);
    }

    void readMimeMessage(const KMime::MessagePtr &msg);

    KMime::Content *createCustomPart() const;
    void parseCustomPart(KMime::Content *);

    KMime::Content *createAttachmentPart(const Attachment &) const;
    void parseAttachmentPart(KMime::Content *);

    QString uid;
    QString title;
    QString text;
    QString from;
    QDateTime creationDate;
    QDateTime lastModifiedDate;
    QMap<QString, QString> custom;
    QVector<Attachment> attachments;
    Classification classification = Public;
    Qt::TextFormat textFormat = Qt::PlainText;
};

void NoteMessageWrapper::NoteMessageWrapperPrivate::readMimeMessage(const KMime::MessagePtr &msg)
{
    if (!msg.data()) {
        qCWarning(AKONADINOTES_LOG) << "Empty message";
        return;
    }
    title = msg->subject(true)->asUnicodeString();
    text = msg->mainBodyPart()->decodedText(true); // remove trailing whitespace, so we get rid of "  " in empty notes
    if (msg->from(false)) {
        from = msg->from(false)->asUnicodeString();
    }
    creationDate = msg->date(true)->dateTime();
    if (msg->mainBodyPart()->contentType(false) && msg->mainBodyPart()->contentType()->mimeType() == "text/html") {
        textFormat = Qt::RichText;
    }

    if (KMime::Headers::Base *lastmod = msg->headerByType(X_NOTES_LASTMODIFIED_HEADER)) {
        lastModifiedDate = QDateTime::fromString(lastmod->asUnicodeString(), Qt::RFC2822Date);
        if (!lastModifiedDate.isValid()) {
            qCWarning(AKONADINOTES_LOG) << "failed to parse lastModifiedDate";
        }
    }

    if (KMime::Headers::Base *uidHeader = msg->headerByType(X_NOTES_UID_HEADER)) {
        uid = uidHeader->asUnicodeString();
    }

    if (KMime::Headers::Base *classificationHeader = msg->headerByType(X_NOTES_CLASSIFICATION_HEADER)) {
        const QString &c = classificationHeader->asUnicodeString();
        if (c == CLASSIFICATION_PRIVATE) {
            classification = Private;
        } else if (c == CLASSIFICATION_CONFIDENTIAL) {
            classification = Confidential;
        }
    }

    const auto list = msg->contents();
    for (KMime::Content *c : list) {
        if (KMime::Headers::Base *typeHeader = c->headerByType(X_NOTES_CONTENTTYPE_HEADER)) {
            const QString &type = typeHeader->asUnicodeString();
            if (type == CONTENT_TYPE_CUSTOM) {
                parseCustomPart(c);
            } else if (type == CONTENT_TYPE_ATTACHMENT) {
                parseAttachmentPart(c);
            } else {
                qCWarning(AKONADINOTES_LOG) << "unknown type " << type;
            }
        }
    }
}

QDomDocument createXMLDocument()
{
    QDomDocument document;
    const QString p = QStringLiteral("version=\"1.0\" encoding=\"UTF-8\"");
    document.appendChild(document.createProcessingInstruction(QStringLiteral("xml"), p));
    return document;
}

QDomDocument loadDocument(KMime::Content *part)
{
    QString errorMsg;
    int errorLine, errorColumn;
    QDomDocument document;
    bool ok = document.setContent(part->body(), &errorMsg, &errorLine, &errorColumn);
    if (!ok) {
        qCWarning(AKONADINOTES_LOG) << part->body();
        qWarning("Error loading document: %s, line %d, column %d", qPrintable(errorMsg), errorLine, errorColumn);
        return QDomDocument();
    }
    return document;
}

KMime::Content *NoteMessageWrapper::NoteMessageWrapperPrivate::createCustomPart() const
{
    KMime::Content *content = new KMime::Content();
    auto header = new KMime::Headers::Generic(X_NOTES_CONTENTTYPE_HEADER);
    header->fromUnicodeString(CONTENT_TYPE_CUSTOM, ENCODING);
    content->appendHeader(header);
    QDomDocument document = createXMLDocument();
    QDomElement element = document.createElement(QStringLiteral("custom"));
    element.setAttribute(QStringLiteral("version"), QStringLiteral("1.0"));
    QMap<QString, QString>::const_iterator end = custom.end();
    for (QMap<QString, QString>::const_iterator it = custom.begin(); it != end; ++it) {
        QDomElement e = element.ownerDocument().createElement(it.key());
        QDomText t = element.ownerDocument().createTextNode(it.value());
        e.appendChild(t);
        element.appendChild(e);
        document.appendChild(element);
    }
    content->setBody(document.toString().toLatin1());
    return content;
}

void NoteMessageWrapper::NoteMessageWrapperPrivate::parseCustomPart(KMime::Content *part)
{
    QDomDocument document = loadDocument(part);
    if (document.isNull()) {
        return;
    }
    QDomElement top = document.documentElement();
    if (top.tagName() != QLatin1String("custom")) {
        qWarning("XML error: Top tag was %s instead of the expected custom", top.tagName().toLatin1().data());
        return;
    }

    for (QDomNode n = top.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (n.isElement()) {
            QDomElement e = n.toElement();
            custom.insert(e.tagName(), e.text());
        } else {
            qCDebug(AKONADINOTES_LOG) << "Node is not an element";
            Q_ASSERT(false);
        }
    }
}

KMime::Content *NoteMessageWrapper::NoteMessageWrapperPrivate::createAttachmentPart(const Attachment &a) const
{
    KMime::Content *content = new KMime::Content();
    auto header = new KMime::Headers::Generic(X_NOTES_CONTENTTYPE_HEADER);
    header->fromUnicodeString(CONTENT_TYPE_ATTACHMENT, ENCODING);
    content->appendHeader(header);
    if (a.url().isValid()) {
        header = new KMime::Headers::Generic(X_NOTES_URL_HEADER);
        header->fromUnicodeString(a.url().toString(), ENCODING);
        content->appendHeader(header);
    } else {
        content->setBody(a.data());
    }
    content->contentType()->setMimeType(a.mimetype().toLatin1());
    if (!a.label().isEmpty()) {
        header = new KMime::Headers::Generic(X_NOTES_LABEL_HEADER);
        header->fromUnicodeString(a.label(), ENCODING);
        content->appendHeader(header);
    }
    content->contentTransferEncoding()->setEncoding(KMime::Headers::CEbase64);
    if (a.dataBase64Encoded()) {
        content->contentTransferEncoding()->setDecoded(false);
    }
    content->contentDisposition()->setDisposition(KMime::Headers::CDattachment);
    content->contentDisposition()->setFilename(QStringLiteral("attachment"));
    if (!a.contentID().isEmpty()) {
        content->contentID()->setIdentifier(a.contentID().toLatin1());
    }
    return content;
}

void NoteMessageWrapper::NoteMessageWrapperPrivate::parseAttachmentPart(KMime::Content *part)
{
    QString label;
    if (KMime::Headers::Base *labelHeader = part->headerByType(X_NOTES_LABEL_HEADER)) {
        label = labelHeader->asUnicodeString();
    }
    if (KMime::Headers::Base *header = part->headerByType(X_NOTES_URL_HEADER)) {
        Attachment attachment(QUrl(header->asUnicodeString()), QLatin1String(part->contentType()->mimeType()));
        attachment.setLabel(label);
        attachment.setContentID(QString::fromLatin1(part->contentID()->identifier()));
        attachments.append(attachment);
    } else {
        Attachment attachment(part->decodedContent(), QLatin1String(part->contentType()->mimeType()));
        attachment.setLabel(label);
        attachment.setContentID(QString::fromLatin1(part->contentID()->identifier()));
        attachments.append(attachment);
    }
}

NoteMessageWrapper::NoteMessageWrapper()
    : d_ptr(new NoteMessageWrapperPrivate())
{
}

NoteMessageWrapper::NoteMessageWrapper(const KMime::MessagePtr &msg)
    : d_ptr(new NoteMessageWrapperPrivate(msg))
{
}

NoteMessageWrapper::~NoteMessageWrapper()
{
    delete d_ptr;
}

KMime::MessagePtr NoteMessageWrapper::message() const
{
    Q_D(const NoteMessageWrapper);
    KMime::MessagePtr msg = KMime::MessagePtr(new KMime::Message());

    QString title = i18nc("The default name for new notes.", "New Note");
    if (!d->title.isEmpty()) {
        title = d->title;
    }
    // Need a non-empty body part so that the serializer regards this as a valid message.
    QString text = QStringLiteral("  ");
    if (!d->text.isEmpty()) {
        text = d->text;
    }

    QDateTime creationDate = QDateTime::currentDateTime();
    if (d->creationDate.isValid()) {
        creationDate = d->creationDate;
    }

    QDateTime lastModifiedDate = QDateTime::currentDateTime();
    if (d->lastModifiedDate.isValid()) {
        lastModifiedDate = d->lastModifiedDate;
    }

    QString uid;
    if (!d->uid.isEmpty()) {
        uid = d->uid;
    } else {
        uid = QUuid::createUuid().toString().mid(1, 36);
    }

    msg->subject(true)->fromUnicodeString(title, ENCODING);
    msg->date(true)->setDateTime(creationDate);
    msg->from(true)->fromUnicodeString(d->from, ENCODING);
    const QString formatDate = QLocale::c().toString(lastModifiedDate, QStringLiteral("ddd, ")) + lastModifiedDate.toString(Qt::RFC2822Date);

    auto header = new KMime::Headers::Generic(X_NOTES_LASTMODIFIED_HEADER);
    header->fromUnicodeString(formatDate, ENCODING);
    msg->appendHeader(header);
    header = new KMime::Headers::Generic(X_NOTES_UID_HEADER);
    header->fromUnicodeString(uid, ENCODING);
    msg->appendHeader(header);

    QString classification = CLASSIFICATION_PUBLIC;
    switch (d->classification) {
    case Private:
        classification = CLASSIFICATION_PRIVATE;
        break;
    case Confidential:
        classification = CLASSIFICATION_CONFIDENTIAL;
        break;
    default:
        // do nothing
        break;
    }
    header = new KMime::Headers::Generic(X_NOTES_CLASSIFICATION_HEADER);
    header->fromUnicodeString(classification, ENCODING);
    msg->appendHeader(header);

    for (const Attachment &a : qAsConst(d->attachments)) {
        msg->addContent(d->createAttachmentPart(a));
    }

    if (!d->custom.isEmpty()) {
        msg->addContent(d->createCustomPart());
    }

    msg->mainBodyPart()->contentType(true)->setCharset(ENCODING);
    msg->mainBodyPart()->fromUnicodeString(text);
    msg->mainBodyPart()->contentType(true)->setMimeType(d->textFormat == Qt::RichText ? "text/html" : "text/plain");

    msg->assemble();
    return msg;
}

void NoteMessageWrapper::setUid(const QString &uid)
{
    Q_D(NoteMessageWrapper);
    d->uid = uid;
}

QString NoteMessageWrapper::uid() const
{
    Q_D(const NoteMessageWrapper);
    return d->uid;
}

void NoteMessageWrapper::setClassification(NoteMessageWrapper::Classification classification)
{
    Q_D(NoteMessageWrapper);
    d->classification = classification;
}

NoteMessageWrapper::Classification NoteMessageWrapper::classification() const
{
    Q_D(const NoteMessageWrapper);
    return d->classification;
}

void NoteMessageWrapper::setLastModifiedDate(const QDateTime &lastModifiedDate)
{
    Q_D(NoteMessageWrapper);
    d->lastModifiedDate = lastModifiedDate;
}

QDateTime NoteMessageWrapper::lastModifiedDate() const
{
    Q_D(const NoteMessageWrapper);
    return d->lastModifiedDate;
}

void NoteMessageWrapper::setCreationDate(const QDateTime &creationDate)
{
    Q_D(NoteMessageWrapper);
    d->creationDate = creationDate;
}

QDateTime NoteMessageWrapper::creationDate() const
{
    Q_D(const NoteMessageWrapper);
    return d->creationDate;
}

void NoteMessageWrapper::setFrom(const QString &from)
{
    Q_D(NoteMessageWrapper);
    d->from = from;
}

QString NoteMessageWrapper::from() const
{
    Q_D(const NoteMessageWrapper);
    return d->from;
}

void NoteMessageWrapper::setTitle(const QString &title)
{
    Q_D(NoteMessageWrapper);
    d->title = title;
}

QString NoteMessageWrapper::title() const
{
    Q_D(const NoteMessageWrapper);
    return d->title;
}

void NoteMessageWrapper::setText(const QString &text, Qt::TextFormat format)
{
    Q_D(NoteMessageWrapper);
    d->text = text;
    d->textFormat = format;
}

QString NoteMessageWrapper::text() const
{
    Q_D(const NoteMessageWrapper);
    return d->text;
}

Qt::TextFormat NoteMessageWrapper::textFormat() const
{
    Q_D(const NoteMessageWrapper);
    return d->textFormat;
}

QString NoteMessageWrapper::toPlainText() const
{
    Q_D(const NoteMessageWrapper);
    if (d->textFormat == Qt::PlainText) {
        return d->text;
    }

    // From cleanHtml in kdepimlibs/kcalutils/incidenceformatter.cpp
    const QRegularExpression rx(QStringLiteral("<body[^>]*>(.*)</body>"), QRegularExpression::CaseInsensitiveOption);
    QString body = rx.match(d->text).captured(1);

    return body.remove(QRegularExpression(QStringLiteral("<[^>]*>"))).trimmed().toHtmlEscaped();
}

QVector<Attachment> &NoteMessageWrapper::attachments()
{
    Q_D(NoteMessageWrapper);
    return d->attachments;
}

QMap<QString, QString> &NoteMessageWrapper::custom()
{
    Q_D(NoteMessageWrapper);
    return d->custom;
}

QString noteIconName()
{
    return QStringLiteral("text-plain");
}

QString noteMimeType()
{
    return QStringLiteral("text/x-vnd.akonadi.note");
}

} // End Namespace
} // End Namespace
