#pragma once
#include <QGraphicsView>
#include <QWheelEvent>

/**
 * @brief Custom graphics view for storyline visualization with enhanced interaction capabilities
 */
class MyStorylineView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit MyStorylineView(QWidget* parent = nullptr);
    ~MyStorylineView() = default;

protected:
    // Interaction overrides
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void drawBackground(QPainter* painter, const QRectF& rect) override;

    // View manipulation
    void zoom(double factor, const QPointF& centerPoint = QPointF());
    void fitContent();

private:
    bool m_panning = false;
    QPoint m_lastPanPoint;
    double m_currentScale = 1.0;
};