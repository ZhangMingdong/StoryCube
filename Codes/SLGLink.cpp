#include "SLGLink.h"
#include "SLGSession.h"
#include <QPainterPath>
#include <QStyleOptionGraphicsItem>

SLGLink::SLGLink(SLGSession* fromNode, SLGSession* toNode, bool bSpline)
    : m_pNodeFrom(fromNode),
    m_pNodeTo(toNode),
    _bSpline(bSpline)
{
    // Register this link with both nodes
    m_pNodeFrom->addLink(this);
    m_pNodeTo->addLink(this);

    // Configure graphics item properties
    //setFlags(ItemIsSelectable);
    setZValue(-1);  // Draw behind nodes
    setColor(Qt::darkRed);

    // Create initial path
    trackNodes();
}

SLGLink::~SLGLink()
{
    // Note: Node removal commented out to prevent double deletion
    // If needed, uncomment and handle in scene cleanup
    // m_pNodeFrom->removeLink(this);
    // m_pNodeTo->removeLink(this);
}

SLGSession* SLGLink::fromNode() const
{
    return m_pNodeFrom;
}

SLGSession* SLGLink::toNode() const
{
    return m_pNodeTo;
}

void SLGLink::setFromNode(SLGSession* node)
{
    if (m_pNodeFrom != node) {
        if (m_pNodeFrom) {
            m_pNodeFrom->removeLink(this);
        }
        m_pNodeFrom = node;
        if (m_pNodeFrom) {
            m_pNodeFrom->addLink(this);
        }
        trackNodes();
    }
}

void SLGLink::setToNode(SLGSession* node)
{
    if (m_pNodeTo != node) {
        if (m_pNodeTo) {
            m_pNodeTo->removeLink(this);
        }
        m_pNodeTo = node;
        if (m_pNodeTo) {
            m_pNodeTo->addLink(this);
        }
        trackNodes();
    }
}

void SLGLink::setColor(const QColor& color)
{
    setPen(QPen(color, 1.0));
}

QColor SLGLink::color() const
{
    return pen().color();
}

void SLGLink::trackNodes()
{
    QPainterPath path;
    const QPointF startCenter = m_pNodeFrom->pos();
    const QPointF endCenter = m_pNodeTo->pos();

    if (_bSpline) {
        // Create curved spline path
        const QPointF startPoint = m_pNodeFrom->getLinkPosition(endCenter);
        const QPointF endPoint = m_pNodeTo->getLinkPosition(startCenter);

        path.moveTo(startPoint);
        path.cubicTo(
            QPointF((startPoint.x() + endPoint.x()) / 2.0, startPoint.y()),
            QPointF((startPoint.x() + endPoint.x()) / 2.0, endPoint.y()),
            endPoint
        );
    }
    else {
        // Create straight line path with single right-angle bend
        const QPointF midPoint(endCenter.x(), startCenter.y());
        const QPointF startPoint = m_pNodeFrom->getLinkPosition(midPoint);
        const QPointF endPoint = m_pNodeTo->getLinkPosition(midPoint);

        path.moveTo(startPoint);
        path.lineTo(midPoint);
        path.lineTo(endPoint);
    }

    setPath(path);
}