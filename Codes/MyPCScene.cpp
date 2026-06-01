#include "MyPCScene.h"
#include "SLData.h"

#include "SLGLink.h"
#include "SLGSession.h"
#include "SLGText.h"
#include "SLGRect.h"
#include "SLGLink.h"
#include "SLOperator.h"
#include "SLGFlow.h"
#include "SLGNode.h"

#include <QDebug>
#include <QTimer>

using namespace std;



MyPCScene::MyPCScene(qreal x, qreal y, qreal width, qreal height)
	: QGraphicsScene(x,y,width,height)
{
	connect(this, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
}

MyPCScene::~MyPCScene()
{
	clearScene();
}

void MyPCScene::GenerateScene(SLData* pData, SLOperator* pOperator) {

	// record data
	_pSL = pData;
	_pSLO = pOperator;

	// clear 
	clearScene();

	generateScene(_nPjkScale);

	updateSceneRect();
}

void MyPCScene::UpdateScene() {

	// clear 
	clearScene();

	generateScene(_nPjkScale);

	updateSceneRect();
}

void MyPCScene::clearScene() {
	QList<QGraphicsItem*> items = this->items();
	if (!items.empty())
	{
		qDeleteAll(items);
	}
	_listFlow.clear();
	_listStat.clear();

}

void MyPCScene::updateSceneRect() {
	/*
	* adjust the scene region
	* here cannot use the following two lines as there are invisible elements
	** QRectF rect = this->itemsBoundingRect();
	** setSceneRect(rect.adjusted(-20, -20, 20, 20));
	*/

	QRectF visibleBoundingRect;
	QList<QGraphicsItem*> itemsList = this->items();  // 获取所有元素
	bool firstItem = true;

	for (QGraphicsItem* item : itemsList) {
		if (item->isVisible()) {  // 仅考虑可见元素
			if (firstItem) {
				visibleBoundingRect = item->sceneBoundingRect();
				firstItem = false;
			}
			else {
				visibleBoundingRect = visibleBoundingRect.united(item->sceneBoundingRect());
			}
		}
	}

	// 设置新的场景可视范围
	setSceneRect(visibleBoundingRect);
}

void MyPCScene::onSelectionChanged() {
	const QList<QGraphicsItem*> items = selectedItems();
	const int itemCount = items.length();

	if (itemCount == 0)
		handleClearSelection();

	if (itemCount == 1) {
		handleSingleSelection(items.first());
	}
	else {
		handleMultiSelection(items);
	}

	QTimer::singleShot(0, this, &MyPCScene::UpdateTips);  // Unified signal emission
}

void MyPCScene::handleClearSelection() {

}
void MyPCScene::handleSingleSelection(QGraphicsItem* item) {
	if (SLIBasicGraphicItem* pItem = dynamic_cast<SLIBasicGraphicItem*>(item)) {
		const QStringView data = pItem->getData();
		const auto type = pItem->getType();

		qDebug() << "void MyPCScene::onSelectionChanged() {" << data;

		switch (type) {
		case SLIBasicGraphicItem::Type::Location:
			_pSLO->SelectLocation(data.toString().toStdString());
			break;
		case SLIBasicGraphicItem::Type::Actor:
			_pSLO->SelectActor(data.toString().toStdString());
			break;
		default:
			break;
		}
	}
}

void MyPCScene::handleMultiSelection(const QList<QGraphicsItem*>& items) {
	vector<string> listActors;
	vector<string> listLocations;

	for (QGraphicsItem* item : items) {
		if (SLIBasicGraphicItem* pItem = dynamic_cast<SLIBasicGraphicItem*>(item)) {
			const QStringView data = pItem->getData();

			switch (pItem->getType()) {
			case SLIBasicGraphicItem::Type::Location:
				listLocations.push_back(data.toString().toStdString());
				break;
			case SLIBasicGraphicItem::Type::Actor:
				listActors.push_back(data.toString().toStdString());
				break;
			default:
				break;
			}
		}
	}

	if (!listActors.empty()) {
		_pSLO->SelectActors(listActors);
	}
	if (!listLocations.empty()) {
		_pSLO->SelectLocations(listLocations);
	}
}

void MyPCScene::onChangeScale(int nScale) {
//	qDebug() << "void MyPCScene::onChangeScale(int nScale) {";
//	qDebug() << nScale;
	clearScene();
	_nPjkScale = nScale;
	generateScene(nScale);

	if (nScale>1)
	{
		// hide flow and statistics
		for (SLGFlow* flow : _listFlow)
		{
			flow->hide();
		}
		for (SLGRect* bar : _listStat)
		{
			bar->hide();
		}

	}

}

void MyPCScene::onFlowVisibilityChanged(int nState) {
	if (_bFlowVisibilityChanged && nState==0)
	{
		for (SLGFlow* flow : _listFlow)
		{
			flow->hide();
		}
		
	}
	else if (!_bFlowVisibilityChanged && nState == 2) {
		for (SLGFlow* flow : _listFlow)
		{
			if(_nPjkScale ==1)
				flow->show();
		}

	}
	_bFlowVisibilityChanged = nState;
	
}

void MyPCScene::onStatVisibilityChanged(int nState) {
	if (_bStatVisibilityChanged && nState == 0)
	{
		for (SLGRect* bar : _listStat)
		{
			bar->hide();
		}

	}
	else if (!_bStatVisibilityChanged && nState == 2) {
		for (SLGRect* bar : _listStat)
		{
			if (_nPjkScale == 1)
				bar->show();
		}

	}
	_bStatVisibilityChanged = nState;
	updateSceneRect();
}

void MyPCScene::clearLocationSelection() {
	_pSLO->ClearLocationSelection();
}
void MyPCScene::clearActorSelection() {
	_pSLO->ClearActorSelection();
}