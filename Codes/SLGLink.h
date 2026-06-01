#pragma once

#include <QGraphicsPathItem>
#include <QColor>

// Forward declaration
class SLGSession;

/**
 * @brief Represents a connection link between two session nodes in the scene.
 *
 * The link can be drawn as either a straight line or a curved spline.
 * It automatically tracks the positions of connected nodes and updates its path.
 */
class SLGLink : public QObject, public QGraphicsPathItem
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a link between two session nodes
     * @param fromNode Source node of the link
     * @param toNode Target node of the link
     * @param bSpline If true, draws as curved spline; if false, as straight lines
     */
    SLGLink(SLGSession* fromNode, SLGSession* toNode, bool bSpline = true);

    ~SLGLink();

    // Node accessors
    SLGSession* fromNode() const;
    SLGSession* toNode() const;

    // Node mutators
    void setFromNode(SLGSession* node);
    void setToNode(SLGSession* node);

    // Appearance control
    void setColor(const QColor& color);
    QColor color() const;

    /**
     * @brief Updates the link path to follow current node positions
     */
    void trackNodes();

protected:
    SLGSession* m_pNodeFrom;  ///< Source node of the connection
    SLGSession* m_pNodeTo;    ///< Target node of the connection
    bool _bSpline = true;     ///< Flag for spline vs straight line drawing

private:
    // Disable copy/assignment
    SLGLink(const SLGLink&) = delete;
    SLGLink& operator=(const SLGLink&) = delete;
};