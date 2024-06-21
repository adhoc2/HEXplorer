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

#include "Nodes/fix_axis_par_list.h"
#include <QMessageBox>
#include "a2lgrammar.h"


//initialise static variables
Factory<Node,FIX_AXIS_PAR_LIST> FIX_AXIS_PAR_LIST::nodeFactory;

// Functions (Predicate)
bool nodeLessThan( const Node *a, const Node *b );
bool itemLessThan( const Item *a, const Item *b );

FIX_AXIS_PAR_LIST::FIX_AXIS_PAR_LIST( Node *parentNode)
    : Node(parentNode, parentNode->lex, parentNode->errorList)
{
    // specific for this node Characteristic
    axis_values = new QList<QString>;

    //get grammar
    A2lGrammar* gram = parentNode->lex->grammar;
    namePar = &gram->fix_axis_par_list.namePar;
    typePar = &gram->fix_axis_par_list.typePar;
    factoryOptNode = &gram->fix_axis_par_list.factoryOptNode;
    factoryOptItem = &gram->fix_axis_par_list.factoryOptItem;

    //opt Parameters
    occOptPar = new QHash<QString, Occurence>;
    //occOptPar->insert("FORMAT", ZeroOrOne);


    //Set the line where the Node starts in ASAP file
    this->a2lLine = lex->getLine();

    //Parse Mandatory PARAMETERS
    parseFixPar(typePar);
    if (parameters.count() > 0)
        name = parameters.at(0);
    else
        name = (QString)"FIX_AXIS_PAR_LIST";

    //Parse optional PARAMETERS
    TokenTyp token = parseOptPar(occOptPar);

    //Parse list of axis_values
    while (token == Integer)
    {
        axis_values->append(lex->getLexem());
        token = nextToken();
    }


    //End
    if (token == BlockEnd)
    {
        token = nextToken();
        if (token == Keyword && lex->getLexem() == "FIX_AXIS_PAR_LIST")
        {
            //Sort the childNodes
            //std::sort(this->childNodes.begin(), this->childNodes.end(), nodeLessThan);

            //Sort The childItems
            //std::sort(this->optItems.begin(), this->optItems.end(), itemLessThan);
        }
        else
        {
            QString s(lex->toString(token));
            this->showError("expected token : BlockEnd AXIS_PTS\nfind token : " + s );
        }
    }
    else
    {
        QString s1(lex->toString(token));
        QString s2(lex->getLexem());
        this->showError("expected end FIX_AXIS_PAR_LIST\nfind : " + s1 + " " + s2 );
    }
    this->sortChildrensName();
}

FIX_AXIS_PAR_LIST::~FIX_AXIS_PAR_LIST()
{
    
    {
        
    };
    delete occOptPar;
}

void FIX_AXIS_PAR_LIST::parseFixPar(QList<TokenTyp> *typePar)
{
    //Mandatory PARAMETERS
    TokenTyp token;
    for (int i = 0; i < typePar->count(); i++)
    {
        token = this->nextToken();
        if (token == typePar->at(i))
        {

            parameters.append(lex->getLexem());
        }
        else if(typePar->at(i) == Float && token == Integer)
        {
            parameters.append(lex->getLexem());
        }
        else
        {
            QString t(lex->toString(typePar->at(i)));
            QString s(lex->toString(token));
            this->showError("expected token : " + t +"\nfind token : " + s );
        }
    }
}

TokenTyp FIX_AXIS_PAR_LIST::parseOptPar(QHash<QString, Occurence> *nameOptPar)
{

    if (nameOptPar->isEmpty())
        return nextToken();
    else
    {
        TokenTyp token = nextToken();
        while (token == BlockBegin || token == Keyword)
        {
            //Nodes
            if (token == BlockBegin)
            {
                token = this->nextToken();
                if (token == Keyword)
                {
                    QString lexem = lex->getLexem();
                    if (factoryOptNode->contains(lexem))
                    {
                        if (this->occOptPar->value(lexem) == ZeroOrOne)
                        {
                            this->occOptPar->insert(lexem, Zero);
                            Node  *instance = factoryOptNode->value(lexem)->createInstance( this);
                            this->addChildNode(instance);
                            token = nextToken();
                        }
                        else if (this->occOptPar->value(lexem) == ZeroOrMore)
                        {
                            Node  *instance = factoryOptNode->value(lexem)->createInstance( this);
                            this->addChildNode(instance);
                            token = nextToken();
                        }
                        else
                        {
                            QString s(lexem);
                            this->showError(" Keyword : " + s + " can only be once declared");
                            return token;
                        }
                    }
                    else
                    {
                        QString s(lexem);
                        this->showError("unknown Keyword : " + s );
                        return token;
                    }
                }
                else
                {
                    QString s(lex->toString(token));
                    this->showError("expected token : BlockBegin or Keyword\nfind token : " + s );
                    return token;
                }
            }
            //Items
            else if (token == Keyword)
            {
                QString lexem = lex->getLexem();
                if (factoryOptItem->contains(lexem))
                {
                    if (this->occOptPar->value(lexem) == ZeroOrOne)
                    {
                        this->occOptPar->insert(lexem, Zero);
                        Item  *instance = factoryOptItem->value(lexem)->createInstance( this);
                        this->addOptItem(instance);
                        token = nextToken();
                    }
                    else if (this->occOptPar->value(lexem) == ZeroOrMore)
                    {
                        Item  *instance = factoryOptItem->value(lexem)->createInstance( this);
                        this->addOptItem(instance);
                        token = nextToken();
                    }
                    else
                    {
                        QString s(lexem);
                        this->showError(" Keyword : " + s + " can only be once declared");
                        return token;
                    }
                }
                else
                {
                    QString s(lexem);
                    this->showError("unknown Keyword : " + s );
                    return token;
                }
            }
        }
        return token;
    }
}

QMap<QString, QString> *FIX_AXIS_PAR_LIST::getParameters()
{
    QMap<QString, QString> *par = new QMap<QString, QString>;
    for (int i = 0; i < namePar->count(); i++)
    {
        par->insert(namePar->at(i), parameters.at(i));
    }
    return par;
}

QString FIX_AXIS_PAR_LIST::getPar(QString str)
{
    int i = namePar->indexOf(str);
    return parameters.at(i);
}

// specific for this node Charactetistic
QList<QString>*  FIX_AXIS_PAR_LIST::getAxisValues()
{

    return axis_values;
}


