#include "SLDataCore.h"

#include <set>
#include <vector>
#include <iostream>

#include "SLActor.h"
#include "SLLocation.h"
#include "SLSession.h"




using namespace std;
using namespace Eigen;


bool g_bStatVisibilityChangedistics = true;

extern int getStrIndex(const vector<string>& list, const string& str) noexcept;


SLDataCore::~SLDataCore() {
}

void SLDataCore::clear() {
	_mapA.clear();
	_mapL.clear();
	_mapS.clear();
	_listA.clear();
	_listL.clear();
	_listS.clear();
}

bool SLDataCore::CreateValidData(SLDataCore& dataValid, vector<string> actorValid) const {
	// 0.clear old map
	dataValid.clear();

	// 1.check valid actor list
	if (actorValid.empty())
	{
		return false;
	}

	// 2.set listA as the valid actors
	dataValid._listA = actorValid;


	set<string> setValidSession;
	set<string> setValidLocation;

	/*
	* 3.create _mapA for the valid actors
	* create the data for valid actors; 
	* create support actor list;
	* create  lists of valid location and session
	*/
	vector<string> listSupportActor = _listA;
	for (string strA : actorValid)
	{
		listSupportActor.erase(find(listSupportActor.begin(), listSupportActor.end(),strA));
		SLActor* pA = new SLActor(_mapA.at(strA));
		pA->SetTimeRange(dataValid._nStart, dataValid._nEnd);
		pA->CollectValidLocationAndSession(setValidLocation, setValidSession);
		dataValid._mapA[strA] = pA;
	}

	// 4.crate _strL and _mapL according to the valid location set
	for (string strL : setValidLocation)
	{
		dataValid._listL.push_back(strL);
		SLLocation* pL = new SLLocation(_mapL.at(strL)->GetName());
		pL->SetDisplayName(_mapL.at(strL)->GetDisplayName());
		dataValid._mapL[strL] = pL;
	}

	// 5.create _strS and _mapS according to the valid session set
	for (string strS : setValidSession)
	{
		dataValid._listS.push_back(strS);
		dataValid._mapS[strS] = new SLSession(_mapS.at(strS)->GetT(), _mapS.at(strS)->GetL(), _mapS.at(strS)->GetDsp());
	}

	// 6.add actors of support actors, only if it contains valid sessions
	for (string strA : listSupportActor)
	{
		SLActor* pA =new SLActor(_mapA.at(strA));
		pA->Filter(setValidSession);
		if (pA->IsValid())
		{
			dataValid._mapA[strA] = pA;
			dataValid._listA.push_back(strA);
		}
		else
			delete pA;
	}

	return true;
}

void SLDataCore::CreatePreVisData(SLDataCore& dataDisplay) const {

	dataDisplay._nStart = _nStart;
	dataDisplay._nEnd = _nEnd;

	// listA
	dataDisplay._listA = _listA;
	dataDisplay._listL = _listL;
	dataDisplay._listS = _listS;
	dataDisplay._mapA = _mapA;
	dataDisplay._mapL = _mapL;
	dataDisplay._mapS = _mapS;

	dataDisplay._listLClusters.clear();
	dataDisplay._listLocationMerge.clear();



	dataDisplay._strFirstLocation = _strFirstLocation;

}

void SLDataCore::CreateVisData(SLDataCore& dataDisplay) const {
	CreatePreVisData(dataDisplay);


	dataDisplay._D_Location = _D_Location;			
	dataDisplay._D_Actor = _D_Actor;
	dataDisplay._listLocationIdsPC1 = _listLocationIdsPC1;
	dataDisplay._listLClusters = _listLClusters;
	dataDisplay._nMaxDepth = _nMaxDepth;

	dataDisplay._dbMaxAW = _dbMaxAW;
	dataDisplay._dbMaxLW = _dbMaxLW;
	dataDisplay._dbMaxAWO = _dbMaxAWO;
	dataDisplay._dbMaxLWO = _dbMaxLWO;
	dataDisplay._dbMaxAW1 = _dbMaxAW1;
	dataDisplay._dbMaxLW1 = _dbMaxLW1;
	dataDisplay._dbMaxSW1 = _dbMaxSW1;
	dataDisplay._listTimestepW = _listTimestepW;
	dataDisplay._listTimeProjection = _listTimeProjection;
	dataDisplay._listLocationMerge = _listLocationMerge;


	dataDisplay._listIdsRef = _listIdsRef;
	dataDisplay._listPC0Ref = _listPC0Ref;
	dataDisplay._listPC1Ref = _listPC1Ref;
	dataDisplay._bInit = true;


	for (string strL : _listLClusters)
	{
		dataDisplay._mapL[strL] = _mapL.at(strL);
	}
	if (g_bStatVisibilityChangedistics) {
		cout << "Actors: " << _listA.size() << endl;
		cout << "Locations:" << _listL.size() << endl;
		cout << "Sences:" << _listS.size() << endl;
	}
	//for (string strName : _listA) cout << strName << "\n";
}

