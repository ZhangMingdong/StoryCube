#include <QtGui>
#include <QDockWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QGraphicsScene>
#include <QColorDialog>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QSettings>
#include <QQueue>
#include <QSplitter>
#include <QApplication>

#include <string>
#include <map>

#include "MainWindow.h"
#include "SLGLink.h"

#include "MyStorylineView.h"
#include "MyStorylineScene.h"
#include "MySessionScene.h"
#include "ControlWidget.h"
#include "ActorWidget.h"
#include "MyActorPCScene.h"
#include "MyLocationPCScene.h"
#include "SLData.h"
#include "SLOperator.h"

using namespace std;


const QString ShowL("ShowL");
const QString ShowS("ShowS");

const QString ShowLT("ShowLT");
const QString ShowST("ShowST");

const QString ShowColor("ShowColor");
//const QString ActorGroup("ActorGroup");
const QString MostRecentFile("MostRecentFile");

const QString TimeRange("TimeRange");
const QString MimeType = "application/LAHS";

template<template<typename T> class S, typename T>
T min(const S<T> &sequence)
{
	Q_ASSERT(!sequence.isEmpty());
	T minimum = sequence.first();
	foreach(const T &item, sequence)
	if (item < minimum)
		minimum = item;
	return minimum;
}

template<template<typename T> class S, typename T>
T max(const S<T> &sequence)
{
	Q_ASSERT(!sequence.isEmpty());
	T maximum = sequence.first();
	foreach(const T &item, sequence)
	if (item > maximum)
		maximum = item;
	return maximum;
}

namespace AQP{

	template<typename T>
	bool okToClearData(bool (T::*saveData)(), T *parent,
		const QString &title, const QString &text,
		const QString &detailedText = QString())
	{
		Q_ASSERT(saveData && parent);
#if QT_VERSION >= 0x040600
		QScopedPointer<QMessageBox> messageBox(new QMessageBox(parent));
#else
		QSharedPointer<QMessageBox> messageBox(new QMessageBox(parent));
#endif
		messageBox->setWindowModality(Qt::WindowModal);
		messageBox->setIcon(QMessageBox::Question);
		messageBox->setWindowTitle(QString("%1 - %2")
			.arg(QApplication::applicationName()).arg(title));
		messageBox->setText(text);
		if (!detailedText.isEmpty())
			messageBox->setInformativeText(detailedText);
		messageBox->addButton(QMessageBox::Save);
		messageBox->addButton(QMessageBox::Discard);
		messageBox->addButton(QMessageBox::Cancel);
		messageBox->setDefaultButton(QMessageBox::Save);
		messageBox->exec();
		if (messageBox->clickedButton() ==
			messageBox->button(QMessageBox::Cancel))
			return false;
		if (messageBox->clickedButton() ==
			messageBox->button(QMessageBox::Save))
			return (parent->*saveData)();
		return true;
	}
}

MainWindow::MainWindow()
{
	createSceneAndView();
	createActions();
	createMenus();
	createToolBars();
	createDockWidgets();
	populateMenusAndToolBars();
	createConnections();

	QSettings settings;
	viewShowLAction->setChecked(settings.value(ShowL, true).toBool());
	viewShowSAction->setChecked(settings.value(ShowS, true).toBool());
	viewShowLTAction->setChecked(settings.value(ShowLT, true).toBool());
	viewShowSTAction->setChecked(settings.value(ShowST, true).toBool());
	viewShowColorAction->setChecked(settings.value(ShowColor, true).toBool());
//	viewGroupingAction->setChecked(settings.value(ActorGroup, true).toBool());


	QString filename = settings.value(MostRecentFile).toString();



	if (filename.isEmpty() || filename == tr("Unnamed"))
		QTimer::singleShot(0, this, SLOT(fileNew()));
	else {
		_nTimeRange = settings.value(TimeRange).toInt();
		//	_nTimeRange = 50;
		_pControlWidget->SetTimeRange(_nTimeRange);
		setWindowFilePath(filename);
		QTimer::singleShot(0, this, SLOT(loadFile()));
	}
	this->showMaximized();

}

MainWindow::~MainWindow() {
	if (_pStoryline) delete _pStoryline;
	if (_pSLOperator) delete _pSLOperator;
}

