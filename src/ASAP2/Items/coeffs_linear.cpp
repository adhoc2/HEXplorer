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

#include "coeffs_linear.h"
#include <QMessageBox>
#include "a2lgrammar.h"

//initialise static variables
Factory<Item,COEFFS_LINEAR> COEFFS_LINEAR::itemFactory;

COEFFS_LINEAR::COEFFS_LINEAR( Node *parentNode)  : Item(parentNode)
{
    //get grammar
    A2lGrammar* gram = parentNode->lex->grammar;
    namePar = &gram->coeffs_linear.namePar;
    typePar = &gram->coeffs_linear.typePar;

    //Parse Mandatory PARAMETERS
    parseFixPar(typePar);
    name = (char*)"COEFFS_LINEAR";
}

COEFFS_LINEAR::~COEFFS_LINEAR()
{
    foreach (char* ptr, parameters)
    {
        delete[] ptr;
    }
}


QMap<std::string, std::string> COEFFS_LINEAR::getParameters()
{
    QMap<std::string, std::string> par;
    for (int i = 0; i < namePar->count(); i++)
    {
        par.insert(namePar->at(i), parameters.at(i));
    }
    return par;
}

char* COEFFS_LINEAR::getPar(std::string str)
{
    int i = namePar->indexOf(str);
    return parameters.at(i);
}
