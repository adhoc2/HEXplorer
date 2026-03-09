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

#include "DataModels/ObdMergeModelEcu4.h"
#include <QFont>
#include <QList>
#include "qdebug"


ObdMergeModelEcu4::ObdMergeModelEcu4(SrecFile *srecFile, QObject *parent)
    :QAbstractTableModel(parent)
{
    nRow = 0;
    nColumn = 0;
    dataContainer = srecFile;
    listErrorCodes();

}

ObdMergeModelEcu4::ObdMergeModelEcu4(CdfxFile *cdfx, QObject *parent)
    :QAbstractTableModel(parent)
{
    nRow = 0;
    nColumn = 0;
    dataContainer = cdfx;
    listErrorCodes();
}

ObdMergeModelEcu4::ObdMergeModelEcu4(Dcm *dcm, QObject *parent)
    :QAbstractTableModel(parent)
{
    nRow = 0;
    nColumn = 0;
    dataContainer = dcm;
    listErrorCodes();
}

ObdMergeModelEcu4::ObdMergeModelEcu4(Csv *csv, QObject *parent)
    :QAbstractTableModel(parent)
{
    nRow = 0;
    nColumn = 0;
    dataContainer = csv;
    listErrorCodes();
}


ObdMergeModelEcu4::~ObdMergeModelEcu4()
{

}

void ObdMergeModelEcu4::listErrorCodes(QStringList listLab)
{
    this->beginResetModel();
    nRow = 0;
    nColumn = 0;
    listDataNameInView.clear();
    listErrorCode.clear();


    QList<Data*> listData;
    if (listLab.isEmpty())
    {
        listData = dataContainer->listData;
    }
    else
    {
        foreach (QString str, listLab)
        {
            Data* data = dataContainer->getData(str);
            if (data)
            {
                listData.append(data);
            }
        }
    }

    foreach (Data *data, listData)
    {
        QString nameStr(data->getName());
        if (!listDataNameInView.contains(nameStr))
        {
            QString nameEnd = nameStr.split(".").last();
            QString nameBegin = "";

            if (nameEnd.compare("Prio", Qt::CaseSensitive) == 0)
            {
                nameBegin = nameStr.split("_C.").first();

                //check if errorCode has been previously created
                //if not create a new one
                ErrorCode* error = getErrorCode(nameBegin);
                if (!error)
                {
                    error = new ErrorCode(nameBegin);
                    listErrorCode.append(error);
                }

                // add the Pcode in DTC field
                error->dtc = data->getComment().remove("DTC-ID: ");
                error->dtc.remove("\"");
                error->dtc.remove("\"");

                // fill listDataNameInView to be used for export
                QList<Data*> _listData = dataContainer->getDataByPrefix(nameBegin + "_C.");
                for (Data* data : _listData)
                {
                    error->listData.append(data);
                    listDataNameInView.append(data->getName());
                }

                _listData = dataContainer->getDataByPrefix(nameBegin + "FrmMask_C.");
                for (Data* data : _listData)
                {
                    error->listData.append(data);
                    listDataNameInView.append(data->getName());
                }

                _listData = dataContainer->getDataByPrefix(nameBegin + "Inhbn_");
                for (Data* data : _listData)
                {
                    error->listData.append(data);
                    listDataNameInView.append(data->getName());
                }
            }            

        }
    }

    nRow = this->listErrorCode.count();
    nColumn = 100;

    this->endResetModel();

}

int ObdMergeModelEcu4::rowCount(const QModelIndex &parent) const
{
    return nRow;
}

int ObdMergeModelEcu4::columnCount(const QModelIndex &parent) const
{
    return nColumn;
}

QVariant ObdMergeModelEcu4::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    int row = index.row();
    int column = index.column();

    if (0 <= row && row < nRow)
    {

        switch (role)
        {
        case Qt::DisplayRole :
            {
               if (column == 0) //DTC
               {
                   return this->listErrorCode.at(row)->dtc;
               }
               else if (column == 1) //Error Name
               {
                   return this->listErrorCode.at(row)->name;
               }
               else
               {
                   Data* data = this->getData(row, column);
                   if (data)
                   {
                       return data->getZ(0);
                   }
                   else
                       return "";
               }


               return QVariant();

            }
            break;

        case Qt::DecorationRole: // The data to be rendered as a decoration in the form of an icon.
            break;

        case Qt::EditRole:
        {
            int row = index.row();
            int col = index.column();

            Data* data = this->getData(row, col);
            if (data)
            {
               return data->getZ(0);
            }

            break;
        }

        case Qt::ToolTipRole:
        {
            break;
        }

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
            //QColor myjaune = QColor::fromHsv(60, 110, 255);
            //QColor mybleu = QColor::fromHsv(200, 110, 255);
            QColor myvert = QColor::fromHsv(120, 110, 255);

            Data* data = this->getData(row, column);

            if (data)
            {
                if (data->getOrgZ(0) != data->getZ(0))
                {
                    return myvert;
                }
            }

            break;
        }

        case Qt::ForegroundRole: // the foreground brush (text color, typically) used for items rendered with the default delegate.
            {
                if (column == 0)
                {
                    //QColor color = Qt::blue;
                    //return color;
                }
                else
                {
                    QColor color1 = Qt::red;
                    QColor color2 = Qt::blue;


                    Data* data = this->getData(row, column);
                    if (data)
                    {
                        bool bl1;
                        double val1 = data->getZ(0).toDouble(&bl1);
                        bool bl2;
                        double val2 = data->getOrgZ(0).toDouble(&bl2);

                        if (bl1 && bl2)
                        {
                            if (val1 > val2)
                                return color1;
                            else if (val1 < val2)
                                return color2;
                        }
                        else
                        {
                            if (data->getZ(0) != data->getOrgZ(0))
                                return color1;
                        }
                    }
                }
            }
            break;
        }
    }

    return QVariant();
}

