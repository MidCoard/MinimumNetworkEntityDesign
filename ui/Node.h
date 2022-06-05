//
// Created by 周蜀杰 on 2022/6/4.
//

#ifndef NETWORKDESIGN_NODE_H
#define NETWORKDESIGN_NODE_H

class Edge;

class GraphWidget;

#include <QtWidgets/qgraphicsitem.h>
#include <QRadialGradient>
#include <QtWidgets/qstyle.h>
#include <QPainter>
#include "QStyleOption"
#include "NetworkEntity.h"

class Node : public QGraphicsItem {
public:
	explicit Node(GraphWidget *graphWidget, NetworkEntity *entity);

	void addEdge(Edge *edge);

	[[nodiscard]] QList<Edge *> edges() const;

	enum {
		Type = UserType + 1
	};

	[[nodiscard]] int type() const override { return Type; }

	void calculateForces();

	bool advancePosition();

	[[nodiscard]] QRectF boundingRect() const override;

	[[nodiscard]] QPainterPath shape() const override;

	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

protected:
	QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

	void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
	QList<Edge *> edgeList;
	QPointF newPos;
	GraphWidget *graph;
	NetworkEntity *entity;
};

#endif //NETWORKDESIGN_NODE_H
