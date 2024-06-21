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

#include "Nodes/project.h"
#include <QMessageBox>
#include "a2lgrammar.h"
#include <qdebug.h>

//using namespace std;

PROJECT::PROJECT(Node *parentNode, A2lLexer *lexer)
    : Node(parentNode, lexer, parentNode->errorList)
{
    //get grammar
    A2lGrammar* gram = lexer->grammar;
    namePar = &gram->project.namePar;
    typePar = &gram->project.typePar;
    factoryOptNode = &gram->project.factoryOptNode;
    factoryOptItem = &gram->project.factoryOptItem;

    //optional parameters
    occOptPar = new QMap<QString, Occurence>;
    occOptPar->insert("HEADER", ZeroOrOne);
    occOptPar->insert("MODULE", ZeroOrMore);    

    //Set the line where the Node starts in ASAP file
    this->a2lLine = lex->getLine();

    //Parse Mandatory PARAMETERS
    parseFixPar(typePar);
    if (parameters.count() > 0)
        name = parameters.at(0);
    else
        name = (QString)"project";

    //Parse optional PARAMETERS
    TokenTyp token = parseOptPar(occOptPar);

    //End
    if (token == BlockEnd)
    {
        token = nextToken();
        if (token == Keyword && lex->getLexem() == "PROJECT")
        {
        }
        else
        {			
            QString s(lex->toString(token));
            this->showError("expected token : BlockEnd PROJECT\nfind token : " + s + " " + QString(lex->getLexem()));
        }
    }
    else if ((token == Eof || token == Identifier) && lex->getLexem() == "CHUNKend")
    {
        std::sort(this->childNodes.begin(), this->childNodes.end(), nodeLessThan);
        //std::sort(this->childNodes.begin(), this->childNodes.end()); // class Node has operator < implemented
        stopped = true;
        //lexer->in->seek(lexer->in->pos() - 8);
        //lexer->backward(8);
        lexer->backward();
    }
    else
    {
        QString s1(lex->toString(token));
        QString s2(lex->getLexem());
        this->showError("expected end PROJECT\nfind : " + s1 + " " + s2);
    }

    std::sort(this->childNodes.begin(), this->childNodes.end(), nodeLessThan);
    //std::sort(this->childNodes.begin(), this->childNodes.end()); // class Node has operator < implemented
}

PROJECT::~PROJECT()
{    
   //qDeleteAll(parameters);
    
    {
        
    }

   delete occOptPar;
}

void PROJECT::parseFixPar(QList<TokenTyp> *typePar)
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

TokenTyp PROJECT::parseOptPar(QMap<QString, Occurence> *nameOptPar)
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
                token = nextToken();
                if (token == Keyword)
                {
                    QString lexem = lex->getLexem();
                    if (factoryOptNode->contains(lexem))
                    {
                        if (occOptPar->value(lexem) == ZeroOrOne)
                        {
                            occOptPar->insert(lexem, Zero);
                            Node  *instance = factoryOptNode->value(lexem)->createInstance(this);
                            addChildNode(instance);
                            token = nextToken();
                        }
                        else if (occOptPar->value(lexem) == ZeroOrMore)
                        {
                            Node *instance;
                            if (lexem == "MODULE")
                            {
                                if (!isChild("MODULE"))
                                {
                                    Node *Char = new Node(this, lex, errorList);
                                    Char->name = (QString)"MODULE";
                                    addChildNode(Char);
                                    Char->_pixmap = "";
                                }
                                instance = factoryOptNode->value(lexem)->createInstance(child("MODULE", false));
                                child("MODULE", false)->addChildNode(instance);
                            }
                            token = nextToken();
                        }
                        else
                        {
                            QString s(lexem);
                            showError(" Keyword : " + s + " can only be once declared");
                            return token;
                        }
                    }
                    else
                    {
                        QString s(lexem);
                        showError("unknown Keyword : " + s);
                        return token;
                    }
                }
                else
                {
                    QString s(lex->toString(token));
                    showError("expected token : BlockBegin or Keyword\nfind token : " + s);
                    return token;
                }
            }
            //Items
            else if (token == Keyword)
            {
                QString lexem = lex->getLexem();
                if (factoryOptItem->contains(lexem))
                {
                    if (occOptPar->value(lexem) == ZeroOrOne)
                    {
                        occOptPar->insert(lexem, Zero);
                        Item  *instance = factoryOptItem->value(lexem)->createInstance(this);
                        addOptItem(instance);
                        token = nextToken();
                    }
                    else if (occOptPar->value(lexem) == ZeroOrMore)
                    {
                        Item  *instance = factoryOptItem->value(lexem)->createInstance(this);
                        addOptItem(instance);
                        token = nextToken();
                    }
                    else
                    {
                        QString s(lexem);
                        showError(" Keyword : " + s + " can only be once declared");
                        return token;
                    }
                }
                else
                {
                    QString s(lexem);
                    showError("unknown Keyword : " + s);
                    return token;
                }
            }
        }
        return token;
    }
}

QString PROJECT::pixmap()
{
    return ":/icones/MacHD.png";
}

QMap<QString, QString> *PROJECT::getParameters()
{
    QMap<QString, QString> *par = new QMap<QString, QString>;
    for (int i = 0; i < namePar->count(); i++)
    {
        par->insert(namePar->at(i), parameters.at(i));
    }
    return par;
}

QString PROJECT::getPar(QString str)
{
    int i = namePar->indexOf(str);
    return parameters.at(i);
}

QList<MODULE*> PROJECT::listModule()
{
    QList<MODULE*> list;
    Node *module = getNode("MODULE");
    if (module)
    {
        foreach(Node *module, module->childNodes)
        {
            list.append((MODULE*)module);
        }
    }
    return list;
}
