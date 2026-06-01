#include "SLDataCoreV.h"



#include "SLSession.h"
#include "SLLocation.h"
#include "SLActor.h"

using namespace std;

extern int getStrIndex(const vector<string>& list, const string& str) noexcept;

SLDataCoreV::SLDataCoreV(const SLDataCore& data) {
	data.CreateVisData(*this);
}

void SLDataCoreV::LocationMove(set<string> setL, int nSteps) {
	const vector<string>& listLocations = _listL;
	// 1.find the set to move, and change their sequence
	vector<int> listMovedSeq;	// sequence to move
	set<string> setMoved;
	for (string strL : setL)
	{
		int nSeq = _mapL[strL]->GetSequence();
		int nTargetSeq = nSeq + nSteps;
		if (nTargetSeq >= 0 && nTargetSeq < listLocations.size()) {
			_mapL[strL]->SetSequence(nTargetSeq);
			setMoved.insert(strL);
			listMovedSeq.push_back(nSeq);
		}
	}

	std::sort(listMovedSeq.begin(), listMovedSeq.end());
	if (nSteps > 0)	std::reverse(listMovedSeq.begin(), listMovedSeq.end());

	// 2.find the set to elude, and change their sequence
	for (string strL : listLocations)
	{
		if (!setMoved.contains(strL))
		{
			if (nSteps > 0) {
				int nSeq = _mapL[strL]->GetSequence();
				for (int iSeq : listMovedSeq) {
					if (iSeq < nSeq && iSeq + nSteps >= nSeq) {
						_mapL[strL]->SetSequence(--nSeq);
					}
				}
			}
			else {
				int nSeq = _mapL[strL]->GetSequence();
				for (int iSeq : listMovedSeq) {
					if (iSeq > nSeq && iSeq + nSteps <= nSeq) {
						_mapL[strL]->SetSequence(++nSeq);
					}
				}
			}
		}
	}

	// 3.sort the actors
	SortActors();
}

bool SLDataCoreV::LocationMoveTo(string strL, int nSeq) {
	const vector<string>& listLocations = _listL;
	int nOldSeq = _mapL[strL]->GetSequence();
	// strL used in sequence nOldSeq, if new seq if beside this one, it means that no change
	if (nSeq < nOldSeq)
	{
		_mapL[strL]->SetSequence(nSeq);
		for (string strID : listLocations) {
			int nCurrentSeq = _mapL[strID]->GetSequence();
			if (strID != strL && nCurrentSeq < nOldSeq && nCurrentSeq >= nSeq)
			{
				_mapL[strID]->SetSequence(nCurrentSeq + 1);
			}
		}
		SortActors();
		return true;
	}
	else if (nSeq > nOldSeq + 1) {
		nSeq--;	// minus one to remove the old self
		_mapL[strL]->SetSequence(nSeq);
		for (string strID : listLocations) {
			int nCurrentSeq = _mapL[strID]->GetSequence();
			if (strID != strL && nCurrentSeq > nOldSeq && nCurrentSeq <= nSeq)
			{
				_mapL[strID]->SetSequence(nCurrentSeq - 1);
			}
		}
		SortActors();
		return true;
	}
	else {
		return false;
	}

}

void SLDataCoreV::SetLocationDepth(int newDepth) {
	// Early return if depth hasn't changed
	if (_nMaxDepth == newDepth) {
		return;
	}

	if (newDepth < _nMaxDepth) {
		// Hierarchical up - remove low level nodes
		removeLowLevelNodes(newDepth);
	}
	else {
		// Hierarchical down - show low leaves and hide higher clusters
		showLowLeaves(newDepth);
		hideHigherClusters(newDepth);
	}

	// Add appropriate clusters for the new depth
	addClustersForNewDepth(newDepth);
	// Debug output
	//qDebug() << _listL;
	//qDebug() << _listLHidden;
	//qDebug() << _listLClusters;

	// Update the depth
	_nMaxDepth = newDepth;
}

void SLDataCoreV::removeLowLevelNodes(int depthThreshold) {
	for (int i = _listL.size() - 1; i >= 0; i--) {
		const string& item = _listL[i];
		if (_mapL[item]->GetD2R() > depthThreshold) {
			if (find(_listLClusters.begin(), _listLClusters.end(), item) == _listLClusters.end()) {
				_listLHidden.push_back(item);
			}
			_listL.erase(_listL.begin()+i);
		}
	}
}

void SLDataCoreV::showLowLeaves(int depthThreshold) {
	for (int i = _listLHidden.size() - 1; i >= 0; i--) {
		const string& item = _listLHidden[i];
		if (_mapL[item]->GetD2R() <= depthThreshold) {
			_listL.push_back(item);
			_listLHidden.erase(_listLHidden.begin()+i);
		}
	}
}

void SLDataCoreV::hideHigherClusters(int depthThreshold) {
	for (int i = _listL.size() - 1; i >= 0; i--) {
		const string& item = _listL[i];
		if (_mapL[item]->GetD2R() <= depthThreshold && find(_listLClusters.begin(), _listLClusters.end(), item) != _listLClusters.end()) {
			_listL.erase(_listL.begin()+i);
		}
	}
}

void SLDataCoreV::addClustersForNewDepth(int targetDepth) {
	for (const string& cluster : _listLClusters) {
		if (_mapL[cluster]->GetD2R() == targetDepth) {
			_listL.push_back(cluster);
		}
	}
}

void SLDataCoreV::GetActorPC(std::vector<double>& vActorPC0, std::vector<double>& vActorPC1) {
	vActorPC0.clear();
	vActorPC1.clear();
	for (string strA : _listA)
	{
		const SLActor* actor = _mapA[strA];
		vActorPC0.push_back(actor->GetPC(0));
		vActorPC1.push_back(actor->GetPC(1));
	}
}

