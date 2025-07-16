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

#include "ui_forms/ui_mdimain.h"
#include <QtWidgets>
#include <QtCore>
//#include <omp.h>
#include <typeinfo>
#include <qtconcurrentrun.h>
#include <QFutureWatcher>
#include <sys/stat.h>
#ifdef WIN32
   #include <windows.h>
#endif
#include "sptablemodel.h"
#include "comparemodel.h"
#include "measmodel.h"
#include "charmodel.h"
#include "FandRModel.h"
#include "obdMergeModel.h"
#include "obdsortfilterproxymodel.h"
#include "a2ltreemodel.h"
#include "treemodelcompleter.h"
#include "a2ltreemodel.h"
#include "mdimain.h"
#include "ui_mdimain.h"
#include "workproject.h"
#include "chtextedit.h"
#include "a2lfile.h"
#include "dbfile.h"
#include "chtextedit.h"
#include "dialog.h"
#include "formcompare.h"
#include "data.h"
#include "csv.h"
#include "dcmfile.h"
#include "dialogcsv.h"
#include "mainwindow.h"
#include "spreadsheetview.h"
#include "dialogchoosemodule.h"
#include <QtConcurrentRun>
#include <Qsci/qscilexerjavascript.h>
#include "cdfxfile.h"
#include <Qsci/qscilexerxml.h>
#include "choosesubset.h"
#include "dialogchooseexportformat.h"
#include "workingdirectory.h"
#include "deletefiledialog.h"
#include "treedirectory.h"
#include "QKeySequence"
#include "onlineupdater.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "qdebug.h"

MDImain::MDImain(QWidget *parent) : QMainWindow(parent), ui(new Ui::MDImain)
{
    //initialize ui
    ui->setupUi(this);

    setStyleSheet(
            "QMainWindow::separator {"
                 "width: 3px;"
                 "height: 3px;}"

             "QMainWindow::separator:hover {"
                 "}"

                 );

    //tabwidget desettings
    ui->tabWidget->clear();
    ui->tabWidget->setAcceptDrops(true);
    ui->tabWidget->setVisible(true);
//    QTextEdit *textEdit = new QTextEdit("drag and drop");
//    textEdit->setAcceptDrops(false);
//    ui->tabWidget->addTab(textEdit, "help");



    //restore main window sate
    this->readSettings();
    ui->Differentlabels_Dock->setVisible(false);
    //showMaximized();

    // menus / toolbars / statusbar
    createActions();
    createMenus();
    initToolBars();
    progBar = new QProgressBar(this);
    statusBar()->addWidget(progBar, 1);
    statusBar()->hide();
    compareTabs = 0;

    //initialize Treeview
    ui->treeView->header()->hide();
    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    connect(ui->treeView, SIGNAL(expanded(QModelIndex)), this, SLOT(resizeColumn0()));
    connect(ui->treeView, SIGNAL(collapsed(QModelIndex)), this, SLOT(resizeColumn0()));
    connect(ui->treeView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(doubleClicked(QModelIndex)));
    ui->treeView->setDragEnabled(true);
    ui->treeView->setAcceptDrops(true);
    ui->treeView->setDropIndicatorShown(true);
    ui->treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->treeView->setSelectionBehavior(QAbstractItemView::SelectItems);

    //initialize Treeview_2 (DiffLabel)
    ui->treeView_2->header()->hide();
    ui->treeView_2->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeView_2, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu_2(QPoint)));
    connect(ui->treeView_2, SIGNAL(expanded(QModelIndex)), this, SLOT(resizeColumn0()));
    connect(ui->treeView_2, SIGNAL(collapsed(QModelIndex)), this, SLOT(resizeColumn0()));

    //initialize Output dockpanel
    ui->listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->outputListWidget->resize(20000, 50);

    //Model
    model = new A2lTreeModel(0);

    //completer
    completer = new TreeModelCompleter(this);
    completer->setSeparator(QLatin1String("/"));
    completer->setCompletionMode(QCompleter::PopupCompletion);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setMaxVisibleItems(20);

    //initialize pointers
    projectList = new QMultiMap<QString, WorkProject*>;
    tabList = new QMap<QString, QWidget*>;

    //connect the slots
    connect(ui->tabWidget, SIGNAL(textDropped(QString)), this, SLOT(checkDroppedFile(QString)));
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabWidget_currentChanged(int)));
    connect(ui->treeView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(nodeSelected()));
    connect(ui->tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(removeTab(int)));

    //clipboard
    indexClipBoard = QModelIndex();
    nodeClipBoard = nullptr;
    pathClipBoard = "";

    //check for updates
    connect(this, SIGNAL(checkUpdates()), this, SLOT(initCheckHttpUpdates()), Qt::QueuedConnection);
    QSettings settings(qApp->organizationName(), qApp->applicationName());
    if ((settings.value("Update/automatic") == true) || (!settings.contains("Update/automatic")))
        emit checkUpdates();

    //create settings variables for multi_thread and lexer type
    if (!settings.contains("qThread"))
        settings.setValue("qThread", 1);
    if (!settings.contains("lexer"))
        settings.setValue("lexer", "mylexer");
    settings.setValue("lexer", "mylexer");
    if (!settings.contains("theme"))
        settings.setValue("theme", "dark");

    // initialize working directory
    int autoWD = settings.value("autoWD").toInt();
    if (autoWD == 1)
    {
        workingDirectory = settings.value("currentWDPath").toString();
        QStringList list = workingDirectory.split(";");
        workingDirectory = "";
        settings.setValue("currentWDPath", "");
        foreach (QString _str, list)
        {
            if (!_str.isEmpty())
            {
                QDir dir(_str);
                if (dir.exists())
                {
                    openWorkingDirectory(_str);
                }

            }
        }
        settings.setValue("currentWDPath", workingDirectory);
    }

    //default stylesheet
    this->default_style_sheet = qApp->styleSheet();

    //set theme
    if (settings.value("theme") == "dark")
    {
        this->setStyle("dark");
    }
    else
        this->setStyle("white");


    //Get userName
    getUserName();
}

MDImain::~MDImain()
{
    delete ui;
    delete projectList;
    delete tabList;
}

// ----------------- Style ----------------------//
void MDImain::setStyle(QString style)
{
    QSettings settings(qApp->organizationName(), qApp->applicationName());
    if (style == "dark")
    {

        QFile file(":/files/darkstyle.qss");
        file.open(QIODevice::ReadOnly);
        QString s;
        QTextStream in(&file);
        s.append(in.readAll());
        file.close();

        qApp->setStyleSheet(s);
         settings.setValue("theme", "dark");
    }
    else if (style == "white")
    {
        qApp->setStyleSheet(this->default_style_sheet);
        settings.setValue("theme", "white");
    }
}

// ----------------- Menu -----------------------//
void MDImain::closeEvent(QCloseEvent *e)
{
    if (checkChangedHexFiles() && checkChangedCsvFiles()
            && checkChangedSrecFiles() && checkChangedCdfxFiles())
    {
        QSettings settings(qApp->organizationName(), qApp->applicationName());
        settings.setValue("geometry", saveGeometry());
        settings.setValue("windowState", saveState());
        e->accept();
    }
    else
        e->ignore();
}

void MDImain::readSettings()
{
    QSettings settings(qApp->organizationName(), qApp->applicationName());
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
}

void MDImain::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MDImain::createActions()
{
    //actions for recent files menu
    for (int i = 0; i < MaxRecentFiles; ++i)
    {
        recentFileActs[i] = new QAction(this);
        recentFileActs[i]->setVisible(false);
        connect(recentFileActs[i], SIGNAL(triggered()), this, SLOT(openRecentFile()));
    }

    createDirectory = new QAction(tr("Add a new directory"), this);
    connect(createDirectory, SIGNAL(triggered()), this, SLOT(onCreateDirectory()));
    createDirectory->setDisabled(true);

    copyDataset = new QAction(tr("Copy"), this);
    copyDataset->setIcon(QIcon(":/icones/copy.png"));
    copyDataset->setShortcut(Qt::CTRL | Qt::Key_C);
    connect(copyDataset, SIGNAL(triggered()), this, SLOT(onCopyDataset()));
    copyDataset->setDisabled(true);

    pasteDataset = new QAction(tr("Paste"), this);
    pasteDataset->setIcon(QIcon(":/icones/paste.png"));
    pasteDataset->setShortcut(Qt::CTRL | Qt::Key_V);
    connect(pasteDataset, SIGNAL(triggered()), this, SLOT(onPasteDataset()));
    pasteDataset->setDisabled(true);

    editInHDrive = new QAction(tr("Show file or folder on disk"), this);
    connect(editInHDrive, SIGNAL(triggered()), this, SLOT(editInHDDrive()));
    editInHDrive->setDisabled(true);

    importSubsets = new QAction(tr("Import subsets"), this);
    importSubsets->setIcon(QIcon(":/icones/milky_importSubset.png"));
    connect(importSubsets, SIGNAL(triggered()), this, SLOT(import_Subsets()));
    importSubsets->setDisabled(true);

    exportSubsets = new QAction(tr("Export subsets"), this);
    exportSubsets->setIcon(QIcon(":/icones/milky_exportSubset.png"));
    connect(exportSubsets, SIGNAL(triggered()), this, SLOT(export_Subsets()));
    exportSubsets->setDisabled(true);

    openJScript = new QAction(tr("Run java script"), this);
    openJScript->setIcon(QIcon(":/icones/run.png"));
    connect(openJScript, SIGNAL(triggered()), this, SLOT(newFormScript()));
    openJScript->setDisabled(true);

    editMeasChannels = new QAction(tr("Edit measuring channels"), this);
    editMeasChannels->setIcon(QIcon(":/icones/milky_peigne.png"));
    connect(editMeasChannels, SIGNAL(triggered()), this, SLOT(editMeasuringChannels()));
    editMeasChannels->setDisabled(true);

    editCharacteristics = new QAction(tr("Edit characteristics"), this);
    editCharacteristics->setIcon(QIcon(":/icones/milky_outils.png"));
    connect(editCharacteristics, SIGNAL(triggered()), this, SLOT(editChar()));
    editCharacteristics->setDisabled(true);

    editFnR = new QAction(tr("Edit FnR"), this);
    editFnR->setIcon(QIcon(":/icones/milky_outils.png"));
    connect(editFnR, SIGNAL(triggered()), this, SLOT(editFandR()));
    editFnR->setDisabled(true);

    editObdMerge = new QAction(tr("Edit OBD matrix view"), this);
    editObdMerge->setIcon(QIcon(":/icones/milky_outils.png"));
    connect(editObdMerge, SIGNAL(triggered()), this, SLOT(editObd_Merge()));
    editObdMerge->setDisabled(true);

    spaceRecover = new QAction(tr("space Recovery"), this);
    spaceRecover->setIcon(QIcon(":/icones/milky_outils.png"));
    connect(spaceRecover, SIGNAL(triggered()), this, SLOT(spaceRecovery()));
    spaceRecover->setDisabled(true);

    exportListData = new QAction(tr("Export list"), this);
    exportListData->setIcon(QIcon(":/icones/export.png"));
    connect(exportListData, SIGNAL(triggered()), this, SLOT(export_ListData()));
    exportListData->setDisabled(false);

    deleteProject = new QAction(tr("Remove project"), this);
    deleteProject->setIcon(QIcon(":/icones/milky_ciseau.png"));
    //deleteProject->setShortcut(Qt::Key_Delete);
    connect(deleteProject, SIGNAL(triggered()), this, SLOT(removeWorkProjects()));
    deleteProject->setDisabled(true);

    addHexFile = new QAction(tr("Import Hex file"), this);
    addHexFile->setIcon(QIcon(":/icones/milky_importHEX.png"));
    connect(addHexFile, SIGNAL(triggered()), this, SLOT(addHexFile2Project()));
    addHexFile->setDisabled(true);

    addSrecFile = new QAction(tr("Import Srec file"), this);
    addSrecFile->setIcon(QIcon(":/icones/milky_importS19.png"));
    connect(addSrecFile, SIGNAL(triggered()), this, SLOT(addSrecFile2Project()));
    addSrecFile->setDisabled(true);

    addCsvFile = new QAction(tr("Import Csv file"), this);
    addCsvFile->setIcon(QIcon(":/icones/milky_importCsv.png"));
    connect(addCsvFile, SIGNAL(triggered()), this, SLOT(addCsvFile2Project()));
    addCsvFile->setDisabled(true);

    addCdfxFile = new QAction(tr("Import Cdf file"), this);
    addCdfxFile->setIcon(QIcon(":/icones/milky_importCdfx.png"));
    connect(addCdfxFile, SIGNAL(triggered()), this, SLOT(addCdfxFile2Project()));
    addCdfxFile->setDisabled(true);

    addDcmFile = new QAction(tr("Import Dcm file"), this);
    addDcmFile->setIcon(QIcon(":/icones/milky_importDcm.png"));
    connect(addDcmFile, SIGNAL(triggered()), this, SLOT(addDcmFile2Project()));
    addDcmFile->setDisabled(true);

    resetAllChangedData = new QAction(tr("Reset all changes"), this);
    resetAllChangedData->setIcon(QIcon(":/icones/milky_resetAll.png"));
    resetAllChangedData->setShortcut(Qt::CTRL | Qt::Key_U);
    connect(resetAllChangedData, SIGNAL(triggered()), this, SLOT(reset_AllChangedData()));
    resetAllChangedData->setDisabled(true);

    sortBySubset = new QAction(tr("Sort changes by subset"), this);
    sortBySubset->setIcon(QIcon(":/icones/viewtree.png"));
    connect(sortBySubset, SIGNAL(triggered()), this, SLOT(sort_BySubset()));
    sortBySubset->setDisabled(true);

    editFile = new QAction(tr("Edit as text"), this);
    editFile->setIcon(QIcon(":/icones/milky_editText.png"));
    connect(editFile, SIGNAL(triggered()), this, SLOT(editTextFile()));
    editFile->setDisabled(true);

    deleteFile = new QAction(tr("Remove file from project"), this);
    deleteFile->setIcon(QIcon(":/icones/milky_ciseau.png"));
    //deleteFile->setShortcut(Qt::Key_Delete);
    connect(deleteFile, SIGNAL(triggered()), this, SLOT(deleteFilesFromProject()));
    deleteFile->setDisabled(true);

    saveFile = new QAction(tr("Save"), this);
    saveFile->setIcon(QIcon(":/icones/milky_save.png"));
    saveFile->setShortcut(Qt::CTRL | Qt::Key_S);
    connect(saveFile, SIGNAL(triggered()), this, SLOT(save_File()));
    saveFile->setDisabled(true);

    saveAsFile = new QAction(tr("Save as ..."), this);
    saveAsFile->setIcon(QIcon(":/icones/milky_saveas.png"));
    saveAsFile->setShortcut(QKeySequence::fromString("Ctrl+Shift+S"));
    connect(saveAsFile, SIGNAL(triggered()), this, SLOT(saveAs_File()));
    saveAsFile->setDisabled(true);

    compareHexFile = new QAction(tr("Compare dataset"), this);
    compareHexFile->setIcon(QIcon(":/icones/copy.png"));
    connect(compareHexFile, SIGNAL(triggered()), this, SLOT(compare_HexFile()));
    compareHexFile->setDisabled(false);

    compareA2lFile = new QAction(tr("Compare A2L files"), this);
    compareA2lFile->setIcon(QIcon(":/icones/copy.png"));
    connect(compareA2lFile, SIGNAL(triggered()), this, SLOT(compare_A2lFile()));
    compareA2lFile->setDisabled(false);

    quicklook = new QAction(tr("Quicklook"), this);
    quicklook->setIcon(QIcon(":/icones/milky_loopHex.png"));
    quicklook->setShortcut(Qt::Key_Space);
    connect(quicklook, SIGNAL(triggered()), this, SLOT(quicklookFile()));
    quicklook->setDisabled(true);

    resetData = new QAction(tr("Reset data"), this);
    resetData->setIcon(QIcon(":/icones/milky_resetAll.png"));
    connect(resetData, SIGNAL(triggered()), this, SLOT(reset_Data()));

    clearOutput = new QAction(tr("Clear"), this);
    clearOutput->setIcon(QIcon(":/icones/milky_pinceau.png"));
    connect(clearOutput, SIGNAL(triggered()), this, SLOT(clear_Output()));

    plotData = new QAction(tr("Plot"), this);
    plotData->setIcon(QIcon(":/icones/AXIS.bmp"));
    plotData->setStatusTip(tr("plot"));
    connect(plotData, SIGNAL(triggered()), this, SLOT(fplotData()));

    readValuesFromCsv = new QAction(tr("Import CSV values"), this);
    readValuesFromCsv->setIcon(QIcon(":/icones/milky_importCsv.png"));
    readValuesFromCsv->setDisabled(true);
    connect(readValuesFromCsv, SIGNAL(triggered()), this, SLOT(read_ValuesFromCsv()));

    readValuesFromCdfx = new QAction(tr("Import CDF values"), this);
    readValuesFromCdfx->setIcon(QIcon(":/icones/milky_importCdfx.png"));
    readValuesFromCdfx->setDisabled(true);
    connect(readValuesFromCdfx, SIGNAL(triggered()), this, SLOT(read_ValuesFromCdfx()));

    showParam = new QAction(tr("Show fix parameters"), this);
    connect(showParam, SIGNAL(triggered()), this, SLOT(showFixPar()));
    showParam->setDisabled(true);

    childCount = new QAction(tr("Count elements"), this);
    connect(childCount, SIGNAL(triggered()), this, SLOT(countChild()));
    childCount->setDisabled(true);

    verify = new QAction(tr("Verify"), this);
    connect(verify, SIGNAL(triggered()), this, SLOT(verifyMaxTorque()));
    verify->setDisabled(true);

    checkFmtc = new QAction(tr("Check FMTC monotony"), this);
    connect(checkFmtc, SIGNAL(triggered()), this, SLOT(checkFmtcMonotony()));
    checkFmtc->setDisabled(true);

    editChanged = new QAction(tr("Edit all changes"), this);
    editChanged->setIcon(QIcon(":/icones/milky_find.png"));
    connect(editChanged, SIGNAL(triggered()), this, SLOT(editChangedLabels()));
    editChanged->setDisabled(true);

    editLabel= new QAction(tr("Edit"), this);
    editLabel->setIcon(QIcon(":/icones/eye.png"));
    connect(editLabel, SIGNAL(triggered()), this, SLOT(edit()));
    editLabel->setDisabled(false);

    editLabelCompare= new QAction(tr("Edit"), this);
    editLabelCompare->setIcon(QIcon(":/icones/eye.png"));
    connect(editLabelCompare, SIGNAL(triggered()), this, SLOT(editCompare()));
    editLabelCompare->setDisabled(false);

    saveA2lDB = new QAction(tr("Save A2l into DB"), this);
    saveA2lDB->setIcon(QIcon(":/icones/milky_exportBaril.png"));
    //connect(saveA2lDB, SIGNAL(triggered()), this, SLOT(exportA2lDb()));
    saveA2lDB->setDisabled(true);

    duplicateDatacontainer = new QAction(tr("Duplicate"), this);
    duplicateDatacontainer->setIcon(QIcon(":/icones/copy.png"));
    duplicateDatacontainer->setShortcut(Qt::CTRL | Qt::Key_D);
    connect(duplicateDatacontainer, SIGNAL(triggered()), this, SLOT(on_actionDuplicate_DataContainer_triggered()));
    duplicateDatacontainer->setDisabled(true);

    toolsMenu = new QMenu();
    toolsMenu->setTitle("tools");
    toolsMenu->setIcon(QIcon(":/icones/ToolboxFolder.png"));
    //toolsMenu->setIcon(QIcon(":/icones/milky_outils.png"));
    toolsMenu->addAction(verify);
    toolsMenu->addAction(checkFmtc);
    //toolsMenu->addAction(childCount);

    recentProMenu = new QMenu();
    recentProMenu->setTitle("recent projects");

}

void MDImain::createMenus()
{
    //window menu
    QMenu* menu = this->createPopupMenu();
    menu->setTitle("Docks and Toolbars");
    ui->menuWindow->addMenu(menu);

    // create recent files menu
    for (int i = 0; i < MaxRecentFiles; ++i)
    {
         ui->menuRecent_Files->addAction(recentFileActs[i]);
        //recentFileActs[i]->setParent(ui->actionRecent_Files);
    }
    updateRecentFileActions();

}

void MDImain::on_listWidget_customContextMenuRequested()
{
    QMenu menu;
    menu.addAction(clearOutput);
    menu.exec(QCursor::pos());
}

void MDImain::initToolBars()
{
    // Working Directory : WD
    ui->toolBar_WD->addAction(ui->actionOpen_Working_Directory);
    ui->toolBar_WD->addAction(ui->actionUpdateWorkingDirectory);

    // Project : A2l
    ui->toolBar_a2l->addAction(ui->actionNewA2lProject);
    ui->toolBar_a2l->addAction(addHexFile);
    ui->toolBar_a2l->addAction(addSrecFile);
    ui->toolBar_a2l->addAction(addCsvFile);
    ui->toolBar_a2l->addAction(addCdfxFile);
    ui->toolBar_a2l->addAction(addDcmFile);
    ui->toolBar_a2l->addSeparator();
    ui->toolBar_a2l->addAction(deleteProject);
    ui->toolBar_a2l->addAction(editFile);
    ui->toolBar_a2l->addAction(editMeasChannels);
    ui->toolBar_a2l->addAction(editCharacteristics);

    // Data : Hex or Csv or Cdf file
    ui->toolBar_data->addAction(quicklook);
    ui->toolBar_data->addAction(readValuesFromCsv);
    ui->toolBar_data->addAction(readValuesFromCdfx);
    ui->toolBar_data->addAction(resetAllChangedData);
    ui->toolBar_data->addAction(sortBySubset);
    ui->toolBar_data->addSeparator();
    ui->toolBar_data->addAction(copyDataset);
    ui->toolBar_data->addAction(pasteDataset);
    ui->toolBar_data->addAction(duplicateDatacontainer);
    ui->toolBar_data->addAction(ui->actionRename_file);
    ui->toolBar_data->addAction(saveFile);
    ui->toolBar_data->addAction(saveAsFile);
    ui->toolBar_data->addAction(deleteFile);

    ui->toolBar_data->hide();
}

void MDImain::on_actionExit_triggered()
{
    close();
}

void MDImain::openRecentFile()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
    {
        //loadFile(action->data().toString());
        if (QFile::exists(action->data().toString()))
        {
            QString qstr = action->data().toString();
            openProject(qstr);
        }
        else
        {
            QMessageBox::information(this, "Hexplorer :: open recent project",
                                     "the file " + action->data().toString() + " cannot be found.");
        }
    }
}

