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

#include "Nodes/characteristic.h"
#include <QMessageBox>
#include <QHash>
#include "a2lgrammar.h"


//initialise static variables
Factory<Node,CHARACTERISTIC> CHARACTERISTIC::nodeFactory;

// Functions (Predicate)
bool nodeLessThan( const Node *a, const Node *b );
bool itemLessThan( const Item *a, const Item *b );

CHARACTERISTIC::CHARACTERISTIC(Node *parentNode)
    : Node(parentNode, parentNode->lex, parentNode->errorList)
{
    // specific for this node Characteristic
    subset = NULL;

    //get grammar
    A2lGrammar* gram = parentNode->lex->grammar;
    namePar = &gram->characteristic.namePar;
    typePar = &gram->characteristic.typePar;
    factoryOptNode = &gram->characteristic.factoryOptNode;
    factoryOptItem = &gram->characteristic.factoryOptItem;

    //Set the line where the Node starts in ASAP file
    a2lLine = lex->getLine();

    //Parse Mandatory PARAMETERS
    parseFixPar(typePar);
    if (parameters.count() > 0)
        name = parameters.at(0);
    else
        name = (QString)"characteristic";

    //Parse optional PARAMETERS
    TokenTyp token = parseOptPar();

    //fianlize parsing
    if (token == BlockEnd)
    {
        token = nextToken();
        if (token == Keyword && lex->getLexem() == "CHARACTERISTIC")
        {
            //Sort the childNodes
            //foreach (Node *node, childNodes)
                //node->sortChildrensName();
        }
        else
        {
            QString s(lex->toString(token));
            this->showError("expected token : BlockEnd CHARACTERISTIC\nfind token : " + s);
        }
    }
    else
    {
        QString s1(lex->toString(token));
        QString s2(lex->getLexem());
        this->showError("expected end CHARACTERISTIC\nfind : " + s1 + " " + s2);
    }

    this->sortChildrensName();
}

CHARACTERISTIC::~CHARACTERISTIC()
{
    
    {
        
    }
}

void CHARACTERISTIC::parseFixPar(QList<TokenTyp> *typePar)
{
    //Mandatory PARAMETERS
    TokenTyp token;
    for (int i = 0; i < typePar->count(); i++)
    {
        //int line = lex->getLine();

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
            this->showError("expected token : " + t +"\nfind token : " + s);
        }
    }
}

TokenTyp CHARACTERISTIC::parseOptPar()
{
    //opt parameters
//    QMap<QString, Occurence> nameOptPar;
     QHash<QString, Occurence> nameOptPar;
     nameOptPar.insert("FORMAT", ZeroOrOne);
     nameOptPar.insert("IF_DATA", ZeroOrMore);
     nameOptPar.insert("EXTENDED_LIMITS", ZeroOrOne);
     nameOptPar.insert("MAX_REFRESH", ZeroOrOne);
     nameOptPar.insert("NUMBER", ZeroOrOne);
     nameOptPar.insert("READ_ONLY", ZeroOrOne);
     nameOptPar.insert("ANNOTATION", ZeroOrMore);
     nameOptPar.insert("AXIS_DESCR", ZeroOrMore);
     nameOptPar.insert("DISPLAY_IDENTIFIER",ZeroOrOne);
     nameOptPar.insert("BIT_MASK",ZeroOrOne);
     nameOptPar.insert("FUNCTION_LIST",ZeroOrOne);
     nameOptPar.insert("ECU_ADDRESS_EXTENSION", ZeroOrOne);
     nameOptPar.insert("MATRIX_DIM", ZeroOrOne);
     nameOptPar.insert("BYTE_ORDER", ZeroOrOne);

    if (nameOptPar.isEmpty())
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
                    if (nameOptPar.contains(lexem))
                    {
                        if (nameOptPar.value(lexem) == ZeroOrOne)
                        {
                           nameOptPar.insert(lexem, Zero);
                           Node  *instance = factoryOptNode->value(lexem)->createInstance(this);
                           this->addChildNode(instance);
                           token = nextToken();
                        }
                        else if (nameOptPar.value(lexem) == ZeroOrMore)
                        {
                            Node *instance = 0;
                            if (lexem == "AXIS_DESCR")
                            {
                                if (!isChild("AXIS_DESCR"))
                                {
                                    Node *node = new Node(this, lex, errorList);
                                    node->name = (QString)"AXIS_DESCR";
                                    addChildNode(node);
                                    node->_pixmap = "";
                                }
                                instance = factoryOptNode->value(lexem)->createInstance( child("AXIS_DESCR", false));
                                child("AXIS_DESCR", false)->addChildNode(instance);
                            }
                            else
                            {
                                instance = factoryOptNode->value(lexem)->createInstance( this);
                                this->addChildNode(instance);
                            }
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
                if (nameOptPar.contains(lexem))
                {
                    if (nameOptPar.value(lexem) == ZeroOrOne)
                    {
                        nameOptPar.insert(lexem, Zero);
                        Item  *instance = factoryOptItem->value(lexem)->createInstance( this);
                        this->addOptItem(instance);
                        token = nextToken();
                    }
                    else if (nameOptPar.value(lexem) == ZeroOrMore)
                    {
                        Item  *instance = factoryOptItem->value(lexem)->createInstance( this);
                        this->addOptItem(instance);
                        token = nextToken();
                    }
                    else
                    {
                        QString s(lexem);
                        this->showError(" Keyword : " + s + " can only be declared once");
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

QMap<QString, QString> *CHARACTERISTIC::getParameters()
{
    QMap<QString, QString> *par = new QMap<QString, QString>;
        
    for (int i = 0; i < namePar->count(); i++)
    {
        par->insert(namePar->at(i), parameters.at(i));
    }        

    return par;
}

QMap<QString, QString> *CHARACTERISTIC::getOptItems()
{
   QMap<QString, QString> *par = new QMap<QString, QString>;

    for (int i = 0; i < optItems.count(); i++)
    {
        Item* item = optItems.at(i);
        QMap<QString, QString> map = item->getParameters();
        for (int i = 0; i < map.count(); i++)
        {
            par->insert(map.keys().at(i), map.values().at(i));
        }
    }

    return par;

}


QString CHARACTERISTIC::getPar(QString str)
{
    int i = namePar->indexOf(str);    
    return parameters.at(i);
}

QString CHARACTERISTIC::getPar(int i)
{
    return parameters.at(i);
}

QString CHARACTERISTIC::pixmap()
{
    return ":/icones/CHAR.bmp";
}

// specific only for this node Charactetistic
QString CHARACTERISTIC::getSubsetName()
{
    if (subset)
    {
        return subset->name;
    }
    else
    {
        return "";
    }
}

void CHARACTERISTIC::setSubset(FUNCTION *fun)
{
    subset = fun;
}


