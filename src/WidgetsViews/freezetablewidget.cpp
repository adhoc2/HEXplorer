// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "freezetablewidget.h"

#include <QScrollBar>
#include <QHeaderView>

FreezeTableWidget::FreezeTableWidget(QAbstractItemModel * model)
{
      setModel(model);
      frozenTableView = new QTableView(this);

      init();

      //connect the headers and scrollbars of both tableviews together
      connect(horizontalHeader(),&QHeaderView::sectionResized, this,
              &FreezeTableWidget::updateSectionWidth);
      connect(verticalHeader(),&QHeaderView::sectionResized, this,
              &FreezeTableWidget::updateSectionHeight);

      connect(frozenTableView->verticalScrollBar(), &QAbstractSlider::valueChanged,
              verticalScrollBar(), &QAbstractSlider::setValue);
      connect(verticalScrollBar(), &QAbstractSlider::valueChanged,
              frozenTableView->verticalScrollBar(), &QAbstractSlider::setValue);

      // Synchroniser l’indicateur de tri
      auto *mainHeader   = this->horizontalHeader();
      auto *frozenHeader = frozenTableView->horizontalHeader();
      connect(mainHeader, &QHeaderView::sortIndicatorChanged,
              this, [ frozenHeader](int col, Qt::SortOrder order)
              {
                  const int frozenCount = 2;
                  if (col >= frozenCount)
                  {
                    frozenHeader->setSortIndicator(col, order);
                  }
              });
      connect(frozenHeader, &QHeaderView::sortIndicatorChanged,
              this, [ mainHeader](int col, Qt::SortOrder order)
              {
                  const int frozenCount = 2;
                  if (col < frozenCount)
                  {
                      mainHeader->setSortIndicator(col, order);
                  }
              });
      frozenHeader->setFixedHeight(mainHeader->height());
}

FreezeTableWidget::~FreezeTableWidget()
{
      delete frozenTableView;
}

void FreezeTableWidget::init()
{
      frozenTableView->setModel(model());
      frozenTableView->setFocusPolicy(Qt::NoFocus);
      frozenTableView->verticalHeader()->hide();

      // Only for Vertical hearders
      VerticalHeader* vHeader = new VerticalHeader(Qt::Horizontal, frozenTableView);
      frozenTableView->setHorizontalHeader(vHeader);
      VerticalHeader* vHeaderOrg = new VerticalHeader(Qt::Horizontal, this);
      this->setHorizontalHeader(vHeaderOrg);
      horizontalHeader()->stackUnder(frozenTableView);
      verticalHeader()->stackUnder(frozenTableView);
      connect(vHeader, &QHeaderView::sortIndicatorChanged,
              vHeaderOrg, &VerticalHeader::setSortIndicator);
      connect(vHeaderOrg, &QHeaderView::sortIndicatorChanged,
              vHeader, &VerticalHeader::setSortIndicator);

      // stackunder
      viewport()->stackUnder(frozenTableView);
      frozenTableView->setSelectionModel(selectionModel());

      // cacher les colonnes >= 2 (on garde 0 et 1 visibles)
      for (int col = 2; col < model()->columnCount(); ++col)
          frozenTableView->setColumnHidden(col, true);

      // Synchroniser les largeurs initiales des deux premières colonnes
      frozenTableView->setColumnWidth(0, columnWidth(0));
      frozenTableView->setColumnWidth(1, columnWidth(1));

      //frozenTableView->setHorizontalHeader(new VerticalHeader(Qt::Horizontal, frozenTableView));
      frozenTableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
      frozenTableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
      frozenTableView->setAlternatingRowColors(true);
      frozenTableView->setSortingEnabled(true);
      frozenTableView->sortByColumn(1, Qt::AscendingOrder);
      frozenTableView->show();

      updateFrozenTableGeometry();

      setHorizontalScrollMode(ScrollPerPixel);
      setVerticalScrollMode(ScrollPerPixel);
      frozenTableView->setVerticalScrollMode(ScrollPerPixel);
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