void MainWindow::createSceneAndView(){
	_pSceneStoryline = new MyStorylineScene(0, 0, 600, 500);
	_pSceneSessionD = new MySessionScene(0, 0, 100, 300);
	_pSceneSessionO = new MySessionScene(0, 0, 100, 300);


	_pScenePCA = new MyActorPCScene(0, 0, 100, 300);
	_pScenePCL = new MyLocationPCScene(0, 0, 100, 300);
	_pScenePCA_O = new MyActorPCScene(0, 0, 100, 300, true);
	_pScenePCL_O = new MyLocationPCScene(0, 0, 100, 300, true);

	_pViewStoryline = new MyStorylineView();
	_pViewStoryline->setScene(_pSceneStoryline);
	_pViewStoryline->setDragMode(QGraphicsView::RubberBandDrag);
	_pViewStoryline->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
	_pViewStoryline->setContextMenuPolicy(Qt::ActionsContextMenu);

	_pViewSession = new MyStorylineView();
	_pViewSession->setScene(_pSceneSessionD);
	_pViewSession->setDragMode(QGraphicsView::RubberBandDrag);
	_pViewSession->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
	_pViewSession->setContextMenuPolicy(Qt::ActionsContextMenu);

	_pViewActorProjection = new MyStorylineView();
	//_pViewActorProjection->setScene(_pSceneSessionD);
	_pViewActorProjection->setScene(_pScenePCA_O);
	_pViewActorProjection->setDragMode(QGraphicsView::RubberBandDrag);
	_pViewActorProjection->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
	_pViewActorProjection->setContextMenuPolicy(Qt::ActionsContextMenu);


	_pViewLocationGrid = new MyStorylineView();
	_pViewLocationGrid->setScene(_pScenePCL_O);
	_pViewLocationGrid->setDragMode(QGraphicsView::RubberBandDrag);
	_pViewLocationGrid->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
	_pViewLocationGrid->setContextMenuPolicy(Qt::ActionsContextMenu);

	_pViewSelectionA = new MyStorylineView();
	_pViewSelectionA->setScene(_pScenePCA);
	_pViewSelectionA->setDragMode(QGraphicsView::RubberBandDrag);
	_pViewSelectionA->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
	_pViewSelectionA->setContextMenuPolicy(Qt::ActionsContextMenu);


	_pViewSelectionL = new MyStorylineView();
	_pViewSelectionL->setScene(_pScenePCL);
	_pViewSelectionL->setDragMode(QGraphicsView::RubberBandDrag);
	_pViewSelectionL->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
	_pViewSelectionL->setContextMenuPolicy(Qt::ActionsContextMenu);

	_pSplitterMain = new QSplitter(Qt::Orientation::Vertical, this);//上下	

		// setting 2: storyline + grid view
	_pSplitterMain->addWidget(_pViewStoryline);


	QSplitter* pSplitter1 = new QSplitter(Qt::Orientation::Horizontal, this);//左右
	pSplitter1->addWidget(_pViewActorProjection);
	pSplitter1->addWidget(_pViewSelectionA);
	pSplitter1->addWidget(_pViewSelectionL);
	pSplitter1->addWidget(_pViewLocationGrid);

	_pSplitterMain->addWidget(pSplitter1);

	setCentralWidget(_pSplitterMain);
}

void MainWindow::createDockWidgets(){
	setDockOptions(QMainWindow::AnimatedDocks);
	QDockWidget::DockWidgetFeatures features 
		= QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetClosable;

	_pControlWidget = new ControlWidget;
	QDockWidget *controlDockWidget = new QDockWidget(tr("Setting"), this);
	controlDockWidget->setFeatures(features);
	controlDockWidget->setWidget(_pControlWidget);
	addDockWidget(Qt::RightDockWidgetArea, controlDockWidget);
	controlToggleAction = controlDockWidget->toggleViewAction();
	controlToggleAction->setIcon(QIcon(":/images/control.png"));
	//controlDockWidget->close();

	{

		_pActorWidget = new ActorWidget;
		QDockWidget* actorDockWidget = new QDockWidget(tr("Actors"), this);
		actorDockWidget->setFeatures(features);
		actorDockWidget->setWidget(_pActorWidget);
		addDockWidget(Qt::LeftDockWidgetArea, actorDockWidget);
		actorToggleAction = actorDockWidget->toggleViewAction();
		actorToggleAction->setIcon(QIcon(":/images/A.png"));
		//actorDockWidget->close();
		// 设置停靠窗口的尺寸
		
		// 延迟设置尺寸
		//QTimer::singleShot(0, [this, actorDockWidget]() {resizeDocks({ actorDockWidget }, { 250 }, Qt::Horizontal);	});// for GOT
		QTimer::singleShot(0, [this, actorDockWidget]() {resizeDocks({ actorDockWidget }, { 150 }, Qt::Horizontal);	});// for Narnia
	}
	{

		_pActorWidget2 = new ActorWidget;
		QDockWidget* actorDockWidget = new QDockWidget(tr("Actors"), this);
		actorDockWidget->setFeatures(features);
		actorDockWidget->setWidget(_pActorWidget2);
		addDockWidget(Qt::LeftDockWidgetArea, actorDockWidget);
		actorToggleAction2 = actorDockWidget->toggleViewAction();
		actorToggleAction2->setIcon(QIcon(":/images/A.png"));
		actorDockWidget->close();
	}


	sceneToggleAction = new QAction(tr("Show Scene View"), this);
	sceneToggleAction->setIcon(QIcon(":/images/icon.png"));
	sceneToggleAction->setCheckable(true);
	sceneToggleAction->setChecked(false);
	
}

