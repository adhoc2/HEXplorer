#include "worker.h"
#include "a2lfile.h"
#include "lexer.h"
#include <QFileInfo>

Worker::Worker(const QString& data, const QString& fileName,  QObject *parent)
    :  QObject(parent), str(data), fullA2lName(fileName)
{
}

Worker::~Worker() {
    qDebug() << "Worker destroyed";
}

void Worker::process()
{
    // start timer
    QElapsedTimer timer;
    timer.start();

    // create a new lexer
    QTextStream out(&str);
    A2lLexer *lexer = new A2lLexer(out);;
    QStringList *errorList = new QStringList();
    lexer->initialize();

    connect(lexer, &A2lLexer::returnedToken, this, &Worker::progress, Qt::DirectConnection);

    // start parsing the file
    A2LFILE* nodeA2l = new A2LFILE(0, lexer, errorList, fullA2lName);

    // change the name
    nodeA2l->name = new char[(QFileInfo(fullA2lName).fileName()).toLocal8Bit().size() + 1];
    strcpy_s(nodeA2l->name, (QFileInfo(fullA2lName).fileName()).toLocal8Bit().size() + 1, QFileInfo(fullA2lName).fileName().toLocal8Bit().data());

    // stop timer
    qint64  elapsedTime = timer.elapsed();

    //QThread::currentThread()->isInterruptionRequested();


    //emit progress();

    emit resultReady(nodeA2l, elapsedTime, errorList);

    emit finished();
}
