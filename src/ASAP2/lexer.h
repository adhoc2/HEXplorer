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

#ifndef LEXER_H
#define LEXER_H

#include "qobject.h"
#include "qobjectdefs.h"
#include <QList>
#include <QString>
#include <QTextStream>
#include <QHash>


class A2lGrammar;

enum TokenTyp
{
    Identifier ,String, StringFormat, Float, Integer, Comment, Hex, myUnknown, Plus, Minus, BlockComment,
    BlockBegin, BlockEnd, Eof, Eol, ValueSeparator, Text,
    Keyword, DataType, Datasize, Addrtype, Byteorder, Indexorder, ConversionType, Type, Attribute, Mode,
    PrgType, MemoryType, MemAttribute, IndexMode, Indentation, UnitType
};

enum Occurence
{
   Zero, ZeroOrOne, ZeroOrMore
};

class Buffer
{
    public:
        Buffer();
        void read(QTextStream *in);
        void read(QTextStream &in);
        QChar getAndClear();
        QChar getValue();
        void clear();
        bool isFull();

    private:
        bool state;
        QChar value;
};

class A2lLexer : public QObject
{
    Q_OBJECT

    public:
        //A2lLexer(QTextStream &in, QObject *parent = 0);
        A2lLexer(QTextStream &in, QObject *parent = nullptr);
        A2lLexer(QObject *parent = nullptr);
        ~A2lLexer();

        virtual TokenTyp getNextToken();
        virtual QString toString(TokenTyp type);
        virtual QString getLexem();
        virtual int getLine();
        virtual void initialize();
        virtual void backward();
        A2lGrammar *grammar;
        QTextStream *in;

    private:        
        Buffer *buffer;
        int position;
        int line;
        int index;
        QString lexem;
        QHash<QString, TokenTyp> keywordsList;
        QString keywords;
        TokenTyp begin(QChar ch);
        TokenTyp identifier(QChar &ch);
        TokenTyp string();
        TokenTyp commentL();
        TokenTyp commentM();
        TokenTyp number(QChar &ch);
        TokenTyp hexadecimal();
        TokenTyp block(QChar &ch);
        TokenTyp getPartialString();

        bool isSeparator(QChar ch);
        bool isDigit(QChar ch);
        bool isHexDigit(QChar ch);
        bool isLetter(QChar ch);
        bool isA2mlSym(QChar ch);


    signals:
        void returnedToken(int );
};



#endif // A2LLEXER_H