void MDImain::on_treeView_clicked(QModelIndex index)
{
    //get the type of the selected node
    Node *node = model->getNode(index);
    QString name = typeid(*node).name();

    //update the context menu
    if (name.endsWith("A2LFILE"))
    {
        ui->toolBar_data->hide();

        importSubsets->setEnabled(false);
        exportSubsets->setEnabled(false);
        addCdfxFile->setEnabled(true);
        addCsvFile->setEnabled(true);
        addDcmFile->setEnabled(true);
        editChanged->setEnabled(false);
        addHexFile->setEnabled(true);
        addSrecFile->setEnabled(true);
        deleteProject->setEnabled(true);
        deleteFile->setEnabled(false);
        editFile->setEnabled(true);
        childCount->setEnabled(true);
        showParam->setEnabled(false);
        resetAllChangedData->setEnabled(false);
        sortBySubset->setEnabled(false);
        saveFile->setEnabled(false);
        saveAsFile->setEnabled(false);
        quicklook->setEnabled(false);
        readValuesFromCsv->setEnabled(false);
        readValuesFromCdfx->setEnabled(false);
        editMeasChannels->setEnabled(true);
        editCharacteristics->setEnabled(true);
        openJScript->setEnabled(true);
        saveA2lDB->setEnabled(false);
        ui->actionClose_Working_Directory->setEnabled(false);
        ui->actionRename_file->setEnabled(false);
        ui->actionUpdateWorkingDirectory->setEnabled(false);
        copyDataset->setEnabled(false);
        pasteDataset->setEnabled(true);
        createDirectory->setEnabled(false);
    }
    else if (name.endsWith("WorkProject"))
    {
        ui->toolBar_data->hide();

        importSubsets->setEnabled(false);
        exportSubsets->setEnabled(false);
        addCdfxFile->setEnabled(true);
        addCsvFile->setEnabled(true);
        addDcmFile->setEnabled(true);
        editChanged->setEnabled(false);
        addHexFile->setEnabled(true);
        addSrecFile->setEnabled(true);
        deleteProject->setEnabled(true);
        deleteFile->setEnabled(false);
        editFile->setEnabled(true);
        childCount->setEnabled(true);
        showParam->setEnabled(false);
        resetAllChangedData->setEnabled(false);
        sortBySubset->setEnabled(false);
        saveFile->setEnabled(false);
        saveAsFile->setEnabled(false);
        quicklook->setEnabled(false);
        readValuesFromCsv->setEnabled(false);
        readValuesFromCdfx->setEnabled(false);
        editMeasChannels->setEnabled(true);
        editCharacteristics->setEnabled(true);
        openJScript->setEnabled(true);
        saveA2lDB->setEnabled(false);
        ui->actionClose_Working_Directory->setEnabled(false);
        ui->actionRename_file->setEnabled(false);
        editInHDrive->setEnabled(true);
        ui->actionUpdateWorkingDirectory->setEnabled(false);
        copyDataset->setEnabled(false);
        pasteDataset->setEnabled(true);
        createDirectory->setEnabled(true);
    }
    else if (name.endsWith("DBFILE"))
    {
        ui->toolBar_data->hide();

        importSubsets->setEnabled(false);
        exportSubsets->setEnabled(false);
        addCdfxFile->setEnabled(false);
        addCsvFile->setEnabled(false);
        addDcmFile->setEnabled(false);
        editChanged->setEnabled(false);
        addHexFile->setEnabled(false);
        addSrecFile->setEnabled(false);
        deleteProject->setEnabled(true);
        deleteFile->setEnabled(false);
        editFile->setEnabled(false);
        childCount->setEnabled(true);
        showParam->setEnabled(false);
        resetAllChangedData->setEnabled(false);
        sortBySubset->setEnabled(false);
        saveFile->setEnabled(false);
        saveAsFile->setEnabled(false);
        quicklook->setEnabled(false);
        readValuesFromCsv->setEnabled(false);
        readValuesFromCdfx->setEnabled(false);
        editMeasChannels->setEnabled(false);
        editCharacteristics->setEnabled(false);
        openJScript->setEnabled(false);
        saveA2lDB->setEnabled(false);
        ui->actionClose_Working_Directory->setEnabled(false);
        ui->actionRename_file->setEnabled(false);
        editInHDrive->setEnabled(false);
        ui->actionUpdateWorkingDirectory->setEnabled(false);
        copyDataset->setEnabled(false);
        pasteDataset->setEnabled(false);
        createDirectory->setEnabled(false);
    }
    else if (name.endsWith("HexFile"))
    {
        importSubsets->setEnabled(true);
        exportSubsets->setEnabled(true);
        addCdfxFile->setEnabled(false);
        addCsvFile->setEnabled(false);
        addHexFile->setEnabled(false);
        addSrecFile->setEnabled(false);
        addDcmFile->setEnabled(false);
        deleteProject->setEnabled(false);
        deleteFile->setEnabled(true);
        editFile->setEnabled(true);
        childCount->setEnabled(true);
        showParam->setEnabled(false);
        resetAllChangedData->setEnabled(true);
        sortBySubset->setEnabled(true);
        saveFile->setEnabled(true);
        saveAsFile->setEnabled(true);
        quicklook->setIcon(QIcon(":/icones/milky_loopHex.png"));
        quicklook->setEnabled(true);
        readValuesFromCsv->setEnabled(true);
        readValuesFromCdfx->setEnabled(true);
        editMeasChannels->setEnabled(false);
        editCharacteristics->setEnabled(false);
        openJScript->setEnabled(false);
        saveA2lDB->setEnabled(false);
        ui->actionClose_Working_Directory->setEnabled(false);
        duplicateDatacontainer->setEnabled(true);
        ui->actionRename_file->setEnabled(true);
        editInHDrive->setEnabled(true);
        ui->actionUpdateWorkingDirectory->setEnabled(false);
        copyDataset->setEnabled(true);
        pasteDataset->setEnabled(true);
        createDirectory->setEnabled(false);

        ui->toolBar_data->show();

    }
    else if (name.endsWith("SrecFile"))
    {
        importSubsets->setEnabled(true);
        exportSubsets->setEnabled(true);
        addCdfxFile->setEnabled(false);
        addCsvFile->setEnabled(false);
        addHexFile->setEnabled(false);
        addSrecFile->setEnabled(false);
        addDcmFile->setEnabled(false);
        deleteProject->setEnabled(false);
        deleteFile->setEnabled(true);
        editFile->setEnabled(true);
        childCount->setEnabled(true);
        showParam->setEnabled(false);
        resetAllChangedData->setEnabled(true);
        sortBySubset->setEnabled(true);
        saveFile->setEnabled(true);
        saveAsFile->setEnabled(true);
        quicklook->setIcon(QIcon(":/icones/milky_loopHex.png"));
        quicklook->setEnabled(true);
        readValuesFromCsv->setEnabled(true);
        readValuesFromCdfx->setEnabled(true);
        editMeasChannels->setEnabled(false);
        editCharacteristics->setEnabled(false);
        openJScript->setEnabled(false);
        saveA2lDB->setEnabled(false);
        ui->actionClose_Working_Directory->setEnabled(false);
        ui->actionRename_file->setEnabled(true);
        duplicateDatacontainer->setEnabled(true);
        editInHDrive->setEnabled(true);
        ui->actionUpdateWorkingDirectory->setEnabled(false);
        copyDataset->setEnabled(true);
        pasteDataset->setEnabled(true);
        createDirectory->setEnabled(false);

        ui->toolBar_data->show();

    }
    else if (name.endsWith("Csv"))
    {
        importSubsets->setEnabled(false);
        exportSubsets->setEnabled(false);
        addCdfxFile->setEnabled(false);
        addCsvFile->setEnabled(false);
        addHexFile->setEnabled(false);
        addSrecFile->setEnabled(false);
        addDcmFile->setEnabled(false);
        deleteProject->setEnabled(false);
        deleteFile->setEnabled(true);
        editFile->setEnabled(true);
        childCount->setEnabled(true);
        showParam->setEnabled(false);
        resetAllChangedData->setEnabled(true);
        sortBySubset->setEnabled(true);
        saveFile->setEnabled(true);
        saveAsFile->setEnabled(true);
        quicklook->setIcon(QIcon(":/icones/milky_loopCsv.png"));
        quicklook->setEnabled(true);
        readValuesFromCsv->setEnabled(false);
        readValuesFromCdfx->setEnabled(false);
        editMeasChannels->setEnabled(false);
        editCharacteristics->setEnabled(false);
        openJScript->setEnabled(false);
        saveA2lDB->setEnabled(false);
        ui->actionClose_Working_Directory->setEnabled(false);
        ui->actionRename_file->setEnabled(false);
        duplicateDatacontainer->setEnabled(false);
        editInHDrive->setEnabled(true);
        ui->actionUpdateWorkingDirectory->setEnabled(false);
        copyDataset->setEnabled(false);
        pasteDataset->setEnabled(false);
        createDirectory->setEnabled(false);

        ui->toolBar_data->show();
    }
    else if (name.endsWith("CdfxFile"))
    {
        exportSubsets->setEnabled(false);
        addCdfxFile->setEnabled(false);
        addCsvFile->setEnabled(false);
        addHexFile->setEnabled(false);
        addSrecFile->setEnabled(false);
        addDcmFile->setEnabled(false);
        deleteProject->setEnabled(false);
        deleteFile->setEnabled(true);
        editFile->setEnabled(true);
        childCount->setEnabled(true);
        showParam->setEnabled(false);
        resetAllChangedData->setEnabled(true);
        sortBySubset->setEnabled(true);
        saveFile->setEnabled(true);
        saveAsFile->setEnabled(true);
        quicklook->setIcon(QIcon(":/icones/milky_loopCsv.png"));
        quicklook->setEnabled(true);
        readValuesFromCsv->setEnabled(false);
        readValuesFromCdfx->setEnabled(false);
        editMeasChannels->setEnabled(false);
        editCharacteristics->setEnabled(false);
        openJScript->setEnabled(false);
        saveA2lDB->setEnabled(false);
        ui->actionClose_Working_Directory->setEnabled(false);
        ui->actionRename_file->setEnabled(false);
        duplicateDatacontainer->setEnabled(false);
        editInHDrive->setEnabled(true);
        ui->actionUpdateWorkingDirectory->setEnabled(false);
        copyDataset->setEnabled(false);
        pasteDataset->setEnabled(false);
        createDirectory->setEnabled(false);

        ui->toolBar_data->show();
    }  
    else if (name.endsWith("Dcm"))
    {
        exportSubsets->setEnabled(false);
        addCdfxFile->setEnabled(false);
        addCsvFile->setEnabled(false);
        addHexFile->setEnabled(false);
        addSrecFile->setEnabled(false);
        addDcmFile->setEnabled(false);
        deleteProject->setEnabled(false);
        deleteFile->setEnabled(true);
        editFile->setEnabled(true);
        childCount->setEnabled(true);
        showParam->setEnabled(false);
        resetAllChangedData->setEnabled(true);
        sortBySubset->setEnabled(true);
        saveFile->setEnabled(false);
        saveAsFile->setEnabled(true);
        quicklook->setIcon(QIcon(":/icones/milky_loopCsv.png"));
        quicklook->setEnabled(true);
        readValuesFromCsv->setEnabled(false);
        readValuesFromCdfx->setEnabled(false);
        editMeasChannels->setEnabled(false);
        editCharacteristics->setEnabled(false);
        openJScript->setEnabled(false);
        saveA2lDB->setEnabled(false);
        ui->actionClose_Working_Directory->setEnabled(false);
        ui->actionRename_file->setEnabled(false);
        duplicateDatacontainer->setEnabled(false);
        editInHDrive->setEnabled(true);
        ui->actionUpdateWorkingDirectory->setEnabled(false);
        copyDataset->setEnabled(false);
        pasteDataset->setEnabled(false);
        createDirectory->setEnabled(false);

        ui->toolBar_data->show();
    }
    else if (name.toLower().endsWith("workingdirectory"))
    {
        ui->toolBar_data->hide();

        ui->actionUpdateWorkingDirectory->setEnabled(true);
        importSubsets->setEnabled(false);
        exportSubsets->setEnabled(false);
        addCdfxFile->setEnabled(false);
        addCsvFile->setEnabled(false);
        addDcmFile->setEnabled(false);
        editChanged->setEnabled(false);
        addHexFile->setEnabled(false);
        addSrecFile->setEnabled(false);
        deleteProject->setEnabled(false);
        deleteFile->setEnabled(false);
        editFile->setEnabled(false);
        childCount->setEnabled(false);
        showParam->setEnabled(false);
        resetAllChangedData->setEnabled(false);
        sortBySubset->setEnabled(false);
        saveFile->setEnabled(false);
        saveAsFile->setEnabled(false);
        quicklook->setEnabled(false);
        readValuesFromCsv->setEnabled(false);
        readValuesFromCdfx->setEnabled(false);
        editMeasChannels->setEnabled(false);
        editCharacteristics->setEnabled(false);
        openJScript->setEnabled(false);
        saveA2lDB->setEnabled(false);
        ui->actionClose_Working_Directory->setEnabled(true);
        ui->actionRename_file->setEnabled(false);
        duplicateDatacontainer->setEnabled(false);
        editInHDrive->setEnabled(true);
        copyDataset->setEnabled(false);
        pasteDataset->setEnabled(false);
        createDirectory->setEnabled(false);

    }
    else if (name.toLower().endsWith("treedirectory"))
    {
        ui->actionUpdateWorkingDirectory->setEnabled(false);
        importSubsets->setEnabled(false);
        exportSubsets->setEnabled(false);
        addCdfxFile->setEnabled(false);
        addCsvFile->setEnabled(false);
        addDcmFile->setEnabled(false);
        editChanged->setEnabled(false);
        addHexFile->setEnabled(false);
        addSrecFile->setEnabled(false);
        deleteProject->setEnabled(false);
        deleteFile->setEnabled(false);
        editFile->setEnabled(false);
        childCount->setEnabled(false);
        showParam->setEnabled(false);
        resetAllChangedData->setEnabled(false);
        sortBySubset->setEnabled(false);
        saveFile->setEnabled(false);
        saveAsFile->setEnabled(false);
        quicklook->setEnabled(false);
        readValuesFromCsv->setEnabled(false);
        readValuesFromCdfx->setEnabled(false);
        editMeasChannels->setEnabled(false);
        editCharacteristics->setEnabled(false);
        openJScript->setEnabled(false);
        saveA2lDB->setEnabled(false);
        ui->actionClose_Working_Directory->setEnabled(true);
        ui->actionRename_file->setEnabled(false);
        duplicateDatacontainer->setEnabled(false);
        editInHDrive->setEnabled(false);
        copyDataset->setEnabled(false);
        pasteDataset->setEnabled(true);
        createDirectory->setEnabled(false);
    }
    else
    {
        ui->toolBar_data->hide();

        importSubsets->setEnabled(false);
        exportSubsets->setEnabled(false);
        addCdfxFile->setEnabled(false);
        addCsvFile->setEnabled(false);
        addHexFile->setEnabled(false);
        addSrecFile->setEnabled(false);
        addDcmFile->setEnabled(false);
        deleteProject->setEnabled(false);
        deleteFile->setEnabled(false);
        editFile->setEnabled(false);
        childCount->setEnabled(true);
        showParam->setEnabled(true);
        resetAllChangedData->setEnabled(false);
        sortBySubset->setEnabled(false);
        saveFile->setEnabled(false);
        saveAsFile->setEnabled(false);
        quicklook->setEnabled(false);
        readValuesFromCsv->setEnabled(false);
        editMeasChannels->setEnabled(false);
        editCharacteristics->setEnabled(false);
        openJScript->setEnabled(false);
        saveA2lDB->setEnabled(false);
        ui->actionClose_Working_Directory->setEnabled(false);
        ui->actionRename_file->setEnabled(false);
        duplicateDatacontainer->setEnabled(false);
        ui->actionLoad_DB->setEnabled(false);
        ui->actionUpdateWorkingDirectory->setEnabled(false);
        createDirectory->setEnabled(false);
    }

    //get the full path of the index into treeView
    QString path = QString(model->getNodeName(index));
    index = model->parent(index);
    while (index.isValid())
    {
        path = QString(model->getNodeName(index)) + "/" + path;
        index = model->parent(index);
    }
}

void MDImain::showContextMenu(QPoint)
{
    QMenu menu;

    if (model->getRootNode() == 0)
    {

        menu.addAction(ui->actionOpen_Working_Directory);
        menu.addSeparator();
        menu.addAction(ui->actionNewA2lProject);

        for (int i = 0; i < MaxRecentFiles; ++i)
        {
             recentProMenu->addAction(recentFileActs[i]);
        }
        updateRecentFileActions();
        menu.addMenu(recentProMenu);
        menu.addSeparator();
        menu.addAction(ui->actionLoad_DB);
    }
    else
    {
        QModelIndexList list = ui->treeView->selectionModel()->selectedIndexes();
        if (list.count() < 1)
        {
            menu.addAction(ui->actionOpen_Working_Directory);
            menu.addSeparator();
            menu.addAction(ui->actionNewA2lProject);

            for (int i = 0; i < MaxRecentFiles; ++i)
            {
                 recentProMenu->addAction(recentFileActs[i]);
            }
            updateRecentFileActions();
            menu.addMenu(recentProMenu);

            menu.addSeparator();
            menu.addAction(ui->actionLoad_DB);
        }
        else if (list.count() == 1)
        {
            QModelIndex index  = list.at(0);

            Node *node = model->getNode(index);
            QString name = typeid(*node).name();

            if (name.endsWith("Data"))
            {
                menu.addAction(resetData);
                menu.addAction(editLabel);
                menu.addAction(plotData);
            }
            else if (name.toLower().endsWith("hexfile"))
            {
                menu.addSeparator();
                menu.addAction(quicklook);
                menu.addAction(editFile);
                menu.addAction(editInHDrive);
                menu.addSeparator();
                menu.addAction(readValuesFromCsv);
                menu.addAction(readValuesFromCdfx);
                menu.addSeparator();
                //menu.addAction(importSubsets);
                menu.addAction(exportSubsets);
                menu.addSeparator();
                menu.addAction(editChanged);
                menu.addAction(resetAllChangedData);
                menu.addAction(sortBySubset);
                menu.addMenu(toolsMenu);
                menu.addSeparator();
                //menu.addAction(ui->actionDuplicate_DataContainer);
                menu.addAction(copyDataset);
                menu.addAction(pasteDataset);
                menu.addAction(duplicateDatacontainer);
                menu.addAction(ui->actionRename_file);
                menu.addAction(saveFile);
                menu.addAction(saveAsFile);
                menu.addSeparator();
                menu.addAction(deleteFile);             

                //menu editModified
                Node *node =  model->getNode(index);
                HexFile *hex = dynamic_cast<HexFile *> (node);

                // if hexfile not read do not open following context menu
                if (hex->isRead())
                {
                    if (((DataContainer*)hex)->getModifiedData().isEmpty())
                    {
                        sortBySubset->setDisabled(true);
                        resetAllChangedData->setDisabled(true);
                        editChanged->setDisabled(true);
                    }
                    else
                    {
                        sortBySubset->setDisabled(false);
                        resetAllChangedData->setDisabled(false);
                        editChanged->setDisabled(false);
                    }

                    //menu tools
                    A2LFILE *a2l = hex->getParentWp()->a2lFile;
                    QString projectName = (static_cast<A2LFILE*>(a2l)->getProject())->getPar("name");
                    projectName = projectName.toLower();

                    if (projectName == "c340" || projectName == "c342" || projectName == "p_662" || projectName == "p1603")
                    {
                        verify->setDisabled(false);
                        checkFmtc->setDisabled(false);
                    }
                    else
                    {
                        verify->setDisabled(true);
                        checkFmtc->setDisabled(true);
                    }

                }

            }
            else if (name.toLower().endsWith("srecfile"))
            {
                menu.addSeparator();
                menu.addAction(quicklook);
                menu.addAction(editFile);
                menu.addAction(editInHDrive);
                menu.addSeparator();
                menu.addAction(readValuesFromCsv);
                menu.addAction(readValuesFromCdfx);
                menu.addSeparator();
                //menu.addAction(importSubsets);
                menu.addAction(exportSubsets);
                menu.addSeparator();
                menu.addAction(editChanged);
                menu.addAction(resetAllChangedData);
                menu.addAction(sortBySubset);
                menu.addMenu(toolsMenu);
                menu.addAction(editFnR);
                menu.addAction(editObdMerge);
                menu.addAction(spaceRecover);
                menu.addSeparator();
                menu.addAction(copyDataset);
                menu.addAction(pasteDataset);
                menu.addAction(duplicateDatacontainer);
                menu.addAction(ui->actionRename_file);
                menu.addAction(saveFile);
                menu.addAction(saveAsFile);
                menu.addSeparator();
                menu.addAction(deleteFile);

                //menu editModified
                Node *node =  model->getNode(index);
                SrecFile *srec = dynamic_cast<SrecFile *> (node);

                // if Srecfile not read do not open context menu
                if (srec->isRead())
                {
                    editFnR->setDisabled(false);
                    editObdMerge->setDisabled(false);
                    spaceRecover->setDisabled(true);
                    if (srec->getModifiedData().isEmpty())
                    {
                        sortBySubset->setDisabled(true);
                        resetAllChangedData->setDisabled(true);
                        editChanged->setDisabled(true);
                    }
                    else
                    {                        
                        sortBySubset->setDisabled(false);
                        resetAllChangedData->setDisabled(false);
                        editChanged->setDisabled(false);
                    }
                }
            }
            else if (name.toLower().endsWith("a2lfile"))
            {
                A2LFILE *a2lFile = (A2LFILE*)node;
                if (a2lFile->isParsed())
                {
                    if (a2lFile->isConform())
                    {
                        menu.addAction(ui->actionNewA2lProject);
                        menu.addAction(ui->actionLoad_DB);
                        menu.addAction(deleteProject);
                        menu.addAction(editFile);
                        menu.addSeparator();
                        menu.addAction(pasteDataset);
                        menu.addAction(addHexFile);
                        menu.addAction(addSrecFile);
                        menu.addAction(addCsvFile);
                        menu.addAction(addCdfxFile);
                        menu.addSeparator();
                        menu.addAction(openJScript);
                        menu.addSeparator();
                        menu.addAction(editMeasChannels);
                        menu.addAction(editCharacteristics);
                        menu.addSeparator();
                        menu.addAction(saveA2lDB);
                        //menu.addAction(childCount);
                    }
                    else
                    {
                        menu.addAction(ui->actionNewA2lProject);
                        menu.addAction(deleteProject);
                        menu.addSeparator();
                        menu.addAction(editFile);
                    }
                }
                else
                {
                    menu.addAction(ui->actionNewA2lProject);
                    menu.addAction(ui->actionLoad_DB);
                    menu.addAction(deleteProject);
                    menu.addAction(editFile);
                    menu.addAction(editInHDrive);
                    menu.addSeparator();
                    menu.addAction(addHexFile);
                    menu.addAction(addSrecFile);
                    menu.addAction(addCsvFile);
                    menu.addAction(addCdfxFile);
                    menu.addSeparator();
                    menu.addAction(openJScript);
                    menu.addSeparator();
                    menu.addAction(editMeasChannels);
                    menu.addAction(editCharacteristics);
                    menu.addSeparator();
                    menu.addAction(saveA2lDB);
                }
             }
            else if (name.toLower().endsWith("workproject"))
            {
                A2LFILE *a2lFile = ((WorkProject*)node)->a2lFile;
                if (a2lFile->isParsed())
                {
                    if (a2lFile->isConform())
                    {
                        menu.addAction(ui->actionNewA2lProject);
                        menu.addAction(ui->actionLoad_DB);
                        menu.addAction(deleteProject);
                        menu.addAction(editFile);
                        menu.addSeparator();
                        menu.addAction(pasteDataset);
                        menu.addAction(addHexFile);
                        menu.addAction(addSrecFile);
                        menu.addAction(addCsvFile);
                        menu.addAction(addCdfxFile);
                        menu.addAction(addDcmFile);
                        menu.addSeparator();
                        menu.addAction(openJScript);
                        menu.addSeparator();
                        menu.addAction(editMeasChannels);
                        menu.addAction(editCharacteristics);
                        menu.addSeparator();
                        menu.addAction(saveA2lDB);
                        //menu.addAction(childCount);
                    }
                    else
                    {
                        menu.addAction(ui->actionNewA2lProject);
                        menu.addAction(deleteProject);
                        menu.addSeparator();
                        menu.addAction(editFile);
                    }
                }
                else
                {
                    menu.addAction(ui->actionNewA2lProject);
                    menu.addAction(ui->actionLoad_DB);
                    menu.addAction(deleteProject);
                    menu.addAction(editFile);
                    menu.addAction(editInHDrive);
                    menu.addSeparator();
                    menu.addAction(createDirectory);

                    menu.addAction(addHexFile);
                    menu.addAction(addSrecFile);
                    menu.addAction(addCsvFile);
                    menu.addAction(addCdfxFile);
                    menu.addAction(addDcmFile);
                    menu.addSeparator();
                    menu.addAction(openJScript);
                    menu.addSeparator();
                    menu.addAction(editMeasChannels);
                    menu.addAction(editCharacteristics);
                    menu.addSeparator();
                    menu.addAction(saveA2lDB);
                }
             }
            else if (name.toLower().endsWith("dbfile"))
            {
                menu.addAction(ui->actionNewA2lProject);
                menu.addAction(ui->actionLoad_DB);
                menu.addAction(deleteProject);
                //menu.addAction(editFile);
                menu.addAction(editInHDrive);
                menu.addSeparator();
                menu.addAction(addHexFile);
                menu.addAction(addSrecFile);
                menu.addAction(addCsvFile);
                menu.addAction(addCdfxFile);
                menu.addSeparator();
                menu.addAction(openJScript);
                //menu.addSeparator();
                //menu.addAction(editMeasChannels);
                //menu.addAction(editCharacteristics);
                //menu.addSeparator();
                //menu.addAction(saveA2lDB);
            }
            else if (name.toLower().endsWith("csv"))
            {
                menu.addAction(deleteFile);
                menu.addAction(editFile);
                menu.addAction(editInHDrive);
                menu.addSeparator();
                menu.addAction(saveFile);
                menu.addAction(saveAsFile);
                menu.addSeparator();
                menu.addAction(editObdMerge);
                menu.addSeparator();
                menu.addAction(quicklook);
                menu.addAction(editChanged);
                menu.addAction(resetAllChangedData);
                menu.addAction(sortBySubset);

                //menu editModified
                Node *node =  model->getNode(index);
                Csv *csv = dynamic_cast<Csv *> (node);

                editObdMerge->setDisabled(false);

                if (csv->getModifiedData().isEmpty())
                    editChanged->setDisabled(true);
                else
                    editChanged->setDisabled(false);
            }
            else if (name.toLower().endsWith("dcm"))
            {
                menu.addAction(deleteFile);
                menu.addAction(editFile);
                menu.addAction(editInHDrive);
                menu.addSeparator();
                menu.addAction(saveFile);
                menu.addAction(saveAsFile);
                menu.addSeparator();
                menu.addAction(editObdMerge);
                menu.addSeparator();
                menu.addAction(quicklook);
                menu.addAction(editChanged);
                menu.addAction(resetAllChangedData);
                menu.addAction(sortBySubset);

                //menu editModified
                Node *node =  model->getNode(index);
                Dcm *dcm = dynamic_cast<Dcm *> (node);

                editObdMerge->setDisabled(false);

                if (dcm->getModifiedData().isEmpty())
                    editChanged->setDisabled(true);
                else
                    editChanged->setDisabled(false);
            }
            else if (name.toLower().endsWith("cdfxfile"))
            {
                menu.addAction(deleteFile);
                menu.addAction(editFile);
                menu.addAction(editInHDrive);
                menu.addSeparator();
                menu.addAction(saveFile);
                menu.addAction(saveAsFile);
                menu.addSeparator();
                menu.addAction(editObdMerge);
                menu.addSeparator();
                menu.addAction(quicklook);
                menu.addAction(editChanged);
                menu.addAction(resetAllChangedData);
                menu.addAction(sortBySubset);

                //menu editModified
                Node *node =  model->getNode(index);
                CdfxFile *cdfx = dynamic_cast<CdfxFile *> (node);

                editObdMerge->setDisabled(false);

                if (cdfx->getModifiedData().isEmpty())
                    editChanged->setDisabled(true);
                else
                    editChanged->setDisabled(false);
            }
            else if (name.toLower().endsWith("workingdirectory"))
            {
                menu.addAction(ui->actionUpdateWorkingDirectory);
                menu.addAction(ui->actionClose_Working_Directory);
                menu.addAction(editInHDrive);

            }
            else if (name.toLower().endsWith("treedirectory"))
            {
                menu.addAction(pasteDataset);
            }
            else
            {
                menu.addAction(ui->actionNewA2lProject);
                menu.addAction(deleteProject);
                menu.addAction(addHexFile);
                menu.addAction(editFile);
                menu.addSeparator();
                menu.addAction(showParam);
                menu.addAction(childCount);
            }
        }
        else if (list.count() >= 2)
        {
            int weiterData = 0;
            int weiterHex = 0;
            int weiterA2l = 0;
            int weiterWD = 0;
            foreach (QModelIndex index, list)
            {
                Node *node = model->getNode(index);
                QString name = typeid(*node).name();
                if (name.endsWith("Data"))
                {
                    weiterData++;
                }
                else if (name.toLower().endsWith("hexfile") || name.toLower().endsWith("srecfile"))
                {
                    weiterHex++;
                }
                else if (name.toLower().endsWith("workproject"))
                {
                    weiterA2l++;
                }
                else if (name.toLower().endsWith("workingdirectory"))
                {
                    weiterWD++;
                }
            }

            if (weiterData == list.count())
            {
                menu.addAction(resetData);
                menu.addAction(editLabel);
                menu.addAction(plotData);
            }
            else if (weiterHex == list.count())
            {
                menu.addAction(deleteFile);
                if (weiterHex == 2)
                    menu.addAction(compareHexFile);
            }
            else if (weiterA2l == list.count())
            {
                menu.addAction(deleteProject);
                if (weiterA2l == 2)
                    menu.addAction(compareA2lFile);
            }
            else if (weiterWD == list.count())
            {
                menu.addAction(ui->actionClose_Working_Directory);
            }
        }
    }

    menu.exec(QCursor::pos());
}

void MDImain::showContextMenu_2(QPoint)
{
    QMenu menu;

    QModelIndex index  = ui->treeView_2->selectionModel()->currentIndex();
    if (index.isValid())
    {
        menu.addAction(exportListData);
        menu.addAction(editLabelCompare);
    }

    menu.exec(QCursor::pos());
}

void MDImain::verifyMaxTorque()
{
    QModelIndex index  = ui->treeView->selectionModel()->currentIndex();
    HexFile *hex = (HexFile*)model->getNode(index);
    hex->verify();
}

void MDImain::checkFmtcMonotony()
{
    QModelIndex index  = ui->treeView->selectionModel()->currentIndex();
    HexFile *hex = (HexFile*)model->getNode(index);
    bool bl;
    QList<int> list = hex->checkFmtcMonotony(&bl);

    if (bl)
    {
        QMessageBox::information(this, "HEXplorer :: FMTC monotony",
                                 "the whole FMTC map is strictly monotonic");
    }
    else
    {
        QString str;
        foreach (int col, list)
        {
            str.append(QString::number(col + 1) + ", ");
        }
        str.chop(2);
        QMessageBox::warning(this, "HEXplorer :: FMTC monotony",
                             "the FMTC map is NOT strictly monotonic : \n"
                             "check column(s) : " + str);
    }
}

void MDImain::on_actionSettings_triggered()
{
    MainWindow *win = new MainWindow();
    win->show();
}

void MDImain::on_actionAbout_triggered()
{
    QByteArray compiler = "";
    #if defined(_MSC_VER)
        compiler = "MSVC version " + QByteArray::number(_MSC_VER) +  " compiler.";
    #elif defined(__clang__)
        compiler = "Clang/LLVM version " +
                   QByteArray::number(__clang_major__) + "." +
                   QByteArray::number(__clang_minor__) + "." +
                   QByteArray::number(__clang_patchlevel__) + " compiler.";
    #elif defined(__GNUC__)
        compiler = "GCC version " + QByteArray::number(__GNUC__) + "." + QByteArray::number(__GNUC_MINOR__) + "." +
                QByteArray::number(__GNUC_PATCHLEVEL__) +  " compiler.";
    #endif

    QByteArray encodedString =
                   "build " + qApp->applicationVersion().toLocal8Bit() + " compiled with " + compiler + "\n\n"
                   "This software uses external libraries :\n"
                   "   - Qt framework " + QT_VERSION_STR + "\n"
                   "   - QScintilla 2.14 (as efficient text editor)\n"
                   "   - Qwt 6.2.0 (as 2D graph plotter)\n\n"
                   " christophe.hoel@gmail.com";

    //QTextCodec *codec = QTextCodec::codecForName("ISO 8859-1");
    //QString string = codec->toUnicode(encodedString);
    QString string = QString::fromLatin1(encodedString);

    QMessageBox::about(this, tr("HEXplorer :: About"), string);
}

void MDImain::on_actionCreate_database_triggered()
{
    //openInit();
}

void MDImain::on_actionSave_session_triggered()
{
    //serialize();
}

void MDImain::doubleClicked(QModelIndex)
{
    QModelIndex index  = ui->treeView->selectionModel()->currentIndex();
    Node *node = model->getNode(index);

    QString name = typeid(*node).name();
    if (name.toLower().endsWith("hexfile") || name.toLower().endsWith("srecfile") ||
        name.toLower().endsWith("csv") || name.toLower().endsWith("cdfxfile") || name.toLower().endsWith("dcm"))
    {
        quicklookFile();
    }
}

bool MDImain::checkChangedHexFiles()
{
    bool bl = true;

    // check if Hex files have changes
    foreach (WorkProject *wp, projectList->values())
    {
        foreach (HexFile *hex, wp->hexFiles().values())
        {
            if (hex->childNodes.count() != 0)
            {
                QMessageBox msgBox;
                msgBox.setText("HEXplorer :: exit");
                msgBox.setInformativeText("Save changes to " + hex->fullName());
                msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
                msgBox.setDefaultButton(QMessageBox::Save);
                int ret = msgBox.exec();


                if (ret == QMessageBox::Save)
                {
                    QModelIndex index = ((A2lTreeModel*)ui->treeView->model())->getIndex((Node*)hex);
                    save_HexFile(index);
                    bl = true;
                }
                else if (ret == QMessageBox::Discard)
                {
                    bl = true;
                }
                else
                {
                    return false;
                }

            }
        }
    }

    return bl;
}