void MainWindow::createConnections(){

	// file    
	connect(fileOpenAction, SIGNAL(triggered()), this, SLOT(fileOpen()));
	connect(fileQuitAction, SIGNAL(triggered()), this, SLOT(close()));

	// update tips
	connect(_pSceneStoryline, SIGNAL(UpdateTips()), this, SLOT(onUpdateTips()));
	connect(_pSceneSessionD, SIGNAL(UpdateTips()), this, SLOT(onUpdateTips()));
	connect(_pScenePCA, SIGNAL(UpdateTips()), this, SLOT(onUpdateTips()));
	connect(_pScenePCL, SIGNAL(UpdateTips()), this, SLOT(onUpdateTips()));
	connect(_pScenePCA_O, SIGNAL(UpdateTips()), this, SLOT(onUpdateTips()));
	connect(_pScenePCL_O, SIGNAL(UpdateTips()), this, SLOT(onUpdateTips()));


	// view
	connect(viewShowLAction, SIGNAL(toggled(bool)), this, SLOT(viewShowL(bool)));
	connect(viewShowSAction, SIGNAL(toggled(bool)), this, SLOT(viewShowS(bool)));
	connect(viewShowLTAction, SIGNAL(toggled(bool)), this, SLOT(viewShowLT(bool)));
	connect(viewShowSTAction, SIGNAL(toggled(bool)), this, SLOT(viewShowST(bool)));
	connect(sceneToggleAction, SIGNAL(toggled(bool)), this, SLOT(viewShowSession(bool)));
	connect(viewShowColorAction, SIGNAL(toggled(bool)), this, SLOT(viewShowColor(bool)));
	connect(viewGrouping1Action, SIGNAL(toggled(bool)), this, SLOT(viewGrouping1(bool)));
	connect(viewGrouping2Action, SIGNAL(toggled(bool)), this, SLOT(viewGrouping2(bool)));

	connect(viewZoomInAction, SIGNAL(triggered()), _pViewStoryline, SLOT(zoomIn()));
	connect(viewZoomOutAction, SIGNAL(triggered()), _pViewStoryline, SLOT(zoomOut()));


	// control

	connect(_pControlWidget, SIGNAL(TimeWindowChanged(int, int)), this, SLOT(setTimeWindow(int, int)));
	connect(_pControlWidget, SIGNAL(ActorThresholdChanged(int)), this, SLOT(setActorThreshold(int)));
	connect(_pControlWidget, SIGNAL(TimeRangeChanged(int)), this, SLOT(setTimeRange(int)));
	connect(_pControlWidget, SIGNAL(WeightMethodChanged(int)), this, SLOT(setWeightMethod(int)));

	connect(_pControlWidget, SIGNAL(SWChanged(int)), this, SLOT(onSWChanged(int)));
	connect(_pControlWidget, SIGNAL(LWChanged(int)), this, SLOT(onLWChanged(int)));




	connect(_pSceneSessionD, SIGNAL(UpdateStoryline()), _pSceneStoryline, SLOT(OnUpdateStoryline()));
	connect(_pSceneSessionD, SIGNAL(UpdateActorView()), this, SLOT(onUpdateActorView()));
	connect(_pSceneStoryline, SIGNAL(UpdateSessionView()), this, SLOT(onUpdateSessionView()));


	connect(_pActorWidget, SIGNAL(UpdateActorWeights(QString, double)), this, SLOT(onUpdateActorWeight(QString, double)));

	connect(_pActorWidget, SIGNAL(UpdateActorCheck(QString, int)), this, SLOT(onUpdateActorCheck(QString, int)));

	connect(_pActorWidget, SIGNAL(SelectAllActors()), this, SLOT(onSelectAllActors()));
	connect(_pActorWidget, SIGNAL(UnselectAllActors()), this, SLOT(onUnselectAllActors()));


	connect(_pControlWidget, SIGNAL(OverviewDetailChanged(bool)), this, SLOT(onOverviewDetailChanged(bool)));

	connect(_pControlWidget, SIGNAL(FlowVisibilityChanged(int)), _pScenePCL, SLOT(onFlowVisibilityChanged(int)));
	connect(_pControlWidget, SIGNAL(StatVisibilityChanged(int)), _pScenePCL, SLOT(onStatVisibilityChanged(int)));
	connect(_pControlWidget, SIGNAL(LScaleChanged(int)), _pScenePCL, SLOT(onChangeScale(int)));
	connect(_pControlWidget, SIGNAL(AScaleChanged(int)), _pScenePCA, SLOT(onChangeScale(int)));

	connect(_pControlWidget, SIGNAL(FlowVisibilityChanged(int)), _pScenePCL_O, SLOT(onFlowVisibilityChanged(int)));
	connect(_pControlWidget, SIGNAL(StatVisibilityChanged(int)), _pScenePCL_O, SLOT(onStatVisibilityChanged(int)));
	connect(_pControlWidget, SIGNAL(OLScaleChanged(int)), _pScenePCL_O, SLOT(onChangeScale(int)));
	connect(_pControlWidget, SIGNAL(OAScaleChanged(int)), _pScenePCA_O, SLOT(onChangeScale(int)));


	connect(_pControlWidget, SIGNAL(FocusATriggered()), this, SLOT(onFocusATriggered()));
	connect(_pControlWidget, SIGNAL(FocusLTriggered()), this, SLOT(onFocusLTriggered()));

	

}

