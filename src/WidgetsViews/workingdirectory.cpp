#include "workingdirectory.h"
#include "workproject.h"
#include "qdebug.h"
#include <QStringList>
#include <treedirectory.h>


WorkingDirectory::WorkingDirectory(QString rootPath, A2lTreeModel *model = NULL, MDImain *parent = 0 ) : Node()
{
    //set node Icon
    this->_pixmap = ":/icones/milky_classeur.png";

    //node name
    QString name = (QFileInfo(rootPath).fileName());
    this->name = name;
    this->rootPath = rootPath;

    // set treeView model from mdiMain
    this->mdimain = parent;
    this->model = model;

    // Create rootNode of TreeView if does not exiat
    Node* rootNode = model->getRootNode();
    if (rootNode == NULL)
        model->createRootNode();

    //set parent of this Node WorkingDirectory
    model->addNode2RootNode(this);

    //parse rootPath
    parseDir(rootPath);

    QObject::connect(&fileWatcher, &QFileSystemWatcher::directoryChanged, [=, this](const QString &path) {
     this->directoryChanged(path);
     } );

    QObject::connect(&fileWatcher, &QFileSystemWatcher::fileChanged, [=, this](const QString &path) {
     this->fileChanged(path);
     } );
}

WorkingDirectory::~WorkingDirectory()
{

}

void WorkingDirectory::parseDir(QString dirPath, WorkProject *wp)
{
    //set a QDir
    QDir dir(dirPath);
    dir.setFilter(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot |QDir::Hidden | QDir::NoSymLinks);
    dir.setSorting(QDir::Name);
    QFileInfoList list = dir.entryInfoList();

    //create a list to store .hex and .s19
    QFileInfoList listDatasets;
    QFileInfoList listDirs;
    TreeDirectory* subDir = nullptr;

    bool dirHasA2l = 0;
    if (wp)
    {
        dirHasA2l = 1;

        //create a node subFolder
        QString name = dir.dirName();
        subDir = new TreeDirectory(name);
        subDir->setPath(dirPath);
        subDir->setParentNode(wp);
        wp->addChildNode(subDir);
        wp->sortChildrensName();

        model->dataInserted(wp, wp->childNodes.indexOf(subDir));
    }

    foreach (QFileInfo file, list)
    {
       if (file.isFile())
       {
           if (file.suffix().toLower() == "a2l" && !dirHasA2l)
           {
               //check if .a2l already exists
               wp = mdimain->getWp(file.absoluteFilePath());

               //if wp does not exist creates a new WorkProject
               if (!wp)
               {
                   //check if a wp has already the same name (fileName) to ensure unique path in treeview model
                   QString displayNameOrg = file.fileName();
                   QString displayName = file.fileName();
                   int i = 0;
                   Node* node = getNode(displayName);
                   while (node)
                   {
                       displayName = displayNameOrg + " (" + QString::number(i) + ")";
                       node =  getNode(displayName);
                       i++;
                   }


                   // create a new Wp
                   wp = new WorkProject(file.absoluteFilePath(), model, mdimain, displayName);
                   wp->init(); //init but do not parse the file
                   wp->attach(mdimain);
                   mdimain->insertWp(wp);

                   //update the ui->treeView                  
                   this->addChildNode(wp);
                   wp->setParentNode(this);
                   this->sortChildrensName();
                   qsizetype index = this->childNodes.indexOf(wp);
                   model->dataInserted(this, index);

                   listWorkProjects.append(dirPath);
               }

               fileWatcher.addPath(file.absoluteFilePath());
               dirHasA2l = true;
           }
           else if (file.suffix().toLower() == "hex")
           {
               listDatasets.append(file);
           }
           else if (file.suffix().toLower() == "s19")
           {
               listDatasets.append(file);
           }
           else if (file.suffix().toLower() == "s32")
           {
               listDatasets.append(file);
           }
           else if (file.suffix().toLower() == "s37")
           {
               listDatasets.append(file);
           }
       }
       else if (file.isDir())
       {
           listDirs.append(file);
       }
    }

    //foreach file .hex or .s19 under selected folder and if .a2l file already exists
    if (dirHasA2l)
    {

        foreach (QFileInfo file, listDatasets)
        {
           if (file.isFile())
           {
               if (file.suffix().toLower() == "hex")
               {
                   HexFile* hex = new HexFile(file.absoluteFilePath(), wp);                   

                   if (subDir)
                   {
                       wp->addHex(hex, subDir);
                   }
                   else
                       wp->addHex(hex, wp);

                   fileWatcher.addPath(file.absoluteFilePath());
               }
               else if (file.suffix().toLower() == "s19")
               {
                   SrecFile* srec = new SrecFile(file.absoluteFilePath(), wp);
                   if (subDir)
                   {
                       wp->addSrec(srec, subDir);
                   }
                   else
                       wp->addSrec(srec, wp);

                   fileWatcher.addPath(file.absoluteFilePath());
               }
               else if (file.suffix().toLower() == "s32")
               {
                   SrecFile* srec = new SrecFile(file.absoluteFilePath(), wp);
                   if (subDir)
                   {
                       wp->addSrec(srec, subDir);
                   }
                   else
                       wp->addSrec(srec, wp);

                   fileWatcher.addPath(file.absoluteFilePath());
               }
               else if (file.suffix().toLower() == "s37")
               {
                   SrecFile* srec = new SrecFile(file.absoluteFilePath(), wp);
                   if (subDir)
                   {
                       wp->addSrec(srec, subDir);
                   }
                   else
                       wp->addSrec(srec, wp);

                   fileWatcher.addPath(file.absoluteFilePath());
               }
           }
        }
    }

    //parse the under directories at the end with the wp if exists
    foreach (QFileInfo file, listDirs)
    {
       if (file.isDir())
       {
           parseDir(file.absoluteFilePath(), wp);
       }
    }

    //set fileWatcher to control external changes to the root path
    fileWatcher.addPath(dirPath);

}

void WorkingDirectory::directoryChanged(QString dirPath)
{
    qDebug() << dirPath;
}

void WorkingDirectory::fileChanged(QString filePath)
{
    qDebug() << filePath;
}

QString WorkingDirectory::getFullPath()
{
    return rootPath;
}
