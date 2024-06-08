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

#include "data.h"
#include <Nodes/characteristic.h>
#include <Nodes/compu_method.h>
#include <Nodes/mod_common.h>
#include <Nodes/axis_pts.h>
#include <Nodes/function.h>
#include <Nodes/project.h>
#include <hexfile.h>
#include <srecfile.h>
#include <typeinfo>
#include <math.h>
#include <QMessageBox>
#include <limits.h>
#include "graph.h"
#include "workproject.h"
#include "dialogexceedwb.h"
#include "csv.h"
#include "cdfxfile.h"
#include "dcmfile.h"
#include <QtCore/qmath.h>
#include "qdebug.h"
#include <stdint.h>
//#include <QTextCodec>
//#include <QScriptEngine>

// --- initialise static variables
bool Data::showWeakBoundsExceeded = true;
bool Data::allowExceedWeakBounds = false;

bool myCompare(QString a, QString b)
{
   if (a.toDouble() < b.toDouble())
       return true;
   else
       return false;
}

// --- CONSTRUCTORS --- //

Data::Data() : QObject(), Node()
{

}

Data::Data(Node *node) : QObject(), Node(node->name)
{

    label = node;
    project = NULL;
    hexParent = NULL;
    csvParent = NULL;
    dcmParent = NULL;
    cdfxParent = NULL;
    axisDescrX = NULL;
    axisDescrY = NULL;
    compuVTabAxisX = NULL;
    compuVTabAxisY = NULL;
    compuVTabAxisZ = NULL;
    displayed = false;
    precisionX = 0;
    precisionY = 0;
    precisionZ = 0;
    addressX = 0;
    addressY = 0;
    addressZ = 0;
    type = "";
    isSortedByRow = 0;

}

Data::Data(CHARACTERISTIC *node, PROJECT *pro, HexFile *hexFile, bool modif) : QObject(), Node(node->name)
{
    //initialize settings
    isComplete = true; //created to check if the creation of the data has been completed or aborted due to missing information
    xOrgSize = 0;
    yOrgSize = 0;
    isAxisXComparable = true;
    isAxisYComparable = true;
    sizeChanged = false;
    modifiable = modif;
    label = node;
    project = pro;
    hexParent = hexFile;
    csvParent = NULL;
    cdfxParent = NULL;
    dcmParent = NULL;
    if (hexParent)
    {
        moduleName = hexParent->getModuleName();
    }
    axisDescrX = NULL;
    axisDescrY = NULL;
    compuVTabAxisX = NULL;
    compuVTabAxisY = NULL;
    compuVTabAxisZ = NULL;
    compuTabAxisY = NULL;
    compuTabAxisZ = NULL;
    compu_methodZ = NULL;
    byteOrderX = "";
    byteOrderY = "";
    byteOrderZ = "";
    displayed = false;
    precisionX = 0;
    precisionY = 0;
    precisionZ = 0;
    isSortedByRow = 0;
    uint nPtsAxisX = 0;
    uint nPtsAxisY = 0;
    uint nPtsAxisX_Max = 0;
    uint nPtsAxisY_Max = 0;
    uint nPtsZvaluesX = 0;
    uint nPtsZvaluesY = 0;


    //initialize the display number of rows of the CHARACTERISTIC
    type = node->getPar("Type");
    if (type.compare("VALUE") == 0)
    {
        size = 3;
    }
    else if (type.compare("CURVE") == 0)
    {
        axisDescrX = (AXIS_DESCR*)node->getNode("AXIS_DESCR")->child(0);
        size = 4;
    }
    else if (type.compare("MAP") == 0)
    {
        axisDescrX = (AXIS_DESCR*)node->getNode("AXIS_DESCR")->child(0);
        axisDescrY = (AXIS_DESCR*)node->getNode("AXIS_DESCR")->child(1);
        size = 4;
    }
    else if (type.compare("VAL_BLK") == 0)
    {
        bool bl;
        NUMBER *item =  (NUMBER*)node->getItem("NUMBER");
        MATRIX_DIM *matrix_dim =  (MATRIX_DIM*)node->getItem("MATRIX_DIM");
        if (item)
        {
            QString toto = item->getPar("Number");
            nPtsAxisX = toto.toInt(&bl, 10);
        }
        else if (matrix_dim)
        {
           nPtsAxisX = ((QString)matrix_dim->getPar("xDim")).toInt();
           nPtsAxisY = ((QString)matrix_dim->getPar("yDim")).toInt();
        }
        else
        {
            nPtsAxisX = 1;
            nPtsAxisY = 1;
        }

        addressX = 0;
        addressY = 0;

        //AXIS_PTS_X
        QString str;
        for (uint i = 0; i < nPtsAxisX; i++)
        {
            listX.append(str.setNum(i));
        }

        //AXIS_PTS_Y
        if (nPtsAxisY > 1)
        {
            for (uint i = 0; i < nPtsAxisY; i++)
            {
                listY.append(str.setNum(i));
            }
        }

        size = 4;
    }
    else if (type.compare("ASCII") == 0)
    {
        bool bl;
        NUMBER *item = (NUMBER*)node->getItem("NUMBER");
        nPtsAxisX = ((QString)item->getPar("Number")).toInt(&bl, 10);


        //AXIS_PTS_X
        QString str;
        for (uint i = 0; i < nPtsAxisX; i++)
        {
            listX.append(str.setNum(i));
        }
        size = 4;
    }
    else
    {
        size = 3;
    }

    //In case of axis : read AXIS_X if axisX is a COM_AXIS or a FIX_AXIS
    if (axisDescrX)
    {
        //Xaxis PRECISION
        FORMAT *format = (FORMAT*)axisDescrX->getItem("FORMAT");
        if (format)
        {
            QString f = format->getPar("FormatString");
            QStringList list = f.split(QRegularExpression("\\D+"));
            if (list.count() < 3)
                precisionX = 0;
            else
                precisionX = list.at(2).toInt();
        }
        else
        {
            QString compu = axisDescrX->getPar("Conversion");
            COMPU_METHOD *cmp = (COMPU_METHOD*)pro->getNode("MODULE/" + moduleName + "/COMPU_METHOD/" + compu);
            if (cmp)
            {
                QString f = cmp->getPar("Format");
                QStringList list = f.split(QRegularExpression("\\D+"));
                if (list.count() < 3)
                    precisionX = 0;
                else
                    precisionX = list.at(2).toInt();
            }
        }

        QString typeAxisX = axisDescrX->getPar("Attribute");
        if (typeAxisX.compare("COM_AXIS") == 0)
        {
            //AXIS_PTS
            AXIS_PTS_REF *axisPtsRef = (AXIS_PTS_REF*)axisDescrX->getItem("AXIS_PTS_REF");
            QString nameAxisX = axisPtsRef->getPar("AxisPoints");
            AXIS_PTS *axisPtsX = (AXIS_PTS*)project->getNode("MODULE/" + moduleName + "/AXIS_PTS/" + nameAxisX);

            //BYTE_ORDER axisX
            Byte_Order *byteOrder = (Byte_Order*)axisPtsX->getItem("BYTE_ORDER");
            if (byteOrder)
            {
                byteOrderX = byteOrder->getPar("ByteOrder");
            }


            //number of points X Maximum
            QString maxAxisPts = axisDescrX->getPar("MaxAxisPoints");
            nPtsAxisX_Max = maxAxisPts.toUInt();

            //axisPts RECORD_LAYOUT
            QString deposit = axisPtsX->getPar("Deposit");
            RECORD_LAYOUT *record_layout = (RECORD_LAYOUT*)hexFile->record_layout->getNode(deposit);


            //read each element of X_RECORD_LAYOUT
            bool bl;
            int offset = 0;
            foreach (Item *item, record_layout->optItems)
            {
                QString type = item->name;

                if (type == "SRC_ADDR_X")
                {
                    std::string datatype = ((SRC_ADDR_X*)item)->getPar("Datatype");

                    //if ALIGNMENT is explicit into RECORD_LAYOUT

                    //else use ALIGNMENT into MOD_COMMON
                    offset += hexParent->getNumByte(datatype);
                }
                else if (type == "NO_AXIS_PTS_X")
                {
                    std::string datatype = ((NO_AXIS_PTS_X*)item)->getPar("Datatype");
                    int nbyte = hexParent->getNumByte(datatype);
                    QString val = hexParent->getHexValue(axisPtsX->getPar("Adress"), offset, nbyte, byteOrderX);
                    bool bl;
                    nPtsAxisX = val.toInt(&bl,16);
                    offset += nbyte;

                    //check if nPts < nPtsmax
                    if (nPtsAxisX > nPtsAxisX_Max)
                        nPtsAxisX = nPtsAxisX_Max;
                }
                else if (type == "AXIS_PTS_X")
                {
                    //read the necessary parameters before reading values into HexFile
                    datatypeX = ((AXIS_PTS_X*)item)->getPar("Datatype");
                    int Xnbyte = hexParent->getNumByte(datatypeX);
                    addressX = QString(axisPtsX->getPar("Adress")).toUInt(&bl, 16) + offset;

                    //read values into HexFile
                    if (nPtsAxisX == 0)
                        nPtsAxisX = nPtsAxisX_Max;
                    QList<double> decX = hexParent->getDecValues(addressX, Xnbyte, nPtsAxisX, datatypeX, byteOrderX);

                    //convert the dec values into phys values
                    listX = dec2Phys(decX, "x");

                    //increment offset of the axisX length
                    offset +=  nPtsAxisX * Xnbyte;
                }
            }

        }
        else if (typeAxisX.compare("FIX_AXIS") == 0)
        {
            //do not compare X axis
            isAxisXComparable = false;

            //number of points X Maximum
            QString maxAxisPts = axisDescrX->getPar("MaxAxisPoints");
            nPtsAxisX_Max = maxAxisPts.toUInt();

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
                nPtsAxisX = napo.toUInt(&bl, 10);

                //check if nPts < nPtsmax
                if (nPtsAxisX > nPtsAxisX_Max)
                    nPtsAxisX = nPtsAxisX_Max;


                QList<int> numberApo;
                for (uint i = 0; i < nPtsAxisX; i++)
                {
                    //str.setNum((int)(offset + i * qPow(2, shift)), 10);
                    //listX.append(str);
                    numberApo.append((int)(offset + i * qPow(2, shift)));
                }

                QString compu_method = axisDescrX->getPar("Conversion");
                COMPU_METHOD *cmp = (COMPU_METHOD*)project->getNode("MODULE/" + moduleName + "/COMPU_METHOD/" + compu_method);
                QString convType = cmp->getPar("ConversionType");

                if (convType.toLower() == "tab_verb")
                {
                    COMPU_TAB_REF *item = (COMPU_TAB_REF*)cmp->getItem("COMPU_TAB_REF");
                    QString compuTabRef = item->getPar("ConversionTable");
                    compuVTabAxisX = (COMPU_VTAB*)project->getNode("MODULE/" + moduleName + "/COMPU_VTAB/" + compuTabRef);

                    for (uint i = 0; i < nPtsAxisX; i++)
                    {
                        listX.append(compuVTabAxisX->getValue(numberApo.at(i)));
                    }
                }
                else
                {
                    QString str;
                    for (uint i = 0; i < nPtsAxisX; i++)
                    {
                        str.setNum(numberApo.at(i), 10);
                        listX.append(str);
                    }
                }


                addressX = 0;
            }
            else if (fixAxisParDist)
            {
                QString off = fixAxisParDist->getPar("Offset");
                QString dist = fixAxisParDist->getPar("Distance");
                QString napo = fixAxisParDist->getPar("Numberapo");
                bool bl;
                int offset = off.toInt(&bl, 10);
                int distance = dist.toInt(&bl, 10);
                nPtsAxisX = napo.toUInt(&bl, 10);

                //check if nPts < nPtsmax
                if (nPtsAxisX > nPtsAxisX_Max)
                    nPtsAxisX = nPtsAxisX_Max;

                QString str;
                for (uint i = 0; i < nPtsAxisX; i++)
                {
                    str.setNum((int)(offset + i * distance), 10);
                    listX.append(str);
                }

                addressX = 0;
            }
        }
        else if (typeAxisX.compare("STD_AXIS") == 0)
        {
            //BYTE_ORDER axisX
            Byte_Order *byteOrder = (Byte_Order*)axisDescrX->getItem("BYTE_ORDER");
            if (byteOrder)
            {
                byteOrderX = byteOrder->getPar("ByteOrder");
            }

            //number of points X Maximum
            QString maxAxisPts = axisDescrX->getPar("MaxAxisPoints");
            nPtsAxisX_Max = maxAxisPts.toUInt();
        }
    }

    //In case of axis :read AXIS_Y if axisY is a COM_AXIS or a FIX_AXIS
    if (axisDescrY)
    {
        //Yaxis PRECISION
        FORMAT *format = (FORMAT*)axisDescrY->getItem("FORMAT");
        if (format)
        {
            QString f = format->getPar("FormatString");
            QStringList list = f.split(QRegularExpression("\\D+"));
            if (list.count() < 3)
                precisionY = 0;
            else
                precisionY = list.at(2).toInt();
        }
        else
        {
            QString compu = axisDescrY->getPar("Conversion");
            COMPU_METHOD *cmp = (COMPU_METHOD*)pro->getNode("MODULE/" + moduleName + "/COMPU_METHOD/" + compu);
            if (cmp)
            {
                QString f = cmp->getPar("Format");
                QStringList list = f.split(QRegularExpression("\\D+"));
                if (list.count() < 3)
                    precisionY = 0;
                else
                    precisionY = list.at(2).toInt();
            }
        }

        QString typeAxisY = axisDescrY->getPar("Attribute");
        if (typeAxisY.compare("COM_AXIS") == 0)
        {

            //AXIS_PTS
            AXIS_PTS_REF *axisPtsRef = (AXIS_PTS_REF*)axisDescrY->getItem("AXIS_PTS_REF");
            QString nameAxisY = axisPtsRef->getPar("AxisPoints");
            AXIS_PTS *axisPtsY = (AXIS_PTS*)project->getNode("MODULE/" + moduleName + "/AXIS_PTS/" + nameAxisY);

            //BYTE_ORDER axisY
            Byte_Order *byteOrder = (Byte_Order*)axisPtsY->getItem("BYTE_ORDER");
            if (byteOrder)
            {
                byteOrderY = byteOrder->getPar("ByteOrder");
            }

            //number of points Y Max
            QString maxAxisPts = axisDescrY->getPar("MaxAxisPoints");
            nPtsAxisY_Max = maxAxisPts.toUInt();

            //axisPts RECORD_LAYOUT
            QString deposit = axisPtsY->getPar("Deposit");
            RECORD_LAYOUT *record_layout = (RECORD_LAYOUT*)hexFile->record_layout->getNode(deposit);

            //read each element of X_RECORD_LAYOUT
            bool bl;
            int offset = 0;
            foreach (Item *item, record_layout->optItems)
            {
                QString type = item->name;

                if (type == "SRC_ADDR_X")
                {
                    std::string datatype = ((SRC_ADDR_X*)item)->getPar("Datatype");
                    offset += hexParent->getNumByte(datatype);
                }
                else if (type == "NO_AXIS_PTS_X")
                {
                    std::string datatype = ((NO_AXIS_PTS_X*)item)->getPar("Datatype");
                    int nbyte = hexParent->getNumByte(datatype);
                    QString val = hexParent->getHexValue(axisPtsY->getPar("Adress"), offset, nbyte, byteOrderY);
                    bool bl;
                    nPtsAxisY = val.toInt(&bl,16);
                    offset += nbyte;

                    //check if nPts < nPtsmax
                    if (nPtsAxisY > nPtsAxisY_Max)
                        nPtsAxisY = nPtsAxisY_Max;
                }
                else if (type == "AXIS_PTS_X")
                {
                    //read the necessary parameters before reading values into HexFile
                    datatypeY = ((AXIS_PTS_X*)item)->getPar("Datatype");
                    int Ynbyte = hexParent->getNumByte(datatypeY);
                    addressY = QString(axisPtsY->getPar("Adress")).toUInt(&bl, 16) + offset;

                    //read values into HexFile
                    // get nPtsY if "NO_AXIS_PTS_X" is not specified
                    if (nPtsAxisY == 0) // done in case "NO_AXIS_PTS_X" is not specified => use "MaxAxisPoints"
                        nPtsAxisY = nPtsAxisY_Max;
                    QList<double> decY = hexParent->getDecValues(addressY, Ynbyte, nPtsAxisY, datatypeY, byteOrderY);

                    //convert the dec values into phys values
                    listY = dec2Phys(decY, "y");

                    //increment offset of the axisX length
                    offset +=  nPtsAxisY * Ynbyte;
                }
            }

        }
        else if (typeAxisY.compare("FIX_AXIS") == 0)
        {
            //do not compare Y axis
            isAxisYComparable = false;

            //number of points Y Max
            QString maxAxisPts = axisDescrY->getPar("MaxAxisPoints");
            nPtsAxisY_Max = maxAxisPts.toUInt();

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
                nPtsAxisY = napo.toUInt(&bl, 10);

                //check if nPts < nPtsmax
                if (nPtsAxisY > nPtsAxisY_Max)
                    nPtsAxisY = nPtsAxisY_Max;

                QList<int> numberApo;
                for (uint i = 0; i < nPtsAxisY; i++)
                {
                    //str.setNum((int)(offset + i * qPow(2, shift)), 10);
                    //listX.append(str);
                    numberApo.append((int)(offset + i * qPow(2, shift)));
                }

                QString compu_method = axisDescrY->getPar("Conversion");
                COMPU_METHOD *cmp = (COMPU_METHOD*)project->getNode("MODULE/" + moduleName + "/COMPU_METHOD/" + compu_method);
                QString convType = cmp->getPar("ConversionType");

                if (convType.toLower() == "tab_verb")
                {
                    COMPU_TAB_REF *item = (COMPU_TAB_REF*)cmp->getItem("COMPU_TAB_REF");
                    QString compuTabRef = item->getPar("ConversionTable");
                    compuVTabAxisY = (COMPU_VTAB*)project->getNode("MODULE/" + moduleName + "/COMPU_VTAB/" + compuTabRef);

                    for (uint i = 0; i < nPtsAxisY; i++)
                    {
                        listY.append(compuVTabAxisY->getValue(numberApo.at(i)));
                    }
                }
                else
                {
                    QString str;
                    for (uint i = 0; i < nPtsAxisY; i++)
                    {
                        str.setNum(numberApo.at(i), 10);
                        listY.append(str);
                    }
                }

                addressY = 0;
            }
            else if (fixAxisParDist)
            {
                QString off = fixAxisParDist->getPar("Offset");
                QString dist = fixAxisParDist->getPar("Distance");
                QString napo = fixAxisParDist->getPar("Numberapo");
                bool bl;
                int offset = off.toInt(&bl, 10);
                int distance = dist.toInt(&bl, 10);
                nPtsAxisY = napo.toUInt(&bl, 10);

                //check if nPts < nPtsmax
                if (nPtsAxisY > nPtsAxisY_Max)
                    nPtsAxisY = nPtsAxisY_Max;

                QString str;
                for (uint i = 0; i < nPtsAxisY; i++)
                {
                     str.setNum((int)(offset + i *  distance), 10);
                    listY.append(str);
                }

                addressY = 0;
            }
        }
        else if (typeAxisY.compare("STD_AXIS") == 0)
        {
            //BYTE_ORDER axisY
            Byte_Order *byteOrder = (Byte_Order*)axisDescrY->getItem("BYTE_ORDER");
            if (byteOrder)
            {
                byteOrderY = byteOrder->getPar("ByteOrder");
            }

            //number of points Y Max
            QString maxAxisPts = axisDescrY->getPar("MaxAxisPoints");
            nPtsAxisY_Max = maxAxisPts.toUInt();
        }
    }

    //RECORD_LAYOUT Zvalues
    QString deposit = node->getPar("Deposit");
    record_layout = (RECORD_LAYOUT*)hexFile->record_layout->getNode(deposit);

    //BYTE_ORDER Zvalues
    Byte_Order *byteOrder = (Byte_Order*)label->getItem("BYTE_ORDER");
    if (byteOrder)
    {
        byteOrderZ = byteOrder->getPar("ByteOrder");
    }

    //PRECISION Zvalues
    FORMAT *format = (FORMAT*)node->getItem("FORMAT");
    if (format)
    {
        QString f = format->getPar("FormatString");
        QRegularExpression rx("%\\d*.(\\d*)");
        //rx.indexIn(f);
        QRegularExpressionMatch match = rx.match(f);
        int index = match.capturedStart();
        //QStringList list = rx.capturedTexts();
        QStringList list = match.capturedTexts();
        if (!list.at(1).isEmpty())
        {
            precisionZ = list.at(1).toInt();
        }
    }
    else
    {
        QString compu = node->getPar("Conversion");
        COMPU_METHOD *cmp = (COMPU_METHOD*)hexFile->compu_method->getNode(compu);
        if (cmp)
        {
            QString f = cmp->getPar("Format");
            QRegularExpression rx("%\\d*.(\\d*)");
            //rx.indexIn(f);
            //QStringList list = rx.capturedTexts();
            QRegularExpressionMatch match = rx.match(f);
            QStringList list = match.capturedTexts();
            if (!list.at(1).isEmpty())
            {
                precisionZ = list.at(1).toInt();
            }
        }
    }

    //compu_methodZ
    QString compu_method = ((CHARACTERISTIC*)label)->getPar("Conversion");
    compu_methodZ = (COMPU_METHOD*)hexFile->compu_method->getNode(compu_method);

    //read each element of Z_RECORD_LAYOUT
    bool bl;
    int offset = 0;
    foreach (Item *item, record_layout->optItems)
    {
        QString type = item->name;

        if (type.compare("FNC_VALUES") == 0)
        {
            //dataType
            datatypeZ = ((FNC_VALUES*)item)->getPar("Datatype");

            //column or row Dir
            QString zDir = ((FNC_VALUES*)item)->getPar("IndexMode");
            if (zDir.compare("COLUMN_DIR") == 0)
            {
                isSortedByRow = 0;
            }
            else
            {
                    isSortedByRow = 1;
            }

            //get the decimal data from hexFile
            if (nPtsAxisX  == 0)
                nPtsAxisX = 1;
            if (nPtsAxisY  == 0)
                nPtsAxisY = 1;
            int Znbyte = hexParent->getNumByte(datatypeZ);
            bool bl;
            addressZ = QString(node->getPar("Adress")).toUInt(&bl, 16) + offset;
            QList<double> decZ = hexParent->getDecValues(addressZ, Znbyte, nPtsAxisX * nPtsAxisY, datatypeZ, byteOrderZ);

            //BIT_MASK
            BIT_MASK *_bitmask = (BIT_MASK*)node->getItem("BIT_MASK");
            if (_bitmask)
            {
                uint32_t mask = QString(_bitmask->getPar("Mask")).toUInt(&bl, 16);
                for (int i = 0; i < decZ.count(); i++ )
                {
                    uint32_t _result = (uint32_t)decZ.at(i) & mask;
                    int32_t _decalage =  tzn(mask);
                    _result = _result >> _decalage;
                    decZ[i] = _result;
                }
            }

            //dec2phys
            listZ = dec2Phys(decZ, "z");

        }
        else if (type.compare("SRC_ADDR_X") == 0)
        {
            std::string datatype = ((SRC_ADDR_X*)item)->getPar("Datatype");
            offset += hexParent->getNumByte(datatype);
        }
        else if (type.compare("SRC_ADDR_Y") == 0)
        {
            std::string datatype = ((SRC_ADDR_Y*)item)->getPar("Datatype");
            offset += hexParent->getNumByte(datatype);
        }
        else if (type.compare("NO_AXIS_PTS_X") == 0)
        {
            std::string datatype = ((NO_AXIS_PTS_X*)item)->getPar("Datatype");
            int nbyte = hexParent->getNumByte(datatype);
            double address = QString(node->getPar("Adress")).toUInt(&bl, 16) + offset;
            nPtsZvaluesX = hexParent->getDecValues(address, nbyte, 1, datatype, byteOrderX).at(0);
            offset += nbyte;
            xOrgSize = nPtsZvaluesX;
        }
        else if (type.compare("NO_AXIS_PTS_Y") == 0)
        {
            std::string datatype = ((NO_AXIS_PTS_Y*)item)->getPar("Datatype");
            int nbyte = hexParent->getNumByte(datatype);
            double address = QString(node->getPar("Adress")).toUInt(&bl, 16) + offset;
            nPtsZvaluesY = hexParent->getDecValues(address, nbyte, 1, datatype, byteOrderY).at(0);
            offset += nbyte;
            yOrgSize = nPtsZvaluesY;
        }
        else if (type.compare("AXIS_PTS_X") == 0)
        {
            //read the necessary parameters before reading values into HexFile
            datatypeX = ((AXIS_PTS_X*)item)->getPar("Datatype");
            int Xnbyte = hexParent->getNumByte(datatypeX);
            addressX = QString(node->getPar("Adress")).toUInt(&bl, 16) + offset;

            //read values into HexFile
            if (nPtsAxisX == 0)
                nPtsAxisX = nPtsZvaluesX;
            QList<double> decX = hexParent->getDecValues(addressX, Xnbyte, nPtsAxisX, datatypeX, byteOrderX);

            //convert the dec values into phys values
            listX = dec2Phys(decX, "x");

            //increment offset of the axisX length
            offset +=  nPtsAxisX * Xnbyte;
        }
        else if (type.compare("AXIS_PTS_Y") == 0)
        {
            //read the necessary parameters before reading values into HexFile
            datatypeY = ((AXIS_PTS_Y*)item)->getPar("Datatype");
            int Ynbyte = hexParent->getNumByte(datatypeY);
            addressY = QString(node->getPar("Adress")).toUInt(&bl, 16) + offset;

            //read values into HexFile
            if (nPtsAxisY == 0)
                nPtsAxisY = nPtsZvaluesY;
            QList<double> decY = hexParent->getDecValues(addressY, Ynbyte, nPtsAxisY, datatypeY, byteOrderY);

            //convert the dec values into phys values
            listY = dec2Phys(decY, "y");

            //increment offset of the axisX length
            offset +=  nPtsAxisY * Ynbyte;
        }

    }

    //define number of rows of data to display
    size += nPtsAxisY;
}