void MainWindow::onOverviewDetailChanged(bool bOverview) {
	qDebug() << "void MainWindow::onOverviewDetailChanged(bool bOverview) {";
	if(bOverview) 
		_pViewSession->setScene(_pSceneSessionO);
	else
		_pViewSession->setScene(_pSceneSessionD);
}

void MainWindow::onUpdateTips() {
	// 1. set tips
	
	// set tips in the setting panel
	if (_pStoryline)
		_pControlWidget->SetTips(QString::fromStdString(_pSLOperator->GetTips()));

	// 2. trigger weight setting
	

	// trigger session weight setting
	if (_pSLOperator->GetSelectedSessions().size()==1)
	{
		// select on session
		_pControlWidget->SetSessionWeight(_pStoryline->GetSessionWO(_pSLOperator->GetSelectedSessions()[0]));
	}
	else {
		_pControlWidget->SetSessionWeight(0);
	}
	
	// trigger location weight setting
	if (_pSLOperator->GetSelectedLocations().size() == 1)
	{
		// select on session
		_pControlWidget->SetLocationWeight(_pStoryline->GetLWO(_pSLOperator->GetSelectedLocations()[0]));
	}
	else {
		_pControlWidget->SetLocationWeight(0);
	}

	// 3. update selection correlation
	set<string> setCuedActors;

	// select locations
//	if (_pSLOperator->GetSelectedLocations().size() > 0)
	{
		vector<string> vLs = _pSLOperator->GetSelectedLocations();
		vector<string> vActors = _pStoryline->GetLocationActors(vLs);
		setCuedActors.insert(vActors.begin(), vActors.end());
	//	_pScenePCA->SetCued(vActors);
	//	_pScenePCA_O->SetCued(vActors);
		_pSceneStoryline->SetCuedLocations(vLs);
	}

	// select sessions
//	if (_pSLOperator->GetSelectedSessions().size() > 0)
	{
		vector<string> vActors = _pStoryline->GetSessionActors(_pSLOperator->GetSelectedSessions());
		setCuedActors.insert(vActors.begin(), vActors.end());
		//_pScenePCA->SetCued(vActors);
		//_pScenePCA_O->SetCued(vActors);

	}

	// select actors
//	if (_pSLOperator->GetSelectedActors().size() > 0)
	{
		vector<string> vAs = _pSLOperator->GetSelectedActors();
		vector<string> vLs =  _pStoryline->GetActorLocations(vAs);
		_pScenePCL->SetCued(vLs);
		_pScenePCL_O->SetCued(vLs);


		vector<string> vSs = _pStoryline->GetActorSessions(vAs);
		_pSceneStoryline->SetActorCuedSessions(vSs);
	}

	vector<string> vCuedActors(setCuedActors.begin(), setCuedActors.end());
	_pScenePCA->SetCued(vCuedActors);
	_pScenePCA_O->SetCued(vCuedActors);

}

void MainWindow::onUpdateActorView() {
	//_pActorWidget->SetActors(_pStoryline->GetActors(), _pStoryline->GetAWMap(), _pStoryline->GetAWOMap());
	
	map<string, double> mapAW = _pStoryline->GetAWMap();
	QMap<QString, double> qmapAW;
	for (const auto& pair : mapAW) {
		qmapAW[QString::fromStdString(pair.first)] = pair.second;
	}
	_pActorWidget->ResetWeights(qmapAW);
	_pActorWidget2->ResetWeights(qmapAW);
}

void MainWindow::onUpdateSessionView() {
	_pSceneSessionD->UpdateScene();
}

