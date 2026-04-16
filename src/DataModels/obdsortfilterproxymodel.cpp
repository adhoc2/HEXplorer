#include "obdsortfilterproxymodel.h"
#include "ObdMergeModel.h"
#include "qdebug.h"

obdSortFilterProxyModel::obdSortFilterProxyModel(QObject *parent, ObdMergeModel *obdModel)
    : QSortFilterProxyModel(parent)
{
    setSourceModel(obdModel);
    //connect(obdModel, SIGNAL(dataChanged()), this , SLOT(setFilterRegExp("")));
}

obdSortFilterProxyModel::obdSortFilterProxyModel(QObject *parent, ObdMergeModelEcu4 *obdModel)
    : QSortFilterProxyModel(parent)
{
    setSourceModel(obdModel);
    //connect(obdModel, SIGNAL(dataChanged()), this , SLOT(setFilterRegExp("")));
}

bool obdSortFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    QVariant leftData  = sourceModel()->data(left);
    QVariant rightData = sourceModel()->data(right);

    // Tentative de conversion numérique
    bool ok1 = false, ok2 = false;
    double n1 = leftData.toString().toDouble(&ok1);
    double n2 = rightData.toString().toDouble(&ok2);

    // Si les 2 valeurs sont numériques → tri numérique
    if (ok1 && ok2)
        return n1 < n2;

    // Sinon → tri texte normal
    return QString::localeAwareCompare(leftData.toString(), rightData.toString()) < 0;
}


bool obdSortFilterProxyModel::filterAcceptsRow_old(int sourceRow,const QModelIndex &sourceParent) const
{

    bool bl = true;
    QList<int> listCol({11, 13, 15, 17, 19, 21, 23});
    bool flagReaction = false;

    foreach (int col, filtersMap.keys())
    {
        if (!listCol.contains(col))
        {
            bool bl_col = false;
            QModelIndex index = sourceModel()->index(sourceRow, col, sourceParent);
            QList<QString> values = filtersMap.values(col);
            for (int i = 0; i < values.size(); ++i)
            {
                QRegularExpression regExp("^" + values.at(i) + "$",  QRegularExpression::CaseInsensitiveOption);
                QString str = sourceModel()->data(index).toString();
                bl_col = bl_col ||  str.contains(regExp);
            }
            bl = bl && bl_col;
        }
        else
            flagReaction = true;
    }

    if (!flagReaction) return bl;
    else
    {
        bool bl_reaction = false;
        foreach (int col, filtersMap.keys())
        {
            if (listCol.contains(col))
            {
                bool bl_col = false;
                QModelIndex index = sourceModel()->index(sourceRow, col, sourceParent);
                QList<QString> values = filtersMap.values(col);
                for (int i = 0; i < values.size(); ++i)
                {
                    QRegularExpression regExp("^" + values.at(i) + "$");
                    QString str = sourceModel()->data(index).toString();
                    bl_col = bl_col ||  str.contains(regExp);
                }
                bl_reaction = bl_reaction || bl_col;
            }
         }
        return bl && bl_reaction;
    }
}

bool obdSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    // Pour chaque colonne ayant un filtre
    for (int col : filtersMap.keys())
    {
        QModelIndex index = sourceModel()->index(sourceRow, col, sourceParent);
        QString cellText = sourceModel()->data(index).toString();

        // Liste des valeurs filtrantes sur cette colonne
        const QList<QString> filterValues = filtersMap.values(col);

        bool columnMatches = false;

        // Une colonne est valide si au moins un filtre correspond
        for (const QString &value : filterValues)
        {
            QRegularExpression regex(
                value,
                QRegularExpression::CaseInsensitiveOption
                );

            if (cellText.contains(regex)) {
                columnMatches = true;
                break;
            }
        }

        // Si la colonne ne matche aucun filtre → ligne rejetée
        if (!columnMatches)
            return false;
    }

    // Toutes les colonnes filtrées sont validées → ligne acceptée
    return true;
}

Data* obdSortFilterProxyModel::getData(QModelIndex indexProxy)
{
    QModelIndex indexSourceModel = mapToSource(indexProxy);
    ObdMergeModel* model = qobject_cast<ObdMergeModel*>(this->sourceModel());
    if (model)
    {
        Data* data = model->getData(indexSourceModel.row(), indexSourceModel.column());
        return data;
    }
    else
    {
        ObdMergeModelEcu4* modelecu4 = qobject_cast<ObdMergeModelEcu4*>(this->sourceModel());
        if (modelecu4)
        {
            Data* data = modelecu4->getData(indexSourceModel.row(), indexSourceModel.column());
            return data;
        }
        else return nullptr;
    }
}

QStringList obdSortFilterProxyModel::getUniqueValues(int column)
{
    QStringList list;
    int nrow = sourceModel()->rowCount();
    for (int i=0; i < nrow; i++)
    {
        QModelIndex index1 = sourceModel()->index(i, column, QModelIndex());
        QString value = sourceModel()->data(index1).toString();
        if (!list.contains(value))
        {
            list.append(value);
        }
    }
    return list;
}

void obdSortFilterProxyModel::addFilter(int column, QString filter)
{
    if (column == 1)
    {
        this->filtersMap.remove(column);
    }
    this->filtersMap.insert(column,filter);
    qDebug() << "add filter : " + filter;

    QRegularExpression regExp("");
    this->setFilterRegularExpression(regExp);
}

void obdSortFilterProxyModel::addFilters(QList<int> list, QString filter)
{
    foreach (int i, list)
    {
        this->filtersMap.insert(i,filter);
    }

    QRegularExpression regExp("");
    this->setFilterRegularExpression(regExp);
}

void obdSortFilterProxyModel::removeFilter(int column, QString filter)
{
    this->filtersMap.remove(column, filter);

    QRegularExpression regExp("");
    this->setFilterRegularExpression(regExp);
}

void obdSortFilterProxyModel::removeFilters(QList<int> list, QString filter)
{
    foreach (int i, list)
    {
        this->filtersMap.remove(i,filter);
    }

    QRegularExpression regExp("");
    this->setFilterRegularExpression(regExp);
}

void obdSortFilterProxyModel::resetAllFilters()
{
    filtersMap.clear();

    beginResetModel();

    // Vide réellement le filtre du QSortFilterProxyModel
    setFilterRegularExpression(QRegularExpression());

    // Force Qt à recalculer le contenu filtré
    endResetModel();

}
