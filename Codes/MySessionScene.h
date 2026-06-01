#pragma once
#include <QGraphicsScene>
#include <QMap>

class SLData;
class SLGSession;
class SLOperator;


class MySessionScene : public QGraphicsScene
{
	Q_OBJECT

public:
	MySessionScene(qreal x, qreal y, qreal width, qreal height);
	~MySessionScene();
public:
	// generate the scene using the storyline
	void GenerateScene(SLData* pData, SLOperator* pOperator);
	// update scene
	void UpdateScene();
private:
	int _nSpaceX = 80;
	int _nSpaceY = 33;
	int _nMovingSteps = 5;
private:
	SLData* _pSL = NULL;
	SLOperator* _pSLO = NULL;

	QMap<QString, SLGSession*> _mapSessions;	// map of the sessions


private slots:
	void onUpdateMovingSteps(int nW);
	void onSelectionChanged();
signals:
	// tell storyline scene to update storyline
	void UpdateStoryline();

	/*
	* tell actor view to update
	* called when location weight or session weight is modified
	*/
	void UpdateActorView();

	// selected a scene
	void SelectSession(QString strID);
	void UpdateTips();
private:
	// set position for a node
	void setNodePos(SLGSession* pNode, int nT, int nSeq);
	// move locations according to the given nRow
	void moveLocations(int nRow);
	// clear the session scene
	void clearScene();
	// generate the graphics
	void generateGraphics(QList<QGraphicsItem*>& listSessions, QList<QGraphicsItem*>& listLinks, QMap<QString, SLGSession*>& mapSessions);

public:
	// update the session weight
	void UpdateSessionWeight();
};


