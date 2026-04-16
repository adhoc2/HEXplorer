#include "surfacegraph.h"
#include "axis_descr.h"
#include "compu_method.h"
#include <QtGui/QImage>
#include <QtCore/qmath.h>

//using namespace QtDataVisualization;
SurfaceGraphModifier::SurfaceGraphModifier(Q3DSurfaceWidgetItem *surface, Data *data)
    : m_graph(surface)
    , m_data(data)
{
    fillDataProxy();
}


SurfaceGraphModifier::~SurfaceGraphModifier()
{

}

void SurfaceGraphModifier::fillDataProxy()
{
    int dimX = m_data->getX().count();
    int dimY = m_data->getY().count();

    QSurfaceDataArray dataArray;
    dataArray.reserve(dimY);

    for (int j = 0; j < dimY; ++j)
    {
        double y = m_data->getY(j).toDouble();

        QSurfaceDataRow row;
        row.reserve(dimX);

        for (int i = 0; i < dimX; ++i)
        {
            double x = m_data->getX(i).toDouble();
            double z = m_data->getZ(i * dimY + j).toDouble();

            row.append(QSurfaceDataItem(QVector3D(x, z, y)));
        }

        dataArray.append(row);
    }

    m_dataSeries = new QSurface3DSeries;
    m_dataSeries->dataProxy()->resetArray(dataArray);
    m_graph->addSeries(m_dataSeries);
    m_dataSeries->setDrawMode(QSurface3DSeries::DrawSurfaceAndWireframe);

    QLinearGradient gr;
    gr.setColorAt(0.f, Qt::darkGreen);
    gr.setColorAt(0.5f, Qt::yellow);
    gr.setColorAt(0.8f, Qt::red);
    gr.setColorAt(1.f, Qt::darkRed);
    m_dataSeries->setBaseGradient(gr);
    m_dataSeries->setColorStyle(QGraphsTheme::ColorStyle::RangeGradient);

    // axisX title
    m_graph->axisX()->setTitleVisible(true);
    AXIS_DESCR *axisX = m_data->getAxisDescrX();
    COMPU_METHOD *cpmX = m_data->getCompuMethodAxisX();
    QString name = "";
    if (axisX)
    {
        name.append(axisX->fixPar("InputQuantity"));
    }
    QString unit = "";
    if (cpmX)
    {
        unit.append(cpmX->fixPar("Unit"));
    }
    QString titleX =  name + " - " + unit;
    m_graph->axisX()->setTitle(titleX);

    // axisZ title
    m_graph->axisZ()->setTitleVisible(true);
    AXIS_DESCR *axisY = m_data->getAxisDescrY();
    COMPU_METHOD *cpmY = m_data->getCompuMethodAxisY();
    name = "";
    if (axisY)
    {
        name.append(axisY->fixPar("InputQuantity"));
    }
    unit = "";
    if (cpmY)
    {
        unit.append(cpmY->fixPar("Unit"));
    }
    QString titleZ =  name + " - " + unit;
    m_graph->axisZ()->setTitle(titleZ);

    m_graph->axisY()->setTitleVisible(true);
    m_graph->axisY()->setTitle(m_data->getName() + " - " + m_data->getUnit());

}
