#pragma once

#include <vector>
#include <string>
#include <set>

#include "SLW.h"


/*
	struct for a span
*/
struct SLSpan
{
	// location of this span
	std::string _strLocationId;

	// start time of this span, duration is set to 1
	int _nT;

	// if the actor is died in this span
	bool _bDeath = false;


	// check if this span is in the valid time range
	bool CheckValidity(int nStart, int nEnd) const {
		return _nT >= nStart && _nT <= nEnd;
	}
};
/*
	struct for an actor
*/
class SLActor:public SLW {
public:
	SLActor() {};
	SLActor(std::string strName);

	/*
	* copy from another SLActor
	*/
	SLActor(const SLActor* pA);

protected:
	std::string _strName = "";				// actor name	
	int _arrColor[3] = { -1,-1,-1 };	// actor color
	std::vector<SLSpan> _listSpan;			// time span list
	std::vector<std::string> _listLocation;		// location in each time	
	std::vector<std::string> _listCompanies;		// companies of this actor	
	double _dbSortingWeight = 0;		// weight for sorting
	double _arrPC[2];					// pcs of the actor
public:	// getter and setter
	/*
	* get actor name
	*/
	std::string GetName() const { return _strName; }

	/*
	* set the actor color
	*/
	void SetColor(int nR, int nG, int nB) { _arrColor[0] = nR;  _arrColor[1] = nG; 	_arrColor[2] = nB; }

	/*
	* get the actor color
	*/
	const int* GetColor() { return _arrColor; }

	const std::vector<SLSpan>& GetSpans() const { return _listSpan; }
	/*
	* get actor location list
	* used in swaping actors
	*/
	const std::vector<std::string>& GetLocations() const { return _listLocation; }
	const SLSpan& GetLastSpan() const;
	double GetSortingWeight()const { return _dbSortingWeight; }
	void SetDisplayWeight(double dbW) { _dbSortingWeight = dbW; }
	void SetPC(int i, double dbValue) { _arrPC[i] = dbValue; };
	double GetPC(int i) const { return _arrPC[i]; }
	const std::vector<std::string>& GetCompanies() const { return _listCompanies; }

public: // operations

	/*
	* get location of nIndex
	* used in GetActorSequence()
	*/
	std::string GetL(int nIndex) const;

	/*
	* add a new span
	*/
	void AddSpan(std::string strL, int nT, bool bDeath);

	/*
	* set time range for the actor, remove outside spans
	*/
	void SetTimeRange(int nStart, int nEnd);

	/*
	* collect valid locations and sessions according to the spans
	* used in creation of valid data
	* get the valid locations and sessions according to the valid actors
	*/
	void CollectValidLocationAndSession(std::set<std::string>& setLocation, std::set<std::string>& setSession);

	/*
	* filter spans according to the valid sessions
	* used in creation of valid data
	* to filter the invalid actors, to only keep the valid sessions (with valid actor in it)
	*/
	void Filter(const std::set<std::string>& setSession);

	/*
	* if the actor has spans
	*/
	bool IsValid() const { return _listSpan.size() > 0; }

	/*
	* used in BindLocationActor
	* add a location name for sessions or "" for non-session timesteps
	*/
	void AppendLocation(std::string strL) { _listLocation.push_back(strL); }
	/*
	* append a compay
	*/
	void AddCompany(std::string strC) { _listCompanies.push_back(strC); }
	/*
	* clear the companies
	*/
	void ClearCompany() { _listCompanies.clear(); }
	/*
	* get display name
	*/
	std::string DisplayName() const;
	/*
	* clear the location information
	*/
	void ClearLocationInfo();

};