bool ObdMergeModelEcu4::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    int row = index.row();
    int col = index.column();

    if (0 <= row && row < nRow)
    {
        Data* data = this->getData(row, col);
        if (data)
        {
            //set the value
            data->setZ(0, value.toString());

           //update the tableView
           //emit dataChanged(index, index);

           return true;
        }
        else
            return false;
    }

   //emit obdDataChanged();
    return true;
}

bool ObdMergeModelEcu4::resetData(const QModelIndexList &indexList, int role)
{
    foreach (QModelIndex index, indexList)
    {
        if (index.isValid() && role == Qt::EditRole)
        {
            int row = index.row();
            int col = index.column();

            if (0 <= row && row < nRow)
            {
                Data* data = this->getData(row, col);
                if (data)
                {
                   //set the value
                   data->resetValZ(0);


                   //update the tableView
                   //emit dataChanged(index, index);
                   //return true;
                }
                else
                    return false;
            }

       }
    }

    //update the tableView
    QModelIndex topLeft = indexList.at(0);
    QModelIndex btmRight = indexList.at(indexList.count() - 1);
    emit dataChanged(topLeft, btmRight);

    return true;
}

bool ObdMergeModelEcu4::undoData(const QModelIndexList &indexList, int role)
{
    foreach (QModelIndex index, indexList)
    {
        if (index.isValid() && role == Qt::EditRole)
        {
            int row = index.row();
            int col = index.column();

            if (0 <= row && row < nRow)
            {
                Data* data = this->getData(row, col);
                if (data)
                {
                   //set the value
                   data->undoValZ(0);

                   //update the tableView
                   //emit dataChanged(index, index);
                   //return true;
                }
                else
                    return false;
            };

          }
    }

    //update the tableView
    QModelIndex topLeft = indexList.at(0);
    QModelIndex btmRight = indexList.at(indexList.count() - 1);
    emit dataChanged(topLeft, btmRight);

    return true;
}

Qt::ItemFlags ObdMergeModelEcu4::flags(const QModelIndex &index) const
 {
     Qt::ItemFlags flags = QAbstractItemModel::flags(index);
     flags |= Qt::ItemIsEditable;
     return flags;
 }

QVariant ObdMergeModelEcu4::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (role)
    {
        case Qt::DisplayRole :
        {
            if (orientation == Qt::Horizontal)
            {
                if (section == 0)
                {
                    return "Default code";
                }
                else if (section == 1)
                {
                    return "Default name";
                }
                else
                {
                    if (section <= (listErrorCode.at(0)->listData.count() + 1))
                    {
                        QString sectionName = listErrorCode.at(0)->listData.at(section - 2)->getName().split(listErrorCode.at(0)->name).last();
                        if (sectionName.startsWith("_C."))
                            sectionName = sectionName.split("_C.").last();
                        return sectionName;
                    }
                }
            }
            else
                return QVariant();

        }
        break;

        case Qt::DecorationRole: // The data to be rendered as a decoration in the form of an icon.
            break;

        case Qt::EditRole:
            break;

        case Qt::ToolTipRole:
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

int ObdMergeModelEcu4::getRow() const
{
    return 0;
}

Data* ObdMergeModelEcu4::getData(const int row, const int col) const
{
    if (col >= 2 && col <= listErrorCode.at(row)->listData.count() + 1)
    {
        Data* data = listErrorCode.at(row)->listData.at(col - 2);
        return data;
    }
    else return nullptr;
}

bool ObdMergeModelEcu4::exportAs(QString format)
{
    if (format.endsWith("cdfx"))
    {
        return dataContainer->exportDataList2Cdf(listDataNameInView);
    }
    else if (format.endsWith("csv"))
    {
        return dataContainer->exportDataList2Csv(listDataNameInView);
    }
    else if (format.endsWith("dcm"))
    {
        return dataContainer->exportDataList2Dcm(listDataNameInView);
    }
    else
    {
        return false;
    }
}

bool ObdMergeModelEcu4::exportModifsAs(QString format)
{
    if (format.endsWith("cdfx"))
    {
        QList<Data*> listData = dataContainer->getModifiedData();
        QStringList listDataNameModified;
        foreach (Data* data, listData)
        {
            listDataNameModified.append(data->getName());
        }
        return dataContainer->exportDataList2Cdf(listDataNameModified);
    }
    else if (format.endsWith("csv"))
    {
        QList<Data*> listData = dataContainer->getModifiedData();
        QStringList listDataNameModified;
        foreach (Data* data, listData)
        {
            listDataNameModified.append(data->getName());
        }
        return dataContainer->exportDataList2Csv(listDataNameModified);
    }
    else if (format.endsWith("dcm"))
    {
        QList<Data*> listData = dataContainer->getModifiedData();
        QStringList listDataNameModified;
        foreach (Data* data, listData)
        {
            listDataNameModified.append(data->getName());
        }
        return dataContainer->exportDataList2Dcm(listDataNameModified);
    }
    else
    {
        return false;
    }
}

ErrorCode* ObdMergeModelEcu4::getErrorCode(QString errorName)
{
    foreach (ErrorCode* errorCode, listErrorCode)
    {
        if (errorCode->name == errorName)
            return errorCode;
    }

    return nullptr;
}
