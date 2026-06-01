#include "MyLocationPCScene.h"

#include "SLData.h"
#include "SLGRect.h"
#include "SLGText.h"
#include "SLGNode.h"
#include "SLGFlow.h"

#include "SLData.h"

#include <map>
#include <string>


using namespace std;


extern int getStrIndex(const vector<string>& list, const string& str) noexcept;

MyLocationPCScene::MyLocationPCScene(qreal x, qreal y, qreal width, qreal height, bool bO)
	: MyPCScene(x,y,width,height)
{
	_bO = bO;
	connect(this, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
}

MyLocationPCScene::~MyLocationPCScene() {

}

void MyLocationPCScene::generateScene(int nScale) {

	// clear old nodes
	_mapNode.clear();

	// get the location list and coordinates
	vector<double> listCY, listCX, listSpanX, listSpanY;
	vector<string> listLY, listLX;

	_pSL->GenerateLocationCoordinates(listCY, listCX, listSpanY, listSpanX, listLY, listLX, _bO);

	// no location, return
	if (listCY.size() < 1) return;

	// only one location
	if (listCY.size() == 1) {
		string strL = listLY[0];
		SLGNode* rect = SLGNode::CreateSLGNode();
		rect->setData(QString::fromStdString(strL));
		rect->setX(0);
		rect->setY(0);
		rect->setWidth(20);
		rect->setHeight(20);
		rect->SetDisplayWeight(_pSL->GetLW1(strL) / _pSL->GetMaxLW());
		//location.GetMax
		this->addItem(rect);
		_mapNode[strL] = rect;
		return;
	}

	int nLen = listLY.size();

	double dbMinSpanX = *std::min_element(listSpanX.begin(), listSpanX.end());
	double dbMaxSpanX = *std::max_element(listSpanX.begin(), listSpanX.end());
	double dbMinSpanY = *std::min_element(listSpanY.begin(), listSpanY.end());
	double dbMaxSpanY = *std::max_element(listSpanY.begin(), listSpanY.end());

	// reset the minimum span according to a given scale
	double dbMaxScale = 0.01;
	if (dbMinSpanX < dbMaxSpanX * dbMaxScale) dbMinSpanX = dbMaxSpanX * dbMaxScale;
	if (dbMinSpanY < dbMaxSpanY * dbMaxScale) dbMinSpanY = dbMaxSpanY * dbMaxScale;

	// scale the span
	for (int i = 0; i < nLen - 1; i++)
	{
		if (listSpanX[i] < dbMinSpanX) listSpanX[i] = dbMinSpanX;
		else listSpanX[i] = (listSpanX[i] / dbMinSpanX - 1.0) / (c_nPjkScale - 1.0) * (_nPjkScale - 1.0) + 1.0;
		if (listSpanY[i] < dbMinSpanY) listSpanY[i] = dbMinSpanY;
		else listSpanY[i] = (listSpanY[i] / dbMinSpanY - 1.0) / (c_nPjkScale - 1.0) * (_nPjkScale - 1.0) + 1.0;

		if (listSpanX[i] < 1.0) listSpanX[i] = 1.0;
		if (listSpanY[i] < 1.0) listSpanY[i] = 1.0;

	}

	// accumulate the span
	QList<double> listXAccum, listYAccum;
	listYAccum.append(0);
	listXAccum.append(0);
	for (double dbSpan : listSpanY)
	{
		listYAccum.append(listYAccum.last() + dbSpan);
	}
	for (double dbSpan : listSpanX)
	{
		listXAccum.append(listXAccum.last() + dbSpan);
	}
	double dbScaleXY = listXAccum.last() / listYAccum.last();
	if (dbScaleXY > 1)
	{
		for (int i = 0; i < nLen; i++) {
			listYAccum[i] *= dbScaleXY;
		}
	}
	else {
		for (int i = 0; i < nLen; i++) {
			listXAccum[i] /= dbScaleXY;
		}
	}


	double dbGridSize = 20.0;
	dbGridSize *= (nLen / (listXAccum.last() + 1.0));



	double dbGridScale = .9;
	int _nWidth = (listXAccum.last() + 1.0) * dbGridSize;
	int _nHeight = (listYAccum.last() + 1.0) * dbGridSize;

	double dbRadius = dbGridSize * dbGridScale;
	if (dbRadius < 20) dbRadius = 20;
	// generate nodes
	for (int i = 0; i < nLen; i++)
	{
		string strL = listLY[i];
		int nIndexX = getStrIndex(listLX, strL);
		//qDebug() << nIndex1;

		SLGNode* rect = SLGNode::CreateSLGNode();
		rect->setData(QString::fromStdString(strL));
		rect->setX(dbGridSize * (listXAccum[nIndexX] + .5));
		rect->setY(dbGridSize * (listYAccum[i] + .5));
		rect->setWidth(dbRadius);
		rect->setHeight(dbRadius);
		rect->SetDisplayWeight(_pSL->GetLW1(strL) / _pSL->GetMaxLW());
		//location.GetMax
		this->addItem(rect);
		_mapNode[strL] = rect;
	}

//	qDebug() << "dbGridSize" << dbGridSize << listXAccum.last() << dbGridSize * dbGridScale;


	// generate links
	Eigen::MatrixXi mStates(listLY.size(), listLY.size());
	_pSL->GetTransitionGraph(mStates,_bO);
	int nMaxFlow = mStates.maxCoeff();

	for (int i = 0; i < nLen; i++)
	{
		for (int j = 0; j < nLen; j++)
		{
			if (mStates(i, j) > 0) {
				SLGFlow* flow = SLGFlow::CreateSLGFlow(_mapNode[listLY[i]], _mapNode[listLX[j]], mStates(i, j) * dbGridSize / nMaxFlow);
				_listFlow.append(flow);
				this->addItem(flow);
				if (!_bFlowVisibilityChanged) flow->hide();
			}
		}
	}
	Eigen::VectorXi rowSums = mStates.rowwise().sum();
	Eigen::VectorXi colSums = mStates.colwise().sum();
	Eigen::VectorXi nonZeroCountsR = (mStates.array() != 0).cast<int>().rowwise().sum();
	Eigen::VectorXi nonZeroCountsC = (mStates.array() != 0).cast<int>().colwise().sum();
	int max1 = rowSums.maxCoeff();
	int max2 = colSums.maxCoeff();
	int max3 = nonZeroCountsR.maxCoeff();
	int max4 = nonZeroCountsC.maxCoeff();
	int maxVal = std::max({ max1,max2,max3,max4 });

	// generate bars of count
	for (int i = 0; i < nLen; i++)
	{
		double dbBarScale = .6;
		double dbBarLen = 100;
		int nBarMargin = 5;
		// R count
		{

			int nCountR = nonZeroCountsR(i);
			SLGRect* rect = SLGRect::CreateSLGRect(SLIBasicGraphicItem::Type::None);
			rect->setY(dbGridSize * (listYAccum[i] + .5));
			rect->setHeight(dbGridSize * dbBarScale);
			int nW = nCountR * dbBarLen / maxVal;
			rect->setWidth(nW);
			rect->setX(-nW / 2 - nBarMargin);
			this->addItem(rect);
			_listStat.append(rect);
			if (!_bStatVisibilityChanged) rect->hide();
		}
		// R weight
		{
			int nCountR = rowSums(i);
			SLGRect* rect = SLGRect::CreateSLGRect(SLIBasicGraphicItem::Type::None);
			rect->setY(dbGridSize * (listYAccum[i] + .5));
			rect->setHeight(dbGridSize * dbBarScale);
			int nW = nCountR * dbBarLen / maxVal;
			rect->setWidth(nW);
			rect->setX(_nWidth + nW / 2 + nBarMargin);
			this->addItem(rect);
			_listStat.append(rect);
			if (!_bStatVisibilityChanged) rect->hide();
		}
		// C count
		{
			int nCountC = nonZeroCountsC(i);
			SLGRect* rect = SLGRect::CreateSLGRect(SLIBasicGraphicItem::Type::None);
			rect->setX(dbGridSize * (listXAccum[i] + .5));
			rect->setWidth(dbGridSize * dbBarScale);
			int nH = nCountC * dbBarLen / maxVal;
			rect->setHeight(nH);
			rect->setY(-nH / 2 - nBarMargin);
			this->addItem(rect);
			_listStat.append(rect);
			if (!_bStatVisibilityChanged) rect->hide();
		}
		// C weight
		{
			int nCountC = colSums(i);
			SLGRect* rect = SLGRect::CreateSLGRect(SLIBasicGraphicItem::Type::None);
			rect->setX(dbGridSize * (listXAccum[i] + .5));
			rect->setWidth(dbGridSize * dbBarScale);
			int nH = nCountC * dbBarLen / maxVal;
			rect->setHeight(nH);
			rect->setY(_nHeight + nH / 2 + nBarMargin);
			this->addItem(rect);
			_listStat.append(rect);
			if (!_bStatVisibilityChanged) rect->hide();
		}
	}

	/*
	// generate range
	SLGRect* rect = SLGRect::CreateSLGRect(SLIBasicGraphicItem::Type::None);
	rect->setX(_nWidth / 2.0);
	rect->setY(_nHeight / 2.0);
	rect->setWidth(_nWidth);
	rect->setHeight(_nHeight);
	this->addItem(rect);
	*/
}


void MyLocationPCScene::SetCued(std::vector<std::string> vCued) {
	for (auto it = _mapNode.begin(); it != _mapNode.end(); ++it) {
		it->second->SetCued(false);
	}
	for (string strA : vCued)
	{
		_mapNode[strA]->SetCued(true);
	}
	update();
}

void MyLocationPCScene::handleClearSelection() {
	clearLocationSelection();
}