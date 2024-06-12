#ifndef PARSERWORKER_H
#define PARSERWORKER_H

#include "qthread.h"
#include <QObject>

class Worker : public QObject
{
    Q_OBJECT

    public slots:
        void process(const QString &parameter)
        {
            QString result;
            QThread::sleep(5);
            emit resultReady(result);
        }

    signals:
        void resultReady(const QString &result);
    };

#endif // PARSERWORKER_H
