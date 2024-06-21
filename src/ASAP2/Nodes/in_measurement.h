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

#ifndef IN_MEASUREMENT_H
#define IN_MEASUREMENT_H

#include "node.h"
#include "item.h"
#include "lexer.h"
#include "basefactory.h"


class IN_MEASUREMENT : public Node
{
    public:
        IN_MEASUREMENT( Node *parentNode);
        ~IN_MEASUREMENT();

        QMap<QString, QString> *getParameters();
        static Factory<Node,IN_MEASUREMENT> nodeFactory;
        QString pixmap();
        QStringList getCharList();
        QString getPar(QString str);

    private:
        // Fix parameters
        QList<TokenTyp> *typePar;
        QList<QString> *namePar;
        QList<QString> parameters;

        // Opt parameters
        QMap<QString, FactoryPlant<Node> *>  *factoryOptNode;
        QMap<QString, FactoryPlant<Item> *>  *factoryOptItem;

        // CHARACTERISTICS list
        QList<QString> charList;

        TokenTyp parseListChar();
        void parseFixPar(QList<TokenTyp> *typePar);
        TokenTyp parseOptPar();
};

#endif // IN_MEASUREMENT_H