bool MDImain::checkChangedSrecFiles()
{
    bool bl = true;

    // check if Hex files have changes
    foreach (WorkProject *wp, projectList->values())
    {
        foreach (SrecFile *srec, wp->srecFiles().values())
        {
            if (srec->childNodes.count() != 0)
            {
                QMessageBox msgBox;
                msgBox.setText("HEXplorer :: exit");
                msgBox.setInformativeText("Save changes to " + srec->fullName());
                msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
                msgBox.setDefaultButton(QMessageBox::Save);
                int ret = msgBox.exec();


                if (ret == QMessageBox::Save)
                {
                    QModelIndex index = ((A2lTreeModel*)ui->treeView->model())->getIndex(srec);
                    save_SrecFile(index);
                    bl = true;
                }
                else if (ret == QMessageBox::Discard)
                {
                    bl = true;
                }
                else
                {
                    return false;
                }

            }
        }
    }

    return bl;
}

bool MDImain::checkChangedCsvFiles()
{
    bool bl = true;

    // check if Hex files have changes
    foreach (WorkProject *wp, projectList->values())
    {
        foreach (Csv *csv, wp->csvFiles().values())
        {
            if (csv->childNodes.count() != 0)
            {
                QMessageBox msgBox;
                msgBox.setText("HEXplorer :: exit");
                msgBox.setInformativeText("Save changes to " + csv->fullName());
                msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
                msgBox.setDefaultButton(QMessageBox::Save);
                int ret = msgBox.exec();

                if (ret == QMessageBox::Save)
                {
                    QModelIndex index = ((A2lTreeModel*)ui->treeView->model())->getIndex(csv);
                    save_CsvFile(index);
                    bl = true;
                }
                else if (ret == QMessageBox::Discard)
                {
                    bl = true;
                }
                else
                {
                    return false;
                }

            }
        }
    }

    return bl;
}

bool MDImain::checkChangedCdfxFiles()
{
    bool bl = true;

    // check if Hex files have changes
    foreach (WorkProject *wp, projectList->values())
    {
        foreach (CdfxFile *cdfx, wp->cdfxFiles().values())
        {
            if (cdfx->childNodes.count() != 0)
            {
                QMessageBox msgBox;
                msgBox.setText("HEXplorer :: exit");
                msgBox.setInformativeText("Save changes to " + cdfx->fullName());
                msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
                msgBox.setDefaultButton(QMessageBox::Save);
                int ret = msgBox.exec();


                if (ret == QMessageBox::Save)
                {
                    QModelIndex index = ((A2lTreeModel*)ui->treeView->model())->getIndex(cdfx);
                    save_CdfxFile(index);
                    bl = true;
                }
                else if (ret == QMessageBox::Discard)
                {
                    bl = true;
                }
                else
                {
                    return false;
                }

            }
        }
    }

    return bl;
}

void MDImain::on_actionUpdateWorkingDirectory_triggered()
{
    QModelIndexList list = ui->treeView->selectionModel()->selectedIndexes();

    if (list.count() == 1)
    {
        //remove original Working Directory
        QModelIndex index = list.at(0);
        QString fullPath = dynamic_cast<WorkingDirectory*>(model->getNode(index))->getFullPath();
        //removeWorkingDirectory(index);

        //get a pointer on the selected item
        Node *nodeWd =  model->getNode(index);

        //remove all wprk projects into working directory
        foreach (Node *_node, nodeWd->childNodes)
        {
            QString name = typeid(*_node).name();
            if (name.toLower().endsWith("workproject"))
            {
                removeWorkProject(model->getIndex(_node));
            }
        }

        //re-load working Directory
        WorkingDirectory* wd = static_cast<WorkingDirectory*> (nodeWd);
        wd->parseDir(fullPath);
    }
}

void MDImain::completerSelected()
{

}

// ------------------ A2lFile ----------------------//

void MDImain::on_actionNewA2lProject_triggered()
{
    QSettings settings(qApp->organizationName(), qApp->applicationName());
    QString path = settings.value("currentA2lPath").toString();

    const QStringList files =
        QFileDialog::getOpenFileNames(this,
                                      tr("select an Asap file (A2L)"), path,
                                      tr("ASAP files (*.a2l);;all files (*.*)"));

    if (files.isEmpty())
    {
       statusBar()->showMessage(tr("Loading canceled"), 2000);
       writeOutput("action open new project : no file selected");
       return;
    }
    else
    {
        //intialize projectList if necessary
       if (projectList == NULL)
            projectList = new QMultiMap<QString, WorkProject*>;

       //check if the project is already open
       foreach (QString fileName, files)
       {
           openProject(fileName);
           ui->treeView->resizeColumnToContents(0);
       }
   }
}

void MDImain::openProject(QString &fullFileName)
{
    if (projectList->contains(fullFileName))
    {
        QMessageBox::information(this, "HEXplorer", fullFileName + " already open !");
        return;
    }

    //update currentA2lPath
    QSettings settings(qApp->organizationName(), qApp->applicationName());
    QString currentA2lPath = QFileInfo(fullFileName).absolutePath();
    settings.setValue("currentA2lPath", currentA2lPath);

    // create a new Wp
    WorkProject *wp = new WorkProject(fullFileName, this->model, this);

    // display status bar
    statusBar()->show();
    progBar->reset();
    connect(wp, SIGNAL(incProgressBar(int,int)), this, SLOT(setValueProgressBar(int,int)), Qt::DirectConnection);

    // parse the a2l file
    wp->parse();
    wp->attach(this);

    // hide the statusbar
    statusBar()->hide();
    progBar->reset();

    //update the ui->treeView
    model->addNode2RootNode(wp);
    wp->addA2lnode(wp->a2lFile->getProject(), wp);

    if (ui->treeView->model() != model)
        ui->treeView->setModel(model);
    ui->treeView->setColumnHidden(1, true);

    //set completer
    completer->setModel(model);

    //insert the new created project into the projectList
    projectList->insert(fullFileName, wp);

    //messagebox in case of a2l parser error
    if (!wp->isOk())
    {
        QMessageBox::warning(this, "HEXplorer" ,"ASAP file parser error");
    }

    //update output console
    ui->listWidget->addItems(wp->_outputList());
    ui->listWidget->scrollToBottom();

    //update current file
    setCurrentFile(fullFileName);
}

void MDImain::insertWp(WorkProject *wp)
{
    this->projectList->insert(QString(wp->getFullA2lFileName()), wp);
}

WorkProject* MDImain::getWp(QString path)
{
    if (projectList->contains(path))
    {
        return projectList->value(path);
    }
    else
        return NULL;
}

void MDImain::on_actionOpen_Working_Directory_triggered()
{
    //path
    QSettings settings(qApp->organizationName(), qApp->applicationName());
    QString path = settings.value("currentWDPath").toString();

    //QFileDialog to select multiple directories
    QFileDialog w;
    w.setFileMode(QFileDialog::Directory);
    w.setOption(QFileDialog::DontUseNativeDialog, true);
    w.setOption(QFileDialog::ShowDirsOnly, true);
    w.setDirectory(QDir(path));
    QListView *l = w.findChild<QListView*>("listView");
    if (l)
    {
        l->setSelectionMode(QAbstractItemView::MultiSelection);
    }
    QTreeView *t = w.findChild<QTreeView*>();
    if (t)
    {
        t->setSelectionMode(QAbstractItemView::MultiSelection);
    }
    QStringList pathList;
    if (w.exec())
        pathList = w.selectedFiles();



    if (pathList.isEmpty())
    {
       statusBar()->showMessage(tr("no Working Directory selected"), 2000);
       writeOutput("action open working directory : no folder selected");
       return;
    }
    else
    {
        //update currentWDPath
        int ret = QMessageBox::question(this, "HEXplorer :: Working Directory",
                              "Do you want HEXplorer to automatically open this directory at application launch?",
                              QMessageBox::Yes, QMessageBox::No);

        if (ret == QMessageBox::Yes)
        {
               settings.setValue("autoWD", 1);
               foreach (QString str, pathList)
                    openWorkingDirectory(str);

        }
        else if (ret == QMessageBox::No)
        {
            foreach (QString str, pathList)
                openWorkingDirectory(str);

        }
    }
}

WorkingDirectory* MDImain::openWorkingDirectory(QString rootPath)
{
    QStringList currentPathList = workingDirectory.split(";");
    QSettings settings(qApp->organizationName(), qApp->applicationName());
    if (!currentPathList.contains(rootPath))
    {
         workingDirectory.append(rootPath + ";");
         settings.setValue("currentWDPath", workingDirectory);
    }
    else
    {
        return 0;
    }

    //create a new WorkingDirectory instance
    WorkingDirectory* nodeWd = new WorkingDirectory(rootPath, model, this);

    //update the model and treeview
    //model->addNode2RootNode(nodeWd);

    //add model to treeview
    if (ui->treeView->model() != model)
        ui->treeView->setModel(model);

    completer->setModel(model);
    ui->treeView->setColumnHidden(1, true);

    return nodeWd;
}

void MDImain::on_actionClose_Working_Directory_triggered()
{
    int n = ui->treeView->selectionModel()->selectedIndexes().count();

    if (n == 1)
    {
        QModelIndex index = ui->treeView->selectionModel()->selectedIndexes().at(0);

        if (index.isValid())
        {
            //get a pointer on the selected item
            Node *node =  model->getNode(index);
            QString name = typeid(*node).name();

            if (!name.toLower().endsWith("workingdirectory"))
            {
                QMessageBox::warning(this, "HEXplorer::remove project", "Please select first a working directory.",
                                                 QMessageBox::Ok);
                return;
            }


            int r = QMessageBox::question(this, "HEXplorer::question", "Are you sure you want to remove the working directory " +
                                          QString(node->name)  + " ?",
                                  QMessageBox::Yes, QMessageBox::No);

            if (r ==  QMessageBox::Yes)
            {
                    removeWorkingDirectory(index);
            }
        }
    }
    else
    {
        int r = QMessageBox::question(this, "HEXplorer::question",
                                      "Remove all the selected working directories ?",
                                       QMessageBox::Yes, QMessageBox::No);

        if (r ==  QMessageBox::Yes)
        {
            removeWorkingDirectory(ui->treeView->selectionModel()->selectedIndexes());
        }
    }
}

void MDImain::addHexFile2Project()
{
    // check if a project is selected in treeView
    QModelIndex index  = ui->treeView->selectionModel()->currentIndex();

    if (index.isValid())
    {
        //get a pointer on the selected item
        Node *node =  model->getNode(index);
        QString name = typeid(*node).name();

        if (!name.endsWith("WorkProject") && !name.endsWith("DBFILE"))
        {
            QMessageBox::warning(this, "HEXplorer::add hex file to project", "Please select first a project.",
                                             QMessageBox::Ok);
            return;
        }

        int row = index.row();
        if ( row < 0)
        {
            QMessageBox::information(this,"HEXplorer","please first select a project");
            writeOutput("action open new dataset cancelled: no project first selected");
            return;
        }
        else
        {
            QSettings settings(qApp->organizationName(), qApp->applicationName());
            QString path = settings.value("currentHexPath").toString();

            QStringList files = QFileDialog::getOpenFileNames(this,
                                              tr("select a dataset (HEX)"), path,
                                              tr("HEX files (*.hex | *.hex_trimmed);;all files (*.*)"));


            if (files.isEmpty())
            {
               statusBar()->showMessage(tr("Loading canceled"), 2000);
               writeOutput("action open new dataset : no HEX file selected");
               return;
            }
            else
            {
                //Get the project (A2l) name
                //QString fullA2lName = model->name(index);

                //create a pointer on the WorkProject
                //WorkProject *wp = projectList->value(fullA2lName);
                WorkProject *wp = (WorkProject*)node;

                if (wp && name.endsWith("WorkProject"))
                {
                    //if the a2lFile is not yet parsed, parse.
                    if (!wp->a2lFile->isParsed())
                    {
                        readA2l(wp);
                    }

                    //check if A2l parsing was successfull
                    if (!wp->isOk())
                    {
                        QMessageBox::information(this,"HEXplorer","action open new dataset failed. A2Lfile is not parsed correctly.");
                        writeOutput("action open new dataset failed : A2Lfile could not be parsed correctly.");
                        return;
                    }

                    //if no MOD_COMMON in ASAP file
                    if (wp->a2lFile->getProject()->getNode("MODULE") == NULL)
                    {
                        QMessageBox::information(this, "HEXplorer", tr("no MOD_COMMON in ASAP file"));
                        writeOutput("action open new dataset : no MOD_COMMON in ASAP file");
                        return;
                    }

                    //check if Hexfile already in project
                    foreach (QString fullHexName, files)
                    {
                        //if the selected Hex file is already into the project => exit
                        if (wp->hexFiles().contains(fullHexName))
                        {
                            QMessageBox::information(this, "HEXplorer", "HEX file : " + fullHexName
                                                     + "\nalready included into the selected project");
                            writeOutput("action open new dataset : HEX file already in project");
                            files.removeOne(fullHexName);
                        }
                    }

                    //Open hex files
                    foreach (QString fullHexName, files)
                    {
                        //update currentHexPath
                        QSettings settings(qApp->organizationName(), qApp->applicationName());
                        QString currentHexPath = QFileInfo(fullHexName).absolutePath();
                        settings.setValue("currentHexPath", currentHexPath);

                        //start a timer
                        QElapsedTimer timer;
                        timer.start();

                        //add the file to the project (module)
                        HexFile *hex = NULL;
                        QList<MODULE*> list = wp->a2lFile->getProject()->listModule();
                        if (list.count() == 0)
                        {
                            writeOutput("action open new dataset : no Module into A2l file !");
                            return;
                        }
                        else if (list.count() == 1)
                        {
                            hex = new HexFile(fullHexName, wp, QString(list.at(0)->name));
                        }
                        else
                        {
                            // select a module
                            QString module;
                            DialogChooseModule *diag = new DialogChooseModule(&module);
                            QStringList listModuleName;
                            foreach (MODULE* module, list)
                            {
                                listModuleName.append(module->name);
                            }
                            diag->setList(listModuleName);
                            int ret = diag->exec();

                            if (ret == QDialog::Accepted)
                            {
                                hex = new HexFile(fullHexName, wp, module);
                            }
                            else
                            {
                                writeOutput("action open new dataset : no module chosen !");
                                return;
                            }
                        }

                        // display status bar
                        //statusBar()->show();
                        progBar->reset();
                        //connect(hex, SIGNAL(lineParsed(int,int)), this, SLOT(setValueProgressBar(int,int)), Qt::DirectConnection);
                        connect(hex, SIGNAL(progress(int,int)), this, SLOT(setValueProgressBar(int,int)), Qt::DirectConnection);

                        if (hex->read())
                        {
                            wp->addHex(hex, wp);
                        }
                        else
                            delete hex;

                        // hide the statusbar
                        statusBar()->hide();
                        progBar->reset();

                        //stop timer
                        qint64 elapsedTime = timer.restart();
                        double _elapsedTime = static_cast<double>(elapsedTime)/1000;

                        //update the treeView model
                        ui->treeView->expand(index);
                        ui->treeView->resizeColumnToContents(0);

                        writeOutput("action open new dataset : HEX file "  + fullHexName + " add to project in " + QString::number(_elapsedTime, 'g', 3) + " sec");

                    }
                }
                else if (wp && name.endsWith("DBFILE"))
                {
                    //TBD : perform a check of the DB to ensure contains everything to open Hexfile

                    // check if Hexfile already in project
                    foreach (QString fullHexName, files)
                    {
                        //if the selected Hex file is already into the project => exit
                        if (wp->hexFiles().contains(fullHexName))
                        {
                            QMessageBox::information(this, "HEXplorer", "HEX file : " + fullHexName
                                                     + "\nalready included into the selected project");
                            writeOutput("action open new dataset : HEX file already in project");
                            files.removeOne(fullHexName);
                        }
                    }

                    //Open hex files
                    foreach (QString fullHexName, files)
                    {
                        //update currentHexPath
                        QSettings settings(qApp->organizationName(), qApp->applicationName());
                        QString currentHexPath = QFileInfo(fullHexName).absolutePath();
                        settings.setValue("currentHexPath", currentHexPath);

                        //start a timer
                        QElapsedTimer timer;
                        timer.start();

                        //add the file to the project (module)
                        HexFile *hex = new HexFile(fullHexName, wp);

                        // display status bar
                        statusBar()->show();
                        progBar->reset();
                        connect(hex, SIGNAL(progress(int,int)), this, SLOT(setValueProgressBar(int,int)), Qt::DirectConnection);

                        //read hexFile and add node to parent Node
                        if (hex->read_db())
                            wp->addHex(hex, wp);
                        else
                            delete hex;

                        // hide the statusbar
                        statusBar()->hide();
                        progBar->reset();

                        //stop timer
                        qint64 elapsedTime = timer.restart();
                        double _elapsedTime = static_cast<double>(elapsedTime)/1000;

                        //update the treeView model
                        ui->treeView->expand(index);
                        ui->treeView->resizeColumnToContents(0);

                        writeOutput("action open new dataset into database : HEX file " + fullHexName + " add to project in " + QString::number(_elapsedTime, 'g', 3) + " sec");

                    }
                }
            }
        }
    }
}

void MDImain::addSrecFile2Project()
{
    // check if a project is selected in treeView
    QModelIndex index  = ui->treeView->selectionModel()->currentIndex();

    if (index.isValid())
    {
        //get a pointer on the selected item
        Node *node =  model->getNode(index);
        QString name = typeid(*node).name();

        if (!name.endsWith("WorkProject"))
        {
            QMessageBox::warning(this, "HEXplorer::add hex file to project", "Please select first a project.",
                                             QMessageBox::Ok);
            return;
        }

        int row = index.row();
        if ( row < 0)
        {
            QMessageBox::information(this,"HEXplorer","please first select a project");
            writeOutput("action open new dataset cancelled: no project first selected");
            return;
        }
        else
        {
            QSettings settings(qApp->organizationName(), qApp->applicationName());
            QString path = settings.value("currentSrecPath").toString();

            QStringList files =
                    QFileDialog::getOpenFileNames(this,
                                              tr("select a dataset (Srec)"), path,
                                              tr("Srec files (*.s19 | *.s32 | *.s37);;all files (*.*)"));


            if (files.isEmpty())
            {
               statusBar()->showMessage(tr("Loading canceled"), 2000);
               writeOutput("action open new dataset : no Srec file selected");
               return;
            }
            else
            {
                //Get the project (A2l) name
                //QString fullA2lName = model->name(index);

                //create a pointer on the WorkProject
                //WorkProject *wp = projectList->value(fullA2lName);
                WorkProject *wp = (WorkProject*)node;

                if (wp)  //to prevent any crash of the aplication
                {
                    //if the a2lFile is not yet parsed, parse.
                    if (!wp->a2lFile->isParsed())
                    {
                        readA2l(wp);
                    }

                    //check if A2l parsing was successfull
                    if (!wp->isOk())
                    {
                        QMessageBox::information(this,"HEXplorer","action open new dataset failed. A2Lfile is not parsed correctly.");
                        writeOutput("action open new dataset failed : A2Lfile could not be parsed correctly.");
                        return;
                    }

                    // if no MOD_COMMON in ASAP file
                    if (wp->a2lFile->getProject()->getNode("MODULE") == NULL)
                    {
                        QMessageBox::information(this, "HEXplorer", tr("no MOD_COMMON in ASAP file"));
                        writeOutput("action open new dataset : no MOD_COMMON in ASAP file");
                        return;
                    }

                    // check if Hexfile already in project
                    foreach (QString fullSrecName, files)
                    {
                        //if the selected Srec file is already into the project => exit
                        if (wp->hexFiles().contains(fullSrecName))
                        {
                            QMessageBox::information(this, "HEXplorer", "Srec file : " + fullSrecName
                                                     + "\nalready included into the selected project");
                            writeOutput("action open new dataset : Srec file already in project");
                            files.removeOne(fullSrecName);
                        }
                    }

                    //Open Srec files
                    foreach (QString fullSrecName, files)
                    {
                        //update currentSrecPath
                        QSettings settings(qApp->organizationName(), qApp->applicationName());
                        QString currentSrecPath = QFileInfo(fullSrecName).absolutePath();
                        settings.setValue("currentSrecPath", currentSrecPath);

                        //start a timer
                        QElapsedTimer timer;
                        timer.start();

                        //add the file to the project (module)
                        SrecFile *srec = NULL;
                        QList<MODULE*> list = wp->a2lFile->getProject()->listModule();
                        if (list.count() == 0)
                        {
                            writeOutput("action open new dataset : no Module into A2l file !");
                            return;
                        }
                        else if (list.count() == 1)
                        {
                            srec = new SrecFile(fullSrecName, wp, QString(list.at(0)->name));
                        }
                        else
                        {
                            // select a module
                            QString module;
                            DialogChooseModule *diag = new DialogChooseModule(&module);
                            QStringList listModuleName;
                            foreach (MODULE* module, list)
                            {
                                listModuleName.append(module->name);
                            }
                            diag->setList(listModuleName);
                            int ret = diag->exec();

                            if (ret == QDialog::Accepted)
                            {
                                srec = new SrecFile(fullSrecName, wp, module);
                            }
                            else
                            {
                                writeOutput("action open new dataset : no module chosen !");
                                return;
                            }
                        }

                        // display status bar
                        //statusBar()->show();
                        progBar->reset();
                        connect(srec, SIGNAL(progress(int,int)), this, SLOT(setValueProgressBar(int,int)), Qt::DirectConnection);

                        if (srec->read())
                        {
                            wp->addSrec(srec, wp);
                        }
                        else
                            delete srec;

                        // hide the statusbar
                        statusBar()->hide();
                        progBar->reset();

                        //stop timer
                        qint64 elapsedTime = timer.restart();
                        double _elapsedTime = static_cast<double>(elapsedTime)/1000;

                        //update the treeView model
                        ui->treeView->expand(index);
                        ui->treeView->resizeColumnToContents(0);                        

                        writeOutput("action open new dataset : Srec file "  + fullSrecName +" add to project in " + QString::number(_elapsedTime, 'g', 3) + " sec");

                    }
                }
            }
        }
    }
}

void MDImain::addCsvFile2Project()
{
    // check if a project is selected in treeView
    QModelIndex index  = ui->treeView->selectionModel()->currentIndex();

    if (index.isValid())
    {
        //get a pointer on the selected item
        Node *node =  model->getNode(index);
        QString name = typeid(*node).name();

        if (!name.endsWith("WorkProject"))
        {
            QMessageBox::warning(this, "HEXplorer::add csv file to project", "Please select first a project.",
                                             QMessageBox::Ok);
            return;
        }

        int row = index.row();
        if ( row < 0)
        {
            QMessageBox::information(this,"HEXplorer","please first select a project");
            writeOutput("action open new csv file cancelled: no project first selected");
            return;
        }
        else
        {
            QSettings settings(qApp->organizationName(), qApp->applicationName());
            QString path = settings.value("currentCsvPath").toString();

            QStringList files = QFileDialog::getOpenFileNames(this,
                                              tr("select CSV files"), path,
                                              tr("CSV files (*.csv);;all files (*.*)"));

            if (files.isEmpty())
            {
               writeOutput("action open new CSV file : no CSV file selected");
               return;
            }
            else
            {
                //Get the project (A2l) name
                //QString fullA2lName = model->name(index);

                //create a pointer on the WorkProject
                //WorkProject *wp = projectList->value(fullA2lName);
                WorkProject *wp = (WorkProject*)node;

                if (wp)  //to prevent any crash of the aplication
                {
                    //if the a2lFile is not yet parsed, parse.
                    if (!wp->a2lFile->isParsed())
                    {
                        readA2l(wp);
                    }

                    //check if A2l parsing was successfull
                    if (!wp->isOk())
                    {
                        QMessageBox::information(this,"HEXplorer","action open new Csv failed. A2Lfile is not parsed correctly.");
                        writeOutput("action open new Csv failed : A2Lfile could not be parsed correctly.");
                        return;
                    }

                    // if no MOD_COMMON in ASAP file
                    if (wp->a2lFile->getProject()->getNode("MODULE") == NULL)
                    {
                        QMessageBox::information(this, "HEXplorer", tr("no MOD_COMMON in ASAP file"));
                        writeOutput("action open new CSV file : no MOD_COMMON in ASAP file");
                        return;
                    }

                    // check if Csv already in project
                    foreach (QString fullCsvName, files)
                    {
                        //if the selected Hex file is already into the project => exit
                        if (wp->csvFiles().contains(fullCsvName))
                        {
                            QMessageBox::information(this, "HEXplorer", tr("file already in project"));
                            writeOutput("action open new CSV file : CSV file already in project");
                            files.removeOne(fullCsvName);
                            return;
                        }
                    }

                    // Read CSV files
                    QList<Csv*> list;
                    foreach (QString fullCsvName, files)
                    {
                        //update currentHexPath
                        QSettings settings(qApp->organizationName(), qApp->applicationName());
                        QString currentCsvPath = QFileInfo(fullCsvName).absolutePath();
                        settings.setValue("currentCsvPath", currentCsvPath);

                        //start a timer
                        QElapsedTimer timer;
                        timer.start();

                        //add the file to the project
                        QString moduleName;
                        QList<MODULE*> listModule = wp->a2lFile->getProject()->listModule();
                        if (listModule.count() == 0)
                        {
                            writeOutput("action open new CSV : no Module into A2l file !");
                            return;
                        }
                        else if (listModule.count() == 1)
                        {
                            moduleName = listModule.at(0)->name;
                        }
                        else
                        {
                            // select a module
                            DialogChooseModule *diag = new DialogChooseModule(&moduleName);
                            QStringList listModuleName;
                            foreach (MODULE* module, listModule)
                            {
                                listModuleName.append(module->name);
                            }
                            diag->setList(listModuleName);
                            int ret = diag->exec();

                            if (ret == QDialog::Accepted)
                            {
                                writeOutput("action open new CSV : module " + moduleName + " selected.");
                            }
                            else
                            {
                                writeOutput("action open new CSV : no module chosen !");
                                return;
                            }
                        }

                        Csv *csv = new Csv(fullCsvName, wp, moduleName);

                        // display status bar
                        statusBar()->show();
                        progBar->reset();
                        connect(csv, SIGNAL(incProgressBar(int,int)), this, SLOT(setValueProgressBar(int,int)), Qt::DirectConnection);

                        if (csv->readFile())
                        {
                            //add csv to the workProject
                            wp->addCsv(csv);
                            list.append(csv);

                            //stop timer
                            qint64 elapsedTime = timer.restart();
                            double _elapsedTime = static_cast<double>(elapsedTime)/1000;

                            //update the treeView model
                            ui->treeView->expand(index);
                            ui->treeView->resizeColumnToContents(0);

                            writeOutput("CSV file " + fullCsvName + " successfully added to the project " + QString::number(_elapsedTime, 'g',3) + " sec");

                        }
                        else
                        {
                            //remove csv from the workProject
                             wp->removeCsv(csv);
                             foreach (QString str, *csv->errorList)
                             {
                                writeOutput(str);
                             }
                             writeOutput("CSV file " + fullCsvName + "  NOT added to project due to wrong format ");
                        }

                        // hide the statusbar
                        statusBar()->hide();
                        progBar->reset();

                    }
                }
            }
        }
    }
}

