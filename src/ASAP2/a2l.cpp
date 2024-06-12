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

#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QtAlgorithms>
#include <QTime>
#include <QMap>
#include <QFileInfo>
#include <QtConcurrent/QtConcurrent>
#include <QFutureWatcher>
#include <QProgressDialog>
#include <QTextEdit>

#include "a2l.h"
#include "hexfile.h"
#include "lexer.h"
#include "node.h"
#include "item.h"
#include "qapplication.h"
#include <string>

#include <QElapsedTimer>
#include <QFileInfo>
#include <QDebug>


//class A2l file
A2l::A2l(QString fullFileName, QObject *parent): QObject(parent),
    progressDialog(nullptr), totalProgress(0), remainingThreads(0)
{
    fullA2lName = fullFileName;
    a2lFile = 0;
    dbFile = 0;
    progressVal = 0;
    progBarMaxValue = 0;
    omp_init_lock(&lockValue);
    is_Ok = true;
}

A2l::~A2l()
{
    //delete a2lFile;
    omp_destroy_lock(&lockValue);

    // Ensure all threads are properly finished
    for (QThread *thread : threads) {
        thread->quit();
        thread->wait();
        delete thread;
    }

    // Delete any remaining workers
    qDeleteAll(workers);
    workers.clear();
}

std::string A2l::getFullA2lFileName()
{
    return fullA2lName.toLocal8Bit().data();
}

void A2l::setFullA2lFileName(std::string str)
{
    fullA2lName = QString(str.c_str());
}

void A2l::setFullA2lFileName(QString str)
{
    fullA2lName = str;
}

void A2l::init()
{
    A2LFILE *nodeA2l = new A2LFILE(fullA2lName);
    int size = (QFileInfo(fullA2lName).fileName()).toLocal8Bit().size() + 1;
    nodeA2l->name = new char[size];
    strcpy_s(nodeA2l->name, size, QFileInfo(fullA2lName).fileName().toLocal8Bit().data());
    a2lFile = nodeA2l;
}

void A2l::parse()
{
    //start a timer
    QElapsedTimer timer;
    timer.start();

    //clear the "console"
    outputList.clear();

    //parse in 1 or 2 threads based on processor architecture
    bool myDebug = 0;
    #ifdef MY_DEBUG
        myDebug = 0;
    #endif
    QString multiThread = "";
    QString lexerType = "";
    QSettings settings(qApp->organizationName(), qApp->applicationName());

    if (omp_get_num_procs() > 1 && !myDebug && settings.value("openMP") == true)
    {
        //parseOpenMPA2l();
        //multiThread = "openMP";

        //int numThreads = QThread::idealThreadCount();
        multiThread = "QThread";
        parseQThreadA2l(2);
    }
    else
    {
        parseSTA2l();
    }

    //test if parsing could be achieved otherwise return;
    if (a2lFile == 0)
        return;
    else
    {
        QElapsedTimer timer;
        timer.start();
        readSubset();
        qDebug() << "5- readSubset " << timer.elapsed();
    }

    //get and write the timer value
    double t = timer.elapsed();
    QString sec ;
    sec.setNum(t/1000);
    this->outputList.append("elapsed time to parse(Lex/" + multiThread + ") the A2L file = " + sec + " s");

}

