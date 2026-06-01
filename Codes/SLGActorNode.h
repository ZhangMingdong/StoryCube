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
class SLGActorNode : public SLIBasicGraphicItem
{
	SLGActorNode();
public:
	static SLGActorNode* CreateSLGActorNode();
public:
	virtual ~SLGActorNode();
protected:
	QRectF calculateOutlineRect() const;
	virtual void drawGraphic(QPainter* painter);
	void paintSelectionOutline(QPainter* painter);
	// paint when selected
	virtual void paintSelected(QPainter* painter);
public:
	void setRadius(double dbR);
private:
	QRectF boundingRect() const;
	QPainterPath shape() const;
private:
	const double c_dbMinR = 20.0;
	// radius of the circle
	double _dbRadius = c_dbMinR;
	// weight of the nodes, 0-1;
	double _dbWeight = 0;

	// cued by selection in other views
	bool _bCued = false;

public:
	/*
	* Set the display weight
	* 0-1
	* used to set the fill color
	*/
	void SetDisplayWeight(double dbW) { _dbWeight = dbW; }

	// set cued node 
	void SetCued(bool bCued) {_bCued = bCued;}

};

