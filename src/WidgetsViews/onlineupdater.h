#ifndef ONLINEUPDATER_H
#define ONLINEUPDATER_H

#include "mdimain.h"
#include <QtCore>
class QString;
class QByteArray;
class QUrl;

class OnlineUpdater : public QObject
{
    Q_OBJECT

public:
    OnlineUpdater(const QUrl &url, QWidget *mainApp,  bool display);

private:
    bool displayUptoDate;
    QString updateDetails;
    MDImain *mdiMain;

    void parseXml(const QByteArray &xmlData);
    void fetchXmlFile(const QUrl &url);
};

#endif // ONLINEUPDATER_H
