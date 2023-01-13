#ifndef CAMERATREEWIDGET_H
#define CAMERATREEWIDGET_H

#include <QTreeWidget>
#include <QWidget>
#include <QMouseEvent>

class CameraTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    explicit CameraTreeWidget(QWidget *parent = nullptr);

signals:

public slots:

private:
//    void dragMoveEvent(QDragMoveEvent *event);
//    void dragEnterEvent(QDragEnterEvent *event);
//    void mouseMoveEvent(QMouseEvent * event);
    void startDrag(Qt::DropActions);
};

#endif // CAMERATREEWIDGET_H
