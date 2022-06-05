//
// Created by 周蜀杰 on 2022/6/4.
//

#ifndef NETWORKDESIGN_GRAPHWIDGET_H
#define NETWORKDESIGN_GRAPHWIDGET_H

class Node;
class Network;

#include <QGraphicsView>

class GraphWidget : public QGraphicsView {
Q_OBJECT

public:

	GraphWidget(QWidget *parent, Network *network);

	void itemMoved();

public slots:

	void shuffle();

	void zoomIn();

	void zoomOut();

protected:
	void keyPressEvent(QKeyEvent *event) override;

	void timerEvent(QTimerEvent *event) override;

#if QT_CONFIG(wheelevent)

	void wheelEvent(QWheelEvent *event) override;

#endif

	void drawBackground(QPainter *painter, const QRectF &rect) override;

	void scaleView(qreal scaleFactor);

private:
	int timerId = 0;
	Node *centerNode;
};


#endif //NETWORKDESIGN_GRAPHWIDGET_H