Data::Data(CHARACTERISTIC *node, PROJECT *pro, SrecFile *srecFile, bool modif) : QObject(), Node(node->name)
{
    //initialize settings
    isComplete = true; //created to check if the creation of the data has been completed or aborted due to missing information
    xOrgSize = 0;
    yOrgSize = 0;
    isAxisXComparable = true;
    isAxisYComparable = true;
    sizeChanged = false;
    modifiable = modif;
    label = node;
    project = pro;
    hexParent = nullptr;
    srecParent = srecFile;
    csvParent = nullptr;
    cdfxParent = nullptr;
    dcmParent = nullptr;
    if (srecParent)
    {
        moduleName = srecParent->getModuleName();
    }
    axisDescrX = nullptr;
    axisDescrY = nullptr;
    compuVTabAxisX = nullptr;
    compuVTabAxisY = nullptr;
    compuVTabAxisZ = nullptr;
    compuTabAxisX = nullptr;
    compuTabAxisY = nullptr;
    compuTabAxisZ = nullptr;
    compu_methodZ = nullptr;
    byteOrderX = "";
    byteOrderY = "";
    byteOrderZ = "";
    displayed = false;
    precisionX = 0;
    precisionY = 0;
    precisionZ = 0;
    isSortedByRow = 0;
    uint nPtsAxisX = 0;
    uint nPtsAxisY = 0;
    uint nPtsAxisX_Max = 0;
    uint nPtsAxisY_Max = 0;
    uint nPtsZvaluesX = 0;
    uint nPtsZvaluesY = 0;


    //initialize the display number of rows of the CHARACTERISTIC
    type = node->getPar("Type");
    if (type.compare("VALUE") == 0)
    {
        size = 3;
    }
    else if (type.compare("CURVE") == 0)
    {
        axisDescrX = (AXIS_DESCR*)node->getNode("AXIS_DESCR")->child(0);
        size = 4;
    }
    else if (type.compare("MAP") == 0)
    {
        axisDescrX = (AXIS_DESCR*)node->getNode("AXIS_DESCR")->child(0);
        axisDescrY = (AXIS_DESCR*)node->getNode("AXIS_DESCR")->child(1);
        size = 4;
    }
    else if (type.compare("VAL_BLK") == 0)
    {
        bool bl;
        NUMBER *item =  (NUMBER*)node->getItem("NUMBER");
        MATRIX_DIM *matrix_dim =  (MATRIX_DIM*)node->getItem("MATRIX_DIM");
        if (item)
        {
            QString toto = item->getPar("Number");
            nPtsAxisX = toto.toInt(&bl, 10);
        }
        else if (matrix_dim)
        {
           nPtsAxisX = ((QString)matrix_dim->getPar("xDim")).toInt();
           nPtsAxisY = ((QString)matrix_dim->getPar("yDim")).toInt();
        }
        else
        {
            nPtsAxisX = 1;
            nPtsAxisY = 1;
        }

        addressX = 0;
        addressY = 0;

        //AXIS_PTS_X
        QString str;
        for (uint i = 0; i < nPtsAxisX; i++)
        {
            listX.append(str.setNum(i));
        }

        //AXIS_PTS_Y
        if (nPtsAxisY > 1)
        {
            for (uint i = 0; i < nPtsAxisY; i++)
            {
                listY.append(str.setNum(i));
            }
        }

        size = 4;
    }
    else if (type.compare("ASCII") == 0)
    {
        bool bl;
        NUMBER *item = (NUMBER*)node->getItem("NUMBER");
        nPtsAxisX = ((QString)item->getPar("Number")).toInt(&bl, 10);

        //AXIS_PTS_X
        QString str;
        for (uint i = 0; i < nPtsAxisX; i++)
        {
            listX.append(str.setNum(i));
        }
        size = 4;
    }
    else
    {
        size = 3;
    }

    //In case of group axis : read AXIS_X if axisX is a COM_AXIS or a FIX_AXIS
    if (axisDescrX)
    {
        //Xaxis PRECISION
        FORMAT *format = (FORMAT*)axisDescrX->getItem("FORMAT");
        if (format)
        {
            QString f = format->getPar("FormatString");
            QStringList list = f.split(QRegularExpression("\\D+"));
            if (list.count() < 3)
                precisionX = 0;
            else
                precisionX = list.at(2).toInt();
        }
        else
        {
            QString compu = axisDescrX->getPar("Conversion");
            COMPU_METHOD *cmp = (COMPU_METHOD*)pro->getNode("MODULE/" + moduleName + "/COMPU_METHOD/" + compu);
            if (cmp)
            {
                QString f = cmp->getPar("Format");
                QStringList list = f.split(QRegularExpression("\\D+"));
                if (list.count() < 3)
                    precisionX = 0;
                else
                    precisionX = list.at(2).toInt();
            }
        }

        QString typeAxisX = axisDescrX->getPar("Attribute");
        if (typeAxisX.compare("COM_AXIS") == 0)
        {
            //AXIS_PTS
            AXIS_PTS_REF *axisPtsRef = (AXIS_PTS_REF*)axisDescrX->getItem("AXIS_PTS_REF");
            QString nameAxisX = axisPtsRef->getPar("AxisPoints");
            AXIS_PTS *axisPtsX = (AXIS_PTS*)project->getNode("MODULE/" + moduleName + "/AXIS_PTS/" + nameAxisX);
            if (axisPtsX == nullptr)
            {
                isComplete = false;
                return;
            }

            //number of points X Maximum
            QString maxAxisPts = axisDescrX->getPar("MaxAxisPoints");
            nPtsAxisX_Max = maxAxisPts.toUInt();

            //BYTE_ORDER axisX
            Byte_Order *byteOrder = (Byte_Order*)axisPtsX->getItem("BYTE_ORDER");
            if (byteOrder)
            {
                byteOrderX = byteOrder->getPar("ByteOrder");
            }

            //axisPts RECORD_LAYOUT
            QString deposit = axisPtsX->getPar("Deposit");
            RECORD_LAYOUT *record_layout = (RECORD_LAYOUT*)srecFile->record_layout->getNode(deposit);

            //read each element of X_RECORD_LAYOUT
            bool bl;
            int offset = 0;
            foreach (Item *item, record_layout->optItems)
            {
                QString type = item->name;

                if (type == "SRC_ADDR_X")
                {
                    std::string datatype = ((SRC_ADDR_X*)item)->getPar("Datatype");
                    offset += srecParent->getNumByte(datatype);
                }
                else if (type == "NO_AXIS_PTS_X")
                {
                    std::string datatype = ((NO_AXIS_PTS_X*)item)->getPar("Datatype");
                    int nbyte = srecParent->getNumByte(datatype);
                    QString val = srecParent->getHexValue(axisPtsX->getPar("Adress"), offset, nbyte, byteOrderX);
                    bool bl;
                    nPtsAxisX = val.toInt(&bl,16);
                    offset += nbyte;

                    //check if nPts < nPtsmax
                    if (nPtsAxisX > nPtsAxisX_Max)
                        nPtsAxisX = nPtsAxisX_Max;
                }
                else if (type == "AXIS_PTS_X")
                {
                    //read the necessary parameters before reading values into HexFile
                    datatypeX = ((AXIS_PTS_X*)item)->getPar("Datatype");
                    int Xnbyte = srecParent->getNumByte(datatypeX);
                    addressX = QString(axisPtsX->getPar("Adress")).toUInt(&bl, 16) + offset;

                    //read values into HexFile
                    if (nPtsAxisX == 0)
                        nPtsAxisX = nPtsAxisX_Max;
                    QList<double> decX = srecParent->getDecValues(addressX, Xnbyte, nPtsAxisX, datatypeX, byteOrderX);

                    //convert the dec values into phys values
                    listX = dec2Phys(decX, "x");

                    //increment offset of the axisX length
                    offset +=  nPtsAxisX * Xnbyte;
                }
            }

        }
        else if (typeAxisX.compare("FIX_AXIS") == 0)
        {
            //do not compare X axis
            isAxisXComparable = false;

            //number of points X Maximum
            QString maxAxisPts = axisDescrX->getPar("MaxAxisPoints");
            nPtsAxisX_Max = maxAxisPts.toUInt();

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
                nPtsAxisX = napo.toUInt(&bl, 10);

                //check if nPts < nPtsmax
                if (nPtsAxisX > nPtsAxisX_Max)
                    nPtsAxisX = nPtsAxisX_Max;


                QList<int> numberApo;
                for (uint i = 0; i < nPtsAxisX; i++)
                {
                    //str.setNum((int)(offset + i * qPow(2, shift)), 10);
                    //listX.append(str);
                    numberApo.append((int)(offset + i * qPow(2, shift)));
                }

                QString compu_method = axisDescrX->getPar("Conversion");
                COMPU_METHOD *cmp = (COMPU_METHOD*)project->getNode("MODULE/" + moduleName + "/COMPU_METHOD/" + compu_method);
                QString convType = cmp->getPar("ConversionType");

                if (convType.toLower() == "tab_verb")
                {
                    COMPU_TAB_REF *item = (COMPU_TAB_REF*)cmp->getItem("COMPU_TAB_REF");
                    QString compuTabRef = item->getPar("ConversionTable");
                    compuVTabAxisX = (COMPU_VTAB*)project->getNode("MODULE/" + moduleName + "/COMPU_VTAB/" + compuTabRef);

                    for (uint i = 0; i < nPtsAxisX; i++)
                    {
                        listX.append(compuVTabAxisX->getValue(numberApo.at(i)));
                    }
                }
                else
                {
                    QString str;
                    for (uint i = 0; i < nPtsAxisX; i++)
                    {
                        str.setNum(numberApo.at(i), 10);
                        listX.append(str);
                    }
                }


                addressX = 0;
            }
            else if (fixAxisParDist)
            {
                QString off = fixAxisParDist->getPar("Offset");
                QString dist = fixAxisParDist->getPar("Distance");
                QString napo = fixAxisParDist->getPar("Numberapo");
                bool bl;
                int offset = off.toInt(&bl, 10);
                int distance = dist.toInt(&bl, 10);
                nPtsAxisX = napo.toUInt(&bl, 10);

                //check if nPts < nPtsmax
                if (nPtsAxisX > nPtsAxisX_Max)
                    nPtsAxisX = nPtsAxisX_Max;

                QString str;
                for (uint i = 0; i < nPtsAxisX; i++)
                {
                    str.setNum((int)(offset + i * distance), 10);
                    listX.append(str);
                }

                addressX = 0;
            }
        }
        else if (typeAxisX.compare("STD_AXIS") == 0)
        {
            //BYTE_ORDER axisX
            Byte_Order *byteOrder = (Byte_Order*)axisDescrX->getItem("BYTE_ORDER");
            if (byteOrder)
            {
                byteOrderX = byteOrder->getPar("ByteOrder");
            }

            //number of points X Maximum
            QString maxAxisPts = axisDescrX->getPar("MaxAxisPoints");
            nPtsAxisX_Max = maxAxisPts.toUInt();
        }
    }

    //In case of group axis :read AXIS_Y if axisY is a COM_AXIS or a FIX_AXIS
    if (axisDescrY)
    {
        //Yaxis PRECISION
        FORMAT *format = (FORMAT*)axisDescrY->getItem("FORMAT");
        if (format)
        {
            QString f = format->getPar("FormatString");
            QStringList list = f.split(QRegularExpression("\\D+"));
            if (list.count() < 3)
                precisionY = 0;
            else
                precisionY = list.at(2).toInt();
        }
        else
        {
            QString compu = axisDescrY->getPar("Conversion");
            COMPU_METHOD *cmp = (COMPU_METHOD*)pro->getNode("MODULE/" + moduleName + "/COMPU_METHOD/" + compu);
            if (cmp)
            {
                QString f = cmp->getPar("Format");
                QStringList list = f.split(QRegularExpression("\\D+"));
                if (list.count() < 3)
                    precisionY = 0;
                else
                    precisionY = list.at(2).toInt();
            }
        }

        QString typeAxisY = axisDescrY->getPar("Attribute");
        if (typeAxisY.compare("COM_AXIS") == 0)
        {
            //AXIS_PTS
            AXIS_PTS_REF *axisPtsRef = (AXIS_PTS_REF*)axisDescrY->getItem("AXIS_PTS_REF");
            QString nameAxisY = axisPtsRef->getPar("AxisPoints");
            AXIS_PTS *axisPtsY = (AXIS_PTS*)project->getNode("MODULE/" + moduleName + "/AXIS_PTS/" + nameAxisY);

            //BYTE_ORDER axisY
            Byte_Order *byteOrder = (Byte_Order*)axisPtsY->getItem("BYTE_ORDER");
            if (byteOrder)
            {
                byteOrderY = byteOrder->getPar("ByteOrder");
            }

            //number of points Y Maximum
            QString maxAxisPts = axisDescrY->getPar("MaxAxisPoints");
            nPtsAxisY_Max = maxAxisPts.toUInt();

            //axisPts RECORD_LAYOUT
            QString deposit = axisPtsY->getPar("Deposit");
            RECORD_LAYOUT *record_layout = (RECORD_LAYOUT*)srecFile->record_layout->getNode(deposit);

            //read each element of X_RECORD_LAYOUT
            bool bl;
            int offset = 0;
            foreach (Item *item, record_layout->optItems)
            {
                QString type = item->name;

                if (type == "SRC_ADDR_X")
                {
                    std::string datatype = ((SRC_ADDR_X*)item)->getPar("Datatype");
                    offset += srecParent->getNumByte(datatype);
                }
                else if (type == "NO_AXIS_PTS_X")
                {
                    std::string datatype = ((NO_AXIS_PTS_X*)item)->getPar("Datatype");
                    int nbyte = srecParent->getNumByte(datatype);
                    QString val = srecParent->getHexValue(axisPtsY->getPar("Adress"), offset, nbyte, byteOrderY);
                    bool bl;
                    nPtsAxisY = val.toInt(&bl,16);
                    offset += nbyte;

                    //check if nPts < nPtsmax
                    if (nPtsAxisY > nPtsAxisY_Max)
                        nPtsAxisY = nPtsAxisY_Max;
                }
                else if (type == "AXIS_PTS_X")
                {
                    //read the necessary parameters before reading values into HexFile
                    datatypeY = ((AXIS_PTS_X*)item)->getPar("Datatype");
                    int Ynbyte = srecParent->getNumByte(datatypeY);
                    //addressY = QString(node->getPar("Adress")).toUInt(&bl, 16) + offset;
                    addressY = QString(axisPtsY->getPar("Adress")).toUInt(&bl, 16) + offset;

                    // get nPtsY if "NO_AXIS_PTS_X" is not specified
                    if (nPtsAxisY == 0) // done in case "NO_AXIS_PTS_X" is not specified => use "MaxAxisPoints"
                        nPtsAxisY = nPtsAxisY_Max;

                    //read values into HexFile
                    QList<double> decY = srecParent->getDecValues(addressY, Ynbyte, nPtsAxisY, datatypeY, byteOrderY);

                    //convert the dec values into phys values
                    listY = dec2Phys(decY, "y");

                    //increment offset of the axisX length
                    offset +=  nPtsAxisY * Ynbyte;
                }
            }

        }
        else if (typeAxisY.compare("FIX_AXIS") == 0)
        {
            //do not compare Y axis
            isAxisYComparable = false;

            //number of points Y Maximum
            QString maxAxisPts = axisDescrY->getPar("MaxAxisPoints");
            nPtsAxisY_Max = maxAxisPts.toUInt();

            //OFFSET, SHIFT and NUMBERAPO
            FIX_AXIS_PAR *fixAxisPar = (FIX_AXIS_PAR*)axisDescrY->getItem("FIX_AXIS_PAR");
            FIX_AXIS_PAR_DIST *fixAxisParDist = (FIX_AXIS_PAR_DIST*)axisDescrY->getItem("FIX_AXIS_PAR_DIST");
            FIX_AXIS_PAR_LIST *fixAxisParList = (FIX_AXIS_PAR_LIST*)axisDescrY->child("FIX_AXIS_PAR_LIST", false);
            if (fixAxisPar)
            {
                QString off = fixAxisPar->getPar("Offset");
                QString sft = fixAxisPar->getPar("Shift");
                QString napo = fixAxisPar->getPar("Numberapo");
                bool bl;
                int offset = off.toInt(&bl, 10);
                int shift = sft.toInt(&bl, 10);
                nPtsAxisY = napo.toUInt(&bl, 10);

                //check if nPts < nPtsmax
                if (nPtsAxisY > nPtsAxisY_Max)
                    nPtsAxisY = nPtsAxisY_Max;

                QList<int> numberApo;
                for (uint i = 0; i < nPtsAxisY; i++)
                {
                    //str.setNum((int)(offset + i * qPow(2, shift)), 10);
                    //listX.append(str);
                    numberApo.append((int)(offset + i * qPow(2, shift)));
                }

                QString compu_method = axisDescrY->getPar("Conversion");
                COMPU_METHOD *cmp = (COMPU_METHOD*)project->getNode("MODULE/" + moduleName + "/COMPU_METHOD/" + compu_method);
                QString convType = cmp->getPar("ConversionType");

                if (convType.toLower() == "tab_verb")
                {
                    COMPU_TAB_REF *item = (COMPU_TAB_REF*)cmp->getItem("COMPU_TAB_REF");
                    QString compuTabRef = item->getPar("ConversionTable");
                    compuVTabAxisY = (COMPU_VTAB*)project->getNode("MODULE/" + moduleName + "/COMPU_VTAB/" + compuTabRef);

                    for (uint i = 0; i < nPtsAxisY; i++)
                    {
                        listY.append(compuVTabAxisY->getValue(numberApo.at(i)));
                    }
                }
                else
                {
                    QString str;
                    for (uint i = 0; i < nPtsAxisY; i++)
                    {
                        str.setNum(numberApo.at(i), 10);
                        listY.append(str);
                    }
                }

                addressY = 0;
            }
            else if (fixAxisParDist)
            {
                QString off = fixAxisParDist->getPar("Offset");
                QString dist = fixAxisParDist->getPar("Distance");
                QString napo = fixAxisParDist->getPar("Numberapo");
                bool bl;
                int offset = off.toInt(&bl, 10);
                int distance = dist.toInt(&bl, 10);
                nPtsAxisY = napo.toUInt(&bl, 10);

                //check if nPts < nPtsmax
                if (nPtsAxisY > nPtsAxisY_Max)
                    nPtsAxisY = nPtsAxisY_Max;

                QString str;
                for (uint i = 0; i < nPtsAxisY; i++)
                {
                    str.setNum((int)(offset + i *  distance), 10);
                    listY.append(str);
                }

                addressY = 0;
            }
            else if (fixAxisParList)
            {
                nPtsAxisY = fixAxisParList->getAxisValues()->count();
                for (const std::string &str : *fixAxisParList->getAxisValues())
                {
                    listY.append(QString::fromStdString(str));
                }

                addressY = 0;
            }
        }
        else if (typeAxisY.compare("STD_AXIS") == 0)
        {
            //BYTE_ORDER axisY
            Byte_Order *byteOrder = (Byte_Order*)axisDescrY->getItem("BYTE_ORDER");
            if (byteOrder)
            {
                byteOrderY = byteOrder->getPar("ByteOrder");
            }

            //number of points Y Maximum
            QString maxAxisPts = axisDescrY->getPar("MaxAxisPoints");
            nPtsAxisY_Max = maxAxisPts.toUInt();
        }
    }

    //RECORD_LAYOUT
    QString deposit = node->getPar("Deposit");
    record_layout = (RECORD_LAYOUT*)srecFile->record_layout->getNode(deposit);

    //BYTE_ORDER Zvalues
    Byte_Order *byteOrder = (Byte_Order*)label->getItem("BYTE_ORDER");
    if (byteOrder)
    {
        byteOrderZ = byteOrder->getPar("ByteOrder");
    }

    //PRECISION Zvalues
    FORMAT *format = (FORMAT*)node->getItem("FORMAT");
    if (format)
    {
        QString f = format->getPar("FormatString");
        QRegularExpression rx("%\\d*.(\\d*)");
        //rx.indexIn(f);
        //QStringList list = rx.capturedTexts();
        QRegularExpressionMatch match = rx.match(f);
        QStringList list = match.capturedTexts();
        if (!list.at(1).isEmpty())
        {
            precisionZ = list.at(1).toInt();
        }
    }
    else
    {
        QString compu = node->getPar("Conversion");
        COMPU_METHOD *cmp = (COMPU_METHOD*)srecFile->compu_method->getNode(compu);
        if (cmp)
        {
            QString f = cmp->getPar("Format");
            QRegularExpression rx("%\\d*.(\\d*)");
            //rx.indexIn(f);
            //QStringList list = rx.capturedTexts();
            QRegularExpressionMatch match = rx.match(f);
            QStringList list = match.capturedTexts();
            if (!list.at(1).isEmpty())
            {
                precisionZ = list.at(1).toInt();
            }
        }
    }

    //compu_methodZ
    QString compu_method = ((CHARACTERISTIC*)label)->getPar("Conversion");
    compu_methodZ = (COMPU_METHOD*)srecFile->compu_method->getNode(compu_method);

    //read each element of Z_RECORD_LAYOUT
    bool bl;
    int offset = 0;
    foreach (Item *item, record_layout->optItems)
    {
        QString type = item->name;

        if (type.compare("FNC_VALUES") == 0)
        {
            //dataType
            datatypeZ = ((FNC_VALUES*)item)->getPar("Datatype");

            //column or row Dir
            QString zDir = ((FNC_VALUES*)item)->getPar("IndexMode");
            if (zDir.compare("COLUMN_DIR") == 0)
            {
                isSortedByRow = 0;
            }
            else
            {
                    isSortedByRow = 1;
            }

            //get the hex data from hexFile
            int Znbyte = srecParent->getNumByte(datatypeZ);
            bool bl;
            addressZ = QString(node->getPar("Adress")).toUInt(&bl, 16) + offset;

            if (nPtsAxisX  == 0)
                nPtsAxisX = 1;
            if (nPtsAxisY  == 0)
                nPtsAxisY = 1;

            QList<double> decZ = srecParent->getDecValues(addressZ, Znbyte, nPtsAxisX * nPtsAxisY, datatypeZ, byteOrderZ);

            //BIT_MASK
            BIT_MASK *_bitmask = (BIT_MASK*)node->getItem("BIT_MASK");
            if (_bitmask)
            {
                uint32_t mask = QString(_bitmask->getPar("Mask")).toUInt(&bl, 16);
                for (int i = 0; i < decZ.count(); i++ )
                {
                    uint32_t _result = (uint32_t)decZ.at(i) & mask;
                    int32_t _decalage =  tzn(mask);
                    _result = _result >> _decalage;
                    decZ[i] = _result;
                }
            }

            //dec2phys
            listZ = dec2Phys(decZ, "z");

        }
        else if (type.compare("SRC_ADDR_X") == 0)
        {
            std::string datatype = ((SRC_ADDR_X*)item)->getPar("Datatype");
            offset += srecParent->getNumByte(datatype);
        }
        else if (type.compare("SRC_ADDR_Y") == 0)
        {
            std::string datatype = ((SRC_ADDR_Y*)item)->getPar("Datatype");
            offset += srecParent->getNumByte(datatype);
        }
        else if (type.compare("NO_AXIS_PTS_X") == 0)
        {
            std::string datatype = ((NO_AXIS_PTS_X*)item)->getPar("Datatype");
            int nbyte = srecParent->getNumByte(datatype);

            double address = QString(node->getPar("Adress")).toUInt(&bl, 16) + offset;
            nPtsZvaluesX = srecParent->getDecValues(address, nbyte, 1, datatype, byteOrderX).at(0);
            xOrgSize = nPtsZvaluesX;
            offset += nbyte;
        }
        else if (type.compare("NO_AXIS_PTS_Y") == 0)
        {
            std::string datatype = ((NO_AXIS_PTS_Y*)item)->getPar("Datatype");
            int nbyte = srecParent->getNumByte(datatype);

            double address = QString(node->getPar("Adress")).toUInt(&bl, 16) + offset;
            nPtsZvaluesY = srecParent->getDecValues(address, nbyte, 1, datatype, byteOrderY).at(0);
            yOrgSize = nPtsZvaluesY;
            offset += nbyte;            
        }
        else if (type.compare("AXIS_PTS_X") == 0)
        {
            //read the necessary parameters before reading values into HexFile
            datatypeX = ((AXIS_PTS_X*)item)->getPar("Datatype");
            int Xnbyte = srecParent->getNumByte(datatypeX);
            addressX = QString(node->getPar("Adress")).toUInt(&bl, 16) + offset;

            //read values into HexFile
            if (nPtsAxisX == 0)
                nPtsAxisX = nPtsZvaluesX;
            QList<double> decX = srecParent->getDecValues(addressX, Xnbyte, nPtsAxisX, datatypeX, byteOrderX);

            //convert the dec values into phys values
            listX = dec2Phys(decX, "x");

            //increment offset of the axisX length
            offset +=  nPtsAxisX * Xnbyte;
        }
        else if (type.compare("AXIS_PTS_Y") == 0)
        {
            //read the necessary parameters before reading values into HexFile
            datatypeY = ((AXIS_PTS_Y*)item)->getPar("Datatype");
            int Ynbyte = srecParent->getNumByte(datatypeY);
            addressY = QString(node->getPar("Adress")).toUInt(&bl, 16) + offset;

            //read values into HexFile
            if (nPtsAxisY == 0)
                nPtsAxisY = nPtsZvaluesY;
            QList<double> decY = srecParent->getDecValues(addressY, Ynbyte, nPtsAxisY, datatypeY, byteOrderY);

            //convert the dec values into phys values
            listY = dec2Phys(decY, "y");

            //increment offset of the axisX length
            offset +=  nPtsAxisY * Ynbyte;
        }

    }

    //define number of rows of data to display
    size += nPtsAxisY;

    isComplete = true;
}

Data::Data(CHARACTERISTIC *node, PROJECT *pro, Csv *csv, bool modif) : QObject(), Node(node->name)
{
    isComplete = true; //created to check if the creation of the data has been completed or aborted due to missing information
    isAxisXComparable = false;
    isAxisYComparable = false;
    sizeChanged = false;
    modifiable = modif;
    label = node;
    project = pro;
    hexParent = NULL;
    srecParent = NULL;
    csvParent = csv;
    cdfxParent = NULL;
    dcmParent = NULL;
    if (csv)
    {
        moduleName = csv->getModuleName();
    }
    axisDescrX = NULL;
    axisDescrY = NULL;
    compuVTabAxisX = NULL;
    compuVTabAxisY = NULL;
    compuVTabAxisZ = NULL;
    compuTabAxisY = NULL;
    compuTabAxisZ = NULL;
    compu_methodZ = NULL;
    displayed = false;
    precisionX = 0;
    precisionY = 0;
    precisionZ = 0;
    uint nPtsAxisX = 1;
    uint nPtsAxisY = 1;

    isSortedByRow = 0;

    //Type
    type = node->getPar("Type");
    if (type.toLower() == "value")
    {
        size = 3;
    }
    else if (type.toLower() == "curve")
    {
        axisDescrX = (AXIS_DESCR*)node->getNode("AXIS_DESCR")->child(0);
        size = 4;
    }
    else if (type.toLower() == "map")
    {
        axisDescrX = (AXIS_DESCR*)node->getNode("AXIS_DESCR")->child(0);
        axisDescrY = (AXIS_DESCR*)node->getNode("AXIS_DESCR")->child(1);
        size = 4;
    }
    else if (type.toLower() == "val_blk")
    {
        bool bl;
        NUMBER *item =  (NUMBER*)node->getItem("NUMBER");
        MATRIX_DIM *matrix_dim =  (MATRIX_DIM*)node->getItem("MATRIX_DIM");
        if (item)
        {
            QString toto = item->getPar("Number");
            nPtsAxisX = toto.toInt(&bl, 10);
        }
        else if (matrix_dim)
        {
           nPtsAxisX = ((QString)matrix_dim->getPar("xDim")).toInt();
           nPtsAxisY = ((QString)matrix_dim->getPar("yDim")).toInt();
        }
        else
        {
            nPtsAxisX = 1;
            nPtsAxisY = 1;
        }

        addressX = 0;
        addressY = 0;

        //AXIS_PTS_X
        QString str;
        for (uint i = 0; i < nPtsAxisX; i++)
        {
            listX.append(str.setNum(i));
        }

        //AXIS_PTS_Y
        if (nPtsAxisY > 1)
        {
            for (uint i = 0; i < nPtsAxisY; i++)
            {
                listY.append(str.setNum(i));
            }
        }

        size = 4;
    }
    else if (type.toLower() == "ascii")
    {
        bool bl;
        NUMBER *item = (NUMBER*)node->getItem("NUMBER");
        nPtsAxisX = ((QString)item->getPar("Number")).toInt(&bl, 10);

        //AXIS_PTS_X
        QString str;
        for (uint i = 0; i < nPtsAxisX; i++)
        {
            listX.append(str.setNum(i));
        }
        size = 4;
    }
    else
    {
        size = 3;
    }

    //read AXIS_X
    if (axisDescrX)
    {
        //Xaxis PRECISION
        FORMAT *format = (FORMAT*)axisDescrX->getItem("FORMAT");
        if (format)
        {
            QString f = format->getPar("FormatString");
            QStringList list = f.split(QRegularExpression("\\D+"));
            if (list.count() < 3)
                precisionX = 0;
            else
                precisionX = list.at(2).toInt();
        }
        else
        {
            QString compu = axisDescrX->getPar("Conversion");
            COMPU_METHOD *cmp = (COMPU_METHOD*)pro->getNode("MODULE/" + moduleName + "/COMPU_METHOD/" + compu);
            if (cmp)
            {
                QString f = cmp->getPar("Format");
                QStringList list = f.split(QRegularExpression("\\D+"));
                if (list.count() < 3)
                    precisionX = 0;
                else
                    precisionX = list.at(2).toInt();
            }
        }
    }

    //read AXIS_Y
    if (axisDescrY)
    {
        //Yaxis PRECISION
        FORMAT *format = (FORMAT*)axisDescrY->getItem("FORMAT");
        if (format)
        {
            QString f = format->getPar("FormatString");
            //QStringList list = f.split(QRegExp("\\D+"));
            QRegularExpression rx = QRegularExpression("\\D+");
            QRegularExpressionMatch match = rx.match(f);
            QStringList list = match.capturedTexts();
            if (list.count() < 3)
                precisionY = 0;
            else
                precisionY = list.at(2).toInt();
        }
        else
        {
            QString compu = axisDescrY->getPar("Conversion");
            COMPU_METHOD *cmp = (COMPU_METHOD*)pro->getNode("MODULE/" + moduleName + "/COMPU_METHOD/" + compu);
            if (cmp)
            {
                QString f = cmp->getPar("Format");
                QStringList list = f.split(QRegularExpression("\\D+"));
                if (list.count() < 3)
                    precisionY = 0;
                else
                    precisionY = list.at(2).toInt();
            }
        }
    }

    //RECORD_LAYOUT
    QString deposit = node->getPar("Deposit");
    record_layout = (RECORD_LAYOUT*)pro->getNode("MODULE/" + moduleName + "/RECORD_LAYOUT/" + deposit);

    //define number of rows of data to display
    size += nPtsAxisY;

    //read each element of Z_RECORD_LAYOUT
    foreach (Item *item, record_layout->optItems)
    {
        QString type = item->name;

        if (type == "AXIS_PTS_X")
        {
            datatypeX = ((AXIS_PTS_X*)item)->getPar("Datatype");
        }
        else if (type == "AXIS_PTS_Y")
        {
            datatypeY = ((AXIS_PTS_Y*)item)->getPar("Datatype");
        }
        else if (type == "FNC_VALUES")
        {
            //column or row Dir
            QString zDir = ((FNC_VALUES*)item)->getPar("IndexMode");
            if (zDir.compare("COLUMN_DIR") == 0)
            {
                isSortedByRow = 0;
            }
            else
            {
                    isSortedByRow = 1;
            }

            //dataType
            datatypeZ = ((FNC_VALUES*)item)->getPar("Datatype");
        }
    }

    //Zaxis PRECISION
    FORMAT *format = (FORMAT*)node->getItem("FORMAT");
    if (format)
    {
        //        QString f = format->getPar("FormatString");
        //        QStringList list = f.split(QRegExp("\\D+"));
        //        if (list.count() < 3)
        //            precisionZ = 0;
        //        else
        //            precisionZ = list.at(2).toInt();
        QString f = format->getPar("FormatString");
        QRegularExpression rx("%\\d*.(\\d*)");
        //rx.indexIn(f);
        //QStringList list = rx.capturedTexts();
        QRegularExpressionMatch match = rx.match(f);
        QStringList list = match.capturedTexts();
        if (!list.at(1).isEmpty())
        {
            precisionZ = list.at(1).toInt();
        }
    }
    else
    {
        QString compu = node->getPar("Conversion");
        COMPU_METHOD *cmp = (COMPU_METHOD*)pro->getNode("MODULE/" + moduleName + "/COMPU_METHOD/" + compu);
        if (cmp)
        {
//            QString f = cmp->getPar("Format");
//            QStringList list = f.split(QRegExp("\\D+"));
//            if (list.count() < 3)
//                precisionZ = 0;
//            else
//                precisionZ = list.at(2).toInt();
            QString f = cmp->getPar("Format");
            QRegularExpression rx("%\\d*.(\\d*)");
            //rx.indexIn(f);
            //QStringList list = rx.capturedTexts();
            QRegularExpressionMatch match = rx.match(f);
            QStringList list = match.capturedTexts();
            if (!list.at(1).isEmpty())
            {
                precisionZ = list.at(1).toInt();
            }

        }
    }

}

Data::Data(CHARACTERISTIC *node, PROJECT *pro, CdfxFile *cdfx, bool modif) : QObject(), Node(node->name)
{
    isComplete = true; //created to check if the creation of the data has been completed or aborted due to missing information
    isAxisXComparable = true;
    isAxisYComparable = true;
    sizeChanged = false;
    modifiable = modif;
    label = node;
    project = pro;
    hexParent = NULL;
    srecParent = NULL;
    csvParent = NULL;
    cdfxParent = cdfx;
    dcmParent = NULL;
    if (cdfx)
    {
        moduleName = cdfx->getModuleName();
    }
    axisDescrX = NULL;
    axisDescrY = NULL;
    compuVTabAxisX = NULL;
    compuVTabAxisY = NULL;
    compuVTabAxisZ = NULL;
    compuTabAxisY = NULL;
    compuTabAxisZ = NULL;
    compu_methodZ = NULL;
    displayed = false;
    precisionX = 0;
    precisionY = 0;
    precisionZ = 0;
    uint nPtsAxisX = 1;
    uint nPtsAxisY = 1;
    isSortedByRow = 0;

    //Type
    type = node->getPar("Type");
    if (type.toLower() == "value")
    {
        size = 3;
    }
    else if (type.toLower() == "curve")
    {
        axisDescrX = (AXIS_DESCR*)node->getNode("AXIS_DESCR")->child(0);
        size = 4;
    }
    else if (type.toLower() == "map")
    {
        axisDescrX = (AXIS_DESCR*)node->getNode("AXIS_DESCR")->child(0);
        axisDescrY = (AXIS_DESCR*)node->getNode("AXIS_DESCR")->child(1);
        size = 4;
    }
    else if (type.toLower() == "val_blk")
    {
        bool bl;
        NUMBER *item =  (NUMBER*)node->getItem("NUMBER");
        MATRIX_DIM *matrix_dim =  (MATRIX_DIM*)node->getItem("MATRIX_DIM");
        if (item)
        {
            QString toto = item->getPar("Number");
            nPtsAxisX = toto.toInt(&bl, 10);
        }
        else if (matrix_dim)
        {
           nPtsAxisX = ((QString)matrix_dim->getPar("xDim")).toInt();
           nPtsAxisY = ((QString)matrix_dim->getPar("yDim")).toInt();
        }
        else
        {
            nPtsAxisX = 1;
            nPtsAxisY = 1;
        }

        addressX = 0;
        addressY = 0;

        //AXIS_PTS_X
        QString str;
        for (uint i = 0; i < nPtsAxisX; i++)
        {
            listX.append(str.setNum(i));
        }

        //AXIS_PTS_Y
        if (nPtsAxisY > 1)
        {
            for (uint i = 0; i < nPtsAxisY; i++)
            {
                listY.append(str.setNum(i));
            }
        }

        size = 4;
    }
    else if (type.toLower() == "ascii")
    {
        bool bl;
        NUMBER *item = (NUMBER*)node->getItem("NUMBER");
        nPtsAxisX = ((QString)item->getPar("Number")).toInt(&bl, 10);

        //AXIS_PTS_X
        QString str;
        for (uint i = 0; i < nPtsAxisX; i++)
        {
            listX.append(str.setNum(i));
        }
        size = 4;
    }
    else
    {
        size = 3;
    }

    //read AXIS_X
    if (axisDescrX)
    {
        //Xaxis PRECISION
        FORMAT *format = (FORMAT*)axisDescrX->getItem("FORMAT");
        if (format)
        {
            QString f = format->getPar("FormatString");
            QStringList list = f.split(QRegularExpression("\\D+"));
            if (list.count() < 3)
                precisionX = 0;
            else
                precisionX = list.at(2).toInt();
        }
        else
        {
            QString compu = axisDescrX->getPar("Conversion");
            COMPU_METHOD *cmp = (COMPU_METHOD*)pro->getNode("MODULE/" + moduleName + "/COMPU_METHOD/" + compu);
            if (cmp)
            {
                QString f = cmp->getPar("Format");
                QStringList list = f.split(QRegularExpression("\\D+"));
                if (list.count() < 3)
                    precisionX = 0;
                else
                    precisionX = list.at(2).toInt();
            }
        }

    }

    //read AXIS_Y
    if (axisDescrY)
    {
        //Yaxis PRECISION
        FORMAT *format = (FORMAT*)axisDescrY->getItem("FORMAT");
        if (format)
        {
            QString f = format->getPar("FormatString");
            QStringList list = f.split(QRegularExpression("\\D+"));
            if (list.count() < 3)
                precisionY = 0;
            else
                precisionY = list.at(2).toInt();
        }
        else
        {
            QString compu = axisDescrY->getPar("Conversion");
            COMPU_METHOD *cmp = (COMPU_METHOD*)pro->getNode("MODULE/" + moduleName + "/COMPU_METHOD/" + compu);
            if (cmp)
            {
                QString f = cmp->getPar("Format");
                QStringList list = f.split(QRegularExpression("\\D+"));
                if (list.count() < 3)
                    precisionY = 0;
                else
                    precisionY = list.at(2).toInt();
            }
        }
    }

    //RECORD_LAYOUT
    QString deposit = node->getPar("Deposit");
    record_layout = (RECORD_LAYOUT*)pro->getNode("MODULE/" + moduleName + "/RECORD_LAYOUT/" + deposit);

    //define number of rows of data to display
    size += nPtsAxisY;

    //read each element of Z_RECORD_LAYOUT
    foreach (Item *item, record_layout->optItems)
    {
        QString type = item->name;

        if (type == "AXIS_PTS_X")
        {
            datatypeX = ((AXIS_PTS_X*)item)->getPar("Datatype");
        }
        else if (type == "AXIS_PTS_Y")
        {
            datatypeY = ((AXIS_PTS_Y*)item)->getPar("Datatype");
        }
        else if (type == "FNC_VALUES")
        {
            //column or row Dir
            QString zDir = ((FNC_VALUES*)item)->getPar("IndexMode");
            if (zDir.compare("COLUMN_DIR") == 0)
            {
                isSortedByRow = 0;
            }
            else
            {
                    isSortedByRow = 1;
            }

            //dataType
            datatypeZ = ((FNC_VALUES*)item)->getPar("Datatype");
        }
    }

    //Zaxis PRECISION
    FORMAT *format = (FORMAT*)node->getItem("FORMAT");
    if (format)
    {
//        QString f = format->getPar("FormatString");
//        QStringList list = f.split(QRegExp("\\D+"));
//        if (list.count() < 3)
//            precisionZ = 0;
//        else
//            precisionZ = list.at(2).toInt();
        QString f = format->getPar("FormatString");
        QRegularExpression rx("%\\d*.(\\d*)");
        //rx.indexIn(f);
        //QStringList list = rx.capturedTexts();
        QRegularExpressionMatch match = rx.match(f);
        QStringList list = match.capturedTexts();
        if (!list.at(1).isEmpty())
        {
            precisionZ = list.at(1).toInt();
        }
    }
    else
    {
        QString compu = node->getPar("Conversion");
        COMPU_METHOD *cmp = (COMPU_METHOD*)pro->getNode("MODULE/" + moduleName + "/COMPU_METHOD/" + compu);
        if (cmp)
        {
//            QString f = cmp->getPar("Format");
//            QStringList list = f.split(QRegExp("\\D+"));
//            if (list.count() < 3)
//                precisionZ = 0;
//            else
//                precisionZ = list.at(2).toInt();
            QString f = cmp->getPar("Format");
            QRegularExpression rx("%\\d*.(\\d*)");
            //rx.indexIn(f);
            //QStringList list = rx.capturedTexts();
            QRegularExpressionMatch match = rx.match(f);
            QStringList list = match.capturedTexts();
            if (!list.at(1).isEmpty())
            {
                precisionZ = list.at(1).toInt();
            }
        }
    }

}

