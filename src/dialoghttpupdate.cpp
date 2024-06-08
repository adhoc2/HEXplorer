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

#include "dialoghttpupdate.h"

#include "mdimain.h"
#include <QByteArray>
#include <QMessageBox>
#include <QGuiApplication>
#include <QScreen>
#include <QTime>
#include <QSysInfo>
#include <QHostInfo>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QAuthenticator>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QAuthenticator>
#include <QDebug>
#include <QXmlStreamReader>
#include <QDomDocument>


DialogHttpUpdate::DialogHttpUpdate(const QUrl& cfgUrl, bool display, QWidget *mdiMain)
{
    //Save Proxy settings with QSettings
    QSettings settings(qApp->organizationName(), qApp->applicationName());
    if (!settings.contains("Update/automatic"))
        settings.setValue("Update/automatic", true);
    if (!settings.contains("Proxy/behindProxy"))
        settings.setValue("Proxy/behindProxy", false);
    if (!settings.contains("Proxy/HostName"))
        settings.setValue("Proxy/HostName", "");
    if (!settings.contains("Proxy/Port"))
        settings.setValue("Proxy/Port", "");
    if (!settings.contains("Proxy/Password"))
        settings.setValue("Proxy/Password", "02DayasaLion$+");
    if (!settings.contains("Proxy/User"))
        settings.setValue("Proxy/User", "lmbhoc2");

     //QNetworkManager
    updater = new HttpUpdater(mdiMain, display, this);

    //configure proxy
    bool bl = settings.value("Proxy/behindProxy").toBool();
    if (bl)
    {
        QNetworkProxy proxy;
        proxy.setType(QNetworkProxy::HttpCachingProxy);
        proxy.setHostName(settings.value("Proxy/HostName").toString());
        proxy.setPort(settings.value("Proxy/Port").toInt());
        updater->manager.setProxy(proxy);
    }

     //check if updates are available
     updater->getXml(cfgUrl);

}

DialogHttpUpdate::~DialogHttpUpdate(void)
{

}

////////////////////////////////////////////////////////////////////////////
///                        Class HTTPUPDATER                             ///
////////////////////////////////////////////////////////////////////////////

HttpUpdater::HttpUpdater(QWidget *mainApp, bool display, DialogHttpUpdate *par)
{
    displayUptoDate = display;
    parent = par;
    mdiMain = (MDImain*)mainApp;

    connect(&manager, SIGNAL(finished(QNetworkReply*)),this, SLOT(managerRequestFinished(QNetworkReply*)));
    connect(&manager, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)),
            this, SLOT(authenticationRequired(QNetworkReply*,QAuthenticator*)));
    connect(&manager, SIGNAL(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)),
           this, SLOT(proxyAuthenticationRequired(QNetworkProxy,QAuthenticator*)));
}

void HttpUpdater::getXml(const QUrl& url)
{
    QNetworkRequest request(url);
    replyXml = manager.get(request);
}

QString HttpUpdater::saveFileName(const QUrl &url)
{
    QString path = url.path();
    QString basename = qApp->applicationDirPath() + "/" + QFileInfo(path).fileName();

    if (basename.isEmpty())
        basename = "download";

    return basename;
}

void HttpUpdater::downloadInstaller(const QUrl& url)
{
    //download the file from net into local place
    QString filename = saveFileName(url);
    binFile.setFileName(filename);

    //check if file is already downloaded
    if (QFile(filename).exists())
    {

    }

    if (!binFile.open(QIODevice::WriteOnly))
    {

        QMessageBox::warning(0, "HEXplorer::update", "Problem opening file " +
                           filename + " for download at " + url.toEncoded().constData() +
                           " : " + url.toEncoded().constData() + "\n",
                              QMessageBox::Ok, QMessageBox::Cancel);

        return;

    }

    QNetworkRequest request(url);
    request.setRawHeader("Accept", "application/octet-stream");
    replyDownloader = manager.get(request);


    connect(replyDownloader, SIGNAL(downloadProgress(qint64,qint64)), SLOT(downloadProgress(qint64,qint64)));
    connect(replyDownloader, SIGNAL(readyRead()), this, SLOT(downloadReadyRead()));

    downloadTime.start();
}

