#pragma once

#include <QList>
#include <QString>
#include <QMap>

#include "SLW.h"

class SLDataCoreInterface;

/*
	structure for a location in the storyline
*/
class SLLocation: public SLW {
private:
// === raw data ===	
	std::string _strName="";					// name of the location	
	std::string _strDisplayName="";				// display name

// === generated data ===	
	int _nMax = 0;							// max number of actors in the duration
	int _nSequence = 0;						// display sequence of this location
	double _arrPC[2];						// pcs of the location
	int _nD2R = 0;							// distance to root

	std::map<int, std::string> _mapT2S;				// map of contained sessions, from time to session name
	std::string _strCluster = "";				// the cluster location this location belong to

public:
	SLLocation() {}
	SLLocation(std::string strName);
	SLLocation(std::string strName, const SLLocation* pL1, const SLLocation* pL2,SLDataCoreInterface* pInterface);
public:	// getter and setter
	std::string GetDisplayName() const { return _strDisplayName; }
	std::string GetName() const { return _strName; }
	int GetMax() const { return _nMax; }
	int GetSequence() const { return _nSequence; }

	void SetDisplayName(std::string strDN) { _strDisplayName = strDN; }
	void SetSequence(int nSeq) { 
		_nSequence = nSeq; 
	}
	void SetMax(int nMax) { _nMax = nMax; }
	void SetPC(int i, double dbValue) { _arrPC[i] = dbValue; };
	double GetPC(int i) const { return _arrPC[i]; }
	int GetD2R() const { return _nD2R; }
	void SetD2R(int nD) { _nD2R = nD; }

	// get sessions, put into the list
	void GetSessions(std::vector<std::string>& listS) const;

	// get session list
	std::vector<std::string> GetSessions() const;

	std::string GetCluster() const { return _strCluster; }
	void SetCluster(std::string strC) { _strCluster = strC; }

public:
	/*
		add a session at given time
	*/
	void AddSession(int nT, std::string strS);

	/*
	* clear session info
	*/
	void ClearSessions();

	/*
	* extend the actor size
	*/
	void ExtendActorSize(int nSize);

	/*
	* get a session at a given time
	*/
	std::string GetSession(int nT) const;



};




















