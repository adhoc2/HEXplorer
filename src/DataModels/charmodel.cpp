// HEXplorer is an Asap and HEX file editor
// Copyright (C) 2011  <Christophe Hoel>
//
// This file is part of HEXplorer.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// please contact the author at : christophe.hoel@gmail.com

#include "charmodel.h"
#include "Nodes/compu_method.h"
#include <QFont>

CharModel::CharModel(QObject *parent)
{
    nRow = 0;
    nColumn = 0;
}

CharModel::~CharModel()
{

}

void CharModel::setList(QList<Node *> labelList)
{

    listChar = labelList;

    nRow = labelList.count();
    nColumn = 11;

    beginResetModel();
    endResetModel();
}

int CharModel::rowCount(const QModelIndex &parent) const
{
    return nRow;
}

int CharModel::columnCount(const QModelIndex &parent) const
{
    return nColumn;
}

QVariant CharModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    int row = index.row();
    int column = index.column();

    if (0 <= row && row < nRow)
    {
        CHARACTERISTIC *label = (CHARACTERISTIC*)listChar.at(row);

        switch (role)
        {
        case Qt::DisplayRole :
            {
                QString compu_method = label->getPar("Conversion");

                Node * node = label->getParentNode()->getParentNode();
                COMPU_METHOD *cmp = (COMPU_METHOD*)node->getNode("COMPU_METHOD/" + compu_method);


                if (column == 0)
                {
                    return QString(label->name);
                }
                else if (column == 1)
                {
                    QString str = label->fixPar("LongIdentifier");
                    return str;
                }
                else if (column == 2)
                {
                    //return cmp->getPar("Unit");
                    if (cmp)
                        return cmp->fixPar("Unit");

                }
                else if (column == 3)
                {
                    return ((CHARACTERISTIC*)label)->getSubsetName();
                }

                else if (column == 4)
                {
                    QString str = label->fixPar("Adress");
                    return str;
                }
                else if (column == 5)
                {
                    QString str = label->fixPar("Type");
                    return str;
                }
                else if (column == 6)
                {
                    QString str = label->fixPar("Conversion");
                    return str;
                }
                else if (column == 7)
                {
                    QString str = label->fixPar("Deposit");
                    return str;
                }
                else if (column == 8)
                {
                    QString str = label->fixPar("MaxDiff");
                    return str;
                }
                else if (column == 9)
                {
                    QString str = label->fixPar("LowerLimit");
                    return str;
                }
                else if (column == 10)
                {
                    QString str = label->fixPar("UpperLimit");
                    return str;
                }
                else
                    return QVariant();


            }
            break;

        case Qt::DecorationRole: // The data to be rendered as a decoration in the form of an icon.
            break;

        case Qt::EditRole:
            {
            }
            break;

        case Qt::ToolTipRole:
            {
            }
            break;

        case Qt::StatusTipRole: // The data displayed in the status bar.
            break;

        case Qt::WhatsThisRole: // The data displayed for the item in "What's This?" mode.
            break;

        case Qt::SizeHintRole: // The size hint for the item that will be supplied to views.
            break;

        case Qt::FontRole : // The font used for items rendered with the default delegate.
            {
                if (column == 0)
                {
                   QFont font;
                   font.setBold(true);
                   return font;
                }
            }
            break;

        case Qt::TextAlignmentRole:
            break;

        case Qt::BackgroundRole:
            {

            }
            break;

        case Qt::ForegroundRole: // the foreground brush (text color, typically) used for items rendered with the default delegate.
            {
                if (column == 0)
                {
                    QColor color = Qt::blue;
                    return color;
                }
            }
            break;
        }
    }

    return QVariant();
}

Qt::ItemFlags CharModel::flags(const QModelIndex &index) const
 {
     Qt::ItemFlags flags = QAbstractItemModel::flags(index);
     flags |= Qt::ItemIsEditable;
     return flags;
 }

QVariant CharModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (role)
    {
        case Qt::DisplayRole :
            {
            if (orientation == Qt::Horizontal)
            {
                if (section == 0)
                {
                    return "name";
                }
                else if (section == 1)
                {
                    return "description";
                }
                else if (section == 2)
                {
                    return "unit";
                }
                else if (section == 3)
                {
                    return "fonction";
                }
                else if (section == 4)
                {
                    return "ECU address";
                }
                else if (section == 5)
                {
                    return "data type";
                }
                else if (section == 6)
                {
                    return "computation method";
                }
                else if (section == 7)
                {
                    return "Deposit";
                }
                else if (section == 8)
                {
                    return "MaxDiff";
                }
                else if (section == 9)
                {
                    return "LowerLimit";
                }
                else if (section == 10)
                {
                    return "UpperLimit";
                }
            }
                else
                    return QVariant();

            }
            break;

        case Qt::DecorationRole: // The data to be rendered as a decoration in the form of an icon.
            break;

        case Qt::EditRole:
            {
            }
            break;

        case Qt::ToolTipRole:
            {
            }
            break;

        case Qt::StatusTipRole: // The data displayed in the status bar.
            break;

        case Qt::WhatsThisRole: // The data displayed for the item in "What's This?" mode.
            break;

        case Qt::SizeHintRole: // The size hint for the item that will be supplied to views.
            break;

        case Qt::FontRole : // The font used for items rendered with the default delegate.
            {
               QFont font;
               font.setBold(true);
               return font;
            }
            break;

        case Qt::TextAlignmentRole:
            break;

        case Qt::BackgroundRole:
            {

            }
            break;

        case Qt::ForegroundRole: // the foreground brush (text color, typically) used for items rendered with the default delegate.
            {
                QColor color = Qt::red;
                return color;
            }
            break;
    }

    return QVariant();
}