void MainWindow::createActions()
{
	fileOpenAction = new QAction(QIcon(":/images/fileopen.png"),tr("Open..."), this);
	fileOpenAction->setShortcuts(QKeySequence::Open);
	fileQuitAction = new QAction(QIcon(":/images/filequit.png"),tr("Quit"), this);
	fileQuitAction->setShortcuts(QKeySequence::Quit);





	// view
	viewShowLAction = new QAction(tr("Show Location"), this);
	viewShowLAction->setIcon(QIcon(":/images/L.png"));
	viewShowLAction->setCheckable(true);
	viewShowSAction = new QAction(tr("Show Session"), this);
	viewShowSAction->setIcon(QIcon(":/images/S.png"));
	viewShowSAction->setCheckable(true);

	viewShowLTAction = new QAction(tr("Show Location Text"), this);
	viewShowLTAction->setIcon(QIcon(":/images/LT.png"));
	viewShowLTAction->setCheckable(true);
	viewShowSTAction = new QAction(tr("Show Session Text"), this);
	viewShowSTAction->setIcon(QIcon(":/images/ST.png"));
	viewShowSTAction->setCheckable(true);

	viewShowColorAction = new QAction(tr("Show Colors"), this);
	viewShowColorAction->setIcon(QIcon(":/images/colors.png"));
	viewShowColorAction->setCheckable(true);

	viewGrouping1Action = new QAction(tr("Grouping Supporting Actors"), this);
	viewGrouping1Action->setIcon(QIcon(":/images/group-of-people.png"));
	viewGrouping1Action->setCheckable(true);

	viewGrouping2Action = new QAction(tr("Grouping All Actors"), this);
	viewGrouping2Action->setIcon(QIcon(":/images/group-of-people-2.png"));
	viewGrouping2Action->setCheckable(true);


	viewZoomInAction = new QAction(QIcon(":/images/zoom-in.png"), tr("Zoom In"), this);
	viewZoomInAction->setShortcut(tr("+"));
	viewZoomOutAction = new QAction(QIcon(":/images/zoom-out.png"), tr("Zoom Out"), this);
	viewZoomOutAction->setShortcut(tr("-"));

}

void MainWindow::createMenus()
{
}

void MainWindow::createToolBars()
{
}

void MainWindow::populateMenusAndToolBars()
{
	QAction *separator = 0;
	// file
	QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
	QToolBar *fileToolBar = addToolBar(tr("File"));
	populateMenuAndToolBar(fileMenu, fileToolBar, QList<QAction*>()<< fileOpenAction);
	fileMenu->addSeparator();
	fileMenu->addAction(fileQuitAction);

	// view
	QMenu* viewMenu = menuBar()->addMenu(tr("&View"));
	QToolBar* viewToolBar = addToolBar(tr("View"));
	populateMenuAndToolBar(viewMenu, viewToolBar, QList<QAction*>()
		<< viewZoomInAction
		<< viewZoomOutAction
		<< separator
		<< viewShowLAction
		<< viewShowSAction
		<< viewShowLTAction
		<< viewShowSTAction
		<< viewShowColorAction
		<< viewGrouping1Action
		<< viewGrouping2Action
		<< separator
		<< controlToggleAction
		<< actorToggleAction
		<< actorToggleAction2
		<< sceneToggleAction
	);
}

void MainWindow::fileOpen()
{
	const QString &filename = QFileDialog::getOpenFileName(this,
		tr("%1 - Open").arg(QApplication::applicationName()),
		".", tr("XML files (*.xml)"));

	if (filename.isEmpty())
		return;

	setWindowFilePath(filename);
	loadFile();

}

void MainWindow::loadFile(){

	QFile file(windowFilePath());
	if (!file.open(QFile::ReadOnly | QFile::Text)) {
		// 		std::cerr << "Error: Cannot read file " << qPrintable(fileName)
		// 			<< ": " << qPrintable(file.errorString())
		// 			<< std::endl;
		return;
	}
	QXmlStreamReader reader(&file);
	if (loadFile(reader)) {
		setControlWidgets();

		if (_pSLOperator) delete _pSLOperator;
		_pSLOperator = new SLOperator(_pStoryline);

		_pSceneStoryline->GenerateScene(_pStoryline, _pSLOperator);
		_pSceneSessionD->GenerateScene(_pStoryline, _pSLOperator);
		_pSceneSessionO->GenerateScene(_pStoryline, _pSLOperator);

		_pScenePCA->GenerateScene(_pStoryline, _pSLOperator);
		_pScenePCL->GenerateScene(_pStoryline, _pSLOperator);
		_pScenePCA_O->GenerateScene(_pStoryline, _pSLOperator);
		_pScenePCL_O->GenerateScene(_pStoryline, _pSLOperator);
		_pActorWidget->SetActors(_pStoryline->GetActors(), _pStoryline->GetAWMap(), _pStoryline->GetAWOMap());
		_pActorWidget2->SetActors(_pStoryline->GetActors(), _pStoryline->GetAWMap(), _pStoryline->GetAWOMap());

		// default setting
		_pSceneStoryline->ShowLocations(viewShowLAction->isChecked());
		_pSceneStoryline->ShowScenes(viewShowSAction->isChecked());
		_pSceneStoryline->ShowLocationsT(viewShowLTAction->isChecked());
		_pSceneStoryline->ShowScenesT(viewShowSTAction->isChecked());


		//this->showMaximized();
	}

	file.close();
	if (reader.hasError()) {
		// 		std::cerr << "Error: Failed to parse file "
		// 			<< qPrintable(fileName) << ": "
		// 			<< qPrintable(reader.errorString()) << std::endl;
		QString errString = reader.errorString();
		return;
	}
	else if (file.error() != QFile::NoError) {
		// 		std::cerr << "Error: Cannot read file " << qPrintable(fileName)
		// 			<< ": " << qPrintable(file.errorString())
		// 			<< std::endl;
		return;
	}
	_pControlWidget->InitializeSetting();
}

