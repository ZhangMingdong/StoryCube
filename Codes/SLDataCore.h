#pragma once


#include <Eigen/Dense>
#include <mathtoolbox/classical-mds.hpp>
#include <map>
#include <string>
#include <vector>


#include "SLDefinition.h"



class QXmlStreamReader;

class SLActor;
class SLLocation;
class SLSession;




/*
* this class used to store the core information of storyline
*/
class SLDataCore: public SLDataCoreInterface {
public:
	~SLDataCore();
protected:
	/*
	* L0: time range to read
	*/
	int _nTimeRange = 0;
	/*
	* the time range
	* L0: time range to read
	* L1: selected time range
	*/
	int _nStart = 10000;					// fist start time of the data
	int _nEnd = 0;							// last start time of the data	

	/*
	* actor threshold
	* with less appear numbers than the threshold will be filtered
	*/
	int _nActorThreshold = 1;

	/*
	* map and list
	* L0:
	* L1:
	*/
	std::map<std::string, SLActor*> _mapA;					// map of actor	
	std::map<std::string, SLLocation*> _mapL;				// map of locations	
	std::map<std::string, SLSession*> _mapS;					// map of sessions
	std::vector<std::string> _listA;							// list of actor name	
	std::vector<std::string> _listL;							// list of location ids	

	/*
	* list of sessions
	* used in:
	* -w
	* --loading data
	* --clear
	* 
	* -r
	* --L2: generate session weights
	* --sort actors globally
	* --sort actors accordingly
	* 
	* -wr
	* --create valid data
	* --create previs data
	*/
	std::vector<std::string> _listS;							

	/*
	* distance matrix
	*/
	Eigen::MatrixXd _D_Location;				// distance matrix between locations
	Eigen::MatrixXd _D_Actor;					// D(i,j) means distance of merging j to i;


	/*
	* max weights
	*/
	double _dbMaxAW = 0;
	double _dbMaxLW = 0;
	double _dbMaxAWO = 1;
	double _dbMaxLWO = 1;
	double _dbMaxAW1 = 0;
	double _dbMaxLW1 = 0;
	double _dbMaxSW1 = 0;
	std::vector<double> _listTimestepW;				// weight of each time step
	std::vector<double> _listTimeProjection;			// project time to the valid sequence


	std::vector<std::string> _listLocationIdsPC1;					// list of location ids, ordered by PC1

	/*
	* for keeping stability
	*/
	std::vector<std::string> _listIdsRef;
	std::vector<double> _listPC0Ref;
	std::vector<double> _listPC1Ref;
	bool _bInit = false;

	std::string _strFirstLocation;					// first location to display

public:	// Operation
	/*
	* create valid data from raw data, 
	* when setting changed, the actor list or the time range
	* return false if actorValid is empty, then the data is empty
	* L0->L1
	*/
	bool CreateValidData(SLDataCore& dataValid, std::vector<std::string> actorValid) const;

	/*
	* create display data from valid data
	* L1->L2
	*/
	void CreatePreVisData(SLDataCore& dataDisplay) const;

	/*
	* L2->V
	*/
	void CreateVisData(SLDataCore& dataDisplay) const;

	// Set time range of the data
	void SetTimeRange(const SLDataCore& data);

	void SetTimeRange(int nStart, int nEnd);

	bool SetActorThreshold(int nThreshold);

	// get list of valid actor according to the time range
	std::vector<std::string> GetValidActors(int nStart, int nEnd);

public:	// Getter
	std::vector<std::string> GetActors();

	std::vector<std::string> GetLocations();

	/*
	* get locations by sequence
	*/
	std::vector<std::string> GetLocationsBySeq();

	std::vector<std::string> GetSessions();

	int GetStart() const { return _nStart; }

	int GetEnd() const { return _nEnd; }

	double GetMaxAW() { return _dbMaxAW; }
	double GetMaxLW() { return _dbMaxLW; }
	double GetMaxAW1() { return _dbMaxAW1; }
	double GetMaxLW1() { return _dbMaxLW1; }
	double GetMaxAWO() { return _dbMaxAWO; }
	double GetMaxLWO() { return _dbMaxLWO; }
	double GetMaxSW1() { return _dbMaxSW1; }

public:	// wrapper

