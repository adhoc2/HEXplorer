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

#include "measmodel.h"
#include "Nodes/compu_method.h"
#include <QFont>

MeasModel::MeasModel(QObject *parent)
{
    nRow = 0;
    nColumn = 0;
}

MeasModel::~MeasModel()
{

}

void MeasModel::setList(QList<Node *> labelList)
{

    listMeas = labelList;

    nRow = labelList.count();
    nColumn = 10;

    beginResetModel();
    endResetModel();
}

int MeasModel::rowCount(const QModelIndex &parent) const
{
    return nRow;
}

int MeasModel::columnCount(const QModelIndex &parent) const
{
    return nColumn;
}

QVariant MeasModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    int row = index.row();
    int column = index.column();


   if (0 <= row && row < nRow)
    {
        MEASUREMENT *label = (MEASUREMENT*)listMeas.at(row);

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
                    //return label->fixPar("Identifier").c_str();
                }
                else if (column == 1)
                {
                    QString str = label->fixPar("LongIdentifier").c_str();
                    return str;
                }
                else if (column == 2)
                {
                    if (cmp)
                     return QString(cmp->getPar("Unit"));
                }
                else if (column == 3)
                {
                    ECU_ADDRESS *ecuAddress = (ECU_ADDRESS*)label->getItem("ecu_address");
                    if (ecuAddress)
                    {
                        return QString(ecuAddress->getPar("Address"));
                    }

                    return QVariant();
                }
                else if (column == 4)
                {
                    QString str = label->fixPar("DataType").c_str();
                    return str;
                }
                else if (column == 5)
                {
                    QString str = label->fixPar("Conversion").c_str();
                    return str;
                }
                else if (column == 6)
                {
                    QString str = label->fixPar("Resolution").c_str();
                    return str;
                }
                else if (column == 7)
                {
                    QString str = label->fixPar("Accuracy").c_str();
                    return str;
                }
                else if (column == 8)
                {
                    QString str = label->fixPar("LowerLimit").c_str();
                    return str;
                }
                else if (column == 9)
                {
                    QString str = label->fixPar("UpperLimit").c_str();
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

Qt::ItemFlags MeasModel::flags(const QModelIndex &index) const
 {
     Qt::ItemFlags flags = QAbstractItemModel::flags(index);
     flags |= Qt::ItemIsEditable;
     return flags;
 }

QVariant MeasModel::headerData(int section, Qt::Orientation orientation, int role) const
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
                    return "ECU address";
                }
                else if (section == 4)
                {
                    return "data type";
                }
                else if (section == 5)
                {
                    return "computation method";
                }
                else if (section == 6)
                {
                    return "resolution";
                }
                else if (section == 7)
                {
                    return "accuracy";
                }
                else if (section == 8)
                {
                    return "LowerLimit";
                }
                else if (section == 9)
                {
                    return "UpperLimit";
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
    }

    return QVariant();
}
