#pragma once
#include <QGraphicsPathItem>

/// <summary>
/// A custom QGraphicsItem for drawing actor timelines in sequence diagrams.
/// This class handles both active and inactive segments of an actor's timeline,
/// including spans, connections between spans, and death glyphs.
/// </summary>
class SLGActorline : public QGraphicsItem
{
public:
    /// <summary>
    /// Constructs an SLGActorline item.
    /// </summary>
    /// <param name="useColor">Whether to use custom color instead of default styling</param>
    /// <param name="r">Red component of the custom color (0-255)</param>
    /// <param name="g">Green component of the custom color (0-255)</param>
    /// <param name="b">Blue component of the custom color (0-255)</param>
    /// <param name="parent">Parent graphics item</param>
    explicit SLGActorline(bool useColor = false, int r = 0, int g = 0, int b = 0, QGraphicsItem* parent = nullptr);

    /// <summary>
    /// Adds a horizontal span segment to the timeline.
    /// </summary>
    /// <param name="x1">Left x-coordinate of the span</param>
    /// <param name="x2">Right x-coordinate of the span</param>
    /// <param name="y">Y-coordinate (vertical position) of the span</param>
    void addSpan(int x1, int x2, int y);

    /// <summary>
    /// Adds a connection curve between two points on the timeline.
    /// </summary>
    /// <param name="fromX">Starting x-coordinate</param>
    /// <param name="fromY">Starting y-coordinate</param>
    /// <param name="toX">Ending x-coordinate</param>
    /// <param name="toY">Ending y-coordinate</param>
    /// <param name="group">Group identifier for connection styling</param>
    /// <param name="groupSeq">Sequence within group for control point calculation</param>
    /// <param name="isActive">Whether this is an active connection</param>
    void addConnection(int fromX, int fromY, int toX, int toY, int group = 0, int groupSeq = 0, bool isActive = true);

    /// <summary>
    /// Adds a death glyph (X mark) at the specified position.
    /// </summary>
    /// <param name="x">X-coordinate of death marker</param>
    /// <param name="y">Y-coordinate of death marker</param>
    /// <param name="size">Size of the death glyph</param>
    void addDeathGlyph(int x, int y, float size);

    /// <summary>
    /// Sets the custom color for this timeline.
    /// </summary>
    /// <param name="color">QColor to use for drawing</param>
    void setColor(const QColor& color);

    /// <summary>
    /// Gets the current custom color of this timeline.
    /// </summary>
    /// <returns>Current QColor used for drawing</returns>
    QColor color() const { return _color; }

    // QGraphicsItem interface
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

private:
    /// <summary>
    /// Calculates the control point for a connection curve.
    /// </summary>
    /// <param name="fromX">Starting x-coordinate</param>
    /// <param name="fromY">Starting y-coordinate</param>
    /// <param name="toX">Ending x-coordinate</param>
    /// <param name="toY">Ending y-coordinate</param>
    /// <param name="group">Group identifier</param>
    /// <param name="groupSeq">Sequence within group</param>
    /// <returns>QPointF representing the Bezier control point</returns>
    QPointF calculateControlPoint(int fromX, int fromY, int toX, int toY, int group, int groupSeq) const;

    QPainterPath _activePath;    // Path for active segments of the timeline
    QPainterPath _inactivePath;  // Path for inactive segments of the timeline
    bool _bUseColor;             // Flag indicating whether to use custom color
    QColor _color;               // Custom color for this timeline
    float _lineWidthA = 3.0f;    // Line width for active segments
    float _lineWidthI = 1.0f;    // Line width for inactive segments
};