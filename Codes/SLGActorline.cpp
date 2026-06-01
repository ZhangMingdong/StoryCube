#include "SLGActorline.h"
#include <QPen>
#include <QPainter>


SLGActorline::SLGActorline(bool useColor,int r, int g, int b,
    QGraphicsItem* parent)
    : QGraphicsItem(parent),
    _bUseColor(useColor),
    _color(r, g, b)
{
    // 设置初始属性
    setFlag(QGraphicsItem::ItemIsSelectable, false);
    setFlag(QGraphicsItem::ItemIsFocusable, false);
}

void SLGActorline::addSpan(int x1, int x2, int y)
{
    _activePath.moveTo(x1, y);
    _activePath.lineTo(x2, y);
}

void SLGActorline::addConnection(int fromX, int fromY, int toX, int toY, int group, int groupSeq, bool isActive)
{
    QPainterPath& path = isActive ? _activePath : _inactivePath;

    if (path.isEmpty()) {
        path.moveTo(fromX, fromY);
    }

    QPointF ctrlPt = calculateControlPoint(fromX, fromY, toX, toY, group, groupSeq);
    path.cubicTo(ctrlPt.x(), fromY, ctrlPt.x(), toY, toX, toY);
}

void SLGActorline::addDeathGlyph(int x, int y, float size)
{
    float halfSize = size / 2.0f;
    _activePath.moveTo(x - halfSize, y - halfSize);
    _activePath.lineTo(x + halfSize, y + halfSize);
    _activePath.moveTo(x - halfSize, y + halfSize);
    _activePath.lineTo(x + halfSize, y - halfSize);
    _activePath.moveTo(x, y);  // 恢复绘图位置
}

void SLGActorline::setColor(const QColor& color)
{
    _color = color;
    update();
}

QRectF SLGActorline::boundingRect() const
{
    QRectF activeRect = _activePath.boundingRect();
    QRectF inactiveRect = _inactivePath.boundingRect();
    return activeRect.united(inactiveRect).adjusted(-2, -2, 2, 2);
}

void SLGActorline::paint(QPainter* painter,
    const QStyleOptionGraphicsItem* option,
    QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QPen pen;

    // 绘制非活跃路径（灰色虚线）
    if (!_inactivePath.isEmpty()) {
        pen.setWidthF(_lineWidthI);
        pen.setColor(Qt::gray);
        pen.setStyle(Qt::DashLine);
        painter->setPen(pen);
        painter->drawPath(_inactivePath);
    }

    // 绘制活跃路径
    if (!_activePath.isEmpty()) {
        pen.setWidthF(_lineWidthA);
        pen.setStyle(Qt::SolidLine);
        pen.setColor(_bUseColor ? _color : Qt::black);
        painter->setPen(pen);
        painter->setBrush(Qt::NoBrush);
        painter->drawPath(_activePath);
    }
}

QPointF SLGActorline::calculateControlPoint(int fromX, int fromY,
    int toX, int toY,
    int group, int groupSeq) const
{
    Q_UNUSED(fromY);
    Q_UNUSED(toY);

    double midX = (fromX + toX) / 2.0;

    if (group > 0) {
        double space = (toX - fromX) / (group + 1.0);
        if (toY < fromY) {
            midX = fromX + (groupSeq + 1.0) * space;
        }
        else {
            midX = fromX + (group - groupSeq) * space;
        }
    }

    return QPointF(midX, midX);  // y值不会被使用
}