void A2l::parseSTA2l()
{
    //open the selected file
    QFile file(fullA2lName);
    if (!file.open(QIODevice::ReadOnly))
    {
        this->outputList.append("Cannot read file " + fullA2lName);
        return;
    }

    QTextStream input(&file);
    QString str;
    while (!file.atEnd()) {
        str.append(file.readLine());
    }
    qint64 size = file.size();
    file.close();

    //char* buffer = new char[size];
    //file.read(buffer, size);
    //QString str = QString::fromLatin1(buffer);

    // set the maximum for the progressbar
    progBarMaxValue = size;

    //Start tokenizer Quex or myLex
    A2lLexer *lexer = 0;
    QStringList *errorList = 0;
    QSettings settings(qApp->organizationName(), qApp->applicationName());
    if (settings.value("lexer") != "Quex")
    {
        //save the buffer into a qtextstream  for my lexer
        QTextStream in(&str);

        //start the tokeniser myLex
        lexer = new A2lLexer(in);
        connect(lexer, &A2lLexer::returnedToken, this, &A2l::checkProgressStream, Qt::DirectConnection);
        lexer->initialize();
        errorList = new QStringList();

        //create an ASAP2 file Node to start parsing the complete a2l
        A2LFILE *nodeA2l = new A2LFILE(0, lexer, errorList, fullA2lName);
        QString filename = QFileInfo(fullA2lName).fileName();
        nodeA2l->name = new char[filename.toLocal8Bit().length() + 1];
        strcpy_s(nodeA2l->name, filename.toLocal8Bit().length() + 1, filename.toLocal8Bit().data());
        a2lFile = nodeA2l;
    }

    //free memory from the char* buffer
    //free(buffer);

    // show error
    if (errorList->isEmpty())
    {
        this->outputList.append("file parsed with success ...");
        is_Ok = true;
    }
    else
    {
        is_Ok = false;
        outputList.append(*errorList);
    }

}

bool A2l::parseOpenMPA2l()
{
    QElapsedTimer time;
    time.start();

    //open the selected file
    QFile file(fullA2lName);
    if (!file.open(QIODevice::ReadOnly))
    {
        this->outputList.append("Cannot read file " + fullA2lName);
        return true;
    }

    QTextStream input(&file);
    QString str;
    while (!file.atEnd()) {
        str.append(file.readLine());
    }
    qint64 size = file.size();
    file.close();

    qDebug() << "\n ---- A2Lfile ---- ";
    qDebug() << "1- read " << time.elapsed();
    time.restart();

    //trunk a2lfile into 2 parts for multi-threading
    QString str1;
    QString str2;
    if (!trunkA2l(str, str1, str2))
    {
        parseSTA2l();
        return true;
    }

    // set the maximum for the progressbar
    progBarMaxValue = str.length();

    qDebug() << "2- trunk " << time.elapsed();
    time.restart();

    //create nodeA2l
    A2LFILE *nodeA2l1 = 0;
    A2LFILE *nodeA2l2 = 0;

    //parse in parallel
    omp_set_dynamic(0);
    omp_set_num_threads(2);
    double t_ref1 = 0, t_final1 = 0;
    double t_ref2 = 0, t_final2 = 0;
    QSettings settings(qApp->organizationName(), qApp->applicationName());

    #pragma omp parallel
      {
          #pragma omp sections
            {
                //thread1
                #pragma omp section
                {
                    // start timer
                    t_ref1 = omp_get_wtime();

                    A2lLexer *lexer1 = 0;
                    QStringList *errorList1 = 0;

                    if (settings.value("lexer") != "Quex")
                    {
                        // create a new lexer
                        QTextStream out1(&str1);
                        lexer1 = new A2lLexer(out1);
                        //connect(lexer1, SIGNAL(returnedToken(int)), this, SLOT(checkProgressStream(int)), Qt::DirectConnection);
                        connect(lexer1, &A2lLexer::returnedToken, this, &A2l::checkProgressStream, Qt::DirectConnection);
                        lexer1->initialize();
                        errorList1 = new QStringList();

                        // start parsing the file
                        nodeA2l1 = new A2LFILE(0, lexer1, errorList1, fullA2lName);

                        // change the name
                        nodeA2l1->name = new char[(QFileInfo(fullA2lName).fileName()).toLocal8Bit().size() + 1];
                        strcpy_s(nodeA2l1->name, (QFileInfo(fullA2lName).fileName()).toLocal8Bit().size() + 1, QFileInfo(fullA2lName).fileName().toLocal8Bit().data());
                    }

                    // stop timer
                    t_final1 = omp_get_wtime();
                }

                //thread2
                #pragma omp section
                {
                    // start timer
                    t_ref2 = omp_get_wtime();

                    A2lLexer *lexer2 = 0;
                    QStringList *errorList2 = 0;

                    if (settings.value("lexer") != "Quex")
                    {
                        // create a new lexer
                        QTextStream out2(&str2);
                        lexer2 = new A2lLexer(out2);
//                        connect(lexer2, SIGNAL(returnedToken(int)), this, SLOT(checkProgressStream(int)),
//                                Qt::DirectConnection);
                        lexer2->initialize();
                        errorList2 = new QStringList();

                        // start parsing the file
                        nodeA2l2 = new A2LFILE(0, lexer2, errorList2, fullA2lName);

                        // change the name
                        nodeA2l2->name = new char[(QFileInfo(fullA2lName).fileName()).toLocal8Bit().size() + 1];
                        strcpy_s(nodeA2l2->name, (QFileInfo(fullA2lName).fileName()).toLocal8Bit().size() + 1, QFileInfo(fullA2lName).fileName().toLocal8Bit().data());
                    }

                    // stop timer
                    t_final2 = omp_get_wtime();
                }
            }
      }


    //display parsing errors
    if (nodeA2l1->errorList->isEmpty() && nodeA2l2->errorList->isEmpty())
    {
        QString num1;
        num1.setNum(t_final1 - t_ref1);
        this->outputList.append("thread 1 parsed in " + num1 + " sec");
        QString num2;
        num2.setNum(t_final2 - t_ref2);
        outputList.append("thread 2 parsed in " + num2 + " sec");
    }
    else
    {
        is_Ok = false;
        outputList.append("ASAP file parser error ...");
        if (!nodeA2l1->errorList->isEmpty())
        {
            outputList.append("error in thread 1:");
            outputList.append(*nodeA2l1->errorList);
        }
        if (!nodeA2l2->errorList->isEmpty())
        {
            outputList.append("error in thread 2:");
            outputList.append(*nodeA2l2->errorList);
        }

        QString num1;
        num1.setNum(t_final1 - t_ref1);
        outputList.append("thread 1 executed in " + num1 + " sec");
        QString num2;
        num2.setNum(t_final2 - t_ref2);
        outputList.append("thread 2 executed in " + num2 + " sec");
    }

    qDebug() << "3- parse " << time.elapsed();
    time.restart();

    //merge the 2 a2lFile
    merge(nodeA2l2, nodeA2l1);
    a2lFile = nodeA2l1;

    qDebug() << "4- merge " << time.elapsed();


    //delete nodeA2l2; MEMORY LEAK !!!
    //cannot be deleted because of the lexer and grammar

    //free memory from the char* buffer
    //delete[] buffer;

    return true;
}

