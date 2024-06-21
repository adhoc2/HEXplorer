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

#ifndef FIX_AXIS_PAR_LIST_H
#define FIX_AXIS_PAR_LIST_H

#include "node.h"
#include "item.h"
#include "lexer.h"
#include "basefactory.h"


class FIX_AXIS_PAR_LIST : public Node
{
    public:
        FIX_AXIS_PAR_LIST(Node *parentNode);
        ~FIX_AXIS_PAR_LIST();
        QMap<QString, QString> *getParameters();
        static Factory<Node, FIX_AXIS_PAR_LIST> nodeFactory;
        QString getPar(QString str);
        QList<QString> *getAxisValues();


    private:
        // specific for this node Characteristic
        QList<QString> *axis_values;

        // Fix parameters
        QList<TokenTyp> *typePar;
        QList<QString> *namePar;
        QList<QString> parameters;
        void parseFixPar(QList<TokenTyp> *typePar);

        // Opt parameters
        QHash<QString, Occurence> *occOptPar;
        QMap<QString, FactoryPlant<Node> *>  *factoryOptNode;
        QMap<QString, FactoryPlant<Item> *>  *factoryOptItem;
        TokenTyp parseOptPar(QHash<QString, Occurence> *nameOptPar);
};

#endif // FIX_AXIS_PAR_LIST_H
