#include "MyStorylineView.h"
#include <QScrollBar>
#include <QPainter>
#include <cmath>

MyStorylineView::MyStorylineView(QWidget* parent)
    : QGraphicsView(parent)
{
    setRenderHint(QPainter::Antialiasing);
    setDragMode(QGraphicsView::ScrollHandDrag);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setOptimizationFlags(QGraphicsView::DontSavePainterState);
    //setBackgroundBrush(Qt::lightGray);
}

void MyStorylineView::wheelEvent(QWheelEvent* event)
{
    const double zoomFactor = std::pow(4.0 / 3.0, (-event->angleDelta().y() / 240.0));
    zoom(zoomFactor, event->position());
}

void MyStorylineView::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::MiddleButton) {
        m_panning = true;
        m_lastPanPoint = event->pos();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
        return;
    }
    QGraphicsView::mousePressEvent(event);
}

void MyStorylineView::mouseMoveEvent(QMouseEvent* event)
{
    if (m_panning) {
        QScrollBar* hBar = horizontalScrollBar();
        QScrollBar* vBar = verticalScrollBar();
        QPoint delta = event->pos() - m_lastPanPoint;
        hBar->setValue(hBar->value() - delta.x());
        vBar->setValue(vBar->value() - delta.y());
        m_lastPanPoint = event->pos();
        event->accept();
        return;
    }
    QGraphicsView::mouseMoveEvent(event);
}

void MyStorylineView::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::MiddleButton) {
        m_panning = false;
        setCursor(Qt::ArrowCursor);
        event->accept();
        return;
    }
    QGraphicsView::mouseReleaseEvent(event);
}

void MyStorylineView::drawBackground(QPainter* painter, const QRectF& rect)
{
    QGraphicsView::drawBackground(painter, rect);

    // Draw grid pattern
    painter->setPen(QPen(Qt::gray, 0.2));
    const qreal gridSize = 20 * m_currentScale;

    qreal left = int(rect.left()) - (int(rect.left()) % int(gridSize));
    qreal top = int(rect.top()) - (int(rect.top()) % int(gridSize));

    for (qreal x = left; x < rect.right(); x += gridSize) {
        painter->drawLine(QPointF(x, rect.top()), QPointF(x, rect.bottom()));
    }
    for (qreal y = top; y < rect.bottom(); y += gridSize) {
        painter->drawLine(QPointF(rect.left(), y), QPointF(rect.right(), y));
    }
}

void MyStorylineView::zoom(double factor, const QPointF& centerPoint)
{
    const double minScale = 0.1;
    const double maxScale = 10.0;

    double newScale = m_currentScale * factor;
    newScale = qBound(minScale, newScale, maxScale);

    if (qAbs(newScale - m_currentScale) > 0.0001) {
        scale(newScale / m_currentScale, newScale / m_currentScale);
        m_currentScale = newScale;
//        emit scaleChanged(m_currentScale);
    }
}

void MyStorylineView::fitContent()
{
    if (scene()) {
        fitInView(scene()->itemsBoundingRect(), Qt::KeepAspectRatio);
        m_currentScale = transform().m11();
    }
}