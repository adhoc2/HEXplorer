#include "zoomer.h"
#include <qwt_scale_widget.h>
#include "QPen"

const unsigned int c_rangeMax = 1000;

Zoomer::Zoomer(QWidget *canvas) : ScrollZoomer(canvas)
{
    setRubberBandPen( QPen( Qt::red ) );
}

QwtText Zoomer::trackerTextF( const QPointF &pos ) const
{
    QColor bg( Qt::white );

    QwtText text = QwtPlotZoomer::trackerTextF( pos );
    text.setBackgroundBrush( QBrush( bg ) );
    return text;
}

void Zoomer::rescale()
{
    QwtScaleWidget *scaleWidget = plot()->axisWidget( yAxis() );
    QwtScaleDraw *sd = scaleWidget->scaleDraw();

    double minExtent = 0.0;
    if ( zoomRectIndex() > 0 )
    {
        // When scrolling in vertical direction
        // the plot is jumping in horizontal direction
        // because of the different widths of the labels
        // So we better use a fixed extent.

        minExtent = sd->spacing() + sd->maxTickLength() + 1;
        minExtent += sd->labelSize(scaleWidget->font(), c_rangeMax ).width();
    }

    sd->setMinimumExtent( minExtent );

    ScrollZoomer::rescale();

};
