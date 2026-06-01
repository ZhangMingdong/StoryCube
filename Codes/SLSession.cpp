#include "SLSession.h"
#include "SLActor.h"
#include "SLLocation.h"


using namespace std;
extern int getStrIndex(const vector<string>& list, const string& str) noexcept;

SLSession::SLSession(int nS, string strL, string strDsp)
	:_nT(nS), _strL(strL), _strDsp(strDsp)
{

}


SLSession::SLSession(std::string strL, const SLSession& s1, const SLSession& s2) {
	assert(s1._nT == s2._nT);
	_nT = s1._nT;
	_strL = strL;
	_strDsp = s1._strDsp + ";" + s2._strDsp;
	set<string> combinedSet(s1._listA.begin(), s1._listA.end());
	combinedSet.insert(s2._listA.begin(), s2._listA.end());
	_listA = vector(combinedSet.begin(), combinedSet.end());
}

bool SLSession::AddSec(string strSec) {
	if (find(_listSecs.begin(), _listSecs.end(),strSec)== _listSecs.end())
	{
		_listSecs.push_back(strSec);
		return true;
	}
	else
		return false;
}

bool SLSession::AddPre(string strPre) {
	if (find(_listPres.begin(), _listPres.end(),strPre)== _listPres.end()) {
		_listPres.push_back(strPre);
		return true;
	}
	else
		return false;
}


void SLSession::SortActors(SLDataCoreInterface* pInterface) {
	pInterface->sortActors(_listA);
}

void SLSession::SortActors(int nTimeIndex, int nRefTimeIndex, const SLDataCoreInterface* pInterface) {
	const int nLen = _listA.size();
	// bubble set
	for (int i = 0; i < nLen - 1; i++)
	{
		for (int j = i + 1; j < nLen; j++)
		{
			pInterface->tryToSwap(_listA, nTimeIndex, nRefTimeIndex, i, j);
		}
	}
}

int SLSession::GetActorSequence(string strA) const {
	return getStrIndex(_listA,strA);
}


string GenerateSessionID(int nT, string strL) {
	return to_string(nT) + "-" + strL;
}