
#include <QtGui>


#include "SLGText.h"

#include <QStyleOptionGraphicsItem>
#include <QStyle>
#include <QGraphicsSceneMouseEvent>
#include <QLineEdit>
#include <QColorDialog>
#include <QInputDialog>


void SLGText::AlignLeft() {
	_align = align_l;
}
SLGText* SLGText::CreateSLGText(SLIBasicGraphicItem::Type type) {
	SLGText* pInstance = new SLGText();
	pInstance->_type = type;
	switch (type)
	{
	case SLGText::Type::Actor:
		pInstance->_align = SLGText::align_r;
		break;
	case SLGText::Type::Location:
		pInstance->_align = align_l;
		pInstance->setFlags(ItemIsMovable | ItemIsSelectable); 
		{
			QFont f = QFont("helvetica");
			f.setPointSizeF(16);
			pInstance->SetFont(f);
		}
		break;
	case SLGText::Type::Session:
		break;
	default:
		break;
	}
	return pInstance;
}

SLGText::SLGText() :SLIBasicGraphicItem()
{
// 	setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsGeometryChanges);
//	setFlags(ItemIsSelectable);
	_font = qApp->font();
}

SLGText::~SLGText()
{
}

void SLGText::setText(const QString &text)
{
	prepareGeometryChange();
	_text = text;
	update();
}

QString SLGText::text() const
{
	return _text;
}

QRectF SLGText::boundingRect() const
{
	const int Margin = 1;
	return calculateOutlineRect().adjusted(-Margin, -Margin, +Margin, +Margin);
}

QPainterPath SLGText::shape() const
{
	QRectF rect = calculateOutlineRect();

	QPainterPath path;
	path.addRoundedRect(rect, roundness(rect.width()),
		roundness(rect.height()));
	return path;
}

void SLGText::drawGraphic(QPainter *painter)
{
	const int Padding = 8;
	QFontMetricsF metrics = QFontMetricsF(_font);
	QRectF rect = metrics.boundingRect(_text);

	rect.adjust(-Padding, -Padding, +Padding, +Padding);
	rect.translate(-rect.center());


	painter->setFont(_font);
	switch (_align)
	{
	case SLGText::align_l:
		rect.translate(QPointF(rect.width() / 2.0, 0));
		painter->drawText(rect, Qt::AlignVCenter|Qt::AlignLeft, _text);
		break;
	case SLGText::align_m:
		painter->drawText(rect, Qt::AlignCenter, _text);
		break;
	case SLGText::align_r:
		rect.translate(-QPointF(rect.width() / 2.0, 0));
		painter->drawText(rect, Qt::AlignVCenter|Qt::AlignRight, _text);
		break;
	default:
		painter->drawText(rect, Qt::AlignCenter, _text);
		break;
	}
//	paintSelectionOutline(painter);
}

void SLGText::paintSelectionOutline(QPainter *painter)
{
	QRectF rect = calculateOutlineRect();
	//painter->drawRoundedRect(rect, roundness(rect.width()),	roundness(rect.height()));
	painter->drawRect(rect);
}

void SLGText::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
	QString text = QInputDialog::getText(event->widget(),
		tr("Edit Text"), tr("Enter new text:"),
		QLineEdit::Normal, _text);
	if (!text.isEmpty())
		setText(text);
}

QRectF SLGText::calculateOutlineRect() const
{
	const int Padding = 8;
//	QFontMetricsF metrics = QFontMetricsF(qApp->font());
	QFontMetricsF metrics = QFontMetricsF(_font);
	QRectF rect = metrics.boundingRect(_text);
	//rect.adjust(-Padding, -Padding, +Padding, +Padding);

	QPointF ptCenter = rect.center();
	switch (_align)
	{
	case SLGText::align_l:
		rect.translate(0, -ptCenter.y());
		break;
	case SLGText::align_m:
		rect.translate(-ptCenter.x(),-ptCenter.y());
		break;
	case SLGText::align_r:
		rect.translate(-2*ptCenter.x(), -ptCenter.y());
		break;
	default:
		break;
	}
	return rect;
}

int SLGText::roundness(double size) const
{
	const int Diameter = 12;
	return 100 * Diameter / int(size);
}

void SLGText::SetFont(QFont f) {
	_font = f;
}

void SLGText::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
	qDebug() << "void SLGText::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {";
	QGraphicsItem::mouseReleaseEvent(event);
	if (this->_type==Type::Location)
	{
		emit MoveLocation();
	}
}