Data::Data(CHARACTERISTIC *node, PROJECT *pro, Dcm *dcm, bool modif) : QObject(), Node(node->name)
{
    isComplete = true; //created to check if the creation of the data has been completed or aborted due to missing information
    isAxisXComparable = false;
    isAxisYComparable = false;
    sizeChanged = false;
    modifiable = modif;
    label = node;
    project = pro;
    hexParent = NULL;
    srecParent = NULL;
    csvParent = NULL;
    dcmParent = dcm;
    cdfxParent = NULL;
    if (dcm)
    {
        moduleName = dcm->getModuleName();
    }
    axisDescrX = NULL;
    axisDescrY = NULL;
    compuVTabAxisX = NULL;
    compuVTabAxisY = NULL;
    compuVTabAxisZ = NULL;
    compuTabAxisY = NULL;
    compuTabAxisZ = NULL;
    compu_methodZ = NULL;
    displayed = false;
    precisionX = 0;
    precisionY = 0;
    precisionZ = 0;
    uint nPtsAxisX = 1;
    uint nPtsAxisY = 1;
    isSortedByRow = 0;

    //Type
    type = node->getPar("Type");
    if (type.toLower() == "value")
    {
        size = 3;
    }
    else if (type.toLower() == "curve")
    {
        axisDescrX = (AXIS_DESCR*)node->getNode("AXIS_DESCR")->child(0);
        size = 4;
    }
    else if (type.toLower() == "map")
    {
        axisDescrX = (AXIS_DESCR*)node->getNode("AXIS_DESCR")->child(0);
        axisDescrY = (AXIS_DESCR*)node->getNode("AXIS_DESCR")->child(1);
        size = 4;
    }
    else if (type.toLower() == "val_blk")
    {
        bool bl;
        NUMBER *item =  (NUMBER*)node->getItem("NUMBER");
        MATRIX_DIM *matrix_dim =  (MATRIX_DIM*)node->getItem("MATRIX_DIM");
        if (item)
        {
            QString toto = item->getPar("Number");
            nPtsAxisX = toto.toInt(&bl, 10);
        }
        else if (matrix_dim)
        {
           nPtsAxisX = ((QString)matrix_dim->getPar("xDim")).toInt();
           nPtsAxisY = ((QString)matrix_dim->getPar("yDim")).toInt();
        }
        else
        {
            nPtsAxisX = 1;
            nPtsAxisY = 1;
        }

        addressX = 0;
        addressY = 0;

        //AXIS_PTS_X
        QString str;
        for (uint i = 0; i < nPtsAxisX; i++)
        {
            listX.append(str.setNum(i));
        }

        //AXIS_PTS_Y
        if (nPtsAxisY > 1)
        {
            for (uint i = 0; i < nPtsAxisY; i++)
            {
                listY.append(str.setNum(i));
            }
        }

        size = 4;
    }
    else if (type.toLower() == "ascii")
    {
        bool bl;
        NUMBER *item = (NUMBER*)node->getItem("NUMBER");
        nPtsAxisX = ((QString)item->getPar("Number")).toInt(&bl, 10);

        //AXIS_PTS_X
        QString str;
        for (uint i = 0; i < nPtsAxisX; i++)
        {
            listX.append(str.setNum(i));
        }
        size = 4;
    }
    else
    {
        size = 3;
    }

    //read AXIS_X
    if (axisDescrX)
    {
        //Xaxis PRECISION
        FORMAT *format = (FORMAT*)axisDescrX->getItem("FORMAT");
        if (format)
        {
            QString f = format->getPar("FormatString");
            QStringList list = f.split(QRegularExpression("\\D+"));
            if (list.count() < 3)
                precisionX = 0;
            else
                precisionX = list.at(2).toInt();
        }
        else
        {
            QString compu = axisDescrX->getPar("Conversion");
            COMPU_METHOD *cmp = (COMPU_METHOD*)pro->getNode("MODULE/" + moduleName + "/COMPU_METHOD/" + compu);
            if (cmp)
            {
                QString f = cmp->getPar("Format");
                QStringList list = f.split(QRegularExpression("\\D+"));
                if (list.count() < 3)
                    precisionX = 0;
                else
                    precisionX = list.at(2).toInt();
            }
        }
    }

    //read AXIS_Y
    if (axisDescrY)
    {
        //Yaxis PRECISION
        FORMAT *format = (FORMAT*)axisDescrY->getItem("FORMAT");
        if (format)
        {
            QString f = format->getPar("FormatString");
            QStringList list = f.split(QRegularExpression("\\D+"));
            if (list.count() < 3)
                precisionY = 0;
            else
                precisionY = list.at(2).toInt();
        }
        else
        {
            QString compu = axisDescrY->getPar("Conversion");
            COMPU_METHOD *cmp = (COMPU_METHOD*)pro->getNode("MODULE/" + moduleName + "/COMPU_METHOD/" + compu);
            if (cmp)
            {
                QString f = cmp->getPar("Format");
                QStringList list = f.split(QRegularExpression("\\D+"));
                if (list.count() < 3)
                    precisionY = 0;
                else
                    precisionY = list.at(2).toInt();
            }
        }
    }

    //RECORD_LAYOUT
    QString deposit = node->getPar("Deposit");
    record_layout = (RECORD_LAYOUT*)pro->getNode("MODULE/" + moduleName + "/RECORD_LAYOUT/" + deposit);

    //define number of rows of data to display
    size += nPtsAxisY;

    //read each element of Z_RECORD_LAYOUT
    foreach (Item *item, record_layout->optItems)
    {
        QString type = item->name;

        if (type == "AXIS_PTS_X")
        {
            datatypeX = ((AXIS_PTS_X*)item)->getPar("Datatype");
        }
        else if (type == "AXIS_PTS_Y")
        {
            datatypeY = ((AXIS_PTS_Y*)item)->getPar("Datatype");
        }
        else if (type == "FNC_VALUES")
        {
            //column or row Dir
            QString zDir = ((FNC_VALUES*)item)->getPar("IndexMode");
            if (zDir.compare("COLUMN_DIR") == 0)
            {
                isSortedByRow = 0;
            }
            else
            {
                    isSortedByRow = 1;
            }

            //dataType
            datatypeZ = ((FNC_VALUES*)item)->getPar("Datatype");
        }
    }

    //Zaxis PRECISION
    FORMAT *format = (FORMAT*)node->getItem("FORMAT");
    if (format)
    {
        //        QString f = format->getPar("FormatString");
        //        QStringList list = f.split(QRegExp("\\D+"));
        //        if (list.count() < 3)
        //            precisionZ = 0;
        //        else
        //            precisionZ = list.at(2).toInt();
        QString f = format->getPar("FormatString");
        QRegularExpression rx("%\\d*.(\\d*)");
        //rx.indexIn(f);
        //QStringList list = rx.capturedTexts();
        QRegularExpressionMatch match = rx.match(f);
        QStringList list = match.capturedTexts();
        if (!list.at(1).isEmpty())
        {
            precisionZ = list.at(1).toInt();
        }
    }
    else
    {
        QString compu = node->getPar("Conversion");
        COMPU_METHOD *cmp = (COMPU_METHOD*)pro->getNode("MODULE/" + moduleName + "/COMPU_METHOD/" + compu);
        if (cmp)
        {
//            QString f = cmp->getPar("Format");
//            QStringList list = f.split(QRegExp("\\D+"));
//            if (list.count() < 3)
//                precisionZ = 0;
//            else
//                precisionZ = list.at(2).toInt();
            QString f = cmp->getPar("Format");
            QRegularExpression rx("%\\d*.(\\d*)");
            //rx.indexIn(f);
            //QStringList list = rx.capturedTexts();
            QRegularExpressionMatch match = rx.match(f);
            QStringList list = match.capturedTexts();
            if (!list.at(1).isEmpty())
            {
                precisionZ = list.at(1).toInt();
            }
        }
    }

}

Data::Data(AXIS_PTS *node, PROJECT *pro, HexFile *hexFile, bool modif) : QObject(), Node(node->name)
{
    isComplete = true; //created to check if the creation of the data has been completed or aborted due to missing information
    isAxisXComparable = false;
    isAxisYComparable = false;
    sizeChanged = false;
    modifiable = modif;
    label = node;
    project = pro;
    hexParent = hexFile;
    srecParent = NULL;
    csvParent = NULL;
    cdfxParent = NULL;
    dcmParent = NULL;
    if (hexParent)
    {
        moduleName = hexParent->getModuleName();
    }
    axisDescrX = NULL;
    axisDescrY = NULL;
    compu_methodZ = NULL;
    compuVTabAxisX = NULL;
    compuVTabAxisY = NULL;
    compuVTabAxisZ = NULL;
    compuTabAxisY = NULL;
    compuTabAxisZ = NULL;
    compu_methodZ = NULL;
    byteOrderX = "";
    byteOrderY = "";
    byteOrderZ = "";
    displayed = false;
    precisionX = 0;
    precisionY = 0;
    precisionZ = 0;
    addressX = 0;
    addressY = 0;
    addressZ = 0;
    isSortedByRow = 0;

    //Type
    type = "AXIS_PTS";

    // RECORD_LAYOUT
    QString deposit = node->getPar("Deposit");
    RECORD_LAYOUT *record_layout = (RECORD_LAYOUT*)hexFile->record_layout->getNode(deposit);

    //compu_methodZ
    QString compu = ((AXIS_PTS*)label)->getPar("Conversion");
    compu_methodZ = (COMPU_METHOD*)hexFile->compu_method->getNode(compu);

    //number of points X
    QString maxAxisPts = ((AXIS_PTS*)label)->getPar("MaxAxisPoints");
    int nPts = maxAxisPts.toUInt();

    //Zaxis PRECISION
    if (compu_methodZ)
    {
        QString f = compu_methodZ->getPar("Format");
        QStringList list = f.split(QRegularExpression("\\D+"));
        if (list.count() < 3)
            precisionZ = 0;
        else
            precisionZ = list.at(2).toInt();
    }

    //BYTE_ORDER axisZ
    Byte_Order *byteOrder = (Byte_Order*)((AXIS_PTS*)label)->getItem("BYTE_ORDER");
    if (byteOrder)
    {
        byteOrderZ = byteOrder->getPar("ByteOrder");
    }

    //read each element of Z_RECORD_LAYOUT
    bool bl;
    int offset = 0;
    foreach (Item *item, record_layout->optItems)
    {
        QString type = item->name;

        if (type == "SRC_ADDR_X")
        {
            std::string datatype = ((SRC_ADDR_X*)item)->getPar("Datatype");
            offset += hexParent->getNumByte(datatype);
        }
        else if (type == "NO_AXIS_PTS_X")
        {
            std::string datatype = ((NO_AXIS_PTS_X*)item)->getPar("Datatype");
            int nbyte = hexParent->getNumByte(datatype);
            QString val = hexParent->getHexValue(node->getPar("Adress"), offset, nbyte, byteOrderZ);
            bool bl;
            nPts = val.toInt(&bl,16);
            offset += nbyte;

            //check if nPts < nPtsmax
            QString maxAxisPts = node->getPar("MaxAxisPoints");
            double nmaxPts = maxAxisPts.toDouble();
            if (nPts > nmaxPts)
                nPts = nmaxPts;
        }
        else if (type == "AXIS_PTS_X")
        {
            //get datatype
            datatypeZ = ((AXIS_PTS_X*)item)->getPar("Datatype");

            //get the size of the type
                //be carefull : if ALIGNMENT is present in record_layout use this one
                //to determine the size and not the one in MODE_COMMON

            int Znbyte = hexParent->getNumByte(datatypeZ);

            //get the address of the data
            addressZ = QString(node->getPar("Adress")).toUInt(&bl, 16) + offset;

            //check if the number of points is specified otherwise take max number of points
            if (nPts == 0)
            {
                QString _maxAxisPoints = node->getPar("MaxAxisPoints");
                nPts = _maxAxisPoints.toUInt();
            }

            // get the hex values from Hex file
            QList<double> decZ = hexParent->getDecValues(addressZ, Znbyte, nPts, datatypeZ, byteOrderZ);

            //dec2phys
            listZ = dec2Phys(decZ, "z");

            offset +=  nPts * Znbyte;
        }
    }

    //FIX_AXIS POINTS
    QString str;
    for (int i = 0; i < nPts; i++)
    {
        listX.append(str.setNum(i));
    }

    size = 5;
}

Data::Data(AXIS_PTS *node, PROJECT *pro, SrecFile *srecFile, bool modif) : QObject(), Node(node->name)
{
    isComplete = true; //created to check if the creation of the data has been completed or aborted due to missing information
    isAxisXComparable = false;
    isAxisYComparable = false;
    sizeChanged = false;
    modifiable = modif;
    label = node;
    project = pro;
    hexParent = NULL;
    srecParent = srecFile;
    csvParent = NULL;
    dcmParent = NULL;
    cdfxParent = NULL;
    if (srecParent)
    {
        moduleName = srecParent->getModuleName();
    }
    axisDescrX = NULL;
    axisDescrY = NULL;
    compu_methodZ = NULL;
    compuVTabAxisX = NULL;
    compuVTabAxisY = NULL;
    compuVTabAxisZ = NULL;
    compuTabAxisY = NULL;
    compuTabAxisZ = NULL;
    compu_methodZ = NULL;
    byteOrderX = "";
    byteOrderY = "";
    byteOrderZ = "";
    displayed = false;
    precisionX = 0;
    precisionY = 0;
    precisionZ = 0;
    addressX = 0;
    addressY = 0;
    addressZ = 0;
    isSortedByRow = 0;

    //Type
    type = "AXIS_PTS";

    // RECORD_LAYOUT
    QString deposit = node->getPar("Deposit");
    RECORD_LAYOUT *record_layout = (RECORD_LAYOUT*)srecFile->record_layout->getNode(deposit);

    //compu_methodZ
    QString compu = ((AXIS_PTS*)label)->getPar("Conversion");
    compu_methodZ = (COMPU_METHOD*)srecFile->compu_method->getNode(compu);

    //Zaxis PRECISION
    if (compu_methodZ)
    {
        QString f = compu_methodZ->getPar("Format");
        QStringList list = f.split(QRegularExpression("\\D+"));
        if (list.count() < 3)
            precisionZ = 0;
        else
            precisionZ = list.at(2).toInt();
    }

    //BYTE_ORDER axisZ
    Byte_Order *byteOrder = (Byte_Order*)((AXIS_PTS*)label)->getItem("BYTE_ORDER");
    if (byteOrder)
    {
        byteOrderZ = byteOrder->getPar("ByteOrder");
    }

    //read each element of Z_RECORD_LAYOUT
    bool bl;
    int offset = 0;
    int nPts = 0;
    foreach (Item *item, record_layout->optItems)
    {
        QString type = item->name;

        if (type == "SRC_ADDR_X")
        {
            std::string datatype = ((SRC_ADDR_X*)item)->getPar("Datatype");
            offset += srecParent->getNumByte(datatype);
        }
        else if (type == "NO_AXIS_PTS_X")
        {
            std::string datatype = ((NO_AXIS_PTS_X*)item)->getPar("Datatype");
            int nbyte = srecParent->getNumByte(datatype);
            QString val = srecParent->getHexValue(node->getPar("Adress"), offset, nbyte, byteOrderZ);
            bool bl;
            nPts = val.toInt(&bl,16);
            offset += nbyte;

            //check if nPts < nPtsmax
            QString maxAxisPts = node->getPar("MaxAxisPoints");
            double nmaxPts = maxAxisPts.toDouble();
            if (nPts > nmaxPts)
                nPts = nmaxPts;
        }
        else if (type == "AXIS_PTS_X")
        {
            datatypeZ = ((AXIS_PTS_X*)item)->getPar("Datatype");
            int Znbyte = srecParent->getNumByte(datatypeZ);
            addressZ = QString(node->getPar("Adress")).toUInt(&bl, 16) + offset;

            //check if the number of points is specified otherwise take max number of points
            if (nPts == 0)
            {
                QString _maxAxisPoints = node->getPar("MaxAxisPoints");
                nPts = _maxAxisPoints.toUInt();
            }

            // get the hex values from Srec file
            QList<double> decZ = srecParent->getDecValues(addressZ, Znbyte, nPts, datatypeZ, byteOrderZ);

            //dec2phys
            listZ = dec2Phys(decZ, "z");

            offset +=  nPts * Znbyte;
        }
    }

    //FIX_AXIS POINTS
    QString str;
    for (int i = 0; i < nPts; i++)
    {
        listX.append(str.setNum(i));
    }

    size = 5;

}

Data::Data(AXIS_PTS *node, PROJECT *pro, Csv *csv, bool modif) : QObject(), Node(node->name)
{
    isComplete = true; //created to check if the creation of the data has been completed or aborted due to missing information
    isAxisXComparable = false;
    isAxisYComparable = false;
    sizeChanged = false;
    modifiable = modif;
    label = node;
    project = pro;
    hexParent = NULL;
    srecParent = NULL;
    dcmParent = NULL;
    csvParent = csv;
    cdfxParent = NULL;
    if (csv)
    {
        moduleName = csv->getModuleName();
    }
    axisDescrX = NULL;
    axisDescrY = NULL;
    compuVTabAxisX = NULL;
    compuVTabAxisY = NULL;
    compuVTabAxisZ = NULL;
    compuTabAxisY = NULL;
    compuTabAxisZ = NULL;
    compu_methodZ = NULL;
    compu_methodZ = NULL;
    displayed = false;
    precisionX = 0;
    precisionY = 0;
    precisionZ = 0;
    addressX = 0;
    addressY = 0;
    addressZ = 0;
    isSortedByRow = 0;

    //Type
    type = "AXIS_PTS";

    // RECORD_LAYOUT
    QString deposit = node->getPar("Deposit");
    record_layout = (RECORD_LAYOUT*)project->getNode("MODULE/" + moduleName + "/RECORD_LAYOUT/" + deposit);

    //read each element of X_RECORD_LAYOUT
    foreach (Item *item, record_layout->optItems)
    {
        QString type = item->name;
        if (type == "AXIS_PTS_X")
        {
            datatypeZ = ((AXIS_PTS_X*)item)->getPar("Datatype");
        }
    }

    //Zaxis PRECISION
    QString compu = node->getPar("Conversion");
    COMPU_METHOD *cmp = (COMPU_METHOD*)pro->getNode("MODULE/" + moduleName + "/COMPU_METHOD/" + compu);
    if (cmp)
    {
        QString f = cmp->getPar("Format");
        //QStringList list = f.split(QRegExp("\\D+"));
        QRegularExpression rx = QRegularExpression("\\D+");
        QRegularExpressionMatch match = rx.match(f);
        QStringList list = match.capturedTexts();
        if (list.count() < 3)
            precisionZ = 0;
        else
            precisionZ = list.at(2).toInt();
    }

    size = 5;

}

Data::Data(AXIS_PTS *node, PROJECT *pro, CdfxFile *cdfx, bool modif) : QObject() , Node(node->name)
{
    isComplete = true; //created to check if the creation of the data has been completed or aborted due to missing information
    isAxisXComparable = false;
    isAxisYComparable = false;
    sizeChanged = false;
    modifiable = modif;
    label = node;
    project = pro;
    hexParent = NULL;
    srecParent = NULL;
    csvParent = NULL;
    dcmParent = NULL;
    cdfxParent = cdfx;
    if (cdfx)
    {
        moduleName = cdfx->getModuleName();
    }
    axisDescrX = NULL;
    axisDescrY = NULL;
    compuVTabAxisX = NULL;
    compuVTabAxisY = NULL;
    compuVTabAxisZ = NULL;
    compuTabAxisY = NULL;
    compuTabAxisZ = NULL;
    compu_methodZ = NULL;
    compu_methodZ = NULL;
    displayed = false;
    precisionX = 0;
    precisionY = 0;
    precisionZ = 0;
    addressX = 0;
    addressY = 0;
    addressZ = 0;
    isSortedByRow = 0;

    //Type
    type = "AXIS_PTS";

    // RECORD_LAYOUT
    QString deposit = node->getPar("Deposit");
    record_layout = (RECORD_LAYOUT*)project->getNode("MODULE/" + moduleName + "/RECORD_LAYOUT/" + deposit);

    //read each element of X_RECORD_LAYOUT
    foreach (Item *item, record_layout->optItems)
    {
        QString type = item->name;
        if (type == "AXIS_PTS_X")
        {
            datatypeZ = ((AXIS_PTS_X*)item)->getPar("Datatype");
        }
    }

    //Zaxis PRECISION
    QString compu = node->getPar("Conversion");
    COMPU_METHOD *cmp = (COMPU_METHOD*)pro->getNode("MODULE/" + moduleName + "/COMPU_METHOD/" + compu);
    if (cmp)
    {
        QString f = cmp->getPar("Format");
        //QStringList list = f.split(QRegExp("\\D+"));
        QRegularExpression rx = QRegularExpression("\\D+");
        QRegularExpressionMatch match = rx.match(f);
        QStringList list = match.capturedTexts();
        if (list.count() < 3)
            precisionZ = 0;
        else
            precisionZ = list.at(2).toInt();
    }

    size = 5;
}

Data::Data(AXIS_PTS *node, PROJECT *pro, Dcm *dcm, bool modif) : QObject(), Node(node->name)
{
    isComplete = true; //created to check if the creation of the data has been completed or aborted due to missing information
    isAxisXComparable = false;
    isAxisYComparable = false;
    sizeChanged = false;
    modifiable = modif;
    label = node;
    project = pro;
    hexParent = NULL;
    srecParent = NULL;
    csvParent = NULL;
    dcmParent = dcm;
    cdfxParent = NULL;
    if (dcm)
    {
        moduleName = dcm->getModuleName();
    }
    axisDescrX = NULL;
    axisDescrY = NULL;
    compuVTabAxisX = NULL;
    compuVTabAxisY = NULL;
    compuVTabAxisZ = NULL;
    compuTabAxisY = NULL;
    compuTabAxisZ = NULL;
    compu_methodZ = NULL;
    compu_methodZ = NULL;
    displayed = false;
    precisionX = 0;
    precisionY = 0;
    precisionZ = 0;
    addressX = 0;
    addressY = 0;
    addressZ = 0;
    isSortedByRow = 0;

    //Type
    type = "AXIS_PTS";

    // RECORD_LAYOUT
    QString deposit = node->getPar("Deposit");
    record_layout = (RECORD_LAYOUT*)project->getNode("MODULE/" + moduleName + "/RECORD_LAYOUT/" + deposit);

    //read each element of X_RECORD_LAYOUT
    foreach (Item *item, record_layout->optItems)
    {
        QString type = item->name;
        if (type == "AXIS_PTS_X")
        {
            datatypeZ = ((AXIS_PTS_X*)item)->getPar("Datatype");
        }
    }

    //Zaxis PRECISION
    QString compu = node->getPar("Conversion");
    COMPU_METHOD *cmp = (COMPU_METHOD*)pro->getNode("MODULE/" + moduleName + "/COMPU_METHOD/" + compu);
    if (cmp)
    {
        QString f = cmp->getPar("Format");
        //QStringList list = f.split(QRegExp("\\D+"));
        QRegularExpression rx = QRegularExpression("\\D+");
        QRegularExpressionMatch match = rx.match(f);
        QStringList list = match.capturedTexts();
        if (list.count() < 3)
            precisionZ = 0;
        else
            precisionZ = list.at(2).toInt();
    }

    size = 5;

}


// --- ASAP info --- //

QString Data::toString()
{
    QString str = "Data* (" + QString(label->name) + " )" ;
    return str;
}

QString Data::getName()
{
    return label->name;
}

QString Data::getComment()
{
    //QString type = typeid(*label).name();

    if (type.compare("CHARACTERISTIC") == 0)
        return ((CHARACTERISTIC*)label)->getPar("LongIdentifier");
    else
        return ((AXIS_PTS*)label)->getPar("LongIdentifier");
}

QString Data::getSubset()
{
    if (type.compare("AXIS_PTS") == 0)
        return ((CHARACTERISTIC*)label)->getSubsetName();
    else
        return ((AXIS_PTS*)label)->getSubsetName();
}

QString Data::getUnit()
{
    QString type = typeid(*label).name();

    if (type.endsWith("CHARACTERISTIC"))
    {
        QString compu_method = ((CHARACTERISTIC*)label)->getPar("Conversion");
        Node *node = label->getParentNode()->getParentNode();
        COMPU_METHOD *cmp = (COMPU_METHOD*)node->getNode("COMPU_METHOD/" + compu_method);
        if (cmp)
        {
            return cmp->getPar("Unit");
        }
        else
            return "";
    }
    else if (type.endsWith("AXIS_PTS"))
    {
        QString compu_method = ((AXIS_PTS*)label)->getPar("Conversion");
        Node * node = label->getParentNode()->getParentNode();
        COMPU_METHOD *cmp = (COMPU_METHOD*)node->getNode("COMPU_METHOD/" + compu_method);
        return cmp->getPar("Unit");
    }
    else
        return "";
}

QString Data::getInputQuantityX()
{
    QString str = "X : ";
    if (axisDescrX)
    {
        str.append(axisDescrX->fixPar("InputQuantity").c_str());
        COMPU_METHOD *cpmX = getCompuMethodAxisX();
        if (cpmX)
            str.append(" - " + QString(cpmX->getPar("Unit")));
    }
    else
        str.append("fix points");

    return str;
}

QString Data::getInputQuantityY()
{
    QString str = "Y : ";
    if (axisDescrY)
    {
        str.append(axisDescrY->fixPar("InputQuantity").c_str());
        COMPU_METHOD *cpmY = getCompuMethodAxisY();
        if (cpmY)
            str.append(" - " + QString(cpmY->getPar("Unit")));
    }
    else
        str.append("fix points");

    return str;
}

QString Data::getOutputQuantityZ()
{
    QString str = "Z : ";

    str.append(getUnit());

    return str;
}

int Data::getZnbyte()
{
    if (hexParent)
        return hexParent->getNumByte(datatypeZ);
    else if (srecParent)
        return srecParent->getNumByte(datatypeZ);
    else if (csvParent)
        return csvParent->getNumByte(datatypeZ);
    else if (cdfxParent)
        return cdfxParent->getNumByte(datatypeZ);
    else if (dcmParent)
        return dcmParent->getNumByte(datatypeZ);
    else
        return 0;
}

std::string Data::getDatatypeZ()
{
    return datatypeZ;
}

QString Data::getMaxDim()
{
    bool display = false;
    QString str = "";
    if (axisDescrX)
    {
        if (xCount() < QString(axisDescrX->fixPar("MaxAxisPoints").c_str()).toInt())
        {
            display = true;
        }
        str = "[";
        str.append(axisDescrX->fixPar("MaxAxisPoints").c_str());

        if (axisDescrY)
        {
            if (yCount() < QString(axisDescrY->fixPar("MaxAxisPoints").c_str()).toInt())
            {
                display = true;
            }

            str.append("x");
            str.append(axisDescrY->fixPar("MaxAxisPoints").c_str());
            str.append("]");
        }
        else
            str.append("x1]");
    }

    if (display)
    {
        return str;
    }
    else
        return "";
}

int Data::getMaxAxisX()
{
    if (axisDescrX)
    {
        return QString(axisDescrX->fixPar("MaxAxisPoints").c_str()).toInt();
    }
    else
        return 0;
}

int Data::getMaxAxisY()
{
    if (axisDescrY)
    {
        return QString(axisDescrY->fixPar("MaxAxisPoints").c_str()).toInt();
    }
    else
        return 0;
}

Node *Data::getA2lNode()
{
    return label;
}

// --- HEX TO PHYS --- //

