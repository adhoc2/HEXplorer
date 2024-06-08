/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtWidgets>

#include "variantdelegate.h"


VariantDelegate::VariantDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
    boolExp.setPattern("true|false");
    boolExp.setPatternOptions(QRegularExpression::CaseInsensitiveOption);

    byteArrayExp.setPattern("[\\x00-\\xff]*");
    charExp.setPattern(".");
    colorExp.setPattern("\\(([0-9]*),([0-9]*),([0-9]*),([0-9]*)\\)");
    doubleExp.setPattern("");
    pointExp.setPattern("\\((-?[0-9]*),(-?[0-9]*)\\)");
    rectExp.setPattern("\\((-?[0-9]*),(-?[0-9]*),(-?[0-9]*),(-?[0-9]*)\\)");
    signedIntegerExp.setPattern("-?[0-9]*");
    sizeExp = pointExp;
    unsignedIntegerExp.setPattern("[0-9]*");

    dateExp.setPattern("([0-9]{,4})-([0-9]{,2})-([0-9]{,2})");
    timeExp.setPattern("([0-9]{,2}):([0-9]{,2}):([0-9]{,2})");
    dateTimeExp.setPattern(dateExp.pattern() + "T" + timeExp.pattern());
}

void VariantDelegate::paint(QPainter *painter,
                            const QStyleOptionViewItem &option,
                            const QModelIndex &index) const
{

    if (index.column() == 2) {
        QVariant value = index.model()->data(index, Qt::UserRole);
        if (!isSupportedType(value.typeId())) {
            QStyleOptionViewItem myOption = option;
            myOption.state &= ~QStyle::State_Enabled;
            QStyledItemDelegate::paint(painter, myOption, index);
            return;
        }
    }

    QStyledItemDelegate::paint(painter, option, index);
}

QWidget *VariantDelegate::createEditor(QWidget *parent,
        const QStyleOptionViewItem & /* option */,
        const QModelIndex &index) const
{
    if (index.column() < 0)
        return 0;

    QVariant originalValue = index.model()->data(index, Qt::UserRole);
    if (!isSupportedType(originalValue.typeId()))
        return 0;

    QLineEdit *lineEdit = new QLineEdit(parent);
    lineEdit->setFrame(false);

    QRegularExpression regExp = QRegularExpression();

    switch (originalValue.typeId()) {
    case QMetaType::Bool:
        regExp = boolExp;
        break;
    case QMetaType::QByteArray:
        regExp = byteArrayExp;
        break;
    case QMetaType::Char:
        regExp = charExp;
        break;
    case QMetaType::QColor:
        regExp = colorExp;
        break;
    case QMetaType::QDate:
        regExp = dateExp;
        break;
    case QMetaType::QDateTime:
        regExp = dateTimeExp;
        break;
    case QMetaType::Double:
        regExp = doubleExp;
        break;
    case QMetaType::Int:
    case QMetaType::LongLong:
        regExp = signedIntegerExp;
        break;
    case QMetaType::QPoint:
        regExp = pointExp;
        break;
    case QMetaType::QRect:
        regExp = rectExp;
        break;
    case QMetaType::QSize:
        regExp = sizeExp;
        break;
    case QMetaType::QTime:
        regExp = timeExp;
        break;
    case QMetaType::UInt:
    case QMetaType::ULongLong:
        regExp = unsignedIntegerExp;
        break;
    default:
        ;
    }

    if ( regExp != QRegularExpression()) {
        QValidator *validator = new QRegularExpressionValidator(regExp, lineEdit);
        lineEdit->setValidator(validator);
    }

    return lineEdit;
}

void VariantDelegate::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const
{
    QVariant value = index.model()->data(index, Qt::UserRole);
    if (QLineEdit *lineEdit = qobject_cast<QLineEdit *>(editor))
    {
        QModelIndex indexSettingName = index.model()->index(index.row(), 0,index.parent());
        QVariant valueName = indexSettingName.model()->data(indexSettingName, Qt::DisplayRole);

        if (valueName.toString() == "Password")
        {
            lineEdit->setEchoMode(QLineEdit::Password);
            lineEdit->setText(displayText(value));
        }
        else
        {
            lineEdit->setText(displayText(value));
        }
    }
}

void VariantDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                   const QModelIndex &index) const
{
    QLineEdit *lineEdit = qobject_cast<QLineEdit *>(editor);
    if (!lineEdit->isModified())
        return;

    QString text = lineEdit->text();
    const QValidator *validator = lineEdit->validator();
    if (validator) {
        int pos;
        if (validator->validate(text, pos) != QValidator::Acceptable)
            return;
    }

    QVariant originalValue = index.model()->data(index, Qt::UserRole);
    QVariant value;

    switch (originalValue.typeId()) {
    case QVariant::Char:
        value = text.at(0);
        break;
    case QVariant::Color:
        {
        //colorExp.exactMatch(text);
        QRegularExpressionMatch match = colorExp.match(text);
        if (match.hasMatch() && match.capturedLength() == text.length())
        {
            value = QColor(qMin(match.captured(1).toInt(), 255),
                           qMin(match.captured(2).toInt(), 255),
                           qMin(match.captured(3).toInt(), 255),
                           qMin(match.captured(4).toInt(), 255));
        }
        break;
        }

    case QVariant::Date:
        {
            QDate date = QDate::fromString(text, Qt::ISODate);
            if (!date.isValid())
                return;
            value = date;
        }
        break;
    case QVariant::DateTime:
        {
            QDateTime dateTime = QDateTime::fromString(text, Qt::ISODate);
            if (!dateTime.isValid())
                return;
            value = dateTime;
        }
        break;
    case QVariant::Point:
        {
            //pointExp.exactMatch(text);
            QRegularExpressionMatch match = pointExp.match(text);
            value = QPoint(match.captured(1).toInt(), match.captured(2).toInt());
            break;
        }
    case QVariant::Rect:
        {
            //rectExp.exactMatch(text);
            QRegularExpressionMatch match = rectExp.match(text);
            value = QRect(match.captured(1).toInt(), match.captured(2).toInt(),
                          match.captured(3).toInt(), match.captured(4).toInt());
            break;
        }
    case QVariant::Size:
        {
            //sizeExp.exactMatch(text);
            QRegularExpressionMatch match = sizeExp.match(text);
            value = QSize(match.captured(1).toInt(), match.captured(2).toInt());
            break;
    }
    case QVariant::StringList:
        value = text.split(",");
        break;
    case QVariant::Time:
        {
        QTime time = QTime::fromString(text, Qt::ISODate);
            if (!time.isValid())
                return;
            value = time;
        }
        break;
    default:
        value = text;
        int targetTypeId = originalValue.typeId();
        if (value.canConvert(QMetaType(targetTypeId)))
        {
            bool success = value.convert(QMetaType(targetTypeId));
        }
        //value.convert(originalValue.typeId());
    }

    model->setData(index, displayText(value), Qt::DisplayRole);
    model->setData(index, value, Qt::UserRole);
}

bool VariantDelegate::isSupportedType(int type)
{
    switch (type) {
    case QMetaType::Bool:
    case QMetaType::QByteArray:
    case QMetaType::Char:
    case QMetaType::QColor:
    case QMetaType::QDate:
    case QMetaType::QDateTime:
    case QMetaType::Double:
    case QMetaType::Int:
    case QMetaType::LongLong:
    case QMetaType::QPoint:
    case QMetaType::QRect:
    case QMetaType::QSize:
    case QMetaType::QString:
    case QMetaType::QStringList:
    case QMetaType::QTime:
    case QMetaType::UInt:
    case QMetaType::ULongLong:
        return true;
    default:
        return false;
    }
}

QString VariantDelegate::displayText(const QVariant &value)
{
    switch (value.typeId()) {
    case QVariant::Bool:
    case QVariant::ByteArray:
    case QVariant::Char:
    case QVariant::Double:
    case QVariant::Int:
    case QVariant::LongLong:
    case QVariant::String:
    case QVariant::UInt:
    case QVariant::ULongLong:
        return value.toString();
    case QVariant::Color:
        {
            QColor color = qvariant_cast<QColor>(value);
            return QString("(%1,%2,%3,%4)")
                   .arg(color.red()).arg(color.green())
                   .arg(color.blue()).arg(color.alpha());
        }
    case QVariant::Date:
        return value.toDate().toString(Qt::ISODate);
    case QVariant::DateTime:
        return value.toDateTime().toString(Qt::ISODate);
    case QVariant::Invalid:
        return "<Invalid>";
    case QVariant::Point:
        {
            QPoint point = value.toPoint();
            return QString("(%1,%2)").arg(point.x()).arg(point.y());
        }
    case QVariant::Rect:
        {
            QRect rect = value.toRect();
            return QString("(%1,%2,%3,%4)")
                   .arg(rect.x()).arg(rect.y())
                   .arg(rect.width()).arg(rect.height());
        }
    case QVariant::Size:
        {
            QSize size = value.toSize();
            return QString("(%1,%2)").arg(size.width()).arg(size.height());
        }
    case QVariant::StringList:
        return value.toStringList().join(",");
    case QVariant::Time:
        return value.toTime().toString(Qt::ISODate);
    default:
        break;
    }
    return QString("<%1>").arg(value.typeName());
}
