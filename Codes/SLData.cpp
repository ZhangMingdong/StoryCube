#include "SLData.h"

#include <algorithm>
#include <set>
#include <iostream>

#include "SLDefinition.h"
#include "SLLocation.h"
#include "SLActor.h"
#include "SLSession.h"
#include "SLDataCoreL0.h"
#include "SLDataCoreL1.h"
#include "SLDataCoreL2.h"
#include "SLDataCoreV.h"

#include <QDebug>
#include <chrono>

using namespace Eigen;
using namespace std;

bool g_bShowActorsGrouping = false;

SLData::SLData()
{
}

SLData::~SLData()
{
	if (!_pDataV_O) delete _pDataV_O;
}

void SLData::ReadStory(QXmlStreamReader& reader, int nTimeRange)
{
	// 开始时间点
	auto t0 = std::chrono::high_resolution_clock::now();

	// 0. read the data
	_dataL0.ReadStory(reader, nTimeRange);


	auto t1 = std::chrono::high_resolution_clock::now();

	// 1. set the valid actor list
	_listValidActors = _dataL0.GetActors();

	auto t2 = std::chrono::high_resolution_clock::now();

	/*
	for (string strA : _listValidActors) {
		QString qstrA = QString::fromStdString(strA);
		qDebug() << qstrA;
	}
	*/

	/*
	* 2. set the time range for l1
	* the time range will be used in dataProcsssing
	* as the time range of L1 is not always insistent with L0
	* , so here should be set before this function calling
	*/
	_dataL1.SetTimeRange(_dataL0);


	auto t3 = std::chrono::high_resolution_clock::now();

	// 3. data process
	dataProcessing();


	auto t4 = std::chrono::high_resolution_clock::now();

	// 计算持续时间
	auto duration1 = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0);
	auto duration2 = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
	auto duration3 = std::chrono::duration_cast<std::chrono::microseconds>(t3 - t2);
	auto duration4 = std::chrono::duration_cast<std::chrono::microseconds>(t4 - t3);

	std::cout << "running time: " << duration1.count() << " ms" << std::endl;
	std::cout << "running time: " << duration2.count() << " ms" << std::endl;
	std::cout << "running time: " << duration3.count() << " ms" << std::endl;
	std::cout << "running time: " << duration4.count() << " ms" << std::endl;

}

void SLData::dataProcessing() {

	// 1.generate L1 from L0 according to the valid actors and time range
	if (!_dataL0.CreateValidData(_dataL1, _listValidActors))
	{
		SLDataCoreV newV;
		_dataV = newV;	// this line is to clear dataL2
		return;
	};
	// filter actors
	_dataL1.FilterActors();

	// 2.grouping the actors according to the setting
	switch (_nGrouping)
	{
	case 1:
		_dataL1.GroupSupportActors(_listValidActors);
		break;
	case 2:
		_dataL1.GroupActors(_listValidActors);
		break;
	default:
		break;
	}

	// 3.bind the locations and the actors
	_dataL1.BindLocationActor();

	// 4.build the session graph
	_dataL1.BuildSessionGraph();


	// 5.prepare for the visualization
	_dataL1.CreatePreVisData(_dataL2);
	if(!_pDataL2_O) _dataL1.CreatePreVisData(_dataL2_O);


	preVisualization();
}

void SLData::preVisualization() {

	_dataL2.PreVisualization(_nWeightMethod);

	_dataV = SLDataCoreV(_dataL2);

	if (!_pDataV_O) {
		_dataL2_O.PreVisualization(_nWeightMethod);
		_pDataV_O = new SLDataCoreV(_dataL2_O);
	}
}

void SLData::SetTimeWindow(int nStart, int nEnd)
{
	// 1.set time range
	_dataL1.SetTimeRange(nStart, nEnd);

	// 2.generate valid actors
	_listValidActors = _dataL0.GetValidActors(nStart, nEnd);

	/*
	if (_pDataV_O) {
		delete _pDataV_O;
		_pDataV_O = NULL;
	}
	*/

	dataProcessing();
}

void SLData::SetActorThreshold(int nThreshold) {
	if (_dataL1.SetActorThreshold(nThreshold)) {
		dataProcessing();
	}

}

void SLData::SetWeightMethod(int nMethod) {
	_nWeightMethod = nMethod;

	if (_pDataV_O)
	{
		delete _pDataV_O;
		_pDataV_O = NULL;
	}
	preVisualization();
}