void SLDataCore::SetTimeRange(const SLDataCore& data) {
	_nStart = data._nStart;
	_nEnd = data._nEnd;
}

void SLDataCore::SetTimeRange(int nStart, int nEnd) {
	_nStart = nStart;
	_nEnd = nEnd;
}

bool SLDataCore::SetActorThreshold(int nThreshold) {

	if (_nActorThreshold == nThreshold) return false;

	_nActorThreshold = nThreshold;
	return true;
}

vector<string> SLDataCore::GetValidActors(int nStart, int nEnd) {
	vector<string> listValidActor;
	if (nStart == _nStart && nEnd == _nEnd)
	{
		listValidActor = _listA;
	}
	else if (nStart >= _nStart && nEnd <= _nEnd) {

		listValidActor.clear();

		for (string strA : _listA)
		{
			const SLActor* pA = _mapA[strA];
			for (const SLSpan& span : pA->GetSpans())
			{
				if (span.CheckValidity(nStart, nEnd)) {
					listValidActor.push_back(strA);
					break;
				}
			}
		}
	}
	else {
		listValidActor.clear();
		cout << "time range error\n";
	}
	return listValidActor;
}

vector<string> SLDataCore::GetActors() {
	return _listA;
}

vector<string> SLDataCore::GetLocations() {
	return _listL;
}

vector<string> SLDataCore::GetLocationsBySeq() {
	vector<string> listL = _listL;
	// 使用 lambda 表达式和标准库排序算法
	sort(listL.begin(), listL.end(),
		[this](const string& a, const string& b) {
			return _mapL[a]->GetSequence() < _mapL[b]->GetSequence();
		});
	return listL;
}

vector<string> SLDataCore::GetSessions() {
	//qDebug() << "vector<string> SLDataCore::GetSessions() {" << _listL;
	vector<string> vS;
	for (string strL:_listL)
	{
		_mapL[strL]->GetSessions(vS);
	}

	// sort sessions according first to the time then to the location sequence
	const map<string, SLSession*>& mapS = _mapS;
	const map<string, SLLocation*>& mapL = _mapL;
	sort(vS.begin(), vS.end(),
		[&mapS,&mapL](const string& a, const string& b) {
			if (mapS.at(a)->GetT() == mapS.at(b)->GetT()) {
				return mapL.at(mapS.at(a)->GetL())->GetSequence() < mapL.at(mapS.at(b)->GetL())->GetSequence();
			}
			else return mapS.at(a)->GetT() < mapS.at(b)->GetT();
			//return mapA.at(a)->GetSortingWeight() < mapA.at(b)->GetSortingWeight();
		});

	return vS;
}

string SLDataCore::GetActorTips(string strId) const {

	if (_mapA.find(strId)==_mapA.end()) {
		return "null\n";
	}
	SLActor* pA = _mapA.at(strId);
	string strText = "A: " + pA->GetName();
	for (string c : pA->GetCompanies())
	{
		strText = strText + ", " + c;
	}
	strText += "\n";
	return strText;
}

string SLDataCore::GetLocationTips(string strId) const
{
	ostringstream oss;
	//oss << "L: " << _mapL.at(strId)->GetDisplayName() << "\nDepth: " << _mapL.at(strId)->GetD2R()<<"\n";
	oss << "L: " << _mapL.at(strId)->GetDisplayName() << "\n";
	return oss.str();
}

