#include "SLIBasicGraphicItem.h"
#include "SLGLink.h"


#include <QPainter>
#include <QStyleOptionGraphicsItem>

#include <QDebug>



SLIBasicGraphicItem::SLIBasicGraphicItem(QObject* parent) : QGraphicsItem()
{
//	setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsGeometryChanges);
}

SLIBasicGraphicItem::~SLIBasicGraphicItem()
{
}



void SLIBasicGraphicItem::setPen(const QPen& pen_) noexcept
{
	_pen = pen_;
}

void SLIBasicGraphicItem::setBrush(const QBrush& brush_) noexcept
{
	_brush = brush_;
}


void SLIBasicGraphicItem::paintSelected(QPainter* painter) {
	QPen pen(Qt::DashLine);
	pen.setWidth(2);
	pen.setColor(Qt::black);
	painter->setPen(pen);
	painter->setBrush(Qt::NoBrush);
	paintSelectionOutline(painter);
}

void SLIBasicGraphicItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget * /* widget */)
{

	painter->setPen(_pen);
	painter->setBrush(_brush);

	//qDebug() << m_brush;

	drawGraphic(painter);

	if (option->state & QStyle::State_Selected) {
		paintSelected(painter);
	}
}


QRectF SLIBasicGraphicItem::calculateOutlineRect() const
{
	return QRectF();
}


/**
 * @brief Calculates corner roundness based on size
 * @param size Reference dimension for calculation
 * @return Roundness percentage
 */
int SLIBasicGraphicItem::roundness(double size) const
{
    const int Diameter = 12;
    return 100 * Diameter / int(size);
}

QVariant SLIBasicGraphicItem::itemChange(GraphicsItemChange change, const QVariant& value) {
	/*
	if (change == ItemPositionHasChanged) {
		updateLinks();
	}
	if (change == QGraphicsItem::ItemPositionChange ||
		change == QGraphicsItem::ItemPositionHasChanged ||
		change == QGraphicsItem::ItemTransformChange ||
		change == QGraphicsItem::ItemTransformHasChanged)
	{

	}
	*/
	return QGraphicsItem::itemChange(change, value);
}


QPointF SLIBasicGraphicItem::getLinkPosition(QPointF ptTarget) const {
    QRectF rect = this->calculateOutlineRect();
    QPointF ptCenter = this->pos();

    if (ptTarget.x() < ptCenter.x())
    {
        return QPointF(ptCenter.x() - rect.width() / 2.0, ptCenter.y());
    }
    else if (ptTarget.x() > ptCenter.x()) {
        return QPointF(ptCenter.x() + rect.width() / 2.0, ptCenter.y());
    }
    else if (ptTarget.y() < ptCenter.y()) {
        return QPointF(ptCenter.x(), ptCenter.y() - rect.height() / 2.0);
    }
    else {
        return QPointF(ptCenter.x(), ptCenter.y() + rect.height() / 2.0);
    }


    QPointF pt[4];
    qreal len[4];
    pt[0] = QPointF(ptCenter.x(), ptCenter.y() - rect.height() / 2.0);
    pt[1] = QPointF(ptCenter.x() - rect.width() / 2.0, ptCenter.y());
    pt[2] = QPointF(ptCenter.x(), ptCenter.y() + rect.height() / 2.0);
    pt[3] = QPointF(ptCenter.x() + rect.width() / 2.0, ptCenter.y());

    len[0] = QLineF(pt[0], ptTarget).length();
    len[1] = QLineF(pt[1], ptTarget).length();
    len[2] = QLineF(pt[2], ptTarget).length();
    len[3] = QLineF(pt[3], ptTarget).length();
    int min = 0;
    for (int i = 1; i < 4; i++)
    {
        if (len[i] < len[min])
        {
            min = i;
        }
    }


    return pt[min];
}