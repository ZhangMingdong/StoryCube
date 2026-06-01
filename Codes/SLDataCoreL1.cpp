#include "SLDataCoreL1.h"

#include <iostream>

#include "SLSession.h"
#include "SLLocation.h"
#include "SLActor.h"

using namespace std;

SLDataCoreL1::~SLDataCoreL1() {
	// 遍历 map，释放每个 SLActor* 指向的内存
	for (auto& pair : _mapA) {
		delete pair.second;  // 释放指针指向的对象
	}

	for (auto& pair : _mapL) {
		delete pair.second;  // 释放指针指向的对象
	}
	for (auto& pair : _mapS) {
		delete pair.second;  // 释放指针指向的对象
	}
}

void SLDataCoreL1::GroupSupportActors(vector<string> listValidActors) {

	vector<string> listValidSupporters;
	for (int i = _listA.size() - 1; i >= 0; i--)
	{
		if (find(listValidActors.begin(), listValidActors.end(), _listA[i]) == listValidActors.end()) {
			bool bFindCompany = false;
			for (string strA : listValidSupporters)
			{
				if (checkCompany(strA, _listA[i])) {
					//qDebug() << strA << "<-" << _listA[i];
					_mapA[strA]->AddCompany(_listA[i]);
					_listA.erase(_listA.begin()+i);
					bFindCompany = true;
					break;
				}
			}
			if (!bFindCompany) listValidSupporters.push_back(_listA[i]);
		}
	}
	//	qDebug() << "SLDataCore::GroupSupportActors: " << _listA.size();
}

void SLDataCoreL1::GroupActors(vector<string> listValidActors) {

	// group them
	vector<string> listValidSupporters;
	for (int i = _listA.size() - 1; i >= 0; i--)
	{
		bool bFindCompany = false;
		for (string strA : listValidSupporters)
		{
			if (checkCompany(strA, _listA[i])) {
				//qDebug() << strA << "<-" << _listA[i];
				_mapA[strA]->AddCompany(_listA[i]);
				_listA.erase(_listA.begin()+i);
				bFindCompany = true;
				break;
			}
		}
		if (!bFindCompany) listValidSupporters.push_back(_listA[i]);
	}
	// check if a valid actor is treat as a company of support actor, swap them
	for (int i = 0; i < _listA.size(); i++)
	{
		if (find(listValidActors.begin(), listValidActors.end(),_listA[i])== listValidActors.end()) {
			vector<string> listC = _mapA.at(_listA[i])->GetCompanies();
			for (string strC : listC)
			{
				if (find(listValidActors.begin(), listValidActors.end(),strC)!= listValidActors.end()) {
					_mapA[_listA[i]]->ClearCompany();
					for (string strC2 : listC) {
						if (strC2 != strC) _mapA.at(strC)->AddCompany(strC2);
					}
					_mapA[strC]->AddCompany(_listA[i]);
					_listA[i] = strC;
					break;
				}
			}
		}
	}
}

void SLDataCoreL1::BindLocationActor() {
	// Initialize first location tracking
	int nFirstLocationT = std::numeric_limits<int>::max();
	string strFirstL;

	// Use const references to avoid copies
	const vector<string>& listLocations = _listL;
	const vector<string>& listActors = _listA;

	// 0. Clear location and actor information
	for (const string& strL : listLocations) {
		_mapL[strL]->ClearSessions();
		_mapL[strL]->SetMax(0);
	}
	for (const string& strA : listActors) {
		_mapA[strA]->ClearLocationInfo();
	}

	// 1. Process actor spans and build location-actor relationships
	for (const string& name : listActors) {
		SLActor* pA = _mapA[name];
		for (const SLSpan& span : pA->GetSpans()) {
			addActorToLocation(span._strLocationId, name, span._nT);

			// Track earliest location
			if (span._nT < nFirstLocationT) {
				nFirstLocationT = span._nT;
				strFirstL = span._strLocationId;
			}
		}
	}

	// Calculate max actor count per location

	for (const string& strL : _listL) {
		calculateLocationWidth(strL);
	}

	// 2. Build location sequence for each actor
	for (const string& name : listActors) {
		SLActor* pA = _mapA[name];
		int nLastTime = _nStart - 1;

		for (const SLSpan& span : pA->GetSpans()) {
			// Skip spans outside our time range
			if (span._nT > _nEnd || span._nT < _nStart) {
				continue;
			}

			// Fill time gaps with empty locations
			for (int i = nLastTime + 1; i < span._nT; ++i) {
				pA->AppendLocation("");
			}

			pA->AppendLocation(span._strLocationId);
			nLastTime = span._nT;
		}
	}

	_strFirstLocation = strFirstL;
}

void SLDataCoreL1::BuildSessionGraph() {
	for (string strName : _listA)
	{
		const SLActor* pA = _mapA[strName];
		int nLen = pA->GetSpans().size();
		for (int i = 0; i < nLen - 1; i++)
		{
			string strSessionId1 = GenerateSessionID(pA->GetSpans()[i]._nT, pA->GetSpans()[i]._strLocationId);
			string strSessionId2 = GenerateSessionID(pA->GetSpans()[i + 1]._nT, pA->GetSpans()[i + 1]._strLocationId);

			if (_mapS[strSessionId1]->AddSec(strSessionId2)) {
				if (pA->GetSpans()[i + 1]._nT <= pA->GetSpans()[i]._nT)
				{
					// this can not occur
					cout << "error\n";
				}
				_mapS[strSessionId2]->AddPre(strSessionId1);
			}
		}
	}
}

void SLDataCoreL1::addActorToLocation(string strL, string strA, int nT) {

	// generate session id
	string strS = GenerateSessionID(nT, strL);

	// add actor for session
	_mapS[strS]->AddActor(strA);

	// add session for location
	_mapL[strL]->AddSession(nT,strS);

}

bool SLDataCoreL1::checkCompany(string strA1, string strA2) {
	const SLActor* pA1 = _mapA[strA1];
	const SLActor* pA2 = _mapA[strA2];


	// find the index of the first span after _nStart
	int nIndex1 = 0;
	int nIndex2 = 0;

	// check spans in the visualization duration
	while (true) {
		// reach the end at the same time
		if (nIndex1 == pA1->GetSpans().size() && nIndex2 == pA2->GetSpans().size())
			return true;

		// only one reach the end
		if (nIndex1 == pA1->GetSpans().size() || nIndex2 == pA2->GetSpans().size())
			return false;


		// check time is same, otherwise return false
		if (pA1->GetSpans()[nIndex1]._nT != pA2->GetSpans()[nIndex2]._nT) return false;

		// check location is same, otherwise reture false;
		string strL1 = pA1->GetSpans()[nIndex1]._strLocationId;
		string strL2 = pA2->GetSpans()[nIndex2]._strLocationId;
		if (strL1 != strL2)
			return false;
		nIndex1++;
		nIndex2++;
	}
	cout << "Error: bool SLData::checkCompany(string strA1, string strA2) {\n";
	return true;
}


#include <QDebug>

void SLDataCoreL1::FilterActors() {
//	cout << "void SLDataCoreL1::FilterActors() {";
	// record how many scenes an actor appear
	int nLen = _listA.size();
	for (int i = nLen - 1; i >= 0; i--)
	{
		string strA = _listA[i];
		if (_mapA[strA]->GetSpans().size()<_nActorThreshold)
		{
			_listA.erase(_listA.begin() + i);
			_mapA.erase(strA);
			QString qstrA = QString::fromStdString(strA);
			qDebug()<<"filter actor: " << qstrA;
		}
	}
}
