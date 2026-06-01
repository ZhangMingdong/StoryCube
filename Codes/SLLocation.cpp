#include "SLLocation.h"
#include "SLActor.h"
#include "SLSession.h"


using namespace std;

SLLocation::SLLocation(std::string strName) {
	_strName = strName;
	_strDisplayName = strName;
}


SLLocation::SLLocation(string strName, const SLLocation* pL1, const SLLocation* pL2, SLDataCoreInterface* pInterface) {
	_strName = strName;
	_strDisplayName = pL1->_strDisplayName + "-" + pL2->_strDisplayName;
	_nSequence = std::min(pL1->_nSequence, pL2->_nSequence);
	_nD2R = std::min(pL1->_nD2R, pL2->_nD2R) - 1;
	_dbW1 = (pL1->_dbW1 + pL2->_dbW1)/2.0;

	// 获取两个map的所有键
	set<int> allKeys;
	for (const auto& pair : pL1->_mapT2S) { allKeys.insert(pair.first); }
	for (const auto& pair : pL2->_mapT2S) { allKeys.insert(pair.first); }

	for (int key : allKeys) {
		auto it1 = pL1->_mapT2S.find(key);
		auto it2 = pL2->_mapT2S.find(key);

		if (it1 != pL1->_mapT2S.end() && it2 != pL2->_mapT2S.end()) {
			_mapT2S[key] = pInterface->mergeSession(strName, it1->second, it2->second);
		}
		else if (it1 != pL1->_mapT2S.end()) {
			_mapT2S[key] = it1->second;
		}
		else {
			_mapT2S[key] = it2->second;
		}
	}
}

void SLLocation::AddSession(int nT, string strS) {
	_mapT2S[nT] = strS;
}

string SLLocation::GetSession(int nT) const {
	return _mapT2S.at(nT);
}

void SLLocation::ClearSessions() {
	_mapT2S.clear();
}

void SLLocation::ExtendActorSize(int nSize) {
	if (nSize>_nMax)
	{
		_nMax = nSize;
	}
}

void SLLocation::GetSessions(vector<string>& listS) const {
	for (const auto& pair :_mapT2S)
	{
		if (find(listS.begin(), listS.end(),pair.second)==listS.end())
			listS.push_back(pair.second);
	}
}

vector<string> SLLocation::GetSessions() const {
	vector<string> listS;
	for (const auto& pair:_mapT2S)
	{
		listS.push_back(pair.second);
	}
	return listS;
}