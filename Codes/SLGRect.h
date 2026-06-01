#pragma once
#include <QApplication>
#include <QColor>
#include <QSet>

#include "SLIBasicGraphicItem.h"

/// <summary>
/// rectangle for storyline
/// used for
///  storyline view
///		location
///		location bar
///		session rectangle
///  pc view
///		nodes
/// </summary>
class SLGRect : public SLIBasicGraphicItem
{
	SLGRect();
public:
	static SLGRect* CreateSLGRect(SLIBasicGraphicItem::Type type);
public:
	virtual ~SLGRect();
protected:
	int _nWidth;
	int _nHeight;
protected:
	QRectF calculateOutlineRect() const;
	void keyPressEvent(QKeyEvent* event);
	void mousePressEvent(QGraphicsSceneMouseEvent* event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);
	void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
	virtual void drawGraphic(QPainter* painter);
	void paintSelectionOutline(QPainter* painter);
public:
	int width();
	int height();
	void setWidth(int width);
	void setHeight(int height);
private:
	int roundness(double size) const;
	QRectF boundingRect() const;
	QPainterPath shape() const;
};

