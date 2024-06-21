#ifndef TREEDIRECTORY_H
#define TREEDIRECTORY_H

#include <node.h>


class TreeDirectory : public Node
{
public:
    TreeDirectory(QString name);
    QString getPath();
    QString fullName();
    void setPath(QString str);

private:
    QString path;
    QString fullDirName;
};

#endif // TREEDIRECTORY_H
