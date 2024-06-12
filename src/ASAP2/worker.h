#ifndef WORKER_H
#define WORKER_H

#include "a2lfile.h"
#include <QObject>
#include <QThread>
#include <QDebug>

class Worker : public QObject
{
    Q_OBJECT

    public:
        explicit Worker(const QString& data, const QString& fileName,  QObject *parent = nullptr);
        ~Worker();

    private:
        QString str;
        QString fullA2lName;

    public slots:
        void process();  // Slot to start processing

    signals:
        void finished(); // Signal to indicate completion
        void resultReady(A2LFILE*, qint64 , QStringList*); // Signal to send result
        void progress(int pos);
};

#endif // WORKER_H
