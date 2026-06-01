#pragma once
#include <QGraphicsScene>
#include <QMap>

class SLData;
class SLGSession;
class SLOperator;
class SLGFlow;
class SLGRect;



class MyPCScene : public QGraphicsScene
{
	Q_OBJECT

public:
	MyPCScene(qreal x, qreal y, qreal width, qreal height);
	~MyPCScene();
public:
	/*
	* generate the scene using the storyline
	*/
	void GenerateScene(SLData* pData, SLOperator* pOperator);
	/*
	* update scene
	*/
	void UpdateScene();
protected:

	// clear the session scene
	void clearScene();

	/*
	* generate the scene
	* derived classes implementation
	*/
	virtual void generateScene(int nScale) = 0;

	// update SceneRect according to visible items
	void updateSceneRect();
private slots:
	void onSelectionChanged();

	void onFlowVisibilityChanged(int);
	void onStatVisibilityChanged(int);
	void onChangeScale(int);
protected:
	SLData* _pSL = NULL;
	SLOperator* _pSLO = NULL;

	bool _bO = false;				// if the view show the original data

	bool _bFlowVisibilityChanged = true;			// show the flow
	bool _bStatVisibilityChanged = true;			// show the statistics
	QList<SLGFlow*> _listFlow;		// list of the lfow, used for control visibility
	QList<SLGRect*> _listStat;		// list of the statistic bars, sued for control visibility
	int _nPjkScale = 1;

signals:
	// selected text: location, actor, or scene
	void UpdateTips();
public:
	const int c_nPjkScale = 10;
private:

	void handleSingleSelection(QGraphicsItem* item);
	void handleMultiSelection(const QList<QGraphicsItem*>& items);
protected:
	virtual void handleClearSelection() = 0;
	void clearLocationSelection();
	void clearActorSelection();

public:
	// set cued actors
	virtual void SetCued(std::vector<std::string> vCued) {};
};


