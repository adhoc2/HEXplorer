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

#ifndef AXIS_DESCR_H
#define AXIS_DESCR_H

#include "node.h"
#include "item.h"
#include "lexer.h"
#include "basefactory.h"
#include "Nodes/fix_axis_par_list.h"
#include "Items/format.h"
#include "Items/extended_limits.h"
#include "Items/deposit.h"
#include "Items/axis_pts_ref.h"
#include "Items/fix_axis_par.h"
#include "Items/fix_axis_par_dist.h"

class AXIS_DESCR : public Node
{
    public:
        AXIS_DESCR(Node *parentNode);
        ~AXIS_DESCR();
        QMap<QString, QString> *getParameters();
        static Factory<Node, AXIS_DESCR> nodeFactory;
        QString getPar(QString str);

    private:
        // Fix parameters
        QList<TokenTyp> *typePar;
        QList<QString> *namePar;
        QList<QString> parameters;

        // Opt parameters
        QHash<QString, Occurence> *occOptPar;
        QMap<QString, FactoryPlant<Node> *>  *factoryOptNode;
        QMap<QString, FactoryPlant<Item> *>  *factoryOptItem;

        void parseFixPar(QList<TokenTyp> *typePar);
        TokenTyp parseOptPar(QHash<QString, Occurence> *nameOptPar);
};

#endif // CHARACTERISTIC_H
