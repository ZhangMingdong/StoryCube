
#include <QtGui>


#include "SLGNode.h"

#include <QGraphicsSceneMouseEvent>
#include <QLineEdit>
#include <QColorDialog>
#include <QInputDialog>

#include <QDebug>


SLGNode* SLGNode::CreateSLGNode() {
	SLGNode* pInstance = new SLGNode();
	pInstance->_type = SLIBasicGraphicItem::Type::Location;
	pInstance->setFlags(ItemIsSelectable);
	return pInstance;
}

SLGNode::SLGNode() :SLIBasicGraphicItem()
, _nWidth(30), _nHeight(30)
{
	// 	myTextColor = Qt::darkGreen;
	// 	myOutlineColor = Qt::darkBlue;
	// 	myBackgroundColor = Qt::white;

	// 	setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsGeometryChanges);
}

SLGNode::~SLGNode()
{
}

QRectF SLGNode::boundingRect() const
{
	const int Margin = 1;
	return calculateOutlineRect().adjusted(-Margin, -Margin, +Margin, +Margin);
}

QPainterPath SLGNode::shape() const
{
	QRectF rect = calculateOutlineRect();

	QPainterPath path;
	path.addRect(rect);
	// 	path.addRoundRect(rect, roundness(rect.width()),
	// 		roundness(rect.height()));
	return path;
}

void SLGNode::drawGraphic(QPainter* painter)
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

	QColor fillColor(0, 0, 200, 255*_dbWeight);
	painter->setBrush(fillColor);
	painter->drawRect(calculateOutlineRect());
}

void SLGNode::paintSelectionOutline(QPainter* painter)
{
}

void SLGNode::setWidth(int width)
{
	_nWidth = abs(width);
}

void SLGNode::setHeight(int height)
{
	_nHeight = abs(height);
}

QRectF SLGNode::calculateOutlineRect() const
{
	// 	const int Padding = 8;
	// 	QFontMetricsF metrics = QFontMetricsF(qApp->font());
	// 	QRectF rect = metrics.boundingRect(_text);
	// 	rect.adjust(-Padding, -Padding, +Padding, +Padding);
	// 	rect.translate(-rect.center());
	// 	return rect;

	const int Padding = 8;
	QRectF rect(-_nWidth / 2.0, -_nHeight / 2.0, _nWidth, _nHeight);
	// 	rect.adjust(-Padding, -Padding, +Padding, +Padding);
	return rect;
}

void SLGNode::paintSelected(QPainter* painter) {

	QPen pen(Qt::red);
	painter->setPen(pen);
	QColor fillColor(0, 0, 200, 255 * _dbWeight);
	painter->setBrush(fillColor);
	painter->drawRect(calculateOutlineRect());
}