string SLDataCore::GetSessionTips(string strId) const
{
	string strL = _mapL.at(_mapS.at(strId)->GetL())->GetDisplayName();
	string strDsp = _mapS.at(strId)->GetDsp();
	int nT = _mapS.at(strId)->GetT();
	vector<string> vAs = _mapS.at(strId)->GetActors();
	ostringstream oss;
	oss << "S: " << strDsp
		<< "\r\nT: " << nT
		<< "\r\nL: " << strL
//		<< "\r\nWO: " << _mapS.at(strId)->GetWO()
//		<< "\tW1: " << _mapS.at(strId)->GetW1()
		<< "\r\nA: " << std::accumulate(std::next(vAs.begin()), vAs.end(), vAs.front(),
			[](std::string a, const std::string& b) {
				return std::move(a) + "; " + b;
			})
		<< "\r\n";
	return oss.str();
}

string SLDataCore::GetSessionLocation(string strS) const {
	string strL = _mapS.at(strS)->GetL();
	while (true) {
		if (find(_listL.begin(), _listL.end(),strL)!= _listL.end()) return strL;
		else {
			strL = _mapL.at(strL)->GetCluster();
		}
		if (strL == "") return "";
	}
}

string SLDataCore::GetDisplayLocation(string strL) const {
	while (true) {
		if (find(_listL.begin(), _listL.end(),strL)!= _listL.end()) return strL;
		else {
			strL = _mapL.at(strL)->GetCluster();
		}
		if (strL == "") return "";
	}
}

int SLDataCore::GetSessionStart(string strS) const {
	return _mapS.at(strS)->GetT();
}

int SLDataCore::GetActorSeqInSession(string strA, int nT, string strL) {
	
	string strS = _mapL[strL]->GetSession(nT);
	if (_mapS.find(strS) == _mapS.end())
	{
		return -1;
	}
	return _mapS[strS]->GetActorSequence(strA);
}

void SLDataCore::GetActorSequence(int nT, string strA, int& nSeq, int& nGroup, int& nGroupSeq) const {
	const SLActor* pA = _mapA.at(strA);
	int nIndex = nT - _nStart;

	string strL1 = pA->GetL(nIndex);
	string strS1 = GenerateSessionID(nT, strL1);
	vector<string> listA1 = _mapS.at(strS1)->GetActors();
	nSeq = getStrIndex(listA1, strA);

	if (nIndex - 1 > 0) {
		string strL2 = pA->GetL(nIndex - 1);
		if (strL2 != "")
		{
			string strS2 = GenerateSessionID(nT-1, strL2);
			vector<string> listA2 = _mapS.at(strS2)->GetActors();

			vector<string> listGA;
			for (string strGA : listA1)
			{
				if (find(listA2.begin(), listA2.end(),strGA)!= listA2.end())
					listGA.push_back(strGA);
			}
			nGroup = listGA.size();
			nGroupSeq = getStrIndex(listGA,strA);
			return;
		}
	}

	nGroup = 0;
	nGroupSeq = 0;
	return;
}

double SLDataCore::GetAW(string strA) {
	if (_mapA.find(strA) == _mapA.end()) return 0;
	return _mapA[strA]->GetW();
}

double SLDataCore::GetLW(string strL) {
	if (_mapL.find(strL) == _mapL.end()) return 0;
	return _mapL[strL]->GetW();
}

double SLDataCore::GetAW1(string strA) {
	if (_mapA.find(strA) == _mapA.end()) return 0;
	return _mapA[strA]->GetW1();
}

double SLDataCore::GetLW1(string strL) {
	if (_mapL.find(strL) == _mapL.end()) return 0;
	return _mapL[strL]->GetW1();
}

double SLDataCore::GetAWO(string strA) {
	if (_mapA.find(strA) == _mapA.end()) return 0;
	return _mapA[strA]->GetWO();
}

double SLDataCore::GetLWO(string strL) {
	if (_mapL.find(strL) == _mapL.end()) return 0;
	return _mapL[strL]->GetWO();
}

void SLDataCore::SetAWO(string strA, double dbW) {
	_mapA[strA]->SetWO(dbW);
}

void SLDataCore::SetLWO(string strL, double dbW) {
	_mapL[strL]->SetWO(dbW);
}

double SLDataCore::GetSWO(string strS) {
	return _mapS[strS]->GetWO();
}

double SLDataCore::GetSW1(string strS) {
	return _mapS[strS]->GetW1();
}

void SLDataCore::SetSWO(string strS, double dbW) {
	_mapS[strS]->SetWO(dbW);

}

