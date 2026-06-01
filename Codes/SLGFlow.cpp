
#include <QtGui>


#include "SLGFlow.h"

#include <QGraphicsSceneMouseEvent>
#include <QLineEdit>
#include <QColorDialog>
#include <QInputDialog>

#include <QDebug>


SLGFlow* SLGFlow::CreateSLGFlow(SLIBasicGraphicItem* item1, SLIBasicGraphicItem* item2, int nW) {
	SLGFlow* pInstance = new SLGFlow();
	pInstance->setX(item2->pos().x());
	pInstance->setY(item1->pos().y());
	pInstance->_nW = nW;
	pInstance->_nWidth = nW;
	pInstance->_nHeight = nW;

	pInstance->_pItem1 = item1;
	pInstance->_pItem2 = item2;

	return pInstance;

}

SLGFlow::SLGFlow() :SLIBasicGraphicItem()
, _nWidth(30), _nHeight(30)
{
	// 	myTextColor = Qt::darkGreen;
	// 	myOutlineColor = Qt::darkBlue;
	// 	myBackgroundColor = Qt::white;

	// 	setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsGeometryChanges);
	setFlags(ItemIsSelectable);
}

SLGFlow::~SLGFlow()
{
}

QRectF SLGFlow::boundingRect() const
{
	QPointF pt1, pt2;
	pt1 = _pItem1->getLinkPosition(this->pos());
	pt2 = _pItem2->getLinkPosition(this->pos());

	const int Margin = 1;
	return calculateOutlineRect().united(QRectF(pt1-pos(),pt2-pos())).adjusted(-Margin, -Margin, +Margin, +Margin);
}

QPainterPath SLGFlow::shape() const
{

	QPainterPath path;

	// in flow line
	{
		QPointF pt1, pt2;
		pt1 = _pItem1->getLinkPosition(this->pos()) - this->pos();
		pt2 = this->getLinkPosition(_pItem1->pos()) - this->pos();
		path.moveTo(pt2);
		path.lineTo(pt1);
	}
	// out flow line
	{
		QPointF pt1, pt2;
		pt1 = _pItem2->getLinkPosition(this->pos()) - this->pos();
		pt2 = this->getLinkPosition(_pItem2->pos()) - this->pos();
		path.moveTo(pt2);
		path.lineTo(pt1);

		/*
		double dbR = 3;
		if (abs(pt2.y() - pt1.y()) > dbR) {
			double dbXBias = 0.5 * dbR;
			double dbYBias = pt1.y() < pt2.y() ? dbR : -dbR;
			path.lineTo(pt1 + QPointF(dbXBias, dbYBias));
			path.lineTo(pt1 + QPointF(-dbXBias, dbYBias));
			path.lineTo(pt1);
		}
		*/
	}
	// connection
	path.addEllipse(calculateOutlineRect());
	return path;


	/*
	QRectF rect = calculateOutlineRect();

	QPainterPath path;
	path.addRect(rect);
	// 	path.addRoundRect(rect, roundness(rect.width()),
	// 		roundness(rect.height()));
	return path;
	*/
}

void SLGFlow::drawGraphic(QPainter* painter)
{
	painter->drawPath(shape());
}

void SLGFlow::paintSelectionOutline(QPainter* painter)
{

}

void SLGFlow::paintSelected(QPainter* painter) {
	QPen pen(Qt::black);
	painter->setPen(pen);
	QColor fillColor(200, 200, 255, 100);
	painter->setBrush(fillColor);
	painter->drawPath(shape());
	//painter->drawRect(boundingRect());	// used for check the bounding rect
}

QRectF SLGFlow::calculateOutlineRect() const
{
	// 	const int Padding = 8;
	// 	QFontMetricsF metrics = QFontMetricsF(qApp->font());
	// 	QRectF rect = metrics.boundingRect(_text);
	// 	rect.adjust(-Padding, -Padding, +Padding, +Padding);
	// 	rect.translate(-rect.center());
	// 	return rect;

	QRectF rect(-_nWidth / 2.0, -_nHeight / 2.0, _nWidth, _nHeight);
	// 	rect.adjust(-Padding, -Padding, +Padding, +Padding);
	return rect;
}