void SLData::LocationMove(set<string> setL, int nSteps) {
	_dataV.LocationMove(setL, nSteps);
}

bool SLData::LocationMoveTo(string strL, int nSeq) {
	return _dataV.LocationMoveTo(strL, nSeq);
}

void SLData::UncheckAllActors() {
	_listValidActors.clear();
	dataProcessing();
}

bool SLData::SetGroupingActors(int nG) {
	if (_nGrouping != nG) {
		_nGrouping = nG;
		dataProcessing();
		return true;
	}
	else return false;
}

void SLData::CheckAllActors() {
	_listValidActors = _dataL0.GetActors();
	dataProcessing();
}

void SLData::KeepOnlyOneActor(string strA) {

	_listValidActors.clear();
	_listValidActors.push_back(strA);
	dataProcessing();
}

void SLData::HideActor(string strA, bool bHide) {
	bool bContain = find(_listValidActors.begin(), _listValidActors.end(), strA) != _listValidActors.end();
	if (bHide && bContain)
	{
		_listValidActors.erase(find(_listValidActors.begin(), _listValidActors.end(), strA));
		dataProcessing();
	}
	else if (!bHide && !bContain) {
		_listValidActors.push_back(strA);
		dataProcessing();
	}
}

void SLData::SetValidActors(std::vector<std::string> vActors) {
	_listValidActors = vActors;
	dataProcessing();
}

void SLData::SetActorWeights(string strA, double dbW) {
//	qDebug() << "void SLData::SetActorWeights(QList<QString> listActors, QList<double> listW) {";
	_dataL2.SetAWO(strA,dbW);
	preVisualization();
}

void SLData::UpdateLocationWeight(string strL, int nW) {
	_dataL2.SetLWO(strL, nW);
	preVisualization();
}

void SLData::UpdateSessionWeight(vector<string> listS, int nW) {
//	qDebug() << "void SLData::UpdateSessionWeight(QSet<QString> setL, int nW) {" << nW;
	for (string strId : listS) {
		_dataL2.SetSWO(strId,nW);
	}
	preVisualization();
}

void SLData::GetActorSequence(int nT, string strA, int& nSeq, int& nGroup, int& nGroupSeq) const {
	_dataV.GetActorSequence(nT, strA, nSeq, nGroup, nGroupSeq);
}

int SLData::GetActorSeqInSession(string strA, int nT, string strL) {
	return _dataV.GetActorSeqInSession(strA, nT, strL);
}

double SLData::GetSessionW1(string strS) {
	return _dataV.GetSessionW1(strS);
}

double SLData::GetSessionWO(string strS) {
	return _dataV.GetSessionWO(strS);
}

string SLData::GetSessionDsp(string strId) {
	return _pDataV_O->GetSessionDsp(strId);
}

vector<string> SLData::GetSessionSecs(string strS) const {
	return _pDataV_O->GetSessionSecs(strS);
}

string SLData::GetSessionLocation(string strS) const {
	return _pDataV_O->GetSessionLocation(strS);
}

int SLData::GetSessionStart(string strS) const {
	return _pDataV_O->GetSessionStart(strS);

}

string SLData::GetActorTips(string strId) const
{
	return _pDataV_O->GetActorTips(strId);
}

string SLData::GetLocationTips(string strId) const
{
	return _pDataV_O->GetLocationTips(strId);
}

string SLData::GetSessionTips(string strId) const
{
	return _pDataV_O->GetSessionTips(strId);
}

int SLData::GetFirstTime() { return _dataL0.GetStart(); }

int SLData::GetLastTime() { return _dataL0.GetEnd(); }

double SLData::GetAW(string strA) {
	return _dataV.GetAW(strA);
}

double SLData::GetLW(string strL) {
	return _dataV.GetLW(strL);
}

map<string, double> SLData::GetAWMap() {
	return _dataV.GetAWMap();
}

map<string, double> SLData::GetAWOMap() {
	return _dataV.GetAWOMap();
}

map<string, double> SLData::GetLWMap()
{
	return _dataV.GetLWMap();
}

map<string, double> SLData::GetLWOMap()
{
	return _dataV.GetLWOMap();
}

int SLData::GetSessionActorNumber(string strS) {
	return _dataV.GetSessionActorNumber(strS);
}

int SLData::GetLocationDepth() {
	return _dataL2.GetLocationDepth();
}

void SLData::SetLocationDepth(int nD) {
	_dataV.SetLocationDepth(nD);
}

