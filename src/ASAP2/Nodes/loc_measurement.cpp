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

#include "Nodes/loc_measurement.h"
#include <QMessageBox>
#include <typeinfo>
#include <QHash>
#include "a2lgrammar.h"

//initialise static variables
Factory<Node,LOC_MEASUREMENT> LOC_MEASUREMENT::nodeFactory;

// Functions (Predicate)
bool nodeLessThan( const Node *a, const Node *b );
bool itemLessThan( const Item *a, const Item *b );

LOC_MEASUREMENT::LOC_MEASUREMENT( Node *parentNode)
    : Node(parentNode, parentNode->lex, parentNode->errorList)
{
    //Set the line where the Node starts in ASAP file
    a2lLine = lex->getLine();

    //Parse Mandatory PARAMETERS
    //parseFixPar(typePar, namePar ,in);
    name = (QString)"LOC_MEASUREMENT";

    //Parse optional PARAMETERS
    //TokenTyp token = parseOptPar(in);

    TokenTyp token = parseListChar();

    //fianlize parsing
    if (token == BlockEnd)
    {
        token = nextToken();
        if (token == Keyword && lex->getLexem() == "LOC_MEASUREMENT")
        {
            //Sort the childNodes
            //std::sort(this->childNodes.begin(), this->childNodes.end(), nodeLessThan);

            //Sort The childItems
            //std::sort(this->optItems.begin(), this->optItems.end(), itemLessThan);

            //sort the charList
            std::sort(charList.begin(), charList.end());
        }
        else
        {
            QString s(lex->toString(token));
            this->showError("expected token : BlockEnd LOC_MEASUREMENT\nfind token : " + s);
        }
    }
    else
    {
        QString s1(lex->toString(token));
        QString s2(lex->getLexem());
        this->showError("expected end LOC_MEASUREMENT\nfind : " + s1 + " " + s2);
    }
}

LOC_MEASUREMENT::~LOC_MEASUREMENT()
{
    
    {
        
    }
}

QMap<QString, QString> *LOC_MEASUREMENT::getParameters()
{
    QMap<QString, QString> *par = new QMap<QString, QString>;
    for (int i = 0; i < namePar->count(); i++)
    {
        par->insert(namePar->at(i), parameters.at(i));
    }
    return par;
}

QString  LOC_MEASUREMENT::pixmap()
{
    return ":/icones/CHAR.bmp";
}

TokenTyp LOC_MEASUREMENT::parseListChar()
{
    TokenTyp token = lex->getNextToken();

    while (token == Identifier)
    {
        charList.append(lex->getLexem());
        token = lex->getNextToken();
    }

    return token;
}

QStringList LOC_MEASUREMENT::getCharList()
{
    QStringList list;
    foreach(QString str, charList)
    {
        list.append(str);
    }
    return list;
}

QString LOC_MEASUREMENT::getPar(QString str)
{
    int i = namePar->indexOf(str);
    return parameters.at(i);
}