map<string, double> SLDataCore::GetAWMap() {
	map<string, double> mapW;
	for (string strA : _listA)
	{
		mapW[strA] = _mapA[strA]->GetW1();
	}
	return mapW;
}

map<string, double> SLDataCore::GetAWOMap() {
	map<string, double> mapW;
	for (string strA : _listA)
	{
		mapW[strA] = _mapA[strA]->GetWO();
	}
	return mapW;

}

map<string, double> SLDataCore::GetLWMap() {
	map<string, double> mapW;

	return mapW;
}

map<string, double> SLDataCore::GetLWOMap() {
	map<string, double> mapW;

	return mapW;
}

double SLDataCore::GetSessionW1(string strS) {
	if (!_mapS.contains(strS))
	{
		return 0;
	}

	return _mapS[strS]->GetW1();
}

double SLDataCore::GetSessionWO(string strS) {
	if (!_mapS.contains(strS))
	{
		return 0;
	}
	return _mapS[strS]->GetWO();
}

string SLDataCore::GetSessionDsp(string strId) {
	return _mapS[strId]->GetDsp();
}

vector<string> SLDataCore::GetSessionSecs(string strS) const {
	return _mapS.at(strS)->GetSecs();
}

int SLDataCore::GetTimeLen() {
	if (_listTimeProjection.empty())
		return 0;
	else
		return _listTimeProjection.back() + 1; 
}

int SLDataCore::GetSessionActorNumber(string strS) {
	return _mapS[strS]->GetActors().size();
}

int SLDataCore::GetLocationDepth() {
	return _nMaxDepth;
}

void SLDataCore::sortActorsGlobally() {

	// 1.build weights
	for (string strA : _listA) {
		SLActor* pA = _mapA[strA];
		//		qDebug()<<"========" << actor.GetName();
		double dbW = 0;
		int nLastTime = -1;
		int nLastLayer = -1;
		const vector<SLSpan>& listSpans = pA->GetSpans();
		for (const SLSpan& span : listSpans) {
			int nLayer = getStrIndex(_listL, span._strLocationId);
			if (nLayer == -1) {
				cout << "error: void SLData::sortActors() {\n";
			}
			if (nLastTime > -1) {
				double dbTimeSpan = span._nT - nLastTime;
				dbW += (nLayer + nLastLayer) / 2.0 * dbTimeSpan;
				//				qDebug() << nLastLayer << "\t" << nLayer << "\t" << dbTimeSpan << "\t" << (nLayer + nLastLayer) / dbTimeSpan;
			}
			nLastTime = span._nT;
			nLastLayer = nLayer;
		}
		//		qDebug() << listSpans.constLast()._nStart - listSpans.constFirst()._nStart;
		pA->SetDisplayWeight(dbW / (listSpans.back()._nT - listSpans.front()._nT));
	}

	// 2.sort actors globally
	for (size_t i = 0; i < _listA.size(); i++) {
		for (size_t j = i + 1; j < _listA.size(); j++) {
			if (_mapA[_listA[i]]->GetSortingWeight() > _mapA[_listA[j]]->GetSortingWeight())
			{
				swap(_listA[i], _listA[j]);
			}
		}
	}

	// 3.sort actors in each location
	for (string strS : _listS)
	{
		_mapS[strS]->SortActors(this);
	}
}

void SLDataCore::sortActorsInLocations() {


	// 1.forward sorting			
	for (int i = 1; i < getTimeRange(); i++) // loop each timestep
	{
		sortActor(i, i - 1);
	}

	// 2.backward sorting
	for (int i = getTimeRange() - 1; i >= 0; i--) {// sort again another direction
		sortActor(i, i + 1);
	}
}

void SLDataCore::sortActor(int nTimeIndex, int nRefTimeIndex) {

	for (string strS : _listS)
	{
		_mapS[strS]->SortActors(nTimeIndex, nRefTimeIndex, this);
	}
}

void SLDataCore::SortActors() {
	cout << "void SLData::sortActors() {\n";

	if (_listA.size() <= 2) return;

	// sort globally
	sortActorsGlobally();

	// sort locally
	sortActorsInLocations();
}