void MainWindow::resizeEvent(QResizeEvent * event)
{
	QSize size = event->size();
	//view->setSceneRect(0, 0, size.width(),size.height());
	//scene->setSceneRect(0, 0, size.width(), size.height());
}

void MainWindow::closeEvent(QCloseEvent *event)
{

	QSettings settings;
	settings.setValue(ShowL, viewShowLAction->isChecked());
	settings.setValue(ShowS, viewShowSAction->isChecked());
	settings.setValue(ShowLT, viewShowLTAction->isChecked());
	settings.setValue(ShowST, viewShowSTAction->isChecked());
	settings.setValue(ShowColor, viewShowColorAction->isChecked());
//	settings.setValue(ActorGroup, viewGroupingAction->isChecked());

	QString str = windowFilePath();
	settings.setValue(MostRecentFile, windowFilePath());

	settings.setValue(TimeRange, _nTimeRange);
	event->accept();
}

void MainWindow::populateMenuAndToolBar(QMenu *menu, QToolBar *toolBar, QList<QAction*> actions)
{
	foreach(QAction *action, actions) {
		if (!action) {
			menu->addSeparator();
			toolBar->addSeparator();
		}
		else {
			menu->addAction(action);
			toolBar->addAction(action);
		}
	}
}

bool sortByLeft(const QGraphicsItem* item1,const QGraphicsItem* item2){
	QRectF rect1 = item1->sceneBoundingRect();
	QRectF rect2 = item2->sceneBoundingRect();
	return rect1.x() < rect2.x();
}
bool sortByRight(const QGraphicsItem* item1, const QGraphicsItem* item2){
	QRectF rect1 = item1->sceneBoundingRect();
	QRectF rect2 = item2->sceneBoundingRect();
	return rect1.x()+rect1.width()> rect2.x()+rect2.width();
}
bool sortByTop(const QGraphicsItem* item1, const QGraphicsItem* item2){
	QRectF rect1 = item1->sceneBoundingRect();
	QRectF rect2 = item2->sceneBoundingRect();
	return rect1.y() < rect2.y();
}
bool sortByBottom(const QGraphicsItem* item1, const QGraphicsItem* item2){
	QRectF rect1 = item1->sceneBoundingRect();
	QRectF rect2 = item2->sceneBoundingRect();
	return rect1.y() + rect1.height()> rect2.y() + rect2.height();
}


struct LinkNode 
{
	int _pres;
	QList<int> _subs;
};

struct Overlap 
{
	int _index1;
	int _index2;
	double _distance;
};

bool sortByDistance(const Overlap& ol1, const Overlap& ol2){
	return ol1._distance < ol2._distance;
}

void MainWindow::setTimeWindow(int nStart, int nEnd) {
//	qDebug() << nStart << nEnd;
	_pStoryline->SetTimeWindow(nStart, nEnd);

	_pActorWidget->SetActors(_pStoryline->GetActors(), _pStoryline->GetAWMap(), _pStoryline->GetAWOMap());
	_pActorWidget2->SetActors(_pStoryline->GetActors(), _pStoryline->GetAWMap(), _pStoryline->GetAWOMap());

	_pSceneStoryline->GenerateScene(_pStoryline, _pSLOperator);

	_pSceneSessionD->UpdateScene();
	_pSceneSessionO->UpdateSessionWeight();


	_pControlWidget->SetActor(_pStoryline->GetActorNum());
	_pControlWidget->SetLocation(_pStoryline->GetLocationNum());



	_pScenePCA->GenerateScene(_pStoryline, _pSLOperator);
	_pScenePCL->GenerateScene(_pStoryline, _pSLOperator);

	
//	_pActorWidget->SetActors(_pStoryline->GetActors(), _pStoryline->GetAWMap(), _pStoryline->GetAWOMap());
}