void SLDataCoreV::GetTransitionGraph(Eigen::MatrixXi& mStates) {

	mStates.setZero();
	for (string strA : _listA) {
		SLActor* pA = _mapA[strA];
		for (int i = 1; i < pA->GetSpans().size(); i++)
		{
			const SLSpan& span0 = pA->GetSpans()[i - 1];
			const SLSpan& span1 = pA->GetSpans()[i];
			//if (span1._nStart==span0._nStart+1)
			{
				string strL0 = span0._strLocationId;
				string strL1 = span1._strLocationId;
				if (strL0 == strL1) continue;
				int nIndex0 = getStrIndex(_listL, strL0);
				int nIndex1 = getStrIndex(_listLocationIdsPC1, strL1);
				mStates(nIndex0, nIndex1)++;
			}
		}
	}
}


/*
* get the actor color
*/
const int* SLDataCoreV::GetActorColor(std::string strA) {
	return _mapA[strA]->GetColor();
}

/*
* get time and location of spans of this actor
*/
void SLDataCoreV::GetActorSpanInfo(std::string strA, std::vector<bool>& vDeath, std::vector<int>& vT, std::vector<std::string>& vL) {
	for (const SLSpan& span : _mapA[strA]->GetSpans())
	{
		vDeath.push_back(span._bDeath);
		vT.push_back(span._nT);
		vL.push_back(span._strLocationId);
	}
}

string SLDataCoreV::GetActorDisplayName(std::string strA) {
	return _mapA[strA]->DisplayName();
}

void SLDataCoreV::GenerateLocationCoordinates(std::vector<double>& vCY, std::vector<double>& vCX, std::vector<double>& vSpanY, std::vector<double>& vSpanX) {
	// get the two list of X direction and Y direction
	vector<string> listLY = _listL;
	vector<string> listLX = _listLocationIdsPC1;
	int nLen = listLY.size();

	// list of coordinates and spans of the two direction
	double dbMinSpanX = 1000000;
	double dbMaxSpanX = 0;
	double dbMinSpanY = 1000000;
	double dbMaxSpanY = 0;

	// generate the 4 list and the border value
	for (string strL : listLY)
	{
		double dbY = _mapL[strL]->GetPC(0);
		//qDebug() << dbY;
		if (!vCY.empty())
		{
			double dbSpan = abs(dbY - vCY.back());
			vSpanY.push_back(dbSpan);
			if (dbSpan < dbMinSpanY) dbMinSpanY = dbSpan;
			if (dbSpan > dbMaxSpanY) dbMaxSpanY = dbSpan;
		}
		vCY.push_back(dbY);
	}
	for (string strL : listLX)
	{
		double dbX = _mapL[strL]->GetPC(1);
		if (!vCX.empty())
		{
			double dbSpan = abs(dbX - vCX.back());
			vSpanX.push_back(dbSpan);
			if (dbSpan < dbMinSpanX) dbMinSpanX = dbSpan;
			if (dbSpan > dbMaxSpanX) dbMaxSpanX = dbSpan;
		}
		vCX.push_back(dbX);
	}
}

int SLDataCoreV::GetLocationMax(std::string strL) {
	return _mapL[strL]->GetMax();
}

string SLDataCoreV::GetLocationDisplayName(string strL) {
	return _mapL[strL]->GetDisplayName();
}

int SLDataCoreV::GetLocationSeq(std::string strL) {
	return _mapL[strL]->GetSequence();
}

vector<string> SLDataCoreV::GetLocationActors(vector<string> vL) {
	vector<string> vATotal;
	set<string> setA;
	for (string strL : vL)
	{
		if (_mapL.find(strL) == _mapL.end())
		{
			continue;
		}
		vector<string> vS = _mapL[strL]->GetSessions();
		for (string strS : vS)
		{
			vector<string> vA = _mapS[strS]->GetActors();
			for (string strA : vA)
			{
				if (setA.insert(strA).second) {
					vATotal.push_back(strA);
				}
			}
		}
	}
	return vATotal;

}

vector<string> SLDataCoreV::GetSessionActors(std::vector<std::string> vS) {
	vector<string> vATotal;
	set<string> setA;
	for (string strS : vS)
	{
		vector<string> vA = _mapS[strS]->GetActors();
		for (string strA : vA)
		{
			if (setA.insert(strA).second) {
				vATotal.push_back(strA);
			}
		}
	}
	return vATotal;

}

vector<string> SLDataCoreV::GetActorLocations(vector<std::string> vA) {

	vector<string> vLTotal;
	set<string> setL;
	for (string strA : vA)
	{
		if (_mapA.find(strA) == _mapA.end())
		{
			continue;
		}
		for (string strL : _mapA[strA]->GetLocations()) {
			if (strL.size()>0)
			{
				if (setL.insert(strL).second) {
					vLTotal.push_back(strL);
				}
			}
		}
	}
	return vLTotal;
}


vector<string> SLDataCoreV::GetActorSessions(vector<std::string> vA) {

	vector<string> vSTotal;
	set<string> setS;
	for (string strA : vA)
	{
		if (_mapA.find(strA) == _mapA.end())
		{
			continue;
		}
		for (const SLSpan& span : _mapA[strA]->GetSpans()) {
			string strS = GenerateSessionID(span._nT, span._strLocationId);
			if (setS.insert(strS).second) {
				vSTotal.push_back(strS);
			}
		}
	}
	return vSTotal;
}