void HttpUpdater::deleteCurrentInstallation()
{
    QString maintenanceToolPath = QCoreApplication::applicationDirPath() + "/maintenancetool.exe";
    QProcess *myProcess = new QProcess();
    myProcess->start(maintenanceToolPath, QStringList() << "--uninstall");
    //myProcess->startDetached(maintenanceToolPath, QStringList() << "--updater");


    myProcess->waitForFinished();

    return;

}

void HttpUpdater::launchInstaller()
{
    QFileInfo info(updateFilePath);
    QString fileName = info.fileName();
    QString application = qApp->applicationDirPath() + "/" + fileName;

    deleteCurrentInstallation();

    QProcess *myProcess = new QProcess();
    myProcess->startDetached(application);

    if(myProcess->waitForStarted())
    {
        mdiMain->close();
    }
}

//// SLOTS ////

void HttpUpdater::authenticationRequired( QAuthenticator *auth)
{
    QSettings settings(qApp->organizationName(), qApp->applicationName());
    auth->setUser(settings.value("Proxy/User").toString());
    auth->setPassword(settings.value("Proxy/Password").toString());
}

void HttpUpdater::proxyAuthenticationRequired( QAuthenticator *auth)
{
    QSettings settings(qApp->organizationName(), qApp->applicationName());
    auth->setUser(settings.value("Proxy/User").toString());
    auth->setPassword(settings.value("Proxy/Password").toString());
}

