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

#include "Nodes/compu_vtab.h"
#include "lexer.h"
#include "a2lgrammar.h"

//initialise static variables
Factory<Node,COMPU_VTAB> COMPU_VTAB::nodeFactory;

// Functions (Predicate)
bool nodeLessThan( const Node *a, const Node *b );
bool itemLessThan( const Item *a, const Item *b );

COMPU_VTAB::COMPU_VTAB(Node *parentNode)
    : Node(parentNode, parentNode->lex, parentNode->errorList)
{
    //get grammar
    A2lGrammar* gram = parentNode->lex->grammar;
    namePar = &gram->compuVtab.namePar;
    typePar = &gram->compuVtab.typePar;
    factoryOptNode = &gram->compuVtab.factoryOptNode;
    factoryOptItem = &gram->compuVtab.factoryOptItem;

    //Set the line where the Node starts in ASAP file
    a2lLine = lex->getLine();

    //Parse Mandatory PARAMETERS
    parseFixPar(typePar);
    if (parameters.count() > 0)
        name = parameters.at(0);
    else
        name = (QString)"compu_vtab";

    //special for compu_vtab
    parsePairs();

    //Parse optional PARAMETERS
    TokenTyp token = parseOptPar();

    //fianlize parsing
    if (token == BlockEnd)
    {
        token = nextToken();
        if (token == Keyword && lex->getLexem() == "COMPU_VTAB")
        {
            //Sort the childNodes
            //std::sort(this->childNodes.begin(), this->childNodes.end(), nodeLessThan);

            //Sort The childItems
            //std::sort(this->optItems.begin(), this->optItems.end(), itemLessThan);
        }
        else
        {
            QString s(lex->toString(token));
            this->showError("expected token : BlockEnd COMPU_VTAB\nfind token : " + s);
        }
    }
    else
    {
        QString s1(lex->toString(token));
        QString s2(lex->getLexem());
        this->showError("expected end COMPU_VTAB\nfind : " + s1 + " " + s2);
    }
}

COMPU_VTAB::~COMPU_VTAB()
{
    
    {
        
    }
}

void COMPU_VTAB::parseFixPar(QList<TokenTyp> *typePar)
{
    //Mandatory PARAMETERS
    TokenTyp token;
    for (int i = 0; i < typePar->count(); i++)
    {
        token = this->nextToken();
        if (token == typePar->at(i))
        {
            //parameters.insert(namePar->at(i), lex->getLexem());
            
            
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

TokenTyp COMPU_VTAB::parseOptPar()
{
    //opt parameters
    QMap<QString, Occurence> nameOptPar;
    nameOptPar.insert("DEFAULT_VALUE", ZeroOrOne);

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

QMap<QString, QString> *COMPU_VTAB::getParameters()
{
    QMap<QString, QString> *par = new QMap<QString, QString>;
    for (int i = 0; i < namePar->count(); i++)
    {
        par->insert(namePar->at(i), parameters.at(i));
    }

    for (int i = 0; i < valuePairs.count(); i++)
    {
        QString str;
        str.setNum(valuePairs.keys().at(i));
		par->insert(str.toLocal8Bit().data(), valuePairs.values().at(i));
    }
    return par;
}

QString  COMPU_VTAB::pixmap()
{
    return ":/icones/CHAR.bmp";
}

void COMPU_VTAB::parsePairs()
{
    QString str = parameters.at(3);
    bool bl = false;
    int count = str.toInt(&bl, 10);

    TokenTyp token;
    int key = 0;
    for (int i = 0; i < count; i++)
    {
        token = lex->getNextToken();
        if (token == Integer || token == Float)
        {
           QString str = lex->getLexem();
           key = str.toInt(&bl,10);
           listKeyPairs.append(key);
           token = lex->getNextToken();
           if (token == String)
           {
               QString value = lex->getLexem();
               value.remove("\"");
              //valuePairs.insert(key, lex->getLexem());
              //listValuePairs.append(lex->getLexem());
               valuePairs.insert(key, value);
               listValuePairs.append(value);
           }
           else
           {
                QString s(lex->toString(token));
                showError("expected token : String \nfind token : " + s);
           }
        }
        else
        {
            QString s(lex->toString(token));
            showError("expected token : Integer \nfind token : " + s);
        }
    }
}

QString COMPU_VTAB::getValue(int i)
{
    int ind = listKeyPairs.indexOf(i);
    if (ind >= 0)
        return listValuePairs.at(ind);
    else
        return "ERROR";

}

int COMPU_VTAB::getPos(QString str)
{
   int ind =  listValuePairs.indexOf(str.toLocal8Bit().data());
   if (ind >= 0)
       return listKeyPairs.at(ind);
   else
       return -1;
}

QString COMPU_VTAB::getPar(QString str)
{
    int i = namePar->indexOf(str);
    return parameters.at(i);
}

QStringList COMPU_VTAB::getValueList()
{
    QStringList list;
    foreach (QString str, listValuePairs)
    {
        list.append(str);
    }

    return list;
}
