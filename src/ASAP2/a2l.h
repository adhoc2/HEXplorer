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

#ifndef A2L_H
#define A2L_H

#include "qobject.h"
//#include <omp.h>
#include <string>
#include <QRegularExpression>
#include <QStringList>
#include <QMap>
#include <a2lfile.h>
#include <dbfile.h>
#include "worker.h"
#include <QEventLoop>
#include <QProgressDialog>
class HexFile;

class A2l : public QObject
{
    Q_OBJECT

    public:
        A2l(QString fullFileName, QObject *parent = 0);
        ~A2l();

        void parseQThreadA2l(int);

        A2LFILE *a2lFile;
        DBFILE *dbFile;
        QString getFullA2lFileName();
        void setFullA2lFileName(QString str);
        QStringList _outputList();
        void init();
        void parse();
        A2LFILE *merge(A2LFILE *src, A2LFILE *trg);
        bool isOk();

    private:
        //omp_lock_t lockValue;
        bool trunkA2l(QString &str, QString &str1, QString &str2);
        void initialize();
        QString fullA2lName;
        QStringList outputList;
        bool is_Ok;
        int progressVal;
        int progBarMaxValue;
        void openA2l();        
        void parseSTA2l();
        bool parseOpenMPA2l();
        void readSubset();

        QMutex progressMutex;
        QEventLoop loop;
        QProgressDialog *progressDialog;
        int totalProgress;
        QList<QThread*> threads;
        QList<Worker*> workers;
        int remainingThreads;
        QList<A2LFILE*> a2lFiles;
        QList<qint64> elapsedTimes;
        QList<QStringList*> errorLists;

    signals:
        void incProgressBar(int,int);
        void operate();
        void allThreadsFinished();

    private slots:
        void handleResults(A2LFILE*a2lfile, qint64 elapsedTime, QStringList*errorList);
        void handleThreadFinished();
        void updateProgressQThread(int pos);
        void updateProgressOpenMp(int pos);

};

#endif // A2L_H