void MainWindow::setActorThreshold(int nActor) {
	qDebug() << "void MainWindow::setActorThreshold(int nActor) {";

	//	qDebug() << nStart << nEnd;

	_pStoryline->SetActorThreshold(nActor);



	_pActorWidget->SetActors(_pStoryline->GetActors(), _pStoryline->GetAWMap(), _pStoryline->GetAWOMap());
	_pActorWidget2->SetActors(_pStoryline->GetActors(), _pStoryline->GetAWMap(), _pStoryline->GetAWOMap());

	_pSceneStoryline->GenerateScene(_pStoryline, _pSLOperator);

	_pSceneSessionD->UpdateScene();
	_pSceneSessionO->UpdateSessionWeight();


	_pControlWidget->SetActor(_pStoryline->GetActorNum());
	_pControlWidget->SetLocation(_pStoryline->GetLocationNum());



	_pScenePCA->GenerateScene(_pStoryline, _pSLOperator);
	_pScenePCL->GenerateScene(_pStoryline, _pSLOperator);


}

void MainWindow::setLocations(int nL) {
	qDebug() << "void MainWindow::setLocations(int nL) {" << nL;
	/*
	clearStorylineScene();
	_pStoryline->SetLocations(nL);
	_pSceneStoryline->GenerateScene(_pStoryline);
	*/
}

void MainWindow::setLevel(int nLevel) {
	qDebug() << nLevel;
	_pSceneStoryline->GenerateScene(_pStoryline, _pSLOperator);

}

void MainWindow::setTimeRange(int nTimeRange) {
	_nTimeRange = nTimeRange;
	qDebug() << "setTimeRange():" << nTimeRange;
}

void MainWindow::setSortingMethod(int nMethod) {

}

void MainWindow::setWeightMethod(int nMethod) {
	_pStoryline->SetWeightMethod(nMethod);
	qDebug() << "setWeightMethod():" << nMethod;
	updateAll();
	return;
}

bool MainWindow::loadFile(QXmlStreamReader& reader) {
	bool bLoadNewData = false;
	while (!reader.atEnd()) {
		if (reader.isStartElement()) {
			if (reader.name() == QString::fromStdString("Story")) {
				// added for read story
				if (_pStoryline) delete _pStoryline;
				_pStoryline = new SLData;
				_pStoryline->ReadStory(reader,_nTimeRange);

				bLoadNewData = true;
			}
			else {
				reader.raiseError(QObject::tr("Not a Diagram file"));
			}
		}
		else
		{
			reader.readNext();
		}
	}

	return bLoadNewData;
}

void MainWindow::viewShowL(bool on) {
	_pSceneStoryline->ShowLocations(on);
}

void MainWindow::viewShowS(bool on) {
	_pSceneStoryline->ShowScenes(on);
}

void MainWindow::viewShowLT(bool on) {
	_pSceneStoryline->ShowLocationsT(on);
}

void MainWindow::viewShowST(bool on) {
	_pSceneStoryline->ShowScenesT(on);
}

void MainWindow::viewShowSession(bool on) {
	if (on)
	{
		// setting 2: storyline + grid view
		_pSplitterMain->replaceWidget(0,_pViewSession);
	}
	else {
		_pSplitterMain->replaceWidget(0, _pViewStoryline);
	}
}

void MainWindow::viewShowColor(bool on) {
	_pSceneStoryline->ShowColor(on);
}

void MainWindow::viewGrouping1(bool on) {

	if (on)
	{
		viewGrouping2Action->blockSignals(true);
		viewGrouping2Action->setChecked(false);
		viewGrouping2Action->blockSignals(false);
		if (_pStoryline->SetGroupingActors(1)) {
			_pSceneStoryline->OnUpdateStoryline();
			_pScenePCA->UpdateScene();
		}
	}
	else {
		if (_pStoryline->SetGroupingActors(0)) {
			_pSceneStoryline->OnUpdateStoryline();
			_pScenePCA->UpdateScene();
		}
	}
}

void MainWindow::viewGrouping2(bool on) {
	if (on)
	{
		viewGrouping1Action->blockSignals(true);
		viewGrouping1Action->setChecked(false);
		viewGrouping1Action->blockSignals(false);
		if (_pStoryline->SetGroupingActors(2)) {
			_pSceneStoryline->OnUpdateStoryline();
			_pScenePCA->UpdateScene();
		}
	}
	else {
		if (_pStoryline->SetGroupingActors(0)) {
			_pSceneStoryline->OnUpdateStoryline();
			_pScenePCA->UpdateScene();
		}
	}
}

// set control widgets after load file
void MainWindow::setControlWidgets() {

	_pControlWidget->SetTimeWindow(_pStoryline->GetFirstTime(), _pStoryline->GetLastTime());
	_pControlWidget->SetActor(_pStoryline->GetActorNum());
	_pControlWidget->SetLocation(_pStoryline->GetLocationNum());
}

