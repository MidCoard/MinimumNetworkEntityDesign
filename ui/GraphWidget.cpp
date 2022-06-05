//
// Created by 周蜀杰 on 2022/6/4.
//

#include "GraphWidget.h"
#include "Edge.h"
#include "Node.h"
#include "QWheelEvent"
#include "Network.h"

GraphWidget::GraphWidget(QWidget *parent, Network* network)
		: QGraphicsView(parent) {
	auto *scene = new QGraphicsScene(this);
	scene->setItemIndexMethod(QGraphicsScene::NoIndex);
	scene->setSceneRect(-200, -200, 400, 400);
	setScene(scene);
	setCacheMode(CacheBackground);
	setViewportUpdateMode(BoundingRectViewportUpdate);
	setRenderHint(QPainter::Antialiasing);
	setTransformationAnchor(AnchorUnderMouse);
	scale(qreal(0.8), qreal(0.8));
	setMinimumSize(400, 400);
	setWindowTitle(tr("Network Graph"));
	std::vector<Node*> nodes;
	centerNode = new Node(this, network->getNodes()[0]);
	scene->addItem(centerNode);
	nodes.push_back(centerNode);
	for (int i = 1;i<network->getNodes().size();i++) {
		auto node = network->getNodes()[i];
		auto * n = new Node(this, node);
		nodes.push_back(n);
		scene->addItem(n);
		n->setPos(i * 5, i * 5);
	}
	std::vector<bool> visited(network->getLinks().size(), false);
	for (int i = 0;i<network->getLinks().size();i++) {
		if (visited[i]) continue;
		auto* link = network->getLinks()[i];
		visited[i] = true;
		visited[link->undirected] = true;
		auto * e = new Edge(nodes[link->father], nodes[link->node]);
		scene->addItem(e);
	}
}

void GraphWidget::itemMoved() {
	if (!timerId)
		timerId = startTimer(1000 / 25);
}

void GraphWidget::keyPressEvent(QKeyEvent *event) {
	switch (event->key()) {
		case Qt::Key_Up:
			centerNode->moveBy(0, -20);
			break;
		case Qt::Key_Down:
			centerNode->moveBy(0, 20);
			break;
		case Qt::Key_Left:
			centerNode->moveBy(-20, 0);
			break;
		case Qt::Key_Right:
			centerNode->moveBy(20, 0);
			break;
		case Qt::Key_Plus:
			zoomIn();
			break;
		case Qt::Key_Minus:
			zoomOut();
			break;
		case Qt::Key_Space:
		case Qt::Key_Enter:
			shuffle();
			break;
		default:
			QGraphicsView::keyPressEvent(event);
	}
}

void GraphWidget::timerEvent(QTimerEvent *event) {
	Q_UNUSED(event);

	QList<Node *> nodes;
	const QList<QGraphicsItem *> items = scene()->items();
	for (QGraphicsItem *item: items) {
		if (Node *node = qgraphicsitem_cast<Node *>(item))
			nodes << node;
	}

	for (Node *node: qAsConst(nodes))
		node->calculateForces();

	bool itemsMoved = false;
	for (Node *node: qAsConst(nodes)) {
		if (node->advancePosition())
			itemsMoved = true;
	}

	if (!itemsMoved) {
		killTimer(timerId);
		timerId = 0;
	}
}

void GraphWidget::wheelEvent(QWheelEvent *event) {
	scaleView(pow(2., -event->angleDelta().y() / 240.0));
}

void GraphWidget::drawBackground(QPainter *painter, const QRectF &rect) {
	Q_UNUSED(rect);
}

void GraphWidget::scaleView(qreal scaleFactor) {
	qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
	if (factor < 0.07 || factor > 100)
		return;

	scale(scaleFactor, scaleFactor);
}

void GraphWidget::shuffle() {
	QList<Node *> nodes;
	const QList<QGraphicsItem *> items = scene()->items();
	for (QGraphicsItem *item: items) {
		if (Node *node = qgraphicsitem_cast<Node *>(item))
			nodes << node;
	}

	for (Node *node: qAsConst(nodes)) {
		node->setPos(rand() % 100 - 50, rand() % 100 - 50);
	}
}

void GraphWidget::zoomIn() {
	scaleView(qreal(1.2));
}

void GraphWidget::zoomOut() {
	scaleView(1 / qreal(1.2));
}
