
#include <QtGui>


#include "SLGActorNode.h"

#include <QGraphicsSceneMouseEvent>
#include <QLineEdit>
#include <QColorDialog>
#include <QInputDialog>

#include <QDebug>


SLGActorNode* SLGActorNode::CreateSLGActorNode() {
	SLGActorNode* pInstance = new SLGActorNode();
	pInstance->_type = SLIBasicGraphicItem::Type::Actor;
	pInstance->setFlags(ItemIsSelectable);
	return pInstance;
}

SLGActorNode::SLGActorNode() :SLIBasicGraphicItem()
, _dbRadius(30)
{
	// 	myTextColor = Qt::darkGreen;
	// 	myOutlineColor = Qt::darkBlue;
	// 	myBackgroundColor = Qt::white;

	// 	setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsGeometryChanges);
}

SLGActorNode::~SLGActorNode()
{
}

QRectF SLGActorNode::boundingRect() const
{
	const int Margin = 1;
	return calculateOutlineRect().adjusted(-Margin, -Margin, +Margin, +Margin);
}

QPainterPath SLGActorNode::shape() const
{
	QRectF rect = calculateOutlineRect();
	QPainterPath path;
	path.addRoundedRect(rect, roundness(rect.width()), roundness(rect.height()));
	return path;
}


void SLGActorNode::paintSelectionOutline(QPainter* painter)
{
}

void SLGActorNode::setRadius(double dbR)
{
	_dbRadius = dbR;
	if (_dbRadius < c_dbMinR) _dbRadius = c_dbMinR;
}


QRectF SLGActorNode::calculateOutlineRect() const
{
	// 	const int Padding = 8;
	// 	QFontMetricsF metrics = QFontMetricsF(qApp->font());
	// 	QRectF rect = metrics.boundingRect(_text);
	// 	rect.adjust(-Padding, -Padding, +Padding, +Padding);
	// 	rect.translate(-rect.center());
	// 	return rect;

	const int Padding = 8;
	QRectF rect(-_dbRadius / 2.0, -_dbRadius / 2.0, _dbRadius, _dbRadius);
	// 	rect.adjust(-Padding, -Padding, +Padding, +Padding);
	return rect;
}

void SLGActorNode::paintSelected(QPainter* painter) {

	QPen pen(Qt::red);
	painter->setPen(pen);
	QColor fillColor(0, 0, 200, 255 * _dbWeight);
	painter->setBrush(fillColor);
	painter->drawPath(shape());
}


void SLGActorNode::drawGraphic(QPainter* painter)
{
	if (_bCued)
	{
		QPen pen(Qt::green);
		painter->setPen(pen);
	}
	else {
		QPen pen(Qt::blue);
		painter->setPen(pen);
	}
	QColor fillColor(0, 0, 200, 255 * _dbWeight);
	painter->setBrush(fillColor);
	painter->drawPath(shape());
}
