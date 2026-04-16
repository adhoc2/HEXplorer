//HEXplorer is an Asap and HEX file editor
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

#include "graph.h"
#include "PlotGraph/plot.h"
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <QPrintDialog>
#include <QPrinter>
#include <qwt_counter.h>
#include <qwt_plot_zoomer.h>
#include <qstatusbar.h>
#include "canvaspicker.h"
#include <qmessagebox.h>
#include <QHeaderView>
#include "qdebug.h"
#include "surfacegraph.h"
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMessageBox>
#include <QtGui/QPainter>
#include <QtGui/QScreen>
//#include "surfacegraph.h"

Graph::Graph(QWidget *parent, Data *dat) :  QMainWindow(parent)
{

    setupUi(this);
    setWindowTitle("HEXplorer :: plot");

    // model
    model = new GraphModel();
    model->setVal(dat);

    // tableview Widget
    table = new SpreadsheetView();
    table->setModel(model);
    table->horizontalHeader()->setDefaultSectionSize(50);
    table->verticalHeader()->setDefaultSectionSize(18);

    // create plots
    data = dat;
    plotXZ = new Plot(model, QString("XZ"));
    verticalLayout->addWidget(plotXZ);
    canvasXZ =  new CanvasPicker(this, plotXZ);
    if (dat->yCount() > 0)
    {
        plotYZ = new Plot(model, QString("YZ"));
        verticalLayout->addWidget(plotYZ);
        plotYZ->hide();
        canvasYZ =  new CanvasPicker(this, plotYZ);
    }

    // tableView
    verticalLayout->addWidget(table);
    table->setVisible(false);

    createButtons();

}

Graph::~Graph()
{

}

void Graph::createButtons()
{
    QToolButton *btnPrint = new QToolButton(toolBar);
    btnPrint->setText("Print");
    btnPrint->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    QToolButton *invertAxis = new QToolButton(toolBar);
    invertAxis->setText("X<->Y");
    invertAxis->setCheckable(true);
    invertAxis->setChecked(false);
    invertAxis->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    QToolButton *btn3D_3 = new QToolButton(toolBar);
    btn3D_3->setText("Q3DSurface");
    btn3D_3->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    btn3D_3->setEnabled(true);

    QToolButton *btnShowData = new QToolButton(toolBar);
    btnShowData->setText("Show data");
    btnShowData->setCheckable(true);
    btnShowData->setChecked(false);
    btnShowData->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    toolBar->addWidget(btnShowData);
    toolBar->addWidget(invertAxis);
    toolBar->addWidget(btn3D_3);
    toolBar->addWidget(btnPrint);


    connect(btnPrint, SIGNAL(clicked()), plotXZ, SLOT(printPlot()) );
    connect(btn3D_3, SIGNAL(clicked()), this, SLOT(plot3D_q3dsurface()));
    connect(btnShowData, SIGNAL(toggled(bool)), this, SLOT(showData(bool)) );
    connect(invertAxis, SIGNAL(toggled(bool)), this, SLOT(invertXY(bool)));
}

void Graph::showInfo(QString text)
{
    if ( text == QString() )
    {
    }

#ifndef QT_NO_STATUSBAR
     statusBar()->showMessage(text);
#endif
}

void Graph::plot3D_q3dsurface()
{
    if (data->getY().count() <= 0)
        return;

    // Widget racine
    QWidget *widget = new QWidget;
    widget->setWindowTitle(QStringLiteral("HEXplorer :: plot3D"));

    // Layouts
    QHBoxLayout *hLayout = new QHBoxLayout(widget);
    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->setAlignment(Qt::AlignTop);

    // QQuickWidget (remplace createWindowContainer)
    QQuickWidget *quickWidget = new QQuickWidget(widget);
    quickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    quickWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    quickWidget->setMinimumSize(800, 600);

    hLayout->addWidget(quickWidget, 1);
    hLayout->addLayout(vLayout);

    widget->show();

    // Surface QtGraphs Widgets
    auto *surface = new Q3DSurfaceWidgetItem;
    surface->setWidget(quickWidget);

    // Création du graphe (classe que tu viens de migrer)
    auto *modifier = new SurfaceGraphModifier(surface, data);
}


void Graph::showData(bool on)
{
    if (on)
    {
        table->setVisible(true);
        this->resize(600, 700);
    }
    else
    {
        table->setVisible(false);
        this->resize(600, 400);
    }
}

void Graph::selectIndexTable(QModelIndex index)
{
    table->selectionModel()->clear();
    table->selectionModel()->select(index, QItemSelectionModel::Select);
}

void Graph::invertXY(bool inverted)
{
    if (data->yCount() > 0)
    {
        if (inverted)
        {
            plotXZ->hide();
            plotYZ->show();
        }
        else
        {
            plotYZ->hide();
            plotXZ->show();
        }
    }
}
