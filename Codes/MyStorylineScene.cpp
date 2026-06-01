#include "MyStorylineScene.h"

#include "SLData.h"
#include "SLGRect.h"
#include "SLGText.h"
#include "SLGActorline.h"
#include "SLOperator.h"
#include <iostream>

#include <QDebug>
#include <QTimer>


using namespace std;

extern int getStrIndex(const vector<string>& list, const string& str) noexcept;


MyStorylineScene::MyStorylineScene(qreal x, qreal y, qreal width, qreal height)
	: QGraphicsScene(x,y,width,height)
{
	connect(this, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
	
}

MyStorylineScene::~MyStorylineScene()
{

}

void MyStorylineScene::GenerateScene(SLData* pData, SLOperator* pOperator) {
	_pSL = pData;
	_pSLO = pOperator;
	generateScene();
}

void MyStorylineScene::generateScene() {
	// 0.clear old scene
	clearScene();
	// 1.calculate position for locations
	calcLocationPos();
	// 2.generate locations
	generateLocations();
	//generateLocationsHierarchy();
	// 3.generate sessions
	generateSessions();
	// 4.generate actors
	generateActors();

	// 5.update the showing states
	updateLocations();
	updateScenes();

	// 6.adjust the scene region
	QRectF rect = this->itemsBoundingRect();
	setSceneRect(rect.adjusted(-20, -20, 20, 20));
}

void MyStorylineScene::addLocationBar(QString strL, double dbW, double x, double y, double w, double h, QColor c) {
	// outer rect
	{
		SLGRect* rectBar = SLGRect::CreateSLGRect(SLGRect::Type::LocationBar);
		rectBar->setData(strL);
		//int nBarX = _nMarginL;

		rectBar->setX(x+w/2.0);
		rectBar->setY(y);
		rectBar->setWidth(w);
		rectBar->setHeight(h);

		QColor color_bar(Qt::blue);
		color_bar.setAlpha(100);
		rectBar->setBrush(Qt::NoBrush);
		rectBar->setPen(color_bar);

		_listLocationBars.append(rectBar);
		this->addItem(rectBar);
	}
	// inner rect
	{
		SLGRect* rectBar = SLGRect::CreateSLGRect(SLGRect::Type::LocationBar);
		rectBar->setData(strL);
		//int nBarX = _nMarginL;
		double dbBarW = w * dbW;
		rectBar->setX(x + dbBarW / 2.0);
		rectBar->setY(y);
		rectBar->setWidth(dbBarW);
		rectBar->setHeight(h);

		c.setAlpha(100);
		rectBar->setBrush(c);
		rectBar->setPen(Qt::NoPen);

		_listLocationBars.append(rectBar);
		this->addItem(rectBar);
	}
}

int MyStorylineScene::getLocationRectWidth() {

	// width of location rectangle
	int nTimeLen = _pSL->GetTimeLen();
	return nTimeLen * _nXSpace + (nTimeLen - 1) * _nXMargin;
}

void MyStorylineScene::generateLocations() {
	// 1.calculate width and x for the locations globally
	int nWidth = getLocationRectWidth();

	// x position of the location, using the middle
	int nX = _nMarginL + nWidth / 2;

	_listLocationY.clear();

	// 2.create graphic for each location

	// get max weight
	double dbMaxLWO = _pSL->GetMaxLWO();
	double dbMaxLW1 = _pSL->GetMaxLW1();
	double dbMaxLW = _pSL->GetMaxLW();
	vector<string> listL = _pSL->GetDisplayedLocationsBySeq();
	for (string strL : listL)
	{
		QString qstrL = QString::fromStdString(strL);
		// 2.0 check validity
		if (_pSL->GetLW1(strL) == 0) continue;

		// 2.1 calculate the Y and height
		int nHeight = _pSL->GetLocationMax(strL) * _nYSapce;
		int nY = _mapLocationBaseY[qstrL] + nHeight / 2 - _nYSapce / 2;
		_listLocationY.append(nY);

		// 2.2.create rectangle: show the row of each location
		SLGRect* rect = SLGRect::CreateSLGRect(SLGRect::Type::Location);
		rect->setData(qstrL);
		rect->setX(nX);
		rect->setY(nY);
		rect->setWidth(nWidth);
		rect->setHeight(nHeight);

		QColor color(Qt::gray);
		double dbOpacity = .2;
		color.setAlpha(100 * dbOpacity);
		rect->setBrush(color);
		rect->setPen(Qt::NoPen);

		_mapLocationRegions[qstrL] = rect;
		this->addItem(rect);

		// 2.3.create weight bar
		int nBarX = _nMarginL + nWidth + _dbTextM;
		// WO
		addLocationBar(qstrL, _pSL->GetLWO(strL) / dbMaxLWO, nBarX, nY, _dbBarWidth, nHeight, Qt::red);
		// W1
		nBarX += (_dbBarWidth + _dbTextM);
		addLocationBar(qstrL, _pSL->GetLW1(strL) / dbMaxLW1, nBarX, nY, _dbBarWidth, nHeight, Qt::green);
		// W			
		nBarX += (_dbBarWidth + _dbTextM);
		addLocationBar(qstrL, _pSL->GetLW1(strL) / dbMaxLW, nBarX, nY, _dbBarWidth, nHeight, Qt::blue);
		nBarX += (_dbBarWidth + _dbTextM);

		// 2.4.create text
		SLGText* pT = SLGText::CreateSLGText(SLGText::Type::Location);
		QString strLocation = QString::fromStdString(_pSL->GetLocationDisplayName(strL));
		//if (strLocation.length() > 10) strLocation = strLocation.first(10) + "..";
		pT->SetText(strLocation);
		pT->setPos(nBarX, nY);
		pT->setData(qstrL);


		_listLocationLabels.append(pT);
		this->addItem(pT);

		connect(pT, SIGNAL(MoveLocation()), this, SLOT(onLocationMoved()));
	}

}

void MyStorylineScene::generateLocationsHierarchy() {

	// the left base of the hierarchy coordinate
	int nLBase = _nMarginL + getLocationRectWidth() + 4 * _dbTextM + 3 * _dbBarWidth;

	// get the merge list
	vector<LocationMerge> listMergeRecord = _pSL->GetMergeRecords();

	// get the hierarchy depth
	int nLD = _pSL->GetLocationDepth();
	vector<string> listL = _pSL->GetDisplayedLocations();
	vector<string> listLSeq = _pSL->GetDisplayedLocationsBySeq();

	// record clusters location
	map<string, pair<double, double>> mapClusterPos;
	for (const LocationMerge& mr:listMergeRecord)
	{
		string strI = mr._strL1;
		string strJ = mr._strL2;
		string strK = mr._strC;
		int nDis = mr._nToRoot;

		double dbXL_I;
		double dbXL_J;
		double dbXR = nLBase + _nHierarchyDepth * (nLD - nDis);
		double dbXM;

		double dbY_I;
		double dbY_J;
		double dbY ;

		if (find(listL.begin(), listL.end(), strI) != listL.end())
		{
			dbXL_I = nLBase;
			dbY_I = _listLocationY[getStrIndex(listLSeq, strI)];
		}
		else if (mapClusterPos.find(strI) != mapClusterPos.end()) {
			dbXL_I = mapClusterPos[strI].first;
			dbY_I = mapClusterPos[strI].second;
		}
		else
			continue;
		
		if (find(listL.begin(), listL.end(), strJ) != listL.end())
		{
			dbXL_J = nLBase;
			dbY_J = _listLocationY[getStrIndex(listLSeq, strJ)];
		}
		else if (mapClusterPos.find(strJ)!= mapClusterPos.end()){
			dbXL_J = mapClusterPos[strJ].first;
			dbY_J = mapClusterPos[strJ].second;
		}
		else
			continue;

		// double dbXL = std::max(dbXL_I, dbXL_J);

		double dbXL = dbXR - _nHierarchyDepth;

		dbXM = (dbXL + dbXR) / 2.0;
		dbY = (dbY_I + dbY_J) / 2.0;

		// curve for clusterA
		{

			QGraphicsPathItem* pItem = new QGraphicsPathItem();
			QPainterPath path;
			path.moveTo(dbXL_I, dbY_I);
			if (dbXL_I < dbXL) path.lineTo(dbXL, dbY_I);
			path.cubicTo(dbXM, dbY_I, dbXM, dbY, dbXR, dbY);
			QPen pen(Qt::red, 2, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
			pItem->setPen(pen);
			pItem->setPath(path);
			this->addItem(pItem);
		}
		// curve for clusterB
		{

			QGraphicsPathItem* pItem = new QGraphicsPathItem();
			QPainterPath path;
			path.moveTo(dbXL_J, dbY_J);
			if (dbXL_J < dbXL) path.lineTo(dbXL, dbY_J);
			path.cubicTo(dbXM, dbY_J, dbXM, dbY, dbXR, dbY);
			QPen pen(Qt::red, 2, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
			pItem->setPen(pen);
			pItem->setPath(path);
			this->addItem(pItem);
		}
		// record pos for the new cluster
		mapClusterPos[strK] = { dbXR, dbY };

//		cout << dbY << "\t" << dbY_I << "\t" << dbY_J << endl;
	}
}

void MyStorylineScene::generateSessions() {
	// create graphic element for Session

	vector<string> listLocations = _pSL->GetDisplayedLocations();
	vector<string> listSessions = _pSL->GetSessions();

	for (string strId : listSessions)
	{
		int nTime = _pSL->GetSessionStart(strId);
		string strL = _pSL->GetSessionLocation(strId);
		int nWidth = _nXSpace;
		int nHeight = _pSL->GetLocationMax(strL) * _nYSapce;
		int nX = _nMarginL + _pSL->GetTimeProjection(nTime) * (_nXMargin + _nXSpace) + nWidth / 2;
		int nY = _mapLocationBaseY[QString::fromStdString(strL)] + nHeight / 2 - _nYSapce / 2;
		nHeight += _nYSapce;

		SLGRect* rect = SLGRect::CreateSLGRect(SLGRect::Type::Session);
		rect->setData(QString::fromStdString(strId));
		rect->setX(nX);
		rect->setY(nY);
		rect->setWidth(nWidth);
		rect->setHeight(nHeight);
		//rect->setBrush(Qt::cyan);
		//rect->setPen(Qt::NoPen);

		QPen pen(Qt::gray, 2, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
		rect->setPen(pen);

		_mapSessionRegions[QString::fromStdString(strId)] = rect;

		this->addItem(rect);
	}
}

void MyStorylineScene::generateActorLabel(std::string strId, std::string strLabel, int nX, int nY) {

	// create SLGText
	SLGText* pLabel = SLGText::CreateSLGText(SLGText::Type::Actor);


	pLabel->SetText(QString::fromStdString(strLabel));
	pLabel->setPos(QPointF(nX,nY));
	pLabel->setData(QString::fromStdString(strId));

	this->addItem(pLabel);
}

void MyStorylineScene::getSpanLocation(std::string strA, int nT, std::string strL, int& nX1, int& nX2, int& nY) {

	// get SLLocation of this span
	strL = _pSL->GetDisplayLocation(strL);

	int nIndex = _pSL->GetActorSeqInSession(strA, nT, strL);

	int nProjectedTime = _pSL->GetTimeProjection(nT);
	nX1 = _nMarginL + nProjectedTime * (_nXSpace + _nXMargin);
	nX2 = _nMarginL + nProjectedTime * (_nXSpace + _nXMargin) + _nXSpace;
	nY = _mapLocationBaseY[QString::fromStdString(strL)] + nIndex * _nYSapce;
}

void MyStorylineScene::generateActor(string strA) {
	// 获取角色颜色
	const int* color = _pSL->GetActorColor(strA);
	SLGActorline* pActor = new SLGActorline(_bColor && (color[0] + color[1] + color[2]) > 0, color[0], color[1], color[2]);

	// 获取角色跨度信息
	vector<int> vT;
	vector<string> vL;
	vector<bool> vDeath;
	_pSL->GetActorSpanInfo(strA, vDeath, vT, vL);

	// 初始化跟踪变量
	int nLastX = 0;
	int nLastY = 0;
	int nLastStart = 0;
	bool bFirstSpan = true;

	for (int i = 0; i < vT.size(); i++) {
		const int nT = vT[i];
		const string& strL = vL[i];
		const bool bDeath = vDeath[i];

		// 计算当前位置
		int nX1, nX2, nY;
		getSpanLocation(strA, nT, strL, nX1, nX2, nY);

		// 添加角色标签（仅第一次）
		if (bFirstSpan) {
			generateActorLabel(strA, _pSL->GetActorDisplayName(strA),nX1 - _dbActorNameMargin, nY);
		}

		// 处理连接线
		if (!bFirstSpan) {
			int nSeq = 0, nGroup = 0, nGroupSeq = 0;
			_pSL->GetActorSequence(nT, strA, nSeq, nGroup, nGroupSeq);

			pActor->addConnection(nLastX, nLastY, nX1, nY, nGroup, nGroupSeq, (nT == nLastStart + 1));
		}

		// 添加主线段
		pActor->addSpan(nX1, nX2, nY);

		// 添加死亡标记
		if (bDeath) {
			pActor->addDeathGlyph(nX2, nY, _nYSapce / 3.0f);
		}

		// 更新跟踪变量
		nLastX = nX2;
		nLastY = nY;
		nLastStart = nT;
		bFirstSpan = false;
	}

	this->addItem(pActor);
}

void MyStorylineScene::generateActors() {
	// create graphic element for actors

	for (string strA : _pSL->GetActors()) {	// loop actors
		//if (_pSL->getAW(strA)>0)
		{
			generateActor(strA);
		}
	}
}

void MyStorylineScene::calcLocationPos() {

	vector<string> listLocations = _pSL->GetDisplayedLocationsBySeq();
	_mapLocationBaseY.clear();
	// 1.build sequence map
	int nCurrentY = _nBaseY;

	for (string strL: listLocations)
	{
		if (_pSL->GetLW1(strL) > 0) {
			int nMax = _pSL->GetLocationMax(strL);
			QString astrL = QString::fromStdString(strL);
			_mapLocationBaseY[astrL] = nCurrentY;
			//qDebug() << astrL << nCurrentY;
			if (nMax > 0)
				nCurrentY += (_nYLocationSpace + _nYSapce * nMax);
		}
	}
	_nBottomY = nCurrentY;

}

void MyStorylineScene::OnUpdateStoryline() {
	qDebug() << "OnUpdateStoryline";
	generateScene();
}

void MyStorylineScene::clearScene() {

	QList<QGraphicsItem*> items = this->items();
	if (!items.empty())
	{
		qDeleteAll(items);
	}
	_listLocationLabels.clear();
	_listLocationBars.clear();
	_listSessionLabels.clear();
	_listSessionLinks.clear();

	_mapSessionRegions.clear(); 
	_mapLocationRegions.clear();

}

void MyStorylineScene::updateLocations() {

	for (auto it = _mapLocationRegions.begin(); it != _mapLocationRegions.end(); ++it) {
		QGraphicsItem* item = it.value();

		if (item) {
			_bShowS ? item->show() : item->hide();

		}
	}

	for (QGraphicsItem* item : _listLocationLabels)
	{
		_bShowL && _bShowLT ? item->show() : item->hide();
	}
	for (QGraphicsItem* item : _listLocationBars)
	{
		_bShowL && _bShowLT ? item->show() : item->hide();
	}
	update();
}

void MyStorylineScene::updateScenes() {
	for (auto it = _mapSessionRegions.begin(); it != _mapSessionRegions.end(); ++it) {
		QGraphicsItem* item = it.value(); 

		if (item) {
			_bShowS ? item->show() : item->hide();

		}
	}

	for (QGraphicsItem* item : _listSessionLabels)
	{
		_bShowS && _bShowST ? item->show() : item->hide();
	}
	for (QGraphicsItem* item : _listSessionLinks)
	{
		_bShowS && _bShowST ? item->show() : item->hide();
	}
	update();
}

void MyStorylineScene::ShowLocations(bool bShowing) {
	if (_bShowL != bShowing)
	{
		_bShowL = bShowing;
		updateLocations();
	}
}

void MyStorylineScene::ShowScenes(bool bShowing) {
//	qDebug() << "void MyStorylineScene::ShowScenes(bool bShowing) {"<<_bShowS;
	if (_bShowS != bShowing)
	{
		_bShowS = bShowing;
		updateScenes();
	}

}

void MyStorylineScene::ShowLocationsT(bool bShowing) {
	if (_bShowLT != bShowing)
	{
		_bShowLT = bShowing;
		updateLocations();
	}
}

void MyStorylineScene::ShowColor(bool bColor) {
	if (_bColor!=bColor)
	{
		_bColor = bColor;
		generateScene();
	}
}

void MyStorylineScene::ShowScenesT(bool bShowing) {
	if (_bShowST != bShowing)
	{
		_bShowST = bShowing;
		updateScenes();
	}

}

void MyStorylineScene::onSelectionChanged() {
	//	qDebug() << "void MySessionScene::onSelectionChanged() {";
	QList<QGraphicsItem*> items = this->selectedItems();
	if (items.length() == 1)
	{
		for (QGraphicsItem* item : items) {
			SLIBasicGraphicItem* pText = dynamic_cast<SLIBasicGraphicItem*>(item);
			if (pText) {
				switch (pText->getType())
				{
				case SLIBasicGraphicItem::Type::Actor:
					_pSLO->SelectActor(pText->getData().toString().toStdString());
					QTimer::singleShot(0, this, [=]() {	emit UpdateTips(); });
					break;
				case SLIBasicGraphicItem::Type::Location:
				case SLIBasicGraphicItem::Type::LocationBar:
					_pSLO->SelectLocation(pText->getData().toString().toStdString());
					QTimer::singleShot(0, this, [=]() {
						emit UpdateTips();
						});
					break;
				case SLIBasicGraphicItem::Type::Session:
					_pSLO->SelectSession(pText->getData().toString().toStdString());
					QTimer::singleShot(0, this, [=]() {
						emit UpdateTips();
						});
					break;
				default:
					break;
				}
				break;
			}
		}
	}
}

void MyStorylineScene::onLocationMoved() {
//	qDebug() << "void MyStorylineScene::onLocationMoved() {";

	QList<QGraphicsItem*> items = this->selectedItems();
	if (items.length() == 1)
	{
		for (QGraphicsItem* item : items) {
			SLGText* pText = dynamic_cast<SLGText*>(item);
			if (pText) {
				switch (pText->getType())
				{
				case SLGText::Type::Actor:
					break;
				case SLGText::Type::Location:
				{
					QStringView strID = pText->getData();
					int nSeq = 0;
					double dbY = pText->y();
					while (nSeq < _listLocationY.length() && dbY > _listLocationY[nSeq]) nSeq++;
					qDebug() << "Move Location: " << strID << " to " << nSeq;
					_pSL->LocationMoveTo(strID.toString().toStdString(), nSeq);
					OnUpdateStoryline();
					emit UpdateSessionView();
				}
					break;
				case SLGText::Type::Session:
					break;
				default:
					break;
				}
			}
		}
	}
}

void MyStorylineScene::SetActorCuedSessions(vector<string> vCued) {
	_listActorCuedSessions.clear();
	for (string strS : vCued)
	{
		_listActorCuedSessions.append(QString::fromStdString(strS));
	}

	for (auto it = _mapSessionRegions.begin(); it != _mapSessionRegions.end(); ++it) {
		QString strS = it.key();
		if (_listActorCuedSessions.contains(strS))
		{
			QColor greenColor(0, 255, 0, 51); // RGBA: 红,绿,蓝,透明度
			QBrush brush(greenColor);
			_mapSessionRegions[strS]->setBrush(brush);
		}
		else {
			_mapSessionRegions[strS]->setBrush(Qt::NoBrush);
		}
	}

	update();
}


void MyStorylineScene::SetCuedLocations(vector<string> vCued) {
	_listCuedLocations.clear();
	for (string strL : vCued)
	{
		_listCuedLocations.append(QString::fromStdString(strL));
	}

	for (auto it = _mapLocationRegions.begin(); it != _mapLocationRegions.end(); ++it) {
		QString strL = it.key();
		if (_listCuedLocations.contains(strL))
		{
			QColor greenColor(0, 255, 0, 255); // RGBA: 红,绿,蓝,透明度
			QPen pen(greenColor);
			_mapLocationRegions[strL]->setPen(pen);
		}
		else {
			_mapLocationRegions[strL]->setPen(Qt::NoPen);
		}
	}


	update();

}