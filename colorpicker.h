#ifndef COLORPICKER_H
#define COLORPICKER_H

#include <QWidget>

class ColorPicker : public QWidget
{
    typedef QWidget inherited;
    Q_OBJECT
public:
    explicit ColorPicker(QWidget *parent = 0);

    QColor color() const { return m_color; }
    void setColor( QColor color ) { m_color = color; update(); }

signals:
    void changed( QColor color);

public slots:

protected:
    QSize sizeHint() const;
    bool hasHeightForWidth() const { return true; }
    int heightForWidth( int width) const { return width; }
    QRect area( ) const;

    void resizeEvent( QResizeEvent *e);
    void paintEvent( QPaintEvent *e);
    void mousePressEvent( QMouseEvent *e);
    void mouseMoveEvent( QMouseEvent *e);

    QPixmap m_wheel;
    QColor m_color;
};

#endif // COLORPICKER_H
