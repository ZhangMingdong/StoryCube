#pragma once
#include <QGraphicsItem>
#include <QPen>
#include <QBrush>

// Forward declaration to reduce header dependencies
class SLGLink;

/**
 * @brief Abstract base class for all graphic items in the SLI (Scene Link Interface) system.
 *
 * Provides common functionality for drawing, selection, and link management.
 * Derived classes must implement pure virtual methods for custom drawing behavior.
 */
class SLIBasicGraphicItem : public QObject, public QGraphicsItem
{
    Q_OBJECT // Enable Qt's meta-object system if signals/slots are needed

public:
    /**
     * @brief Enumeration of possible graphic item types.
     *
     * Using enum class for better type safety and scoping.
     */
    enum class Type {
        None,         ///< Undefined type
        Location,     ///< Represents a physical location
        LocationBar,  ///< Visual bar element for locations
        Session,      ///< Connection session between elements
        Actor         ///< Represents an active entity
    };

    // ========== Construction/Destruction ==========
    explicit SLIBasicGraphicItem(QObject* parent = nullptr);
    virtual ~SLIBasicGraphicItem() override;

    // ========== Public Interface ==========
    /**
     * @brief Gets the custom data associated with this item.
     * @return Data as QStringView to avoid unnecessary copying
     */
    QStringView getData() const noexcept { return _dataId; }

    /**
     * @brief Sets custom data for this item.
     * @param data New data to store (passed by const reference for efficiency)
     */
    void setData(const QString& data) { _dataId = data; }

    /**
     * @brief Gets the item type.
     * @return Type enum value
     */
    Type getType() const noexcept { return _type; }

    // Pen/brush management
    void setPen(const QPen& pen) noexcept;
    void setBrush(const QBrush& brush) noexcept;


    /**
    * @brief Calculates connection point for links.
    * @param targetPoint The target point to connect to
    * @return Optimal connection position on this item
    */
    virtual QPointF getLinkPosition(QPointF targetPoint) const;


protected:
    // ========== Core Drawing Functionality ==========
    /**
     * @brief Pure virtual function for custom drawing.
     * @param painter The painter to use for drawing
     */
    virtual void drawGraphic(QPainter* painter) = 0;

    /**
     * @brief Pure virtual function for drawing selection outline.
     * @param painter The painter to use for drawing
     */
    virtual void paintSelectionOutline(QPainter* painter) = 0;

    /**
     * @brief Virtual function for additional selected state painting.
     * @param painter The painter to use for drawing
     */
    virtual void paintSelected(QPainter* painter);

    // Overridden from QGraphicsItem
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

    /**
     * @brief Calculates roundness for graphical elements
     * @param size The base size to calculate from
     * @return The calculated roundness value
     */
    int roundness(double size) const;

    // ========== Link Management ==========
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;



    // ========== Protected Members ==========
    Type _type = Type::None;  ///< Current item type
    QPen _pen;                ///< Current pen style
    QBrush _brush;            ///< Current brush style

private:
    // ========== Private Helpers ==========
    /**
     * @brief Calculates the bounding rectangle for the item.
     * @return The outline rectangle
     */
    virtual QRectF calculateOutlineRect() const;

    // ========== Private Data ==========
    QString _dataId;  ///< Custom data storage
};