void Data::hex2phys()
{

    //axis X
    if (listX.count() > 0)
    {
        //hex2dec
        QList<double> decX = hex2dec(listX, datatypeX, 16);

        //dec2phys
        if (axisDescrX)
        {
            QString compu_method = axisDescrX->getPar("Conversion");
            COMPU_METHOD *cmp = (COMPU_METHOD*)project->getNode("MODULE/" + moduleName + "/COMPU_METHOD/" + compu_method);
            QString convType = cmp->getPar("ConversionType");

            bool bl;
            double dec;
            if (convType.toLower() == "rat_func")
            {
                COEFFS *item = (COEFFS*)cmp->getItem("COEFFS");
                double a = ((QString)item->getPar("float1")).toDouble(&bl);
                double b = ((QString)item->getPar("float2")).toDouble(&bl);
                double c = ((QString)item->getPar("float3")).toDouble(&bl);
                double d = ((QString)item->getPar("float4")).toDouble(&bl);
                double e = ((QString)item->getPar("float5")).toDouble(&bl);
                double f = ((QString)item->getPar("float6")).toDouble(&bl);

                for (int i = 0; i < listX.count(); i++)
                {
                    dec = decX.at(i);
                    QString phys;
                    if (dec * d - a == 0)
                    {
                        phys = QString::number((c - dec * f) / (dec * e - b),'f', precisionX);
                    }
                    else
                    {
                        phys = QString::number(((b - dec * e) + sqrt(pow((dec * e - b), 2) - 4 * (dec * d - a) * (dec * f - c))) / (2 * (dec * d - a)),'f', precisionX);
                    }

                    double dbl = phys.toDouble();
                    if (dbl == 0)
                    {
                        listX[i] = QString::number(0,'f', precisionX);
                    }
                    else
                    {
                        listX[i] = phys;
                    }
                }
            }
            else if (convType.toLower() == "tab_verb")
            {
                COMPU_TAB_REF *item = (COMPU_TAB_REF*)cmp->getItem("COMPU_TAB_REF");
                QString compuTabRef = item->getPar("ConversionTable");
                compuVTabAxisX = (COMPU_VTAB*)project->getNode("MODULE/" + moduleName + "/COMPU_VTAB/" + compuTabRef);

                for (int i = 0; i < listX.count(); i++)
                {
                    dec = decX.at(i);
                    listX[i] = compuVTabAxisX->getValue(dec);
                }
            }
            else if (convType.toLower() == "identical")
            {

            }
            else if (convType.toLower() == "form")
            {

            }
            else if (convType.toLower() == "linear")
            {

            }
            else if (convType.toLower() == "tab_intp")
            {
                COMPU_TAB_REF *item = (COMPU_TAB_REF*)cmp->getItem("COMPU_TAB_REF");
                QString compuTabRef = item->getPar("ConversionTable");
                compuTabAxisX = (COMPU_TAB*)project->getNode("MODULE/" + moduleName + "/COMPUTAB/" + compuTabRef);

                QList<float> list = compuTabAxisX->getKeys();
                for (int i = 0; i < listX.count(); i++)
                {
                    //get the decimal value to be calculated
                    dec = decX.at(i);

                    //get the position of the decimal into the keys
                    QList<float>::iterator pos = std::lower_bound(list.begin(), list.end(), dec);

                    //if dec is not included into the keys
                    if (pos == list.end())
                        listX[i] = compuTabAxisX->getValueList().last();
                    else if (pos == list.begin())
                        listX[i] = compuTabAxisX->getValueList().first();
                    else
                    {
                        //interpolate (checking that denominator is difefrent to 0)
                        float x1,x2,y1,y2 = 0;
                        x1 = *(pos - 1);
                        x2 = *pos;
                        y1 = compuTabAxisX->getValue(x1).toFloat();
                        y2 = compuTabAxisX->getValue(x2).toFloat();
                        float denom = (x2 - x1);
                        if (denom)
                            listX[i] = QString::number(y1 + (dec - x1)*(y2 - y1)/denom, 'f', precisionX);
                        else
                            listX[i] = QString::number(0, 'f', precisionX);
                    }
                }
            }
            else if (convType.toLower() == "tab_nointp")
            {

            }
            else
            {

            }
        }
    }

    //axis Y
    if (listY.count() > 0)
    {
        //hex2dec
        QList<double> decY = hex2dec(listY, datatypeY, 16);

        //dec2phys
        if (axisDescrY)
        {
            QString compu_method = axisDescrY->getPar("Conversion");
            COMPU_METHOD *cmp = (COMPU_METHOD*)project->getNode("MODULE/" + moduleName + "/COMPU_METHOD/" + compu_method);
            QString convType = cmp->getPar("ConversionType");

            bool bl;
            double dec;
            if (convType.toLower() == "rat_func")
            {
                COEFFS *item = (COEFFS*)cmp->getItem("COEFFS");
                double a = ((QString)item->getPar("float1")).toDouble(&bl);
                double b = ((QString)item->getPar("float2")).toDouble(&bl);
                double c = ((QString)item->getPar("float3")).toDouble(&bl);
                double d = ((QString)item->getPar("float4")).toDouble(&bl);
                double e = ((QString)item->getPar("float5")).toDouble(&bl);
                double f = ((QString)item->getPar("float6")).toDouble(&bl);

                for (int i = 0; i < listY.count(); i++)
                {
                    dec = decY.at(i);
                    QString phys;
                    if (dec * d - a == 0)
                    {
                        phys = QString::number((c - dec * f) / (dec * e - b),'f', precisionY);
                    }
                    else
                    {
                        phys = QString::number(((b - dec * e) + sqrt(pow((dec * e - b), 2) - 4 * (dec * d - a) * (dec * f - c))) / (2 * (dec * d - a)),'f', precisionY);
                    }

                    double dbl = phys.toDouble();
                    if (dbl == 0)
                    {
                        listY[i] = QString::number(0,'f', precisionY);
                    }
                    else
                    {
                        listY[i] = phys;
                    }
                }
            }
            else if (convType.toLower() == "tab_verb")
            {
                COMPU_TAB_REF *item = (COMPU_TAB_REF*)cmp->getItem("COMPU_TAB_REF");
                QString compuTabRef = item->getPar("ConversionTable");
                compuVTabAxisY = (COMPU_VTAB*)project->getNode("MODULE/" + moduleName + "/COMPU_VTAB/" + compuTabRef);

                for (int i = 0; i < listY.count(); i++)
                {
                    dec = decY.at(i);
                    listY[i] = compuVTabAxisY->getValue(dec);
                }
            }
            else if (convType.toLower() == "identical")
            {

            }
            else if (convType.toLower() == "form")
            {

            }
            else if (convType.toLower() == "linear")
            {

            }
            else if (convType.toLower() == "tab_intp")
            {
                COMPU_TAB_REF *item = (COMPU_TAB_REF*)cmp->getItem("COMPU_TAB_REF");
                QString compuTabRef = item->getPar("ConversionTable");
                compuTabAxisY = (COMPU_TAB*)project->getNode("MODULE/" + moduleName + "/COMPUTAB/" + compuTabRef);

                QList<float> list = compuTabAxisY->getKeys();
                for (int i = 0; i < listY.count(); i++)
                {
                    //get the decimal value to be calculated
                    dec = decY.at(i);

                    //get the position of the decimal into the keys
                    QList<float>::iterator pos = std::lower_bound(list.begin(), list.end(), dec);

                    //if dec is not included into the keys
                    if (pos == list.end())
                        listY[i] = compuTabAxisY->getValueList().last();
                    else if (pos == list.begin())
                        listY[i] = compuTabAxisY->getValueList().first();
                    else
                    {
                        //interpolate (checking that denominator is difefrent to 0)
                        float x1,x2,y1,y2 = 0;
                        x1 = *(pos - 1);
                        x2 = *pos;
                        y1 = compuTabAxisY->getValue(x1).toFloat();
                        y2 = compuTabAxisY->getValue(x2).toFloat();
                        float denom = (x2 - x1);
                        if (denom)
                            listY[i] = QString::number(y1 + (dec - x1)*(y2 - y1)/denom, 'f', precisionY);
                        else
                            listY[i] = QString::number(0, 'f', precisionY);
                    }
                }
            }
            else if (convType.toLower() == "tab_nointp")
            {

            }
            else
            {

            }
        }
    }

    //axis Z
    if (listZ.count() > 0)
    {
        //hex2dec
        QList<double> decZ = hex2dec(listZ, datatypeZ, 16);

        //dec2phys
        QString type = typeid(*label).name();
        QString compu_method;
        if (type.endsWith("CHARACTERISTIC"))
        {
            compu_method = ((CHARACTERISTIC*)label)->getPar("Conversion");
        }
        else
        {
            compu_method = ((AXIS_PTS*)label)->getPar("Conversion");
        }
        COMPU_METHOD *cmpZ = (COMPU_METHOD*)project->getNode("MODULE/" + moduleName + "/COMPU_METHOD/" + compu_method);
        QString convType = cmpZ->getPar("ConversionType");

        bool bl;
        double dec;
        if (convType.toLower() == "rat_func")
        {
            COEFFS *item = (COEFFS*)cmpZ->getItem("COEFFS");
            double a = ((QString)item->getPar("float1")).toDouble(&bl);
            double b = ((QString)item->getPar("float2")).toDouble(&bl);
            double c = ((QString)item->getPar("float3")).toDouble(&bl);
            double d = ((QString)item->getPar("float4")).toDouble(&bl);
            double e = ((QString)item->getPar("float5")).toDouble(&bl);
            double f = ((QString)item->getPar("float6")).toDouble(&bl);

            for (int i = 0; i < listZ.count(); i++)
            {
                dec = decZ.at(i);
                QString phys;
                if (dec * d - a == 0)
                {
                    phys = QString::number((c - dec * f) / (dec * e - b),'f', precisionZ);
                }
                else
                {
                    phys = QString::number(((b - dec * e) + sqrt(pow((dec * e - b), 2) - 4 * (dec * d - a) * (dec * f - c))) / (2 * (dec * d - a)),'f', precisionZ);
                }

                double dbl = phys.toDouble();
                if (dbl == 0)
                {
                    listZ[i] = QString::number(0,'f', precisionZ);
                }
                else
                {
                    listZ[i] = phys;
                }
            }
        }
        else if (convType.toLower() == "tab_verb")
        {
            COMPU_TAB_REF *item = (COMPU_TAB_REF*)cmpZ->getItem("COMPU_TAB_REF");
            QString compuTabRef = item->getPar("ConversionTable");
            compuVTabAxisZ = (COMPU_VTAB*)project->getNode("MODULE/" + moduleName + "/COMPU_VTAB/" + compuTabRef);

            for (int i = 0; i < listZ.count(); i++)
            {
                dec = decZ.at(i);
                listZ[i] = compuVTabAxisZ->getValue((int)dec);
            }

        }
        else if (convType.toLower() == "identical")
        {

        }
        else if (convType.toLower() == "form")
        {

        }
        else if (convType.toLower() == "linear")
        {

        }
        else if (convType.toLower() == "tab_intp")
        {
            COMPU_TAB_REF *item = (COMPU_TAB_REF*)cmpZ->getItem("COMPU_TAB_REF");
            QString compuTabRef = item->getPar("ConversionTable");
            compuTabAxisZ = (COMPU_TAB*)project->getNode("MODULE/" + moduleName + "/COMPUTAB/" + compuTabRef);

            QList<float> list = compuTabAxisZ->getKeys();
            for (int i = 0; i < listZ.count(); i++)
            {
                //get the decimal value to be calculated
                dec = decZ.at(i);

                //get the position of the decimal into the keys
                QList<float>::iterator pos = std::lower_bound(list.begin(), list.end(), dec);

                //if dec is not included into the keys
                if (pos == list.end())
                    listZ[i] = compuTabAxisZ->getValueList().last();
                else if (pos == list.begin())
                    listZ[i] = compuTabAxisZ->getValueList().first();
                else
                {
                    //interpolate (checking that denominator is difefrent to 0)
                    float x1,x2,y1,y2 = 0;
                    x1 = *(pos - 1);
                    x2 = *pos;
                    y1 = compuTabAxisZ->getValue(x1).toFloat();
                    y2 = compuTabAxisZ->getValue(x2).toFloat();
                    float denom = (x2 - x1);
                    if (denom)
                        listZ[i] = QString::number(y1 + (dec - x1)*(y2 - y1)/denom, 'f', precisionZ);
                    else
                        listZ[i] = QString::number(0, 'f', precisionZ);
                }
            }
        }
        else if (convType.toLower() == "tab_nointp")
        {

        }
        else
        {

        }
    }
}

QString Data::hex2phys(QString hex, QString axis)
{
    QString phys = "";
    if (axis == "x" && axisDescrX)
    {
        //hex2dec
        double dec = hex2dec(hex, datatypeX, 16);

        //dec2phys
        QString compu_method = axisDescrX->getPar("Conversion");
        COMPU_METHOD *cmp = (COMPU_METHOD*)project->getNode("MODULE/" + moduleName + "/COMPU_METHOD/" + compu_method);
        if (cmp)
        {
            QString convType = cmp->getPar("ConversionType");

            bool bl;
            if (convType.toLower() == "rat_func")
            {
                COEFFS *item = (COEFFS*)cmp->getItem("COEFFS");
                double a = ((QString)item->getPar("float1")).toDouble(&bl);
                double b = ((QString)item->getPar("float2")).toDouble(&bl);
                double c = ((QString)item->getPar("float3")).toDouble(&bl);
                double d = ((QString)item->getPar("float4")).toDouble(&bl);
                double e = ((QString)item->getPar("float5")).toDouble(&bl);
                double f = ((QString)item->getPar("float6")).toDouble(&bl);

                if (dec * d - a == 0)
                {
                    phys.setNum((c - dec * f) / (dec * e - b),'f', precisionX);
                }
                else
                {
                    phys.setNum(((b - dec * e) + sqrt(pow((dec * e - b), 2) - 4 * (dec * d - a) * (dec * f - c))) / (2 * (dec * d - a)),'f', precisionX);
                }

                double dbl = phys.toDouble();
                if (dbl == 0)
                {
                    return QString::number(0,'f', precisionX);
                }
                else
                {
                    return phys;
                }
            }
            else if (convType.toLower() == "tab_verb")
            {
                COMPU_TAB_REF *item = (COMPU_TAB_REF*)cmp->getItem("COMPU_TAB_REF");
                QString compuTabRef = item->getPar("ConversionTable");
                compuVTabAxisX = (COMPU_VTAB*)project->getNode("MODULE/" + moduleName + "/COMPU_VTAB/" + compuTabRef);

                phys = compuVTabAxisX->getValue(dec);
                return phys;

            }
            else if (convType.toLower() == "tab_intp")
            {
                COMPU_TAB_REF *item = (COMPU_TAB_REF*)cmp->getItem("COMPU_TAB_REF");
                QString compuTabRef = item->getPar("ConversionTable");
                compuTabAxisX = (COMPU_TAB*)project->getNode("MODULE/" + moduleName + "/COMPUTAB/" + compuTabRef);

                QList<float> list = compuTabAxisX->getKeys();

                //get the position of the decimal into the keys
                QList<float>::iterator pos = std::lower_bound(list.begin(), list.end(), dec);

                //if dec is not included into the keys
                if (pos == list.end())
                    return compuTabAxisX->getValueList().last();
                else if (pos == list.begin())
                    return compuTabAxisX->getValueList().first();
                else
                {
                    //interpolate (checking that denominator is difefrent to 0)
                    float x1,x2,y1,y2 = 0;
                    x1 = *(pos - 1);
                    x2 = *pos;
                    y1 = compuTabAxisX->getValue(x1).toFloat();
                    y2 = compuTabAxisX->getValue(x2).toFloat();
                    float denom = (x2 - x1);
                    if (denom)
                        return QString::number(y1 + (dec - x1)*(y2 - y1)/denom, 'f', precisionX);
                    else
                        return QString::number(0, 'f', precisionX);
                }
            }
            else
                return hex;
        }
        else
        {
            return QString::number(dec,'f', precisionX);
        }
    }
    else if (axis == "y" && axisDescrY)
    {
        //hex2dec
        double dec = hex2dec(hex, datatypeY, 16);

        //dec2phys
        QString compu_method = axisDescrY->getPar("Conversion");
        COMPU_METHOD *cmp = (COMPU_METHOD*)project->getNode("MODULE/" + moduleName + "/COMPU_METHOD/" + compu_method);
        if (cmp)
        {
            QString convType = cmp->getPar("ConversionType");

            bool bl;
            if (convType.toLower() == "rat_func")
            {
                COEFFS *item = (COEFFS*)cmp->getItem("COEFFS");
                double a = ((QString)item->getPar("float1")).toDouble(&bl);
                double b = ((QString)item->getPar("float2")).toDouble(&bl);
                double c = ((QString)item->getPar("float3")).toDouble(&bl);
                double d = ((QString)item->getPar("float4")).toDouble(&bl);
                double e = ((QString)item->getPar("float5")).toDouble(&bl);
                double f = ((QString)item->getPar("float6")).toDouble(&bl);

                if (dec * d - a == 0)
                {
                    phys.setNum((c - dec * f) / (dec * e - b),'f', precisionY);
                }
                else
                {
                    phys.setNum(((b - dec * e) + sqrt(pow((dec * e - b), 2) - 4 * (dec * d - a) * (dec * f - c))) / (2 * (dec * d - a)),'f', precisionY);
                }

                double dbl = phys.toDouble();
                if (dbl == 0)
                {
                    return QString::number(0,'f', precisionY);
                }
                else
                {
                    return phys;
                }
            }
            else if (convType.toLower() == "tab_verb")
            {
                COMPU_TAB_REF *item = (COMPU_TAB_REF*)cmp->getItem("COMPU_TAB_REF");
                QString compuTabRef = item->getPar("ConversionTable");
                compuVTabAxisY = (COMPU_VTAB*)project->getNode("MODULE/" + moduleName + "/COMPU_VTAB/" + compuTabRef);

                phys = compuVTabAxisY->getValue(dec);
                return phys;

            }
            else if (convType.toLower() == "tab_intp")
            {
                COMPU_TAB_REF *item = (COMPU_TAB_REF*)cmp->getItem("COMPU_TAB_REF");
                QString compuTabRef = item->getPar("ConversionTable");
                compuTabAxisY = (COMPU_TAB*)project->getNode("MODULE/" + moduleName + "/COMPUTAB/" + compuTabRef);

                QList<float> list = compuTabAxisY->getKeys();

                //get the position of the decimal into the keys
                QList<float>::iterator pos = std::lower_bound(list.begin(), list.end(), dec);

                //if dec is not included into the keys
                if (pos == list.end())
                    return compuTabAxisY->getValueList().last();
                else if (pos == list.begin())
                    return compuTabAxisY->getValueList().first();
                else
                {
                    //interpolate (checking that denominator is difefrent to 0)
                    float x1,x2,y1,y2 = 0;
                    x1 = *(pos - 1);
                    x2 = *pos;
                    y1 = compuTabAxisY->getValue(x1).toFloat();
                    y2 = compuTabAxisY->getValue(x2).toFloat();
                    float denom = (x2 - x1);
                    if (denom)
                        return QString::number(y1 + (dec - x1)*(y2 - y1)/denom, 'f', precisionY);
                    else
                        return QString::number(0, 'f', precisionY);
                }
            }
            else
                return hex;
        }
        else
        {
            return QString::number(dec,'f', precisionY);
        }
    }
    else if (axis == "z")
    {
        //hex2dec
        long double dec = hex2dec(hex, datatypeZ, 16);

        //dec2phys
        QString type = typeid(*label).name();
        QString compu_method;
        if (type.endsWith("CHARACTERISTIC"))
        {
            compu_method = ((CHARACTERISTIC*)label)->getPar("Conversion");
        }
        else
        {
            compu_method = ((AXIS_PTS*)label)->getPar("Conversion");
        }

        COMPU_METHOD *cmpZ = (COMPU_METHOD*)project->getNode("MODULE/" + moduleName + "/COMPU_METHOD/" + compu_method);
        if (cmpZ)
        {
            QString convType = cmpZ->getPar("ConversionType");
            if (convType.toLower() == "rat_func")
            {
                COEFFS *item = (COEFFS*)cmpZ->getItem("COEFFS");
                long double a = ((QString)item->getPar("float1")).toDouble();
                long double b = ((QString)item->getPar("float2")).toDouble();
                long double c = ((QString)item->getPar("float3")).toDouble();
                long double d = ((QString)item->getPar("float4")).toDouble();
                long double e = ((QString)item->getPar("float5")).toDouble();
                long double f = ((QString)item->getPar("float6")).toDouble();

                double dbl = 0;
                if (dec * d - a == 0)
                {
                    double num = c - dec * f;
                    double denum = dec * e - b;
                    dbl = num / denum;
                    //dbl = (c - dec * f) / (dec * e - b);
                }
                else
                {
                    double num = (b - dec * e) + sqrt(pow((dec * e - b), 2) - 4 * (dec * d - a) * (dec * f - c));
                    double denum = 2 * (dec * d - a);
                    dbl = num / denum;
                    //dbl = ((b - dec * e) + sqrt(pow((dec * e - b), 2) - 4 * (dec * d - a) * (dec * f - c))) / (2 * (dec * d - a));
                }

                if (dbl == 0)
                {
                    return QString::number(0,'f', precisionZ);
                }
                else
                {
                    return QString::number(dbl,'f', precisionZ);
                }
            }
            else if (convType.toLower() == "tab_verb")
            {
                COMPU_TAB_REF *item = (COMPU_TAB_REF*)cmpZ->getItem("COMPU_TAB_REF");
                QString compuTabRef = item->getPar("ConversionTable");
                compuVTabAxisZ = (COMPU_VTAB*)project->getNode("MODULE/" + moduleName + "/COMPU_VTAB/" + compuTabRef);

                return compuVTabAxisZ->getValue(dec);
            }
            else if (convType.toLower() == "tab_intp")
            {
                COMPU_TAB_REF *item = (COMPU_TAB_REF*)cmpZ->getItem("COMPU_TAB_REF");
                QString compuTabRef = item->getPar("ConversionTable");
                compuTabAxisZ = (COMPU_TAB*)project->getNode("MODULE/" + moduleName + "/COMPU_TAB/" + compuTabRef);

                QList<float> list = compuTabAxisZ->getKeys();

                //get the position of the decimal into the keys
                QList<float>::iterator pos = std::lower_bound(list.begin(), list.end(), dec);

                //if dec is not included into the keys
                if (pos == list.end())
                    return compuTabAxisZ->getValueList().last();
                else if (pos == list.begin())
                    return compuTabAxisZ->getValueList().first();
                else
                {
                    //interpolate (checking that denominator is difefrent to 0)
                    float x1,x2,y1,y2 = 0;
                    x1 = *(pos - 1);
                    x2 = *pos;
                    y1 = compuTabAxisZ->getValue(x1).toFloat();
                    y2 = compuTabAxisZ->getValue(x2).toFloat();
                    float denom = (x2 - x1);
                    if (denom)
                        return QString::number(y1 + (dec - x1)*(y2 - y1)/denom, 'f', precisionZ);
                    else
                        return QString::number(0, 'f', precisionZ);
                }
            }
            return hex;
        }
        else
        {
            //return QString::number((uint)dec);
            return QString::number(dec,'f', precisionZ);
        }
    }
    else
        return hex;

}

QStringList Data::hex2phys(QStringList list, QString axis)
{
    QStringList phys;
    if (axis == "x" && axisDescrX)
    {
        //hex2dec
        QList<double> decList = hex2dec(list, datatypeX, 16);

        //dec2phys
        QString compu_method = axisDescrX->getPar("Conversion");
        COMPU_METHOD *cmp = (COMPU_METHOD*)project->getNode("MODULE/" + moduleName + "/COMPU_METHOD/" + compu_method);
        QString convType = cmp->getPar("ConversionType");

        bool bl;
        if (convType.toLower() == "rat_func")
        {
            COEFFS *item = (COEFFS*)cmp->getItem("COEFFS");
            double a = ((QString)item->getPar("float1")).toDouble(&bl);
            double b = ((QString)item->getPar("float2")).toDouble(&bl);
            double c = ((QString)item->getPar("float3")).toDouble(&bl);
            double d = ((QString)item->getPar("float4")).toDouble(&bl);
            double e = ((QString)item->getPar("float5")).toDouble(&bl);
            double f = ((QString)item->getPar("float6")).toDouble(&bl);

            foreach (double dec, decList)
            {
                QString str;
                if (dec * d - a == 0)
                {
                    str = QString::number((c - dec * f) / (dec * e - b),'f', precisionX);
                }
                else
                {
                    str = QString::number(((b - dec * e) + sqrt(pow((dec * e - b), 2) - 4 * (dec * d - a) * (dec * f - c))) / (2 * (dec * d - a)),'f', precisionX);
                }

                double dbl = str.toDouble();
                if (dbl == 0)
                {
                    phys.append(QString::number(0,'f', precisionX));
                }
                else
                {
                    phys.append(str);
                }
            }
            return phys;
        }
        else if (convType.toLower() == "tab_verb")
        {
            COMPU_TAB_REF *item = (COMPU_TAB_REF*)cmp->getItem("COMPU_TAB_REF");
            QString compuTabRef = item->getPar("ConversionTable");
            compuVTabAxisX = (COMPU_VTAB*)project->getNode("MODULE/" + moduleName + "/COMPU_VTAB/" + compuTabRef);

            foreach (double dec, decList)
            {
                phys.append(compuVTabAxisX->getValue(dec));
            }
            return phys;

        }
        else if (convType.toLower() == "tab_intp")
        {
            COMPU_TAB_REF *item = (COMPU_TAB_REF*)cmp->getItem("COMPU_TAB_REF");
            QString compuTabRef = item->getPar("ConversionTable");
            compuTabAxisX = (COMPU_TAB*)project->getNode("MODULE/" + moduleName + "/COMPUTAB/" + compuTabRef);

            QList<float> list = compuTabAxisX->getKeys();
            foreach (double dec, decList)
            {
                //get the position of the decimal into the keys
                QList<float>::iterator pos = std::lower_bound(list.begin(), list.end(), dec);

                //if dec is not included into the keys
                if (pos == list.end())
                    phys.append(compuTabAxisX->getValueList().last());
                else if (pos == list.begin())
                    phys.append(compuTabAxisX->getValueList().first());
                else
                {
                    //interpolate (checking that denominator is different to 0)
                    float x1,x2,y1,y2 = 0;
                    x1 = *(pos - 1);
                    x2 = *pos;
                    y1 = compuTabAxisX->getValue(x1).toFloat();
                    y2 = compuTabAxisX->getValue(x2).toFloat();
                    float denom = (x2 - x1);
                    if (denom)
                        phys.append(QString::number(y1 + (dec - x1)*(y2 - y1)/denom, 'f', precisionX));
                    else
                        phys.append(QString::number(0, 'f', precisionX));
                }
            }
            return phys;
        }
        else
            return list;
    }
    else if (axis == "y" && axisDescrY)
    {
        //hex2dec
        QList<double> decList = hex2dec(list, datatypeY, 16);

        //dec2phys
        QString compu_method = axisDescrY->getPar("Conversion");
        COMPU_METHOD *cmp = (COMPU_METHOD*)project->getNode("MODULE/" + moduleName + "/COMPU_METHOD/" + compu_method);
        QString convType = cmp->getPar("ConversionType");

        bool bl;
        if (convType.toLower() == "rat_func")
        {
            COEFFS *item = (COEFFS*)cmp->getItem("COEFFS");
            double a = ((QString)item->getPar("float1")).toDouble(&bl);
            double b = ((QString)item->getPar("float2")).toDouble(&bl);
            double c = ((QString)item->getPar("float3")).toDouble(&bl);
            double d = ((QString)item->getPar("float4")).toDouble(&bl);
            double e = ((QString)item->getPar("float5")).toDouble(&bl);
            double f = ((QString)item->getPar("float6")).toDouble(&bl);

            foreach (double dec, decList)
            {
                QString str;
                if (dec * d - a == 0)
                {
                    str = QString::number((c - dec * f) / (dec * e - b),'f', precisionY);
                }
                else
                {
                    str = QString::number(((b - dec * e) + sqrt(pow((dec * e - b), 2) - 4 * (dec * d - a) * (dec * f - c))) / (2 * (dec * d - a)),'f', precisionY);
                }

                double dbl = str.toDouble();
                if (dbl == 0)
                {
                    phys.append(QString::number(0,'f', precisionY));
                }
                else
                {
                    phys.append(str);
                }
            }

            return phys;
        }
        else if (convType.toLower() == "tab_verb")
        {
            COMPU_TAB_REF *item = (COMPU_TAB_REF*)cmp->getItem("COMPU_TAB_REF");
            QString compuTabRef = item->getPar("ConversionTable");
            compuVTabAxisY = (COMPU_VTAB*)project->getNode("MODULE/" + moduleName + "/COMPU_VTAB/" + compuTabRef);

            foreach (double dec, decList)
            {
                phys.append(compuVTabAxisY->getValue(dec));
            }

            return phys;

        }
        else if (convType.toLower() == "tab_intp")
        {
            COMPU_TAB_REF *item = (COMPU_TAB_REF*)cmp->getItem("COMPU_TAB_REF");
            QString compuTabRef = item->getPar("ConversionTable");
            compuTabAxisY = (COMPU_TAB*)project->getNode("MODULE/" + moduleName + "/COMPUTAB/" + compuTabRef);

            QList<float> list = compuTabAxisY->getKeys();
            foreach (double dec, decList)
            {
                //get the position of the decimal into the keys
                QList<float>::iterator pos = std::lower_bound(list.begin(), list.end(), dec);

                //if dec is not included into the keys
                if (pos == list.end())
                    phys.append(compuTabAxisY->getValueList().last());
                else if (pos == list.begin())
                    phys.append(compuTabAxisY->getValueList().first());
                else
                {
                    //interpolate (checking that denominator is difefrent to 0)
                    float x1,x2,y1,y2 = 0;
                    x1 = *(pos - 1);
                    x2 = *pos;
                    y1 = compuTabAxisY->getValue(x1).toFloat();
                    y2 = compuTabAxisY->getValue(x2).toFloat();
                    float denom = (x2 - x1);
                    if (denom)
                        phys.append(QString::number(y1 + (dec - x1)*(y2 - y1)/denom, 'f', precisionY));
                    else
                        phys.append(QString::number(0, 'f', precisionY));
                }
            }
            return phys;
        }
        else
            return list;
    }
    else if (axis == "z")
    {
        //hex2dec
        QList<double> decList = hex2dec(list, datatypeZ, 16);

        //dec2phys
        QString type = typeid(*label).name();
        QString compu_method;
        if (type.endsWith("CHARACTERISTIC"))
        {
            compu_method = ((CHARACTERISTIC*)label)->getPar("Conversion");
        }
        else
        {
            compu_method = ((AXIS_PTS*)label)->getPar("Conversion");
        }
        COMPU_METHOD *cmpZ = (COMPU_METHOD*)project->getNode("MODULE/" + moduleName + "/COMPU_METHOD/" + compu_method);

        if (cmpZ)
        {
            QString convType = cmpZ->getPar("ConversionType");

            bool bl;
            if (convType.toLower() == "rat_func")
            {
                COEFFS *item = (COEFFS*)cmpZ->getItem("COEFFS");
                long double a = ((QString)item->getPar("float1")).toDouble(&bl);
                long double b = ((QString)item->getPar("float2")).toDouble(&bl);
                long double c = ((QString)item->getPar("float3")).toDouble(&bl);
                long double d = ((QString)item->getPar("float4")).toDouble(&bl);
                long double e = ((QString)item->getPar("float5")).toDouble(&bl);
                long double f = ((QString)item->getPar("float6")).toDouble(&bl);

                foreach (long double dec, decList)
                {
                    QString str;
                    if (dec * d - a == 0)
                    {
                        double num = c - dec * f;
                        double denum = dec * e - b;
                        double dbl = num / denum;
                        str = QString::number(dbl,'f', precisionZ);
                    }
                    else
                    {
                        double num = (b - dec * e) + sqrt(pow((dec * e - b), 2) - 4 * (dec * d - a) * (dec * f - c));
                        double denum = 2 * (dec * d - a);
                        double dbl = num / denum;
                        str = QString::number(dbl,'f', precisionZ);
                    }

                    double dbl = str.toDouble();
                    if (dbl == 0)
                    {
                        phys.append(QString::number(0,'f', precisionZ));
                    }
                    else
                    {
                        phys.append(str);
                    }
                }

                return phys;
            }
            else if (convType.toLower() == "tab_verb")
            {
                COMPU_TAB_REF *item = (COMPU_TAB_REF*)cmpZ->getItem("COMPU_TAB_REF");
                QString compuTabRef = item->getPar("ConversionTable");
                compuVTabAxisZ = (COMPU_VTAB*)project->getNode("MODULE/" + moduleName + "/COMPU_VTAB/" + compuTabRef);

                foreach (double dec, decList)
                {
                    phys.append(compuVTabAxisZ->getValue(dec));
                }

                return phys;
            }
            else if (convType.toLower() == "tab_intp")
            {
                COMPU_TAB_REF *item = (COMPU_TAB_REF*)cmpZ->getItem("COMPU_TAB_REF");
                QString compuTabRef = item->getPar("ConversionTable");
                compuTabAxisZ = (COMPU_TAB*)project->getNode("MODULE/" + moduleName + "/COMPUTAB/" + compuTabRef);

                QList<float> list = compuTabAxisZ->getKeys();

                foreach (double dec, decList)
                {
                    //get the position of the decimal into the keys
                    QList<float>::iterator pos = std::lower_bound(list.begin(), list.end(), dec);

                    //if dec is not included into the keys
                    if (pos == list.end())
                        phys.append(compuTabAxisZ->getValueList().last());
                    else if (pos == list.begin())
                        phys.append(compuTabAxisZ->getValueList().first());
                    else
                    {
                        //interpolate (checking that denominator is difefrent to 0)
                        float x1,x2,y1,y2 = 0;
                        x1 = *(pos - 1);
                        x2 = *pos;
                        y1 = compuTabAxisZ->getValue(x1).toFloat();
                        y2 = compuTabAxisZ->getValue(x2).toFloat();
                        float denom = (x2 - x1);
                        if (denom)
                            phys.append(QString::number(y1 + (dec - x1)*(y2 - y1)/denom, 'f', precisionZ));
                        else
                            phys.append(QString::number(0, 'f', precisionZ));
                    }

                    return phys;
                }

                return phys;
            }
            else
                return list;
        }
        else
        {
            return list;
        }
    }
    else
        return list;
}

