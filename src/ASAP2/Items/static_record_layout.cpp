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

#include "static_record_layout.h"
#include <QMessageBox>
#include "a2lgrammar.h"

//initialise static variables
Factory<Item,STATIC_RECORD_LAYOUT> STATIC_RECORD_LAYOUT::itemFactory;

STATIC_RECORD_LAYOUT::STATIC_RECORD_LAYOUT( Node *parentNode)  : Item(parentNode)
{
    //get grammar
    A2lGrammar* gram = parentNode->lex->grammar;
    namePar = &gram->static_record_layout.namePar;
    typePar = &gram->static_record_layout.typePar;

    //Parse Mandatory PARAMETERS
    //parseFixPar(typePar, namePar ,in, parentWidget);
    //name = parameters->value("STATIC_RECORD_LAYOUT");
    name = (char*)"";
}

STATIC_RECORD_LAYOUT::~STATIC_RECORD_LAYOUT()
{
    foreach (char* ptr, parameters)
    {
        delete[] ptr;
    }
}

QMap<std::string, std::string> STATIC_RECORD_LAYOUT::getParameters()
{    
    QMap<std::string, std::string> par;
    for (int i = 0; i < namePar->count(); i++)
    {
        par.insert(namePar->at(i), parameters.at(i));
    }
    return par;
}

char* STATIC_RECORD_LAYOUT::getPar(std::string str)
{
    int i = namePar->indexOf(str);
    return parameters.at(i);
}