void MDImain::addDcmFile2Project()
{
    // check if a project is selected in treeView
    QModelIndex index  = ui->treeView->selectionModel()->currentIndex();

    if (index.isValid())
    {
        //get a pointer on the selected item
        Node *node =  model->getNode(index);
        QString name = typeid(*node).name();

        if (!name.endsWith("WorkProject"))
        {
            QMessageBox::warning(this, "HEXplorer::add Dcm file to project", "Please select first a project.",
                                             QMessageBox::Ok);
            return;
        }

        int row = index.row();
        if ( row < 0)
        {
            QMessageBox::information(this,"HEXplorer","please first select a project");
            writeOutput("action open new Dcm file cancelled: no project first selected");
            return;
        }
        else
        {
            QSettings settings(qApp->organizationName(), qApp->applicationName());
            QString path = settings.value("currentDcmPath").toString();

            QStringList files = QFileDialog::getOpenFileNames(this,
                                              tr("select DCM files"), path,
                                              tr("DCM files (*.dcm);;all files (*.*)"));

            if (files.isEmpty())
            {
               writeOutput("action open new DCM file : no DCM file selected");
               return;
            }
            else
            {
                //Get the project (A2l) name
                WorkProject *wp = (WorkProject*)node;

                if (wp)  //to prevent any crash of the aplication
                {
                    //if the a2lFile is not yet parsed, parse.
                    if (!wp->a2lFile->isParsed())
                    {
                        readA2l(wp);
                    }

                    //check if A2l parsing was successfull
                    if (!wp->isOk())
                    {
                        QMessageBox::information(this,"HEXplorer","action open new DCM failed. A2Lfile is not parsed correctly.");
                        writeOutput("action open new DCM failed : A2Lfile could not be parsed correctly.");
                        return;
                    }

                    // if no MOD_COMMON in ASAP file
                    if (wp->a2lFile->getProject()->getNode("MODULE") == NULL)
                    {
                        QMessageBox::information(this, "HEXplorer", tr("no MOD_COMMON in ASAP file"));
                        writeOutput("action open new DCM file : no MOD_COMMON in ASAP file");
                        return;
                    }

                    // check if Csv already in project
                    foreach (QString fullDcmName, files)
                    {
                        //if the selected Hex file is already into the project => exit
                        if (wp->dcmFiles().contains(fullDcmName))
                        {
                            QMessageBox::information(this, "HEXplorer", tr("file already in project"));
                            writeOutput("action open new Dcm file : Dcm file already in project");
                            files.removeOne(fullDcmName);
                            return;
                        }
                    }

                    // Read CSV files
                    QList<Dcm*> list;
                    foreach (QString fullDcmName, files)
                    {
                        //update currentHexPath
                        QSettings settings(qApp->organizationName(), qApp->applicationName());
                        QString currentCsvPath = QFileInfo(fullDcmName).absolutePath();
                        settings.setValue("currentDcmPath", currentCsvPath);

                        //start a timer
                        QElapsedTimer timer;
                        timer.start();

                        //add the file to the project
                        QString moduleName;
                        QList<MODULE*> listModule = wp->a2lFile->getProject()->listModule();
                        if (listModule.count() == 0)
                        {
                            writeOutput("action open new Dcm : no Module into A2l file !");
                            return;
                        }
                        else if (listModule.count() == 1)
                        {
                            moduleName = listModule.at(0)->name;
                        }
                        else
                        {
                            // select a module
                            DialogChooseModule *diag = new DialogChooseModule(&moduleName);
                            QStringList listModuleName;
                            foreach (MODULE* module, listModule)
                            {
                                listModuleName.append(module->name);
                            }
                            diag->setList(listModuleName);
                            int ret = diag->exec();

                            if (ret == QDialog::Accepted)
                            {
                                writeOutput("action open new Dcm : module " + moduleName + " selected.");
                            }
                            else
                            {
                                writeOutput("action open new Dcm : no module chosen !");
                                return;
                            }
                        }

                        Dcm *dcm = new Dcm(fullDcmName, wp, moduleName);

                        // display status bar
                        statusBar()->show();
                        progBar->reset();
                        connect(dcm, SIGNAL(incProgressBar(int,int)), this, SLOT(setValueProgressBar(int,int)), Qt::DirectConnection);

                        if (dcm->readFile())
                        {
                            //add csv to the workProject
                            wp->addDcm(dcm);
                            list.append(dcm);

                            //stop timer
                            qint64 elapsedTime = timer.restart();
                            double _elapsedTime = static_cast<double>(elapsedTime)/1000;

                            //update the treeView model
                            ui->treeView->expand(index);
                            ui->treeView->resizeColumnToContents(0);

                            writeOutput("Dcm file " + fullDcmName + " successfully added to the project " + QString::number(_elapsedTime, 'g',3) + " sec");

                        }
                        else
                        {
                            //remove csv from the workProject
                             wp->removeDcm(dcm);
                             writeOutput("Dcm file " + fullDcmName + "  NOT added to project due to wrong format ");
                        }

                        // hide the statusbar
                        statusBar()->hide();
                        progBar->reset();

                    }
                }
            }
        }
    }
}

void MDImain::addCdfxFile2Project()
{
    // check if a project is selected in treeView
    QModelIndex index  = ui->treeView->selectionModel()->currentIndex();

    if (index.isValid())
    {
        //get a pointer on the selected item
        Node *node =  model->getNode(index);
        QString name = typeid(*node).name();

        if (!name.endsWith("WorkProject"))
        {
            QMessageBox::warning(this, "HEXplorer::add cdfx file to project", "Please select first a project.",
                                             QMessageBox::Ok);
            return;
        }

        int row = index.row();
        if ( row < 0)
        {
            QMessageBox::information(this,"HEXplorer","please first select a project");
            writeOutput("action open new cdfx file cancelled: no project first selected");
            return;
        }
        else
        {
            QSettings settings(qApp->organizationName(), qApp->applicationName());
            QString path = settings.value("currentCdfxPath").toString();

            QStringList files = QFileDialog::getOpenFileNames(this,
                                              tr("select CDFX files"), path,
                                              tr("CDFX files (*.cdfx);;all files (*.*)"));

            if (files.isEmpty())
            {
               writeOutput("action open new CDFX file : no CDFX file selected");
               return;
            }
            else
            {
                //Get the project (A2l) name
//                QString fullA2lName = model->name(index);

                //create a pointer on the WorkProject
//                WorkProject *wp = projectList->value(fullA2lName);
                WorkProject* wp = (WorkProject*)node;

                if (wp)  //to prevent any chrash of the aplication
                {
                    //if the a2lFile is not yet parsed, parse.
                    if (!wp->a2lFile->isParsed())
                    {
                        readA2l(wp);
                    }

                    //check if A2l parsing was successfull
                    if (!wp->isOk())
                    {
                        QMessageBox::information(this,"HEXplorer","action open new dataset failed. A2Lfile is not parsed correctly.");
                        writeOutput("action open new dataset failed : A2Lfile could not be parsed correctly.");
                        return;
                    }

                    // if no MOD_COMMON in ASAP file
                    if (wp->a2lFile->getProject()->getNode("MODULE") == NULL)
                    {
                        QMessageBox::information(this, "HEXplorer", tr("no MOD_COMMON in ASAP file"));
                        writeOutput("action open new CDFX file : no MOD_COMMON in ASAP file");
                        return;
                    }

                    // check if Cdfx already in project
                    foreach (QString fullCdfxName, files)
                    {
                        //if the selected Hex file is already into the project => exit
                        if (wp->cdfxFiles().contains(fullCdfxName))
                        {
                            QMessageBox::information(this, "HEXplorer", tr("file already in project"));
                            writeOutput("action open new CDFX file : CDFX file already in project");
                            files.removeOne(fullCdfxName);
                            return;
                        }
                    }

                    // Read CDFX files
                    QList<CdfxFile*> list;
                    foreach (QString fullCdfxName, files)
                    {
                        //update currentHexPath
                        QSettings settings(qApp->organizationName(), qApp->applicationName());
                        QString currentCdfxPath = QFileInfo(fullCdfxName).absolutePath();
                        settings.setValue("currentCdfxPath", currentCdfxPath);

                        //start a timer
                        QElapsedTimer timer;
                        timer.start();

                        //add the file to the project
                        QString moduleName;
                        QList<MODULE*> listModule = wp->a2lFile->getProject()->listModule();
                        if (listModule.count() == 0)
                        {
                            writeOutput("action open new dataset : no Module into A2l file !");
                            return;
                        }
                        else if (listModule.count() == 1)
                        {
                            moduleName = listModule.at(0)->name;
                        }
                        else
                        {
                            // select a module
                            DialogChooseModule *diag = new DialogChooseModule(&moduleName);
                            QStringList listModuleName;
                            foreach (MODULE* module, listModule)
                            {
                                listModuleName.append(module->name);
                            }
                            diag->setList(listModuleName);
                            int ret = diag->exec();

                            if (ret == QDialog::Accepted)
                            {
                                writeOutput("action open new Cdfx : module " + moduleName + " selected.");
                            }
                            else
                            {
                                writeOutput("action open new Cdfx : no module chosen !");
                                return;
                            }
                        }

                        // display status bar
                        statusBar()->show();
                        progBar->reset();

                        CdfxFile *cdfx = new CdfxFile(fullCdfxName, wp, moduleName);
                        connect(cdfx, SIGNAL(incProgressBar(int,int)), this, SLOT(setValueProgressBar(int,int)), Qt::DirectConnection);

                        if (cdfx->readFile())
                        {
                            //add csv to the workProject
                            wp->addCdfx(cdfx);
                            list.append(cdfx);

                            //stop timer
                            qint64 elapsedTime = timer.restart();
                            double _elapsedTime = static_cast<double>(elapsedTime)/1000;

                            //update the treeView model
                            ui->treeView->expand(index);
                            ui->treeView->resizeColumnToContents(0);

                            writeOutput(cdfx->getInfoList());
                            writeOutput("CDFX file " + fullCdfxName + " successfully added to the project " + QString::number(_elapsedTime, 'g', 3) + " sec");

                        }
                        else
                        {
                             // display error
                             writeOutput(cdfx->getErrorList());
                             writeOutput("CDFX file " + fullCdfxName + "  NOT added to project due to wrong format ");

                             //remove cdfx from the workProject
                             wp->removeCdfxFile(cdfx);
                        }

                        // hide the statusbar
                        statusBar()->hide();
                        progBar->reset();

                    }
                }
            }
        }
    }
}

void MDImain::deleteFilesFromProject()
{
    //get a pointer on the selected items
    QList<Node*> listNodes;
    foreach (QModelIndex index, ui->treeView->selectionModel()->selectedIndexes())
    {
        Node *node =  model->getNode(index);
        listNodes.append(node);
    }

    QStringList list;
    foreach (QModelIndex index, ui->treeView->selectionModel()->selectedIndexes())
    {
        list.append( model->getNode(index)->fullName());
    }

    DeleteFileDialog *msg = new DeleteFileDialog();
    msg->setFileNames(list);
    int ret = msg->exec();

    switch (ret)
    {
          case 1:
              // Save was clicked
            foreach (Node* node, listNodes)
            {
                QModelIndex index = model->getIndex(node);
                deleteFileFromProject(index, msg->deletePermanently());
            }
              break;
          case 0:
              // Cancel was clicked
              break;
          default:
              // should never be reached
              break;
    }
}

void MDImain::deleteFileFromProject(QModelIndex index, bool bl)
{
    if (index.isValid())
    {
        //get a pointer on the selected item
        Node *node =  model->getNode(index);
        QModelIndex indexParent = index.parent();
        QString name = typeid(*node).name();

        if (name.endsWith("HexFile"))
        {
            //ask for save changes
            HexFile *hex = dynamic_cast<HexFile *> (node);
            QString hexName = hex->fullName();
            if (hex->childNodes.count() != 0)
            {
                // ask for saving changes
                int r = QMessageBox::question(this, "HEXplorer::question", tr("Save changes ?"),
                                      QMessageBox::Yes, QMessageBox::No);

                if (r ==  QMessageBox::Yes)
                {
                    if (!save_HexFile(index))
                    {
                        return;
                    }
                }
            }

            //update treeview
            Node* nodeParent = node->getParentNode();
            model->dataRemoved(nodeParent, index.row(), 1);

            //get the parentNode of the SrecFile
            WorkProject *wp = hex->getParentWp();
            wp->removeHexFile(hex);

            //update the treeView
            ui->treeView->resizeColumnToContents(0);

            //remove from HDD permanently
            if (bl)
            {
                QFile::remove(hexName);
            }

            //hide toolbar hex
            ui->toolBar_data->hide();
        }
        else if (name.endsWith("SrecFile"))
        {
            //ask for save changes
            SrecFile *srec = dynamic_cast<SrecFile *> (node);
            QString srecName = srec->fullName();
            if (srec->childNodes.count() != 0)
            {
                // ask for saving changes
                int r = QMessageBox::question(this, "HEXplorer::question", tr("Save changes ?"),
                                      QMessageBox::Yes, QMessageBox::No);

                if (r ==  QMessageBox::Yes)
                {
                    if (!save_SrecFile(index))
                    {
                        return;
                    }
                }

            }

            //update treeview
            Node* nodeParent = node->getParentNode();
            model->dataRemoved(nodeParent, index.row(), 1);

            //get the parentNode of the SrecFile
            WorkProject *wp = srec->getParentWp();
            wp->removeSrecFile(srec);

            //update the treeView
            ui->treeView->resizeColumnToContents(0);

            //delete file from disk
            if (bl)
            {
                QFile::remove(srecName);
            }

            //hide toolbar hex
            ui->toolBar_data->hide();


        }
        else if (name.endsWith("Csv"))
        {
            //ask for save changes
            Csv *csv = dynamic_cast<Csv *> (node);
            QString csvName = csv->fullName();
            if (!csv->getModifiedData().isEmpty())
            {
                // ask for saving changes
                int r = QMessageBox::question(this, "HEXplorer::question", tr("Save changes ?"),
                                      QMessageBox::Yes, QMessageBox::No);

                if (r ==  QMessageBox::Yes)
                {
                    if (!save_CsvFile(index))
                    {
                        return;
                    }
                }

            }

            //get the parentNode of the HexFile (which must be an A2LFILE !!)
            WorkProject *wp = csv->getParentWp();
            model->dataRemoved(wp, index.row(), 1);
            wp->removeCsv(csv);

            //update the treeView
            ui->treeView->expand(indexParent);
            ui->treeView->resizeColumnToContents(0);

            //delete file from disk
            if (bl)
            {
                QFile::remove(csvName);
            }

            //hide toolbar hex
            ui->toolBar_data->hide();
        }
        else if (name.endsWith("CdfxFile"))
        {
            //ask for save changes
            CdfxFile *cdfx = dynamic_cast<CdfxFile *> (node);
            QString cdfxName = cdfx->fullName();
            if (!cdfx->getModifiedData().isEmpty())
            {
                // ask for saving changes
                int r = QMessageBox::question(this, "HEXplorer::question", tr("Save changes ?"),
                                      QMessageBox::Yes, QMessageBox::No);

                if (r ==  QMessageBox::Yes)
                {
                    if (!save_CdfxFile(index))
                    {
                        return;
                    }
                }

            }

            //get the parentNode of the HexFile (which must be an A2LFILE !!)
            WorkProject *wp = cdfx->getParentWp();
            model->dataRemoved(wp, index.row(), 1);
            wp->removeCdfxFile(cdfx);

            //update the treeView
            ui->treeView->expand(indexParent);
            ui->treeView->resizeColumnToContents(0);


            //delete file from disk
            if (bl)
            {
                QFile::remove(cdfxName);
            }

            //hide toolbar hex
            ui->toolBar_data->hide();
        }
        else if (name.endsWith("Dcm"))
        {
            //ask for save changes
            Dcm *dcm = dynamic_cast<Dcm *> (node);
            QString dcmName = dcm->fullName();
            if (!dcm->getModifiedData().isEmpty())
            {
                // ask for saving changes
                int r = QMessageBox::question(this, "HEXplorer::question", tr("Save changes ?"),
                                      QMessageBox::Yes, QMessageBox::No);

                if (r ==  QMessageBox::Yes)
                {
                    if (!save_DcmFile(index))
                    {
                        return;
                    }
                }

            }

            //get the parentNode of the HexFile (which must be an A2LFILE !!)
            WorkProject *wp = dcm->getParentWp();
            model->dataRemoved(wp, index.row(), 1);
            wp->removeDcm(dcm);

            //update the treeView
            ui->treeView->expand(indexParent);
            ui->treeView->resizeColumnToContents(0);


            //delete file from disk
            if (bl)
            {
                QFile::remove(dcmName);
            }

            //hide toolbar hex
            ui->toolBar_data->hide();
        }
        else
        {
            QMessageBox::warning(this, "HEXplorer::remove file from project", "Please select first an hex, srec, csv, cdfx or dcm file.",
                                             QMessageBox::Ok);
            return;
        }
    }
}

void MDImain::reAppendProject(WorkProject *wp)
{
    wp->attach(this);

    //insert the new created project into the projectList
    projectList->insert(wp->getFullA2lFileName(), wp);

    //update model
    Node* parentNode = wp->getParentNode();
    parentNode->addChildNode(wp);
    parentNode->sortChildrensName();
//    wp->setParentNode(parentNode);
    model->dataInserted(parentNode, parentNode->childNodes.indexOf(wp));


    //update treeView
    QModelIndex parentIndex = model->getIndex(parentNode);
    ui->treeView->expand(parentIndex);
    QModelIndex index = model->getIndex(wp);
    ui->treeView->expand(index);
    ui->treeView->setColumnHidden(1, true);
}

void MDImain::showFixPar()
{
    //search the index of the current node in the Model
    QModelIndex index  = ui->treeView->selectionModel()->currentIndex();

    //create a dialog window
    QMap<QString, QString> *param = model->getPar(index);

    if (param)
    {
        Dialog *diag = new Dialog();

        QMap<QString, QString>::const_iterator i = param->constBegin();
        while (i != param->constEnd())
        {
                diag->addItem(QString((i.key())) + " : " + QString((i.value())));
                ++i;
        }

        diag->setWindowTitle(QString((param->value("Name"))));

        delete param;

        diag->show();
    }
}

void MDImain::countChild()
{
    //get the index of the selected hex file
    QModelIndex index  = ui->treeView->selectionModel()->currentIndex();

    QString str;
    str.setNum(model->rowCount(index));

    ui->listWidget->addItem(str + " nodes");
}

void MDImain::editTextFile()
{
    // check if a file is selected in treeWidget
    QModelIndex index  = ui->treeView->selectionModel()->currentIndex();

    if (index.isValid())
    {
        //get a pointer on the selected item
        Node *node =  model->getNode(index);
        QString name = typeid(*node).name();

        int row = index.row();
        if ( row < 0)
        {
            QMessageBox::information(this,"HEXplorer","please first select a file to be edited");
            writeOutput("action edit  file cancelled: no project first selected");
            return;
        }
        else
        {
            if (name.endsWith("HexFile") || name.endsWith("Csv") || name.endsWith("SrecFile") ||
                name.endsWith("CdfxFile") || name.endsWith("A2LFILE") || name.endsWith("WorkProject") ||
                name.endsWith("Dcm"))
            {
                //Get the selected file name
                QString fullFileName = model->name(index);
                QFile file(fullFileName);

                //update output
                QsciLexer *lexer = 0;
                if (name.endsWith("CdfxFile"))
                {
                    lexer = new QsciLexerXML();
                }

                if (editTextFile(file, lexer))
                    writeOutput("action edit File : file " + fullFileName + " edit with success.");
                else
                    writeOutput("action edit File : file " + fullFileName + " cannot be edited.");
            }
            else
            {
                QMessageBox::warning(this, "HEXplorer::edit file", "Please select first an hex, csv or Cdfx file.",
                                                 QMessageBox::Ok);
                return;
            }
        }
    }
}

bool MDImain::editTextFile(QFile &file, QsciLexer *lexer)
{
       //open and read the file
       if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
       {
          QMessageBox::warning(this, tr("HEXplorer"),
                                tr("Cannot read file %1 :\n%2.")
                                .arg(file.fileName())
                                .arg(file.errorString()));

          return false;
       }

       //get the name of the file
       QString fileName = file.fileName();

       //intialize tabList if necessary
       if (tabList == NULL)
       {
            tabList = new QMap<QString, QWidget*>;
        }

       //check if the file is already open in a Tab
       if (tabList->contains(fileName))
           QMessageBox::warning(this, tr("information"), tr("file already open"));
       else
       {
           //create the textEditor (Qscintilla)
           ChTextEdit *textEdit = new ChTextEdit;
           textEdit->loadFile(fileName);

           //set the lexer if necessary
           if (lexer)
           {
               textEdit->setLexer(lexer);
           }

           //add the Qscintilla editor to listTab
           tabList->insert(fileName, textEdit);

           //draw the tab in tabWidget
           QIcon icon;
           if (fileName.endsWith(".hex") || fileName.endsWith(".HEX"))
               icon.addFile(":/icones/ram.png");
           else if (fileName.endsWith(".a2l") || fileName.endsWith(".A2L"))
               icon.addFile(":/icones/milky_cartable.png");
           else if (fileName.endsWith(".js"))
               icon.addFile("");

           ui->tabWidget->addTab(tabList->value(fileName), icon, fileName);

           //set new FormCompare as activated
           ui->tabWidget->setCurrentWidget(tabList->value(fileName));
        }

       return true;
   }

bool MDImain::editTextFile(QString &text)
{

       //create the textEditor (Qscintilla)
       ChTextEdit *textEdit = new ChTextEdit;
       QFont font;
       font.setFixedPitch(true);
       textEdit->setFont(font);
       textEdit->setText(text);

       //add the Qscintilla editor to listTab
       tabList->insert("hex", textEdit);

       //draw the tab in tabWidget
       ui->tabWidget->addTab(tabList->value("hex"), "hex");

       return true;
}

void MDImain::removeWorkProjects()
{
    int n = ui->treeView->selectionModel()->selectedIndexes().count();

    if (n == 1)
    {
        QModelIndex index = ui->treeView->selectionModel()->selectedIndexes().at(0);

        if (index.isValid())
        {
            //get a pointer on the selected item
            Node *node =  model->getNode(index);
            QString name = typeid(*node).name();

            if (!name.endsWith("WorkProject") && !name.endsWith("DBFILE"))
            {
                QMessageBox::warning(this, "HEXplorer::remove project", "Please select first a project.",
                                                 QMessageBox::Ok);
                return;
            }


            int r = QMessageBox::question(this, "HEXplorer::question", "Are you sure you want to delete the project " +
                                          QString(node->name)  + " ?",
                                  QMessageBox::Yes, QMessageBox::No);

            if (r ==  QMessageBox::Yes)
            {
                    removeWorkProject(index);
            }
        }
    }
    else
    {
        int r = QMessageBox::question(this, "HEXplorer::question",
                                      "Remove all the selected projects ?",
                                       QMessageBox::Yes, QMessageBox::No);

        if (r ==  QMessageBox::Yes)
        {
            removeWorkProject(ui->treeView->selectionModel()->selectedIndexes());
        }
    }
}

void MDImain::removeWorkProject(QModelIndex index)
{
    if (index.isValid())
    {
        //get a pointer on the selected item
        Node *node =  model->getNode(index);
        QString name = typeid(*node).name();

        //ensure a correct file is selected
        if (!name.endsWith("WorkProject") && !name.endsWith("DBFILE"))
        {
            QMessageBox::warning(this, "HEXplorer::remove project", "Please select first a project.",
                                             QMessageBox::Ok);
            return;
        }

        //remove file
        if (name.endsWith("WorkProject"))
        {
            //As the selected node is an A2l file we can cast the node into its real type : A2LFILE
            WorkProject *wp = dynamic_cast<WorkProject *> (node);

            //ask to save the modified nodes (hex or Csv)
            foreach (Node *node, wp->childNodes)
            {
                QString name = typeid(*node).name();
                if (name.toLower().endsWith("hexfile"))
                {
                    HexFile *hex = (HexFile*)node;
                    if (!hex->getModifiedData().isEmpty())
                    {
                          int r = QMessageBox::question(this, "HEXplorer::question",
                                                        "Save changes in " + QString(hex->name) + "?",
                                                        QMessageBox::Yes, QMessageBox::No);
                          if (r == QMessageBox::Yes)
                          {
                              QModelIndex hexIndex = model->getIndex(hex);
                              save_HexFile(hexIndex);
                          }
                    }
                }
                else if (name.toLower().endsWith("srecfile"))
                {
                    SrecFile *srec = (SrecFile*)node;
                    if (!srec->getModifiedData().isEmpty())
                    {
                        int r = QMessageBox::question(this, "HEXplorer::question",
                                                      "Save changes in " + QString(srec->name) + "?",
                                                      QMessageBox::Yes, QMessageBox::No);
                        if (r == QMessageBox::Yes)
                        {
                          QModelIndex srecIndex = model->getIndex(srec);
                          save_SrecFile(srecIndex);
                        }
                    }
                }
                else if (name.toLower().endsWith("csv"))
                {
                    Csv *csv = (Csv*)node;
                    if (!csv->getModifiedData().isEmpty())
                    {
                        int r = QMessageBox::question(this, "HEXplorer::question",
                                                      "Save changes in " + QString(csv->name) + "?",
                                                      QMessageBox::Yes, QMessageBox::No);
                        if (r == QMessageBox::Yes)
                        {
                          QModelIndex csvIndex = model->getIndex(csv);
                          save_CsvFile(csvIndex);
                        }
                    }
                }
                else if (name.toLower().endsWith("cdfxfile"))
                {
                    CdfxFile *cdfx = (CdfxFile*)node;
                    if (!cdfx->getModifiedData().isEmpty())
                    {
                        int r = QMessageBox::question(this, "HEXplorer::question",
                                                      "Save changes in " + QString(cdfx->name) + "?",
                                                      QMessageBox::Yes, QMessageBox::No);
                        if (r == QMessageBox::Yes)
                        {
                          QModelIndex cdfxIndex = model->getIndex(cdfx);
                          save_CdfxFile(cdfxIndex);
                        }
                    }
                }
            }

            //remove the node from treeView model
            Node* nodeParent = node->getParentNode();
            model->dataRemoved(nodeParent, index.row(), 1);

            //update the treeView
            ui->treeView->resizeColumnToContents(0);

            //remove the project from the this->projectList
            projectList->remove(wp->fullName());

            //delete the selected project
            wp->detach(this);
        }
        else if (name.endsWith("DBFILE"))
        {
            //As the selected node is an A2l file we can cast the node into its real type : A2LFILE
            DBFILE *dbfile = dynamic_cast<DBFILE *> (node);

            //ask to save the modified nodes (hex or Csv)
            foreach (Node *node, dbfile->childNodes)
            {
                QString name = typeid(*node).name();
                if (name.toLower().endsWith("hexfile"))
                {
                    HexFile *hex = (HexFile*)node;
                    if (!hex->getModifiedData().isEmpty())
                    {
                          int r = QMessageBox::question(this, "HEXplorer::question",
                                                        "Save changes in " + QString(hex->name) + "?",
                                                        QMessageBox::Yes, QMessageBox::No);
                          if (r == QMessageBox::Yes)
                          {
                              QModelIndex hexIndex = model->getIndex(hex);
                              save_HexFile(hexIndex);
                          }
                    }
                }
                else if (name.toLower().endsWith("srecfile"))
                {
                    SrecFile *srec = (SrecFile*)node;
                    if (!srec->getModifiedData().isEmpty())
                    {
                        int r = QMessageBox::question(this, "HEXplorer::question",
                                                      "Save changes in " + QString(srec->name) + "?",
                                                      QMessageBox::Yes, QMessageBox::No);
                        if (r == QMessageBox::Yes)
                        {
                          QModelIndex srecIndex = model->getIndex(srec);
                          save_SrecFile(srecIndex);
                        }
                    }
                }
                else if (name.toLower().endsWith("csv"))
                {
                    Csv *csv = (Csv*)node;
                    if (!csv->getModifiedData().isEmpty())
                    {
                        int r = QMessageBox::question(this, "HEXplorer::question",
                                                      "Save changes in " + QString(csv->name) + "?",
                                                      QMessageBox::Yes, QMessageBox::No);
                        if (r == QMessageBox::Yes)
                        {
                          QModelIndex csvIndex = model->getIndex(csv);
                          save_CsvFile(csvIndex);
                        }
                    }
                }
                else if (name.toLower().endsWith("cdfxfile"))
                {
                    CdfxFile *cdfx = (CdfxFile*)node;
                    if (!cdfx->getModifiedData().isEmpty())
                    {
                        int r = QMessageBox::question(this, "HEXplorer::question",
                                                      "Save changes in " + QString(cdfx->name) + "?",
                                                      QMessageBox::Yes, QMessageBox::No);
                        if (r == QMessageBox::Yes)
                        {
                          QModelIndex cdfxIndex = model->getIndex(cdfx);
                          save_CdfxFile(cdfxIndex);
                        }
                    }
                }
            }

            //remove the node from treeView model
            Node* nodeParent = node->getParentNode();
            model->dataRemoved(nodeParent, index.row(), 1);

            //update the treeView
            //model->update();
            ui->treeView->resizeColumnToContents(0);

            //get the project
            WorkProject *wp = projectList->value(dbfile->fullName());

            //remove the project from the this->projectList
            projectList->remove(dbfile->fullName());

            //delete the selected project
            //delete wp;
            wp->detach(this);
        }
    }
}

