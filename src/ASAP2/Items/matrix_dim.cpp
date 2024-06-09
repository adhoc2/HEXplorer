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

#include "matrix_dim.h"
#include <QMessageBox>
#include "a2lgrammar.h"

//initialise static variables
Factory<Item,MATRIX_DIM> MATRIX_DIM::itemFactory;

MATRIX_DIM::MATRIX_DIM( Node *parentNode) : Item(parentNode)
{
    //get grammar
    A2lGrammar* gram = parentNode->lex->grammar;
    namePar = &gram->matrix_dim.namePar;
    typePar = &gram->matrix_dim.typePar;

    //Parse Mandatory PARAMETERS
    parseFixPar(typePar);
    name = (char*)"MATRIX_DIM";
}


MATRIX_DIM::~MATRIX_DIM()
{
    foreach (char* ptr, parameters)
    {
        delete[] ptr;
    }
}


QMap<std::string, std::string> MATRIX_DIM::getParameters()
{
    QMap<std::string, std::string> par;
    for (int i = 0; i < namePar->count(); i++)
    {
        par.insert(namePar->at(i), parameters.at(i));
    }
    return par;
}

char* MATRIX_DIM::getPar(std::string str)
{
    int i = namePar->indexOf(str);
    return parameters.at(i);
}