double Data::hex2dec(QString hex, std::string type,  int base)
{
    bool bl;
    //unsigned int uval = hex.mid(0,8).toUInt(&bl, base);
    unsigned int uval = hex.toUInt(&bl, base);

    if (bl)
    {
        if(type == "SBYTE")
        {
            int i;
            if (uval > CHAR_MAX)
                i =  uval - UCHAR_MAX - 1;
            else
                i = uval;

            double d = (double)i;
            return d;

        }
        else if(type == "SWORD")
        {
            int i;
            if (uval > SHRT_MAX)
                i =  uval - USHRT_MAX - 1;
            else
                i = uval;
            double d = (double)i;
            return d;
        }
        else if(type == "SLONG")
        {
            int i;
            if (uval > INT_MAX )
                i =  uval - UINT_MAX - 1;
            else
                i = uval;
            double d = (double)i;
            return d;

        }
        else if(type == "FLOAT32_IEEE")
        {
            float f;
                    sscanf(hex.toLocal8Bit().data(), "%x", (int*) &f);
            return f;

        }
        else
        {
            double d = (double)uval;
            return d;
        }
    }
    else
        return 0;
}

QList<double> Data::hex2dec(QStringList listHex, std::string type,  int base)
{
    QList<double> listDec;
    unsigned int uval = 0;
    bool bl = false;

    if(type == "SBYTE")
    {
        int i = 0;
        for (int n = 0; n < listHex.count(); n++)
        {
            uval = listHex.at(n).toUInt(&bl, base);
            if (uval > CHAR_MAX)
                i =  uval - UCHAR_MAX - 1;
            else
                i = uval;

            listDec.append((double)i);
        }
        return listDec;
    }
    else if(type == "SWORD")
    {
        int i = 0;
        for (int n = 0; n < listHex.count(); n++)
        {
            uval = listHex.at(n).toUInt(&bl, base);
            if (uval > SHRT_MAX)
                i =  uval - USHRT_MAX - 1;
            else
                i = uval;

            listDec.append((double)i);
        }
         return listDec;
    }
    else if(type == "SLONG")
    {
        int i;
        for (int n = 0; n < listHex.count(); n++)
        {
            uval = listHex.at(n).toUInt(&bl, base);
            if (uval > INT_MAX )
                i =  uval - UINT_MAX - 1;
            else
                i = uval;

             listDec.append((double)i);
        }
        return listDec;
    }
    else if(type == "FLOAT32_IEEE")
    {
        float f;
        for (int n = 0; n < listHex.count(); n++)
        {
            sscanf(listHex.at(n).toLocal8Bit().data(), "%x", (int*) &f);
            listDec.append(f);
        }
        return listDec;
    }
    else
    {
        for (int n = 0; n < listHex.count(); n++)
        {
            uval = listHex.at(n).toUInt(&bl, base);
            double d = (double)uval;
            listDec.append(d);
        }
        return listDec;
    }

}

QStringList Data::dec2Phys(QList<double> decValues, QString axis)
{
    QStringList list;

    if (axis == "x" && axisDescrX)
    {
        QString compu_method = axisDescrX->getPar("Conversion");
        COMPU_METHOD *cmp = (COMPU_METHOD*)project->getNode("MODULE/" + moduleName + "/COMPU_METHOD/" + compu_method);
        if (cmp)
        {
            QString convType = cmp->getPar("ConversionType");

            bool bl;
            double dec;
            if (convType.toLower() == "rat_func")
            {
                COEFFS *item = (COEFFS*)cmp->getItem("COEFFS");
                double a = ((QString)item->getPar("float1")).toDouble(&bl);
                double b = ((QString)item->getPar("float2")).toDouble(&bl);
                double c = ((QString)item->getPar("float3")).toDouble(&bl);
                double d = ((QString)item->getPar("float4")).toDouble(&bl);
                double e = ((QString)item->getPar("float5")).toDouble(&bl);
                double f = ((QString)item->getPar("float6")).toDouble(&bl);

                for (int i = 0; i < decValues.count(); i++)
                {
                    dec = decValues.at(i);
                    QString phys;
                    if (dec * d - a == 0)
                    {
                        phys = QString::number((c - dec * f) / (dec * e - b),'f', precisionX);
                    }
                    else
                    {
                        phys = QString::number(((b - dec * e) + sqrt(pow((dec * e - b), 2) - 4 * (dec * d - a) * (dec * f - c))) / (2 * (dec * d - a)),'f', precisionX);
                    }

                    double dbl = phys.toDouble();
                    if (dbl == 0)
                    {
                        list.append(QString::number(0,'f', precisionX));
                    }
                    else
                    {
                        list.append(phys);
                    }
                }
            }
            else if (convType.toLower() == "tab_verb")
            {
                COMPU_TAB_REF *item = (COMPU_TAB_REF*)cmp->getItem("COMPU_TAB_REF");
                QString compuTabRef = item->getPar("ConversionTable");
                compuVTabAxisX = (COMPU_VTAB*)project->getNode("MODULE/" + moduleName + "/COMPU_VTAB/" + compuTabRef);

                for (int i = 0; i < decValues.count(); i++)
                {
                    dec = decValues.at(i);
                    list.append(compuVTabAxisX->getValue(dec));
                }
            }
            else if (convType.toLower() == "tab_intp")
            {
                COMPU_TAB_REF *item = (COMPU_TAB_REF*)cmp->getItem("COMPU_TAB_REF");
                QString compuTabRef = item->getPar("ConversionTable");
                compuTabAxisX = (COMPU_TAB*)project->getNode("MODULE/" + moduleName + "/COMPU_TAB/" + compuTabRef);

                QList<float> listKeys = compuTabAxisX->getKeys();
                foreach (double dec, decValues)
                {
                    //get the position of the decimal into the keys
                    QList<float>::iterator pos = std::lower_bound(listKeys.begin(), listKeys.end(), dec);

                    //if dec is not included into the keys
                    if (pos == listKeys.end())
                        list.append(compuTabAxisX->getValueList().last());
                    else if (pos == listKeys.begin())
                        list.append(compuTabAxisX->getValueList().first());
                    else
                    {
                        //interpolate (checking that denominator is difefrent to 0)
                        float x1,x2,y1,y2 = 0;
                        x1 = *(pos - 1);
                        x2 = *pos;
                        y1 = compuTabAxisX->getValue(x1).toFloat();
                        y2 = compuTabAxisX->getValue(x2).toFloat();
                        float denom = (x2 - x1);
                        if (denom)
                            list.append(QString::number(y1 + (dec - x1)*(y2 - y1)/denom, 'f', precisionX));
                        else
                            list.append(QString::number(0, 'f', precisionX));
                    }
                }
            }
        }
        else
        {
            int cnt = decValues.count();
            for (int i = 0; i < cnt; i++)
            {
                list.append(QString::number(decValues.at(i),'f', precisionY));
            }
        }
    }
    else if (axis == "y" && axisDescrY)
    {
        QString compu_method = axisDescrY->getPar("Conversion");
        COMPU_METHOD *cmp = (COMPU_METHOD*)project->getNode("MODULE/" + moduleName + "/COMPU_METHOD/" + compu_method);
        if (cmp)
        {
            QString convType = cmp->getPar("ConversionType");

            bool bl;
            double dec;
            if (convType.toLower() == "rat_func")
            {
                COEFFS *item = (COEFFS*)cmp->getItem("COEFFS");
                double a = ((QString)item->getPar("float1")).toDouble(&bl);
                double b = ((QString)item->getPar("float2")).toDouble(&bl);
                double c = ((QString)item->getPar("float3")).toDouble(&bl);
                double d = ((QString)item->getPar("float4")).toDouble(&bl);
                double e = ((QString)item->getPar("float5")).toDouble(&bl);
                double f = ((QString)item->getPar("float6")).toDouble(&bl);

                for (int i = 0; i < decValues.count(); i++)
                {
                    dec = decValues.at(i);
                    QString phys;
                    if (dec * d - a == 0)
                    {
                        phys = QString::number((c - dec * f) / (dec * e - b),'f', precisionY);
                    }
                    else
                    {
                        phys = QString::number(((b - dec * e) + sqrt(pow((dec * e - b), 2) - 4 * (dec * d - a) * (dec * f - c))) / (2 * (dec * d - a)),'f', precisionY);
                    }

                    double dbl = phys.toDouble();
                    if (dbl == 0)
                    {
                        list.append(QString::number(0,'f', precisionY));
                    }
                    else
                    {
                        list.append(phys);
                    }
                }
            }
            else if (convType.toLower() == "tab_verb")
            {
                COMPU_TAB_REF *item = (COMPU_TAB_REF*)cmp->getItem("COMPU_TAB_REF");
                QString compuTabRef = item->getPar("ConversionTable");
                compuVTabAxisY = (COMPU_VTAB*)project->getNode("MODULE/" + moduleName + "/COMPU_VTAB/" + compuTabRef);

                for (int i = 0; i < decValues.count(); i++)
                {
                    dec = decValues.at(i);
                    list.append(compuVTabAxisY->getValue(dec));
                }
            }
            else if (convType.toLower() == "tab_intp")
            {
                COMPU_TAB_REF *item = (COMPU_TAB_REF*)cmp->getItem("COMPU_TAB_REF");
                QString compuTabRef = item->getPar("ConversionTable");
                compuTabAxisY = (COMPU_TAB*)project->getNode("MODULE/" + moduleName + "/COMPU_TAB/" + compuTabRef);

                QList<float> listKeys = compuTabAxisY->getKeys();
                foreach (double dec, decValues)
                {
                    //get the position of the decimal into the keys
                    QList<float>::iterator pos = std::lower_bound(listKeys.begin(), listKeys.end(), dec);

                    //if dec is not included into the keys
                    if (pos == listKeys.end())
                        list.append(compuTabAxisY->getValueList().last());
                    else if (pos == listKeys.begin())
                        list.append(compuTabAxisY->getValueList().first());
                    else
                    {
                        //interpolate (checking that denominator is difefrent to 0)
                        float x1,x2,y1,y2 = 0;
                        x1 = *(pos - 1);
                        x2 = *pos;
                        y1 = compuTabAxisY->getValue(x1).toFloat();
                        y2 = compuTabAxisY->getValue(x2).toFloat();
                        float denom = (x2 - x1);
                        if (denom)
                            list.append(QString::number(y1 + (dec - x1)*(y2 - y1)/denom, 'f', precisionY));
                        else
                            list.append(QString::number(0, 'f', precisionY));
                    }
                }
            }
        }
        else
        {
            int cnt = decValues.count();
            for (int i = 0; i < cnt; i++)
            {
                list.append(QString::number(decValues.at(i),'f', precisionY));
            }
        }
    }
    else if (axis == "z")
    {
        if (compu_methodZ)
        {
            QString convType = compu_methodZ->getPar("ConversionType");

            if (convType.toLower() == "rat_func")
            {
                COEFFS *item = (COEFFS*)compu_methodZ->getItem("COEFFS");
                long double a = ((QString)item->getPar("float1")).toDouble();
                long double b = ((QString)item->getPar("float2")).toDouble();
                long double c = ((QString)item->getPar("float3")).toDouble();
                long double d = ((QString)item->getPar("float4")).toDouble();
                long double e = ((QString)item->getPar("float5")).toDouble();
                long double f = ((QString)item->getPar("float6")).toDouble();

                int cnt = decValues.count();
                for (int i = 0; i < cnt; i++)
                {
                    double dbl = 0;
                    long double dec = decValues.at(i);
                    if (dec * d - a == 0)
                    {
                        double num = c - dec * f;
                        double denum = dec * e - b;
                        dbl = num / denum;
                        //dbl = (c - dec * f) / (dec * e - b);
                    }
                    else
                    {
                        double num = (b - dec * e) + sqrt(pow((dec * e - b), 2) - 4 * (dec * d - a) * (dec * f - c));
                        double denum = 2 * (dec * d - a);
                        dbl = num / denum;
                        //dbl = ((b - dec * e) + sqrt(pow((dec * e - b), 2) - 4 * (dec * d - a) * (dec * f - c))) / (2 * (dec * d - a));
                    }

                    if (dbl == 0)
                    {
                        list.append(QString::number(0,'f', precisionZ));
                    }
                    else
                    {
                        list.append(QString::number(dbl,'f', precisionZ));
                    }
                }
            }
            else if (convType.toLower() == "tab_verb")
            {
                COMPU_TAB_REF *item = (COMPU_TAB_REF*)compu_methodZ->getItem("COMPU_TAB_REF");
                QString compuTabRef = item->getPar("ConversionTable");
                if (hexParent)
                    compuVTabAxisZ = (COMPU_VTAB*)hexParent->compu_vtab->getNode(compuTabRef);
                else if (srecParent)
                    compuVTabAxisZ = (COMPU_VTAB*)srecParent->compu_vtab->getNode(compuTabRef);

                for (int i = 0; i < decValues.count(); i++)
                {
                    double dec = decValues.at(i);
                    list.append(compuVTabAxisZ->getValue((int)dec));
                }

            }
            else if (convType.toLower() == "tab_intp")
            {
                COMPU_TAB_REF *item = (COMPU_TAB_REF*)compu_methodZ->getItem("COMPU_TAB_REF");
                QString compuTabRef = item->getPar("ConversionTable");
                if (hexParent)
                {
                    compuTabAxisZ = (COMPU_TAB*)hexParent->compu_tab->getNode(compuTabRef);
                }
                else if (srecParent)
                    compuTabAxisZ = (COMPU_TAB*)srecParent->compu_tab->getNode(compuTabRef);

                QList<float> listKeys = compuTabAxisZ->getKeys();
                for (int i = 0; i < decValues.count(); i++)
                {

                    //get the position of the decimal into the keys
                    QList<float>::iterator pos = std::lower_bound(listKeys.begin(), listKeys.end(), decValues.at(i));

                    //if dec is not included into the keys
                    if (pos == listKeys.end())
                        list.append( compuTabAxisZ->getValueList().last());
                    else if (pos == listKeys.begin())
                        list.append(compuTabAxisZ->getValueList().first());
                    else
                    {
                        //interpolate (checking that denominator is difefrent to 0)
                        float x1,x2,y1,y2 = 0;
                        x1 = *(pos - 1);
                        x2 = *pos;
                        y1 = compuTabAxisZ->getValue(x1).toFloat();
                        y2 = compuTabAxisZ->getValue(x2).toFloat();
                        float denom = (x2 - x1);
                        if (denom)
                            list.append(QString::number(y1 + (decValues.at(i) - x1)*(y2 - y1)/denom , 'f', precisionZ));
                    }
                }
            }
        }
        else
        {
            int cnt = decValues.count();
            for (int i = 0; i < cnt; i++)
            {
                list.append(QString::number(decValues.at(i),'f', precisionZ));
            }
        }
    }

    return list;
}

// --- PHYS TO HEX --- //

void Data::phys2hex()
{
    //axis X
    if (listX.count() > 0)
    {
        //phys2dec
        QList<double> decX;
        if (axisDescrX)
        {
            QString compu_method = axisDescrX->getPar("Conversion");
            COMPU_METHOD *cmp = (COMPU_METHOD*)project->getNode("MODULE/" + moduleName + "/COMPU_METHOD/" + compu_method);
            QString convType = cmp->getPar("ConversionType");

            bool bl;
            double phys;
            if (convType.toLower() == "rat_func")
            {
                COEFFS *item = (COEFFS*)cmp->getItem("COEFFS");
                double a = ((QString)item->getPar("float1")).toDouble(&bl);
                double b = ((QString)item->getPar("float2")).toDouble(&bl);
                double c = ((QString)item->getPar("float3")).toDouble(&bl);
                double d = ((QString)item->getPar("float4")).toDouble(&bl);
                double e = ((QString)item->getPar("float5")).toDouble(&bl);
                double f = ((QString)item->getPar("float6")).toDouble(&bl);

                for (int i = 0; i < listX.count(); i++)
                {
                    //phys = listX.at(i).toDouble();
                    phys = checkLimitsX(listX[i]).toDouble();
                    double dec = (a * pow(phys, 2) + b * phys + c) / (d * pow(phys, 2) + e * phys + f);
                    decX.append(dec);
                }
            }
            else if (convType.toLower() == "tab_verb")
            {
                COMPU_TAB_REF *item = (COMPU_TAB_REF*)cmp->getItem("COMPU_TAB_REF");
                QString compuTabRef = item->getPar("ConversionTable");
                compuVTabAxisX = (COMPU_VTAB*)project->getNode("MODULE/" + moduleName + "/COMPU_VTAB/" + compuTabRef);

                for (int i = 0; i < listX.count(); i++)
                {
                    QString str  = listX.at(i);
                    double dec = compuVTabAxisX->getPos(str);
                    decX.append(dec);
                }
            }
            else if (convType.toLower() == "identical")
            {

            }
            else if (convType.toLower() == "form")
            {

            }
            else if (convType.toLower() == "linear")
            {

            }
            else if (convType.toLower() == "tab_intp")
            {

            }
            else if (convType.toLower() == "tab_nointp")
            {

            }
            else
            {

            }

            //dec2hex
            listX = dec2hex(decX, datatypeX, 16);
        }

    }

    //axis Y
    if (listY.count() > 0)
    {
        //phys2dec
         QList<double> decY;
        if (axisDescrY)
        {
            QString compu_method = axisDescrY->getPar("Conversion");
            COMPU_METHOD *cmp = (COMPU_METHOD*)project->getNode("MODULE/" + moduleName + "/COMPU_METHOD/" + compu_method);
            QString convType = cmp->getPar("ConversionType");

            bool bl;
            double phys;
            if (convType.toLower() == "rat_func")
            {
                COEFFS *item = (COEFFS*)cmp->getItem("COEFFS");
                double a = ((QString)item->getPar("float1")).toDouble(&bl);
                double b = ((QString)item->getPar("float2")).toDouble(&bl);
                double c = ((QString)item->getPar("float3")).toDouble(&bl);
                double d = ((QString)item->getPar("float4")).toDouble(&bl);
                double e = ((QString)item->getPar("float5")).toDouble(&bl);
                double f = ((QString)item->getPar("float6")).toDouble(&bl);

                for (int i = 0; i < listY.count(); i++)
                {
                    //phys = listY.at(i).toDouble();
                    phys = checkLimitsY(listY[i]).toDouble();
                    double dec = (a * pow(phys, 2) + b * phys + c) / (d * pow(phys, 2) + e * phys + f);
                    decY.append(dec);
                }
            }
            else if (convType.toLower() == "tab_verb")
            {
                COMPU_TAB_REF *item = (COMPU_TAB_REF*)cmp->getItem("COMPU_TAB_REF");
                QString compuTabRef = item->getPar("ConversionTable");
                compuVTabAxisY = (COMPU_VTAB*)project->getNode("MODULE/" + moduleName + "/COMPU_VTAB/" + compuTabRef);

                for (int i = 0; i < listY.count(); i++)
                {
                    QString str  = listY.at(i);
                    double dec = compuVTabAxisY->getPos(str);
                    decY.append(dec);
                }
            }
            else if (convType.toLower() == "identical")
            {

            }
            else if (convType.toLower() == "form")
            {

            }
            else if (convType.toLower() == "linear")
            {

            }
            else if (convType.toLower() == "tab_intp")
            {

            }
            else if (convType.toLower() == "tab_nointp")
            {

            }
            else
            {

            }

            //dec2hex
            listY = dec2hex(decY, datatypeY, 16);
        }
    }

    //axis Z
    if (listZ.count() > 0)
    {
        //phys2dec
        QList<double> decZ;
        QString type = typeid(*label).name();
        QString compu_method;
        if (type.endsWith("CHARACTERISTIC"))
        {
            compu_method = ((CHARACTERISTIC*)label)->getPar("Conversion");
        }
        else
        {
            compu_method = ((AXIS_PTS*)label)->getPar("Conversion");
        }
        COMPU_METHOD *cmpZ = (COMPU_METHOD*)project->getNode("MODULE/" + moduleName + "/COMPU_METHOD/" + compu_method);
        QString convType = cmpZ->getPar("ConversionType");

        bool bl;
        double phys;
        if (convType.toLower() == "rat_func")
        {
            COEFFS *item = (COEFFS*)cmpZ->getItem("COEFFS");
            double a = ((QString)item->getPar("float1")).toDouble(&bl);
            double b = ((QString)item->getPar("float2")).toDouble(&bl);
            double c = ((QString)item->getPar("float3")).toDouble(&bl);
            double d = ((QString)item->getPar("float4")).toDouble(&bl);
            double e = ((QString)item->getPar("float5")).toDouble(&bl);
            double f = ((QString)item->getPar("float6")).toDouble(&bl);

            for (int i = 0; i < listZ.count(); i++)
            {
                phys = checkLimitsZ(listZ[i]).toDouble();
                double dec = (a * pow(phys, 2) + b * phys + c) / (d * pow(phys, 2) + e * phys + f);
                decZ.append(dec);
            }
        }
        else if (convType.toLower() == "tab_verb")
        {
            COMPU_TAB_REF *item = (COMPU_TAB_REF*)cmpZ->getItem("COMPU_TAB_REF");
            QString compuTabRef = item->getPar("ConversionTable");
            compuVTabAxisZ = (COMPU_VTAB*)project->getNode("MODULE/" + moduleName + "/COMPU_VTAB/" + compuTabRef);

            for (int i = 0; i < listZ.count(); i++)
            {
                QString str  = listZ.at(i);
                double dec = compuVTabAxisZ->getPos(str);
                decZ.append(dec);
            }
        }
        else if (convType.toLower() == "identical")
        {

        }
        else if (convType.toLower() == "form")
        {

        }
        else if (convType.toLower() == "linear")
        {

        }
        else if (convType.toLower() == "tab_intp")
        {
            COMPU_TAB_REF *item = (COMPU_TAB_REF*)cmpZ->getItem("COMPU_TAB_REF");
            QString compuTabRef = item->getPar("ConversionTable");
            compuTabAxisZ = (COMPU_TAB*)project->getNode("MODULE/" + moduleName + "/COMPU_TAB/" + compuTabRef);

            QStringList list = compuTabAxisZ->getValueList();
            for (int i = 0; i < listZ.count(); i++)
            {
                //str to convert from QString to hex
                QString str  = listZ.at(i);

                //get the position of the decimal into the keys
                QStringList::iterator pos = std::lower_bound(list.begin(), list.end(), str, myCompare);

                //if dec is not included into the keys
                if (pos == list.end())
                    decZ.append(compuTabAxisZ->getKeys().last());
                else if (pos == list.begin())
                    decZ.append(compuTabAxisZ->getKeys().first());
                else
                {
                    //interpolate (checking that denominator is difefrent to 0)
                    float x1,x2,y1,y2 = 0;
                    x1 = (*(pos - 1)).toFloat();
                    x2 = (*pos).toFloat();
                    y1 = compuTabAxisZ->getValue(x1).toFloat();
                    y2 = compuTabAxisZ->getValue(x2).toFloat();
                    float denom = (y2 - y1);
                    if (denom)
                        decZ.append(x1 + (str.toFloat() - y1)*(x2 - x1)/denom);
                    else
                        decZ.append(0);
                }
            }
        }
        else if (convType.toLower() == "tab_nointp")
        {

        }
        else
        {

        }

        //dec2hex
        listZ = dec2hex(decZ, datatypeZ, 16);

    }

}

QString Data::phys2hex(QString phys, QString axis)
{
    if (axis == "x" && axisDescrX)
    {
        //phys2dec
        double dec = 0;
        QString compu_method = axisDescrX->getPar("Conversion");
        COMPU_METHOD *cmp = (COMPU_METHOD*)project->getNode("MODULE/" + moduleName + "/COMPU_METHOD/" + compu_method);
        QString convType = cmp->getPar("ConversionType");

        bool bl;
        if (convType.toLower() == "rat_func")
        {
            COEFFS *item = (COEFFS*)cmp->getItem("COEFFS");
            double a = ((QString)item->getPar("float1")).toDouble(&bl);
            double b = ((QString)item->getPar("float2")).toDouble(&bl);
            double c = ((QString)item->getPar("float3")).toDouble(&bl);
            double d = ((QString)item->getPar("float4")).toDouble(&bl);
            double e = ((QString)item->getPar("float5")).toDouble(&bl);
            double f = ((QString)item->getPar("float6")).toDouble(&bl);

            double val = checkLimitsX(phys).toDouble();

            dec = (a * pow(val, 2) + b * val + c) / (d * pow(val, 2) + e * val + f);
        }
        else if (convType.toLower() == "tab_verb")
        {
            COMPU_TAB_REF *item = (COMPU_TAB_REF*)cmp->getItem("COMPU_TAB_REF");
            QString compuTabRef = item->getPar("ConversionTable");
            compuVTabAxisX = (COMPU_VTAB*)project->getNode("MODULE/" + moduleName + "/COMPU_VTAB/" + compuTabRef);

            dec = compuVTabAxisX->getPos(phys);

        }
        else if (convType.toLower() == "identical")
        {

        }
        else if (convType.toLower() == "form")
        {

        }
        else if (convType.toLower() == "linear")
        {

        }
        else if (convType.toLower() == "tab_intp")
        {

        }
        else if (convType.toLower() == "tab_nointp")
        {

        }
        else
        {

        }

        //dec2hex
        return dec2hex(dec, datatypeX, 16);
    }
    else if (axis == "y" && axisDescrY)
    {
        //phys2dec
        double dec = 0;
        QString compu_method = axisDescrY->getPar("Conversion");
        COMPU_METHOD *cmp = (COMPU_METHOD*)project->getNode("MODULE/" + moduleName + "/COMPU_METHOD/" + compu_method);
        QString convType = cmp->getPar("ConversionType");

        bool bl;
        if (convType.toLower() == "rat_func")
        {
            COEFFS *item = (COEFFS*)cmp->getItem("COEFFS");
            double a = ((QString)item->getPar("float1")).toDouble(&bl);
            double b = ((QString)item->getPar("float2")).toDouble(&bl);
            double c = ((QString)item->getPar("float3")).toDouble(&bl);
            double d = ((QString)item->getPar("float4")).toDouble(&bl);
            double e = ((QString)item->getPar("float5")).toDouble(&bl);
            double f = ((QString)item->getPar("float6")).toDouble(&bl);

            double val = checkLimitsY(phys).toDouble();
            dec = (a * pow(val, 2) + b * val + c) / (d * pow(val, 2) + e * val + f);
        }
        else if (convType.toLower() == "tab_verb")
        {
            COMPU_TAB_REF *item = (COMPU_TAB_REF*)cmp->getItem("COMPU_TAB_REF");
            QString compuTabRef = item->getPar("ConversionTable");
            compuVTabAxisY = (COMPU_VTAB*)project->getNode("MODULE/" + moduleName + "/COMPU_VTAB/" + compuTabRef);

            dec = compuVTabAxisY->getPos(phys);

        }
        else if (convType.toLower() == "identical")
        {

        }
        else if (convType.toLower() == "form")
        {

        }
        else if (convType.toLower() == "linear")
        {

        }
        else if (convType.toLower() == "tab_intp")
        {

        }
        else if (convType.toLower() == "tab_nointp")
        {

        }
        else
        {

        }

        //dec2hex
        return dec2hex(dec, datatypeY, 16);
    }
    else if (axis == "z")
    {
        double dec = phys.toDouble();
        QString type = typeid(*label).name();
        QString compu_method;
        if (type.endsWith("CHARACTERISTIC"))
        {
            compu_method = ((CHARACTERISTIC*)label)->getPar("Conversion");
        }
        else
        {
            compu_method = ((AXIS_PTS*)label)->getPar("Conversion");
        }


        COMPU_METHOD *cmpZ = (COMPU_METHOD*)project->getNode("MODULE/" + moduleName + "/COMPU_METHOD/" + compu_method);
        if (cmpZ)
        {
            QString convType = cmpZ->getPar("ConversionType");

            bool bl;
            if (convType.toLower() == "rat_func")
            {
                COEFFS *item = (COEFFS*)cmpZ->getItem("COEFFS");
                double a = ((QString)item->getPar("float1")).toDouble(&bl);
                double b = ((QString)item->getPar("float2")).toDouble(&bl);
                double c = ((QString)item->getPar("float3")).toDouble(&bl);
                double d = ((QString)item->getPar("float4")).toDouble(&bl);
                double e = ((QString)item->getPar("float5")).toDouble(&bl);
                double f = ((QString)item->getPar("float6")).toDouble(&bl);

                double val = checkLimitsZ(phys).toDouble();
                dec = (a * pow(val, 2) + b * val + c) / (d * pow(val, 2) + e * val + f);
            }
            else if (convType.toLower() == "tab_verb")
            {
                COMPU_TAB_REF *item = (COMPU_TAB_REF*)cmpZ->getItem("COMPU_TAB_REF");
                QString compuTabRef = item->getPar("ConversionTable");
                compuVTabAxisZ = (COMPU_VTAB*)project->getNode("MODULE/" + moduleName + "/COMPU_VTAB/" + compuTabRef);

                dec = compuVTabAxisZ->getPos(phys);
            }
            else if (convType.toLower() == "identical")
            {

            }
            else if (convType.toLower() == "form")
            {

            }
            else if (convType.toLower() == "linear")
            {

            }
            else if (convType.toLower() == "tab_intp")
            {
                COMPU_TAB_REF *item = (COMPU_TAB_REF*)cmpZ->getItem("COMPU_TAB_REF");
                QString compuTabRef = item->getPar("ConversionTable");
                compuTabAxisZ = (COMPU_TAB*)project->getNode("MODULE/" + moduleName + "/COMPU_TAB/" + compuTabRef);

                QStringList list = compuTabAxisZ->getValueList();


                //get the position of the decimal into the keys
                QStringList::iterator pos = std::lower_bound(list.begin(), list.end(), phys, myCompare);

                //if dec is not included into the keys
                if (pos == list.end())
                    dec = compuTabAxisZ->getKeys().last();
                else if (pos == list.begin())
                    dec = compuTabAxisZ->getKeys().first();
                else
                {
                    //interpolate (checking that denominator is difefrent to 0)
                    float x1,x2,y1,y2 = 0;
                    x1 = (*(pos - 1)).toFloat();
                    x2 = (*pos).toFloat();
                    y1 = compuTabAxisZ->getValue(x1).toFloat();
                    y2 = compuTabAxisZ->getValue(x2).toFloat();
                    float denom = (y2 - y1);
                    if (denom)
                        dec = x1 + (phys.toFloat() - y1)*(x2 - x1)/denom;
                    else
                        dec = 0;
                }
            }
            else if (convType.toLower() == "tab_nointp")
            {

            }
            else
            {

            }
        }
            //dec2hex
            return dec2hex(dec, datatypeZ, 16);
        }
    else
        return phys;
}

