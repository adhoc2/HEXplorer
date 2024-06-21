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

#include "default_value.h"
#include <QMessageBox>
#include "a2lgrammar.h"

//initialise static variables
Factory<Item,DEFAULT_VALUE> DEFAULT_VALUE::itemFactory;

DEFAULT_VALUE::DEFAULT_VALUE(Node *parentNode)  : Item(parentNode)
{
    //get grammar
    A2lGrammar* gram = parentNode->lex->grammar;
    namePar = &gram->default_value.namePar;
    typePar = &gram->default_value.typePar;

    //Parse Mandatory PARAMETERS
    parseFixPar(typePar);
    if (parameters.count() > 0)
        name = parameters.at(0);
    else
        name = (QString)"default_value";
}

DEFAULT_VALUE::~DEFAULT_VALUE()
{
    
    {
        
    }
}


QMap<QString, QString> DEFAULT_VALUE::getParameters()
{
    QMap<QString, QString> par;
    for (int i = 0; i < namePar->count(); i++)
    {
        par.insert(namePar->at(i), parameters.at(i));
    }
    return par;
}

QString DEFAULT_VALUE::getPar(QString str)
{
    int i = namePar->indexOf(str);
    return parameters.at(i);
}
