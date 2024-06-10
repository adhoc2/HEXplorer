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

#include "workproject.h"
#include <Nodes/a2lfile.h>
#include "csv.h"
#include "cdfxfile.h"
#include "dcmfile.h"
#include "qfileinfo.h"
#include "qdebug.h"

WorkProject::WorkProject(QString fullFileName, A2lTreeModel *model, MDImain *parent, QString displayName)
    :  A2l(fullFileName, parent), Node()
{

    treeModel = model;
    parentWidget = parent;

    if (QString::compare(displayName, "") != 0)
    {
        name = new char[displayName.toLocal8Bit().size() + 1];
        strcpy_s(name, displayName.toLocal8Bit().size() + 1, displayName.toLocal8Bit().data());
    }
    else
    {
        name = new char[(QFileInfo(fullFileName).fileName()).toLocal8Bit().size() + 1];
        strcpy_s(name, (QFileInfo(fullFileName).fileName()).toLocal8Bit().size() + 1, QFileInfo(fullFileName).fileName().toLocal8Bit().data());
    }

}

WorkProject::~WorkProject()
{
}

QString WorkProject::fullName()
{
    return getFullA2lFileName().c_str();
}

void WorkProject::attach(QObject *o)
{
    //check owner for validity
    if(o==0)return;

    //register
    owners.append(o);

    connect(o,SIGNAL(destroyed(QObject*)),this,SLOT(detach(QObject*)));

}

void WorkProject::detach(QObject *o)
{
    //remove it
    owners.removeOne(o);

    //remove self after last one
    if(owners.size()==0)
    {
        qDebug() << "WP : " << this->a2lFile->name << " deleted";
        delete this;
    }
}

QMap<QString, HexFile*> WorkProject::hexFiles()
{
    return hexList;
}

QMap<QString, SrecFile*> WorkProject::srecFiles()
{
    return srecList;
}

QMap<QString, Csv*> WorkProject::csvFiles()
{
    return csvList;
}

QMap<QString, Dcm*> WorkProject::dcmFiles()
{
    return dcmList;
}

QMap<QString, CdfxFile*> WorkProject::cdfxFiles()
{
    return cdfxList;
}

void WorkProject::addHex(HexFile *hex, Node* subDir )
{
    // add hex to subDir childrenslist for treeView
    subDir->addChildNode(hex);
    hex->setParentNode(subDir);
    subDir->sortChildrensName();

    treeModel->dataInserted(subDir, subDir->childNodes.indexOf(hex));

    // add hex to this hexList
    hexList.insert(hex->fullName(), hex);

    // add this to the hex owners (pseudo garbage collector)
    hex->attach(this);
}

void WorkProject::addA2lnode(PROJECT *a2l, Node* subDir)
{
    // add hex to subDir childrenslist for treeView
    subDir->addChildNode(a2l);
    a2l->setParentNode(subDir);
    subDir->sortChildrensName();

    treeModel->dataInserted(subDir, subDir->childNodes.indexOf(a2l));

}

void WorkProject::addSrec(SrecFile *srec, Node* subDir)
{
    // add hex to subDir childrenslist for treeView
    subDir->addChildNode(srec);
    srec->setParentNode(subDir);
    subDir->sortChildrensName();

    treeModel->dataInserted(subDir, subDir->childNodes.indexOf(srec));

    // add srec to this srecList
    srecList.insert(srec->fullName(), srec);

    // add this to the hex owners (pseudo garbage collector)
    srec->attach(this);
}

void WorkProject::addCsv(Csv *csv )
{
    // add csv to a2lfile childrenslist
    this->addChildNode(csv);
    this->sortChildrensName();

    // update treeView
    treeModel->dataInserted(this, this->childNodes.indexOf(csv));

    // add csv to this hexList
    csvList.insert(csv->fullName(), csv);

    // add this to the csv owners
    csv->attach(this);

}

void WorkProject::addDcm(Dcm *dcm)
{
    // add dcm to a2lfile childrenslist
    this->addChildNode(dcm);
    this->sortChildrensName();

    // update treeView
    treeModel->dataInserted(this, this->childNodes.indexOf(dcm));

    // add csv to this hexList
    dcmList.insert(dcm->fullName(), dcm);

    // add this to the csv owners
    dcm->attach(this);

}

