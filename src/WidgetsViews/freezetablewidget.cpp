// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "freezetablewidget.h"
#include <QScrollBar>
#include <QHeaderView>
#include <qcheckbox.h>
#include <qmenu.h>
#include <qwidgetaction.h>

FreezeTableWidget::FreezeTableWidget(QAbstractItemModel * model)
{
    setModel(model);
    frozenTableView = new QTableView(this);

    init();
}

FreezeTableWidget::~FreezeTableWidget()
{
      delete frozenTableView;
}

void FreezeTableWidget::init()
{
    // set Model and global aspect
    frozenTableView->setModel(model());
    frozenTableView->setFocusPolicy(Qt::NoFocus);
    frozenTableView->verticalHeader()->hide();
    frozenTableView->setAlternatingRowColors(true);
    frozenTableView->setSortingEnabled(true);
    frozenTableView->sortByColumn(1, Qt::AscendingOrder);

    // context Menu
    frozenTableView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(frozenTableView, &QWidget::customContextMenuRequested,
            this, &FreezeTableWidget::showCustomContextMenu);

    // horizonzal header displayed vertically (Frozen + Main)
    auto *vHeaderFrozen = new VerticalHeader(Qt::Horizontal, frozenTableView);
    frozenTableView->setHorizontalHeader(vHeaderFrozen);

    auto *vHeaderMain = new VerticalHeader(Qt::Horizontal, this);
    setHorizontalHeader(vHeaderMain);

    // stack main headers under frozenTableView headers
    horizontalHeader()->stackUnder(frozenTableView);
    verticalHeader()->stackUnder(frozenTableView);

    // Synchronise col and rows size
    connect(horizontalHeader(), &QHeaderView::sectionResized,
            this, &FreezeTableWidget::updateSectionWidth);
    connect(verticalHeader(), &QHeaderView::sectionResized,
            this, &FreezeTableWidget::updateSectionHeight);
    frozenTableView->setColumnWidth(0, columnWidth(0));
    frozenTableView->setColumnWidth(1, columnWidth(1));

    // hide col >= 2 of frozenTableView
    for (int col = 2; col < model()->columnCount(); ++col)
        frozenTableView->setColumnHidden(col, true);

    // Scrollbars synchronised (main + frozen)
    setHorizontalScrollMode(ScrollPerPixel);
    setVerticalScrollMode(ScrollPerPixel);
    frozenTableView->setVerticalScrollMode((ScrollPerPixel));
    frozenTableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    frozenTableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    connect(frozenTableView->verticalScrollBar(), &QAbstractSlider::valueChanged,
            verticalScrollBar(), &QAbstractSlider::setValue);

    connect(verticalScrollBar(), &QAbstractSlider::valueChanged,
            frozenTableView->verticalScrollBar(), &QAbstractSlider::setValue);

    // Synchronise order selection indicator (Frozen + Main)
    auto *mainHeader   = horizontalHeader();
    auto *frozenHeader = frozenTableView->horizontalHeader();
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
    viewport()->stackUnder(frozenTableView);
    frozenTableView->setSelectionModel(selectionModel());
    frozenTableView->show();

    updateFrozenTableGeometry();
}

void FreezeTableWidget::updateSectionWidth(int logicalIndex, int /* oldSize */, int newSize)
{

    if (logicalIndex == 0 || logicalIndex == 1) {
        frozenTableView->setColumnWidth(logicalIndex, newSize);
        updateFrozenTableGeometry();
    }

}

void FreezeTableWidget::updateSectionHeight(int logicalIndex, int /* oldSize */, int newSize)
{
      frozenTableView->setRowHeight(logicalIndex, newSize);
}

void FreezeTableWidget::resizeEvent(QResizeEvent * event)
{
      QTableView::resizeEvent(event);
      updateFrozenTableGeometry();
 }

 namespace {
 inline int frozenWidth(const QTableView* main, const QTableView* frozen) {
     return frozen->columnWidth(0) + frozen->columnWidth(1);
    }
 }

 QModelIndex FreezeTableWidget::moveCursor(CursorAction cursorAction,
                                           Qt::KeyboardModifiers modifiers)
 {
     QModelIndex current = QTableView::moveCursor(cursorAction, modifiers);

     if (cursorAction == MoveLeft && current.column() > 1
         && visualRect(current).topLeft().x() < frozenWidth(this, frozenTableView)) {

         const int newValue = horizontalScrollBar()->value()
         + visualRect(current).topLeft().x()
             - frozenWidth(this, frozenTableView);
         horizontalScrollBar()->setValue(newValue);
     }
     return current;
 }

void FreezeTableWidget::scrollTo (const QModelIndex & index, ScrollHint hint){
    if (index.column() > 1)
        QTableView::scrollTo(index, hint);
}

void FreezeTableWidget::updateFrozenTableGeometry()
{
    frozenTableView->setGeometry(
        verticalHeader()->width(),                 // x
        0,                                         // y
        frozenWidth(this, frozenTableView) + 1,             // largeur (le +1 évite une ligne d’1 px)
        viewport()->height() + horizontalHeader()->height() // hauteur
        );
}

void FreezeTableWidget::showCustomContextMenu(const QPoint &pos)
{
    SpreadsheetView* parentView = qobject_cast<SpreadsheetView*>(this);

    if (!parentView)
        return;

    // Appelle le menu prévu dans SpreadsheetView
    parentView->showContextMenu(mapToGlobal(pos));
}






