#pragma once
#include <QGraphicsScene>
#include <QMap>

class SLData;
class SLOperator;
struct SLSpan;
class SLIBasicGraphicItem;
/*
* Scene for the storyline
*/
class MyStorylineScene : public QGraphicsScene
{
	Q_OBJECT

public:
	MyStorylineScene(qreal x, qreal y, qreal width, qreal height);
	~MyStorylineScene();
public:
	// generate the scene using the storyline
	void GenerateScene(SLData* pData,SLOperator* pOperator);

	
public:	// switch
	void ShowLocations(bool bShowing);
	void ShowScenes(bool bShowing);
	void ShowLocationsT(bool bShowing);
	void ShowScenesT(bool bShowing);
	void ShowColor(bool bColor);

private:		// visualization settings	
	int _nMarginL = 150;			// left margin
	int _nMarginR = 350;			// right margin
	int _nXSpace = 50;				// X space of each timestep	
	int _nXMargin = 50;				// Margin between timesteps
	int _nBaseY = 100;				// base of Y	
	int _nBottomY = 0;				// bottom of Y
	int _nYSapce = 12;				// Y Space between adjacent actors	
	int _nYLocationSpace = 20;		// Y space between locations	
	double _dbTextH = 7;			// height of text
	double _dbTextW = 7;			// width of text //En:3;CN:7;
	double _dbTextM = 5;			// margin of text 	
	double _dbActorNameMargin = 4;	// margin of actor name
	double _dbBarWidth = 100;		// total width of the bar

	int _nHierarchyDepth = 100;		// depth of each hierarchy level

	SLData* _pSL = NULL;
	SLOperator* _pSLO = NULL;

	QList<QGraphicsItem*> _listLocationLabels;
	QList<QGraphicsItem*> _listLocationBars;
	QList<QGraphicsItem*> _listSessionLabels;
	QList<QGraphicsItem*> _listSessionLinks;

	QMap<QString, SLIBasicGraphicItem*> _mapSessionRegions;
	QMap<QString, SLIBasicGraphicItem*> _mapLocationRegions;

	/*
	* sessions of selected actors
	*/
	QList<QString> _listActorCuedSessions;

	/*
	* sessions of selected locations
	*/
	QList<QString> _listCuedLocations;


	/*
	* Y for each location
	* calculated when generate locations
	* used for moving locations
	*/
	QList<int> _listLocationY;

	/*
	* map of location to base y
	* calculated before generate graphics
	*/
	QMap<QString, int> _mapLocationBaseY;

	bool _bShowL = true;			// if show locations graphics
	bool _bShowS = true;			// if show scenes graphics
	bool _bShowLT = true;			// if show locations text
	bool _bShowST = true;			// if show scenes text
	bool _bColor = true;			// if show actorline color
private slots:
	void onSelectionChanged();
	void onLocationMoved();
public slots:
	void OnUpdateStoryline();		// update the position of graphics
signals:
	void UpdateTips();
	void UpdateSessionView();
private:
	// generate the scene
	void generateScene();

	/*
	* generate location graphics
	* calculate y of each locations for location moving
	*/
	void generateLocations();

	/*
	* generate the hierarchical structure of the locations
	*/
	void generateLocationsHierarchy();

	/*
	* add a location bar of given
	* location id
	* location weight (w0, w1, or w)
	* x
	* y
	* w
	* h
	* color
	*/
	void addLocationBar(QString strL,double dbW,double x, double y, double w, double h, QColor c);

	// generate sessions graphics
	void generateSessions();

	// generate actors graphics
	void generateActors();

	// generate one actor graphics
	void generateActor(std::string strA);

	/*
	* generate the actor label
	*/
	void generateActorLabel(std::string strId, std::string strA, int nX, int nY);

	/*
		calculate position for each location
	*/
	void calcLocationPos();

	void updateLocations();

	void updateScenes();

	void getSpanLocation(std::string strA, int nT, std::string strL, int& nX1, int& nX2, int& nY);

	// clear all the items in the scene
	void clearScene();

	// get the width of the location rectangle
	int getLocationRectWidth();


public:
	/*
	* set cued sessions by selected actors
	*/
	void SetActorCuedSessions(std::vector<std::string> vCued);

	/*
	* set cued locations by select location in storymap
	*/
	void SetCuedLocations(std::vector<std::string> vCued);


};