void MDImain::removeWorkProject(QModelIndexList indexList)
{
    //get a pointer on the selected items and delete all its childrens
    QList<WorkProject*> listNodes;
    foreach (QModelIndex index, indexList)
    {
        Node *node =  model->getNode(index);
        QString name = typeid(*node).name();

        //ensure a correct file is selected
        if (!name.endsWith("WorkProject"))
        {
            QMessageBox::warning(this, "HEXplorer::remove project", "Please select first a project.",
                                             QMessageBox::Ok);
        }
        else
        {
            WorkProject *wp = dynamic_cast<WorkProject*> (node);
            listNodes.append(wp);
        }

    }

    //remove node workProject from tree
    foreach (WorkProject* node, listNodes)
    {
        Node* parentNode = node->getParentNode();
        if (parentNode)
        {
            QModelIndex index = model->getIndex(node);
            removeWorkProject(index);
        }
    }

    ui->treeView->resizeColumnToContents(0);

}

void MDImain::removeWorkingDirectory(QModelIndex index)
{
    if (index.isValid())
    {
        //get a pointer on the selected item
        Node *nodeWd =  model->getNode(index);
        QString name = typeid(*nodeWd).name();

        if (!name.toLower().endsWith("workingdirectory"))
        {
            QMessageBox::warning(this, "HEXplorer::remove project", "Please select first a working directory.",
                                             QMessageBox::Ok);
            return;
        }
        else
        {
            //As the selected node is an A2l file we can cast the node into its real type
            WorkingDirectory *wd = dynamic_cast<WorkingDirectory *> (nodeWd);

            //remove all wprk projects into working directory
            foreach (Node *_node, wd->childNodes)
            {
                QString name = typeid(*_node).name();
                if (name.toLower().endsWith("workproject"))
                {
                    removeWorkProject(model->getIndex(_node));
                }
            }

            //remove the node from treeView model
            Node* nodeParent = nodeWd->getParentNode();
            model->dataRemoved(nodeParent, index.row(), 1);

            //remove working directory from list of Path
            QStringList listPath  = workingDirectory.split(";");
            listPath.removeAll(wd->getFullPath());
            workingDirectory = "";
            foreach (QString str, listPath)
            {
                workingDirectory.append(str + ";");
            }
            QSettings settings(qApp->organizationName(), qApp->applicationName());
            settings.setValue("currentWDPath", workingDirectory);

        }
    }
}

void MDImain::removeWorkingDirectory(QModelIndexList indexList)
{
    //get a pointer on the selected items
    QList<Node*> listNodes;
    foreach (QModelIndex index, indexList)
    {
        Node *node =  model->getNode(index);
        listNodes.append(node);
    }

    //delete each node
    foreach (Node* node, listNodes)
    {
        Node* parentNode = node->getParentNode();
        if (parentNode)
        {
            QModelIndex index = model->getIndex(node);
            removeWorkingDirectory(index);
        }
    }
}

void MDImain::export_ListData()
{
    QStringList strList;
    strList.append("[Label]");

    QStringList listData = ((A2lTreeModel*)ui->treeView_2->model())->getListDataName();
    strList.append(listData);

    QSettings settings(qApp->organizationName(), qApp->applicationName());
    QString currentLabPath = settings.value("currentLabPath").toString();
    QString fileName = QFileDialog::getSaveFileName(this,"save lab files", currentLabPath,
                                                    "lab files (*.lab);;all files (*.*)");
    if (fileName.isEmpty())
        return;

    if (!fileName.endsWith(".lab"))
    {
        fileName.append(".lab");
    }
    currentLabPath = QFileInfo(fileName).absolutePath();
    settings.setValue("currentLabPath", currentLabPath);

    QFile file(fileName);
    file.open(QFile::WriteOnly);
    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    foreach (QString str, strList)
        out << str << "\r\n";
    QApplication::restoreOverrideCursor();
}

void MDImain::editMeasuringChannels()
{
    // check if a file is selected in treeWidget
    QModelIndex index  = ui->treeView->selectionModel()->currentIndex();

    if (index.isValid())
    {
        //get a pointer on the selected item
        Node *node =  model->getNode(index);
        QString name = typeid(*node).name();

        if (!name.endsWith("WorkProject"))
        {
            QMessageBox::warning(this, "HEXplorer::edit measuring channels", "Please select first a project.",
                                             QMessageBox::Ok);
            return;
        }

        int row = index.row();
        if ( row < 0)
        {
            QMessageBox::information(this,"HEXplorer","please first select a file to be edited");
            writeOutput("action edit  file cancelled: no project first selected");
            return;
        }
        else
        {
            //Get the selected file name
            //QString fullFileName = model->name(index);

            //create a pointer on the WorkProject
            //WorkProject *wp = projectList->value(fullFileName);
            WorkProject* wp = (WorkProject*)node;


            //if the a2lFile is not yet parsed, parse.
            if (!wp->a2lFile->isParsed())
            {
                readA2l(wp);
            }

            //if parser error stop
            if (!wp->isOk())
            {
                writeOutput("measurement channels editing not possible due to a2l file issues.");
                return;
            }

            // get the list of MEASUREMET
            QList<Node*> list;
            Node *module = wp->a2lFile->getProject()->getNode("MODULE");
            if (module == NULL)
            {
                return;
            }
            else
            {
                QString _module;
                QList<MODULE*> listModule = wp->a2lFile->getProject()->listModule();
                if (listModule.count() == 0)
                {
                    writeOutput("action edit measurement channels : no Module into A2l file !");
                    return;
                }
                else if (listModule.count() == 1)
                {
                    _module = QString(listModule.at(0)->name);
                }
                else
                {
                    // select a module
                    QString module;
                    DialogChooseModule *diag = new DialogChooseModule(&module);
                    QStringList listModuleName;
                    foreach (MODULE* module, listModule)
                    {
                        listModuleName.append(module->name);
                    }
                    diag->setList(listModuleName);
                    int ret = diag->exec();

                    if (ret == QDialog::Accepted)
                    {
                         _module = module;
                    }
                    else
                    {
                        writeOutput("action open new dataset : no module chosen !");
                        return;
                    }
                }

                Node *dim = module->getNode(_module);
                if (dim == NULL)
                {
                    return;
                }
                else
                {
                    Node *measurment = dim->getNode("MEASUREMENT");
                    if (measurment)
                    {
                        list = measurment->childNodes;
                    }
                    else
                    {
                        return;
                    }
                }
            }

            //display the measuring channels in view
            MeasModel *measModel = new MeasModel();
            measModel->setList(list);

            //create a new spreadSheet
            SpreadsheetView *view = new SpreadsheetView();
            view->setModel(measModel);
            wp->attach(view);

//            FreezeTableWidget *view = new FreezeTableWidget(measModel);
            view->setAlternatingRowColors(true);

            //add a new tab with the spreadsheet
            QIcon icon;
            icon.addFile(":/icones/milky_peigne.png");
            ui->tabWidget->addTab(view, icon, wp->getFullNodeName());

            //set new FormCompare as activated
            ui->tabWidget->setCurrentWidget(view);

            //write output
            writeOutput(QString::number(list.count()) + " measurement labels edited.");
        }
    }
}

void MDImain::editChar()
{
    // check if a file is selected in treeWidget
    QModelIndex index  = ui->treeView->selectionModel()->currentIndex();

    if (index.isValid())
    {
        //get a pointer on the selected item
        Node *node =  model->getNode(index);
        QString name = typeid(*node).name();

        if (!name.endsWith("WorkProject"))
        {
            QMessageBox::warning(this, "HEXplorer::edit measuring channels", "Please select first a project.",
                                             QMessageBox::Ok);
            return;
        }

        int row = index.row();
        if ( row < 0)
        {
            QMessageBox::information(this,"HEXplorer","please first select a file to be edited");
            writeOutput("action edit  file cancelled: no project first selected");
            return;
        }
        else
        {
            //get the Wp
            WorkProject *wp = (WorkProject*)node;

            //if the a2lFile is not yet parsed, parse.
            if (!wp->a2lFile->isParsed())
            {
                readA2l(wp);
            }

            //if parser error stop
            if (!wp->isOk())
            {
                writeOutput("characteristics channels editing not possible due to a2l file issues.");
                return;
            }

            // get the list of CHARACTERISTICS
            QList<Node*> list;
            Node *module = wp->a2lFile->getProject()->getNode("MODULE");
            if (module == NULL)
            {
                return;
            }
            else
            {
                QString _module;
                QList<MODULE*> listModule = wp->a2lFile->getProject()->listModule();
                if (listModule.count() == 0)
                {
                    writeOutput("action edit characteristics : no Module into A2l file !");
                    return;
                }
                else if (listModule.count() == 1)
                {
                    _module = QString(listModule.at(0)->name);
                }
                else
                {
                    // select a module
                    QString module;
                    DialogChooseModule *diag = new DialogChooseModule(&module);
                    QStringList listModuleName;
                    foreach (MODULE* module, listModule)
                    {
                        listModuleName.append(module->name);
                    }
                    diag->setList(listModuleName);
                    int ret = diag->exec();

                    if (ret == QDialog::Accepted)
                    {
                         _module = module;
                    }
                    else
                    {
                        writeOutput("action open new dataset : no module chosen !");
                        return;
                    }
                }

                Node *dim = module->getNode(_module);
                if (dim == NULL)
                {
                    return;
                }
                else
                {
                    Node *charact = dim->getNode("CHARACTERISTIC");
                    if (charact)
                    {
                        list = charact->childNodes;
                    }
                    else
                    {
                        return;
                    }
                }
            }

            //display the characterisitc channels in view
            CharModel *charModel = new CharModel();
            charModel->setList(list);

            //create a new spreadSheet
            SpreadsheetView *view = new SpreadsheetView();
            view->setModel(charModel);
            //QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel(this);

            wp->attach(view);

            view->setAlternatingRowColors(true);

            //add a new tab with the spreadsheet
            QIcon icon;
            icon.addFile(":/icones/milky_outils.png");
            ui->tabWidget->addTab(view, icon, wp->getFullNodeName());

            //set new FormCompare as activated
            ui->tabWidget->setCurrentWidget(view);

            //write output
            writeOutput(QString::number(list.count()) + " characterisitcs edited.");

        }
    }
}

void MDImain::editFandR()
{
    // check if a file is selected in treeWidget
    QModelIndex index  = ui->treeView->selectionModel()->currentIndex();

    if (index.isValid())
    {
        //get a pointer on the selected item
        Node *node =  model->getNode(index);
        QString name = typeid(*node).name();

        if (!name.endsWith("SrecFile"))
        {
            QMessageBox::warning(this, "HEXplorer::edit measuring channels", "Please select first a dataset.",
                                             QMessageBox::Ok);
            return;
        }

        int row = index.row();
        if ( row < 0)
        {
            QMessageBox::information(this,"HEXplorer","please first select a file to be edited");
            writeOutput("action edit  file cancelled: no project first selected");
            return;
        }
        else
        {
            //get the Wp
            SrecFile *srec = (SrecFile*)node;

            //display the characterisitc channels in view
            FandRModel *fnrModel = new FandRModel(srec);

            //create a new spreadSheet
            SpreadsheetView *view = new SpreadsheetView();
            view->setModel(fnrModel);
            srec->attach(view);

            view->setAlternatingRowColors(true);

            //add a new tab with the spreadsheet
            QIcon icon;
            icon.addFile(":/icones/milky_outils.png");
            ui->tabWidget->addTab(view, icon, srec->name);

            //set new FormCompare as activated
            ui->tabWidget->setCurrentWidget(view);

            //write output
            writeOutput("failure and reaction edited.");

        }
    }

}

void MDImain::editObd_Merge()
{
    // check if a file is selected in treeWidget
    QModelIndex index  = ui->treeView->selectionModel()->currentIndex();

    if (index.isValid())
    {
        //get a pointer on the selected item
        Node *node =  model->getNode(index);
        QString name = typeid(*node).name();

        if (!name.endsWith("SrecFile") && !name.endsWith("CdfxFile") && !name.endsWith("Dcm")
                && !name.endsWith("Csv"))
        {
            QMessageBox::warning(this, "HEXplorer::edit measuring channels", "Please select first a dataset.",
                                             QMessageBox::Ok);
            return;
        }

        int row = index.row();
        if ( row < 0)
        {
            QMessageBox::information(this,"HEXplorer","please first select a file to be edited");
            writeOutput("action edit  file cancelled: no project first selected");
            return;
        }
        else
        {
            if (name.toLower().endsWith("srecfile"))
            {
                //get the Wp
                SrecFile *srec = (SrecFile*)node;

                //create a new spreadSheet and attach the model
                SpreadsheetView *view = new SpreadsheetView(this);
                //view->setModel(obdModel);
                srec->attach(view);

                //create a new model for obdMerge function
                ObdMergeModel *obdModel = new ObdMergeModel(srec);

                //sort and filter model
                obdSortFilterProxyModel *proxyModel = new obdSortFilterProxyModel(this, obdModel);

                view->setModel(proxyModel);
                view->setSortingEnabled(true);
                proxyModel->sort(1, Qt::AscendingOrder);

                //sorting functions
                //view->setSortingEnabled(true);
                view->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
                view->setColumnWidth(1,350);
                view->setColumnWidth(2, 50);
                view->setColumnWidth(3, 50);
                view->setColumnWidth(4, 50);
                view->setColumnWidth(5, 50);

                //add a new tab with the spreadsheet
                QIcon icon;
                icon.addFile(":/icones/milky_outils.png");
                ui->tabWidget->addTab(view, icon, srec->name);

                //set new view as activated
                ui->tabWidget->setCurrentWidget(view);

                view->setAlternatingRowColors(true);

                //write output
                writeOutput("OBD merge edited.");
            }
            else if (name.toLower().endsWith("cdfxfile"))
            {
                //get the Wp
                CdfxFile *cdfx = (CdfxFile*)node;

                //create a new spreadSheet and attach the model
                SpreadsheetView *view = new SpreadsheetView(this);
                //view->setModel(obdModel);
                cdfx->attach(view);

                //create a new model for obdMerge function
                ObdMergeModel *obdModel = new ObdMergeModel(cdfx);

                //sort and filter model
                obdSortFilterProxyModel *proxyModel = new obdSortFilterProxyModel(this, obdModel);

                view->setModel(proxyModel);
                view->setSortingEnabled(true);
                proxyModel->sort(1, Qt::AscendingOrder);

                //sorting functions
                //view->setSortingEnabled(true);
                view->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
                view->setColumnWidth(1,350);
                view->setColumnWidth(2, 50);
                view->setColumnWidth(3, 50);
                view->setColumnWidth(4, 50);
                view->setColumnWidth(5, 50);

                //add a new tab with the spreadsheet
                QIcon icon;
                icon.addFile(":/icones/milky_outils.png");
                ui->tabWidget->addTab(view, icon, cdfx->name);

                //set new view as activated
                ui->tabWidget->setCurrentWidget(view);

                view->setAlternatingRowColors(true);

                //write output
                writeOutput("OBD merge edited.");

            }
            else if (name.toLower().endsWith("dcm"))
            {
                //get the Wp
                Dcm *dcm = (Dcm*)node;

                //create a new spreadSheet and attach the model
                SpreadsheetView *view = new SpreadsheetView(this);
                //view->setModel(obdModel);
                dcm->attach(view);

                //create a new model for obdMerge function
                ObdMergeModel *obdModel = new ObdMergeModel(dcm);

                //sort and filter model
                obdSortFilterProxyModel *proxyModel = new obdSortFilterProxyModel(this, obdModel);

                view->setModel(proxyModel);
                view->setSortingEnabled(true);
                proxyModel->sort(1, Qt::AscendingOrder);

                //sorting functions
                //view->setSortingEnabled(true);
                view->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
                view->setColumnWidth(1,350);
                view->setColumnWidth(2, 50);
                view->setColumnWidth(3, 50);
                view->setColumnWidth(4, 50);
                view->setColumnWidth(5, 50);

                //add a new tab with the spreadsheet
                QIcon icon;
                icon.addFile(":/icones/milky_outils.png");
                ui->tabWidget->addTab(view, icon, dcm->name);

                //set new view as activated
                ui->tabWidget->setCurrentWidget(view);

                view->setAlternatingRowColors(true);

                //write output
                writeOutput("OBD merge edited.");

            }
            else if (name.toLower().endsWith("csv"))
            {
                //get the Wp
                Csv *csv = (Csv*)node;

                //create a new spreadSheet and attach the model
                SpreadsheetView *view = new SpreadsheetView(this);
                //view->setModel(obdModel);
                csv->attach(view);

                //create a new model for obdMerge function
                ObdMergeModel *obdModel = new ObdMergeModel(csv);

                //sort and filter model
                obdSortFilterProxyModel *proxyModel = new obdSortFilterProxyModel(this, obdModel);

                view->setModel(proxyModel);
                view->setSortingEnabled(true);
                proxyModel->sort(1, Qt::AscendingOrder);

                //sorting functions
                //view->setSortingEnabled(true);
                view->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
                view->setColumnWidth(1,350);
                view->setColumnWidth(2, 50);
                view->setColumnWidth(3, 50);
                view->setColumnWidth(4, 50);
                view->setColumnWidth(5, 50);

                //add a new tab with the spreadsheet
                QIcon icon;
                icon.addFile(":/icones/milky_outils.png");
                ui->tabWidget->addTab(view, icon, csv->name);

                //set new view as activated
                ui->tabWidget->setCurrentWidget(view);

                view->setAlternatingRowColors(true);

                //write output
                writeOutput("OBD merge edited.");

            }

        }
    }

}

void MDImain::spaceRecovery()
{
    // check if a file is selected in treeWidget
    QModelIndex index  = ui->treeView->selectionModel()->currentIndex();

    if (index.isValid())
    {
        //get a pointer on the selected item
        Node *node =  model->getNode(index);
        QString name = typeid(*node).name();

        if (!name.endsWith("SrecFile"))
        {
            QMessageBox::warning(this, "HEXplorer::edit measuring channels", "Please select first a dataset.",
                                             QMessageBox::Ok);
            return;
        }

        int row = index.row();
        if ( row < 0)
        {
            QMessageBox::information(this,"HEXplorer","please first select a file to be edited");
            writeOutput("action edit  file cancelled: no project first selected");
            return;
        }
        else
        {
            //get the Wp
            SrecFile *srec = (SrecFile*)node;

            //display the characterisitc channels in view
            FandRModel *fnrModel = new FandRModel(srec);

            //create a new spreadSheet
            SpreadsheetView *view = new SpreadsheetView();
            view->setModel(fnrModel);
            srec->attach(view);

            view->setAlternatingRowColors(true);

            //add a new tab with the spreadsheet
            QIcon icon;
            icon.addFile(":/icones/milky_outils.png");
            ui->tabWidget->addTab(view, icon, srec->name);

            //set new FormCompare as activated
            ui->tabWidget->setCurrentWidget(view);

            //write output
            writeOutput("failure and reaction edited.");

        }
    }
}

void MDImain::newFormScript()
{
    // check if a project is selected in treeView
    //QModelIndex index  = ui->treeView->selectionModel()->currentIndex();

    //Get the project name
    //QString fullA2lName = model->name(index);

    //create a pointer on the WorkProject
    //WorkProject *wp = projectList->value(fullA2lName);

    //FormScript *form = new FormScript(wp, this);
    //form->show();
}

void MDImain::compare_A2lFile()
{
    //get a2lFiles path
    QModelIndexList list = ui->treeView->selectionModel()->selectedIndexes();

    if (list.count() == 2)
    {
        //QString moduleName = "DIM";
        QString moduleName1;
        QString moduleName2;

        // get a2l file path
//        QString str1 = model->name(list.at(0));
//        QString str2 = model->name(list.at(1));

        Node *node1 =  model->getNode(list.at(0));
        Node *node2 =  model->getNode(list.at(1));


        // get listChar1
//        WorkProject *wp1 = projectList->value(str1);
        WorkProject *wp1 = (WorkProject*)node1;
        QString str1 = wp1->getFullA2lFileName();
        QStringList listChar1;
        QStringList listMeas1;
        if (wp1)  //to prevent any crash of the aplication
        {
            //if the a2lFile is not yet parsed, parse.
            if (!wp1->a2lFile->isParsed())
            {
                readA2l(wp1);
            }

            // if MODULE in ASAP file
            if (wp1->a2lFile->getProject()->getNode("MODULE") != NULL)
            {
                //choose the module
                QList<MODULE*> list = wp1->a2lFile->getProject()->listModule();
                if (list.count() == 0)
                {
                    writeOutput("action comapre a2l cancelled : no Module into A2l file !");
                    return;
                }
                else if (list.count() == 1)
                {
                    moduleName1 = QString(list.at(0)->name);
                }
                else
                {
                    // select a module
                    QString module;
                    DialogChooseModule *diag = new DialogChooseModule(&module);
                    QStringList listModuleName;
                    foreach (MODULE* module, list)
                    {
                        listModuleName.append(module->name);
                    }
                    diag->setList(listModuleName);
                    int ret = diag->exec();

                    if (ret == QDialog::Accepted)
                    {
                       moduleName1 = QString(list.at(0)->name);
                    }
                    else
                    {
                        writeOutput("action compare a2l : no module chosen !");
                        return;
                    }
                }

                MODULE *mod = (MODULE*)wp1->a2lFile->getProject()->getNode("MODULE/" + moduleName1);
                listChar1 = mod->listChar;
                listMeas1 = mod->listMeas;
            }
        }

        // get CHAR listChar2
//        WorkProject *wp2 = projectList->value(str2);
        WorkProject *wp2 = (WorkProject*)node2;
        QString str2 = wp2->getFullA2lFileName();
        QStringList listChar2;
        QStringList listMeas2;
        if (wp2)  //to prevent any crash of the aplication
        {
            //if the a2lFile is not yet parsed, parse.
            if (!wp2->a2lFile->isParsed())
            {
                readA2l(wp2);
            }

            // if MODULE in ASAP file
            if (wp2->a2lFile->getProject()->getNode("MODULE") != NULL)
            {
                //choose the module
                QList<MODULE*> list = wp2->a2lFile->getProject()->listModule();
                if (list.count() == 0)
                {
                    writeOutput("action comapre a2l cancelled : no Module into A2l file !");
                    return;
                }
                else if (list.count() == 1)
                {
                    moduleName2 = QString(list.at(0)->name);
                }
                else
                {
                    // select a module
                    QString module;
                    DialogChooseModule *diag = new DialogChooseModule(&module);
                    QStringList listModuleName;
                    foreach (MODULE* module, list)
                    {
                        listModuleName.append(module->name);
                    }
                    diag->setList(listModuleName);
                    int ret = diag->exec();

                    if (ret == QDialog::Accepted)
                    {
                       moduleName2 = QString(list.at(0)->name);
                    }
                    else
                    {
                        writeOutput("action compare a2l : no module chosen !");
                        return;
                    }
                }

                listChar2 = ((MODULE*)wp2->a2lFile->getProject()->getNode("MODULE/" + moduleName2))->listChar;
                listMeas2 = ((MODULE*)wp2->a2lFile->getProject()->getNode("MODULE/" + moduleName2))->listMeas;
            }
        }

        //Missing labels
        QStringList missingChars;
        QStringList missingMeas;
        foreach (QString str, listChar1)
        {
            //QStringList::iterator i = qBinaryFind(list2.begin(), list2.end(), str);
            QStringList::iterator i = std::lower_bound(listChar2.begin(), listChar2.end(), str);
            if (i == listChar2.end())
            {
                missingChars.append(str);
            }
            else
            {
                if (str.compare(*i) != 0)
                {
                    missingChars.append(str);
                }
            }
        }      
        foreach (QString str, listMeas1)
        {
            //QStringList::iterator i = qBinaryFind(list2.begin(), list2.end(), str);
            QStringList::iterator i = std::lower_bound(listMeas2.begin(), listMeas2.end(), str);
            if (i == listMeas2.end())
            {
                missingMeas.append(str);
            }
            else
            {
                if (str.compare(*i) != 0)
                {
                    missingMeas.append(str);
                }
            }
        }

        //New labels
        QStringList newChars;
        QStringList newMeas;
        foreach (QString str, listChar2)
        {
            //QStringList::iterator i = qBinaryFind(list1.begin(), list1.end(), str);
            QStringList::iterator i = std::lower_bound(listChar1.begin(), listChar1.end(), str);
            //if (i == list1.end())
            if (i == listChar1.end())
            {
                newChars.append(str);
            }
            else
            {
                if (str.compare(*i) != 0)
                {
                    newChars.append(str);
                }
            }
        }
        foreach (QString str, listMeas2)
        {
            //QStringList::iterator i = qBinaryFind(list1.begin(), list1.end(), str);
            QStringList::iterator i = std::lower_bound(listMeas1.begin(), listMeas1.end(), str);
            //if (i == list1.end())
            if (i == listMeas1.end())
            {
                newMeas.append(str);
            }
            else
            {
                if (str.compare(*i) != 0)
                {
                    newMeas.append(str);
                }
            }
        }

        //Missing, new, modified subsets
        Node *fun1 = wp1->a2lFile->getProject()->getNode("MODULE/" + moduleName1 + "/FUNCTION");
        Node *fun2 = wp2->a2lFile->getProject()->getNode("MODULE/" + moduleName2 + "/FUNCTION");
        QStringList missingSubsets;
        QStringList newSubsets;
        QStringList modifiedSubsets;
        if (fun1 && fun2)
        {
            QStringList listSubset1;
            foreach (Node *subset, fun1->childNodes)
            {
                listSubset1.append(subset->name);
            }
            listSubset1.sort();
            QStringList listSubset2;
            foreach (Node *subset, fun2->childNodes)
            {
                listSubset2.append(subset->name);

            }
            listSubset2.sort();

            //missing subsets
            foreach (QString str, listSubset1)
            {
                if (!listSubset2.contains(str))
                    missingSubsets.append(str);
            }

            //new subsets
            foreach (QString str, listSubset2)
            {
                if (!listSubset1.contains(str))
                    newSubsets.append(str);
            }

            //modified subsets
            foreach (Node *subset1, fun1->childNodes)
            {
                //get Node into listSubsets2
                Node *subset2 = fun2->getNode(subset1->name);
                if (subset2)
                {

                    //get the label list from subset1
                    QStringList listChar_subset1;
                    DEF_CHARACTERISTIC *def_char = (DEF_CHARACTERISTIC*)subset1->getNode("DEF_CHARACTERISTIC");
                    REF_CHARACTERISTIC *ref_char = (REF_CHARACTERISTIC*)subset1->getNode("REF_CHARACTERISTIC");
                    if (def_char)
                        listChar_subset1 = def_char->getCharList();
                    else if (ref_char)
                        listChar_subset1 = ref_char->getCharList();
                    else
                    {
                        Node *group = wp1->a2lFile->getProject()->getNode("MODULE/" + moduleName1 + "/GROUP");
                        if (group)
                        {
                            GGROUP *grp = (GGROUP*)group->getNode(subset1->name);
                            if (grp)
                            {
                                DEF_CHARACTERISTIC *def_char = (DEF_CHARACTERISTIC*)grp->getNode("DEF_CHARACTERISTIC");
                                REF_CHARACTERISTIC *ref_char = (REF_CHARACTERISTIC*)grp->getNode("REF_CHARACTERISTIC");
                                if (def_char)
                                    listChar_subset1 = def_char->getCharList();
                                else if (ref_char)
                                    listChar_subset1 = ref_char->getCharList();
                            }
                        }
                    }


                    //get the label list from subset2
                    QStringList listChar_subset2;
                    def_char = (DEF_CHARACTERISTIC*)subset2->getNode("DEF_CHARACTERISTIC");
                    ref_char = (REF_CHARACTERISTIC*)subset2->getNode("REF_CHARACTERISTIC");
                    if (def_char)
                        listChar_subset2 = def_char->getCharList();
                    else if (ref_char)
                        listChar_subset2 = ref_char->getCharList();
                    else
                    {
                        Node *group = wp2->a2lFile->getProject()->getNode("MODULE/" + moduleName2 + "/GROUP");
                        if (group)
                        {
                            GGROUP *grp = (GGROUP*)group->getNode(subset1->name);
                            if (grp)
                            {
                                DEF_CHARACTERISTIC *def_char = (DEF_CHARACTERISTIC*)grp->getNode("DEF_CHARACTERISTIC");
                                REF_CHARACTERISTIC *ref_char = (REF_CHARACTERISTIC*)grp->getNode("REF_CHARACTERISTIC");
                                if (def_char)
                                    listChar_subset2 = def_char->getCharList();
                                else if (ref_char)
                                    listChar_subset2 = ref_char->getCharList();
                            }
                        }
                    }


                    //compare listChar_Subset
                    bool same = true;
                    foreach (QString str, listChar_subset1)
                    {
                        if (!listChar_subset2.contains(str))
                            same = false;
                    }

                    if (same)
                    {
                        foreach (QString str, listChar_subset2)
                        {
                            if (!listChar_subset1.contains(str))
                                same = false;
                        }
                    }

                    if (!same)
                    {
                        modifiedSubsets.append(subset1->name);
                    }

                }
            }

        }

        //create a string to load into QScintilla
        QString text;

        //summary
        text.append("--------------- Calibration Labels ----------------\n");
        text.append(QString::number(missingChars.count()) + " missing calibration labels into " + str2 + "\n");
        text.append(QString::number(newChars.count()) + " new calibration labels into " + str2 + "\n");
        text.append("--------------- Measurement Labels ----------------\n");
        text.append(QString::number(missingMeas.count()) + " missing measurement labels into " + str2 + "\n");
        text.append(QString::number(newMeas.count()) + " new measurement labels into " + str2 + "\n");
        text.append("-------------- Subsets (Functions) ----------------\n");
        text.append(QString::number(missingSubsets.count()) + " missing subsets into " + str2 + "\n");
        text.append(QString::number(newSubsets.count()) + " new subsets into " + str2 + "\n");
        text.append(QString::number(modifiedSubsets.count()) + " modified subsets\n");
        text.append("---------------------------------------------------\n");

        //missing labels
        text.append("\n");
        text.append(QString::number(missingChars.count()) + " missing labels into " + str2 + " : \n");
        foreach (QString str, missingChars)
        {
            text.append("\t" + str + "\n");
        }

        //new labels
        text.append("\n");
        text.append(QString::number(newChars.count()) + " new labels into " + str2 + " : \n");
        foreach (QString str, newChars)
        {
            text.append("\t" + str + "\n");
        }

        //missing Measurements
        text.append("\n");
        text.append(QString::number(missingMeas.count()) + " missing measurement labels into " + str2 + " : \n");
        foreach (QString str, missingMeas)
        {
            text.append("\t" + str + "\n");
        }

        //new Measurements
        text.append("\n");
        text.append(QString::number(newMeas.count()) + " new measurement labels into " + str2 + " : \n");
        foreach (QString str, newMeas)
        {
            text.append("\t" + str + "\n");
        }


        //missing subsets
        text.append("\n");
        text.append(QString::number(missingSubsets.count()) + " missing subsets into " + str2 + " : \n");
        foreach (QString str, missingSubsets)
        {
            text.append("\t" + str + "\n");
        }

        //new subsets
        text.append("\n");
        text.append(QString::number(newSubsets.count()) + " new subsets into " + str2 + " : \n");
        foreach (QString str, newSubsets)
        {
            text.append("\t" + str + "\n");
        }

        //modified subsets
        text.append("\n");
        text.append(QString::number(modifiedSubsets.count()) + " modified subsets\n");
        foreach (QString str, modifiedSubsets)
        {
            text.append("\t" + str + "\n");
        }

        //create the textEditor (Qscintilla)
        ChTextEdit *textEdit = new ChTextEdit;
        textEdit->setText(text);

        //add the Qscintilla editor to listTab
        tabList->insert("fileName", textEdit);

        //draw the tab in tabWidget
        QIcon icon;
        icon.addFile(":/icones/milky_cartable.png");

        ui->tabWidget->addTab(tabList->value("fileName"), icon, "compare A2L");

        //set new FormCompare as activated
        ui->tabWidget->setCurrentWidget(tabList->value("fileName"));



    }
}

