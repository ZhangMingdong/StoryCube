#pragma once


#include <string>
#include <list>
#include <map>

#include "SLW.h"
#include "SLDefinition.h"


/*
	Structure for a session
*/
class SLSession : public SLW
{
public:
	SLSession(){}
	SLSession(int nS, std::string strL, std::string strDsp);
	SLSession(std::string strL, const SLSession& s1, const SLSession& s2);
private:
	/*
	* time of this session
	*/
	int _nT=-1;

	/*
	* location of this session
	*/
	std::string _strL="";

	/*
	* scene description
	*/
	std::string _strDsp="";

	/*
	* actors in this session
	*/
	std::vector<std::string> _listA;

	/*
	* list of success sessions
	*/
	std::vector<std::string> _listSecs;

	/*
	* list of previous sessions
	*/
	std::vector<std::string> _listPres;

public:
	int GetT() const { return _nT; }
	std::string GetL() const { return _strL; }
	std::string GetDsp() const { return _strDsp; }
	void AddActor(std::string strA) { _listA.push_back(strA); }
	std::vector<std::string> GetActors() const { return _listA; }
	bool AddSec(std::string strSec);
	bool AddPre(std::string strPre);
	std::vector<std::string> GetSecs() const { return _listSecs; }
	std::vector<std::string> GetPres() const { return _listPres; }


	/*
	* sort the actors in this session globally
	*/
	void SortActors(SLDataCoreInterface* pInterface);


	/*
	* sort index according to ref index
	* given the interface to compare and try to swap two actors
	*/
	void SortActors(int nTimeIndex, int nRefTimeIndex, const SLDataCoreInterface* pInterface);

	/*
	* get the sequence of an actor in a time
	*/
	int GetActorSequence(std::string strA) const;
};