void A2l::parseQThreadA2l(int numThreads)
{

    remainingThreads = numThreads;

    QElapsedTimer time;
    time.start();

    //open the selected file
    QFile file(fullA2lName);
    if (!file.open(QIODevice::ReadOnly))
    {
        this->outputList.append("Cannot read file " + fullA2lName);
        return;
    }

    QTextStream input(&file);
    QString str;
    while (!file.atEnd())
    {
        str.append(file.readLine());
    }
    file.close();

    //trunk a2lfile into 2 parts for multi-threading
    QString str1;
    QString str2;
    if (!trunkA2l(str, str1, str2))
    {
        parseSTA2l();
        return;
    }

    // set the maximum for the progressbar
    progBarMaxValue = str.length();
    QStringList listChunckA2l;
    listChunckA2l.append(str1);
    listChunckA2l.append(str2);

    //parse in parallel
    double t_ref1 = 0, t_final1 = 0;
    double t_ref2 = 0, t_final2 = 0;
    QSettings settings(qApp->organizationName(), qApp->applicationName());

    progressDialog = new QProgressDialog();
    progressDialog->setLabelText(QString("Parsing a2l file using %1 thread(s)...").arg(numThreads)); //QThread::idealThreadCount()
    progressDialog->setMaximum(100);
    progressDialog->setWindowModality(Qt::WindowModal);
    progressDialog->show();

    for (int i = 0; i < numThreads; ++i)
    {
        QThread *workerThread = new QThread();
        Worker *worker = new Worker(listChunckA2l[i], fullA2lName);
        worker->moveToThread(workerThread);
        workers.append(worker);
        threads.append(workerThread);

        connect(workerThread, &QThread::finished, worker, &QObject::deleteLater);
        connect(this, &A2l::operate, worker, &Worker::process);
        connect(worker, &Worker::resultReady, this, &A2l::handleResults);
        connect(worker, &Worker::finished, this, &A2l::handleThreadFinished);
        connect(worker, &Worker::progress, this, &A2l::updateProgress);
        //connect(worker, &Worker::progress, this, &A2l::updateProgressOpenMp);

        workerThread->start();
    }

    // start all threads execution
    emit operate();

    // Start event loop and wait until all threads are finished
    connect(this, &A2l::allThreadsFinished, &loop, &QEventLoop::quit);
    loop.exec();

    // remove dialog with progressbar
    progressDialog->hide();
    delete progressDialog;
    progressDialog = nullptr;

    //merge the 2 chuncks a2lFile
    a2lFile = merge(results[1], results[0]);

    return;
}

