
#include <QtGui>


#include "SLGRect.h"

#include <QGraphicsSceneMouseEvent>
#include <QLineEdit>
#include <QColorDialog>
#include <QInputDialog>

#include <QDebug>


SLGRect* SLGRect::CreateSLGRect(SLIBasicGraphicItem::Type type) {
	SLGRect* pInstance = new SLGRect();
	pInstance->_type = type;
	switch (type) {
	case Type::None:
		//pInstance->setPen(QColor(0, 0, 0, 100));
		break;
	case Type::Location:
		pInstance->setFlags(ItemIsSelectable);
		break;
	case Type::LocationBar:
		pInstance->setFlags(ItemIsSelectable);
		break;
	case Type::Session:
		pInstance->setFlags(ItemIsSelectable);
		break;
	case Type::Actor:
		pInstance->setFlags(ItemIsSelectable);
		break;
	default:
		break;
	}
	return pInstance;
}

SLGRect::SLGRect() :SLIBasicGraphicItem()
, _nWidth(30), _nHeight(30)
{
	// 	myTextColor = Qt::darkGreen;
	// 	myOutlineColor = Qt::darkBlue;
	// 	myBackgroundColor = Qt::white;

	// 	setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsGeometryChanges);
}

SLGRect::~SLGRect()
{
}

QRectF SLGRect::boundingRect() const
{
	const int Margin = 1;
	return calculateOutlineRect().adjusted(-Margin, -Margin, +Margin, +Margin);
}

QPainterPath SLGRect::shape() const
{
	QRectF rect = calculateOutlineRect();

	QPainterPath path;
	path.addRect(rect);
	// 	path.addRoundRect(rect, roundness(rect.width()),
	// 		roundness(rect.height()));
	return path;
}

void SLGRect::drawGraphic(QPainter* painter)
{
	painter->drawRect(calculateOutlineRect());
}

void SLGRect::paintSelectionOutline(QPainter* painter)
{
	painter->drawRect(calculateOutlineRect());
}

void SLGRect::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
	qDebug() << "On Mouse double click";
}

void SLGRect::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
	qDebug() << "On Mouse Move";
	SLIBasicGraphicItem::mouseMoveEvent(event);
}

int SLGRect::roundness(double size) const
{
	const int Diameter = 12;
	return 100 * Diameter / int(size);
}



int SLGRect::width()
{
	return _nWidth;
}

int SLGRect::height()
{
	return _nHeight;
}

void SLGRect::setWidth(int width)
{
	_nWidth = abs(width);
}

void SLGRect::setHeight(int height)
{
	_nHeight = abs(height);
}


QRectF SLGRect::calculateOutlineRect() const
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


void SLGRect::keyPressEvent(QKeyEvent* event)
{
	// 	if (event->modifiers() & Qt::ShiftModifier ||
	// 		event->modifiers() & Qt::ControlModifier) {
	// 		bool move = event->modifiers() & Qt::ControlModifier;
	// 		bool changed = true;
	// 		double dx1 = 0.0;
	// 		double dy1 = 0.0;
	// 		double dx2 = 0.0;
	// 		double dy2 = 0.0;
	// 		switch (event->key()) {
	// 		case Qt::Key_Left:
	// 			if (move)
	// 				dx1 = -1.0;
	// 			dx2 = -1.0;
	// 			break;
	// 		case Qt::Key_Right:
	// 			if (move)
	// 				dx1 = 1.0;
	// 			dx2 = 1.0;
	// 			break;
	// 		case Qt::Key_Up:
	// 			if (move)
	// 				dy1 = -1.0;
	// 			dy2 = -1.0;
	// 			break;
	// 		case Qt::Key_Down:
	// 			if (move)
	// 				dy1 = 1.0;
	// 			dy2 = 1.0;
	// 			break;
	// 		default:
	// 			changed = false;
	// 		}
	// 		if (changed) {
	// 			setRect(rect().adjusted(dx1, dy1, dx2, dy2));
	// 			event->accept();
	// 			
	// 			return;
	// 		}
	// 	}
	SLIBasicGraphicItem::keyPressEvent(event);
}

void SLGRect::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	SLIBasicGraphicItem::mousePressEvent(event);
}



void SLGRect::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
	SLIBasicGraphicItem::mouseReleaseEvent(event);
}





