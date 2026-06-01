#pragma once
#include <QFont>
#include "SLIBasicGraphicItem.h"

/**
 * @class SLGSession
 * @brief Represents a session item in the session scene.
 *
 * This class inherits from SLIBasicGraphicItem and provides specialized functionality
 * for displaying and managing session items in a graphics scene, including text display
 * and weight management.
 */
class SLGSession : public SLIBasicGraphicItem
{
public:
    /**
     * @brief Default constructor
     */
    SLGSession();

    /**
     * @brief Virtual destructor
     */
    virtual ~SLGSession();

public:
    // Text management
    /**
     * @brief Sets the display text for this session
     * @param text The text to display
     */
    void setText(const QString& text);

    /**
     * @brief Gets the current display text
     * @return Current display text
     */
    QString text() const;

    /**
     * @brief Alias for setText
     * @param strText The text to display
     */
    void SetText(QString strText) { _strLabel = strText; }

    // Graphic properties
    /**
     * @brief Gets the bounding rectangle of the item
     * @return The bounding rectangle
     */
    QRectF boundingRect() const override;

    /**
     * @brief Gets the shape of the item for collision detection
     * @return The shape as a QPainterPath
     */
    QPainterPath shape() const override;

    // Weight management
    /**
     * @brief Sets the display weight value
     * @param dbW The weight value to display
     */
    void SetDisplayWeight(double dbW);

    /**
     * @brief Sets the original weight value
     * @param nWO The original weight value
     */
    void SetWeightO(int nWO);

    // Link management
    void addLink(SLGLink* link);
    void removeLink(SLGLink* link);
    void removeAllLinks();
protected:
    /**
     * @brief Draws the main graphic representation
     * @param painter The painter to use for drawing
     */
    void drawGraphic(QPainter* painter) override;

    /**
     * @brief Draws the selection outline
     * @param painter The painter to use for drawing
     */
    void paintSelectionOutline(QPainter* painter) override;

    /**
     * @brief Calculates the outline rectangle
     * @return The outline rectangle
     */
    QRectF calculateOutlineRect() const;



    /**
     * @brief Updates all connected links.
     */
    void updateLinks();


    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
private:
    // Display properties
    QString _strLabel;   ///< The text label displayed for this session
    QFont _font;         ///< The font used for text rendering

    // Weight properties
    int _nWO = 1;        ///< Original weight value
    double _dbW1;        ///< Calculated/displayed weight value
    QSet<SLGLink*> _links;    ///< Set of connected links
};