// ----------------- Data containers -----------------------//

void MDImain::readA2l(WorkProject* wp)
{

    // display status bar   
    QSettings settings(qApp->organizationName(), qApp->applicationName());
    bool openMP = settings.value("openMP").toBool();
    if (!openMP)
    {
        statusBar()->show();
        progBar->reset();
        connect(wp, SIGNAL(incProgressBar(int,int)), this, SLOT(setValueProgressBar(int,int)),Qt::DirectConnection );
    }

    // parse the a2l file
    wp->parse();

    wp->addA2lnode(wp->a2lFile->getProject() , wp);

    // hide the statusbar
    statusBar()->hide();
    progBar->reset();

    //messagebox in case of a2l parser error
    if (!wp->isOk())
    {
        QMessageBox::warning(this, "HEXplorer" ,"ASAP file parser error");
    }

    //update output console
    ui->listWidget->addItems(wp->_outputList());
    ui->listWidget->scrollToBottom();

}

HexFile* MDImain::readHexFile(HexFile *hex)
{
    //start timer
    QElapsedTimer timer;
    timer.start();
    //double ti = omp_get_wtime();

    //read hex file if not read
    QString fullName = hex->fullName();


    if (!QFile(fullName).exists())
    {
        writeOutput("action open new dataset : cancelled because file does not exist on drive. Please reload the working directory.");
        return NULL;
    }

    //get parent WP
    WorkProject *wp = hex->getParentWp();

    //get parent Node in treeView (might be different than WP)
    Node* nodeParent = hex->getParentNode();

    //if the a2lFile is not yet parsed, parse.
    if (!wp->a2lFile->isParsed())
    {
        readA2l(wp);
    }

    //check if A2l parsing was successfull
    if (!wp->isOk())
    {
        QMessageBox::warning(this,"HEXplorer","action open new dataset failed. A2Lfile is not parsed correctly.");
        writeOutput("action open new dataset failed : A2Lfile could not be parsed correctly.");
        return 0;
    }

    //get the index of the orginal hex node in treeview
    QModelIndex index = model->getIndex(hex);

    //read hex file : hex* pointer will point on another memory cell
    QList<MODULE*> list = wp->a2lFile->getProject()->listModule();
    if (list.count() == 0)
    {
        writeOutput("action open new dataset : no Module into A2l file !");
        return 0;
    }
    else if (list.count() == 1)
    {
        //delete hex;
        hex = new HexFile(fullName, wp, QString(list.at(0)->name));
    }
    else
    {
        // select a module
        QString module;
        DialogChooseModule *diag = new DialogChooseModule(&module);
        QStringList listModuleName;
        foreach (MODULE* module, list)
        {
            listModuleName.append(module->name);
        }
        diag->setList(listModuleName);
        int ret = diag->exec();

        if (ret == QDialog::Accepted)
        {
            hex = new HexFile(fullName, wp, module);
        }
        else
        {
            writeOutput("action open new dataset : no module chosen !");
            return 0;
        }
    }


    connect(hex, SIGNAL(progress(int,int)), this, SLOT(setValueProgressBar(int,int)), Qt::DirectConnection);

    if (hex->read())
    {
        QModelIndexList listIndex;
        foreach (QModelIndex index, model->getPersistentIndexList())
         {
             if (ui->treeView->isExpanded(index))
             {
                 listIndex << index;
             }
         }

        //remove original hex file from node/tree
        deleteFileFromProject(index);

        //add new hex node in WP
        wp->addHex(hex, nodeParent);

        // hide the statusbar
        statusBar()->hide();
        progBar->reset();

        //stop timer
        //double tf = omp_get_wtime();
        qint64 elapsedTime = timer.restart();
        double _elapsedTime = static_cast<double>(elapsedTime)/1000;

        //update the treeView model
        foreach (QModelIndex index, listIndex)
        {
            if (index.isValid())
            {
                ui->treeView->expand(index);
            }
        }

        ui->treeView->resizeColumnToContents(0);

        writeOutput("action open new dataset : HEX file add to project in " + QString::number(_elapsedTime, 'g', 3) + " sec");

        return hex;
    }
    else
    {
        delete hex;

        // hide the statusbar
        statusBar()->hide();
        progBar->reset();

        writeOutput("action open new dataset : cancelled");

        return NULL;
    }

}

SrecFile* MDImain::readSrecFile(SrecFile* srec)
{
    //start timer
    //double ti = omp_get_wtime();
    QElapsedTimer timer;
    timer.start();

    //store Srec fullname
    QString fullName = srec->fullName();
    if (!QFile(fullName).exists())
    {
        writeOutput("action open new dataset : cancelled because file does not exist on drive. Please reload the working directory.");
        return NULL;
    }

    //get parent WP
    WorkProject *wp = srec->getParentWp();

    //get parent Node in treeView (might be different than WP)
    Node* nodeParent = srec->getParentNode();

    //if the a2lFile is not yet parsed, parse.
    if (!wp->a2lFile->isParsed())
    {
        readA2l(wp);
    }

    //check if A2l parsing was successfull
    if (!wp->isOk())
    {
        QMessageBox::warning(this,"HEXplorer","action open new dataset failed. A2Lfile is not parsed correctly.");
        writeOutput("action open new dataset failed : A2Lfile could not be parsed correctly.");
        return 0;
    }

    //get the index of the orginal hex node in treeview
    QModelIndex index = model->getIndex(srec);

    //read srec file
    QList<MODULE*> list = wp->a2lFile->getProject()->listModule();
    if (list.count() == 0)
    {
        writeOutput("action open new dataset : no Module into A2l file !");
        return 0;
    }
    else if (list.count() == 1)
    {
        srec = new SrecFile(fullName, wp, QString(list.at(0)->name));
    }
    else
    {
        // select a module
        QString module;
        DialogChooseModule *diag = new DialogChooseModule(&module);
        QStringList listModuleName;
        foreach (MODULE* module, list)
        {
            listModuleName.append(module->name);
        }
        diag->setList(listModuleName);
        int ret = diag->exec();

        if (ret == QDialog::Accepted)
        {
            srec = new SrecFile(fullName, wp, module);
        }
        else
        {
            writeOutput("action open new dataset : no module chosen !");
            return 0;
        }
    }

    // display status bar
    connect(srec, SIGNAL(progress(int,int)), this, SLOT(setValueProgressBar(int,int)), Qt::DirectConnection);

    if (srec->read())
    {
        QModelIndexList listIndex;
        foreach (QModelIndex index, model->getPersistentIndexList())
         {
             if (ui->treeView->isExpanded(index))
             {
                 listIndex << index;
             }
         }

        //remove original srec file from node/tree
        deleteFileFromProject(index);

        //attach srec to WP and as node in nodeParent
        wp->addSrec(srec, nodeParent);

        // hide the statusbar
        statusBar()->hide();
        progBar->reset();

        //stop timer
        //double tf = omp_get_wtime();
        qint64 elapsedTime = timer.restart();
        double _elapsedTime = static_cast<double>(elapsedTime)/1000;

        //update the treeView model
        foreach (QModelIndex index, listIndex)
        {
             ui->treeView->expand(index);
        }
        ui->treeView->resizeColumnToContents(0);

        writeOutput("action open new dataset : Srec file add to project in " + QString::number(_elapsedTime,'g', 3) + " sec.");

        return srec;
    }
    else
    {
        delete srec;

        // hide the statusbar
        statusBar()->hide();
        progBar->reset();

        writeOutput("action open new dataset : cancelled");

        return NULL;
    }
}

void MDImain::sort_BySubset()
{
    //get the index of the selected hex file
    QModelIndex index  = ui->treeView->selectionModel()->currentIndex();

    //get a pointer on the selected item
    Node *node =  model->getNode(index);

    QString name = typeid(*node).name();
    if (name.endsWith("HexFile"))
    {
        //As the selected node is an Hex file we can cast the node into its real type : HexFile
        HexFile *hex = dynamic_cast<HexFile *> (node);

        //call sort by subset
        if (hex->isSortedBySubsets())
            hex->sortModifiedDataBySubset(false);
        else
            hex->sortModifiedDataBySubset(true);
    }
    else if (name.endsWith("SrecFile"))
    {
        //As the selected node is an Srec file we can cast the node into its real type : SrecFile
        SrecFile *srec = dynamic_cast<SrecFile *> (node);

        //call sort by subset
        if (srec->isSortedBySubsets())
            srec->sortModifiedDataBySubset(false);
        else
            srec->sortModifiedDataBySubset(true);
    }
    else if (name.endsWith("Csv"))
    {
        //As the selected node is an Hex file we can cast the node into its real type : HexFile
        Csv *csv = dynamic_cast<Csv *> (node);

        //call sort by subset
        if (csv->isSortedBySubsets())
            csv->sortModifiedDataBySubset(false);
        else
            csv->sortModifiedDataBySubset(true);
    }
    else if (name.endsWith("CdfxFile"))
    {
        //As the selected node is an Hex file we can cast the node into its real type : HexFile
        CdfxFile *cdfx = dynamic_cast<CdfxFile *> (node);

        //call sort by subset
        if (cdfx->isSortedBySubsets())
            cdfx->sortModifiedDataBySubset(false);
        else
            cdfx->sortModifiedDataBySubset(true);
    }
    else if (name.endsWith("Dcm"))
    {
        //As the selected node is an Hex file we can cast the node into its real type : HexFile
        Dcm *dcm = dynamic_cast<Dcm *> (node);

        //call sort by subset
        if (dcm->isSortedBySubsets())
            dcm->sortModifiedDataBySubset(false);
        else
            dcm->sortModifiedDataBySubset(true);
    }
    else
    {
        QMessageBox::warning(this, "HEXplorer::sort by subset", "Please select first an hex, csv or Cdfx file.",
                                         QMessageBox::Ok);
        return;
    }
}

void MDImain::reset_Data()
{
    QModelIndexList listIndex = ui->treeView->selectionModel()->selectedIndexes();

    foreach (QModelIndex index, listIndex)
    {
        Data *data = (Data*)model->getNode(index);
        data->resetAll();
        updateView();
    }
}

void MDImain::read_ValuesFromCsv()
{
    // check if a project is selected in treeView
    QModelIndex index  = ui->treeView->selectionModel()->currentIndex();

    if (index.isValid())
    {
        //get a pointer on the selected item
        Node *node =  model->getNode(index);
        QString name = typeid(*node).name();

        if (name.endsWith("HexFile"))
        {
            int row = index.row();
            if ( row < 0)
            {
                QMessageBox::information(this,"HEXplorer","please first select a project");
                writeOutput("action open new dataset cancelled: no project first selected");
                return;
            }
            else
            {
                HexFile *hex  = (HexFile*)model->getNode(index);

                QSettings settings(qApp->organizationName(), qApp->applicationName());
                QString path = settings.value("currentCsvPath").toString();

                QStringList files = QFileDialog::getOpenFileNames(this,
                                                  tr("select CSV files"), path,
                                                  tr("CSV files (*.csv);;all files (*.*)"));

                if (files.isEmpty())
                {
                   writeOutput("action open new CSV file : no CSV file selected");
                   return;
                }
                else
                {
                    //Get the project (A2l) name
                    QString fullA2lName = model->name(model->parent(index));

                    //create a pointer on the WorkProject
                    WorkProject *wp = projectList->value(fullA2lName);

                    // if no MOD_COMMON in ASAP file
                    if (wp->a2lFile->getProject()->getNode("MODULE") == NULL)
                    {
                        QMessageBox::information(this, "HEXplorer", tr("no MOD_COMMON in ASAP file"));
                        writeOutput("action open new dataset : no MOD_COMMON in ASAP file");
                        return;
                    }

                    // check if Csv already in project
                    foreach (QString fullCsvName, files)
                    {
                        //if the selected Hex file is already into the project => exit
                        if (wp->csvFiles().contains(fullCsvName))
                        {
                            QMessageBox::information(this, "HEXplorer", tr("file already in project"));
                            writeOutput("action open new CSV file : CSV file already in project");
                            files.removeOne(fullCsvName);
                            return;
                        }
                    }

                    // Read CSV files
                    QList<Csv*> list;
                    foreach (QString fullCsvName, files)
                    {
                        //update currentHexPath
                        QSettings settings(qApp->organizationName(), qApp->applicationName());
                        QString currentCsvPath = QFileInfo(fullCsvName).absolutePath();
                        settings.setValue("currentCsvPath", currentCsvPath);

                        //start a timer
                        //double ti = omp_get_wtime();
                        QElapsedTimer timer;
                        timer.start();

                        //add the file to the project
                        Csv *csv = new Csv(fullCsvName, wp, hex->getModuleName());

                        // display status bar
                        statusBar()->show();
                        progBar->reset();
                        connect(csv, SIGNAL(incProgressBar(int,int)), this, SLOT(setValueProgressBar(int,int)), Qt::DirectConnection);

                        if (csv->readFile())
                        {
                            list.append(csv);

                            //stop timer
                            //double tf = omp_get_wtime();
                            qint64 elapsedTime = timer.restart();
                            double _elapsedTime = static_cast<double>(elapsedTime)/1000;

                            writeOutput("action open new CSV file : CSV file add to project in " + QString::number(_elapsedTime, 'g', 3) + " sec");
                        }
                        else
                        {
                             writeOutput("action open new CSV file : CSV file NOT added to project due to wrong format ");
                        }

                        // hide the statusbar
                        statusBar()->hide();
                        progBar->reset();
                    }

                    // display CSV files
                    if (list.isEmpty())
                    {
                        return;
                    }
                    else
                    {
                        QStringList listLabel;
                        A2lTreeModel *mod = new A2lTreeModel();
                        DialogCsv *w = new DialogCsv(this, &listLabel);

                        foreach (Csv *csv, list)
                        {
                            csv->addLabelsAsChild();
                            mod->addNode2RootNode(csv);
                        }
                        w->setModel(mod);
                        w->exec();

                        // copy the selected data
                        Node *root = mod->getRootNode();
                        foreach (QString str, listLabel)
                        {
                            Data *data1 = (Data*)root->getNode(str);
                            if (data1)
                            {
                                // get the data from destination container
                                Data* data2 = hex->getData(data1->name);

                                // if destination data exists perform copy
                                if (data2)
                                {
                                    data2->copyAllFrom(data1);
                                }
                            }
                        }

                        //delete csv files
                        delete mod;
                    }
                }
            }
        }
        else if (name.endsWith("SrecFile"))
        {
            int row = index.row();
            if ( row < 0)
            {
                QMessageBox::information(this,"HEXplorer","please first select a project");
                writeOutput("action open new dataset cancelled: no project first selected");
                return;
            }
            else
            {
                SrecFile *srec  = (SrecFile*)model->getNode(index);

                QSettings settings(qApp->organizationName(), qApp->applicationName());
                QString path = settings.value("currentCsvPath").toString();

                QStringList files = QFileDialog::getOpenFileNames(this,
                                                  tr("select CSV files"), path,
                                                  tr("CSV files (*.csv);;all files (*.*)"));

                if (files.isEmpty())
                {
                   writeOutput("action open new CSV file : no CSV file selected");
                   return;
                }
                else
                {
                    //Get the project (A2l) name
                    QString fullA2lName = model->name(model->parent(index));

                    //create a pointer on the WorkProject
                    WorkProject *wp = projectList->value(fullA2lName);

                    // if no MOD_COMMON in ASAP file
                    if (wp->a2lFile->getProject()->getNode("MODULE") == NULL)
                    {
                        QMessageBox::information(this, "HEXplorer", tr("no MOD_COMMON in ASAP file"));
                        writeOutput("action open new dataset : no MOD_COMMON in ASAP file");
                        return;
                    }

                    // check if Csv already in project
                    foreach (QString fullCsvName, files)
                    {
                        //if the selected Hex file is already into the project => exit
                        if (wp->csvFiles().contains(fullCsvName))
                        {
                            QMessageBox::information(this, "HEXplorer", tr("file already in project"));
                            writeOutput("action open new CSV file : CSV file already in project");
                            files.removeOne(fullCsvName);
                            return;
                        }
                    }

                    // Read CSV files
                    QList<Csv*> list;
                    foreach (QString fullCsvName, files)
                    {
                        //update currentHexPath
                        QSettings settings(qApp->organizationName(), qApp->applicationName());
                        QString currentCsvPath = QFileInfo(fullCsvName).absolutePath();
                        settings.setValue("currentCsvPath", currentCsvPath);

                        //start a timer
                        QElapsedTimer timer;
                        timer.start();

                        //add the file to the project
                        Csv *csv = new Csv(fullCsvName, wp, srec->getModuleName());

                        // display status bar
                        statusBar()->show();
                        progBar->reset();
                        connect(csv, SIGNAL(incProgressBar(int,int)), this, SLOT(setValueProgressBar(int,int)), Qt::DirectConnection);

                        if (csv->readFile())
                        {
                            list.append(csv);

                            //stop timer
                            qint64 elapsedTime = timer.restart();
                            double _elapsedTime = static_cast<double>(elapsedTime)/1000;

                            writeOutput("action open new CSV file : CSV file add to project in " + QString::number(_elapsedTime, 'g', 3) + " sec");
                        }
                        else
                        {
                             writeOutput("action open new CSV file : CSV file NOT added to project due to wrong format ");
                        }

                        // hide the statusbar
                        statusBar()->hide();
                        progBar->reset();
                    }

                    // display CSV files
                    if (list.isEmpty())
                    {
                        return;
                    }
                    else
                    {
                        QStringList listLabel;
                        A2lTreeModel *mod = new A2lTreeModel();
                        DialogCsv *w = new DialogCsv(this, &listLabel);

                        foreach (Csv *csv, list)
                        {
                            csv->addLabelsAsChild();
                            mod->addNode2RootNode(csv);
                        }
                        w->setModel(mod);
                        w->exec();

                        // copy the selected data
                        Node *root = mod->getRootNode();
                        foreach (QString str, listLabel)
                        {
                            Data *data1 = (Data*)root->getNode(str);
                            if (data1)
                            {
                                // get the data from destination container
                                Data* data2 = srec->getData(data1->name);

                                // if destination data exists perform copy
                                if (data2)
                                {
                                    data2->copyAllFrom(data1);
                                }
                            }
                        }

                        //delete csv files
                        delete mod;
                    }
                }
            }

        }
        else
        {
            QMessageBox::warning(this, "HEXplorer::read csv file values", "Please select first an hex or Srec file.",
                                             QMessageBox::Ok);
            return;
        }


    }
}

void MDImain::read_ValuesFromCdfx()
{
    // check if a project is selected in treeView
    QModelIndex index  = ui->treeView->selectionModel()->currentIndex();

    if (index.isValid())
    {
        //get a pointer on the selected item
        Node *node =  model->getNode(index);
        QString name = typeid(*node).name();

        if (name.endsWith("HexFile"))
        {
            int row = index.row();
            if ( row < 0)
            {
                QMessageBox::information(this,"HEXplorer","please first select an Hex file");
                writeOutput("action read CDFX file cancelled: no Hex file selected");
                return;
            }
            else
            {
                HexFile *hex  = (HexFile*)model->getNode(index);

                QSettings settings(qApp->organizationName(), qApp->applicationName());
                QString path = settings.value("currentCdfxPath").toString();

                QStringList files = QFileDialog::getOpenFileNames(this,
                                                  tr("select CDFX files"), path,
                                                  tr("CDFX files (*.cdfx);;all files (*.*)"));

                if (files.isEmpty())
                {
                   writeOutput("action read CDFX values : no CDFX file selected");
                   return;
                }
                else
                {
                    //Get the project (A2l) name
                    QString fullA2lName = model->name(model->parent(index));

                    //create a pointer on the WorkProject
                    WorkProject *wp = projectList->value(fullA2lName);

                    // if no MOD_COMMON in ASAP file
                    if (wp->a2lFile->getProject()->getNode("MODULE") == NULL)
                    {
                        QMessageBox::information(this, "HEXplorer", tr("no MOD_COMMON in ASAP file"));
                        writeOutput("action open new dataset : no MOD_COMMON in ASAP file");
                        return;
                    }

                    // check if Cdfx already in project
                    foreach (QString fullCdfxName, files)
                    {
                        //if the selected Hex file is already into the project => exit
                        if (wp->csvFiles().contains(fullCdfxName))
                        {
                            QMessageBox::information(this, "HEXplorer", tr("file already in project"));
                            writeOutput("action read CDFX values : CDFX file already in project");
                            files.removeOne(fullCdfxName);
                            return;
                        }
                    }

                    // Read CSV files
                    QList<CdfxFile*> list;
                    foreach (QString fullCdfxName, files)
                    {
                        //update currentHexPath
                        QSettings settings(qApp->organizationName(), qApp->applicationName());
                        QString currentCdfxPath = QFileInfo(fullCdfxName).absolutePath();
                        settings.setValue("currentCdfxPath", currentCdfxPath);

                        //start a timer
                        QElapsedTimer timer;
                        timer.start();

                        //add the file to the project
                        CdfxFile *cdfx = new CdfxFile(fullCdfxName, wp, hex->getModuleName());
                        if (cdfx->isRead)
                        {
                            list.append(cdfx);

                            //stop timer
                            qint64 elapsedTime = timer.restart();
                            double _elapsedTime = static_cast<double>(elapsedTime)/1000;

                            writeOutput("read CDFX values : CDFX file read in " + QString::number(_elapsedTime, 'g',3) + " sec");
                        }
                        else
                        {
                             writeOutput("read CDFX values : CDFX file NOT read due to wrong format ");
                        }

                    }

                    // display CSV files
                    if (list.isEmpty())
                    {
                        return;
                    }
                    else
                    {
                        QStringList listLabel;
                        A2lTreeModel *mod = new A2lTreeModel();
                        DialogCsv *w = new DialogCsv(this, &listLabel);

                        foreach (CdfxFile *cdfx, list)
                        {
                            cdfx->addLabelsAsChild();
                            mod->addNode2RootNode(cdfx);
                        }
                        w->setModel(mod);
                        w->exec();

                        // copy the selected data
                        Node *root = mod->getRootNode();
                        foreach (QString str, listLabel)
                        {
                            Data *data1 = (Data*)root->getNode(str);
                            if (data1)
                            {
                                // get the data into destination container
                                Data* data2 = hex->getData(data1->name);

                                // if destination data exists perform copy
                                if (data2)
                                {
                                    data2->copyAllFrom(data1);
                                }
                            }
                        }

                        //delete cdfx files
                        delete mod;
                    }
                }
            }

        }
        else if (name.endsWith("SrecFile"))
        {
            int row = index.row();
            if ( row < 0)
            {
                QMessageBox::information(this,"HEXplorer","please first select an Srec file");
                writeOutput("action read CDFX file cancelled: no Srec file selected");
                return;
            }
            else
            {
                SrecFile *srec  = (SrecFile*)model->getNode(index);

                QSettings settings(qApp->organizationName(), qApp->applicationName());
                QString path = settings.value("currentCdfxPath").toString();

                QStringList files = QFileDialog::getOpenFileNames(this,
                                                  tr("select CDFX files"), path,
                                                  tr("CDFX files (*.cdfx);;all files (*.*)"));

                if (files.isEmpty())
                {
                   writeOutput("action read CDFX values : no CDFX file selected");
                   return;
                }
                else
                {
                    //Get the project (A2l) name
                    QString fullA2lName = model->name(model->parent(index));

                    //create a pointer on the WorkProject
                    WorkProject *wp = projectList->value(fullA2lName);

                    // if no MOD_COMMON in ASAP file
                    if (wp->a2lFile->getProject()->getNode("MODULE") == NULL)
                    {
                        QMessageBox::information(this, "HEXplorer", tr("no MOD_COMMON in ASAP file"));
                        writeOutput("action open new dataset : no MOD_COMMON in ASAP file");
                        return;
                    }

                    // check if Cdfx already in project
                    foreach (QString fullCdfxName, files)
                    {
                        //if the selected srec file is already into the project => exit
                        if (wp->csvFiles().contains(fullCdfxName))
                        {
                            QMessageBox::information(this, "HEXplorer", tr("file already in project"));
                            writeOutput("action read CDFX values : CDFX file already in project");
                            files.removeOne(fullCdfxName);
                            return;
                        }
                    }

                    // Read CSV files
                    QList<CdfxFile*> list;
                    foreach (QString fullCdfxName, files)
                    {
                        //update currentHexPath
                        QSettings settings(qApp->organizationName(), qApp->applicationName());
                        QString currentCdfxPath = QFileInfo(fullCdfxName).absolutePath();
                        settings.setValue("currentCdfxPath", currentCdfxPath);

                        //start a timer
                        QElapsedTimer timer;
                        timer.start();

                        //add the file to the project
                        CdfxFile *cdfx = new CdfxFile(fullCdfxName, wp, srec->getModuleName());
                        if (cdfx->isRead)
                        {
                            list.append(cdfx);

                            //stop timer
                            qint64 elapsedTime = timer.restart();
                            double _elapsedTime = static_cast<double>(elapsedTime)/1000;

                            writeOutput("read CDFX values : CDFX file read in " + QString::number(_elapsedTime, 'g', 3) + " sec");
                        }
                        else
                        {
                             writeOutput("read CDFX values : CDFX file NOT read due to wrong format ");
                        }

                    }

                    // display CSV files
                    if (list.isEmpty())
                    {
                        return;
                    }
                    else
                    {
                        QStringList listLabel;
                        A2lTreeModel *mod = new A2lTreeModel();
                        DialogCsv *w = new DialogCsv(this, &listLabel);

                        foreach (CdfxFile *cdfx, list)
                        {
                            cdfx->addLabelsAsChild();
                            mod->addNode2RootNode(cdfx);
                        }
                        w->setModel(mod);
                        w->exec();

                        // copy the selected data
                        Node *root = mod->getRootNode();
                        foreach (QString str, listLabel)
                        {
                            Data *data1 = (Data*)root->getNode(str);
                            if (data1)
                            {
                                // get the data into destination container
                                Data* data2 = srec->getData(data1->name);

                                // if destination data exists perform copy
                                if (data2)
                                {
                                    data2->copyAllFrom(data1);
                                }
                            }
                        }

                        //delete cdfx files
                        delete mod;
                    }
                }
            }
        }
        else
        {
            QMessageBox::warning(this, "HEXplorer::read cdfx values", "Please select first an Hex or an Srec file.",
                                             QMessageBox::Ok);
            return;
        }

    }
}

