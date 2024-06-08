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

#ifndef SPREADSHEETVIEW_H
#define SPREADSHEETVIEW_H

#include <QTableView>

class SpreadsheetView : public QTableView
{   
    Q_OBJECT

    public:
        SpreadsheetView(QWidget *parent = 0);
        ~SpreadsheetView();


    private:
        QWidget* mdimain;
        QAction *export2Excel;
        QAction *changeSize;
        QAction *selectAllLabel;
        QAction *axisXInterpolate;
        QAction *axisYInterpolate;
        QAction *interpolateX;
        QAction *interpolateY;
        QAction *undoModif;
        QAction *resetModif;
        QAction *factorMulti;
        QAction *factorDiv;
        QAction *offsetPlus;
        QAction *offsetMinus;
        QAction *fillAllWith;
        QAction *copyAction;
        QAction *pasteAction;
        QAction *pasteActionObd;
        QAction *plotAction;
        QAction *editText;
        QAction *editBit;
        QAction *editHex;
        QAction *editProperties;
        QAction *findObdView;
        QAction *filterColumns;
        QAction *resetAllFilters;
        QAction *saveAs;
        QAction *saveModifsAs;
        QAction *hideColumns;
        void createActions();
        QString getExcelCell(int row, int col);
        QMap<int, QString> hiddenHeaders;

    private slots:
        #ifdef Q_WS_WIN32
            void exportToExcel();
        #endif
        void myResize(int b);
        void changeLabelSize();
        void selectAll_label();
        void editAsText();
        void editAsBit();
        void editAsHex();
        void editProp();
        void interpAxisX();
        void interpAxisY();
        void interpX();
        void interpY();
        void undoM();
        void resetM();
        void factorD();
        void factorM();
        void offsetP();
        void offsetM();
        void fillAll();
        void copy();
        void paste();
        void pasteObdView();
        void plot();
        void findInObdView();
        void filterColumn(QString value, bool activate);
        void setFilter_Pattern();
        void filterColumn_Name(QString text);
        void importLabFileinObdView();
        void resetAll_Filters();
        void saveObdViewAs();
        void saveObdViewModifsAs();
        void selectColumns2Hide(int i, QString str);
        void funHideColumns();
        void contextMenuEvent( QPoint p );
        void updateActions(QModelIndex);

};

#endif // SPREADSHEETVIEW_H
