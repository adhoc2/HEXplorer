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

#include "Nodes/annotation_text.h"
#include "characteristic.h"
#include <QMessageBox>
#include "a2lgrammar.h"


//initialise static variables
Factory<Node,ANNOTATION_TEXT> ANNOTATION_TEXT::nodeFactory;

// Functions (Predicate)
bool nodeLessThan( const Node *a, const Node *b );
bool itemLessThan( const Item *a, const Item *b );

ANNOTATION_TEXT::ANNOTATION_TEXT( Node *parentNode )
    : Node(parentNode, parentNode->lex, parentNode->errorList)
{
    //get grammar
    A2lGrammar* gram = parentNode->lex->grammar;
    namePar = &gram->annotation_text.namePar;
    typePar = &gram->annotation_text.typePar;
    factoryOptNode = &gram->annotation_text.factoryOptNode;
    factoryOptItem = &gram->annotation_text.factoryOptItem;

    //Set the line where the Node starts in ASAP file
    this->a2lLine = lex->getLine();

    //Parse Mandatory PARAMETERS
    //parseFixPar(typePar ,in);
    name = (QString)"ANNOTATION_TEXT";

    TokenTyp token = parseListString();

    //End
    if (token == BlockEnd)
    {
        token = nextToken();
        if (token == Keyword && lex->getLexem() == "ANNOTATION_TEXT")
        {
        }
        else
        {
            QString s(lex->toString(token));
            this->showError("expected token : BlockEnd ANNOTATION_TEXT\nfind token : " + s );
        }
    }
    else
    {
        QString s1(lex->toString(token));
        QString s2(lex->getLexem());
        this->showError("expected end ANNOTATION_TEXT\nfind : " + s1 + " " + s2 );
    }
}

ANNOTATION_TEXT::~ANNOTATION_TEXT()
{

}

void ANNOTATION_TEXT::parseFixPar(QList<TokenTyp> *typePar)
{
    //Mandatory PARAMETERS
    TokenTyp token;
    token = this->nextToken();
    if (token == String)
    {
        QString str;
        while (token == String)
        {
            str += lex->getLexem();
            token = this->nextToken();
        }
        parameters.append(str);
    }
    else
    {
        QString t(lex->toString(typePar->at(0)));
        QString s(lex->toString(token));
        this->showError("expected token : " + t +"\nfind token : " + s );
    }

}

TokenTyp ANNOTATION_TEXT::parseOptPar(QMap<QString, Occurence> *nameOptPar)
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

QMap<QString, QString> *ANNOTATION_TEXT::getParameters()
{
    QMap<QString, QString> *par = new  QMap<QString, QString>;
    for (int i = 0; i < namePar->count(); i++)
    {
        par->insert(namePar->at(i), parameters.at(i));
    }
    return par;
}

QString ANNOTATION_TEXT::getPar(QString str)
{
    int i = namePar->indexOf(str);
    return parameters.at(i);
}

// only for ANNOTATION_TEXT
TokenTyp ANNOTATION_TEXT::parseListString()
{
    TokenTyp token = lex->getNextToken();
    QString str;

    while (token == String)
    {
        str.append(lex->getLexem());
        token = lex->getNextToken();
    }

    char *c = new char[str.length() + 1];
    strcpy_s(c, str.length()+1, str.toLocal8Bit().data());
    parameters.append(lex->getLexem());
    return token;
}