void MainWindow::onUpdateActorCheck(QString strA, int nState) {
	qDebug() << "void MainWindow::onUpdateActorCheck(QString strA, int nState) {" << strA << nState;
	// 1. update data
	_pStoryline->HideActor(strA.toStdString(), nState == 0);

	_pScenePCA->GenerateScene(_pStoryline, _pSLOperator);
	_pScenePCL->GenerateScene(_pStoryline, _pSLOperator);

	updateAll();
}

void MainWindow::onUpdateActorWeight(QString strA, double dbW) {
//	qDebug() << "void MainWindow::onUpdateActorWeight(QList<QString> _listActorNames, QList<double> _listW) {";
	
	// 1. update data
	_pStoryline->SetActorWeights(strA.toStdString(), dbW);

	updateAll();
}

void MainWindow::onKeepOnlyOneActor(QString strA) {
	_pStoryline->KeepOnlyOneActor(strA.toStdString());

	updateAll();
}

void MainWindow::onFocusATriggered() {
	vector<string> vActors = _pSLOperator->GetSelectedActors();

	// 1. update data
	_pStoryline->SetValidActors(vActors);

	_pScenePCA->GenerateScene(_pStoryline, _pSLOperator);
	_pScenePCL->GenerateScene(_pStoryline, _pSLOperator);

	updateAll();
}

void MainWindow::onFocusLTriggered() {
	vector<string> vLocations = _pSLOperator->GetSelectedLocations();

	// 1. update data
	vector<string> vActors = _pStoryline->GetLocationActors(vLocations);
	_pStoryline->SetValidActors(vActors);

	_pScenePCA->GenerateScene(_pStoryline, _pSLOperator);
	_pScenePCL->GenerateScene(_pStoryline, _pSLOperator);

	updateAll();
}

void MainWindow::onUnselectAllActors() {
	qDebug() << "void MainWindow::onUnselectAllActors() {";
	_pStoryline->UncheckAllActors();

	_pScenePCA->GenerateScene(_pStoryline, _pSLOperator);
	_pScenePCL->GenerateScene(_pStoryline, _pSLOperator);

	updateAll();
}

void MainWindow::onSelectAllActors() {
	qDebug() << "void MainWindow::onSelectAllActors() {";
	_pStoryline->CheckAllActors();

	_pScenePCA->GenerateScene(_pStoryline, _pSLOperator);
	_pScenePCL->GenerateScene(_pStoryline, _pSLOperator);

	updateAll();
}

void MainWindow::updateAll() {
	// 1. update storyline scene
	_pSceneStoryline->GenerateScene(_pStoryline, _pSLOperator);

	// 2. update session scene
	_pSceneSessionD->UpdateScene();
	_pSceneSessionO->UpdateSessionWeight();


	// 3. update actor lists
	_pActorWidget->ResetWeights(_pStoryline->GetAWMap());
	_pActorWidget2->ResetWeights(_pStoryline->GetAWMap());

	// 4. update the pc scene
	_pScenePCL->UpdateScene();
	_pScenePCA->UpdateScene();
	_pScenePCL_O->UpdateScene();
	_pScenePCA_O->UpdateScene();
}

void MainWindow::onSWChanged(int nW) {
	qDebug() << "void MainWindow::onSWChanged(int nW) {";
	_pStoryline->UpdateSessionWeight(_pSLOperator->GetSelectedSessions(), nW);

	_pSceneSessionD->UpdateSessionWeight();

	_pSceneStoryline->OnUpdateStoryline();
	_pActorWidget->ResetWeights(_pStoryline->GetAWMap());
	_pActorWidget2->ResetWeights(_pStoryline->GetAWMap());

	// 4. update the pc scene
	_pScenePCL->UpdateScene();
	_pScenePCA->UpdateScene();
	_pScenePCL_O->UpdateScene();
	_pScenePCA_O->UpdateScene();
}

void MainWindow::onLWChanged(int nW) {
	qDebug() << "void MainWindow::onLWChanged(int nW) {" << nW;
	_pStoryline->UpdateLocationWeight(_pSLOperator->GetSelectedLocations()[0], nW);

	_pSceneSessionD->UpdateSessionWeight();

	_pSceneStoryline->OnUpdateStoryline();

	_pActorWidget->ResetWeights(_pStoryline->GetAWMap());
	_pActorWidget2->ResetWeights(_pStoryline->GetAWMap());


	// 4. update the pc scene
	_pScenePCL->UpdateScene();
	_pScenePCA->UpdateScene();
	_pScenePCL_O->UpdateScene();
	_pScenePCA_O->UpdateScene();

}