QStringList Data::phys2hex(QStringList list, QString axis)
{
    if (axis == "x" && axisDescrX)
    {
        //phys2dec
        QList<double> listDec;
        QString compu_method = axisDescrX->getPar("Conversion");
        COMPU_METHOD *cmp = (COMPU_METHOD*)project->getNode("MODULE/" + moduleName + "/COMPU_METHOD/" + compu_method);
        QString convType = cmp->getPar("ConversionType");

        bool bl;
        if (convType.toLower() == "rat_func")
        {
            COEFFS *item = (COEFFS*)cmp->getItem("COEFFS");
            double a = ((QString)item->getPar("float1")).toDouble(&bl);
            double b = ((QString)item->getPar("float2")).toDouble(&bl);
            double c = ((QString)item->getPar("float3")).toDouble(&bl);
            double d = ((QString)item->getPar("float4")).toDouble(&bl);
            double e = ((QString)item->getPar("float5")).toDouble(&bl);
            double f = ((QString)item->getPar("float6")).toDouble(&bl);

            foreach (QString phys, list)
            {
                double val = checkLimitsX(phys).toDouble();
                listDec.append((a * pow(val, 2) + b * val + c) / (d * pow(val, 2) + e * val + f));
            }
        }
        else if (convType.toLower() == "tab_verb")
        {
            COMPU_TAB_REF *item = (COMPU_TAB_REF*)cmp->getItem("COMPU_TAB_REF");
            QString compuTabRef = item->getPar("ConversionTable");
            compuVTabAxisX = (COMPU_VTAB*)project->getNode("MODULE/" + moduleName + "/COMPU_VTAB/" + compuTabRef);

            foreach (QString phys, list)
            {
                listDec.append(compuVTabAxisX->getPos(phys));
            }

        }
        else if (convType.toLower() == "identical")
        {

        }
        else if (convType.toLower() == "form")
        {

        }
        else if (convType.toLower() == "linear")
        {

        }
        else if (convType.toLower() == "tab_intp")
        {

        }
        else if (convType.toLower() == "tab_nointp")
        {

        }
        else
        {

        }

        //dec2hex
        return dec2hex(listDec, datatypeX, 16);
    }
    else if (axis == "y" && axisDescrY)
    {
        //phys2dec
        QList<double> listDec;
        QString compu_method = axisDescrY->getPar("Conversion");
        COMPU_METHOD *cmp = (COMPU_METHOD*)project->getNode("MODULE/" + moduleName + "/COMPU_METHOD/" + compu_method);
        QString convType = cmp->getPar("ConversionType");

        bool bl;
        if (convType.toLower() == "rat_func")
        {
            COEFFS *item = (COEFFS*)cmp->getItem("COEFFS");
            double a = ((QString)item->getPar("float1")).toDouble(&bl);
            double b = ((QString)item->getPar("float2")).toDouble(&bl);
            double c = ((QString)item->getPar("float3")).toDouble(&bl);
            double d = ((QString)item->getPar("float4")).toDouble(&bl);
            double e = ((QString)item->getPar("float5")).toDouble(&bl);
            double f = ((QString)item->getPar("float6")).toDouble(&bl);

            foreach (QString phys, list)
            {
                double val = checkLimitsY(phys).toDouble();
                listDec.append((a * pow(val, 2) + b * val + c) / (d * pow(val, 2) + e * val + f));
            }
        }
        else if (convType.toLower() == "tab_verb")
        {
            COMPU_TAB_REF *item = (COMPU_TAB_REF*)cmp->getItem("COMPU_TAB_REF");
            QString compuTabRef = item->getPar("ConversionTable");
            compuVTabAxisY = (COMPU_VTAB*)project->getNode("MODULE/" + moduleName + "/COMPU_VTAB/" + compuTabRef);

            foreach (QString phys, list)
            {
                listDec.append(compuVTabAxisY->getPos(phys));
            }

        }
        else if (convType.toLower() == "identical")
        {

        }
        else if (convType.toLower() == "form")
        {

        }
        else if (convType.toLower() == "linear")
        {

        }
        else if (convType.toLower() == "tab_intp")
        {

        }
        else if (convType.toLower() == "tab_nointp")
        {

        }
        else
        {

        }

        //dec2hex
        return dec2hex(listDec, datatypeY, 16);
    }
    else if (axis == "z")
    {
        QList<double> listDec;
        QString type = typeid(*label).name();
        QString compu_method;
        if (type.endsWith("CHARACTERISTIC"))
        {
            compu_method = ((CHARACTERISTIC*)label)->getPar("Conversion");
        }
        else
        {
            compu_method = ((AXIS_PTS*)label)->getPar("Conversion");
        }
        COMPU_METHOD *cmpZ = (COMPU_METHOD*)project->getNode("MODULE/" + moduleName + "/COMPU_METHOD/" + compu_method);
        QString convType = cmpZ->getPar("ConversionType");

        bool bl;
        if (convType.toLower() == "rat_func")
        {
            COEFFS *item = (COEFFS*)cmpZ->getItem("COEFFS");
            double a = ((QString)item->getPar("float1")).toDouble(&bl);
            double b = ((QString)item->getPar("float2")).toDouble(&bl);
            double c = ((QString)item->getPar("float3")).toDouble(&bl);
            double d = ((QString)item->getPar("float4")).toDouble(&bl);
            double e = ((QString)item->getPar("float5")).toDouble(&bl);
            double f = ((QString)item->getPar("float6")).toDouble(&bl);

            foreach (QString phys, list)
            {
                double val = checkLimitsZ(phys).toDouble();
                listDec.append((a * pow(val, 2) + b * val + c) / (d * pow(val, 2) + e * val + f));
            }
        }
        else if (convType.toLower() == "tab_verb")
        {
            COMPU_TAB_REF *item = (COMPU_TAB_REF*)cmpZ->getItem("COMPU_TAB_REF");
            QString compuTabRef = item->getPar("ConversionTable");
            compuVTabAxisZ = (COMPU_VTAB*)project->getNode("MODULE/" + moduleName + "/COMPU_VTAB/" + compuTabRef);

            foreach (QString phys, list)
            {
                listDec.append(compuVTabAxisZ->getPos(phys));
            }
        }
        else if (convType.toLower() == "identical")
        {

        }
        else if (convType.toLower() == "form")
        {

        }
        else if (convType.toLower() == "linear")
        {

        }
        else if (convType.toLower() == "tab_intp")
        {
            COMPU_TAB_REF *item = (COMPU_TAB_REF*)cmpZ->getItem("COMPU_TAB_REF");
            QString compuTabRef = item->getPar("ConversionTable");
            compuTabAxisZ = (COMPU_TAB*)project->getNode("MODULE/" + moduleName + "/COMPU_TAB/" + compuTabRef);

            QStringList listKeys = compuTabAxisZ->getValueList();
            foreach (QString phys, list)
            {

                //get the position of the decimal into the keys
                QStringList::iterator pos = std::lower_bound(listKeys.begin(), listKeys.end(), phys, myCompare);

                //if dec is not included into the keys
                if (pos == listKeys.end())
                    listDec.append(compuTabAxisZ->getKeys().last());
                else if (pos == listKeys.begin())
                    listDec.append(compuTabAxisZ->getKeys().first());
                else
                {
                    //interpolate (checking that denominator is difefrent to 0)
                    float x1,x2,y1,y2 = 0;
                    x1 = (*(pos - 1)).toFloat();
                    x2 = (*pos).toFloat();
                    y1 = compuTabAxisZ->getValue(x1).toFloat();
                    y2 = compuTabAxisZ->getValue(x2).toFloat();
                    float denom = (y2 - y1);
                    if (denom)
                        listDec.append(x1 + (phys.toFloat() - y1)*(x2 - x1)/denom);
                    else
                        listDec.append(0);
                }
            }
        }
        else if (convType.toLower() == "tab_nointp")
        {

        }
        else
        {

        }

        //dec2hex
        return dec2hex(listDec, datatypeZ, 16);
    }
    else
        return list;
}

QStringList Data::dec2hex(QList<double> listDec, std::string type,  int base)
{
    QStringList listHex;
    for (int i = 0; i < listDec.count(); i++)
    {
        listHex.append(dec2hex(listDec.at(i), type, base));
    }
    return listHex;
}

QString Data::dec2hex(double dec, std::string type, int base)
{
    QString qHex = "";
    char hex[31];
    base ++; //useless but to prevent from warning unused variable

    if (type == "")
        type = "UBYTE";

    if(type == "SBYTE")
    {
        int8_t E = (int8_t)dec;

        // if(E > SHRT_MAX)
        // {
        //     E = SHRT_MAX;
        // }

        if ((0.5 <= dec - E && dec >= 0 )|| (dec - E > -0.5 && dec < 0))
        {
            if (E < CHAR_MAX)
            {
                sprintf(hex, "%X", (int8_t)ceil(dec));

            }
            else
            {
                sprintf(hex, "%X", E);
            }
        }
        else
        {
            if (E > CHAR_MIN)
            {
                sprintf(hex, "%X", (int8_t)floor(dec));
            }
            else
            {
                sprintf(hex, "%X", E);
            }
        }

        qHex = QString(hex).right(2);
        while (qHex.length() < 2)
            qHex = "0" + qHex;

        return qHex;
    }
    else if(type == "UBYTE")
    {

        uint8_t E = (uint8_t)dec;

        if(E > UCHAR_MAX)
        {
            E = UCHAR_MAX;
        }

        if (0.5 <= dec - E)
        {
            if (E < UCHAR_MAX)
            {
                sprintf(hex, "%X", (uint8_t)ceil(dec));
            }
            else
            {
                sprintf(hex, "%X", E);
            }
        }
        else
        {
            if (E > 0)
            {
                sprintf(hex, "%X", (uint8_t)floor(dec));
            }
            else
            {
                sprintf(hex, "%X", E);
            }
        }
        qHex = QString(hex).right(2);
        while (qHex.length() < 2)
            qHex = "0" + qHex;

        return qHex;
    }
    else if(type == "SWORD")
    {
        int16_t E = (int16_t)dec;

        // if(E > SHRT_MAX)
        // {
        //     E = SHRT_MAX;
        // }

        if ((0.5 <= dec - E && dec >=0 )|| (dec - E > -0.5 && dec < 0))
        {
            if (E < SHRT_MAX)
            {
                sprintf(hex, "%X", (int16_t)ceil(dec));
            }
            else
            {
                sprintf(hex, "%X", E);
            }
        }
        else
        {
            if (E > SHRT_MIN)
            {
                sprintf(hex, "%X", (int16_t)floor(dec));
            }
            else
            {
                sprintf(hex, "%X", E);
            }
        }
        qHex = QString(hex).right(4);
        while (qHex.length() < 4)
            qHex = "0" + qHex;

        return qHex;
    }
    else if(type == "UWORD")
    {
        uint16_t E = (uint16_t)dec;

        if(E > USHRT_MAX)
        {
            E = USHRT_MAX;
        }

        if (0.5 <= dec - E)
        {
            if (E < USHRT_MAX)
            {
                sprintf(hex, "%X", (uint16_t)ceil(dec));
            }
            else
            {
                sprintf(hex, "%X", E);
            }
        }
        else
        {
            if (E > 0)
            {
                sprintf(hex, "%X", (uint16_t)floor(dec));
            }
            else
            {
                sprintf(hex, "%X", E);
            }
        }
        qHex = QString(hex).right(4);
        while (qHex.length() < 4)
            qHex = "0" + qHex;

        return qHex;
    }
    else if(type == "SLONG")
    {
        int32_t E = (int32_t)dec;

        if(E > INT_MAX)
        {
            E = INT_MAX;
        }

        if ((0.5 <= dec - E && dec >=0 ) || (dec - E > -0.5 && dec < 0))
        {
            if (E < INT_MAX)
            {
                sprintf(hex, "%X", (int32_t)ceil(dec));
            }
            else
            {
                sprintf(hex, "%X", E);
            }
        }
        else
        {
            if (E > INT_MIN)
            {
                sprintf(hex, "%X", (int32_t)floor(dec));
            }
            else
            {
                sprintf(hex, "%X", E);
            }
        }
        qHex = QString(hex).right(8);
        while (qHex.length() < 8)
            qHex = "0" + qHex;

        return qHex;
    }
    else if(type == "ULONG")
    {
        uint32_t E = (uint32_t)dec;

        //E = (unsigned int)dec;
        if(E > UINT_MAX)
        {
            E = UINT_MAX;
        }

        if (0.5 <= dec - E)
        {
            if (E < (unsigned int)UINT_MAX)
            {
                sprintf(hex, "%X", (uint32_t)ceil(dec));
            }
            else
            {
                sprintf(hex, "%X", E);
            }
        }
        else
        {
            if (E > 0)
            {
                sprintf(hex, "%X", (uint32_t)floor(dec));
            }
            else
            {
                sprintf(hex, "%X", E);
            }
        }
        qHex = QString(hex).right(8);
        while (qHex.length() < 8)
            qHex = "0" + qHex;

        return qHex;
    }
    else if(type == "FLOAT32_IEEE")
    {
        float f = (float)dec;
        sprintf(hex, "%08X", *(int*)&f);
        qHex = QString(hex);

        return qHex;
    }

    return qHex;
}

// --- get VALUES --- //

bool Data::checkValues()
{
    // check if the changed values are different to the original value
    bool sameAsOriginal = true;
    if (!xListVal.isEmpty())
    {
        foreach(int i, xListVal.keys())
        {
            if (listX.at(i) != xListVal.value(i).first())
            {
                sameAsOriginal = false;
            }
        }
    }
    if (!yListVal.isEmpty())
    {
        foreach(int i, yListVal.keys())
        {
            if (listY.at(i) != yListVal.value(i).first())
            {
                sameAsOriginal = false;
            }
        }
    }
    if (!zListVal.isEmpty())
    {
        foreach(int i, zListVal.keys())
        {
            if (listZ.at(i) != zListVal.value(i).first())
            {
                sameAsOriginal = false;
            }
        }
    }
    if (sizeChanged)
    {
        sameAsOriginal = false;
    }

    // check if the parent file has been deleted or not
    if (hexParent)
    {
        hexParent->checkDisplay();
    }
    else if (srecParent)
    {
        srecParent->checkDisplay();
    }
    else if (csvParent)
    {
        csvParent->checkDisplay();
    }
    else if (dcmParent)
    {
        dcmParent->checkDisplay();
    }
    else
    {
        cdfxParent->checkDisplay();
    }

    //display the data name as node into treeView
    if (!sameAsOriginal && !displayed)
    {
        if (hexParent)
        {
            hexParent->addModifiedData(this);
        }
        else if (srecParent)
        {
            srecParent->addModifiedData(this);
        }
        else if (csvParent)
        {
            csvParent->addModifiedData(this);
        }
        else if (dcmParent)
        {
            dcmParent->addModifiedData(this);
        }
        else
        {
            cdfxParent->addModifiedData(this);
        }

        displayed = true;
        return true;
    }
    else if (sameAsOriginal && displayed)
    {
        if (hexParent)
        {
            hexParent->removeModifiedData(this);
        }
        else if (srecParent)
        {
            srecParent->removeModifiedData(this);
        }
        else if (csvParent)
        {
            csvParent->removeModifiedData(this);
        }
        else if (dcmParent)
        {
            dcmParent->removeModifiedData(this);
        }
        else
        {
            cdfxParent->removeModifiedData(this);
        }

        displayed = false;
        return false;
    }

    //update all table views
    if (hexParent)
    {
        hexParent->getParentWp()->parentWidget->resetAllTableView();
    }
    else if (srecParent)
    {
        srecParent->getParentWp()->parentWidget->resetAllTableView();
    }
    else if (csvParent)
    {
        csvParent->getParentWp()->parentWidget->resetAllTableView();
    }
    else if (dcmParent)
    {
        dcmParent->getParentWp()->parentWidget->resetAllTableView();
    }
    else
    {
        cdfxParent->getParentWp()->parentWidget->resetAllTableView();
    }

    return false;
}

//--- X ---//

int Data::xCount()
{
    return listX.count();
}

double Data::getAddressX()
{
    return addressX;
}

AXIS_DESCR *Data::getAxisDescrX()
{
    return axisDescrX;
}

COMPU_METHOD *Data::getCompuMethodAxisX()
{
    if (axisDescrX)
    {
        QString conversion = axisDescrX->getPar("Conversion");
        COMPU_METHOD *cpm = (COMPU_METHOD*)project->getNode("MODULE/" + moduleName + "/COMPU_METHOD/" + conversion);

        return cpm;
    }
    else
        return NULL;
}

QString Data::getByteOrderX()
{
    return byteOrderX;
}

COMPU_VTAB *Data::getCompuTabAxisX()
{
    return compuVTabAxisX;
}

QString Data::getOrgX(int i)
{
    if (axisDescrX)
    {
        QString typeAxisX = axisDescrX->getPar("Attribute");
        if (typeAxisX == "COM_AXIS")
        {
            //get data corrisponding to the AXIS_PTS
            AXIS_PTS_REF *axisPtsRef = (AXIS_PTS_REF*)axisDescrX->getItem("AXIS_PTS_REF");
            QString nameAxisX = axisPtsRef->getPar("AxisPoints");

            if (hexParent)
            {
                Data *axisData = hexParent->getData(nameAxisX);
                if (axisData)
                {
                    return axisData->getOrgZ(i);
                }
                else
                    return "error";
            }
            else if (srecParent)
            {
                Data *axisData = srecParent->getData(nameAxisX);
                if (axisData)
                {
                    return axisData->getOrgZ(i);
                }
                else
                    return "error";
            }
            else if (csvParent)
            {
                Data *axisData = csvParent->getData(nameAxisX);
                if (axisData)
                {
                    return axisData->getOrgZ(i);
                }
                else
                    return QString::number(i);
            }
            else if (cdfxParent)
            {
                Data *axisData = cdfxParent->getData(nameAxisX);
                if (axisData)
                {
                    return axisData->getOrgZ(i);
                }
                else
                    return QString::number(i);
            }
            else if (dcmParent)
            {
                Data *axisData = dcmParent->getData(nameAxisX);
                if (axisData)
                {
                    return axisData->getOrgZ(i);
                }
                else
                    return QString::number(i);
            }
            else
            {
                return "error";
            }

        }
        else
        {
            if (xListVal.contains(i))
            {
                return xListVal.value(i).first();
            }
            else
                return listX.at(i);
        }
    }
    else
    {
        return listX.at(i);
    }
}

QString Data::getX(int i)
{
    if (axisDescrX)
    {
        QString typeAxisX = axisDescrX->getPar("Attribute");
        if (typeAxisX == "COM_AXIS")
        {
            //get data corrisponding to the AXIS_PTS
            AXIS_PTS_REF *axisPtsRef = (AXIS_PTS_REF*)axisDescrX->getItem("AXIS_PTS_REF");
            QString nameAxisX = axisPtsRef->getPar("AxisPoints");

            if (hexParent)
            {
                Data *axisData = hexParent->getData(nameAxisX);
                if (axisData)
                {
                    return axisData->getZ(i);
                }
                else
                    return "error";
            }
            else if (srecParent)
            {
                Data *axisData = srecParent->getData(nameAxisX);
                if (axisData)
                {
                    return axisData->getZ(i);
                }
                else
                    return "error";
            }
            else if (csvParent)
            {
                Data *axisData = csvParent->getData(nameAxisX);
                if (axisData)
                {
                    return axisData->getZ(i);
                }
                else
                    return QString::number(i);
            }
            else if (dcmParent)
            {
                Data *axisData = dcmParent->getData(nameAxisX);
                if (axisData)
                {
                    return axisData->getZ(i);
                }
                else
                    return QString::number(i);
            }
            else if (cdfxParent)
            {
                Data *axisData = cdfxParent->getData(nameAxisX);
                if (axisData)
                {
                    return axisData->getZ(i);
                }
                else
                    return QString::number(i);
            }
            else
            {
                return "error";
            }
        }
        else
        {
            return listX.at(i);
        }
    }
    else
        return listX.at(i);
}

QString Data::getHexX(int col)
{
    QString str = getX(col);
    if (!str.isEmpty())
        str = phys2hex(str,"x");
    else
        return "";

    return str;
}

QStringList Data::getX()
{
    if (axisDescrX)
    {
        QString typeAxisX = axisDescrX->getPar("Attribute");
        if (typeAxisX == "COM_AXIS")
        {
            //get data corrisponding to the AXIS_PTS
            AXIS_PTS_REF *axisPtsRef = (AXIS_PTS_REF*)axisDescrX->getItem("AXIS_PTS_REF");
            QString nameAxisX = axisPtsRef->getPar("AxisPoints");

            if (hexParent)
            {
                Data *axisData = hexParent->getData(nameAxisX);
                if (axisData)
                {
                    return axisData->getZ();
                }
                else
                {
                    return listX;
                }
            }
            else if (srecParent)
            {
                Data *axisData = srecParent->getData(nameAxisX);
                if (axisData)
                {
                    return axisData->getZ();
                }
                else
                {
                    return listX;
                }
            }
            else if (csvParent)
            {
                Data *axisData = csvParent->getData(nameAxisX);
                if (axisData)
                {
                    return axisData->getZ();
                }
                else
                {
                    return listX;
                }
            }
            else if (dcmParent)
            {
                Data *axisData = dcmParent->getData(nameAxisX);
                if (axisData)
                {
                    return axisData->getZ();
                }
                else
                {
                    return listX;
                }
            }
            else if (cdfxParent)
            {
                Data *axisData = cdfxParent->getData(nameAxisX);
                if (axisData)
                {
                    return axisData->getZ();
                }
                else
                {
                    return listX;
                }

            }
            else
            {
                return listX;
            }
        }
        else
        {
            return listX;
        }
    }
    else
        return listX;
}

QList<double> Data::getXtoDouble()
{
    QList<double> list;
    foreach (QString str, listX)
    {
        list.append(str.toDouble());
    }
    return list;
}

//--- Y ---//

int Data::yCount()
{
    return listY.count();
}

double Data::getAddressY()
{
    return addressY;
}

COMPU_METHOD *Data::getCompuMethodAxisY()
{
    if (axisDescrY)
    {
        QString conversion = axisDescrY->getPar("Conversion");
        COMPU_METHOD *cpm = (COMPU_METHOD*)project->getNode("MODULE/" + moduleName + "/COMPU_METHOD/" + conversion);

        return cpm;
    }
    else
        return NULL;
}

AXIS_DESCR *Data::getAxisDescrY()
{
    return axisDescrY;
}

COMPU_VTAB *Data::getCompuTabAxisY()
{
    return compuVTabAxisY;
}

QString Data::getByteOrderY()
{
    return byteOrderY;
}

QString Data::getOrgY(int i)
{
    if (axisDescrY)
    {
        QString typeAxisY = axisDescrY->getPar("Attribute");
        if (typeAxisY == "COM_AXIS")
        {
            //get data corrisponding to the AXIS_PTS
            AXIS_PTS_REF *axisPtsRef = (AXIS_PTS_REF*)axisDescrY->getItem("AXIS_PTS_REF");
            QString nameAxisY = axisPtsRef->getPar("AxisPoints");

            if (hexParent)
            {
                Data *axisData = hexParent->getData(nameAxisY);
                if (axisData)
                {
                    return axisData->getOrgZ(i);
                }
                else
                    return "error";
            }
            else if (srecParent)
            {
                Data *axisData = srecParent->getData(nameAxisY);
                if (axisData)
                {
                    return axisData->getOrgZ(i);
                }
                else
                    return "error";
            }
            else if (csvParent)
            {
                Data *axisData = csvParent->getData(nameAxisY);
                if (axisData)
                {
                    return axisData->getOrgZ(i);
                }
                else
                    return QString::number(i);
            }
            else if (dcmParent)
            {
                Data *axisData = dcmParent->getData(nameAxisY);
                if (axisData)
                {
                    return axisData->getOrgZ(i);
                }
                else
                    return QString::number(i);
            }
            else if (cdfxParent)
            {
                Data *axisData = cdfxParent->getData(nameAxisY);
                if (axisData)
                {
                    return axisData->getOrgZ(i);
                }
                else
                    return QString::number(i);
            }
            else
            {
                return "error";
            }

        }
        else
        {
            if (yListVal.contains(i))
            {
                return yListVal.value(i).first();
            }
            else
                return listY.at(i);
        }
    }
    else
    {
        return listY.at(i);
    }
}

QString Data::getY(int i)
{
    if (axisDescrY)
    {
        QString typeAxisY = axisDescrY->getPar("Attribute");
        if (typeAxisY == "COM_AXIS")
        {
            //get data corrisponding to the AXIS_PTS
            AXIS_PTS_REF *axisPtsRef = (AXIS_PTS_REF*)axisDescrY->getItem("AXIS_PTS_REF");
            QString nameAxisY = axisPtsRef->getPar("AxisPoints");

            if (hexParent)
            {
                Data *axisData = hexParent->getData(nameAxisY);
                if (axisData)
                {
                    return axisData->getZ(i);
                }
                else
                    return "error";
            }
            else if (srecParent)
            {
                Data *axisData = srecParent->getData(nameAxisY);
                if (axisData)
                {
                    return axisData->getZ(i);
                }
                else
                    return "error";
            }
            else if (csvParent)
            {
                Data *axisData = csvParent->getData(nameAxisY);
                if (axisData)
                {
                    return axisData->getZ(i);
                }
                else
                    return QString::number(i);
            }
            else if (dcmParent)
            {
                Data *axisData = dcmParent->getData(nameAxisY);
                if (axisData)
                {
                    return axisData->getZ(i);
                }
                else
                    return QString::number(i);
            }
            else if (cdfxParent)
            {
                Data *axisData = cdfxParent->getData(nameAxisY);
                if (axisData)
                {
                    return axisData->getZ(i);
                }
                else
                    return QString::number(i);
            }
            else
            {
                return "error";
            }

        }
        else
        {
            return listY.at(i);
        }
    }
    else
        return listY.at(i);
}

QString Data::getHexY(int row)
{
    QString str = getY(row);
    if (!str.isEmpty())
        str = phys2hex(str,"y");
    else
        return "";

    return str;
}

QStringList Data::getY()
{
    if (axisDescrY)
    {
        QString typeAxisY = axisDescrY->getPar("Attribute");
        if (typeAxisY == "COM_AXIS")
        {
            //get data corrisponding to the AXIS_PTS
            AXIS_PTS_REF *axisPtsRef = (AXIS_PTS_REF*)axisDescrY->getItem("AXIS_PTS_REF");
            QString nameAxisY = axisPtsRef->getPar("AxisPoints");

            if (hexParent)
            {
                Data *axisData = hexParent->getData(nameAxisY);
                if (axisData)
                {
                    return axisData->getZ();
                }
                else
                {
                    return listY;
                }
            }
            else if (srecParent)
            {
                Data *axisData = srecParent->getData(nameAxisY);
                if (axisData)
                {
                    return axisData->getZ();
                }
                else
                {
                    return listY;
                }
            }
            else if (csvParent)
            {
                Data *axisData = csvParent->getData(nameAxisY);
                if (axisData)
                {
                    return axisData->getZ();
                }
                else
                {
                    return listY;
                }
            }
            else if (dcmParent)
            {
                Data *axisData = dcmParent->getData(nameAxisY);
                if (axisData)
                {
                    return axisData->getZ();
                }
                else
                {
                    return listY;
                }
            }
            else if (cdfxParent)
            {
                Data *axisData = cdfxParent->getData(nameAxisY);
                if (axisData)
                {
                    return axisData->getZ();
                }
                else
                {
                    return listY;
                }
            }
            else
            {
                return listY;
            }

        }
        else
        {
            return listY;
        }
    }
    else
        return listY;
}

QList<double> Data::getYtoDouble()
{
    QList<double> list;
    foreach (QString str, listY)
    {
        list.append(str.toDouble());
    }
    return list;
}

//--- Z ---//

QString Data::getByteOrderZ()
{
    return byteOrderZ;
}

QString Data::getOrgZ(int i)
{
    if (zListVal.contains(i))
    {
        return zListVal.value(i).first();
    }
    else if (0 <= i && i < listZ.size())
    {
        return listZ.at(i);
    }
    else
        return "";
}

QString Data::getOrgZ(int row, int col)
{
    int i = 0;

    //Map
    if (yCount() > 0)
    {
        if (isSortedByRow)
        {
             i = col + row * xCount();
        }
        else
        {
            i = row  + col * yCount();
        }
    }
    else
    {
        i = col;
    }

    return getOrgZ(i);
}

double Data::getOrgZ(int row, int col, bool *bl)
{
    return getOrgZ(row, col).toDouble(bl);
}

QString Data::getZ(int i)
{
    if (0 <= i && i < listZ.size())
        return listZ.at(i);
    else
        return "";
}

QString Data::getHexZ(int i)
{
    QString str = getZ(i);
    if (!str.isEmpty())
        str = phys2hex(str,"z");
    else
        return "";

    return str;
}

QString Data::getZ(int row, int col)
{
    //Map
    if (yCount() > 0)
    {
        if (isSortedByRow)
        {
             return getZ(col + row * xCount());
        }
        else
        {
            return getZ(row  + col * yCount());
        }
    }
    else
    {
        return getZ(col);
    }
}

QString Data::getHexZ(int row, int col)
{
    QString str = getZ(row, col);
    if (!str.isEmpty())
        str = phys2hex(str,"z");
    else
        return "";

    return str;
}

QStringList Data::getZ()
{
    return listZ;
}

QList<QList<double> > Data::getZtoDouble()
{
    int xDim = 0;
    if (xCount() == 0)
    {
        xDim = 1;
    }
    else
    {
        xDim = xCount();
    }

    int yDim = 0;
    if (yCount() == 0)
    {
        yDim = 1;
    }
    else
    {
        yDim = yCount();
    }

    QList<QList<double> > list;
    for (int i = 0; i < yDim; i++)
    {
        QList<double> listi;
        for (int j = 0; j < xDim; j++)
        {
            listi.append(getZ(i,j).toDouble());
        }
        list.append(listi);
    }
    return list;
}

double Data::getZ(int row, int col, bool *bl)
{
    return getZ(row, col).toDouble(bl);
}

double Data::getMaxZ()
{
    double d = getZ(0).toDouble();

    for (int i = 0; i < zCount(); i++)
    {
        if (getZ(i).toDouble() > d)
        {
            d = getZ(i).toDouble();
        }
    }

    return d;
}

int Data::zCount()
{
    return listZ.count();
}

double Data::getAddressZ()
{
    return addressZ;
}

QString  Data::getType()
{
    return type;
}

COMPU_VTAB *Data::getCompuTabAxisZ()
{
    return compuVTabAxisZ;
}

// --- set VALUES --- //

void Data::setX(int i, QString str)
{
    if (axisDescrX)
    {
        QString typeAxisX = axisDescrX->getPar("Attribute");
        if (typeAxisX == "COM_AXIS")
        {
            //get data corrisponding to the AXIS_PTS
            AXIS_PTS_REF *axisPtsRef = (AXIS_PTS_REF*)axisDescrX->getItem("AXIS_PTS_REF");
            QString nameAxisX = axisPtsRef->getPar("AxisPoints");
            Data *axisData;
            if (hexParent)
                axisData = hexParent->getData(nameAxisX);
            else if (srecParent)
                axisData = srecParent->getData(nameAxisX);
            else if (csvParent)
                axisData = csvParent->getData(nameAxisX);
            else if( cdfxParent)
                axisData = cdfxParent->getData(nameAxisX);
            else if( dcmParent)
                axisData = dcmParent->getData(nameAxisX);
            else
                return;

            if (axisData)
            {
                axisData->setZ(i, str);
            }
        }
        else if (typeAxisX == "STD_AXIS")
        {
            if (compuVTabAxisX)
            {
                if (compuVTabAxisX->getValueList().contains(str))
                {
                    //PHYS -> HEX -> PHYS
                    QString hex = phys2hex(str, "x");
                    QString phys = hex2phys(hex, "x");

                    //PHYS <> OLD
                    if (phys != listX.at(i))
                    {
                        QStringList list = xListVal.value(i);
                        list.append(listX.at(i));
                        xListVal.insert(i, list);

                        listX[i] = phys;
                    }

                    //update treeView
                    checkValues();
                }
            }
            else
            {
                //WEAK BOUNDS
                QString strLowerWB = axisDescrX->getPar("LowerLimit");
                double dblLowerWB = strLowerWB.toDouble();
                QString strUpperWB = axisDescrX->getPar("UpperLimit");
                double dblUpperWB = strUpperWB.toDouble();

                //EXTENDED_LIMITS
                EXTENDED_LIMITS *extL = (EXTENDED_LIMITS*)axisDescrX->getItem("EXTENDED_LIMITS");
                QString strUpperExtLim, strLowerExtLim;
                if (extL)
                {
                    strUpperExtLim = ((QString)extL->getPar("UpperLimit"));
                    strLowerExtLim = ((QString)extL->getPar("LowerLimit"));
                }
                double dblUpperExtLim = strUpperExtLim.toDouble();
                double dblLowerExtLim = strLowerExtLim.toDouble();

                //CHECK LIMITS (weak/extended)
                double dbl = str.toDouble();
                if (dbl > dblUpperWB)
                {
                    if (showWeakBoundsExceeded)
                    {
                        DialogExceedWB *diag = new DialogExceedWB( &showWeakBoundsExceeded ,0);
                        int ret = diag->exec();

                        if (ret == QDialog::Accepted)
                        {
                            allowExceedWeakBounds = true;
                        }
                        else
                        {
                            allowExceedWeakBounds = false;
                        }
                    }

                    if (allowExceedWeakBounds && (dbl > dblUpperWB))
                    {
                        if (dbl > dblUpperExtLim)
                        {
                            str = strUpperExtLim;
                        }
                    }
                    else if (!allowExceedWeakBounds && (dbl > dblUpperWB))
                    {
                        str = strUpperWB;
                    }

                }
                else if(dbl < dblLowerWB)
                {
                    if (showWeakBoundsExceeded)
                    {
                        DialogExceedWB *diag = new DialogExceedWB( &showWeakBoundsExceeded ,0);
                        int ret = diag->exec();

                        if (ret == QDialog::Accepted)
                        {
                            allowExceedWeakBounds = true;
                        }
                        else
                        {
                            allowExceedWeakBounds = false;
                        }
                    }

                    if (allowExceedWeakBounds && (dbl < dblLowerWB))
                    {
                        if (dbl < dblLowerExtLim)
                            str = strLowerExtLim;
                    }
                    else if (!allowExceedWeakBounds && (dbl < dblLowerWB))
                        str = strLowerWB;
                }

                //PHYS -> HEX -> PHYS
                QString hex = phys2hex(str, "x");
                QString phys = hex2phys(hex, "x");

                //PHYS <> OLD
                if (phys != listX.at(i))
                {
                    QStringList list = xListVal.value(i);
                    list.append(listX.at(i));
                    xListVal.insert(i, list);

                    listX[i] = phys;
                }

                //update treeView
                checkValues();
            }
        }
    }
}

