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

#include "csv.h"

#include "data.h"
#include "lexer.h"
#include "lexerCsv.h"
#include "formcompare.h"
#include <QtCore/qmath.h>
#include <QFileInfo>
#include <QMessageBox>
#include <QDir>
#include <QFileDialog>
#include <QSettings>
#include <QDomDocument>
#include <QHeaderView>
#include <QApplication>
#include <typeinfo>

bool comp(Data *a, Data *b)
{
   if (a->getName() < b->getName())
       return true;
   else return false;
}

// ___________ Csv Ctor/Dtor __________ //

Csv::Csv(QString fullCsvFileName, WorkProject *parentWP, QString modName, QObject *parent)
    : QObject(parent) , DataContainer(parentWP, modName)
{
    a2lProject = (PROJECT*)getParentWp()->a2lFile->getProject();
    fullPath = fullCsvFileName;
    name = new char[(QFileInfo(fullPath).fileName()).toLocal8Bit().size() + 1];
    strcpy(name, (QFileInfo(fullPath).fileName()).toLocal8Bit().data());
    maxValueProgbar = 0;
    valueProgBar = 0;
    omp_init_lock(&lock);

    //get the byte_order
    MOD_COMMON *modCommon = (MOD_COMMON*)a2lProject->getNode("MODULE/" + getModuleName() + "/MOD_COMMON");
    if (modCommon)
    {
        Byte_Order *item = (Byte_Order*)modCommon->getItem("BYTE_ORDER");
        if (item)
            byteOrderCommon = item->getPar("ByteOrder");

        //define dataType map
        bool bl;
        ALIGNMENT_BYTE *item1 = (ALIGNMENT_BYTE*)modCommon->getItem("alignment_byte");
        if (item1)
        {
            QString str = item1->getPar("AlignmentBorder");
            nByte.insert("UBYTE", str.toInt(&bl,10));
            nByte.insert("SBYTE", str.toInt(&bl,10));
        }
        else
        {
            nByte.insert("UBYTE", 1);
            nByte.insert("SBYTE", 1);
        }
        ALIGNMENT_WORD *item2 = (ALIGNMENT_WORD*)modCommon->getItem("alignment_word");
        if (item2)
        {
            QString str = item2->getPar("AlignmentBorder");
            nByte.insert("UWORD", str.toInt(&bl,10));
            nByte.insert("SWORD", str.toInt(&bl,10));
        }
        else
        {
            nByte.insert("UWORD", 2);
            nByte.insert("SWORD", 2);
        }
        ALIGNMENT_LONG *item3 = (ALIGNMENT_LONG*)modCommon->getItem("alignment_long");
        if (item3)
        {
            QString str = item3->getPar("AlignmentBorder");
            nByte.insert("ULONG", str.toInt(&bl,10));
            nByte.insert("SLONG", str.toInt(&bl,10));
        }
        else
        {
            nByte.insert("ULONG", 4);
            nByte.insert("SLONG", 4);
        }
        ALIGNMENT_FLOAT32_IEEE *item4 = (ALIGNMENT_FLOAT32_IEEE*)modCommon->getItem("alignment_float32_ieee");
        if (item4)
        {
            QString str = item4->getPar("AlignmentBorder");
            nByte.insert("FLOAT32_IEEE", 4);
        }
        else
        {
            nByte.insert("FLOAT32_IEEE", 4);
        }
        ALIGNMENT_FLOAT64_IEEE *item5 = (ALIGNMENT_FLOAT64_IEEE*)modCommon->getItem("alignment_float64_ieee");
        if (item5)
        {
            QString str = item5->getPar("AlignmentBorder");
            nByte.insert("FLOAT64_IEEE", str.toInt(&bl,10));
        }
        else
        {
            nByte.insert("FLOAT64_IEEE", 8);
        }
    }


}

Csv::~Csv()
{
    omp_destroy_lock(&lock);
    delete[] name;
}

// ____________ read / write ____________ //

