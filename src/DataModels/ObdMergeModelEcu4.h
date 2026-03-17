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

#ifndef OBDMERGEMODELECU4_H
#define OBDMERGEMODELECU4_H

#include <QAbstractTableModel>
#include <Nodes/characteristic.h>
#include "srecfile.h"
#include "dcmfile.h"
#include "cdfxfile.h"
#include "csv.h"
#include "errorcode.h"
#include <QObject>
#include <qidentityproxymodel.h>


class ObdMergeModelEcu4 : public QAbstractTableModel
{
    Q_OBJECT

    public:
        ObdMergeModelEcu4(SrecFile *dataContainer, QObject *parent = 0);
        ObdMergeModelEcu4(CdfxFile *cdfx, QObject *parent = 0);
        ObdMergeModelEcu4(Dcm *dcm, QObject *parent = 0);
        ObdMergeModelEcu4(Csv *csv, QObject *parent = 0);
        ~ObdMergeModelEcu4();

        int rowCount(const QModelIndex &parent) const;
        int columnCount(const QModelIndex &parent) const;
        int getRow() const;
        int sourceData(int col) const;
        Data* getData(const int row, const int col) const;
        QVariant data(const QModelIndex &index, int role) const;
        bool setData(const QModelIndex &index, const QVariant &value, int role);
        bool resetData(const QModelIndexList &indexList, int role);
        bool undoData(const QModelIndexList &indexList, int role);
        QVariant headerData(int section, Qt::Orientation orientation, int role) const;
        Qt::ItemFlags flags(const QModelIndex &index) const;
        bool exportAs(QString format);
        bool exportModifsAs(QString format);
        DataContainer* getDataContainer() {return dataContainer;}

    private:
        QList<ErrorCode*> listErrorCode;
        ErrorCode* getErrorCode(QString name);
        QStringList listDataNameInView;
        DataContainer *dataContainer;
        int nColumn;
        int nRow;

        void listErrorCodes(QStringList listLab = QStringList());

    signals:
        void obdDataChanged();
};


#endif // OBDMERGEMODELECU4_H
