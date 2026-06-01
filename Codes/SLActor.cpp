
#include "SLActor.h"
#include "SLDefinition.h"


using namespace std;

void SLActor::AddSpan(string strL, int nT, bool bDeath) {
	SLSpan span;
	span._strLocationId = strL;
	span._nT = nT;
	span._bDeath = bDeath;
	_listSpan.push_back(span);
}

void SLActor::SetTimeRange(int nStart, int nEnd) {
	for (int i = _listSpan.size() - 1; i >= 0; i--)
	{
		if (_listSpan[i]._nT<nStart || _listSpan[i]._nT>nEnd) {
			_listSpan.erase(_listSpan.begin() + i);
		}
	}
}

void  SLActor::CollectValidLocationAndSession(set<string>& setLocation, set<string>& setSession) {
	for (const SLSpan& span : _listSpan)
	{
		int nT = span._nT;
		string strL = span._strLocationId;
		setLocation.insert(strL);
		setSession.insert(GenerateSessionID(nT, strL));
	}
}

void SLActor::Filter(const set<std::string>& setSession) {
	for (int i = _listSpan.size() - 1; i >= 0; i--)
	{
		const SLSpan& span = _listSpan[i];
		int nT = span._nT;
		string strL = span._strLocationId;
		string strS = GenerateSessionID(nT, strL);
		if (!setSession.contains(strS))
		{
			_listSpan.erase(_listSpan.begin()+i);
		}
	}

}

SLActor::SLActor(const SLActor* pA) {
	_strName = pA->_strName;
	_arrColor[0] = pA->_arrColor[0];
	_arrColor[1] = pA->_arrColor[1];
	_arrColor[2] = pA->_arrColor[2];
	for (SLSpan span : pA->_listSpan)
	{
		_listSpan.push_back(span);
	}
}

SLActor::SLActor(std::string strName):_strName(strName)
{

}

const SLSpan& SLActor::GetLastSpan() const {
	return _listSpan.back();
}

string SLActor::DisplayName() const {
	bool bShowCompany = false;

	if (bShowCompany) {
		string strResult = _strName;
		for (string c : _listCompanies)
		{
			strResult = strResult + "|" + c;
		}
		return strResult;
	}
	else {
		string strResult = _strName;
		if (!_listCompanies.empty())
		{
			strResult += "("; 
			strResult += _listCompanies.size() + ")";
		}
		return strResult;
	}
}

void SLActor::ClearLocationInfo() {
	_listLocation.clear();
}

string SLActor::GetL(int nIndex) const {
	return _listLocation[nIndex];
}