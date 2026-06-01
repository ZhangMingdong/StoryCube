#pragma once
#include <QApplication>
#include <QColor>
#include <QSet>

#include "SLIBasicGraphicItem.h"

/// <summary>
/// flow in the storymap view
/// </summary>
class SLGFlow : public SLIBasicGraphicItem
{
	SLGFlow();
public:
	static SLGFlow* CreateSLGFlow(SLIBasicGraphicItem* item1, SLIBasicGraphicItem* item2, int nW);
public:
	virtual ~SLGFlow();
protected:
	int _nWidth;
	int _nHeight;
	SLIBasicGraphicItem* _pItem1;
	SLIBasicGraphicItem* _pItem2;
	int _nW = 0;	// weight of the flow
protected:
	QRectF calculateOutlineRect() const;
	virtual void drawGraphic(QPainter* painter);
	void paintSelectionOutline(QPainter* painter);
	// paint when selected
	virtual void paintSelected(QPainter* painter);
private:
	// this function define the region for update when this item changed
	QRectF boundingRect() const;
	QPainterPath shape() const;
};

