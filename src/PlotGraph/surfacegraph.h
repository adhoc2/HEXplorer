#ifndef SURFACEGRAPH_H
#define SURFACEGRAPH_H

#include <QObject>
#include <QtGraphs/QSurface3DSeries>
#include <QtGraphs/QSurfaceDataProxy>
#include <QtGraphs/QHeightMapSurfaceDataProxy>
#include <QtGraphsWidgets/Q3DSurfaceWidgetItem>

#include "data.h"

class SurfaceGraphModifier : public QObject
{
    Q_OBJECT
public:
    explicit SurfaceGraphModifier(Q3DSurfaceWidgetItem *surface, Data *data);
    ~SurfaceGraphModifier();

private:
    // Surface QtGraphs (Widgets)
    Q3DSurfaceWidgetItem *m_graph;

    // Séries QtGraphs
    QSurfaceDataProxy *m_dataProxy;
    QSurface3DSeries *m_heightMapSeries;
    QSurface3DSeries *m_dataSeries;

    // Helpers
    void fillDataProxy();

    Data *m_data;
};

#endif // SURFACEGRAPH_H