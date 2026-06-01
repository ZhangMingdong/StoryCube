#include "SLData.h"
#include "SLOperator.h"

#include <QDebug>

using namespace std;

SLOperator::SLOperator(SLData* pData):_pData(pData) {

}

SLOperator::~SLOperator() {

}


void SLOperator::SelectLocations(vector<string> listL) {
	clearSelection();
	_listSelectedL = listL;
}
void SLOperator::SelectActors(vector<string> listA) {
	clearSelection();
	_listSelectedA = listA;

}
void SLOperator::SelectSessions(vector<string> listS) {
	clearSelection();
	_listSelectedS = listS;
}

void SLOperator::SelectLocation(string strL) {
	clearSelection();
	_listSelectedL.push_back(strL);
}
void SLOperator::SelectActor(string strA) {
	clearSelection();
	_listSelectedA.push_back(strA);
}
void SLOperator::SelectSession(string strS) {
	clearSelection();
	_listSelectedS.push_back(strS);
}

void SLOperator::ClearActorSelection() {
	_listSelectedA.clear();

}
void SLOperator::ClearLocationSelection() {
	_listSelectedL.clear();

}
void SLOperator::clearSelection() {
	_listSelectedL.clear();
	_listSelectedA.clear();
	_listSelectedS.clear();
}


string SLOperator::GetTips() {
	if (_listSelectedL.size() > 0)
	{
		string strResult;
		for (string strL : _listSelectedL)
		{
			strResult += _pData->GetLocationTips(strL);
		}
		return strResult;
	}

	if (_listSelectedA.size() > 0)
	{
		string strResult;
		for (string strA : _listSelectedA)
		{
			strResult += _pData->GetActorTips(strA);
		}
		return strResult;
	}


	if (_listSelectedS.size() > 0)
	{
		string strResult;
		for (string strS : _listSelectedS)
		{
			strResult += _pData->GetSessionTips(strS);
		}
		return strResult;
	}


	return "";
}