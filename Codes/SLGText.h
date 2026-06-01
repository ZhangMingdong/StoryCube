#pragma once
#include <QApplication>
#include <QColor>
#include <QSet>
#include <QFont>
#include "SLIBasicGraphicItem.h"




/// <summary>
/// text graphics for storyline
/// used for:
///		actor names
///		location names
///		session names
/// </summary>
class SLGText :public SLIBasicGraphicItem
{
	Q_OBJECT

	SLGText();

public:
	static SLGText* CreateSLGText(SLIBasicGraphicItem::Type type);

	virtual ~SLGText();

	void setText(const QString &text);
	QString text() const;

	QRectF boundingRect() const;
	QPainterPath shape() const;
	void SetText(QString strText){ _text = strText; }
	void SetFont(QFont f);
protected:
	virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
	virtual void drawGraphic(QPainter *painter);
	virtual void paintSelectionOutline(QPainter *painter); 
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
private:
	QRectF calculateOutlineRect() const;
	int roundness(double size) const;
	QString _text;
	QFont _font;
	// text alignment
	enum enumAligh{
		align_l,
		align_m,
		align_r
	} _align=align_m;
signals:
	void MoveLocation();
public:
	void AlignLeft();
};