void Data::setX(QStringList list)
{
    if (axisDescrX)
    {
        QString typeAxisX = axisDescrX->getPar("Attribute");
        if (typeAxisX == "COM_AXIS")
        {
            //get data corrisponding to the AXIS_PTS
            AXIS_PTS_REF *axisPtsRef = (AXIS_PTS_REF*)axisDescrX->getItem("AXIS_PTS_REF");
            QString nameAxisX = axisPtsRef->getPar("AxisPoints");
            Data *axisData;
            if (hexParent)
                axisData = hexParent->getData(nameAxisX);
            else if (srecParent)
                axisData = srecParent->getData(nameAxisX);
            else if (csvParent)
                axisData = csvParent->getData(nameAxisX);
            else if( cdfxParent)
                axisData = cdfxParent->getData(nameAxisX);
            else if( dcmParent)
                axisData = dcmParent->getData(nameAxisX);
            else
                return;

            if (axisData)
            {
                axisData->setZ(list);
            }
        }
        else if (typeAxisX == "STD_AXIS")
        {
            if (compuVTabAxisX)
            {
                //PHYS -> HEX -> PHYS
                QStringList hexList = phys2hex(list, "x");
                QStringList physList = hex2phys(hexList, "x");

                //PHYS <> OLD
                for (int i = 0; i < xCount(); i++)
                {
                    QString phys = physList.at(i);
                    if (phys != listX.at(i) && phys != "ERROR")
                    {
                        QStringList list = xListVal.value(i);
                        list.append(listX.at(i));
                        xListVal.insert(i, list);

                        listX[i] = phys;
                    }
                }

                //update treeView
                checkValues();
            }
            else
            {
                //WEAK BOUNDS
                bool bl;
                QString lower = axisDescrX->getPar("LowerLimit");
                double lowerLimit = lower.toDouble();
                QString upper = axisDescrX->getPar("UpperLimit");
                double upperLimit = upper.toDouble();

                //EXTENDED_LIMITS
                EXTENDED_LIMITS *extL = (EXTENDED_LIMITS*)axisDescrX->getItem("EXTENDED_LIMITS");
                double upperL = 0;
                double lowerL = 0;
                if (extL)
                {
                    upperL = ((QString)extL->getPar("UpperLimit")).toDouble();
                    lowerL = ((QString)extL->getPar("LowerLimit")).toDouble();
                }

                //CHECK LIMITS (weak/extended)
                QStringList limitList;
                foreach(QString str, list)
                {
                    double dbl = str.toDouble(&bl);
                    if (dbl > upperLimit)
                    {
                        if (showWeakBoundsExceeded)
                        {
                            DialogExceedWB *diag = new DialogExceedWB( &showWeakBoundsExceeded ,0);
                            int ret = diag->exec();

                            if (ret == QDialog::Accepted)
                            {
                                allowExceedWeakBounds = true;
                            }
                            else
                            {
                                allowExceedWeakBounds = false;
                            }
                        }

                        if (allowExceedWeakBounds && (dbl > upperLimit))
                        {
                            if (dbl > upperL)
                                str = QString::number(upperL, 'f');
                        }
                        else if (!allowExceedWeakBounds && (dbl > upperLimit))
                            str = QString::number(upperLimit, 'f');

                    }
                    else if(dbl < lowerLimit)
                    {
                        if (showWeakBoundsExceeded)
                        {
                            DialogExceedWB *diag = new DialogExceedWB( &showWeakBoundsExceeded ,0);
                            int ret = diag->exec();

                            if (ret == QDialog::Accepted)
                            {
                                allowExceedWeakBounds = true;
                            }
                            else
                            {
                                allowExceedWeakBounds = false;
                            }
                        }

                        if (allowExceedWeakBounds && (dbl < lowerLimit))
                        {
                            if (dbl < lowerL)
                                str = QString::number(lowerL, 'f');
                        }
                        else if (!allowExceedWeakBounds && (dbl < lowerLimit))
                            str = QString::number(lowerLimit, 'f');
                    }

                    limitList.append(str);
                }

                //PHYS -> HEX -> PHYS
                QStringList hexList = phys2hex(limitList, "x");
                QStringList physList = hex2phys(hexList, "x");

                //PHYS <> OLD
                for (int i = 0; i < xCount(); i++)
                {
                    QString phys = physList.at(i);

                    //compare with original value
                    if (phys != listX.at(i))
                    {
                        QStringList list = xListVal.value(i);
                        list.append(listX.at(i));
                        xListVal.insert(i, list);

                        listX[i] = phys;
                    }
                }

                //update treeView
                checkValues();
            }
        }
    }
}

void Data::setY(int i, QString str)
{
    if (axisDescrY)
    {
        QString typeAxisY = axisDescrY->getPar("Attribute");
        if (typeAxisY == "COM_AXIS")
        {
            //get data corrisponding to the AXIS_PTS
            AXIS_PTS_REF *axisPtsRef = (AXIS_PTS_REF*)axisDescrY->getItem("AXIS_PTS_REF");
            QString nameAxisY = axisPtsRef->getPar("AxisPoints");
            Data *axisData;
            if (hexParent)
                axisData = hexParent->getData(nameAxisY);
            else if (srecParent)
                axisData = srecParent->getData(nameAxisY);
            else if (csvParent)
                axisData = csvParent->getData(nameAxisY);
            else if( cdfxParent)
                axisData = cdfxParent->getData(nameAxisY);
            else if( dcmParent)
                axisData = dcmParent->getData(nameAxisY);
            else
                return;

            if (axisData)
            {
                axisData->setZ(i, str);
            }

        }
        else if (typeAxisY == "STD_AXIS")
        {
            if (compuVTabAxisY)
            {
                if (compuVTabAxisY->getValueList().contains(str))
                {
                    //PHYS -> HEX -> PHYS
                    QString hex = phys2hex(str, "y");
                    QString phys = hex2phys(hex, "y");

                    //PHYS <> OLD
                    if (phys != listY.at(i))
                    {
                        QStringList list = yListVal.value(i);
                        list.append(listY.at(i));
                        yListVal.insert(i, list);

                        listY[i] = phys;
                    }

                    //update treeView
                    checkValues();
                }
            }
            else
            {
                //WEAK BOUNDS
                bool bl;
                QString strLowerWB = axisDescrY->getPar("LowerLimit");
                double dblLowerWB = strLowerWB.toDouble();
                QString strUpperWB = axisDescrY->getPar("UpperLimit");
                double dblUpperWB = strUpperWB.toDouble();

                //EXTENDED_LIMITS
                EXTENDED_LIMITS *extL = (EXTENDED_LIMITS*)axisDescrY->getItem("EXTENDED_LIMITS");
                QString strUpperExtLim, strLowerExtLim;
                double dbl = str.toDouble(&bl);
                if (extL && bl)
                {
                    strUpperExtLim = ((QString)extL->getPar("UpperLimit"));
                    strLowerExtLim = ((QString)extL->getPar("LowerLimit"));
                }
                double dblUpperExtLim = strUpperExtLim.toDouble();
                double dblLowerExtLim = strLowerExtLim.toDouble();

                //CHECK LIMITS (weak/extended)
                if (dbl > dblUpperWB)
                {
                    if (showWeakBoundsExceeded)
                    {
                        DialogExceedWB *diag = new DialogExceedWB( &showWeakBoundsExceeded ,0);
                        int ret = diag->exec();

                        if (ret == QDialog::Accepted)
                        {
                            allowExceedWeakBounds = true;
                        }
                        else
                        {
                            allowExceedWeakBounds = false;
                        }
                    }

                    if (allowExceedWeakBounds && (dbl > dblUpperWB))
                    {
                        if (dbl > dblUpperExtLim)
                            str = strUpperExtLim;
                    }
                    else if (!allowExceedWeakBounds && (dbl > dblUpperWB))
                        str = strUpperWB;

                }
                else if(dbl < dblLowerWB)
                {
                    if (showWeakBoundsExceeded)
                    {
                        DialogExceedWB *diag = new DialogExceedWB( &showWeakBoundsExceeded ,0);
                        int ret = diag->exec();

                        if (ret == QDialog::Accepted)
                        {
                            allowExceedWeakBounds = true;
                        }
                        else
                        {
                            allowExceedWeakBounds = false;
                        }
                    }

                    if (allowExceedWeakBounds && (dbl < dblLowerWB))
                    {
                        if (dbl < dblLowerExtLim)
                            str = strLowerExtLim;
                    }
                    else if (!allowExceedWeakBounds && (dbl < dblLowerWB))
                        str = strLowerWB;
                }

                //PHYS -> HEX -> PHYS
                QString hex = phys2hex(str, "y");
                QString phys = hex2phys(hex, "y");

                //PHYS <> OLD
                if (phys != listY.at(i))
                {
                    QStringList list = yListVal.value(i);
                    list.append(listY.at(i));
                    yListVal.insert(i, list);

                    listY[i] = phys;
                }

                //update treeView
                checkValues();
            }
        }
    }
}

void Data::setY(QStringList list)
{
    if (axisDescrY)
    {
        QString typeAxisY = axisDescrY->getPar("Attribute");
        if (typeAxisY == "COM_AXIS")
        {
            //get data corrisponding to the AXIS_PTS
            AXIS_PTS_REF *axisPtsRef = (AXIS_PTS_REF*)axisDescrY->getItem("AXIS_PTS_REF");
            QString nameAxisY = axisPtsRef->getPar("AxisPoints");
            Data *axisData;
            if (hexParent)
                axisData = hexParent->getData(nameAxisY);
            else if (srecParent)
                axisData = srecParent->getData(nameAxisY);
            else if (csvParent)
                axisData = csvParent->getData(nameAxisY);
            else if( cdfxParent)
                axisData = cdfxParent->getData(nameAxisY);
            else if( dcmParent)
                axisData = dcmParent->getData(nameAxisY);
            else
                return;

            if (axisData)
            {
                axisData->setZ(list);
            }
        }
        else if (typeAxisY == "STD_AXIS")
        {
            if (compuVTabAxisY)
            {
                //PHYS -> HEX -> PHYS
                QStringList hexList = phys2hex(list, "y");
                QStringList physList = hex2phys(hexList, "y");

                //PHYS <> OLD
                for (int i = 0; i < yCount(); i++)
                {
                    QString phys = physList.at(i);
                    if (phys != listY.at(i) && phys != "ERROR")
                    {
                        QStringList list = yListVal.value(i);
                        list.append(listY.at(i));
                        yListVal.insert(i, list);

                        listY[i] = phys;
                    }
                }

                //update treeView
                checkValues();
            }
            else
            {
                //WEAK BOUNDS
                bool bl;
                QString lower = axisDescrY->getPar("LowerLimit");
                double lowerLimit = lower.toDouble();
                QString upper = axisDescrY->getPar("UpperLimit");
                double upperLimit = upper.toDouble();

                //EXTENDED_LIMITS
                EXTENDED_LIMITS *extL = (EXTENDED_LIMITS*)axisDescrY->getItem("EXTENDED_LIMITS");
                double upperL = 0;
                double lowerL = 0;
                if (extL)
                {
                    upperL = ((QString)extL->getPar("UpperLimit")).toDouble();
                    lowerL = ((QString)extL->getPar("LowerLimit")).toDouble();
                }

                //CHECK LIMITS (weak/extended)
                QStringList limitList;
                foreach(QString str, list)
                {
                    double dbl = str.toDouble(&bl);
                    if (dbl > upperLimit)
                    {
                        if (showWeakBoundsExceeded)
                        {
                            DialogExceedWB *diag = new DialogExceedWB( &showWeakBoundsExceeded ,0);
                            int ret = diag->exec();

                            if (ret == QDialog::Accepted)
                            {
                                allowExceedWeakBounds = true;
                            }
                            else
                            {
                                allowExceedWeakBounds = false;
                            }
                        }

                        if (allowExceedWeakBounds && (dbl > upperLimit))
                        {
                            if (dbl > upperL)
                                str = QString::number(upperL, 'f');
                        }
                        else if (!allowExceedWeakBounds && (dbl > upperLimit))
                            str = QString::number(upperLimit, 'f');

                    }
                    else if(dbl < lowerLimit)
                    {
                        if (showWeakBoundsExceeded)
                        {
                            DialogExceedWB *diag = new DialogExceedWB( &showWeakBoundsExceeded ,0);
                            int ret = diag->exec();

                            if (ret == QDialog::Accepted)
                            {
                                allowExceedWeakBounds = true;
                            }
                            else
                            {
                                allowExceedWeakBounds = false;
                            }
                        }

                        if (allowExceedWeakBounds && (dbl < lowerLimit))
                        {
                            if (dbl < lowerL)
                                str = QString::number(lowerL, 'f');
                        }
                        else if (!allowExceedWeakBounds && (dbl < lowerLimit))
                            str = QString::number(lowerLimit, 'f');
                    }

                    limitList.append(str);
                }

                //PHYS -> HEX -> PHYS
                QStringList hexList = phys2hex(limitList, "y");
                QStringList physList = hex2phys(hexList, "y");

                //PHYS <> OLD
                for (int i = 0; i < yCount(); i++)
                {
                    QString phys = physList.at(i);

                    //compare with original value
                    if (phys != listY.at(i))
                    {
                        QStringList list = yListVal.value(i);
                        list.append(listY.at(i));
                        yListVal.insert(i, list);

                        listY[i] = phys;
                    }
                }

                //update treeView
                checkValues();
            }
        }
    }
}

void Data::setZ(int i, QString str)
{
    if (compuVTabAxisZ)
    {
        if (compuVTabAxisZ->getValueList().contains(str))
        {
            //PHYS -> HEX -> PHYS
            QString hex = phys2hex(str, "z");
            QString phys = hex2phys(hex, "z");

            //PHYS <> OLD
            if (phys != listZ.at(i))
            {
                QStringList list = zListVal.value(i);
                list.append(listZ.at(i));
                zListVal.insert(i, list);

                listZ[i] = phys;
            }

            //update treeView
            checkValues();
        }
    }
    else
    {
        //check weak bounds
        str = checkWeakBoundsZ(str);

        //PHYS -> HEX -> PHYS
        QString hex = phys2hex(str, "z");
        QString phys = hex2phys(hex, "z");

        qDebug() << str << hex << phys;

        //PHYS <> OLD
        if (phys != listZ.at(i))
        {
            QStringList list = zListVal.value(i);
            list.append(listZ.at(i));
            zListVal.insert(i, list);

            listZ[i] = phys;
        }

        //update treeView
        checkValues();
    }
}

void Data::setHexZ(int i, QString str)
{
    QString val = hex2phys(str, "z");
    setZ(i, val);
}

void Data::setZ(QStringList list)
{
    if (compuVTabAxisZ)
    {
        //PHYS -> HEX -> PHYS
        QStringList hexList = phys2hex(list, "z");
        QStringList physList = hex2phys(hexList, "z");

        //PHYS <> OLD
        for (int i = 0; i < zCount(); i++)
        {
            QString phys = physList.at(i);
            if (phys != listZ.at(i) && phys != "ERROR")
            {
                QStringList list = zListVal.value(i);
                list.append(listZ.at(i));
                zListVal.insert(i, list);

                listZ[i] = phys;
            }
        }

        //update treeView
        checkValues();
    }
    else
    {
        //check weak bounds
        QStringList listLim;
        foreach (QString str, list)
        {
            listLim.append(checkWeakBoundsZ(str));
        }

        //PHYS -> HEX -> PHYS
        QStringList hexList = phys2hex(listLim, "z");
        QStringList physList = hex2phys(hexList, "z");

        //PHYS <> OLD
        for (int i = 0; i < zCount(); i++)
        {
            QString phys = physList.at(i);

            //compare with original value
            if (phys != listZ.at(i))
            {
                QStringList list = zListVal.value(i);
                list.append(listZ.at(i));
                zListVal.insert(i, list);

                listZ[i] = phys;
            }
        }

        //update treeView
        checkValues();
    }

}

void Data::setZ(int row, int col, QString str)
{
    int xDim = 0;
    if (yCount() == 0)
    {
        xDim = 1;
    }
    else
    {
        xDim = xCount();
    }

    int yDim = 0;
    if (yCount() == 0)
    {
        yDim = 1;
    }
    else
    {
        yDim = yCount();
    }

    // calculate the right index into the Z QStringList
    int i = 0;
    if (isSortedByRow)
    {
         i = col + row * xDim;
    }
    else
    {
        i = row  + col * yDim;
    }

    setZ(i, str);
}

void Data::setHexZ(int row, int col, QString str)
{
    QString val = hex2phys(str, "z");
    setZ(row, col, val);
}

void Data::appendX(QString str)
{
    listX.append(str);
}

void Data::removeIndexX(int i)
{
    listX.removeAt(i);
}

void Data::appendY(QString str)
{
    listY.append(str);
}

void Data::appendZ(QString str)
{
    listZ.append(str);
}

void Data::clearX()
{
    listX.clear();
}

void Data::clearY()
{
    listY.clear();
}

void Data::clearZ()
{
    listZ.clear();
}

void Data::copyAllFrom(Data *dataSrc)
{
    //axisX : copy only if STD_AXIS
    if (isAxisXComparable && dataSrc->isAxisXComparable)
    {
        if (getAxisDescrX() && QString(getAxisDescrX()->getPar("Attribute")) == "STD_AXIS")
        {
            if (xCount() == dataSrc->xCount() && xCount() > 0)
            {
                setX(dataSrc->getX());
            }
        }
    }

    //axisY : copy only if STD_AXIS
    if (isAxisYComparable && dataSrc->isAxisYComparable)
    {
        if (getAxisDescrY() && QString(getAxisDescrY()->getPar("Attribute")) == "STD_AXIS")
        {
            if (yCount() == dataSrc->yCount() && yCount() > 0)
            {
                setY(dataSrc->getY());
            }
        }
    }

    //axisZ
    if (zCount() == dataSrc->zCount())
    {
        if (dataSrc->isSortedByRow != this->isSortedByRow && yCount() > 0)
        {
            QStringList list;
            if (dataSrc->isSortedByRow)
            {
                list = dataSrc->sortByColumn();
            }
            else
            {
                list = dataSrc->sortByRow();
            }

            setZ(list);
        }
        else
        {
            setZ(dataSrc->getZ());
        }
    }
}

QString Data::checkWeakBoundsZ(QString str)
{
    //WEAK BOUNDS
    bool bl;
    QString strLowerWB, strUpperWB;
    if (type == "CHARACTERISTIC")
    {
        strLowerWB = ((CHARACTERISTIC*)label)->getPar("LowerLimit");
        strUpperWB = ((CHARACTERISTIC*)label)->getPar("UpperLimit");
    }
    else
    {
        strLowerWB = ((AXIS_PTS*)label)->getPar("LowerLimit");
        strUpperWB = ((AXIS_PTS*)label)->getPar("UpperLimit");
    }

    double dblLowerWB = strLowerWB.toDouble();
    double dblUpperWB = strUpperWB.toDouble();

    //EXTENDED_LIMITS
    EXTENDED_LIMITS *extL = (EXTENDED_LIMITS*)label->getItem("EXTENDED_LIMITS");
    QString strLowerExtLim, strUpperExtLim;
    if (extL)
    {
        strLowerExtLim = ((QString)extL->getPar("LowerLimit"));
        strUpperExtLim = ((QString)extL->getPar("UpperLimit"));

        double dblLowerExtLim = strLowerExtLim.toDouble();
        double dblUpperExtLim = strUpperExtLim.toDouble();

        //CHECK LIMITS (weak/extended)
        double dbl = str.toDouble(&bl);
        if (dbl > dblUpperWB)
        {
            if (showWeakBoundsExceeded)
            {
                DialogExceedWB *diag = new DialogExceedWB( &showWeakBoundsExceeded ,0);
                int ret = diag->exec();

                if (ret == QDialog::Accepted)
                {
                    allowExceedWeakBounds = true;
                }
                else
                {
                    allowExceedWeakBounds = false;
                }
            }

            if (allowExceedWeakBounds && (dbl > dblUpperWB))
            {
                if (dbl > dblUpperExtLim)
                    str = strUpperExtLim;
            }
            else if (!allowExceedWeakBounds && (dbl > dblUpperWB))
                str = strUpperWB;

        }
        else if(dbl < dblLowerWB)
        {
            if (showWeakBoundsExceeded)
            {
                DialogExceedWB *diag = new DialogExceedWB( &showWeakBoundsExceeded ,0);
                int ret = diag->exec();

                if (ret == QDialog::Accepted)
                {
                    allowExceedWeakBounds = true;
                }
                else
                {
                    allowExceedWeakBounds = false;
                }
            }

            if (allowExceedWeakBounds && (dbl < dblLowerWB))
            {
                if (dbl < dblLowerExtLim)
                    str = strLowerExtLim;
            }
            else if (!allowExceedWeakBounds && (dbl < dblLowerWB))
                str = strLowerWB;
        }
    }
    else
    {
        //CHECK LIMITS (weak/extended)
        double dbl = str.toDouble(&bl);
        if (dbl > dblUpperWB)
        {
            str = strUpperWB;
        }
        else if (dbl < dblLowerWB)
        {
            str = strLowerWB;
        }

    }

    return str;

}

QString Data::checkLimitsX(QString str)
{

    //EXTENDED_LIMITS
    EXTENDED_LIMITS *extL = (EXTENDED_LIMITS*)axisDescrX->getItem("EXTENDED_LIMITS");
    QString strUpperExtLim, strLowerExtLim;
    if (extL)
    {
        strUpperExtLim = ((QString)extL->getPar("UpperLimit"));
        strLowerExtLim = ((QString)extL->getPar("LowerLimit"));
        double dblUpperExtLim = strUpperExtLim.toDouble();
        double dblLowerExtLim = strLowerExtLim.toDouble();

        //CHECK LIMITS
        double dbl = str.toDouble();
        if (dbl > dblUpperExtLim)
            str = strUpperExtLim;

        if (dbl < dblLowerExtLim)
            str = strLowerExtLim;
    }
    else //WEAK BOUNDS
    {
        QString strLowerWB = axisDescrX->getPar("LowerLimit");
        double dblLowerWB = strLowerWB.toDouble();
        QString strUpperWB = axisDescrX->getPar("UpperLimit");
        double dblUpperWB = strUpperWB.toDouble();

        //CHECK LIMITS (weak/extended)
        double dbl = str.toDouble();
        if (dbl > dblUpperWB)
            str = QString::number(dblUpperWB);

        if (dbl < dblLowerWB)
            str = QString::number(dblLowerWB);

    }

    return str;
}

QString Data::checkLimitsY(QString str)
{
    //EXTENDED_LIMITS
    EXTENDED_LIMITS *extL = (EXTENDED_LIMITS*)axisDescrY->getItem("EXTENDED_LIMITS");
    QString strUpperExtLim, strLowerExtLim;
    if (extL)
    {
        strUpperExtLim = ((QString)extL->getPar("UpperLimit"));
        strLowerExtLim = ((QString)extL->getPar("LowerLimit"));
        double dblUpperExtLim = strUpperExtLim.toDouble();
        double dblLowerExtLim = strLowerExtLim.toDouble();

        //CHECK LIMITS
        double dbl = str.toDouble();
        if (dbl > dblUpperExtLim)
            str = strUpperExtLim;

        if (dbl < dblLowerExtLim)
            str = strLowerExtLim;
    }
    else //WEAK BOUNDS
    {
        QString strLowerWB = axisDescrY->getPar("LowerLimit");
        double dblLowerWB = strLowerWB.toDouble();
        QString strUpperWB = axisDescrY->getPar("UpperLimit");
        double dblUpperWB = strUpperWB.toDouble();

        //CHECK LIMITS (weak/extended)
        double dbl = str.toDouble();
        if (dbl > dblUpperWB)
            str = QString::number(dblUpperWB);

        if (dbl < dblLowerWB)
            str = QString::number(dblLowerWB);

    }

    return str;

}

QString Data::checkLimitsZ(QString str)
{
    //WEAK BOUNDS
    bool bl;
    QString strLowerWB, strUpperWB;
    if (type == "CHARACTERISTIC")
    {
        strLowerWB = ((CHARACTERISTIC*)label)->getPar("LowerLimit");
        strUpperWB = ((CHARACTERISTIC*)label)->getPar("UpperLimit");
    }
    else
    {
        strLowerWB = ((AXIS_PTS*)label)->getPar("LowerLimit");
        strUpperWB = ((AXIS_PTS*)label)->getPar("UpperLimit");
    }

    double dblLowerWB = strLowerWB.toDouble();
    double dblUpperWB = strUpperWB.toDouble();

    //EXTENDED_LIMITS
    EXTENDED_LIMITS *extL = (EXTENDED_LIMITS*)label->getItem("EXTENDED_LIMITS");
    QString strLowerExtLim, strUpperExtLim;
    if (extL) //CHECK that str is inside EXTENDED LIMITS
    {
        strLowerExtLim = ((QString)extL->getPar("LowerLimit"));
        strUpperExtLim = ((QString)extL->getPar("UpperLimit"));

        double dblLowerExtLim = strLowerExtLim.toDouble();
        double dblUpperExtLim = strUpperExtLim.toDouble();


        double dbl = str.toDouble(&bl);

        if (dbl > dblUpperExtLim)
            str = strUpperExtLim;

        if (dbl < dblLowerExtLim)
            str = strLowerExtLim;

    }
    else //CHECK that str is inside WEAK LIMITS
    {
        double dbl = str.toDouble(&bl);
        if (dbl > dblUpperWB)
        {
            str = strUpperWB;
        }
        else if (dbl < dblLowerWB)
        {
            str = strLowerWB;
        }

    }

    return str;
}

// --- reset VALUES ---  //

void Data::resetValX(int i)
{
    if (axisDescrX)
    {
        QString typeAxisX = axisDescrX->getPar("Attribute");
        if (typeAxisX == "COM_AXIS")
        {
            //get data corrisponding to the AXIS_PTS
            AXIS_PTS_REF *axisPtsRef = (AXIS_PTS_REF*)axisDescrX->getItem("AXIS_PTS_REF");
            QString nameAxisX = axisPtsRef->getPar("AxisPoints");

            if (hexParent)
            {
                Data *axisData = hexParent->getData(nameAxisX);
                if (axisData)
                {
                    axisData->resetValZ(i);
                }
            }
            else if (srecParent)
            {
                Data *axisData = srecParent->getData(nameAxisX);
                if (axisData)
                {
                    axisData->resetValZ(i);
                }
            }
            else if (csvParent)
            {
                Data *axisData = csvParent->getData(nameAxisX);
                if (axisData)
                {
                    axisData->resetValZ(i);
                }
            }
            else if (cdfxParent)
            {
                Data *axisData = cdfxParent->getData(nameAxisX);
                if (axisData)
                {
                    axisData->resetValZ(i);
                }
            }
            else if (dcmParent)
            {
                Data *axisData = dcmParent->getData(nameAxisX);
                if (axisData)
                {
                    axisData->resetValZ(i);
                }
            }
        }
        else if (xListVal.contains(i))
    {
        setX(i,  xListVal.value(i).first());
        checkValues();
    }
    }
}

void Data::resetValY(int i)
{
    if (axisDescrY)
    {
        QString typeAxisY = axisDescrX->getPar("Attribute");
        if (typeAxisY == "COM_AXIS")
        {
            //get data corrisponding to the AXIS_PTS
            AXIS_PTS_REF *axisPtsRef = (AXIS_PTS_REF*)axisDescrY->getItem("AXIS_PTS_REF");
            QString nameAxisY = axisPtsRef->getPar("AxisPoints");

            if (hexParent)
            {
                Data *axisData = hexParent->getData(nameAxisY);
                if (axisData)
                {
                    axisData->resetValZ(i);
                }
            }
            else if (srecParent)
            {
                Data *axisData = srecParent->getData(nameAxisY);
                if (axisData)
                {
                    axisData->resetValZ(i);
                }
            }
            else if (csvParent)
            {
                Data *axisData = csvParent->getData(nameAxisY);
                if (axisData)
                {
                    axisData->resetValZ(i);
                }
            }
            else if (cdfxParent)
            {
                Data *axisData = cdfxParent->getData(nameAxisY);
                if (axisData)
                {
                    axisData->resetValZ(i);
                }
            }
            else if (dcmParent)
            {
                Data *axisData = dcmParent->getData(nameAxisY);
                if (axisData)
                {
                    axisData->resetValZ(i);
                }
            }
        }
        else  if (yListVal.contains(i))
    {
        setY(i,  yListVal.value(i).first());
        checkValues();
    }
    }
}

void Data::resetValZ(int i)
{
    if (zListVal.contains(i))
    {
        setZ(i,  zListVal.value(i).first());
        checkValues();
    }
}

void Data::resetValZ(int row, int col)
{
    int i = 0;
    if (isSortedByRow)
    {
         i = col + row * xCount();
    }
    else
    {
        i = row  + col * yCount();
    }

    resetValZ(i);
}

void Data::resetAll()
{
    for (int i = 0; i < listX.count(); i++)
    {
        resetValX(i);
    }

    for (int i = 0; i < listY.count(); i++)
    {
        resetValY(i);
    }

    for (int i = 0; i < listZ.count(); i++)
    {
        resetValZ(i);
    }
}

// --- undo VALUES ---  //

void Data::undoValX(int i)
{
    if (axisDescrX)
    {
        QString typeAxisX = axisDescrX->getPar("Attribute");
        if (typeAxisX == "COM_AXIS")
        {
            //get data corrisponding to the AXIS_PTS
            AXIS_PTS_REF *axisPtsRef = (AXIS_PTS_REF*)axisDescrX->getItem("AXIS_PTS_REF");
            QString nameAxisX = axisPtsRef->getPar("AxisPoints");

            if (hexParent)
            {
                Data *axisData = hexParent->getData(nameAxisX);
                if (axisData)
                {
                    axisData->undoValZ(i);
                }
            }
            else if (srecParent)
            {
                Data *axisData = srecParent->getData(nameAxisX);
                if (axisData)
                {
                    axisData->undoValZ(i);
                }
            }
            else if (csvParent)
            {
                Data *axisData = csvParent->getData(nameAxisX);
                if (axisData)
                {
                    axisData->undoValZ(i);
                }
            }
            else if (cdfxParent)
            {
                Data *axisData = cdfxParent->getData(nameAxisX);
                if (axisData)
                {
                    axisData->undoValZ(i);
                }
            }
            else if (dcmParent)
            {
                Data *axisData = dcmParent->getData(nameAxisX);
                if (axisData)
                {
                    axisData->undoValZ(i);
                }
            }
        }
        else if (xListVal.contains(i))
        {
            //get the changes list
            QStringList list = xListVal.value(i);

            //set data to the last changes
            setX(i,  list.last());

            //remove the last from list
            list.removeLast();
            if (list.isEmpty())
            {
                xListVal.remove(i);
            }
            else
            {
                xListVal.insert(i, list);
            }

            //check display
            checkValues();
        }
    }
}

void Data::undoValY(int i)
{
    if (axisDescrY)
    {
        QString typeAxisY = axisDescrX->getPar("Attribute");
        if (typeAxisY == "COM_AXIS")
        {
            //get data corrisponding to the AXIS_PTS
            AXIS_PTS_REF *axisPtsRef = (AXIS_PTS_REF*)axisDescrY->getItem("AXIS_PTS_REF");
            QString nameAxisY = axisPtsRef->getPar("AxisPoints");

            if (hexParent)
            {
                Data *axisData = hexParent->getData(nameAxisY);
                if (axisData)
                {
                    axisData->undoValZ(i);
                }
            }
            else if (srecParent)
            {
                Data *axisData = hexParent->getData(nameAxisY);
                if (axisData)
                {
                    axisData->undoValZ(i);
                }
            }
            else if (csvParent)
            {
                Data *axisData = csvParent->getData(nameAxisY);
                if (axisData)
                {
                    axisData->undoValZ(i);
                }
            }
            else if (cdfxParent)
            {
                Data *axisData = cdfxParent->getData(nameAxisY);
                if (axisData)
                {
                    axisData->undoValZ(i);
                }
            }
            else if (dcmParent)
            {
                Data *axisData = dcmParent->getData(nameAxisY);
                if (axisData)
                {
                    axisData->undoValZ(i);
                }
            }
        }
        else if (yListVal.contains(i))
    {
        //get the changes list
        QStringList list = yListVal.value(i);

        //set data to the last changes
        setY(i,  list.last());

        //remove the last from list
        list.removeLast();
        if (list.isEmpty())
        {
            yListVal.remove(i);
        }
        else
        {
            yListVal.insert(i, list);
        }

        //check display
        checkValues();
    }
    }
}

