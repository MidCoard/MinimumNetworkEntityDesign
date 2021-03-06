//
// Created by 周蜀杰 on 2022/6/4.
//

#include "Node.h"
#include "Edge.h"
#include "GraphWidget.h"
#include "NetworkEntity.h"

Node::Node(GraphWidget *graphWidget, NetworkEntity *entity)
		: graph(graphWidget),entity(entity) {
	setFlag(ItemIsMovable);
	setFlag(ItemSendsGeometryChanges);
	setCacheMode(DeviceCoordinateCache);
	setZValue(-1);
}

void Node::addEdge(Edge *edge) {
	edgeList << edge;
	edge->adjust();
}

QList<Edge *> Node::edges() const {
	return edgeList;
}

void Node::calculateForces() {
	if (!scene() || scene()->mouseGrabberItem() == this) {
		newPos = pos();
		return;
	}
	// Sum up all forces pushing this item away
	qreal xvel = 0;
	qreal yvel = 0;
	const QList<QGraphicsItem *> items = scene()->items();
	for (QGraphicsItem *item: items) {
		Node *node = qgraphicsitem_cast<Node *>(item);
		if (!node)
			continue;

		QPointF vec = mapToItem(node, 0, 0);
		qreal dx = vec.x();
		qreal dy = vec.y();
		double l = 2.0 * (dx * dx + dy * dy);
		if (l > 0) {
			xvel += (dx * 150.0) / l;
			yvel += (dy * 150.0) / l;
		}
	}
	double weight = (edgeList.size() + 1) * 10;
	for (const Edge *edge: qAsConst(edgeList)) {
		QPointF vec;
		if (edge->sourceNode() == this)
			vec = mapToItem(edge->destNode(), 0, 0);
		else
			vec = mapToItem(edge->sourceNode(), 0, 0);
		xvel -= vec.x() / weight;
		yvel -= vec.y() / weight;
	}
	if (qAbs(xvel) < 0.1 && qAbs(yvel) < 0.1)
		xvel = yvel = 0;
	QRectF sceneRect = scene()->sceneRect();
	newPos = pos() + QPointF(xvel, yvel);
	newPos.setX(qMin(qMax(newPos.x(), sceneRect.left() + 10), sceneRect.right() - 10));
	newPos.setY(qMin(qMax(newPos.y(), sceneRect.top() + 10), sceneRect.bottom() - 10));
}

bool Node::advancePosition() {
	if (newPos == pos())
		return false;

	setPos(newPos);
	return true;
}

QRectF Node::boundingRect() const {
	qreal adjust = 50;
	return {-10 - adjust, -10 - adjust, 23 + adjust, 23 + adjust};
}

QPainterPath Node::shape() const {
	QPainterPath path;
	path.addEllipse(boundingRect());
	return path;
}

void Node::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *) {
	QRadialGradient gradient(-20, -20, 50);
	gradient.setFocalPoint(3, 3);
	if (this->entity->isISP()) {
		if (option->state & QStyle::State_Sunken) {
			gradient.setColorAt(1, QColor(Qt::yellow).lighter(120));
			gradient.setColorAt(0, QColor(Qt::darkYellow).lighter(120));
		} else {
			gradient.setColorAt(0, Qt::yellow);
			gradient.setColorAt(1, Qt::darkYellow);
		}
	} else if (this->entity->isRouter()) {
		if (option->state & QStyle::State_Sunken) {
			gradient.setColorAt(1, QColor(Qt::blue).lighter(120));
			gradient.setColorAt(0, QColor(Qt::darkBlue).lighter(120));
		} else {
			gradient.setColorAt(0, Qt::blue);
			gradient.setColorAt(1, Qt::darkBlue);
		}
	} else if (this->entity->isSwitch()) {
		if (option->state & QStyle::State_Sunken) {
			gradient.setColorAt(1, QColor(Qt::green).lighter(120));
			gradient.setColorAt(0, QColor(Qt::darkGreen).lighter(120));
		} else {
			gradient.setColorAt(0, Qt::green);
			gradient.setColorAt(1, Qt::darkGreen);
		}
	}  else {
		if (option->state & QStyle::State_Sunken) {
			gradient.setColorAt(1, QColor(Qt::white).lighter(120));
			gradient.setColorAt(0, QColor(Qt::gray).lighter(120));
		} else {
			gradient.setColorAt(0, Qt::white);
			gradient.setColorAt(1, Qt::gray);
		}
	}

	painter->setBrush(gradient);
	painter->setPen(QPen(Qt::black, 0));
	painter->drawEllipse(boundingRect());
	painter->setPen(Qt::yellow);
	painter->drawText(boundingRect(),QString::fromStdString(entity->getName()));
}

QVariant Node::itemChange(GraphicsItemChange change, const QVariant &value) {
	switch (change) {
		case ItemPositionHasChanged:
			for (Edge *edge: qAsConst(edgeList))
				edge->adjust();
			graph->itemMoved();
			break;
		default:
			break;
	};

	return QGraphicsItem::itemChange(change, value);
}

void Node::mousePressEvent(QGraphicsSceneMouseEvent *event) {
	update();
	QGraphicsItem::mousePressEvent(event);
}



void Node::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
	update();
	QGraphicsItem::mouseReleaseEvent(event);
}