void WorkProject::addCdfx(CdfxFile *cdfx)
{
    // add csv to a2lfile childrenslist
    this->addChildNode(cdfx);
    this->sortChildrensName();

    // update treeView
    treeModel->dataInserted(this, this->childNodes.indexOf(cdfx));


    // add cdfx to this hexList
    cdfxList.insert(cdfx->fullName(), cdfx);

    // add this to the cdfx owners
    cdfx->attach(this);
}

void WorkProject::removeHexFile(HexFile *hex)
{
    hexList.remove(hex->fullName());

    // remove this Wp to the csv owners
    hex->detach(this);
}

void WorkProject::removeSrecFile(SrecFile *srec)
{
    srecList.remove(srec->fullName());

    // remove this Wp to the csv owners
    srec->detach(this);
}

void WorkProject::removeCsv(Csv *csv )
{
    csvList.remove(csv->fullName());

    // remove this Wp to the csv owners
    csv->detach(this);

}

void WorkProject::removeDcm(Dcm *dcm )
{
    dcmList.remove(dcm->fullName());

    // remove this Wp to the csv owners
    dcm->detach(this);

}

void WorkProject::removeCdfxFile(CdfxFile *cdfx)
{
    cdfxList.remove(cdfx->fullName());

    // remove this Wp to the csv owners
    cdfx->detach(this);

}

void WorkProject::rename(HexFile *hex)
{
    QString key = hexList.key(hex);
    hexList.remove(key);
    hexList.insert(hex->fullName(), hex);
}

bool WorkProject::containsHex(QString str)
{
        return (hexList.keys().contains(str) || hexList.keys().contains(str.replace('/', '\\')));
}

void WorkProject::rename(SrecFile *srec)
{
    QString key = srecList.key(srec);
    srecList.remove(key);
    srecList.insert(srec->fullName(), srec);
}

bool WorkProject::containsSrec(QString str)
{
    return (srecList.keys().contains(str) || srecList.keys().contains(str.replace('/', '\\')));
}

void WorkProject::rename(Csv *csv)
{
    QString key = csvList.key(csv);
    csvList.remove(key);
    csvList.insert(csv->fullName(), csv);
}

void WorkProject::rename(Dcm *dcm)
{
    QString key = dcmList.key(dcm);
    dcmList.remove(key);
    dcmList.insert(dcm->fullName(), dcm);
}

bool WorkProject::containsCsv(QString str)
{
    return (csvList.keys().contains(str) || csvList.keys().contains(str.replace('/', '\\')));
}

bool WorkProject::containsDcm(QString str)
{
    return (dcmList.keys().contains(str) || dcmList.keys().contains(str.replace('/', '\\')));
}

void WorkProject::rename(CdfxFile *cdfx)
{
    QString key = cdfxList.key(cdfx);
    cdfxList.remove(key);
    cdfxList.insert(cdfx->fullName(), cdfx);
}

bool WorkProject::containsCdfx(QString str)
{
    return (cdfxList.keys().contains(str) || cdfxList.keys().contains(str.replace('/', '\\')));
}

HexFile* WorkProject::getHex(QString str)
{
    foreach (HexFile* hex, hexList.values())
    {
        if (hex->fullName() == str)
        {
            return hex;
        }
    }

    return NULL;
}

SrecFile* WorkProject::getSrec(QString str)
{
    foreach (SrecFile* srec, srecList.values())
    {
        if (srec->fullName() == str)
        {
            return srec;
        }
    }

    return NULL;
}

QStringList WorkProject::getHexList()
{
    return hexList.keys();
}

QStringList WorkProject::getSrecList()
{
    return srecList.keys();
}

QString WorkProject::toString()
{
    QString str = "WorkProject* (" + QString(a2lFile->name) + " )" ;
    return str;
}

QString WorkProject::getFullNodeName()
{
    if (treeModel)
    {
        QModelIndex index = treeModel->getIndex(this);
        return treeModel->getFullNodeTreePath(index);
    }
    else
        return "xxx";
}

std::string WorkProject::pixmap()
{
    return ":/icones/milky_cartable.png";
}
