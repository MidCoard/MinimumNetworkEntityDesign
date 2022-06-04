//
// Created by 周蜀杰 on 2022/6/4.
//

#ifndef NETWORKDESIGN_EDGE_H
#define NETWORKDESIGN_EDGE_H

class Node;

#include <QtWidgets/qgraphicsitem.h>


class Edge : public QGraphicsItem {
public:
	Edge(Node *sourceNode, Node *destNode);

	[[nodiscard]] Node *sourceNode() const;

	[[nodiscard]] Node *destNode() const;

	void adjust();

	enum {
		Type = UserType + 2
	};

	[[nodiscard]] int type() const override { return Type; }

protected:
	[[nodiscard]] QRectF boundingRect() const override;

	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
	Node *source, *dest;

	QPointF sourcePoint;
	QPointF destPoint;
	qreal arrowSize = 10;
};


#endif //NETWORKDESIGN_EDGE_H
