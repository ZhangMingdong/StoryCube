#include "MySessionScene.h"
#include "SLData.h"

#include "SLGLink.h"
#include "SLGSession.h"
#include "SLOperator.h"

#include <QDebug>

using namespace std;


MySessionScene::MySessionScene(qreal x, qreal y, qreal width, qreal height)
	: QGraphicsScene(x,y,width,height)
{
	connect(this, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
}


MySessionScene::~MySessionScene()
{
	clearScene();
}

void MySessionScene::onSelectionChanged() {
//	qDebug() << "void MySessionScene::onSelectionChanged() {";
	QList<QGraphicsItem*> items = this->selectedItems();

	vector<string> vS;
	for (QGraphicsItem* item : items) {
		SLGSession* pText = dynamic_cast<SLGSession*>(item);
		if (pText) {
			vS.push_back(pText->getData().toString().toStdString());
		}
	}

	if (!vS.empty())
	{
		_pSLO->SelectSessions(vS);
		emit UpdateTips();
	}
}

void MySessionScene::setNodePos(SLGSession* pNode, int nT, int nSeq) {
	int nX = nT * _nSpaceX + _nSpaceX;
	int nY = nSeq * _nSpaceY;
	pNode->setPos(nX, nY);
}

void MySessionScene::clearScene() {
	QList<QGraphicsItem*> items = this->items();
	if (!items.empty())
	{
		//	QMutableListIterator<QGraphicsItem*> i(items);
		//	while (i.hasNext()) {
		//		SLGLink* link = dynamic_cast<SLGLink*>(i.next());
		//		if (link) {
		//			delete link;
		//			i.remove();
		//		}
		//	}
		qDeleteAll(items);
	}

}

void MySessionScene::UpdateScene() {

	// clear 
	_mapSessions.clear();


	// generate graphics
	QList<QGraphicsItem*> listSessions;
	QList<QGraphicsItem*> listLinks;
	QMap<QString, SLGSession*> mapSessions;

	QList<QGraphicsItem*> items = this->items();

	generateGraphics(listSessions, listLinks, mapSessions);

	// update scene, if show detail
	clearScene();


	for (QGraphicsItem* pItem : listSessions) this->addItem(pItem);
	for (QGraphicsItem* pItem : listLinks) this->addItem(pItem);

	_mapSessions = mapSessions;



	// 6.adjust the scene region
	QRectF rect = this->itemsBoundingRect();
	setSceneRect(rect.adjusted(-20, -20, 20, 20));
}

void MySessionScene::UpdateSessionWeight() {
	for (QString strId : _mapSessions.keys())
	{
		// if the session is invalid, the returned weight will be 0
		_mapSessions[strId]->SetDisplayWeight(_pSL->GetSessionW1(strId.toStdString())/ _pSL->GetMaxSW1());
		_mapSessions[strId]->SetWeightO(_pSL->GetSessionWO(strId.toStdString()));
	}
	update();
}

void MySessionScene::generateGraphics(QList<QGraphicsItem*>& listSessions
	, QList<QGraphicsItem*>& listLinks
	, QMap<QString, SLGSession*>& mapSessions) {

	// 0. calculate Location Y
	vector<string> listLocations = _pSL->GetDisplayedLocations();
	QMap<QString, int> mapLocationY;
	{
		QMap<QString, int> mapLocationSeq;
		QList<QString> listLocation;
		for (string strL : listLocations)
		{
			QString qstrL = QString::fromStdString(strL);
			mapLocationSeq[qstrL] = _pSL->GetLocationSeq(strL);
			listLocation.append(qstrL);
		}
		for (string strL : listLocations)
		{
			QString qstrL = QString::fromStdString(strL);
			listLocation[mapLocationSeq[qstrL]] = qstrL;
		}
		int nIndex = 0;
		while (nIndex < listLocation.length()) {
			if (_pSL->GetLW1(listLocation[nIndex].toStdString())>0)
			{
				nIndex++;
			}
			else {
				listLocation.removeAt(nIndex);
			}
		}
		for (int i = 0; i < listLocation.length(); i++)
		{
			mapLocationY[listLocation[i]] = i;
		}

	}
	// 1.generate graphics for sessions
	QMap<QString, QList<SLGSession*>> mapSinL;		// used for generate link in the same location

	vector<string> listSessionIds = _pSL->GetSessions();
	for (string strId : listSessionIds)
	{
		QString qstrId = QString::fromStdString(strId);
		SLGSession* pSession = new SLGSession();
		pSession->SetDisplayWeight(_pSL->GetSessionW1(strId) / _pSL->GetMaxSW1());

		//pSession->SetText(_pSL->GetSessionDsp(strId).first(2).toUpper());
		int nActorNum = _pSL->GetSessionActorNumber(strId);
		QString strActorNum = QString::number(nActorNum);
		if (nActorNum < 10)
			strActorNum = " " + strActorNum + " ";
		pSession->setText(strActorNum);

		
		pSession->setData(qstrId);
		listSessions.append(pSession);
		mapSessions[qstrId] = pSession;

		string strL = _pSL->GetSessionLocation(strId);
		QString qstrL = QString::fromStdString(strL);
		if (!mapSinL.keys().contains(qstrL))
		{
			mapSinL[qstrL] = QList<SLGSession*>();
		}
		mapSinL[qstrL].append(pSession);

		// set the position according to the storyline
		setNodePos(pSession, _pSL->GetTimeProjection(_pSL->GetSessionStart(strId)), mapLocationY[qstrL]);
	}


	// 2.generate links for sessions in the same location
	for (QString strL : mapSinL.keys())
	{
		if (mapSinL[strL].length() > 1) {
			for (size_t i = 1; i < mapSinL[strL].length(); i++)
			{
				SLGLink* pLink = new SLGLink(mapSinL[strL][i - 1], mapSinL[strL][i]);
				pLink->setPen(QPen(Qt::blue, 20, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
				pLink->setOpacity(.1);
				listLinks.append(pLink);
			}
		}
	}

	// 3.generate links between sessions
	for (string strId : listSessionIds)
	{
		vector<string> listSecs = _pSL->GetSessionSecs(strId);

		for (string strSec : listSecs)
		{
			QString qstrSec = QString::fromStdString(strSec);
			if (mapSessions.keys().contains(qstrSec)) {
				SLGLink* pLink = new SLGLink(mapSessions[QString::fromStdString(strId)], mapSessions[QString::fromStdString(strSec)]);
				if (_pSL->GetSessionStart(strSec) - _pSL->GetSessionStart(strId) > 1) {
					pLink->setPen(QPen(Qt::gray, 1, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin));
				}
				listLinks.append(pLink);
			}
		}
	}
}

void MySessionScene::GenerateScene(SLData* pData, SLOperator* pOperator) {

	// record data
	_pSL = pData;
	_pSLO = pOperator;

	// clear 
	clearScene();
	_mapSessions.clear();

	// generate graphics
	QList<QGraphicsItem*> listSessions;
	QList<QGraphicsItem*> listLinks; 
	QMap<QString, SLGSession*> mapSessions;

	generateGraphics(listSessions, listLinks, mapSessions);

	for (QGraphicsItem* pItem : listSessions) this->addItem(pItem);
	for (QGraphicsItem* pItem : listLinks) this->addItem(pItem);
	_mapSessions = mapSessions;


	// 6.adjust the scene region
	QRectF rect = this->itemsBoundingRect();
	setSceneRect(rect.adjusted(-20, -20, 20, 20));
}

void MySessionScene::moveLocations(int nRow) {
	// 1.get the locations to move

	// 1.1. get all the selected items
	QList<QGraphicsItem*> items = this->selectedItems();

	// 1.2. get the list of session indices of the selected items
	QList<QString> listSessionIds;
	for (QGraphicsItem* item : items)
	{
		SLGSession* pText = dynamic_cast<SLGSession*>(item);
		if (pText) {
			listSessionIds.append(pText->getData().toString());
		}
	}

	if (listSessionIds.isEmpty()) return;


	// 1.3. get the related locations by a set
	set<string> setL;
	for (QString strId : listSessionIds)
	{
		setL.insert(_pSL->GetSessionLocation(strId.toStdString()));
	}

	// 2.update StoryLine
	_pSL->LocationMove(setL, nRow);

	// 3.update graphic items
	{
		QList<QGraphicsItem*> items = this->items();
		for (QGraphicsItem* item : items)
		{
			SLGSession* pText = dynamic_cast<SLGSession*>(item);
			if (pText) {
				setNodePos(pText, _pSL->GetSessionStart(pText->getData().toString().toStdString()),
					_pSL->GetLocationSeq(_pSL->GetSessionLocation(pText->getData().toString().toStdString())));
			}
		}
	}

	// 4.tell storyline view
	emit UpdateStoryline();
}


void MySessionScene::onUpdateMovingSteps(int nSteps) {
	_nMovingSteps = nSteps;
}