bool Csv::readFile()
{
    QFile file(fullPath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QString str;
    QTextStream in;

    QTextStream in1(&file);
    str = in1.readAll();
    file.close();
    in.setString(&str);

    //set progressBar max value
    maxValueProgbar = str.length();

    //Parser
    QStringList outList;
    LexerCsv mylex;
    connect(&mylex, SIGNAL(returnedToken(int)), this, SLOT(checkProgressStream(int)),
            Qt::DirectConnection);
    QString header = in.readLine();
    if (header.startsWith("CALIBRATION VALUES V"))
    {
        char ch;

        //parse "Description Header" for "unit delimiter, string delimiter, comment
        in.seek(20);
        mylex.getNextToken(in);
        if (mylex.getToken() == Float)
        {
            mylex.getNextToken(in);
            if (mylex.getToken() == ValueSeparator && (mylex.getLexem() == "," || mylex.getLexem() == ";" || mylex.getLexem() == "\t"))
            {
                mylex.valueSeparator = mylex.getLexem().at(0);
                mylex.getNextToken(in);
                if (mylex.getToken() == Text && (mylex.getLexem() == "," || mylex.getLexem() == "."))
                {
                    mylex.decimalPointSeparator = mylex.getLexem().at(0);
                    in >> ch;
                    if (ch == mylex.valueSeparator)
                    {
                        in >> mylex.commentIndicator;
                        in >> ch;
                        if (ch == mylex.valueSeparator)
                        {
                            char c1, c2;
                            in >> c1;
                            in >> c2;
                            if (c1 == c2)
                                mylex.stringDelimiter = c1;
                            else
                            {
                                showError("CSV description Header : parser error at line " + QString::number(mylex.getLine())
                                           + " with lexem " + QString(mylex.getLexem().c_str()));
                                return false;
                            }
                        }
                        else
                        {
                            showError("CSV description Header : parser error at line " + QString::number(mylex.getLine())
                                      + " with lexem " + QString(mylex.getLexem().c_str()));
                            return false;
                        }
                    }
                    else
                    {
                        showError("CSV description Header : parser error at line " + QString::number(mylex.getLine())
                                  + " with lexem " + QString(mylex.getLexem().c_str()));
                        return false;
                    }
                    mylex.unitDelimiter = ':';
                }
            }
        }
        else
        {
            showError("CSV description Header : parser error at line " + QString::number(mylex.getLine())
                      + " with lexem " + QString(mylex.getLexem().c_str()));
            return false;
        }
        mylex.getNextToken(in); //valueSeparator

        //parse data
        TokenTyp token;
        token = mylex.getNextToken(in);

        while (!in.atEnd())
        {
            while (token == Eol)
            {
                token = mylex.getNextToken(in);
            }

            if (token == Keyword && mylex.getLexem() == "FUNCTION_HDR")
            {
                //parse until Eol
                token = mylex.getNextToken(in);
                if (token == ValueSeparator)
                {
                    while (token != Eol)
                    {
                        token = mylex.getNextToken(in);
                    }
                }
                token = mylex.getNextToken(in);

                //parse the <Function Block record> until Eol
                while (token != Eol)
                {
                    token = mylex.getNextToken(in);
                }
            }
            else if (token == Keyword && mylex.getLexem() == "VARIANT_HDR")
            {
                token = mylex.getNextToken(in);
                while (token != Eol && token != Eof)
                {
                    token = mylex.getNextToken(in);
                }
            }
            else if (token == Keyword && mylex.getLexem() == "FUNCTION")
            {
                token = mylex.getNextToken(in);
                while (token != Eol && token != Eof)
                {
                    token = mylex.getNextToken(in);
                }
            }
            else if (token == Identifier)
            {
                // create a data if not already created
                Data *data = nullptr;
                if (!getData(mylex.getLexem().c_str()))
                {
                    //get the characteristic node from a2l
                    Node *nodeChar = a2lProject->getNode("MODULE/" + getModuleName() + "/CHARACTERISTIC");
                    Node *nodeAxis = a2lProject->getNode("MODULE/" + getModuleName() + "/AXIS_PTS");
                    Node *label = nodeChar->getNode(mylex.getLexem().c_str());
                    if (label)
                    {
                        //create a data
                        data = new Data((CHARACTERISTIC*)label, a2lProject, this, false);
                        //data->isSortedByRow = 1;
                        //QList<Data*>::iterator i = qLowerBound(listData.begin(), listData.end(), data, comp);
                        QList<Data*>::iterator i = std::lower_bound(listData.begin(), listData.end(), data, comp);
                        listData.insert(i, data);
                    }
                    else
                    {
                        Node *label2 = nodeAxis->getNode(mylex.getLexem().c_str());
                        if (label2)
                        {
                            //create a data
                            data = new Data((AXIS_PTS*)label2, a2lProject, this, false);
                            //QList<Data*>::iterator i = qLowerBound(listData.begin(), listData.end(), data, comp);
                            QList<Data*>::iterator i = std::lower_bound(listData.begin(), listData.end(), data, comp);
                            listData.insert(i, data);
                        }
                        else
                        {
                            outList.append("read CSV file : " + QString(mylex.getLexem().c_str()) + " not found into "
                                           + QString(this->getParentWp()->a2lFile->name));
                        }
                    }
                }
                else
                {
                    data = getData(mylex.getLexem().c_str());
                }

                bool ok = false;
                if (data)
                {
                    while (!ok)
                    {
                        // Z values
                        token = mylex.getNextToken(in);

                        //parse the line until Eol
                        while (token != Eol && token != Eof)
                        {
                            token = mylex.getNextToken(in);
                        }
                        token = mylex.getNextToken(in);

                        if (token == Keyword && mylex.getLexem() == "VALUE")
                        {
                            int count = 0;
                            while (count < 2)
                            {
                                token = mylex.getNextToken(in);
                                if (token == ValueSeparator)
                                    count++;
                            }

                            // in case a label is twice in the subset!!
                            data->clearZ();

                            // read the Z values
                            QStringList list;
                            token = mylex.getNextToken(in);

                            mylex.initialize();
                            if (token ==  Integer || token == Float || token  == String)
                            {
                                QString str = mylex.getLexem().c_str();
                                str.replace(",",".");
                                str.remove("\"");
                                list.append(str);
                                token = mylex.getNextToken(in);
                            }

                            // copy z values to data
                            foreach (QString str, list)
                            {
                                // convert PHYS value from Csv into HEX value
                                QString hex = data->phys2hex(str, "z");

                                // convert HEX value into PHYS value
                                QString phys = data->hex2phys(hex, "z");

                                // copy PHYS value into Zaxis
                                data->appendZ(phys);
                            }

                            // define size (lines)
                            data->updateSize();

                            // exit loop                            
                            ok = true;                            

                        }
                        else if (token == Keyword && mylex.getLexem() == "CURVE")
                        {
                            //parse the line where identifier CURVE until EOL
                            token = mylex.getNextToken(in);
                            while (token != Eol && token != Eof)
                                token = mylex.getNextToken(in);

                            // check for 2x Indentation : [<Anything>]<value separator>[<Anything>]<value separator>
                            int count = 0;
                            while (count < 2)
                            {
                                token = mylex.getNextToken(in);
                                if (token == ValueSeparator)
                                    count++;
                            }

                            // increment the number of rows
                            int nRows = 1;

                            // in case a label is twice in the subset!!
                            data->clearZ();

                            // read the Z values
                            token = mylex.getNextToken(in);
                            QStringList list;
                            mylex.initialize();
                            while (token ==  Integer || token == Float || token  == String)
                            {
                                QString str = mylex.getLexem().c_str();
                                str.replace(",",".");
                                str.remove("\"");
                                list.append(str);
                                token = mylex.getNextToken(in);
                                if (token == ValueSeparator)
                                    token = mylex.getNextToken(in);
                            }
                            int nCols = list.count() / nRows;

                            // copy z values to data
                            foreach (QString str, list)
                            {
                                // convert PHYS value from Csv into HEX value
                                QString hex = data->phys2hex(str, "z");

                                // convert HEX value into PHYS value
                                QString phys = data->hex2phys(hex, "z");

                                // copy PHYS value into Zaxis
                                data->appendZ(phys);
                            }

                            // create the x-points if it is a curve
                            AXIS_DESCR *axisDescrX = data->getAxisDescrX();
                            if (axisDescrX)
                            {
                                QString typeAxisX = axisDescrX->getPar("Attribute");
                                if (typeAxisX == "FIX_AXIS")
                                {
                                    //OFFSET, SHIFT and NUMBERAPO
                                    FIX_AXIS_PAR *fixAxisPar = (FIX_AXIS_PAR*)axisDescrX->getItem("FIX_AXIS_PAR");
                                    FIX_AXIS_PAR_DIST *fixAxisParDist = (FIX_AXIS_PAR_DIST*)axisDescrX->getItem("FIX_AXIS_PAR_DIST");
                                    if (fixAxisPar)
                                    {
                                        QString off = fixAxisPar->getPar("Offset");
                                        QString sft = fixAxisPar->getPar("Shift");
                                        QString napo = fixAxisPar->getPar("Numberapo");
                                        bool bl;
                                        int offset = off.toInt(&bl, 10);
                                        int shift = sft.toInt(&bl, 10);
                                        int nPtsX = napo.toUInt(&bl, 10);

                                        //check if nPts < nPtsmax
                                        QString maxAxisPts = axisDescrX->getPar("MaxAxisPoints");
                                        double nmaxPts = maxAxisPts.toDouble();
                                        if (nPtsX > nmaxPts)
                                            nPtsX = nmaxPts;

                                        QString str;
                                        for (int i = 0; i < nPtsX; i++)
                                        {
                                            str.setNum((int)(offset + i * qPow(2, shift)), 16);
                                            data->appendX(data->hex2phys(str, "x"));
                                        }
                                    }
                                    else if (fixAxisParDist)
                                    {
                                        QString off = fixAxisParDist->getPar("Offset");
                                        QString dist = fixAxisParDist->getPar("Distance");
                                        QString napo = fixAxisParDist->getPar("Numberapo");
                                        bool bl;
                                        int offset = off.toInt(&bl, 10);
                                        int distance = dist.toInt(&bl, 10);
                                        int nPtsX = napo.toUInt(&bl, 10);

                                        //check if nPts < nPtsmax
                                        QString maxAxisPts = data->getAxisDescrX()->getPar("MaxAxisPoints");
                                        double nmaxPts = maxAxisPts.toDouble();
                                        if (nPtsX > nmaxPts)
                                            nPtsX = nmaxPts;

                                        QString str;
                                        for (int i = 0; i < nPtsX; i++)
                                        {
                                            str.setNum((int)(offset + i * distance));
                                            data->appendX(data->hex2phys(str, "x"));
                                        }
                                    }
                                }
                                else
                                {
                                    for (int i = 0; i < nCols; i++)
                                    {
                                        data->appendX(QString::number(i));
                                    }
                                }
                            }

                            // define size (lines)
                            data->updateSize();

                            // exit loop
                            ok = true;

                        }
                        else if (token == Keyword && mylex.getLexem() == "MAP")
                        {
                            //count 2 lines forward
                            int count = 0;
                            while (count < 2)
                            {
                                 token = mylex.getNextToken(in);
                                 if (token == Eol)
                                     count++;
                            }

                            //count 2 identation forward
                            count = 0;
                            while (count < 2)
                            {
                                 token = mylex.getNextToken(in);
                                 if (token == ValueSeparator)
                                     count++;
                            }

                            // increment the number of rows
                            int nRows = 0;

                            // in case a label is twice in the subset!!
                            data->clearZ();

                            // read the Z values
                            QStringList list;
                            token = mylex.getNextToken(in);

                            while (token ==  Integer || token == Float || token  == String)
                            {
                                QString str = mylex.getLexem().c_str();
                                str.replace(",",".");
                                str.remove("\"");
                                list.append(str);
                                token = mylex.getNextToken(in);
                                if (token == ValueSeparator)
                                    token = mylex.getNextToken(in);
                                else if (token == Eol) //new line
                                {
                                    nRows++;
                                    token = mylex.getNextToken(in);
                                    if (token != Eol)
                                    {
                                        //count 2 identation forward
                                        count = 0;
                                        if (token == ValueSeparator)
                                            count = 1;
                                        while (count < 2)
                                        {
                                             token = mylex.getNextToken(in);
                                             if (token == ValueSeparator)
                                                 count++;
                                        }
                                        token = mylex.getNextToken(in);
                                    }
                                }
                            }
                            if (nRows == 0)
                                nRows++;
                            int nCols = list.count() / nRows;

                            // copy z values to data
                            foreach (QString str, list)
                            {
                                // convert PHYS value from Csv into HEX value
                                QString hex = data->phys2hex(str, "z");

                                // convert HEX value into PHYS value
                                QString phys = data->hex2phys(hex, "z");

                                // copy PHYS value into Zaxis
                                data->appendZ(phys);
                            }

                            // create the x-points if it is a curve
                            AXIS_DESCR *axisDescrX = data->getAxisDescrX();
                            if (axisDescrX)
                            {
                                QString typeAxisX = axisDescrX->getPar("Attribute");
                                if (typeAxisX == "FIX_AXIS")
                                {
                                    //OFFSET, SHIFT and NUMBERAPO
                                    FIX_AXIS_PAR *fixAxisPar = (FIX_AXIS_PAR*)axisDescrX->getItem("FIX_AXIS_PAR");
                                    FIX_AXIS_PAR_DIST *fixAxisParDist = (FIX_AXIS_PAR_DIST*)axisDescrX->getItem("FIX_AXIS_PAR_DIST");
                                    if (fixAxisPar)
                                    {
                                        QString off = fixAxisPar->getPar("Offset");
                                        QString sft = fixAxisPar->getPar("Shift");
                                        QString napo = fixAxisPar->getPar("Numberapo");
                                        bool bl;
                                        int offset = off.toInt(&bl, 10);
                                        int shift = sft.toInt(&bl, 10);
                                        int nPtsX = napo.toUInt(&bl, 10);

                                        //check if nPts < nPtsmax
                                        QString maxAxisPts = axisDescrX->getPar("MaxAxisPoints");
                                        double nmaxPts = maxAxisPts.toDouble();
                                        if (nPtsX > nmaxPts)
                                            nPtsX = nmaxPts;

                                        QString str;
                                        for (int i = 0; i < nPtsX; i++)
                                        {
                                            str.setNum((int)(offset + i * qPow(2, shift)), 16);
                                            data->appendX(data->hex2phys(str, "x"));
                                        }
                                    }
                                    else if (fixAxisParDist)
                                    {
                                        QString off = fixAxisParDist->getPar("Offset");
                                        QString dist = fixAxisParDist->getPar("Distance");
                                        QString napo = fixAxisParDist->getPar("Numberapo");
                                        bool bl;
                                        int offset = off.toInt(&bl, 10);
                                        int distance = dist.toInt(&bl, 10);
                                        int nPtsX = napo.toUInt(&bl, 10);

                                        //check if nPts < nPtsmax
                                        QString maxAxisPts = data->getAxisDescrX()->getPar("MaxAxisPoints");
                                        double nmaxPts = maxAxisPts.toDouble();
                                        if (nPtsX > nmaxPts)
                                            nPtsX = nmaxPts;

                                        QString str;
                                        for (int i = 0; i < nPtsX; i++)
                                        {
                                            str.setNum((int)(offset + i * distance));
                                            data->appendX(data->hex2phys(str, "x"));
                                        }
                                    }
                                }
                                else
                                {
                                    for (int i = 0; i < nCols; i++)
                                    {
                                        data->appendX(QString::number(i));
                                    }
                                }
                            }

                            // create the y-points if it is a map
                            AXIS_DESCR *axisDescrY = data->getAxisDescrY();
                            if (axisDescrY)
                            {
                                QString typeAxisY = axisDescrY->getPar("Attribute");
                                if (typeAxisY == "FIX_AXIS")
                                {
                                    //OFFSET, SHIFT and NUMBERAPO
                                    FIX_AXIS_PAR *fixAxisPar = (FIX_AXIS_PAR*)axisDescrY->getItem("FIX_AXIS_PAR");
                                    FIX_AXIS_PAR_DIST *fixAxisParDist = (FIX_AXIS_PAR_DIST*)axisDescrY->getItem("FIX_AXIS_PAR_DIST");
                                    if (fixAxisPar)
                                    {
                                        QString off = fixAxisPar->getPar("Offset");
                                        QString sft = fixAxisPar->getPar("Shift");
                                        QString napo = fixAxisPar->getPar("Numberapo");
                                        bool bl;
                                        int offset = off.toInt(&bl, 10);
                                        int shift = sft.toInt(&bl, 10);
                                        int nPtsY = napo.toUInt(&bl, 10);

                                        //check if nPts < nPtsmax
                                        QString maxAxisPts = axisDescrY->getPar("MaxAxisPoints");
                                        double nmaxPts = maxAxisPts.toDouble();
                                        if (nPtsY > nmaxPts)
                                            nPtsY = nmaxPts;

                                        QString str;
                                        for (int i = 0; i < nPtsY; i++)
                                        {
                                            str.setNum((int)(offset + i * qPow(2, shift)));
                                            data->appendY(data->hex2phys(str, "y"));
                                        }
                                    }
                                    else if (fixAxisParDist)
                                    {
                                        QString off = fixAxisParDist->getPar("Offset");
                                        QString dist = fixAxisParDist->getPar("Distance");
                                        QString napo = fixAxisParDist->getPar("Numberapo");
                                        bool bl;
                                        int offset = off.toInt(&bl, 10);
                                        int distance = dist.toInt(&bl, 10);
                                        int nPtsY = napo.toUInt(&bl, 10);

                                        //check if nPts < nPtsmax
                                        QString maxAxisPts = data->getAxisDescrY()->getPar("MaxAxisPoints");
                                        double nmaxPts = maxAxisPts.toDouble();
                                        if (nPtsY > nmaxPts)
                                            nPtsY = nmaxPts;

                                        QString str;
                                        for (int i = 0; i < nPtsY; i++)
                                        {
                                            str.setNum((int)(offset + i * distance));
                                            data->appendY(data->hex2phys(str, "y"));
                                        }
                                    }
                                }
                                else
                                {
                                    for (int i = 0; i < nRows; i++)
                                    {
                                        data->appendY(QString::number(i));
                                    }
                                }
                            }

                            // define size (lines)
                            data->updateSize();

                            // exit loop
                            ok = true;

                        }
                        else if (token == Keyword && ( mylex.getLexem() == "VAL_BLK"))
                        {
                            // check for Indentation
                            int count = 0;
                            while (count < 2)
                            {
                                token = mylex.getNextToken(in);
                                if (token == ValueSeparator)
                                    count++;
                            }
                            token = mylex.getNextToken(in);

                            // increment the number of rows
                            int nRows = 0;

                            // in case a label is twice in the subset!!
                            data->clearZ();

                            // read the Z values
                            QStringList list;

                            while (token ==  Integer || token == Float || token  == String)
                            {
                                QString str = mylex.getLexem().c_str();
                                str.replace(",",".");
                                str.remove("\"");
                                list.append(str);
                                token = mylex.getNextToken(in);
                                if (token == ValueSeparator)
                                {
                                    token = mylex.getNextToken(in);
                                }
                                else if (token == Eol) //new line
                                {
                                    nRows++;
                                    token = mylex.getNextToken(in);
                                    if (token != Eol)
                                    {
                                        //count 2 identation forward
                                        count = 0;
                                        if (token == ValueSeparator)
                                            count = 1;
                                        while (count < 2)
                                        {
                                             token = mylex.getNextToken(in);
                                             if (token == ValueSeparator)
                                                 count++;
                                        }
                                        token = mylex.getNextToken(in);
                                    }
                                }
                            }
                            if (nRows == 0)
                                nRows++;
                            //int nCols = list.count() / nRows;

                            // copy z values to data
                            foreach (QString str, list)
                            {
                                // convert PHYS value from Csv into HEX value
                                QString hex = data->phys2hex(str, "z");

                                // convert HEX value into PHYS value
                                QString phys = data->hex2phys(hex, "z");

                                // copy PHYS value into Zaxis
                                data->appendZ(phys);
                            }

                            // check length of axisX (when import Csv into project, it is possible that
                            // the length specified into the A2l is longer than the ine into csv
//                            if (data->xCount() > data->zCount())
//                            {
//                                while (data->zCount() != data->xCount())
//                                {
//                                    data->removeIndexX(data->xCount() - 1);
//                                }

//                            }

                            // define size (lines)
                            data->updateSize();

                            // exit loop
                            ok = true;

                        }
                        else if (token == Keyword && mylex.getLexem() == "AXIS_PTS")
                        {
                            // check for Indentation
                            int count = 0;
                            while (count < 2)
                            {
                                token = mylex.getNextToken(in);
                                if (token == ValueSeparator)
                                    count++;
                            }
                            token = mylex.getNextToken(in);

                            // read the Z values
                            QStringList list;
                            while (token ==  Integer || token == Float || token  == String)
                            {
                                QString str = mylex.getLexem().c_str();
                                str.replace(",",".");
                                str.remove("\"");
                                list.append(str);
                                token = mylex.getNextToken(in);
                                if (token == ValueSeparator)
                                    token = mylex.getNextToken(in);
                            }

                            // add Z values to data
                            data->clearZ(); // in case a label is twice in the subset!!
                            foreach (QString str, list)
                            {
                                // convert PHYS value from Csv into HEX value
                                QString hex = data->phys2hex(str, "z");

                                // convert HEX value into PHYS value
                                QString phys = data->hex2phys(hex, "z");

                                // copy PHYS value into Zaxis
                                data->appendZ(phys);
                            }

                            // add X values to data
                            data->clearX(); // in case a label is twice in the subset!!
                            for (int i = 0; i < data->zCount(); i++)
                            {
                                data->appendX(QString::number(i));
                            }

                            // define size (lines)
                            data->updateSize();

                            // exit loop
                            ok = true;
                        }
                        else if (token == Keyword && mylex.getLexem() == "X_AXIS_PTS")
                        {
                            // check for Indentation
                            int count = 0;
                            while (count < 2)
                            {
                                token = mylex.getNextToken(in);
                                if (token == ValueSeparator)
                                    count++;
                            }
                            token = mylex.getNextToken(in);

                            QStringList list;
                            while (token == Integer || token == Float || token == String)
                            {
                                QString str = mylex.getLexem().c_str();
                                str.replace(",",".");
                                str.remove("\"");
                                list.append(str);
                                token = mylex.getNextToken(in);
                                if (token == ValueSeparator)
                                    token = mylex.getNextToken(in);
                            }

                            data->clearX();
                            foreach (QString str, list)
                            {
                                // convert PHYS value from Csv into HEX value
                                QString hex = data->phys2hex(str, "x");

                                // convert HEX value into PHYS value
                                QString phys = data->hex2phys(hex, "x");

                                // copy PHYS value into Zaxis
                                data->appendX(phys);
                            }
                            data->updateSize();

                            // exit loop
                            data->isAxisXComparable = true;
                            ok = true;
                        }
                        else if (token == Keyword && mylex.getLexem() == "Y_AXIS_PTS")
                        {
                            // check for Indentation
                            int count = 0;
                            while (count < 2)
                            {
                                token = mylex.getNextToken(in);
                                if (token == ValueSeparator)
                                    count++;
                            }
                            token = mylex.getNextToken(in);

                            QStringList list;
                            while (token == Integer ||token == Float || token == String)
                            {
                                QString str = mylex.getLexem().c_str();
                                str.replace(",",".");
                                str.remove("\"");
                                list.append(str);
                                token = mylex.getNextToken(in);
                                if (token == ValueSeparator)
                                    token = mylex.getNextToken(in);
                            }

                            data->clearY();
                            foreach (QString str, list)
                            {
                                // convert PHYS value from Csv into HEX value
                                QString hex = data->phys2hex(str, "y");

                                // convert HEX value into PHYS value
                                QString phys = data->hex2phys(hex, "y");

                                // copy PHYS value into Zaxis
                                data->appendY(phys);
                            }
                            data->updateSize();

                            // exit loop
                            data->isAxisYComparable = true;
                            ok = true;
                        }
                        else if (token == Keyword && mylex.getLexem() == "ASCII")
                        {

                            // check for Indentation
                            int count = 0;
                            while (count < 2)
                            {
                                token = mylex.getNextToken(in);
                                if (token == ValueSeparator)
                                    count++;
                            }

                            // in case a label is twice in the subset!!
                            data->clearZ();

                            // read the ASCII text
                            QString text;
                            token = mylex.getNextToken(in);
                            if (token == String)
                            {
                                text = mylex.getLexem().c_str();
                                token = mylex.getNextToken(in);
                            }

                            // convert text to char array
                            int asciiVal;
                            for(int i = 0; i < data->xCount(); i++)
                            {
                                if (i < text.length() - 2)
                                {
                                    asciiVal = text.at(i + 1).toLatin1();
                                    data->appendZ(QString::number(asciiVal));
                                }
                                else
                                {
                                    data->appendZ("0");
                                }
                            }

                            // define size (lines)
                            data->updateSize();

                            // exit loop
                            ok = true;
                        }
                        else if (token == Comment)
                        {

                        }
                        else
                        {
                            showError("CSV parser error at line " + QString::number(mylex.getLine())
                                      + " with lexem " + QString(mylex.getLexem().c_str()));
                            return false;
                        }
                    }
                }
                else
                {
                    token = mylex.getNextToken(in);
                    while (token != Identifier && token != Eof)
                    {
                       token = mylex.getNextToken(in);
                    }
                }
            }
            else if (token == Comment)
            {
                token = mylex.getNextToken(in);
            }
            else if (token == Eof)
            {
                return true;
            }
            else if (token == ValueSeparator)
            {
                token = mylex.getNextToken(in);
            }
            else
            {
                showError("CSV parser error at line " + QString::number(mylex.getLine())
                          + " with lexem " + QString(mylex.getLexem().c_str()));
                return false;
            }
        }

        this->getParentWp()->parentWidget->writeOutput(outList);
        return true;
    }
    else
    {
        showError("CSV parser error at line " + QString::number(mylex.getLine())
                  + " with lexem " + QString(mylex.getLexem().c_str()));
        return false;
    }
}

bool Csv::save(QString fileName)
{

    if (fileName == "")
    {
      if (exportDataList2Csv(getListNameData(), this->fullName()))
      {
          //remove childrens nodes
          foreach (Data* data, modifiedData)
          {
              data->clearOldValues();
              removeModifiedData(data);
          }

          return true;
      }
      else
          return false;
    }
    else
    {
        if (exportDataList2Csv(getListNameData(), fileName))
        {
           //remove childrens nodes
            foreach (Data* data, modifiedData)
            {
                data->clearOldValues();
                removeModifiedData(data);
            }

            return true;
        }
        else
            return false;
    }
}

// ____________ Other Methods ____________ //

QStringList Csv::getListNameData()
{
    QStringList list;
    foreach (Data *data, listData)
    {
        list.append(data->name);
    }
    return list;
}

void Csv::checkDisplay()
{
    //check if parentWp still exists
    if (!parentNode->getParentNode()->isChild(this->parentNode->name))
    {
        int ret = QMessageBox::warning(0, "HEXplorer :: add project",
                                        "This project was deleted !\nDo you want to reload it ?",
                                        QMessageBox::Ok, QMessageBox::Cancel);

        if (ret == QMessageBox::Ok)
        {
            MDImain *mdi = getParentWp()->parentWidget;
            mdi->reAppendProject(getParentWp());
        }
    }

    //check if this is alaways a child of its parentWp
    if (!parentNode->isChild(this->name))
    {

        int ret = QMessageBox::warning(0, "HEXplorer :: add CSVFile",
                                        "This CSV file was deleted !\nDo you want to reload it ?",
                                        QMessageBox::Ok, QMessageBox::Cancel);

        if (ret == QMessageBox::Ok)
        {
            WorkProject *wp = getParentWp();
            wp->addCsv(this);
            this->attach(wp);
        }
    }
}

void Csv::addLabelsAsChild()
{
    foreach (Data *data, listData)
    {
        data->setParentNode(this);
        this->addChildNode(data);
        getParentWp()->treeModel->dataInserted(this, childNodes.indexOf(data));
    }
}

QString Csv::fullName()
{
    return fullPath;
}

void Csv::setFullName(QString fullName)
{
    fullPath = fullName;
    WorkProject *wp = getParentWp();
    wp->rename(this);

    // change the name displayed into owners
    foreach(QObject *obj, owners)
    {
        QString type = typeid(*obj).name();
        if (type.toLower().endsWith("formcompare"))
        {
            FormCompare *fcomp = (FormCompare*)obj;
            if (fcomp->getCsv1() == this)
            {
                 QString str = QFileInfo(getParentWp()->getFullA2lFileName().c_str()).fileName()
                               + "/"
                               + QFileInfo(fullPath).fileName();
                 fcomp->setDataset1(str);
            }
            else if (fcomp->getCsv2() == this)
            {
                QString str = QFileInfo(getParentWp()->getFullA2lFileName().c_str()).fileName()
                              + "/"
                              + QFileInfo(fullPath).fileName();
                 fcomp->setDataset2(str);
            }
        }
    }
}

void Csv::attach(QObject *o)
{
    //check owner for validity
    if(o==0)return;

    //check for duplicates
    //if(owners.contains(o))return;

    //register
    owners.append(o);
    connect(o,SIGNAL(destroyed(QObject*)),this,SLOT(detach(QObject*)));

}

void Csv::detach(QObject *o)
{
    //remove it
    //owners.removeAll(o);
    owners.removeOne(o);

    //remove self after last one
    if(owners.size()==0)
        delete this;
}

std::string Csv::pixmap()
{
    return ":/icones/excel.png";
}

void Csv::checkProgressStream(int n)
{
    omp_set_lock(&lock);

    valueProgBar += n;
    emit incProgressBar(valueProgBar, maxValueProgbar);

    omp_unset_lock(&lock);

}

int Csv::getNumByte(std::string str)
{
    return nByte.value(str.c_str());
}