	/*
	* get tips of actor, used when select an actor
	*/
	std::string GetActorTips(std::string strId) const;

	/*
	* get tips of location, used when select a location
	*/
	std::string GetLocationTips(std::string strId) const;
	/*
	* get tips of session, used when select a session
	*/
	std::string GetSessionTips(std::string strId) const;

	/*
	* get sequence nSeq, group size nGroup, and sequence in group nGroupSeq of actor strA in time nT
	* a group means some actor moving together from the previous location to this location
	*/
	void GetActorSequence(int nT, std::string strA, int& nSeq, int& nGroup, int& nGroupSeq) const;

	/*
	* get the actor sequence in a session
	*/
	int GetActorSeqInSession(std::string strA, int nT, std::string strL);


	int GetSessionStart(std::string strS) const;

	std::string GetSessionLocation(std::string strS) const;

	/*
	* get display location in current hierarchy
	*/
	std::string GetDisplayLocation(std::string strL) const;

	double GetAWO(std::string strA);
	double GetLWO(std::string strL);
	double GetAW1(std::string strA);
	double GetLW1(std::string strL);
	double GetAW(std::string strA);
	double GetLW(std::string strL);
	std::map<std::string, double> GetAWMap();
	std::map<std::string, double> GetAWOMap();
	std::map<std::string, double> GetLWMap();
	std::map<std::string, double> GetLWOMap();

	void SetAWO(std::string strA, double dbW);
	void SetLWO(std::string strL, double dbW);

	double GetSWO(std::string strS);
	double GetSW1(std::string strS);
	void SetSWO(std::string strS, double dbW);


	// get location id list ordered by pc1
	std::vector<std::string> GetLocationsPC1() { return _listLocationIdsPC1; }

	double GetSessionW1(std::string strS);

	double GetSessionWO(std::string strS);

	std::vector<std::string> GetSessionSecs(std::string strS) const;

	std::string GetSessionDsp(std::string strId);

	// get the time step length
	int GetTimeLen();

	// get the timestep projected sequence
	int GetTimeProjection(int nTime) { return _listTimeProjection[nTime - _nStart]; }

	int GetALen() { return _listA.size(); }
	int GetLLen() { return _listL.size(); }

	// get the actor number of a session
	int GetSessionActorNumber(std::string strS);

	int GetLocationDepth();

protected:

	/*
	* get the length of the time range
	*/
	int getTimeRange() const { return _nEnd - _nStart + 1; }

	/*
	* clear list and map
	*/
	void clear();

	/*
	*	sort actors globally
	*/
	void sortActorsGlobally();

	/*
		sort the actors in each location
	*/
	void sortActorsInLocations();

	/*
		sort actor at a given time according to a reference time
	*/
	void sortActor(int nTimeIndex, int nRefTimeIndex);

	/*
	* sort the actors
	* called when
	* - L2: previsualization and
	* - V: location move
	*/
	void SortActors();

	/*
	* calculate the width of a location
	*/
	void calculateLocationWidth(std::string strL);

protected:	// used for L2 and V
	std::vector<LocationMerge> _listLocationMerge;	// merge record of the hierarchical clustering
	std::vector<std::string> _listLClusters;				// clusters of the location
	int _nMaxDepth = 1;							// max depth
public:

	/*
	* try to swap actors at j and k in listA at time index nTimeIndex according to time index nRefTimeIndex
	*/
	virtual void tryToSwap(std::vector<std::string>& listA, int nTimeIndex, int nRefTimeIndex, int j, int k) const;
	/*
	* merge two session to a new one
	* put into the list
	* return the id
	*/
	virtual std::string mergeSession(std::string strL, std::string strS1, std::string strS2);


	/*
	* sort actors in vA
	*/
	virtual void sortActors(std::vector<std::string>& vA);
};