void A2l::updateProgress(int pos)
{
    QMutexLocker locker(&progressMutex);
    totalProgress += pos;
    int progress = static_cast<int>((static_cast<double>(totalProgress) / progBarMaxValue) * 100);

    QMetaObject::invokeMethod(progressDialog, "setValue", Qt::QueuedConnection, Q_ARG(int, progress));

    if (progressDialog->wasCanceled())
    {
        for (QThread *thread : threads)
        {
            thread->requestInterruption();
        }
    }
}

void A2l::updateProgressOpenMp(int pos)
{
    if (progressVal < progBarMaxValue)
    {
        omp_set_lock(&lockValue);
        progressVal += pos;

        double div = (double)progressVal/(double)progBarMaxValue;

        if (div > 0.98)
        {
            emit incProgressBar(progBarMaxValue, progBarMaxValue);
        }
        else
        {
            emit incProgressBar(progressVal, progBarMaxValue);
        }

        omp_unset_lock(&lockValue);
    }
}

void A2l::handleResults(A2LFILE* a2lfile, qint64 elapsedTime, QStringList* errorList)
{
    results.append(a2lfile);
}

void A2l::handleThreadFinished()
{
    remainingThreads--;

    if (remainingThreads == 0)
    {
        //delete workers
        qDeleteAll(workers);
        workers.clear();

        // Signal that all threads are finished to Qeventloop
        emit allThreadsFinished();
    }
}

bool A2l::trunkA2l(QString &str, QString &str1, QString &str2)
{
    //devide the file into 2 QString
    int a = str.indexOf(QRegularExpression("/begin MODULE"));
    int b = str.lastIndexOf(QRegularExpression("/end MODULE"));
    int middle = str.size() / 2;

    if ((a + 13 < middle) && (middle < b))
    {
        //split str into the middle
        if (str.size() % 2 == 0)
        {
            str1 = str.left(middle);
            str2 = str.right(middle);
        }
        else
        {
            str1 = str.left(middle + 1);
            str2 = str.right(middle);
        }

        //search for a MODULE child node
        QRegularExpression rxlen("/begin (\\w+)");
        QString nodeType = "";
        int lastBeginChar = -1;

        while (nodeType != "CHARACTERISTIC" &&
               nodeType != "MEASUREMENT" &&
               nodeType != "RECORD_LAYOUT" &&
               nodeType != "FUNCTION" &&
               nodeType != "AXIS_PTS" &&
               nodeType != "RECORD_LAYOUT")
        {

            QRegularExpressionMatchIterator it = rxlen.globalMatch(str1.left(lastBeginChar));
            while (it.hasNext()) {
                QRegularExpressionMatch match = it.next();
                lastBeginChar = match.capturedStart(0); // Store the start index of the last match
                nodeType = match.captured(1);
            }

            if (lastBeginChar < 0)
                return false;
        }

        //create 2 ASAP file for parallel parsing
        str2 = "ASAP2_VERSION  1 4 \n"
               "/begin PROJECT PRO_HEXplorer \"PROJECT\"\n"
               "  /begin HEADER \"\"\n"
               "    VERSION    \"VERSION\"\n"
               "    PROJECT_NO P_NUMBER\n"
               "  /end HEADER \n"
               "  /begin MODULE CHUNKstart DIM \"\"\n" + str1.right(str1.size()- lastBeginChar) + str2;


        str1 = str1.left(lastBeginChar) + "\nCHUNKend";

        return true;
    }
    else
        return false;
}

