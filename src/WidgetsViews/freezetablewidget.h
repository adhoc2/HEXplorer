// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef FREEZETABLEWIDGET_H
#define FREEZETABLEWIDGET_H

#include "spreadsheetview.h"
#include <QTableView>
#include <qheaderview.h>
#include <qpainter.h>

class VerticalHeader : public QHeaderView
{
    Q_OBJECT
public:
    VerticalHeader(Qt::Orientation orientation, QWidget *parent = nullptr)
        : QHeaderView(orientation, parent)
    {
        setSectionsClickable(true);      // allow resizing / clicking
        setHighlightSections(true);      // show selection highlight
        setSortIndicatorShown(true);
    }

public slots:
    void setSortIndicator(int column, Qt::SortOrder order)
    {
        m_sortColumn = column;
        m_sortOrder = order;
        viewport()->update(); // repaint le header
    }

protected:
    void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const override
    {
        if (!painter) return;
        painter->save();

        // --- Dessiner background + bordures via style ---
        QStyleOptionHeader opt;
        initStyleOption(&opt);
        opt.rect = rect;
        opt.section = logicalIndex;
        style()->drawControl(QStyle::CE_Header, &opt, painter, this);

        // --- Récupérer texte, font, couleur depuis le modèle ---
        QString text = model()->headerData(logicalIndex, orientation(), Qt::DisplayRole).toString();

        QFont font = model()->headerData(logicalIndex, orientation(), Qt::FontRole).value<QFont>();
        if (!font.family().size()) font = this->font();

        QBrush brush = model()->headerData(logicalIndex, orientation(), Qt::ForegroundRole).value<QBrush>();
        if (!brush.style()) brush = QBrush(palette().color(QPalette::WindowText));

        painter->setFont(font);
        painter->setPen(brush.color());

        // --- Dessiner flèche tri si colonne triée ---
        if (logicalIndex == m_sortColumn)
        {
            int arrowSize = 4;
            int margin = 2; // marge par rapport aux bords

            // Placer la flèche en bas à droite **dans le rect**
            QPoint arrowPos(
                rect.right() - 2*arrowSize - margin, // on décale pour que la largeur de la flèche tienne
                rect.bottom() - arrowSize - margin   // on décale pour que la hauteur tienne
                );

            QPolygon arrow;
            if (m_sortOrder == Qt::AscendingOrder)
            {
                arrow << QPoint(0, arrowSize) << QPoint(arrowSize, 0) << QPoint(2*arrowSize, arrowSize);
            }
            else
            {
                arrow << QPoint(0, 0) << QPoint(arrowSize, arrowSize) << QPoint(2*arrowSize, 0);
            }

            painter->setBrush(Qt::black);
            painter->drawPolygon(arrow.translated(arrowPos));
        }

        // --- Rotation pour texte vertical ---
        painter->translate(rect.bottomLeft());
        painter->rotate(-90);

        // --- Dessiner texte vertical ---
        int padding = 5;
        QRect textRect(padding, 0, rect.height(), rect.width());
        painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, text);

        painter->restore();
    }

    QSize sectionSizeFromContents(int logicalIndex) const override
    {
        QString text = model()->headerData(logicalIndex, orientation(), Qt::DisplayRole).toString();
        QFont font = model()->headerData(logicalIndex, orientation(), Qt::FontRole).value<QFont>();
        if (!font.family().size()) font = this->font();

        QFontMetrics fm(font);
        QRect bounding = fm.boundingRect(text); // taille exacte du texte

        int padding = 5;           // marge interne
        int verticalOffset = 5;    // décalage du texte vers le haut

        // Pour texte vertical, swap width/height
        int width  = bounding.height() + padding;
        int height = bounding.width() + padding + verticalOffset; // ajoute offset

        return QSize(width, height);
    }
private:
    int m_sortColumn = -1;
    Qt::SortOrder m_sortOrder = Qt::AscendingOrder;
};

class FreezeTableWidget : public SpreadsheetView {
     Q_OBJECT

public:
      FreezeTableWidget(QAbstractItemModel * model);
      ~FreezeTableWidget();


protected:
      void resizeEvent(QResizeEvent *event) override;
      QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers) override;
      void scrollTo (const QModelIndex & index, ScrollHint hint = EnsureVisible) override;

private:
      QTableView *frozenTableView;
      void init();
      void updateFrozenTableGeometry();


private slots:
      void updateSectionWidth(int logicalIndex, int oldSize, int newSize);
      void updateSectionHeight(int logicalIndex, int oldSize, int newSize);
      void showCustomContextMenu(const QPoint &pos);

};
#endif // FREEZETABLEWIDGET_H