void MDImain::reset_AllChangedData()
{
    //get the index of the selected hex file
    QModelIndex index  = ui->treeView->selectionModel()->currentIndex();

    //get a pointer on the selected item
    Node *node =  model->getNode(index);

    QString name = typeid(*node).name();
    if (name.endsWith("HexFile"))
    {
        //As the selected node is an Hex file we can cast the node into its real type : HexFile
        HexFile *hex = dynamic_cast<HexFile *> (node);

        //reset all labels;
        hex->resetAllModifiedData();
    }
    else if (name.endsWith("SrecFile"))
    {
        //As the selected node is an Srec file we can cast the node into its real type : SrecFile
        SrecFile *srec = dynamic_cast<SrecFile *> (node);

        //reset all labels;
        srec->resetAllModifiedData();
    }
    else if (name.endsWith("Csv"))
    {
        //As the selected node is an Hex file we can cast the node into its real type : HexFile
        Csv *csv = dynamic_cast<Csv *> (node);

        //reset all labels;
        csv->resetAllModifiedData();
    }
    else if (name.endsWith("CdfxFile"))
    {
        //As the selected node is an Hex file we can cast the node into its real type : HexFile
        CdfxFile *cdfx = dynamic_cast<CdfxFile *> (node);

        //reset all labels;
        cdfx->resetAllModifiedData();
    }
    else if (name.endsWith("Dcm"))
    {
        //As the selected node is an Hex file we can cast the node into its real type : HexFile
        Dcm *dcm = dynamic_cast<Dcm *> (node);

        //reset all labels;
        dcm->resetAllModifiedData();
    }
    else
    {
        QMessageBox::warning(this, "HEXplorer::reset all changed data", "Please select first an hex or csv file.",
                                         QMessageBox::Ok);
        return;
    }

    //update all the table views
    updateView();
}

bool MDImain::save_File()
{
    QModelIndex index  = ui->treeView->selectionModel()->currentIndex();

    //get a pointer on the selected item
    Node *node =  model->getNode(index);

    QString name = typeid(*node).name();
    if (name.endsWith("HexFile"))
    {
        return save_HexFile(index);
    }
    else if (name.endsWith("SrecFile"))
    {
        return save_SrecFile(index);
    }
    else if (name.endsWith("Csv"))
    {
        return save_CsvFile(index);
    }
    else if (name.endsWith("CdfxFile"))
    {
        return save_CdfxFile(index);
    }
    else
    {
        QMessageBox::warning(this, "HEXplorer::save file", "Please select first an hex,csv or cdfx file.",
                                         QMessageBox::Ok);
        return false;
    }
}

bool MDImain::save_CsvFile(QModelIndex index)
{

    Csv *csv = (Csv*)model->getNode(index);

    int ret = QMessageBox::question(0, "HEXplorer :: save Csv file",
                          "overwrite " + csv->fullName() + " ?", QMessageBox::Yes, QMessageBox::Cancel);

    if (ret == QMessageBox::Yes)
    {
        return csv->save();
    }
    else
    {
        return false;
    }
}

bool MDImain::save_DcmFile(QModelIndex index)
{

    Dcm *dcm = (Dcm*)model->getNode(index);

    int ret = QMessageBox::question(0, "HEXplorer :: save Dcm file",
                          "overwrite " + dcm->fullName() + " ?", QMessageBox::Yes, QMessageBox::Cancel);

    if (ret == QMessageBox::Yes)
    {
        return dcm->save();
    }
    else
    {
        return false;
    }
}

bool MDImain::save_CdfxFile(QModelIndex index)
{

    CdfxFile *cdfx = (CdfxFile*)model->getNode(index);

    int ret = QMessageBox::question(0, "HEXplorer :: save Cdfx file",
                          "overwrite " + cdfx->fullName() + " ?", QMessageBox::Yes, QMessageBox::Cancel);

    if (ret == QMessageBox::Yes)
    {
        return cdfx->save();
    }
    else
    {
        return false;
    }
}

bool MDImain::save_HexFile(QModelIndex index)
{
    QElapsedTimer timer;
    timer.start();

    if (index.row() > -1)
    {

        HexFile *hex = (HexFile*)model->getNode(index);

        int ret = QMessageBox::question(0, "HEXplorer :: save Hex file",
                              "overwrite " + hex->fullName() + " ?", QMessageBox::Yes, QMessageBox::Cancel);

        if (ret == QMessageBox::Yes)
        {
            // display status bar
    //        statusBar()->show();
    //        progBar->reset();

            // write all
            QStringList list = hex->writeHex();

            if (list.isEmpty())
            {
                writeOutput("action save dataset : cancelled");
                return false;
            }

            QString fileName = hex->fullName();
            if (fileName.isEmpty())
                return false;

            QFile file(fileName);
            if (!file.open(QFile::WriteOnly))
            {
                QMessageBox::warning(this, tr("Application"),
                                     tr("Cannot write file %1:\n%2.")
                                     .arg(fileName)
                                     .arg(file.errorString()));
                return false;
            }


            // write into file
            QTextStream out(&file);
            QString str = list.join("\r\n");

            out << str << "\r\n";
    //        int i = progBar->value();
    //        int max = progBar->maximum();
    //        foreach (QString str, list)
    //        {
    //            out << str << "\r\n";
    //            i++;
    //            setValueProgressBar(i, max);
    //        }

            // hide the statusbar
    //        statusBar()->hide();
    //        progBar->reset();


            qint64 elapsedTime = timer.restart();
            double _elapsedTime = static_cast<double>(elapsedTime)/1000;

            writeOutput("action save dataset : performed with success in " + QString::number(_elapsedTime, 'g', 3) + " s");

            file.close();
            return true;
        }
        else
        {
            writeOutput("action save dataset : cancelled");
            return false;
        }
    }
    else
        return false;


}

bool MDImain::save_SrecFile(QModelIndex index)
{
    QElapsedTimer timer;
    timer.start();

    if (index.row() > -1)
    {
        SrecFile *srec = (SrecFile*)model->getNode(index);

        int ret = QMessageBox::question(0, "HEXplorer :: save Srec file",
                              "overwrite " + srec->fullName() + " ?", QMessageBox::Yes, QMessageBox::Cancel);

        if (ret == QMessageBox::Yes)
        {
            // display status bar
            //statusBar()->show();
            //progBar->reset();

            // write all
            QStringList list = srec->writeBlock2HexLines();

            if (list.isEmpty())
            {
                writeOutput("action save dataset : cancelled");
                return false;
            }

            QString fileName = srec->fullName();
            if (fileName.isEmpty())
                return false;

            QFile file(fileName);
            if (!file.open(QFile::WriteOnly))
            {
                QMessageBox::warning(this, tr("Application"),
                                     tr("Cannot write file %1:\n%2.")
                                     .arg(fileName)
                                     .arg(file.errorString()));
                return false;
            }

            QApplication::setOverrideCursor(Qt::WaitCursor);

            // write into file
            QTextStream out(&file);
            //int i = progBar->value();
            //int max = progBar->maximum();
            QString str = list.join("\r\n");
            out << str << "\r\n";

            // foreach (QString str, list)
            // {
            //     out << str << "\r\n";
            //     i++;
            //     setValueProgressBar(i, max);
            // }

            // hide the statusbar
            //statusBar()->hide();
            //progBar->reset();

            QApplication::restoreOverrideCursor();

            qint64 elapsedTime = timer.restart();
            double _elapsedTime = static_cast<double>(elapsedTime)/1000;
            writeOutput("action save dataset : performed with success in " + QString::number(_elapsedTime, 'g', 3) + " s");

            file.close();
            return true;
        }
        else
        {
            writeOutput("action save dataset : cancelled");
            return false;
        }
    }
    else
    {
        return false;
    }
}

void MDImain::saveAs_File()
{
    QModelIndex index  = ui->treeView->selectionModel()->currentIndex();

    //get a pointer on the selected item
    Node *node =  model->getNode(index);

    QString name = typeid(*node).name();
    if (name.endsWith("HexFile"))
    {
        return saveAs_HexFile(index);
    }
    else if (name.endsWith("SrecFile"))
    {
        return saveAs_SrecFile(index);
    }
    else if (name.endsWith("Csv"))
    {
        return saveAs_CsvFile(index);
    }
    else if (name.endsWith("CdfxFile") || name.endsWith("Dcm") )
    {
        return saveAs_CdfxFile(index);
    }
    else
    {
        QMessageBox::warning(this, "HEXplorer::save as file", "Please select first an hex, Srec, Csv or or Cdfx file.",
                                         QMessageBox::Ok);
        return;
    }
}

void MDImain::saveAs_HexFile(QModelIndex index)
{
    // get the HexFile Node
    HexFile *orgHex = (HexFile*)model->getNode(index);

    // ask for new HexFile name
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    QFileInfo(orgHex->fullName()).fileName(),
                                                    QFileInfo(orgHex->fullName()).absolutePath(),
                                                    tr("hex files (*.hex);;all files (*.*)"));
    if (fileName.isEmpty())
    {
        writeOutput("action save dataset : cancelled");
        return;
    }
    if ( QFileInfo(fileName).suffix().toLower() != "hex" &&
         QFileInfo(fileName).suffix().toLower() != "hex_trimmed")
    {
        fileName.append(".hex");
    }

    //check if the file is already open in HEXplorer
    if (orgHex->getParentWp()->containsHex(fileName))
    {
        QMessageBox::warning(this, tr("Application"),
                             tr("file %1 is already open.\nFirst close the open version or enter another file name.")
                             .arg(fileName));
        return;
    }

    //copy the orginal file to a new one
    on_actionDuplicate_DataContainer_triggered(fileName);

    //remove the changed labels from org Hex
    orgHex->resetAllModifiedData();

    // log
    writeOutput("action save dataset : performed with success ");

}

void MDImain::saveAs_SrecFile(QModelIndex index)
{
    // get the HexFile Node
    SrecFile *orgSrec = (SrecFile*)model->getNode(index);

    // ask for new HexFile name
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    QFileInfo(orgSrec->fullName()).fileName(),
                                                    QFileInfo(orgSrec->fullName()).absolutePath(),
                                                    tr("Srec files (*.s19 | *.s32 | *.s37);;all files (*.*)"));
    if (fileName.isEmpty())
    {
        writeOutput("action save dataset : cancelled");
        return;
    }
    if ( QFileInfo(fileName).suffix().toLower() != "s19" &&
         QFileInfo(fileName).suffix().toLower() != "s32" &&
         QFileInfo(fileName).suffix().toLower() != "s37")
    {
        fileName.append(".s19");
    }

    //check if the file is already open in HEXplorer
    if (orgSrec->getParentWp()->containsSrec(fileName))
    {
        QMessageBox::warning(this, tr("Application"),
                             tr("file %1 is already open.\nFirst close the open version or enter another file name.")
                             .arg(fileName));
        return;
    }

    //copy the orginal file to a new one
    on_actionDuplicate_DataContainer_triggered(fileName);

    //save the dataset changes on disk file
    //save_SrecFile(newIndex);

    //remove the changed labels from org Srec
    orgSrec->resetAllModifiedData();

    // log
    writeOutput("action save dataset : performed with success ");
}

void MDImain::saveAs_CsvFile(QModelIndex index)
{
    // get the HexFile Node
    Csv *csv = (Csv*)model->getNode(index);

    // ask for new HexFile name
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    QFileInfo(csv->fullName()).fileName(),
                                                    QFileInfo(csv->fullName()).absolutePath(),
                                                    tr("CSV files (*.csv);;all files (*.*)"));
    if (fileName.isEmpty())
    {
        writeOutput("action save CSV file : cancelled");
        return;
    }
    if ( QFileInfo(fileName).suffix().toLower() != "csv")
    {
        fileName.append(".csv");
    }

    //check if the file is already open in HEXplorer
    if (csv->getParentWp()->containsCsv(fileName))
    {
        QMessageBox::warning(this, tr("Application"),
                             tr("file %1 is already open.\nFirst close the open version or enter another file name.")
                             .arg(fileName));
        return;
    }

    // write the file
    //QApplication::setOverrideCursor(Qt::WaitCursor);
    if (csv->save(fileName))
    {
        // update Node name with the new file name
        model->renameNode(index, QFileInfo(fileName).fileName());
        csv->setFullName(fileName);

        // log
        writeOutput("action save CSV file : performed with success ");
    }
    else
    {
        // log
        writeOutput("action save CSV file : not performed ");
    }

    //QApplication::restoreOverrideCursor();


}

void MDImain::saveAs_CdfxFile(QModelIndex index)
{
    //QString name = typeid(*model->getNode(index)).name();
    QString name;
    Node* _model = model->getNode(index);
    if (_model)
        name = typeid(*_model).name();
    else
    {
        return;
    }
    if (name.toLower().endsWith("cdfxfile"))
    {
        // get the HexFile Node
        CdfxFile *cdfx = (CdfxFile*)model->getNode(index);

        // ask for new HexFile name
        QString fileName = QFileDialog::getSaveFileName(this,
                                                        QFileInfo(cdfx->fullName()).fileName(),
                                                        QFileInfo(cdfx->fullName()).absolutePath(),
                                                        tr("CDFX files (*.cdfx);;all files (*.*)"));
        if (fileName.isEmpty())
        {
            writeOutput("action save CDFX file : cancelled");
            return;
        }
        if ( QFileInfo(fileName).suffix().toLower() != "cdfx")
        {
            fileName.append(".cdfx");
        }

        //check if the file is already open in HEXplorer
        if (cdfx->getParentWp()->containsCdfx(fileName))
        {
            QMessageBox::warning(this, tr("Application"),
                                 tr("file %1 is already open.\nFirst close the open version or enter another file name.")
                                 .arg(fileName));
            return;
        }

        // check if the new file can be written
        QFile file(fileName);
        if (!file.open(QFile::WriteOnly))
        {
            QMessageBox::warning(this, tr("Application"),
                                 tr("Cannot write file %1:\n%2.")
                                 .arg(fileName)
                                 .arg(file.errorString()));

            writeOutput("action save CDFX file : impossible because file not ready for writing");
            return;
        }

        // write the file
        QApplication::setOverrideCursor(Qt::WaitCursor);
        cdfx->save(fileName);
        QApplication::restoreOverrideCursor();

        // update Node name with the new file name
        model->renameNode(index, QFileInfo(fileName).fileName());
        cdfx->setFullName(fileName);

        // log
        writeOutput("action save as CDFX : performed with success ");
    }
}

void MDImain::compare_HexFile()
{
    //get hexFiles path
    QModelIndexList list = ui->treeView->selectionModel()->selectedIndexes();    

    if (list.count() == 2)
    {

        //get node names
        QString str1 = model->getFullNodeTreePath(list.at(0));
        QString str2 = model->getFullNodeTreePath(list.at(1));

        //create a new FormCompare
        FormCompare *fComp = on_actionCompare_dataset_triggered();
        fComp->setDataset1(str1);
        fComp->setDataset2(str2);
        fComp->on_compare_clicked();

        //set new FormCompare as activated
        ui->tabWidget->setCurrentWidget(fComp);
    }
}

void MDImain::quicklookFile()
{
    //get hexFiles path
    QModelIndex index  = ui->treeView->selectionModel()->currentIndex();

    QString str1 = model->getFullNodeTreePath(index);

     if (index.isValid())
    {
        //get a pointer on the selected item
        Node *node =  model->getNode(index);

        //open DataContainer for quicklook
        QString name = typeid(*node).name();

        //check if file and a2l are read
        if (name.endsWith("HexFile"))
        {
            //read hex file if not read
            HexFile *hex = dynamic_cast<HexFile*>(node);          
            if (!hex->isRead())
            {
                if (readHexFile(hex) == NULL)
                {
                    return;
                }
            }
        }
        else if (name.endsWith("SrecFile"))
        {
            //read hex file if not read
            SrecFile *srec = dynamic_cast<SrecFile*>(node);
            if (!srec->isRead())
            {
                if (readSrecFile(srec) == NULL)
                    return;
            }
        }


        if (name.endsWith("HexFile") || name.endsWith("SrecFile") ||
                name.endsWith("Csv") || name.endsWith("CdfxFile") ||
                name.endsWith("Dcm"))
        {

            //create a new FormCompare
            FormCompare *fComp = on_actionCompare_dataset_triggered();
            fComp->setDataset1(str1);

            //perform a quicklook
            fComp->on_quicklook_clicked();

            //set new FormCompare as activated
            ui->tabWidget->setCurrentWidget(fComp);
        }
        else
        {
            QMessageBox::warning(this, "HEXplorer::quicklook", "Please select first an hex,Srec Cdfx or csv file.",
                                             QMessageBox::Ok);
            return;
        }
    }
}

void MDImain::editChangedLabels()
{
    //get hexFiles path
    QModelIndex index  = ui->treeView->selectionModel()->currentIndex();

    //get a pointer on the selected item
    Node *node =  model->getNode(index);

    QString name = typeid(*node).name();
    if (name.endsWith("HexFile"))
    {
        HexFile *hex = (HexFile*)model->getNode(index);
        QString str1 = model->getFullNodeTreePath(index);

        //create a new FormCompare
        FormCompare *fComp = on_actionCompare_dataset_triggered();
        fComp->setDataset1(str1);

        // set the list to edit
        fComp->charList.clear();
        foreach(Data *data, hex->getModifiedData())
        {
            fComp->charList.append(data->name);
        }
        fComp->on_quicklook_clicked();

        //set new FormCompare as activated
        ui->tabWidget->setCurrentWidget(fComp);
    }
    else if (name.endsWith("SrecFile"))
    {
        SrecFile *srec = (SrecFile*)model->getNode(index);
        QString str1 = model->getFullNodeTreePath(index);

        //create a new FormCompare
        FormCompare *fComp = on_actionCompare_dataset_triggered();
        fComp->setDataset1(str1);

        // set the list to edit
        fComp->charList.clear();
        foreach(Data *data, srec->getModifiedData())
        {
            fComp->charList.append(data->name);
        }
        fComp->on_quicklook_clicked();

        //set new FormCompare as activated
        ui->tabWidget->setCurrentWidget(fComp);

    }
    else if (name.endsWith("Csv"))
    {
        Csv *csv = (Csv*)model->getNode(index);
        QString str1 = model->getFullNodeTreePath(index);

        //create a new FormCompare
        FormCompare *fComp = on_actionCompare_dataset_triggered();
        fComp->setDataset1(str1);

        // set the list to edit
        fComp->charList.clear();
        foreach(Data *data, csv->getModifiedData())
        {
            fComp->charList.append(data->name);
        }
        fComp->on_quicklook_clicked();

        //set new FormCompare as activated
        ui->tabWidget->setCurrentWidget(fComp);
    }
    else if (name.endsWith("CdfxFile"))
    {
        CdfxFile *cdfx = (CdfxFile*)model->getNode(index);
        QString str1 = model->getFullNodeTreePath(index);

        //create a new FormCompare
        FormCompare *fComp = on_actionCompare_dataset_triggered();
        fComp->setDataset1(str1);

        // set the list to edit
        fComp->charList.clear();
        foreach(Data *data, cdfx->getModifiedData())
        {
            fComp->charList.append(data->name);
        }
        fComp->on_quicklook_clicked();

        //set new FormCompare as activated
        ui->tabWidget->setCurrentWidget(fComp);

    }
    else if (name.endsWith("Dcm"))
    {
        Dcm *dcm = (Dcm*)model->getNode(index);
        QString str1 = model->getFullNodeTreePath(index);

        //create a new FormCompare
        FormCompare *fComp = on_actionCompare_dataset_triggered();
        fComp->setDataset1(str1);

        // set the list to edit
        fComp->charList.clear();
        foreach(Data *data, dcm->getModifiedData())
        {
            fComp->charList.append(data->name);
        }
        fComp->on_quicklook_clicked();

        //set new FormCompare as activated
        ui->tabWidget->setCurrentWidget(fComp);

    }
    else
    {
        QMessageBox::warning(this, "HEXplorer :: edit changed data", "Please select first an hex, csv or Cdfx file.",
                                         QMessageBox::Ok);
        return;
    }
}

void MDImain::fplotData()
{
    QModelIndexList listIndex = ui->treeView->selectionModel()->selectedIndexes();

    foreach (QModelIndex index, listIndex)
    {
        Data *data = (Data*)model->getNode(index);
        data->plot(this);
    }
}

void MDImain::edit()
{
    QModelIndexList listIndex = ui->treeView->selectionModel()->selectedIndexes();

    foreach (QModelIndex index, listIndex)
    {
        Data *data = (Data*)this->model->getNode(index);

        QMainWindow *win = new QMainWindow(0);
        SpreadsheetView *view = new SpreadsheetView(win);
        view->horizontalHeader()->setVisible(false);
        view->verticalHeader()->setVisible(false);
        SpTableModel *model = new SpTableModel();
        QList<Data*> *list = new QList<Data*>;
        list->append(data);
        model->setList(list);
        view->setModel(model);
        view->horizontalHeader()->setDefaultSectionSize(50);
        view->verticalHeader()->setDefaultSectionSize(18);
        view->setColumnWidth(0, 200);
        win->setCentralWidget(view);

        QString parentName = "";
        if (data->getHexParent())
            parentName = data->getHexParent()->name;
        else if (data->getSrecParent())
            parentName = data->getSrecParent()->name;
        else if (data->getCsvParent())
            parentName = data->getCsvParent()->name;
        else if (data->getCdfxParent())
            parentName = data->getCdfxParent()->name;
        else if (data->getDcmParent())
            parentName = data->getDcmParent()->name;
        win->setWindowTitle("HEXplorer :: " + QString(data->name) + " (" + parentName + ")");

        win->show();
        win->resize(600, 400);

    }
}

void MDImain::editCompare()
{
    QModelIndex index = ui->treeView_2->selectionModel()->selectedIndexes().at(0);

    //get the data name
    Node *node = (Node*)((A2lTreeModel*)ui->treeView_2->model())->getNode(index);

    myWidget = this->ui->tabWidget->currentWidget();
    QString name = typeid(*myWidget).name();
    if (name.toLower().endsWith("formcompare"))
    {
        FormCompare *fComp = (FormCompare*)myWidget;

        //get the DataContainer
        Data *data1 = 0;
        Data *data2 = 0;
        SrecFile *_srec1 = fComp->getSrec1();
        SrecFile *_srec2 = fComp->getSrec2();
        HexFile *hex1 = fComp->getHex1();
        HexFile *hex2 = fComp->getHex2();
        Csv *csv1 = fComp->getCsv1();
        Csv *csv2 = fComp->getCsv2();
        CdfxFile *cdfx1 = fComp->getCdf1();
        CdfxFile *cdfx2 = fComp->getCdf2();
        Dcm *dcm1 = fComp->getDcm1();
        Dcm *dcm2 = fComp->getDcm2();

        if (_srec1)
            data1 = _srec1->getData(node->name);
        else if (hex1)
            data1 = hex1->getData(node->name);
        else if (csv1)
            data1 = csv1->getData(node->name);
        else if (cdfx1)
            data1 = cdfx1->getData(node->name);
        else if (dcm1)
            data1 = dcm1->getData(node->name);

        if (_srec2)
            data2 = _srec2->getData(node->name);
        else if (hex2)
            data2 = hex2->getData(node->name);
        else if (csv2)
            data2 = csv2->getData(node->name);
        else if (cdfx2)
            data2 = cdfx2->getData(node->name);
        else if (dcm2)
            data2 = dcm2->getData(node->name);

        if (data1 && data2)
        {
            QMainWindow *win = new QMainWindow(0);
            SpreadsheetView *view = new SpreadsheetView(win);

            CompareModel *model = new CompareModel();
            QList<Data*> *list1 = new QList<Data*>;
            QList<Data*> *list2 = new QList<Data*>;
            list1->append(data1);
            list2->append(data2);
            model->setList(list1, list2);
            view->setModel(model);
            view->horizontalHeader()->setDefaultSectionSize(50);
            view->verticalHeader()->setDefaultSectionSize(18);
            view->setColumnWidth(0, 200);
            win->setCentralWidget(view);

            QString parentName1 = "";
            if (data1->getHexParent())
            {
                parentName1 = data1->getHexParent()->name;
            }
            else if (data1->getSrecParent())
            {
                parentName1 = data1->getSrecParent()->name;
            }
            else if (data1->getCsvParent())
            {
                parentName1 = data1->getCsvParent()->name;
            }
            else if (data1->getCdfxParent())
            {
                parentName1 = data1->getCdfxParent()->name;
            }

            QString parentName2 = "";
            if (data2->getHexParent())
            {
                parentName2 = data2->getHexParent()->name;
            }
            else if (data2->getSrecParent())
            {
                parentName2 = data2->getSrecParent()->name;
            }
            else if (data2->getCsvParent())
            {
                parentName2 = data2->getCsvParent()->name;
            }
            else if (data2->getCdfxParent())
            {
                parentName2 = data2->getCdfxParent()->name;
            }
            win->setWindowTitle("HEXplorer :: " + parentName1 + " vs " + parentName2);

            win->show();
            win->resize(600, 400);
        }
    }

}

FormCompare *MDImain::on_actionCompare_dataset_triggered()
{
    //create a new formCompare
    FormCompare *fComp = new FormCompare(model, ui->treeView, ui->treeView_2, this);

    //draw the tab in tabWidget
    QIcon icon;
    icon.addFile(":/icones/milky_loopHex.png");

    //add the widget into tabList
    QString nameTab = "quicklook (" + QString::number(compareTabs) + ")";
    tabList->insert(nameTab, fComp);
    ui->tabWidget->addTab(fComp, icon, nameTab);

    ui->Differentlabels_Dock->setVisible(true);

    compareTabs++;

    return fComp;
}

void MDImain::export_Subsets()
{
    // check if a project is selected in treeView
    QModelIndex index  = ui->treeView->selectionModel()->currentIndex();

    if (index.isValid())
    {
        //get a pointer on the selected item
        Node *node =  model->getNode(index);
        QString name = typeid(*node).name();

        if (name.endsWith("HexFile"))
        {
            HexFile *hex  = (HexFile*)model->getNode(index);
            A2LFILE *a2l = hex->getParentWp()->a2lFile;

            //choose subset(s)
            QStringList subsetList;
            ChooseSubset *chooseSubset = new ChooseSubset(a2l, hex, subsetList, this);
            chooseSubset->exec();

            if (subsetList.isEmpty())
                return;
            else
            {
                //choose format
                QString exportFormat = "";
                DialogChooseExportFormat *chooseFormat = new DialogChooseExportFormat(&exportFormat, this);
                chooseFormat->exec();

                // display status bar
                statusBar()->show();
                progBar->reset();
                connect(hex, SIGNAL(lineParsed(int,int)), this, SLOT(setValueProgressBar(int,int)), Qt::DirectConnection);

                //export
                if (exportFormat == "csv")
                    hex->exportSubsetList2Csv(subsetList);
                else if (exportFormat == "cdf")
                    hex->exportSubsetList2Cdf(subsetList);


                // hide the statusbar
                statusBar()->hide();
                progBar->reset();
            }
        }
        else if (name.endsWith("SrecFile"))
        {
            SrecFile *srec  = (SrecFile*)model->getNode(index);
            A2LFILE *a2l = srec->getParentWp()->a2lFile;

            //choose subset(s)
            QStringList subsetList;
            ChooseSubset *chooseSubset = new ChooseSubset(a2l, srec, subsetList, this);
            chooseSubset->exec();

            if (subsetList.isEmpty())
                return;
            else
            {
                //choose format
                QString exportFormat = "";
                DialogChooseExportFormat *chooseFormat = new DialogChooseExportFormat(&exportFormat, this);
                chooseFormat->exec();

                // display status bar
                statusBar()->show();
                progBar->reset();
                connect(srec, SIGNAL(lineParsed(int,int)), this, SLOT(setValueProgressBar(int,int)), Qt::DirectConnection);

                //export
                if (exportFormat == "csv")
                    srec->exportSubsetList2Csv(subsetList);
                else if (exportFormat == "cdf")
                    srec->exportSubsetList2Cdf(subsetList);


                // hide the statusbar
                statusBar()->hide();
                progBar->reset();
            }
        }

        else
        {

            QMessageBox::warning(this, "HEXplorer::export subsets from Hex file", "Please select first an hex file.",
                                             QMessageBox::Ok);
            return;


        }
    }
}

