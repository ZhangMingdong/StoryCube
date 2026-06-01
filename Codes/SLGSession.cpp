#include <QtGui>
#include "SLGSession.h"
#include "SLGLink.h"

/**
 * @brief Constructs a SLGSession item with default settings
 */
SLGSession::SLGSession() : SLIBasicGraphicItem()
{
    // Configure selectable and movable behavior
    setFlags(ItemIsSelectable | ItemSendsGeometryChanges);
    //setFlags(ItemIsSelectable | ItemSendsGeometryChanges | ItemIsMovable);

    // Set default font
    _font = QFont("Consolas", 9);
}

SLGSession::~SLGSession()
{
    // Destructor implementation
}

// ================= Text Management =================

/**
 * @brief Sets the display text and triggers visual update
 * @param text The text to display
 */
void SLGSession::setText(const QString& text)
{
    prepareGeometryChange();
    _strLabel = text;
    update();
}

/**
 * @brief Gets the current display text
 * @return Current text content
 */
QString SLGSession::text() const
{
    return _strLabel;
}

// ================= Geometry & Shape =================

/**
 * @brief Calculates the bounding rectangle with margin
 * @return Expanded bounding rectangle
 */
QRectF SLGSession::boundingRect() const
{
    const int Margin = 1;
    return calculateOutlineRect().adjusted(-Margin, -Margin, +Margin, +Margin);
}

/**
 * @brief Defines the clickable shape of the item
 * @return Rounded rectangle path
 */
QPainterPath SLGSession::shape() const
{
    QRectF rect = calculateOutlineRect();
    QPainterPath path;
    path.addRoundedRect(rect, roundness(rect.width()), roundness(rect.height()));
    return path;
}

/**
 * @brief Calculates the base outline rectangle
 * @return Text-centered rectangle with padding
 */
QRectF SLGSession::calculateOutlineRect() const
{
    const int Padding = 8;
    QFontMetricsF metrics(_font);
    QRectF rect = metrics.boundingRect(_strLabel);
    rect.adjust(-Padding, -Padding, +Padding, +Padding);
    rect.translate(-rect.center());  // Center the rectangle
    return rect;
}


// ================= Drawing Methods =================

/**
 * @brief Main drawing routine for the session item
 * @param painter Qt painter object
 */
void SLGSession::drawGraphic(QPainter* painter)
{
    const int Padding = 4;

    // Get base rectangle and adjusted content rectangle
    QRectF rectBG = calculateOutlineRect();
    QRectF rect = calculateOutlineRect();
    rect.adjust(Padding, Padding, -Padding, -Padding);

    // Draw background
    painter->setBrush(QColor(255, 255, 255));
    painter->drawRoundedRect(rectBG, roundness(rectBG.width()),
        roundness(rectBG.height()));

    // Draw weight indicator pie
    painter->setBrush(QColor(50, 50, 50));
    painter->drawPie(rectBG, 0, _nWO * 16 / 10 * 360);

    // Draw content area and text
    painter->setFont(_font);
    painter->setBrush(_brush);
    painter->drawRoundedRect(rect, roundness(rect.width()),
        roundness(rect.height()));
    painter->drawText(rect, Qt::AlignCenter, _strLabel);
}

/**
 * @brief Draws selection highlight outline
 * @param painter Qt painter object
 */
void SLGSession::paintSelectionOutline(QPainter* painter)
{
    QRectF rect = calculateOutlineRect();
    painter->drawRoundedRect(rect, roundness(rect.width()),
        roundness(rect.height()));
}

// ================= Weight Management =================

/**
 * @brief Sets the visual weight representation
 * @param dbW Weight value (0.0-1.0)
 */
void SLGSession::SetDisplayWeight(double dbW)
{
    _dbW1 = dbW;

    // Calculate color based on weight
    int nColor = (1.0 - dbW) * 255;
    QColor fillColor = (dbW > 0) ? QColor(255, nColor, nColor)
        : QColor(128, 128, 128);

    setBrush(fillColor);
}

/**
 * @brief Sets the original weight value
 * @param nWO Integer weight value
 */
void SLGSession::SetWeightO(int nWO)
{
    _nWO = nWO;
}

// ================= Links ========================


void SLGSession::addLink(SLGLink* link)
{
    _links.insert(link);
}

void SLGSession::removeLink(SLGLink* link)
{
    _links.remove(link);
}

void SLGSession::removeAllLinks() {
    while (!_links.empty())
    {
        delete* (_links.begin());
    }
}

void SLGSession::updateLinks() {
    for (SLGLink* link : _links)
        link->trackNodes();
}

QVariant SLGSession::itemChange(GraphicsItemChange change, const QVariant& value) {
    if (change == ItemPositionHasChanged) {
        updateLinks();
    }
    return QGraphicsItem::itemChange(change, value);
}
