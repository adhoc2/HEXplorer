#include "onlineupdater.h"
#include "qapplication.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QXmlStreamReader>
#include <QDebug>
#include <QEventLoop>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QAuthenticator>
#include <QNetworkRequest>
#include <QAuthenticator>
#include <QDebug>
#include <QXmlStreamReader>
#include <QDomDocument>

OnlineUpdater::OnlineUpdater(const QUrl &url, QWidget *mainApp, bool display)
{
    mdiMain = (MDImain*)mainApp;
    this->displayUptoDate = display;
    fetchXmlFile(url);

}

void OnlineUpdater::parseXml(const QByteArray &xmlData)
{
    QDomDocument document;
    if (!document.setContent(xmlData))
    {
        QMessageBox::warning(0, "HEXplorer::update", "Unable to parse Updates.xml file",
                             QMessageBox::Ok, QMessageBox::Cancel);
        return;
    }

    QStringList listVersion ;
    QDomNodeList items = document.elementsByTagName("PackageUpdate");
    for (int i = 0; i < items.count(); ++i) {
        QDomNode itemNode = items.at(i);
        QDomElement itemElement = itemNode.toElement();
        if (!itemElement.isNull()) {
            QDomNodeList children = itemElement.childNodes();
            for (int j = 0; j < children.count(); ++j) {
                QDomNode childNode = children.at(j);
                QDomElement childElement = childNode.toElement();
                if (childElement.tagName() == "Version") {
                    listVersion.append(childElement.text());
                }
            }
        }
    }

    bool update = false;
    QString newVersion = listVersion.at(0);
    QString old_Version = qApp->applicationVersion();
    QStringList oldList = old_Version.split(".");
    QStringList newList = newVersion.split(".");
    if (newList.at(0).toDouble() > oldList.at(0).toDouble())
    {

        update = true;

    }
    else if (newList.at(0).toDouble() == oldList.at(0).toDouble())
    {
        if (newList.at(1).toDouble() > oldList.at(1).toDouble())
        {
            update = true;
        }
        else if (newList.at(1).toDouble() == oldList.at(1).toDouble())
        {
            if (newList.at(2).toDouble() > oldList.at(2).toDouble())
            {
                update = true;
            }
        }
    }


    //if update is or not available, do:
    if(!update)
    {
        if (displayUptoDate)
        {
            QMessageBox::information(0, "HEXplorer::update", "You are up to date at version "
                                                                 + qApp->applicationVersion(),
                                     QMessageBox::Ok);
        }
    }
    else if (update)
    {
        QMessageBox msgBox;
        msgBox.setIconPixmap(QPixmap(":/icones/updates.png").scaled(80,80));
        msgBox.setText("A new update is available :\n - current version installed: " + qApp->applicationVersion() + "\n - new version available: " + newVersion);
        msgBox.setInformativeText("would you like to install it?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        msgBox.setDetailedText(updateDetails);
        int ret = msgBox.exec();

        if (ret == QMessageBox::Yes)
        {
            QString maintenanceToolPath = QCoreApplication::applicationDirPath() + "/maintenancetool.exe";
            QProcess *myProcess = new QProcess();
            myProcess->start(maintenanceToolPath, QStringList() << "--updater");

            if(myProcess->waitForFinished())
            {
                QProcess *myProcess = new QProcess();
                myProcess->startDetached(QApplication::applicationFilePath(), QStringList());
                mdiMain->close();
            }
            return;
        }
        else
            return;
    }


}

void OnlineUpdater::fetchXmlFile(const QUrl &url)
{
    QNetworkAccessManager manager;
    QNetworkRequest request(url);
    QNetworkReply *reply = manager.get(request);

    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if (reply->error() != QNetworkReply::NoError)
    {
        QMessageBox::warning(0, "HEXplorer::update", QString(reply->errorString()) +
                                                         "\n\n" +
                                                         "HEXplorer could not read Updates.xml file for available updates.\n\n" +
                                                         "Please control or configure your internet connection \n" +
                                                         "setting properly the proxy parameters if you are behind a proxy\n" +
                                                         "or simply disabling the automatic updates into Edit/Settings.\n",
                             QMessageBox::Ok, QMessageBox::Cancel);

        return; //exit download
        reply->deleteLater();
        return;
    }


    // Ensure the reply has finished and check the state
    if (reply->isFinished())
    {
        QByteArray xmlData = reply->readAll();
        reply->deleteLater();

        parseXml(xmlData);
    }
    else
    {
        reply->deleteLater();
    }

    return;
}