void SLData::GetActorSpanInfo(string strA, vector<bool>& vDeath, vector<int>& vT, vector<string>& vL) 
{
	_dataV.GetActorSpanInfo(strA, vDeath, vT, vL); 
};

string SLData::GetActorDisplayName(string strA)
{
	return _dataV.GetActorDisplayName(strA);
}
const int* SLData::GetActorColor(string strA) { return _dataV.GetActorColor(strA); };

void SLData::GetActorPC(vector<double>& vActorPC0, vector<double>& vActorPC1)
{ _dataV.GetActorPC(vActorPC0, vActorPC1); };


vector<string> SLData::GetActorPC(std::vector<double>& vActorPC0, std::vector<double>& vActorPC1, bool bO) {
	SLDataCoreV* pV = bO ? _pDataV_O : &_dataV;
	vector<string> vActors = pV->GetActors();
	pV->GetActorPC(vActorPC0, vActorPC1);
	return vActors;
}


int SLData::GetActorNum() {
	return _dataV.GetALen(); }

vector<string> SLData::GetActors() {
	return _dataV.GetActors(); }


void SLData::GetTransitionGraph(Eigen::MatrixXi& mStates, bool bO)
{
	SLDataCoreV* pV = bO ? _pDataV_O : &_dataV;
	pV->GetTransitionGraph(mStates);
};

vector<LocationMerge> SLData::GetMergeRecords() { return _dataV.GetMergeRecords(); }
double SLData::GetMaxAW() { return  _dataV.GetMaxAW(); }
double SLData::GetMaxLW() { return  _dataV.GetMaxLW(); }
double SLData::GetMaxAW1() { return _dataV.GetMaxAW1(); }
double SLData::GetMaxLW1() { return _dataV.GetMaxLW1(); }
double SLData::GetMaxAWO() { return _dataV.GetMaxAWO(); }
double SLData::GetMaxLWO() { return _dataV.GetMaxLWO(); }
double SLData::GetMaxSW1() { return _dataV.GetMaxSW1(); }


double SLData::GetLWO(string strL) { return _dataV.GetLWO(strL); }

double SLData::GetLW1(string strL) { return _dataV.GetLW1(strL); }


string SLData::GetDisplayLocation(string strL) { return _dataV.GetDisplayLocation(strL); }


vector<string> SLData::GetSessions() { return _dataV.GetSessions(); }



// get the time step length
int SLData::GetTimeLen() { return _dataV.GetTimeLen(); }

// get the timestep projected sequence
int SLData::GetTimeProjection(int nTime) { return _dataV.GetTimeProjection(nTime); }

// get location id list ordered by pc0
vector<string> SLData::GetDisplayedLocations() {
	return _dataV.GetLocations();
}


// get location id list ordered by pc0
vector<string> SLData::GetDisplayedLocationsBySeq() {
	return _dataV.GetLocationsBySeq();
}

// get location id list ordered by pc1
vector<string> SLData::GetLocationsPC1() { return _dataV.GetLocationsPC1(); }

int SLData::GetStart() { return _dataV.GetStart(); }

int SLData::GetEnd() { return _dataV.GetEnd(); }

int SLData::GetLocationNum() { return _dataV.GetLLen(); }



void SLData::GenerateLocationCoordinates(vector<double>& vCY
	, vector<double>& vCX
	, vector<double>& vSpanY
	, vector<double>& vSpanX
	, vector<string>& vPC0
	, vector<string>& vPC1
	, bool bO
) {
	SLDataCoreV* pV = bO ? _pDataV_O : &_dataV;

	pV->GenerateLocationCoordinates(vCY, vCX, vSpanY, vSpanX);
	vPC0 = pV->GetLocations();
	vPC1 = pV->GetLocationsPC1();
}


int SLData::GetLocationMax(std::string strL) {
	return _dataV.GetLocationMax(strL);
}

string SLData::GetLocationDisplayName(string strL) {
	return _dataV.GetLocationDisplayName(strL);
}


int SLData::GetLocationSeq(std::string strL) {
	return _dataV.GetLocationSeq(strL);
}

vector<string> SLData::GetLocationActors(vector<string> vL) {
	return _dataV.GetLocationActors(vL);

}

vector<string> SLData::GetSessionActors(vector<string> vS) {
	return _dataV.GetSessionActors(vS);

}

vector<string> SLData::GetActorLocations(vector<std::string> vA) {
	return _dataV.GetActorLocations(vA);
}

vector<string> SLData::GetActorSessions(vector<std::string> vA) {
	return _dataV.GetActorSessions(vA);
}
