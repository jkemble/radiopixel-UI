#include "math.h"
#include <QDebug>
#include <QPainter>
#include <QMouseEvent>
#include "colorpicker.h"

const double PI = 3.141592653;

ColorPicker::ColorPicker(QWidget *parent)
    : QWidget(parent)
{
}

QSize ColorPicker::sizeHint() const
{
    return QSize( 150, 150 );
}

QRect ColorPicker::area() const
{
    int s( std::min( width(), height()));
    return QRect( ( width( ) - s ) / 2, (height() - s ) / 2, s, s );
}

void ColorPicker::resizeEvent(QResizeEvent *e)
{
    QImage original( ":/ui/wheel.png");
    QImage scaled( original.scaled( area().size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    m_wheel = QPixmap::fromImage( scaled);

    inherited::resizeEvent( e);
}

void ColorPicker::paintEvent(QPaintEvent *e)
{
    QPainter p( this);
    QRect a( area());

    p.drawPixmap( a, m_wheel );

    double angle = 2 * PI * m_color.hue() / 360;
    double radius = a.width() / 2 * m_color.saturation() / 255;
    int x = a.center().x() + radius * sin( angle );
    int y = a.center().y() - radius * cos( angle );
    p.setBrush( Qt::black);
    QFontMetrics fm( font());
    int s( fm.averageCharWidth());
    p.setPen( QPen( Qt::white, s/2));
    p.drawEllipse( QPoint(x, y), s, s);

    inherited::paintEvent( e);
}

void ColorPicker::mousePressEvent( QMouseEvent *e)
{
    mouseMoveEvent( e);
}

void ColorPicker::mouseMoveEvent( QMouseEvent *e)
{
    QRect a( area());
    QPoint c( a.center());
    double angle( atan2( e->x() - c.x(), c.y() - e->y()));
    if ( angle < 0) angle += 2*PI;
    int hue( 360 * angle / (2*PI) );
    double radius( sqrt( pow( e->x() - c.x(), 2) + pow( e->y() - c.y(), 2)));
    int sat( 255 * radius / (a.width()/2) );
    if ( sat > 255) sat = 255;
    m_color.setHsv( hue, sat, 255 );

    update();
    emit changed( m_color);
}
