// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "spreadsheetviewwithfrozencol.h"
#include <QScrollBar>
#include <QHeaderView>
#include <qcheckbox.h>
#include <qmenu.h>
#include <qwidgetaction.h>

SpreadsheetViewWithFrozenCol::SpreadsheetViewWithFrozenCol(QAbstractItemModel * model)
{
    setModel(model);
    frozenColsView = new QTableView(this);

    init();
}

SpreadsheetViewWithFrozenCol::~SpreadsheetViewWithFrozenCol()
{
      delete frozenColsView;
}

void SpreadsheetViewWithFrozenCol::init()
{
    // set Model and global aspect
    frozenColsView->setModel(model());
    frozenColsView->setFocusPolicy(Qt::NoFocus);
    frozenColsView->verticalHeader()->hide();
    frozenColsView->setAlternatingRowColors(true);
    frozenColsView->setSortingEnabled(true);
    frozenColsView->sortByColumn(1, Qt::AscendingOrder);

    // context Menu
    frozenColsView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(frozenColsView, &QWidget::customContextMenuRequested,
            this, &SpreadsheetViewWithFrozenCol::showCustomContextMenu);

    // horizonzal header displayed vertically (Frozen + Main)
    auto *vHeaderFrozen = new VerticalHeader(Qt::Horizontal, frozenColsView);
    frozenColsView->setHorizontalHeader(vHeaderFrozen);

    auto *vHeaderMain = new VerticalHeader(Qt::Horizontal, this);
    setHorizontalHeader(vHeaderMain);

    // stack main headers under frozenTableView headers
    horizontalHeader()->stackUnder(frozenColsView);
    verticalHeader()->stackUnder(frozenColsView);

    // Synchronise col and rows size
    connect(horizontalHeader(), &QHeaderView::sectionResized,
            this, &SpreadsheetViewWithFrozenCol::updateSectionWidth);
    connect(verticalHeader(), &QHeaderView::sectionResized,
            this, &SpreadsheetViewWithFrozenCol::updateSectionHeight);
    frozenColsView->setColumnWidth(0, columnWidth(0));
    frozenColsView->setColumnWidth(1, columnWidth(1));

    // hide col >= 2 of frozenTableView
    for (int col = 2; col < model()->columnCount(); ++col)
        frozenColsView->setColumnHidden(col, true);

    // Scrollbars synchronised (main + frozen)
    setHorizontalScrollMode(ScrollPerPixel);
    setVerticalScrollMode(ScrollPerPixel);
    frozenColsView->setVerticalScrollMode((ScrollPerPixel));
    frozenColsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    frozenColsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    connect(frozenColsView->verticalScrollBar(), &QAbstractSlider::valueChanged,
            verticalScrollBar(), &QAbstractSlider::setValue);

    connect(verticalScrollBar(), &QAbstractSlider::valueChanged,
            frozenColsView->verticalScrollBar(), &QAbstractSlider::setValue);

    // Synchronise order selection indicator (Frozen + Main)
    auto *mainHeader   = horizontalHeader();
    auto *frozenHeader = frozenColsView->horizontalHeader();
    connect(mainHeader, &QHeaderView::sortIndicatorChanged,
            this,[frozenHeader](int col, Qt::SortOrder order)
            {
                if (col >= 2)
                    frozenHeader->setSortIndicator(col, order);
            });

    connect(frozenHeader, &QHeaderView::sortIndicatorChanged,
            this,[mainHeader] (int col, Qt::SortOrder order)
            {
                if (col < 2)
                    mainHeader->setSortIndicator(col, order);
            });
    frozenHeader->setFixedHeight(mainHeader->height());
    connect(vHeaderFrozen, &QHeaderView::sortIndicatorChanged,
            vHeaderMain, &VerticalHeader::setSortIndicator);
    connect(vHeaderMain, &QHeaderView::sortIndicatorChanged,
            vHeaderFrozen, &VerticalHeader::setSortIndicator);

    // Global positionning and display
    viewport()->stackUnder(frozenColsView);
    frozenColsView->setSelectionModel(selectionModel());
    frozenColsView->show();

    updateFrozenTableGeometry();
}

void SpreadsheetViewWithFrozenCol::updateSectionWidth(int logicalIndex, int /* oldSize */, int newSize)
{

    if (logicalIndex == 0 || logicalIndex == 1) {
        frozenColsView->setColumnWidth(logicalIndex, newSize);
        updateFrozenTableGeometry();
    }

}

void SpreadsheetViewWithFrozenCol::updateSectionHeight(int logicalIndex, int /* oldSize */, int newSize)
{
      frozenColsView->setRowHeight(logicalIndex, newSize);
}

void SpreadsheetViewWithFrozenCol::resizeEvent(QResizeEvent * event)
{
      QTableView::resizeEvent(event);
      updateFrozenTableGeometry();
 }

 namespace {
 inline int frozenWidth(const QTableView* main, const QTableView* frozen) {
     return frozen->columnWidth(0) + frozen->columnWidth(1);
    }
 }

 QModelIndex SpreadsheetViewWithFrozenCol::moveCursor(CursorAction cursorAction,
                                           Qt::KeyboardModifiers modifiers)
 {
     QModelIndex current = QTableView::moveCursor(cursorAction, modifiers);

     if (cursorAction == MoveLeft && current.column() > 1
         && visualRect(current).topLeft().x() < frozenWidth(this, frozenColsView)) {

         const int newValue = horizontalScrollBar()->value()
         + visualRect(current).topLeft().x()
             - frozenWidth(this, frozenColsView);
         horizontalScrollBar()->setValue(newValue);
     }
     return current;
 }

void SpreadsheetViewWithFrozenCol::scrollTo (const QModelIndex & index, ScrollHint hint){
    if (index.column() > 1)
        QTableView::scrollTo(index, hint);
}

void SpreadsheetViewWithFrozenCol::updateFrozenTableGeometry()
{
    frozenColsView->setGeometry(
        verticalHeader()->width(),                 // x
        0,                                         // y
        frozenWidth(this, frozenColsView) + 1,             // largeur (le +1 évite une ligne d’1 px)
        viewport()->height() + horizontalHeader()->height() // hauteur
        );
}

void SpreadsheetViewWithFrozenCol::showCustomContextMenu(const QPoint &pos)
{
    SpreadsheetView* parentView = qobject_cast<SpreadsheetView*>(this);

    if (!parentView)
        return;

    // Appelle le menu prévu dans SpreadsheetView
    //parentView->showContextMenu(mapToGlobal(pos));
    parentView->showContextMenu(pos);
}






