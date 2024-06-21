#include "treedirectory.h"

TreeDirectory::TreeDirectory(QString name) :  Node(name)
{

}

void TreeDirectory::setPath(QString str)
{
    path = str;
    fullDirName = path + "/" + QString(this->name);
}

QString TreeDirectory::getPath()
{
    return path;
}

QString TreeDirectory::fullName()
{
    return fullDirName;
}
