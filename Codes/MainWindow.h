#pragma once
#include <QMainWindow>
#include <QPair>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QUuid>
#include <QMap>

class SLData;
class SLOperator;

class QAction;
class QGraphicsItem;
class QGraphicsScene;
class QGraphicsItemGroup;
class IAbstractItem;
class MyStorylineView;
class IBasicGraphicItem;
class LayersWidget;
class QVBoxLayout;
class ControlWidget;
class ActorWidget;
class BGGroup;
class MyStorylineScene;
class MySessionScene;
class MyPCScene;
class QSplitter;


/*
* main window
* - define the global operation and controls
* - last modified: 2024/05/12
*/
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
	~MainWindow();
private:
	// === Initialization
	void createSceneAndView();
	void createActions();
	void createDockWidgets();
	void createConnections();
	void createMenus();
	void createToolBars();
	void populateMenusAndToolBars();

private slots:
	// === slots:

	// == menu
	void fileOpen();
	void loadFile();
	void viewShowL(bool on);
	void viewShowS(bool on);
	void viewShowLT(bool on);
	void viewShowST(bool on);
	void viewShowSession(bool on);
	void viewShowColor(bool on);
	void viewGrouping1(bool on);
	void viewGrouping2(bool on);

	// == control widget
	void setTimeWindow(int nStart, int nEnd);
	void setLevel(int nLevel);
	void setActorThreshold(int nActor);
	void setLocations(int nL);
	void setTimeRange(int nRange);
	void setSortingMethod(int nMethod);
	void setWeightMethod(int nMethod);
	void onUpdateActorWeight(QString strA, double dbW);
	void onUpdateActorCheck(QString strA, int nState);
	void onSelectAllActors();
	void onUnselectAllActors();
	void onKeepOnlyOneActor(QString strA);
	void onFocusATriggered();
	void onFocusLTriggered();

	// call back
	void onUpdateActorView();
	void onUpdateSessionView();

	void onOverviewDetailChanged(bool);
	void onUpdateTips();

	void onSWChanged(int nW);
	void onLWChanged(int nW);

private:
	// === actions

	// == file
	QAction *fileOpenAction;
	QAction *fileQuitAction;

	// == view
	QAction *viewZoomInAction;
	QAction *viewZoomOutAction;
	QAction *viewShowLAction;
	QAction *viewShowSAction;
	QAction* viewShowLTAction;
	QAction* viewShowSTAction;
	QAction *viewShowColorAction;
	QAction* viewGrouping1Action;
	QAction* viewGrouping2Action;


	QAction* actorToggleAction;
	QAction* actorToggleAction2;
	QAction* controlToggleAction;
	QAction* sceneToggleAction;

private:
	// === scenes, views, and widgetss

	QSplitter* _pSplitterMain;

	// ==  scenes
	MyStorylineScene* _pSceneStoryline;
	MySessionScene* _pSceneSessionD;
	MySessionScene* _pSceneSessionO;
	MyPCScene* _pScenePCA;
	MyPCScene* _pScenePCL;
	MyPCScene* _pScenePCA_O;
	MyPCScene* _pScenePCL_O;

	// == views
    MyStorylineView *_pViewStoryline;

	MyStorylineView* _pViewSession;
	MyStorylineView* _pViewActorProjection;
	MyStorylineView* _pViewLocationGrid;
	MyStorylineView* _pViewSelectionA;
	MyStorylineView* _pViewSelectionL;

	// == widgets
	ControlWidget* _pControlWidget;
	ActorWidget* _pActorWidget;
	ActorWidget* _pActorWidget2;


private:
	// === private operations
	
	// add actions in menu and toolbar
	void populateMenuAndToolBar(QMenu *menu, QToolBar *toolBar, QList<QAction*> actions);
	// load file
	bool loadFile(QXmlStreamReader& reader);

	/*
	* update all the views when data modified
	*/
	void updateAll();
protected:
	// === system event
	virtual void resizeEvent(QResizeEvent * event);  
	virtual void closeEvent(QCloseEvent *event);

// ============================== for storyline =============================
private:
	// storyline data
	SLData* _pStoryline = NULL;

	SLOperator* _pSLOperator = NULL;
	// global time range
	int _nTimeRange = 10000;
private:
	// set control widgets after load file
	void setControlWidgets();

};



