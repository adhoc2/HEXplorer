#ifndef ZOOMER_H
#define ZOOMER_H

#include <QObject>
#include "scrollzoomer.h"

class Zoomer: public ScrollZoomer
{

    Q_OBJECT

public:
    Zoomer(const Zoomer &) = delete;
    Zoomer(Zoomer &&) = delete;
    Zoomer &operator=(const Zoomer &) = delete;
    Zoomer &operator=(Zoomer &&) = delete;

    Zoomer(QWidget *canvas);
    virtual QwtText trackerTextF( const QPointF &pos ) const;
    virtual void rescale();


};

#endif // ZOOMER_H
