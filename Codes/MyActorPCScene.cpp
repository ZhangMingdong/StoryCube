#include "MyActorPCScene.h"

#include "SLData.h"
#include "SLGRect.h"
#include "SLGText.h"
#include "SLGActorNode.h"

#include <map>
#include <string>

using namespace std;

void SortStringByValue(vector<double>& listV, vector<string>& listS);

extern int getStrIndex(const vector<string>& list, const string& str) noexcept;

MyActorPCScene::MyActorPCScene(qreal x, qreal y, qreal width, qreal height, bool bO)
	: MyPCScene(x,y,width,height)
{
    _bO = bO;
	connect(this, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
}

MyActorPCScene::~MyActorPCScene()
{
}

void MyActorPCScene::generateScene(int nScale) noexcept
{
    // x projection and y projection of each actor
    std::map<std::string, double> mapX, mapY;
    constexpr int nRange = 1000;
    // Get actor lists and reserve space
    const auto& actors = generateActorProjection(mapX, mapY, nRange);
    if (actors.empty()) return;

    const int nLen = actors.size();


    vector<std::string> listAY = actors;
    vector<std::string> listAX = actors;
    vector<double> listPC0, listPC1;
    listPC0.reserve(nLen);
    listPC1.reserve(nLen);

    // Populate PC values
    for (size_t i = 0; i < nLen; ++i) {
        listPC0.push_back(mapY[listAY[i]]);
        listPC1.push_back(mapX[listAX[i]]);
    }

    // Sort with move semantics
    SortStringByValue(listPC0, listAY);
    SortStringByValue(listPC1, listAX);

    // Calculate spans
    std::vector<double> listSpanX, listSpanY;
    listSpanX.reserve(nLen - 1);
    listSpanY.reserve(nLen - 1);

    double dbMinSpanX = std::numeric_limits<double>::max();
    double dbMaxSpanX = 0;
    double dbMinSpanY = dbMinSpanX;
    double dbMaxSpanY = 0;

    for (int i = 1; i < nLen; ++i) {
        const double dbSpanY = std::abs(listPC0[i] - listPC0[i - 1]);
        const double dbSpanX = std::abs(listPC1[i] - listPC1[i - 1]);

        listSpanY.push_back(dbSpanY);
        listSpanX.push_back(dbSpanX);

        dbMinSpanY = std::min(dbMinSpanY, dbSpanY);
        dbMaxSpanY = std::max(dbMaxSpanY, dbSpanY);
        dbMinSpanX = std::min(dbMinSpanX, dbSpanX);
        dbMaxSpanX = std::max(dbMaxSpanX, dbSpanX);
    }

    // Adjust minimum spans
    constexpr double dbMaxScale = 0.01;
    dbMinSpanX = std::max(dbMinSpanX, dbMaxSpanX * dbMaxScale);
    dbMinSpanY = std::max(dbMinSpanY, dbMaxSpanY * dbMaxScale);

    // Scale spans
    const double scaleFactorX = (_nPjkScale - 1.0) / (c_nPjkScale - 1.0);
    const double scaleFactorY = (_nPjkScale - 1.0) / (c_nPjkScale - 1.0);

    for (size_t i = 0; i < nLen - 1; ++i) {
        listSpanX[i] = (listSpanX[i] < dbMinSpanX) ? 1.0 :
            std::max(1.0, ((listSpanX[i] / dbMinSpanX - 1.0) * scaleFactorX + 1.0));

        listSpanY[i] = (listSpanY[i] < dbMinSpanY) ? 1.0 :
            std::max(1.0, ((listSpanY[i] / dbMinSpanY - 1.0) * scaleFactorY + 1.0));
    }

    // Accumulate spans
    std::vector<double> listXAccum{ 0 }, listYAccum{ 0 };
    listXAccum.reserve(nLen);
    listYAccum.reserve(nLen);

    std::partial_sum(listSpanX.begin(), listSpanX.end(), std::back_inserter(listXAccum));
    std::partial_sum(listSpanY.begin(), listSpanY.end(), std::back_inserter(listYAccum));

    // Adjust scale
    const double dbScaleXY = listXAccum.back() / listYAccum.back();
    if (dbScaleXY > 1) {
        for (auto& y : listYAccum) y *= dbScaleXY;
    }
    else {
        for (auto& x : listXAccum) x /= dbScaleXY;
    }

    generateGraphics(nLen, listAX, listAY, listXAccum, listYAccum);
}

vector<string> MyActorPCScene::generateActorProjection(map<string, double>& mapX, map<string, double>& mapY, int nRange) noexcept {

    /*
	const vector<string>& listA = _pSL->GetActors();
    if (listA.empty()) return;
    int nLen = listA.size();

    vector<double> vPC0, vPC1;
    _pSL->GetActorPC(vPC0, vPC1);
    */

    vector<double> vPC0, vPC1;
    vector<string> listA = _pSL->GetActorPC(vPC0, vPC1,_bO);
    if (listA.empty()) return listA;
    int nLen = listA.size();



	// Initialize with first actor's values
    double dbYMin = vPC0[0];
    double dbYMax = dbYMin;

    double dbXMin = vPC1[0];
	double dbXMax = dbXMin;

	// First pass: find min/max and populate maps
    for (int i = 0; i < nLen; i++)
	{
        string strA = listA[i];

        double dbY = vPC0[i];
        double dbX = vPC1[i];

		dbXMin = std::min(dbXMin, dbX);
		dbXMax = std::max(dbXMax, dbX);
		dbYMin = std::min(dbYMin, dbY);
		dbYMax = std::max(dbYMax, dbY);

		mapX[strA] = dbX;
		mapY[strA] = dbY;
	}
	// Calculate scales (with protection against division by zero)
	const double dbXScale = dbXMax - dbXMin;
	const double dbYScale = dbYMax - dbYMin;
	const double xScaleFactor = (dbXScale != 0) ? nRange / dbXScale : 0;
	const double yScaleFactor = (dbYScale != 0) ? nRange / dbYScale : 0;

	// Second pass: normalize values
	for (string strL : listA)
	{
		mapX[strL] = (mapX[strL] - dbXMin) * xScaleFactor;
		mapY[strL] = (mapY[strL] - dbYMin) * yScaleFactor;
	}
    return listA;
}

void MyActorPCScene::generateGraphics(int nLen, const vector<string>& vStrX, const vector<string>& vStrY, const vector<double>& vX, const vector<double>& vY) {

    _mapNode.clear();

	double dbGridSize = 20.0;
	dbGridSize *= (nLen / (vX.back() + 1.0));
    /*
    * this line is used to keep the dbGridSize multiply grid number (vX.back()) stable
    */

	double dbGridScale = .9;

//    qDebug() << "dbGridSize: " << dbGridSize << "vX Range: " << vX.back() << "vY Range: " << vY.back();

	// generate nodes
	for (int i = 0; i < nLen; i++)
	{
		string strA = vStrY[i];
		int nIndexX = getStrIndex(vStrX, strA);
		//qDebug() << nIndex1;

		double dbX = dbGridSize * (vX[nIndexX] + .5);
		double dbY = dbGridSize * (vY[i] + .5);
		SLGActorNode* pNode = SLGActorNode::CreateSLGActorNode();
		pNode->setData(QString::fromStdString(strA));
		pNode->setX(dbX);
		pNode->setY(dbY);
		pNode->setRadius(dbGridSize * dbGridScale);
        pNode->SetDisplayWeight(_pSL->GetAW(strA) / _pSL->GetMaxAW());
		this->addItem(pNode);
        _mapNode[strA] = pNode;
	}

    /*
    // generate range
    int _nWidth = (vX.back() + 1.0) * dbGridSize;
    int _nHeight = (vY.back() + 1.0) * dbGridSize;
    SLGRect* rect = SLGRect::CreateSLGRect(SLIBasicGraphicItem::Type::None);
    rect->setX(_nWidth / 2.0);
    rect->setY(_nHeight / 2.0);
    rect->setWidth(_nWidth);
    rect->setHeight(_nHeight);
    this->addItem(rect);
    */
}


void MyActorPCScene::SetCued(std::vector<std::string> vCued) {
    qDebug() << "void MyActorPCScene::SetCued(std::vector<std::string> vCued) {" << vCued.size();
    for (auto it = _mapNode.begin(); it != _mapNode.end(); ++it) {
        it->second->SetCued(false);
    }
    for (string strA : vCued)
    {
        _mapNode[strA]->SetCued(true);
    }
    update();
}


void MyActorPCScene::handleClearSelection() {
    clearActorSelection();
}