void HttpUpdater::managerRequestFinished(QNetworkReply *reply)
{
    if (reply == replyXml)
    {
        //if xml file could not be downloaded
        if (reply->error())
        {
            QMessageBox::warning(0, "HEXplorer::update", QString(reply->errorString()) +
                                  "\n\n" +
                                  "HEXplorer could not read Updates.xml file for available updates.\n\n" +
                                  "Please control or configure your internet connection \n" +
                                  "setting properly the proxy parameters if you are behind a proxy\n" +
                                  "or simply disabling the automatic updates into Edit/Settings.\n",
                                  QMessageBox::Ok, QMessageBox::Cancel);

            return; //exit download
        }

        if (!reply->open(QFile::ReadOnly | QFile::Text))
        {
            QMessageBox::warning(0, "HEXplorer::update", "Unable to open Updates.xml file",
                                  QMessageBox::Ok, QMessageBox::Cancel);

            return;
        }
        else
        {
            if (0)
            {
                QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll());
                QJsonObject rootObject = jsonDocument.object();
                QByteArray data = QByteArray::fromBase64(rootObject.value("content").toVariant().toByteArray());

                QString str = reply->readAll();

                QXmlStreamReader reader(str);
                while (!reader.atEnd())
                {
                    reader.readNext();
                    if (reader.isStartElement() && reader.attributes().hasAttribute("Version"))
                    {
                        newVersion = reader.attributes().value("version").toString();
                        QString old_Version = qApp->applicationVersion();
                        QStringList oldList = old_Version.split(".");
                        QStringList newList = newVersion.split(".");
                        if (newList.at(0).toDouble() > oldList.at(0).toDouble())
                        {
                            reader.readNext();
                            if(reader.isCharacters())
                            {
                                updateFilePath = reader.text().toString();
                            }
                        }
                        else if (newList.at(0).toDouble() == oldList.at(0).toDouble())
                        {
                            if (newList.at(1).toDouble() > oldList.at(1).toDouble())
                            {
                                reader.readNext();
                                if(reader.isCharacters())
                                {
                                    updateFilePath = reader.text().toString();
                                }
                            }
                            else if (newList.at(1).toDouble() == oldList.at(1).toDouble())
                            {
                                if (newList.at(2).toDouble() > oldList.at(2).toDouble())
                                {
                                    reader.readNext();
                                    if(reader.isCharacters())
                                    {
                                        updateFilePath = reader.text().toString();
                                    }
                                }
                            }
                        }
                    }
                    else if (reader.isStartElement() && reader.qualifiedName() == QString("details"))
                    {
                        reader.readNext();
                        if(reader.isCharacters())
                        {
                            updateDetails = reader.text().toString();
                        }
                    }
                }

                //display streamreader error
                if (reader.error())
                {
                    QMessageBox::critical(0, "HEXplorer::update", reader.errorString(),
                                          QMessageBox::Ok, QMessageBox::Cancel);
                    return;
                }

                if(updateFilePath.isEmpty())
                {
                    if (displayUptoDate)
                    {
                        QMessageBox::information(0, "HEXplorer::update", "You are up to date at version "
                                                                             + qApp->applicationVersion(),
                                                 QMessageBox::Ok);
                    }
                }
                else
                {
                    QMessageBox msgBox;
                    msgBox.setIconPixmap(QPixmap(":/icones/updates.png").scaled(80,80));
                    msgBox.setText("A new update is available :\n - current version installed: " + qApp->applicationVersion() + "\n - new version available: " + newVersion);
                    msgBox.setInformativeText("would you like to download it?");
                    //msgBox.setInformativeText("Download at https://github.com/adhoc2/HEXplorer/releases/");
                    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                    msgBox.setDefaultButton(QMessageBox::Yes);
                    msgBox.setDetailedText(updateDetails);
                    int ret = msgBox.exec();

                    if (ret == QMessageBox::Yes)
                    {
                        //binUrl.setUrl(updateFilePath);
                        //binUrl.setUrl("https://api.github.com/repos/adhoc2/HEXplorer/releases/latest/setup_HEXplorer.exe");
                        binUrl.setUrl("https://github.com/adhoc2/HEXplorer/releases/download/v" + newVersion + "/setup_HEXplorer_" + newVersion + ".exe");
                        downloadInstaller(binUrl);
                    }
                    else
                        return;
                }

            }

            QString str = reply->readAll();
            QDomDocument document;
            if (!document.setContent(str))
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
            else
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
                }
                else
                    return;
            }

        }
    }
    else if (reply == replyDownloader)
    {
        progressBar.hide();
        binFile.close();

        if (reply->error())
        {
            QMessageBox::warning(0, "HEXplorer::update", QString(reply->errorString()) +
                                  "\n\n" +
                                  "Please control your internet connection \n" +
                                  "or set the proxy parameters properly into Edit/Settings",
                                  QMessageBox::Ok, QMessageBox::Cancel);

            return; //exit
        }

        QMessageBox msgBox;
        msgBox.setIconPixmap(QPixmap(":/icones/updates.png").scaled(80,80));
        msgBox.setText("Download completed.");
        msgBox.setInformativeText("Would you like to install ?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        int ret = msgBox.exec();

        //update user_List
        if (ret == QMessageBox::Yes)
        {
            //execute the downloaded file
            launchInstaller();
        }
        else
            return;
    }
}

void HttpUpdater::downloadReadyRead()
{
    binFile.write(replyDownloader->readAll());
}

void HttpUpdater::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    progressBar.show();
    progressBar.setStatus(bytesReceived, bytesTotal);

    // calculate the download speed
    double speed = bytesReceived * 1000.0 / downloadTime.elapsed();
    QString unit;
    if (speed < 1024) {
        unit = "bytes/sec";
    } else if (speed < 1024*1024) {
        speed /= 1024;
        unit = "kB/s";
    } else {
        speed /= 1024*1024;
        unit = "MB/s";
    }

    progressBar.setMessage(QFileInfo(updateFilePath).fileName() + " " + QString::fromLatin1("%1 %2")
                          .arg(speed, 3, 'f', 1).arg(unit));

}