void MDImain::import_Subsets()
{
    // check if a project is selected in treeView
    QModelIndex index  = ui->treeView->selectionModel()->currentIndex();

    if (index.isValid())
    {
        //get a pointer on the selected item
        Node *node =  model->getNode(index);
        QString name = typeid(*node).name();

        if (!name.endsWith("HexFile"))
        {
            QMessageBox::warning(this, "HEXplorer::export subsets from Hex file", "Please select first an hex file.",
                                             QMessageBox::Ok);
            return;
        }
        else
        {
            HexFile *hex  = (HexFile*)model->getNode(index);

            QSettings settings(qApp->organizationName(), qApp->applicationName());
            QString path = settings.value("currentCsvPath").toString();

            QStringList files = QFileDialog::getOpenFileNames(this,
                                              tr("select Subset files"), path,
                                              tr("CSV files (*.csv);;Cdfx files(*.cdfx)"));

            if (files.isEmpty())
            {
               writeOutput("action open new Subset file : no subset file selected");
               return;
            }
            else
            {
                //Get the project (A2l) name
                QString fullA2lName = model->name(model->parent(index));

                //create a pointer on the WorkProject
                WorkProject *wp = projectList->value(fullA2lName);

                // if no MODULE in ASAP file
                if (wp->a2lFile->getProject()->getNode("MODULE") == NULL)
                {
                    QMessageBox::information(this, "HEXplorer", tr("no MODULE in ASAP file"));
                    writeOutput("action open new subset : no MODULE in ASAP file");
                    return;
                }

                // Read CSV files
                QMap<QString, Csv*> list;
                foreach (QString fullCsvName, files)
                {
                    //update currentHexPath
                    QSettings settings(qApp->organizationName(), qApp->applicationName());
                    QString currentCsvPath = QFileInfo(fullCsvName).absolutePath();
                    settings.setValue("currentCsvPath", currentCsvPath);

                    //start a timer
                    QElapsedTimer timer;
                    timer.start();

                    //add the file to the project
                    Csv *csv = new Csv(fullCsvName, wp, hex->getModuleName());

                    // display status bar
                    statusBar()->show();
                    progBar->reset();
                    connect(csv, SIGNAL(incProgressBar(int,int)), this, SLOT(setValueProgressBar(int,int)), Qt::DirectConnection);

                    if (csv->readFile())
                    {
                        QString nameCsv = QFileInfo(fullCsvName).baseName();
                        QStringList listName = nameCsv.split("_");
                        list.insert(listName.at(0), csv);

                        //stop timer
                        qint64 elapsedTime = timer.restart();
                        double _elapsedTime = static_cast<double>(elapsedTime)/1000;

                        writeOutput("action open new CSV file : CSV file add to project in " + QString::number(_elapsedTime, 'g', 3) + " sec");
                    }
                    else
                    {
                         writeOutput("action open new CSV file : CSV file NOT added to project due to wrong format ");
                    }

                    // hide the statusbar
                    statusBar()->hide();
                    progBar->reset();
                }

                // copy subset
                if (list.isEmpty())
                {
                    return;
                }
                else
                {
                    // copy subset if compatible
                    foreach (QString str, list.keys())
                    {
                       // Csv *csv = list.value(str);

                       // bool missingLabels = false;
                       // bool newLabels = false;
                    }

                }
            }
        }
    }
}

void MDImain::editInHDDrive()
{
    //get hexFiles path
    QModelIndex index  = ui->treeView->selectionModel()->currentIndex();
    Node *node = model->getNode(index);
    QString name = typeid(*node).name();

    QString PathToReveal = "";
    if (name.endsWith("HexFile"))
    {
        HexFile *hex = dynamic_cast<HexFile*>(node);
        PathToReveal = hex->fullName();
    }
    else if (name.endsWith("SrecFile"))
    {
        SrecFile *srec = dynamic_cast<SrecFile*>(node);
        PathToReveal = srec->fullName();
    }

    // Mac, Windows support folder or file.
#if defined(Q_OS_WIN)
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QString windirstr = env.value("WINDIR");
    QDir windir = QDir(windirstr);
    QStringList listWindDirEntries = windir.entryList();
    if (listWindDirEntries.contains("explorer.exe"))
    {
        QString explorer = windirstr + "\\explorer.exe";
        //QString param = "";
        //if (!QFileInfo(PathToReveal).isDir())
        //    param = QLatin1String("/select,");
        //param += QDir::toNativeSeparators(PathToReveal);
        //QString command = explorer + " " + param;
        //QProcess::startDetached(param);
        QStringList params;
        params.append("/select,");
        params.append(QDir::toNativeSeparators(PathToReveal));
        QProcess::startDetached(explorer, params);

    }
    else
    {
        QMessageBox::warning(this,
                             tr("Launching Windows Explorer failed"),
                             tr("Could not find explorer.exe in path to launch Windows Explorer."));
        return;

    }
#elif defined(Q_OS_MAC)
    QStringList scriptArgs;
    scriptArgs << QLatin1String("-e")
            << QString::fromLatin1("tell application \"Finder\" to reveal POSIX file \"%1\"").arg(PathToReveal);
    QProcess::execute(QLatin1String("/usr/bin/osascript"), scriptArgs);
    scriptArgs.clear();
    scriptArgs << QLatin1String("-e")
            << QLatin1String("tell application \"Finder\" to activate");
    QProcess::execute("/usr/bin/osascript", scriptArgs);
#endif

}

QModelIndex  MDImain::on_actionDuplicate_DataContainer_triggered(QString fullFileName)
{
    //get the index of the selected hex file
    QModelIndex index  = ui->treeView->selectionModel()->currentIndex();

    //get a pointer on the selected item
    Node *node =  model->getNode(index);
    QString name = typeid(*node).name();
    Node* nodeParent = node->getParentNode();

    if (name.toLower().endsWith("hexfile"))
    {

        //As the selected node is an Hex file we can cast the node into its real type : HexFile
        HexFile *orgHex = dynamic_cast<HexFile *> (node);

        //get the parentWp of the HexFile
        WorkProject *wp = orgHex->getParentWp();

        // copy the file
        QString orgName = orgHex->fullName();
        QString baseOrgName = QFileInfo(orgName).baseName();
        QString baseName = baseOrgName;
        QString newName = orgName;
        if (fullFileName.isEmpty())
        {
            int i = 1;
            bool check = true;
            while (check)
            {
                newName = newName.replace(baseName, baseOrgName + " (" + QString::number(i) + ")");
                baseName = QFileInfo(newName).baseName();
                check = QFile(newName).exists();
                i++;
            }
        }
        else
        {
            newName = fullFileName;
        }

        if (QFile::copy(orgHex->fullName(), newName ))
        {
            HexFile *cloneHex = new HexFile(newName, wp);
            wp->addHex(cloneHex, nodeParent);

            //if changes in orgSrec => copy changes to cloneSrec
            if (!orgHex->childNodes.isEmpty())
            {
                //read file
                HexFile* hex= readHexFile(cloneHex);

                //get source and destination dataList and copy
                QList<Data*> listCopySrc = orgHex->getModifiedData();
                foreach (Data* dataSrc, listCopySrc)
                {
                    Data *dataTrg = hex->getData(dataSrc->getName());
                    if (dataTrg)
                    {
                        dataTrg->copyAllFrom(dataSrc);
                    }
                }

                writeOutput("action duplicate file : performed with success");

                return model->getIndex(hex);
            }
            else
            {
                //read file
                //HexFile* hex = readHexFile(cloneHex);

                writeOutput("action duplicate file : performed with success");
                 return QModelIndex();
            }
        }
        else
        {
            writeOutput("action duplicate file : not possible");
            return QModelIndex();
        }

    }
    else if (name.toLower().endsWith("srecfile"))
    {
        //As the selected node is an Hex file we can cast the node into its real type : HexFile
        SrecFile *orgSrec = dynamic_cast<SrecFile *> (node);

        //get the parentWp of the HexFile
        WorkProject *wp = orgSrec->getParentWp();

        // copy the file
        QString orgName = orgSrec->fullName();
        QString baseOrgName = QFileInfo(orgName).baseName();
        QString baseName = baseOrgName;
        QString newName = orgName;
        if (fullFileName.isEmpty())
        {
            int i = 1;
            bool check = true;
            while (check)
            {
                newName = newName.replace(baseName, baseOrgName + " (" + QString::number(i) + ")");
                baseName = QFileInfo(newName).baseName();
                check = QFile(newName).exists();
                i++;
            }
        }
        else
        {
            newName = fullFileName;
        }

        if (QFile::copy(orgSrec->fullName(), newName ))
        {
            SrecFile *cloneSrec = new SrecFile(newName, wp);
            wp->addSrec(cloneSrec, nodeParent);

            //if changes in orgSrec => copy changes to cloneSrec
            if (!orgSrec->childNodes.isEmpty())
            {
                //read file
                SrecFile* srec= readSrecFile(cloneSrec);

                //get source and destination dataList and copy
                QList<Data*> listCopySrc = orgSrec->getModifiedData();
                foreach (Data* dataSrc, listCopySrc)
                {
                    Data *dataTrg = srec->getData(dataSrc->getName());
                    if (dataTrg)
                    {
                        dataTrg->copyAllFrom(dataSrc);
                    }
                }

                writeOutput("action duplicate file : performed with success");
                return model->getIndex(srec);
            }
            else
            {
                //read file
                //SrecFile* srec= readSrecFile(cloneSrec);

                writeOutput("action duplicate file : performed with success");
                return QModelIndex();
            }
        }
        else
        {
            writeOutput("action duplicate file : not possible");
            return QModelIndex();
        }

    }
    else
    {
        return QModelIndex();
    }

}

void MDImain::on_actionRename_file_triggered()
{

    QModelIndex index  = ui->treeView->selectionModel()->currentIndex();
    ui->treeView->edit(index);

}

void MDImain::onCopyDataset()
{
    //remove previous node from hex/srec owner list
    if (nodeClipBoard)
    {
        QString name = typeid(*nodeClipBoard).name();
        if (name.toLower().endsWith("hexfile"))
        {
           HexFile *hex = dynamic_cast<HexFile *> (nodeClipBoard);
           hex->detach(this);

        }
        else if(name.toLower().endsWith("srecfile"))
        {
            SrecFile *srec = dynamic_cast<SrecFile *> (nodeClipBoard);
            srec->detach(this);

        }
    }

    //get the dataset information in Clipboard
    indexClipBoard  = ui->treeView->selectionModel()->currentIndex();
    nodeClipBoard =  model->getNode(indexClipBoard);
    QString name = typeid(*nodeClipBoard).name();
    if (name.toLower().endsWith("hexfile"))
    {
         HexFile *hex = dynamic_cast<HexFile *> (nodeClipBoard);
         pathClipBoard = hex->fullName();
         hex->attach(this);


    }
    else if(name.toLower().endsWith("srecfile"))
    {
         SrecFile *srec = dynamic_cast<SrecFile *> (nodeClipBoard);
         pathClipBoard = srec->fullName();
         srec->attach(this);
    }
}

void MDImain::onPasteDataset()
{
    if (!indexClipBoard.isValid()) //index not valid.
    {
        return;
    }
    else if (nodeClipBoard == nullptr || nodeClipBoard == model->getRootNode()) //pointer null
    {
        return;
    }
    else if (!QFile(pathClipBoard).exists()) //file is deleted.
    {
        QMessageBox::warning(this, "HEXplorer :: Paste action.",
                             "Could not find this item.\n"
                             "Dataset with path " + pathClipBoard + " is no longer available on disk.");

        return;
    }
    else
    {
        QModelIndex index  = ui->treeView->selectionModel()->currentIndex();
        Node *nodePaste =  model->getNode(index);
        Node *nodePasteParent = nodePaste->getParentNode();
        QString nameNodePaste = typeid(*nodePaste).name();
        QString newPath = "";
        if (nameNodePaste.toLower().endsWith("hexfile") || nameNodePaste.toLower().endsWith("srecfile") )
        {
            newPath = QFileInfo(nodePaste->fullName()).path();
        }
        else if (nameNodePaste.toLower().endsWith("treedirectory"))
        {
            TreeDirectory *dir = static_cast<TreeDirectory*>(nodePaste);
            newPath = dir->getPath();
            nodePasteParent = nodePaste;
        }

        QString name = typeid(*nodeClipBoard).name();

        if (name.toLower().endsWith("hexfile"))
        {
            //As the selected node is an Hex file we can cast the node into its real type : HexFile
            HexFile *orgHex = dynamic_cast<HexFile *> (nodeClipBoard);

            //get the parentWp of the HexFile
            WorkProject *wp = orgHex->getParentWp();

            // copy the file
            QString orgName = orgHex->fullName();
            QString extension = QFileInfo(orgName).suffix();
            QString baseOrgName = QFileInfo(orgName).baseName();
            QString baseName = baseOrgName;
            QString newName = orgName;
            int i = 1;
            bool check = true;
            while (check)
            {
                //newName = newName.replace(baseName, baseOrgName + " (" + QString::number(i) + ")");
                newName = newPath + "/" + baseOrgName + " (" + QString::number(i) + ")." + extension;
                baseName = QFileInfo(newName).baseName();
                check = QFile(newName).exists();
                i++;
            }


            if (QFile::copy(orgHex->fullName(), newName ))
            {
                HexFile *cloneHex = new HexFile(newName, wp);
                wp->addHex(cloneHex, nodePasteParent);

                //if changes in orgSrec => copy changes to cloneSrec
                if (!orgHex->childNodes.isEmpty())
                {
                    //read file
                    HexFile* hex= readHexFile(cloneHex);

                    //get source and destination dataList and copy
                    QList<Data*> listCopySrc = orgHex->getModifiedData();
                    foreach (Data* dataSrc, listCopySrc)
                    {
                        Data *dataTrg = hex->getData(dataSrc->getName());
                        if (dataTrg)
                        {
                            dataTrg->copyAllFrom(dataSrc);
                        }
                    }

                    writeOutput("action paste file : performed with success");

                    return;
                }
                else
                    return;
            }
            else
            {
                writeOutput("action paste file : not possible");
                return;
            }

        }
        else if (name.toLower().endsWith("srecfile"))
        {
            //As the selected node is an Hex file we can cast the node into its real type : HexFile
            SrecFile *orgSrec = dynamic_cast<SrecFile *> (nodeClipBoard);

            //get the parentWp of the HexFile
            WorkProject *wp = orgSrec->getParentWp();

            // copy the file
            QString orgName = orgSrec->fullName();
            QString extension = QFileInfo(orgName).suffix();
            QString baseOrgName = QFileInfo(orgName).baseName();
            QString baseName = baseOrgName;
            QString newName = orgName;
            int i = 1;
            bool check = true;
            while (check)
            {
                //newName = newName.replace(baseName, baseOrgName + " (" + QString::number(i) + ")");
                newName = newPath + "/" + baseOrgName + " (" + QString::number(i) + ")." + extension ;
                baseName = QFileInfo(newName).baseName();
                check = QFile(newName).exists();
                i++;
            }

            if (QFile::copy(orgSrec->fullName(), newName ))
            {
                SrecFile *cloneSrec = new SrecFile(newName, wp);
                wp->addSrec(cloneSrec, nodePasteParent);

                //if changes in orgSrec => copy changes to cloneSrec
                if (!orgSrec->childNodes.isEmpty())
                {
                    //read file
                    SrecFile* srec= readSrecFile(cloneSrec);

                    //get source and destination dataList and copy
                    QList<Data*> listCopySrc = orgSrec->getModifiedData();
                    foreach (Data* dataSrc, listCopySrc)
                    {
                        Data *dataTrg = srec->getData(dataSrc->getName());
                        if (dataTrg)
                        {
                            dataTrg->copyAllFrom(dataSrc);
                        }
                    }

                    writeOutput("action paste file : performed with success");
                    return;
                }
                else
                {
                    writeOutput("action paste file : performed with success");
                    return;
                }
            }
            else
            {
                writeOutput("action paste file : not possible");
                return;
            }

        }
    }
}

void MDImain::onCreateDirectory()
{
    QModelIndex index  = ui->treeView->selectionModel()->currentIndex();
    Node *wp =  model->getNode(index);
    QString name = typeid(*wp).name();
    QString dirPath = "";
    if (name.toLower().endsWith("workproject"))
    {
        dirPath = QFileInfo(wp->fullName()).path();
    }
    else
        return;

    QString dirName = "new Dir";
    QDir dir(dirPath);
    int i = 1;
    while (dir.entryList().contains(dirName))
    {
        dirName = dirName + " (" + QString::number(i) + ")";
        i++;
    }
    bool bl = dir.mkdir(dirName);
    if (!bl)
    {
        QMessageBox::warning(this, "HEXplorer::newDirectory",
                             "HEXplorer could not create the new Directory.");
    }
    else
    {
        TreeDirectory *subDir = new TreeDirectory(dirName);
        subDir->setPath(dirPath);

        subDir->setParentNode(wp);
        wp->addChildNode(subDir);
        wp->sortChildrensName();
        model->dataInserted(wp, wp->childNodes.indexOf(subDir));

        ui->treeView->expand(index);
        QModelIndex indexSubDir = model->getIndex(subDir);
        ui->treeView->edit(indexSubDir);
    }

}

//-------------------- TabList ---------------------//

void MDImain::removeTab( int index )
{
    // get the name of the tab
    QString _name = ui->tabWidget->tabText(index);

   //remove tab from tabWidget

    ui->tabWidget->removeTab(index);

    //delete the widget
    //if (_name.contains("quicklook"))
    //    compareTabs--;

    //delete QScintilla pointer
    delete tabList->value(_name);

    //remove widget from tabList
    tabList->remove(_name);

    if (tabList->count() == 0)
    {
        ui->Differentlabels_Dock->hide();
    }
}

void MDImain::resetAllTableView()
{
    updateView();
}

void MDImain::updateView()
{
    foreach(QWidget *widget, tabList->values())
    {
        QString name = typeid(*widget).name();
        if (name.toLower().endsWith("formcompare"))
        {
            FormCompare *fcomp = (FormCompare*)widget;
            fcomp->resetModel();
        }
    }
}

void MDImain::writeOutput(QString str)
{
    ui->listWidget->addItem(str);
    ui->listWidget->scrollToBottom();
}

void MDImain::writeOutput(QStringList list)
{
    ui->listWidget->addItems(list);
    ui->listWidget->scrollToBottom();
}

void MDImain::nodeSelected()
{
    //QModelIndex index  = ui->treeView->selectionModel()->currentIndex();

    //int line = model->line(index).toInt() -1;
    //int length = editor->lineLength(line);
    //editor->setCursorPosition(line, 0);

    //editor->setSelection(line , 0, line , length - 1);
}

void MDImain::resizeColumn0()
{
    ui->treeView->resizeColumnToContents(0);
}

void MDImain::tabWidget_currentChanged(int )
{
    myWidget = this->ui->tabWidget->currentWidget();
    if (myWidget)
    {
        QString name = typeid(*myWidget).name();
        if (name.toLower().endsWith("formcompare"))
        {
            FormCompare *fComp = (FormCompare*)myWidget;
            if (fComp->getDiffModel())
            {
                ui->treeView_2->setModel(fComp->getDiffModel());
                ui->treeView_2->setColumnHidden(1, true);
            }
            else
            {
                ui->treeView_2->setModel(0);
            }
        }
        else
        {
            ui->treeView_2->setModel(0);
        }
    }
}

void MDImain::clear_Output()
{
    ui->listWidget->clear();
}

bool MDImain::FileExists(QString strFilename)
{
    struct stat stFileInfo;
    bool blnReturn;
    int intStat;

    // Attempt to get the file attributes
    intStat = stat(strFilename.toStdString().c_str(),&stFileInfo);
    if(intStat == 0)
    {
    blnReturn = true;
    }
    else
    {
    blnReturn = false;
    }

    return blnReturn;
}

void MDImain::setCurrentFile(const QString &fileName)
{
    curFile = fileName;
    setWindowFilePath(curFile);

    QSettings settings(qApp->organizationName(), qApp->applicationName());
    QStringList files = settings.value("recentFileList").toStringList();
    files.removeAll(fileName);
    files.prepend(fileName);
    while (files.size() > MaxRecentFiles)
        files.removeLast();

    settings.setValue("recentFileList", files);

    foreach (QWidget *widget, QApplication::topLevelWidgets()) {
        MDImain *mainWin = qobject_cast<MDImain *>(widget);
        if (mainWin)
            mainWin->updateRecentFileActions();
    }
}

void MDImain::updateRecentFileActions()
{
    QSettings settings(qApp->organizationName(), qApp->applicationName());
    QStringList files = settings.value("recentFileList").toStringList();

    int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);

    for (int i = 0; i < numRecentFiles; ++i) {
        QString text = tr("&%1 %2").arg(i + 1).arg(files[i]);
        recentFileActs[i]->setText(text);
        recentFileActs[i]->setData(files[i]);
        recentFileActs[i]->setVisible(true);
    }
    for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
        recentFileActs[j]->setVisible(false);

    //separatorAct->setVisible(numRecentFiles > 0);
}

QString MDImain::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}


void MDImain::on_actionCheck_for_updates_triggered()
{
    QString url = "https://nexus.lmb.liebherr.i/repository/raw-pd1/HEXplorer_repo/Updates.xml";
    OnlineUpdater updater(url, this, true);

}

void MDImain::initCheckHttpUpdates()
{

   QUrl url("https://nexus.lmb.liebherr.i/repository/raw-pd1/HEXplorer_repo/Updates.xml");
   OnlineUpdater updater(url, this, false);
}

void MDImain::expandNode(Node *node)
{
    QModelIndex index = model->getIndex(node);
    ui->treeView->expand(index);
}

void MDImain::checkDroppedFile(QString str)
{
    Node *rootNode = model->getRootNode();
    Node *node = rootNode->getNode(str);

    if (node == NULL)
    {
        QMessageBox::information(this,"HEXplorer::drop file","please drop an Hex or Csv file");
        return;
    }

    QString name = typeid(*node).name();

    // attach this to the new dropped hex file
    if (name.toLower().endsWith("hexfile"))
    {
        //check if hex is read
        HexFile* hex = dynamic_cast<HexFile*>(node);
        if (!hex->isRead())
        {
            HexFile* h = readHexFile(hex);
            //exit if a2l and hex are not combined() otherwise crashes
            if (!h->isRead())
                return;
         }


        //create a new FormCompare
        FormCompare *fComp = on_actionCompare_dataset_triggered();
        fComp->setDataset1(str);
        fComp->on_quicklook_clicked();

        //set new FormCompare as activated
        ui->tabWidget->setCurrentWidget(fComp);

    }
    else if (name.toLower().endsWith("srecfile"))
    {
        //check if srec is read
        SrecFile* srec = dynamic_cast<SrecFile*>(node);
        if (!srec->isRead())
        {
           SrecFile* s19 = readSrecFile(srec);
           //exit if a2l and srec are not combined() otherwise crashes
           if (!s19->isRead())
               return;
        }

        //create a new FormCompare
        FormCompare *fComp = on_actionCompare_dataset_triggered();
        fComp->setDataset1(str);
        fComp->on_quicklook_clicked();

        //set new FormCompare as activated
        ui->tabWidget->setCurrentWidget(fComp);

    }
    else if(name.toLower().endsWith("csv"))
    {
        //create a new FormCompare
        FormCompare *fComp = on_actionCompare_dataset_triggered();
        fComp->setDataset1(str);
        fComp->on_quicklook_clicked();

        //set new FormCompare as activated
        ui->tabWidget->setCurrentWidget(fComp);
    }
    else if(name.toLower().endsWith("workproject"))
    {
        //create a textEditor
        A2LFILE *a2l = (A2LFILE*)node;
        QFile qfile(a2l->fullName());
        editTextFile(qfile);
    }
    else if (name.toLower().endsWith("cdfxfile"))
    {
        //create a new FormCompare
        FormCompare *fComp = on_actionCompare_dataset_triggered();
        fComp->setDataset1(str);
        fComp->on_quicklook_clicked();

        //set new FormCompare as activated
        ui->tabWidget->setCurrentWidget(fComp);
    }
    else if (name.toLower().endsWith("dcm"))
    {
        //create a new FormCompare
        FormCompare *fComp = on_actionCompare_dataset_triggered();
        fComp->setDataset1(str);
        fComp->on_quicklook_clicked();

        //set new FormCompare as activated
        ui->tabWidget->setCurrentWidget(fComp);
    }
    else
    {
        QMessageBox::information(this,"HEXplorer::drop file","please drop an A2l,Hex , Srec, Dcm or Csv file");
    }
}

void MDImain::setValueProgressBar(int n, int max)
{
    qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    progBar->setMaximum(max);
    progBar->setValue(n);
}

QString MDImain::getUserName()
{
#ifdef WIN32
    TCHAR userName[MAX_PATH];
    DWORD userNameSize = sizeof(userName) / sizeof(userName[0]);
    if (GetUserName(userName, &userNameSize))
    {
        QNetworkAccessManager manager;
        QUrl url("https://nexus.lmb.liebherr.i/repository/raw-pd1/HEXplorer_repo/users.json");
        readJsonFile(&manager, url, QString::fromWCharArray(userName));
        qDebug() << "Windows Username:" << QString::fromWCharArray(userName);
        return QString::fromWCharArray(userName);
    } else
    {
        qDebug() << "Failed to retrieve Windows username";
        return "";
    }
#else
    return "no username";
#endif

}

void MDImain::readJsonFile(QNetworkAccessManager* manager, const QUrl& url, QString user)
{
    QNetworkRequest request(url);
    QNetworkReply* reply = manager->get(request);

    // Use QEventLoop to wait for the network request to complete
    QEventLoop eventLoop;
    QObject::connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
    eventLoop.exec();  // This will block until the reply is finished

    if (reply->error() == QNetworkReply::NoError)
    {
        QByteArray response = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
        if (!jsonDoc.isNull())
        {
            QJsonObject jsonObj = jsonDoc.object();

            QString key = user;

            if (jsonObj.contains(key) && jsonObj[key].isDouble())
            {
                // If the key exists and the value is a number, increment it
                int currentValue = jsonObj[key].toInt();
                jsonObj[key] = currentValue + 1;
            }
            else
            {
                // If the key does not exist, create it with value 1
                jsonObj[key] = 1;
            }

            // Convert the modified JSON object back to a QJsonDocument
            QJsonDocument modifiedJsonDoc(jsonObj);
            QByteArray modifiedJsonData = modifiedJsonDoc.toJson();

            // Save the modified JSON back to the server
            QNetworkRequest putRequest(reply->url());
            putRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
            QString concatenated = "px9TApa9:5CyCZuBf_-fXkYQY6XzQqeQJ2jN8CZeWJ88pSTkEyM0h";
            QByteArray data = concatenated.toLocal8Bit().toBase64();
            QString headerData = "Basic " + data;
            putRequest.setRawHeader("Authorization", headerData.toLocal8Bit());

            QNetworkReply* putReply = reply->manager()->put(putRequest, modifiedJsonData);

            // Use QEventLoop to wait for the network request to complete
            QEventLoop eventLoopPut;
            QObject::connect(putReply, &QNetworkReply::finished, &eventLoopPut, &QEventLoop::quit);
            eventLoopPut.exec();  // This will block until the reply is finished

            if (putReply->error() == QNetworkReply::NoError)
            {
                qDebug() << "JSON successfully saved to the server.";
            } else
            {
                qDebug() << "Error saving JSON to the server:" << putReply->errorString();
            }

            putReply->deleteLater();

        }
        else
        {
            qDebug() << "Failed to parse JSON.";
        }
    }
    else
    {
        qDebug() << "Error reading JSON from the server:" << reply->errorString();
    }
    reply->deleteLater();
}


//-------------------- Dark side ---------------------//

void MDImain::on_actionback_to_the_white_side_of_the_moon_triggered()
{
    this->setStyle("white");
}

void MDImain::on_actiondark_theme_triggered()
{
    this->setStyle("dark");
}

