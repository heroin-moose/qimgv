#ifndef ZZSCROLLAREA_H
#define ZZSCROLLAREA_H

#include <QScrollArea>
#include <QScrollBar>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QPoint>
#include <QDebug>

class zzScrollArea : public QScrollArea
{
    Q_OBJECT
public:
    explicit zzScrollArea(QWidget *parent = 0);
    
signals:
    void sendDoubleClick();
    void resized();

public slots:

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *event);
QPoint lastDragPos,temp;
    
};

#endif // ZZSCROLLAREA_H