A2LFILE* A2l::merge(A2LFILE *a2l1, A2LFILE *a2l2)
{
    A2LFILE* src = nullptr;
    A2LFILE* trg = nullptr;
    qDebug() << a2l1->getProject()->name;
    if (a2l1->getProject())
    {
        QString project1 = a2l1->getProject()->name;
        if (project1 == "PRO_HEXplorer")
        {
            src = a2l1; trg = a2l2;
        }
        else
        {
             src = a2l2; trg = a2l1;
        }
    }


    //find Module i stopped in source file
    Node *pro = src->getProject();

    if (!pro)
        return nullptr;

    pro->sortChildrensName();
    Node *srcModule = pro->getNode("MODULE");
    if (srcModule == NULL)
    {
        this->outputList.append("ERROR : could not merge the 2 threads (source module NULL)");
		this->outputList.append("PROJECT name: " + QString(pro->name));
		outputList.append(*pro->errorList);
        return nullptr;
    }

    int i = 0;
    bool foundSrcStopped = false;
    while (!foundSrcStopped && i < srcModule->childNodes.count())
    {
        if (!srcModule->child(i)->stopped)
            i++;
        else if (srcModule->child(i)->stopped)
        {
            foundSrcStopped = true;
        }
    }

    if (foundSrcStopped)
    {
        //find Module stopped in target file;
        Node *trgModule= trg->getProject()->getNode("MODULE");

        if (trgModule == NULL)
        {
            this->outputList.append("ERROR : could not merge the 2 threads (target module NULL)");
            return nullptr;
        }

        int j = 0;
        bool foundTrgStopped = false;
        while (!foundTrgStopped && j < trgModule->childNodes.count())
        {
            if (!trgModule->child(j)->stopped)
                j++;
            else if (trgModule->child(j)->stopped)
            {
                foundTrgStopped = true;
            }
        }

        if (foundTrgStopped)
        {
            //merge the cut module nodes
            foreach(Node* node, srcModule->child(i)->childNodes)
            {
                if (trgModule->child(j)->isChild(node->name))
                {
                    Node *trgNode = trgModule->child(j)->child((QString)node->name, false);
                    foreach (Node* n, node->childNodes)
                    {
                        trgNode->addChildNode(n);
                        n->setParentNode(trgNode);
                        node->removeChildNode(n);
                    }
                    trgNode->sortChildrensName();
                }
                else
                {
                    trgModule->child(j)->addChildNode(node);
                    node->setParentNode(trgModule->child(j));
                    srcModule->child(i)->removeChildNode(node);
                }
            }

            //sort the nodes from target module
            trgModule->child(j)->sortChildrensName();

            //merge also the listChar from cut Module
            MODULE *srcMod = (MODULE*)srcModule->child(i);
            MODULE *trgMod = (MODULE*)trgModule->child(j);
            trgMod->listChar.append(srcMod->listChar);
            trgMod->listChar.sort();

            //merge also the listMeas from cut Module
            trgMod->listMeas.append(srcMod->listMeas);
            trgMod->listMeas.sort();

            //add the other modules that were not splitted for parsing
            foreach(Node* node, srcModule->childNodes)
            {
                //complete Node
                if (!node->stopped)
                {
                    trgModule->addChildNode(node);
                    srcModule->removeChildNode(node);
                    node->setParentNode(trgModule);
                }
            }
        }
        return trg;
    }
    else
    {
        return nullptr;
    }
}

QStringList A2l::_outputList()
{
    return this->outputList;
}