void SLDataCore::tryToSwap(vector<string>& listA, int nTimeIndex, int nRefTimeIndex, int j, int k) const {

	// 0. get the actors tried to swap
	const SLActor* pAJ = _mapA.at(listA[j]);
	const SLActor* pAK = _mapA.at(listA[k]);


	// 1. get location sequence of actor j and k
	const vector<string>& listLocationsJ = pAJ->GetLocations();
	const vector<string>& listLocationsK = pAK->GetLocations();

	// 2. check if this actor not exist in the reference index
	if (listLocationsJ.size() <= nRefTimeIndex)
		return;
	if (listLocationsK.size() <= nRefTimeIndex)
		return;


	int seqJ = -1;
	int seqK = -1;

	// 3. get the valid reference time index of actor j and k
	int nValidRefJ = nRefTimeIndex;
	int nValidRefK = nRefTimeIndex;


	if (nRefTimeIndex > nTimeIndex)	// according to the latter time
	{
		while (nValidRefJ < listLocationsJ.size() && listLocationsJ[nValidRefJ] == "")
			nValidRefJ++;
		if (nValidRefJ < listLocationsJ.size())
			seqJ = _mapL.at(listLocationsJ[nValidRefJ])->GetSequence();

		while (nValidRefK < listLocationsK.size() && listLocationsK[nValidRefK] == "")
			nValidRefK++;
		if (nValidRefK < listLocationsK.size())
			seqK = _mapL.at(listLocationsK[nValidRefK])->GetSequence();
	}
	else {
		while (nValidRefJ >= 0 && listLocationsJ[nValidRefJ] == "")
			--nValidRefJ;
		if (nValidRefJ >= 0)
			seqJ = _mapL.at(listLocationsJ[nValidRefJ])->GetSequence();

		while (nValidRefK >= 0 && listLocationsK[nValidRefK] == "")
			--nValidRefK;
		if (nValidRefK >= 0)
			seqK = _mapL.at(listLocationsK[nValidRefK])->GetSequence();
	}

	// 4. check if the reference is valid
	if (seqJ == -1 || seqK == -1) return;

	// 5. check if need swap
	if (seqJ > seqK) {
		// location order dominate, swap
		swap(listA[j], listA[k]);
	}
	else if (seqJ == seqK) {

		string strL = listLocationsJ[nValidRefJ];
		string strSJ = GenerateSessionID(_nStart + nValidRefJ, strL);
		string strSK = GenerateSessionID(_nStart + nValidRefK, strL);


		const SLSession* sessionJ = _mapS.at(strSJ);
		const SLSession* sessionK = _mapS.at(strSK);


		seqJ = getStrIndex(sessionJ->GetActors(),listA[j]);
		seqK = getStrIndex(sessionK->GetActors(),listA[k]);
		if (seqJ > seqK)
		{
			swap(listA[j], listA[k]);
		}
	}
}

string SLDataCore::mergeSession(string strL, string strS1, string strS2) {
	const SLSession* s1 = _mapS[strS1];
	const SLSession* s2 = _mapS[strS2];
	assert(s1->GetT() == s2->GetT());

	// Create new merged session
	SLSession* newS = new SLSession(s1->GetT(), strL, s1->GetDsp() + s2->GetDsp());

	// Get actors from both sessions
	vector<string> listA1 = s1->GetActors();
	vector<string> listA2 = s2->GetActors();

	// Combine and deduplicate actors using std::set
	std::set<string> uniqueActors(listA1.begin(), listA1.end());
	uniqueActors.insert(listA2.begin(), listA2.end());

	// Add all unique actors to the new session
	for (const string& strA : uniqueActors) {
		newS->AddActor(strA);
	}

	// Store and return the new session
	string strS = GenerateSessionID(s1->GetT(), strL);
	_mapS[strS] = newS;
	return strS;
}

void SLDataCore::calculateLocationWidth(string strL) {
	auto& location = _mapL[strL];
	for (const string& strS : location->GetSessions()) {
		const auto& session = _mapS[strS];
		location->ExtendActorSize(session->GetActors().size());
	}
}


void SLDataCore::sortActors(vector<string>& vA) {
	const map<string, SLActor*>& mapA = _mapA;
	// sort actors according to its GetSortingWeight();
	sort(vA.begin(), vA.end(),
		[&mapA](const string& a, const string& b) {
			return mapA.at(a)->GetSortingWeight() < mapA.at(b)->GetSortingWeight();
		});
}