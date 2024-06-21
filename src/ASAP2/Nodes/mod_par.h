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

#ifndef MOD_PAR_H
#define MOD_PAR_H

#include "node.h"
#include "item.h"
#include "lexer.h"
#include "basefactory.h"
#include "Nodes/memory_segment.h"
#include "Nodes/memory_layout.h"
#include "Nodes/calibration_method.h"
#include "Items/version.h"
#include "Items/addr_epk.h"
#include "Items/epk.h"
#include "Items/customer.h"
#include "Items/customer_no.h"
#include "Items/user.h"
#include "Items/phone_no.h"
#include "Items/ecu.h"
#include "Items/cpu_type.h"
#include "Items/system_constant.h"
#include "Items/supplier.h"

class MOD_PAR : public Node
{
    public:
        MOD_PAR( Node *parentNode);
        ~MOD_PAR();
        QMap<QString, QString> *getParameters();
        static Factory<Node, MOD_PAR> nodeFactory;
        QString getPar(QString str);

    private:
        // Fix parameters
        QList<TokenTyp> *typePar;
        QList<QString> *namePar;
        QList<QString> parameters;

        // Opt parameters
        QMap<QString, Occurence> *occOptPar;
        QMap<QString, FactoryPlant<Node> *>  *factoryOptNode;
        QMap<QString, FactoryPlant<Item> *>  *factoryOptItem;

        void parseFixPar(QList<TokenTyp> *typePar);
        TokenTyp parseOptPar(QMap<QString, Occurence> *nameOptPar);
};

#endif // MOD_PAR_H