void Data::undoValZ(int i)
{
    if (zListVal.contains(i))
    {
        //get the changes list
        QStringList list = zListVal.value(i);

        //set data to the last changes
        setZ(i,  list.last());

        //remove the last from list
        list.removeLast();
        if (list.isEmpty())
        {
            zListVal.remove(i);
        }
        else
        {
            zListVal.insert(i, list);
        }

        //check display
        checkValues();
    }
}

void Data::undoValZ(int row, int col)
{
    int i = 0;
    if (isSortedByRow)
    {
         i = col + row * xCount();
    }
    else
    {
        i = row  + col * yCount();
    }

    undoValZ(i);
}

// --- redo VALUES ---  //



// --- others ---  //
QString Data::getnPtsXHexa()
{
    QString str = "";
    QString datatype = getNPtsXDatatype();
    str = dec2hex(xCount(), datatype.toStdString(), 16);
    return str;
}

QString Data::getnPtsYHexa()
{
    QString str = "";
    QString datatype = getNPtsYDatatype();
    str = dec2hex(yCount(), datatype.toStdString(), 16);
    return str;
}

QString Data::getNPtsXDatatype()
{
    QString deposit = ((CHARACTERISTIC*)label)->getPar("Deposit");
    RECORD_LAYOUT* record_layout = (RECORD_LAYOUT*)project->getNode("MODULE/" + moduleName + "/RECORD_LAYOUT/" + deposit);

    QString datatype = "";
    foreach (Item *item, record_layout->optItems)
    {
        QString type = item->name;
        if (type == "NO_AXIS_PTS_X")
        {
            datatype = ((NO_AXIS_PTS_X*)item)->getPar("Datatype");
        }
    }
    return datatype;
}

QString Data::getNPtsYDatatype()
{
    QString deposit = ((CHARACTERISTIC*)label)->getPar("Deposit");
    RECORD_LAYOUT* record_layout = (RECORD_LAYOUT*)project->getNode("MODULE/" + moduleName + "/RECORD_LAYOUT/" + deposit);

    QString datatype = "";
    foreach (Item *item, record_layout->optItems)
    {
        QString type = item->name;
        if (type == "NO_AXIS_PTS_Y")
        {
            datatype = ((NO_AXIS_PTS_X*)item)->getPar("Datatype");
        }
    }
    return datatype;
}

bool Data::isSizeChanged()
{
    return sizeChanged;
}

void Data::extendSize(int newDimX, int newDimY)
{
    int diffX = newDimX - xCount();
    int diffY = newDimY - yCount();
    bool xSizeChanged = false;
    bool ySizeChanged = false;

    //update axisX
    if (axisDescrX)
    {
        QString typeAxisX = axisDescrX->getPar("Attribute");
        if (typeAxisX == "STD_AXIS")
        {
            if (newDimX <= QString(axisDescrX->fixPar("MaxAxisPoints").c_str()).toInt()
                     && newDimX > 0)
            {
                //reduce axis X size
                if (newDimX < xCount())
                {
                    for (int i = xCount() - 1; i >= newDimX; i--)
                    {

                        //save the value
                        QStringList list = xListVal.value(i);
                        list.append(listX.at(i));
                        xListVal.insert(i, list);

                        // remove value
                        listX.removeAt(i);
                    }


                }
                //increase axis X size
                else if (newDimX > xCount())
                {
                    for (int i = 0; i < diffX; i++)
                    {
                        listX.append(listX.at(xCount() - 1));
                    }
                }

                // set xSizeChanged
                xSizeChanged = true;
            }
        }
    }


    //update axisY
    if (axisDescrY)
    {
        QString typeAxisY = axisDescrY->getPar("Attribute");
        if (typeAxisY == "STD_AXIS")
        {
            if (newDimY <= QString(axisDescrY->fixPar("MaxAxisPoints").c_str()).toInt()
                    &&  newDimY > 0)
            {
                //reduce axis Y size
                if (newDimY < yCount())
                {
                    for (int i = yCount() - 1; i >= newDimY; i--)
                    {
                        //save the value
                        QStringList list = yListVal.value(i);
                        list.append(listY.at(i));
                        yListVal.insert(i, list);

                        //remove values
                        listY.removeAt(i);
                    }
                }
                //increase axis Y size
                else if (newDimY > yCount())
                {
                    for (int i = 0; i < diffY; i++)
                    {
                        listY.append(listY.at(yCount() - 1));
                    }
                }

                // set ySizeChanged
                ySizeChanged = true;
            }
        }
    }

    //update Z values
    if (xSizeChanged || ySizeChanged)
    {
        if (newDimY <= 0)
            newDimY = 1;
        int newDimZ = newDimX * newDimY;
        int diffZ = newDimZ - zCount();
        if (newDimZ < zCount())
        {
            for (int i = zCount() - 1; i >= newDimZ; i--)
            {
                // save values
                QStringList list = zListVal.value(i);
                list.append(listZ.at(i));
                zListVal.insert(i, list);

                //remove
                listZ.removeAt(i);
            }

        }
        else if (newDimZ > zCount())
        {
            for (int i = 0; i < diffZ; i++)
            {
                listZ.append(listZ.at(zCount() - 1));
            }
        }

        if (xOrgSize != newDimX || yOrgSize != newDimY)
            sizeChanged = true;
        else
            sizeChanged = false;
    }

    updateSize();
    checkValues();
}

QDomElement Data::writeValue2Node(QDomDocument &doc)
{
    QDomElement swInstance = doc.createElement("SW-INSTANCE");

    // node SHORT-NAME
    QDomElement shortName = doc.createElement("SHORT-NAME");
    QDomText text = doc.createTextNode(this->name);
    shortName.appendChild(text);
    swInstance.appendChild(shortName);

    // node CATEGORY
    QDomElement category = doc.createElement("CATEGORY");
    QString cat = type;
    if (cat == "AXIS_PTS")
        cat = "COM_AXIS";
    text = doc.createTextNode(cat);
    category.appendChild(text);
    swInstance.appendChild(category);

    // node SW-FEATURE-REF
    QDomElement swFeatureRef = doc.createElement("SW-FEATURE-REF");
    text = doc.createTextNode(this->getSubset());
    swFeatureRef.appendChild(text);
    swInstance.appendChild(swFeatureRef);

    // node SW-VALUE-CONT
    QDomElement swValueCont = doc.createElement("SW-VALUE-CONT");
    swInstance.appendChild(swValueCont);

    // node SW-VALUE-CONT\UNIT-DISPLAY-NAME
    QDomElement unitDisplayName = doc.createElement("UNIT-DISPLAY-NAME");
    QString unit = getUnit();
    if (unit.startsWith('\"'))
        unit.remove(0, 1);
    if (unit.endsWith('\"'))
        unit.chop(1);

    //added for \u00B0 unicode for �  when unit is "�C"
    QChar *data = unit.data();
    QString str;
    while (!data->isNull())
    {
        str.append(data->toLatin1());
        ++data;
    }

    text = doc.createTextNode(str);
    unitDisplayName.appendChild(text);
    swValueCont.appendChild(unitDisplayName);



    // node SW-VALUE-CONT\SW-VALUES-PHYS
    QDomElement swValuePhys = doc.createElement("SW-VALUES-PHYS");
    if (type == "VALUE" || type == "CURVE" || type == "AXIS_PTS")
    {
        // SW-VALUE-CONT\SW-VALUES-PHYS
        if (getCompuTabAxisZ())
        {
            foreach (QString str, listZ)
            {
                // node SW-VALUE-CONT\SW-VALUES-PHYS\VT
                QDomElement value = doc.createElement("VT");
                text = doc.createTextNode(str.remove("\""));
                value.appendChild(text);
                swValuePhys.appendChild(value);
            }
        }
        else
        {
            foreach (QString str, listZ)
            {
                // node SW-VALUE-CONT\SW-VALUES-PHYS\V
                QDomElement value = doc.createElement("V");
                text = doc.createTextNode(str);
                value.appendChild(text);
                swValuePhys.appendChild(value);
            }
        }
    }
    else if (type == "VAL_BLK")
    {
        // node SW-VALUE-CONT\SW-ARRAYSIZE
        QDomElement swArraySize = doc.createElement("SW-ARRAYSIZE");
        swValueCont.appendChild(swArraySize);

        // node SW-VALUE-CONT\SW-ARRAYSIZE\V
        QDomElement v = doc.createElement("V");
        text = doc.createTextNode(QString::number(xCount()));
        v.appendChild(text);
        swArraySize.appendChild(v);

        // SW-VALUE-CONT\SW-VALUES-PHYS
        if (getCompuTabAxisZ())
        {
            foreach (QString str, listZ)
            {
                // node SW-VALUE-CONT\SW-VALUES-PHYS\VT
                QDomElement value = doc.createElement("VT");
                text = doc.createTextNode(str.remove("\""));
                value.appendChild(text);
                swValuePhys.appendChild(value);
            }
        }
        else
        {
            foreach (QString str, listZ)
            {
                // node SW-VALUE-CONT\SW-VALUES-PHYS\V
                QDomElement value = doc.createElement("V");
                text = doc.createTextNode(str);
                value.appendChild(text);
                swValuePhys.appendChild(value);
            }
        }

    }
    else if (type == "ASCII")
    {
        // node SW-VALUE-CONT\SW-VALUES-PHYS\VT
        QDomElement value = doc.createElement("VT");

        // text
        QString str = QString("");
        double c;
        for (int i = 0; i < xCount(); i++)
        {
            QString val = getZ(i);
            c = val.toDouble();
            if (32 <= c && c < 127)
                //str.append((unsigned char)c);
                str.append((char)c);
        }

        text = doc.createTextNode(str);

        // create a text node
        value.appendChild(text);
        swValuePhys.appendChild(value);
    }
    else if (type == "MAP")
    {
        // SW-VALUE-CONT\SW-VALUES-PHYS
        for (int i = 0; i < yCount(); i++)
        {
            // node SW-VALUE-CONT\SW-VALUES-PHYS\VG
            QDomElement vg = doc.createElement("VG");
            swValuePhys.appendChild(vg);

            // node SW-VALUE-CONT\SW-VALUES-PHYS\VG\LABEL
            QDomElement label = doc.createElement("LABEL");
            text = doc.createTextNode(listY.at(i));
            label.appendChild(text);
            vg.appendChild(label);

            // check if tab_verb or rat_fun
            if (getCompuTabAxisZ())
            {
                for (int j = 0; j < xCount(); j++)
                {
                    // node SW-VALUE-CONT\SW-VALUES-PHYS\VG\VT
                    QDomElement value = doc.createElement("VT");
                    text = doc.createTextNode(getZ(i, j).remove("\""));
                    value.appendChild(text);
                    vg.appendChild(value);
                }
            }
            else
            {
                for (int j = 0; j < xCount(); j++)
                {
                    // node SW-VALUE-CONT\SW-VALUES-PHYS\VG\V
                    QDomElement value = doc.createElement("V");
                    text = doc.createTextNode(getZ(i, j));
                    value.appendChild(text);
                    vg.appendChild(value);
                }
            }
        }
    }
    swValueCont.appendChild(swValuePhys);

    // node SW-AXIS-CONTS
    QDomElement swAxisConts = doc.createElement("SW-AXIS-CONTS");
    bool append = false;
    if (axisDescrX)
    {
        // node SW-AXIS-CONTS
        append = true;

        // node SW-AXIS-CONTS\SW-AXIS-CONT
        QDomElement swAxisContX = doc.createElement("SW-AXIS-CONT");
        swAxisConts.appendChild(swAxisContX);

        if (QString(axisDescrX->getPar("Attribute")) == "STD_AXIS")
        {
            // node SW-AXIS-CONTS\SW-AXIS-CONT\CATEGORY
            QDomElement category = doc.createElement("CATEGORY");
            text = doc.createTextNode("STD_AXIS");
            category.appendChild(text);
            swAxisContX.appendChild(category);

            // node SW-AXIS-CONTS\SW-AXIS-CONT\UNIT-DISPLAY-NAME
            QDomElement unitDisplayName = doc.createElement("UNIT-DISPLAY-NAME");
            QString unitX;
            COMPU_METHOD *cpmX = getCompuMethodAxisX();
            if (cpmX)
                unitX = QString(cpmX->getPar("Unit"));
            else
                unitX = "-";
            text = doc.createTextNode(unitX);
            unitDisplayName.appendChild(text);
            swAxisContX.appendChild(unitDisplayName);

            // node SW-AXIS-CONTS\SW-AXIS-CONT\SW-VALUES-PHYS
            QDomElement swValuePhys = doc.createElement("SW-VALUES-PHYS");
            swAxisContX.appendChild(swValuePhys);

            // check if tab_verb or rat_fun
            if (getCompuTabAxisX())
            {
                foreach (QString str, listX)
                {
                    // node  SW-AXIS-CONTS\SW-AXIS-CONT\SW-VALUES-PHYS\VT
                    QDomElement value = doc.createElement("VT");
                    text = doc.createTextNode(str.remove("\""));
                    value.appendChild(text);
                    swValuePhys.appendChild(value);
                }
            }
            else
            {
                foreach (QString str, listX)
                {
                    // node  SW-AXIS-CONTS\SW-AXIS-CONT\SW-VALUES-PHYS\V
                    QDomElement value = doc.createElement("V");
                    text = doc.createTextNode(str);
                    value.appendChild(text);
                    swValuePhys.appendChild(value);
                }
            }
        }
        else if (QString(axisDescrX->getPar("Attribute")) == "COM_AXIS")
        {
            // node SW-AXIS-CONTS\SW-AXIS-CONT\CATEGORY
            QDomElement category = doc.createElement("CATEGORY");
            text = doc.createTextNode("COM_AXIS");
            category.appendChild(text);
            swAxisContX.appendChild(category);

            // node SW-AXIS-CONTS\SW-AXIS-CONT\SW-INSTANCE-REF
            QDomElement swInstanceRef = doc.createElement("SW-INSTANCE-REF");
            QString nameAxisX = "";
            AXIS_PTS_REF *axisPtsRef = (AXIS_PTS_REF*)axisDescrX->getItem("AXIS_PTS_REF");
            if (axisPtsRef)
            {
                nameAxisX = axisPtsRef->getPar("AxisPoints");
            }
            text = doc.createTextNode(nameAxisX);
            swInstanceRef.appendChild(text);
            swAxisContX.appendChild(swInstanceRef);
        }
    }
    if (axisDescrY)
    {
        // node SW-AXIS-CONTS
        append = true;

        // node SW-AXIS-CONTS\SW-AXIS-CONT
        QDomElement swAxisContY = doc.createElement("SW-AXIS-CONT");
        swAxisConts.appendChild(swAxisContY);

        if (QString(axisDescrY->getPar("Attribute")) == "STD_AXIS")
        {
            // node SW-AXIS-CONTS\SW-AXIS-CONT\CATEGORY
            QDomElement category = doc.createElement("CATEGORY");
            text = doc.createTextNode("STD_AXIS");
            category.appendChild(text);
            swAxisContY.appendChild(category);

            // node SW-AXIS-CONTS\SW-AXIS-CONT\UNIT-DISPLAY-NAME
            QDomElement unitDisplayName = doc.createElement("UNIT-DISPLAY-NAME");
            QString unitY;
            COMPU_METHOD *cpmY = getCompuMethodAxisY();
            if (cpmY)
                unitY = QString(cpmY->getPar("Unit"));
            else
                unitY = "-";
            text = doc.createTextNode(unitY);
            unitDisplayName.appendChild(text);
            swAxisContY.appendChild(unitDisplayName);

            // node SW-AXIS-CONTS\SW-AXIS-CONT\SW-VALUES-PHYS
            QDomElement swValuePhys = doc.createElement("SW-VALUES-PHYS");
            swAxisContY.appendChild(swValuePhys);

            // check if tab_verb or rat_fun
            if (getCompuTabAxisY())
            {
                foreach (QString str, listY)
                {
                    // node  SW-AXIS-CONTS\SW-AXIS-CONT\SW-VALUES-PHYS\VT
                    QDomElement value = doc.createElement("VT");
                    text = doc.createTextNode(str.remove("\""));
                    value.appendChild(text);
                    swValuePhys.appendChild(value);
                }
            }
            else
            {
                foreach (QString str, listY)
                {
                    // node  SW-AXIS-CONTS\SW-AXIS-CONT\SW-VALUES-PHYS\V
                    QDomElement value = doc.createElement("V");
                    text = doc.createTextNode(str);
                    value.appendChild(text);
                    swValuePhys.appendChild(value);
                }
            }
        }
        else if (QString(axisDescrY->getPar("Attribute")) == "COM_AXIS")
        {
            // node SW-AXIS-CONTS\SW-AXIS-CONT\CATEGORY
            QDomElement category = doc.createElement("CATEGORY");
            text = doc.createTextNode("COM_AXIS");
            category.appendChild(text);
            swAxisContY.appendChild(category);

            // node SW-AXIS-CONTS\SW-AXIS-CONT\SW-INSTANCE-REF
            QDomElement swInstanceRef = doc.createElement("SW-INSTANCE-REF");
            QString nameAxisY = "";
            AXIS_PTS_REF *axisPtsRef = (AXIS_PTS_REF*)axisDescrY->getItem("AXIS_PTS_REF");
            if (axisPtsRef)
            {
                nameAxisY = axisPtsRef->getPar("AxisPoints");
            }
            text = doc.createTextNode(nameAxisY);
            swInstanceRef.appendChild(text);
            swAxisContY.appendChild(swInstanceRef);
        }
    }
    if (append) swInstance.appendChild(swAxisConts);

    return swInstance;
}

void Data::writeValue2Csv(FUNCTION *subset, QTextStream &out)
{
    //write data name
    out << ";" << QString(name) << "\r\n";
    out << type.toLocal8Bit().data();
    if (type.toLower() == "map")
    {
        out << ";\r\n\r\n";
    }
    else if (type.toLower() == "curve")
    {
        out << "\r\n";
        //out << ";\r\n"; //as in CVX_SPEC.DOC V2.1
    }

    //write z values
    if (type.toLower() == "value" || type.toLower() == "curve"
        || type.toLower() == "axis_pts" || type.toLower() == "val_blk")
    {
        out << ";";
        foreach (QString str, listZ)
        {
            if (!compuVTabAxisZ)
                out << ";" << str;
            else
                out << ";" << "\"" << str << "\"";
        }
        out << "\r\n";
    }
    else if (type.toLower() == "map")
    {
        int dimX = listX.count();
        int dimY = listY.count();

        if (!isSortedByRow)
        {
            for (int j = 0; j < dimY; j++)
            {
                out << ";";
                for (int i = 0; i < dimX; i++)
                {
                    if (!compuVTabAxisZ)
                       out << ";" << listZ.at(i * dimY + j);
                    else
                       out << ";" << "\"" << listZ.at(i * dimY + j) << "\"";

                }
                 out << "\r\n";
            }
        }
        else
        {
            for (int i = 0; i < dimY; i++)
            {
                out << ";";
                for (int j = 0; j < dimX; j++)
                {
                    if (!compuVTabAxisZ)
                       out << ";" << listZ.at(i * dimX + j);
                    else
                       out << ";" << "\"" << listZ.at(i * dimX + j) << "\"";
                }
                 out << "\r\n";
            }
        }
    }
    else if (type.toLower() == "ascii")
    {
        out << ";";
        QString str = "";
        double c;
        for (int i = 0; i < xCount(); i++)
        {
            QString val = getZ(i);
            c = val.toDouble();
            if (32 <= c && c < 127)
                //str.append((unsigned char)c);
                str.append((char)c);
        }
        out << ";\"" << str << "\"";
        out << "\r\n";
    }
    out << "\r\n";

    //write function name
    if (subset)
    {
        out << "FUNCTION;;";
        //out << "FUNCTION;"; //as in CVX_SPEC.DOC V2.1
        out << "\"" << subset->name  << "\"";
        out << ";" << subset->getPar("LongIdentifier") << "\r\n\r\n";
    }
    else
    {
        out << "\r\n";
    }

    //write x values
    if (axisDescrX)
    {
        QString attributeX = axisDescrX->getPar("Attribute");
        if ( listX.count() > 0 && attributeX.toLower() == "std_axis")
        {
            out << ";" << QString(name) << "\r\n";
            out << "X_AXIS_PTS";

            out << ";";
            foreach (QString str, listX)
            {
                if (!compuVTabAxisX)
                    out << ";" << str;
                else
                    out << ";" << "\"" << str << "\"";
            }
            out << "\r\n\r\n";
        }
    }

    //wrize y values
    if (axisDescrY)
    {
        QString attributeY = axisDescrX->getPar("Attribute");
        if ( listY.count() > 0 && attributeY.toLower() == "std_axis")
        {
            out << ";" << QString(name) << "\r\n";
            out << "Y_AXIS_PTS";

            out << ";";
            foreach (QString str, listY)
            {
                if (!compuVTabAxisY)
                    out << ";" << str;
                else
                    out << ";" << "\"" << str << "\"";
            }
            out << "\r\n\r\n";
        }
    }


}

void Data::writeValue2Dcm(FUNCTION *subset, QTextStream &out)
{
    //write data name
    if (type.toLower() == "value")
    {
        out << "FESTWERT " << QString(name) << "\r\n";
        out << "   LANGNAME " << this->getComment() << "\r\n";
        if (subset)
            out << "   FUNKTION " << subset->name << "\r\n";
        else
            out << "   FUNKTION " << "" << "\r\n";
        out << "   EINHEIT_W " << this->getUnit() << "\r\n";
        if (this->getCompuTabAxisZ() != nullptr)
            out << "   TEXT   \"" << this->getZ(0) << "\"\r\n";
        else
            out << "   WERT   " << this->getZ(0) << "\r\n";
        out << "END\r\n\r\n";
    }
}

void Data::plot(QWidget *parent)
{
    if (type != "VAL_BLK")
    {
        int dimX = listX.count();

        if (dimX > 0)
        {
            Graph *plot = new Graph(parent, this);
            plot->resize(600, 400);
            plot->show();
        }
    }
    else
    {
        QMessageBox::information(parent, "HEXplorer :: plot",
                                 QString(this->name) + " is a VAL_BLK and cannot be plotted.");
    }
}

void Data::updateSize()
{
    if (listX.isEmpty())
        size = 4;
    else if (listY.isEmpty())
        size = 5;
    else
        size = 4 + listY.count();
}

double Data::interp2D(double x, double y)
{
    if (yCount() == 0)
    {
        return 0;
    }

    double z = 0;

    //search index of x
    int x1 = -1;
    int x2 = -1;
    int i = 0;
    while (getX(i).toDouble() < x && i < xCount() - 1)
    {
        i++;
    }
    if (getX(i).toDouble() == x || i == 0)
        x1 = x2 = i;
    else
    {
        if (getX(i).toDouble() > x)
        {
            x1 = i - 1;
            x2 = i;
        }
        else
            x1 = x2 = xCount() - 1;

    }

    //create Z1 and Z2 curves
    double *Xn1 = new double[yCount()];
    double *Xn2 = new double[yCount()];
    if (isSortedByRow)
    {
        for (int i = 0; i < yCount(); i++)
        {
            Xn1[i] = getZ(x1 + i * xCount()).toDouble();
            Xn2[i] = getZ(x2 + i * xCount()).toDouble();
        }
    }
    else
    {
        for (int i = 0; i < yCount(); i++)
        {
            Xn1[i] = getZ(i  + x1 * yCount()).toDouble();
            Xn2[i] = getZ(i  + x2 * yCount()).toDouble();
        }
    }

    //interpolate in X direction
    double *Zn = new double[yCount()];
    double X1 = getX(x1).toDouble();
    double X2 = getX(x2).toDouble();
    for (int i = 0; i < yCount();i++)
    {
        if (X1 == X2)
        {
           Zn[i] = Xn1[i];
        }
        else
        {
            Zn[i] = (x - X1) * (Xn2[i] - Xn1[i])/ (X2 - X1) + Xn1[i];
        }
    }

    //search index of Y
    int y1 = -1;
    int y2 = -1;
    i = 0;
    while (getY(i).toDouble() < y && i < yCount() - 1)
    {
        i++;
    }
    if (getY(i).toDouble() == y || i == 0)
        y1 = y2 = i;
    else
    {
        if (getY(i).toDouble() > y)
        {
            y1 = i - 1;
            y2 = i;
        }
        else
            y1 = y2 = yCount() - 1;

    }

    //interpolate in Y direction
    double Y1 = getY(y1).toDouble();
    double Y2 = getY(y2).toDouble();
    if (Y1 == Y2)
    {
        z = Y1;
    }
    else
    {
        z = (y - Y1) * (Zn[y2] - Zn[y1])/ (Y2 - Y1) + Zn[y1];
    }

    return z;
}

void Data::clearOldValues()
{
    xListVal.clear();
    yListVal.clear();
    zListVal.clear();
    displayed = false;
}

bool Data::checkAxisXMonotony()
{
    QStringList listValuesX;
    if (axisDescrX)
    {
        QString typeAxisX = axisDescrX->getPar("Attribute");
        if (typeAxisX == "COM_AXIS")
        {
            //get data corrisponding to the AXIS_PTS
            AXIS_PTS_REF *axisPtsRef = (AXIS_PTS_REF*)axisDescrX->getItem("AXIS_PTS_REF");
            QString nameAxisX = axisPtsRef->getPar("AxisPoints");

            Data *axisData = 0;
            if (hexParent)
            {
                axisData = hexParent->getData(nameAxisX);
            }
            else if (csvParent)
            {
                axisData = csvParent->getData(nameAxisX);
            }
            else if (cdfxParent)
            {
                axisData = cdfxParent->getData(nameAxisX);
            }
            else if (dcmParent)
            {
                axisData = dcmParent->getData(nameAxisX);
            }

            if (axisData)
            {
                listValuesX = axisData->getZ();
            }

        }
        else
        {
            listValuesX = listX;
        }
    }

    if (listValuesX.count() > 0)
    {
        for (int i = 0; i < xCount() - 1; i++)
        {
            bool bl1 = false;
            double dbl1 = listValuesX.at(i).toDouble(&bl1);
            bool bl2 = false;
            double dbl2 = listValuesX.at(i + 1).toDouble(&bl2);

            if (dbl1 >= dbl2 && bl1 && bl2)
            {
                return false;
            }
        }
    }
    return true;
}

bool Data::checkAxisYMonotony()
{
    QStringList listValuesY;
    if (axisDescrY)
    {
        QString typeAxisY = axisDescrY->getPar("Attribute");
        if (typeAxisY == "COM_AXIS")
        {
            //get data corrisponding to the AXIS_PTS
            AXIS_PTS_REF *axisPtsRef = (AXIS_PTS_REF*)axisDescrY->getItem("AXIS_PTS_REF");
            QString nameAxisY = axisPtsRef->getPar("AxisPoints");

            Data *axisData = 0;
            if (hexParent)
            {
                axisData = hexParent->getData(nameAxisY);
            }
            else if (csvParent)
            {
                axisData = csvParent->getData(nameAxisY);
            }
            else if (cdfxParent)
            {
                axisData = cdfxParent->getData(nameAxisY);
            }
            else if (dcmParent)
            {
                axisData = dcmParent->getData(nameAxisY);
            }

            if (axisData)
            {
                listValuesY = axisData->getZ();
            }

        }
        else
        {
            listValuesY = listY;
        }
    }

    if (listValuesY.count() > 0)
    {
        for (int i = 0; i < yCount() - 1; i++)
        {
            bool bl1 = false;
            double dbl1 = listValuesY.at(i).toDouble(&bl1);
            bool bl2 = false;
            double dbl2 = listValuesY.at(i + 1).toDouble(&bl2);

            if (dbl1 >= dbl2 && bl1 && bl2)
            {
                return false;
            }
        }
    }

    return true;
}

bool Data::checkAxisZMonotony()
{
    return true;
}

HexFile *Data::getHexParent()
{
    return hexParent;
}

SrecFile *Data::getSrecParent()
{
    return srecParent;
}

Csv *Data::getCsvParent()
{
    return csvParent;
}

CdfxFile *Data::getCdfxParent()
{
    return cdfxParent;
}

Dcm *Data::getDcmParent()
{
    return dcmParent;
}

bool Data::isModified()
{
    return displayed;
}

QStringList Data::sortByRow()
{
    QStringList list;
    if (!isSortedByRow)
    {
        for (int i = 0; i < yCount();i++)
        {
            for (int j = 0; j < xCount();j++)
            {
                list.append(getZ(i,j));
            }

        }
    }

    return list;
}

QStringList Data::sortByColumn()
{
    QStringList list;
    if (isSortedByRow)
    {
        for (int j = 0; j < xCount();j++)
        {
            for (int i = 0; i < yCount();i++)
            {
                list.append(getZ(i,j));
            }
        }
    }

    return list;
}

Node *Data::getLabel()
{
    return label;
}

PROJECT *Data::getProject()
{
    return project;
}

unsigned int Data::tzn(unsigned int v)
{
    unsigned int c;     // c will be the number of zero bits on the right,
                    // so if v is 1101000 (base 2), then c will be 3
    // NOTE: if 0 == v, then c = 31.
    if (v & 0x1)
    {
      // special case for odd v (assumed to happen half of the time)
      c = 0;
    }
    else
    {
      c = 1;
      if ((v & 0xffff) == 0)
      {
        v >>= 16;
        c += 16;
      }
      if ((v & 0xff) == 0)
      {
        v >>= 8;
        c += 8;
      }
      if ((v & 0xf) == 0)
      {
        v >>= 4;
        c += 4;
      }
      if ((v & 0x3) == 0)
      {
        v >>= 2;
        c += 2;
      }
      c -= v & 0x1;
    }

    return c;
}

int Data::getZnbyte_Recordlayout(RECORD_LAYOUT *rcd_lyt)
{
   bool bl;
   if (strcmp(datatypeZ.c_str(), "SBYTE") == 0 || strcmp(datatypeZ.c_str(), "UBYTE") == 0 )
   {
       ALIGNMENT_BYTE *item = (ALIGNMENT_BYTE*)rcd_lyt->getItem("alignment_byte");
       if (item)
       {
           QString str = item->getPar("AlignmentBorder");
           return str.toInt(&bl,10);
       }
   }
   else if (strcmp(datatypeZ.c_str(), "SWORD") == 0 || strcmp(datatypeZ.c_str(), "UWORD") == 0 )
   {
       ALIGNMENT_WORD *item = (ALIGNMENT_WORD*)rcd_lyt->getItem("alignment_word");
       if (item)
       {
           QString str = item->getPar("AlignmentBorder");
           return str.toInt(&bl,10);
       }
   }
   else if (strcmp(datatypeZ.c_str(), "SLONG") == 0 || strcmp(datatypeZ.c_str(), "ULONG") == 0 )
   {
       ALIGNMENT_LONG *item = (ALIGNMENT_LONG*)rcd_lyt->getItem("alignment_long");
       if (item)
       {
           QString str = item->getPar("AlignmentBorder");
           return str.toInt(&bl,10);
       }
   }
   else if (strcmp(datatypeZ.c_str(), "FLOAT32_IEEE") == 0)
   {
       ALIGNMENT_FLOAT32_IEEE *item = (ALIGNMENT_FLOAT32_IEEE*)rcd_lyt->getItem("alignment_float32_ieee");
       if (item)
       {
           QString str = item->getPar("AlignmentBorder");
           return str.toInt(&bl,10);
       }
   }
   else if (strcmp(datatypeZ.c_str(), "FLOAT64_IEEE") == 0)
   {
       ALIGNMENT_FLOAT64_IEEE *item = (ALIGNMENT_FLOAT64_IEEE*)rcd_lyt->getItem("alignment_float64_ieee");
       if (item)
       {
           QString str = item->getPar("AlignmentBorder");
           return str.toInt(&bl,10);
       }
   }

   return 0;
}