void A2l::readSubset()
{
    if (a2lFile)
    {
        PROJECT *project = a2lFile->getProject();
        if (project)
        {
            QList<MODULE*> listModule = a2lFile->getProject()->listModule();

            foreach (MODULE *module, listModule)
            {
                Node *fun = module->getNode("FUNCTION");
                Node *group = module->getNode("GROUP");
                Node *charac = module->getNode("CHARACTERISTIC");
                Node *axis_pts =  module->getNode("AXIS_PTS");

                if (fun)
                {
                    foreach (Node *subset, fun->childNodes)
                    {
                        bool found = false;
                        DEF_CHARACTERISTIC *def = (DEF_CHARACTERISTIC*)subset->getNode("DEF_CHARACTERISTIC");
                        if (def)
                        {
                            QStringList listDefChar = def->getCharList();
                            if (listDefChar.length() > 0)
                            {
                               found = true;
                            }

                            foreach(QString str, listDefChar)
                            {
                                CHARACTERISTIC *node = (CHARACTERISTIC*)charac->getNode(str);
                                if (node)
                                {
                                    node->setSubset((FUNCTION*)subset);
                                }
                                else
                                {
                                    AXIS_PTS *node = (AXIS_PTS*)axis_pts->getNode(str);
                                    if (node)
                                    {
                                        node->setSubset((FUNCTION*)subset);
                                    }
                                }
                            }
                        }
                        else
                        {
                            REF_CHARACTERISTIC *ref = (REF_CHARACTERISTIC*)subset->getNode("REF_CHARACTERISTIC");
                            if (ref)
                            {
                                QStringList listDefChar = ref->getCharList();
                                if (listDefChar.length() > 0)
                                {
                                   found = true;
                                }

                                foreach(QString str, listDefChar)
                                {
                                    CHARACTERISTIC *node = (CHARACTERISTIC*)charac->getNode(str);
                                    if (node)
                                    {
                                        node->setSubset((FUNCTION*)subset);
                                    }
                                    else
                                    {
                                        AXIS_PTS *node = (AXIS_PTS*)axis_pts->getNode(str);
                                        if (node)
                                        {
                                            node->setSubset((FUNCTION*)subset);
                                        }
                                    }
                                }
                            }
                        }

                        if (!found)
                        {
                            if (group)
                            {
                                GGROUP *grp = (GGROUP*)group->getNode(subset->name);
                                if (grp)
                                {
                                    DEF_CHARACTERISTIC *def = (DEF_CHARACTERISTIC*)grp->getNode("DEF_CHARACTERISTIC");
                                    if (def)
                                    {
                                        QStringList listDefChar = def->getCharList();
                                        if (listDefChar.length() > 0)
                                        {
                                           found = true;
                                        }

                                        foreach(QString str, listDefChar)
                                        {
                                            CHARACTERISTIC *node = (CHARACTERISTIC*)charac->getNode(str);
                                            if (node)
                                            {
                                                node->setSubset((FUNCTION*)subset);
                                            }
                                            else
                                            {
                                                AXIS_PTS *node = (AXIS_PTS*)axis_pts->getNode(str);
                                                if (node)
                                                {
                                                    node->setSubset((FUNCTION*)subset);
                                                }
                                            }
                                        }
                                    }
                                    else
                                    {
                                        REF_CHARACTERISTIC *ref = (REF_CHARACTERISTIC*)grp->getNode("REF_CHARACTERISTIC");
                                        if (ref)
                                        {
                                            QStringList listDefChar = ref->getCharList();
                                            if (listDefChar.length() > 0)
                                            {
                                               found = true;
                                            }

                                            foreach(QString str, listDefChar)
                                            {
                                                CHARACTERISTIC *node = (CHARACTERISTIC*)charac->getNode(str);
                                                if (node)
                                                {
                                                    node->setSubset((FUNCTION*)subset);
                                                }
                                                else
                                                {
                                                    AXIS_PTS *node = (AXIS_PTS*)axis_pts->getNode(str);
                                                    if (node)
                                                    {
                                                        node->setSubset((FUNCTION*)subset);
                                                    }
                                                }
                                            }
                                        }
                                    }

                                }
                                else
                                {

                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void A2l::checkProgressStream(int pos)
{
    if (progressVal < progBarMaxValue)
    {
        omp_set_lock(&lockValue);
        progressVal += pos * 2;

        double div = (double)progressVal/(double)progBarMaxValue;

        if (div > 0.98)
        {
            emit incProgressBar(progBarMaxValue, progBarMaxValue);
        }
        else
        {
             emit incProgressBar(progressVal, progBarMaxValue);
        }

        omp_unset_lock(&lockValue);
    }
}

bool A2l::isOk()
{
    return is_Ok;
}
