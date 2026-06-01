#pragma once

#include <QList>
#include <QString>
#include <QMap>
#include <QXmlStreamReader>

#include <Eigen/Dense>
#include <mathtoolbox/classical-mds.hpp>

#include <string>
#include <vector>
#include <map>
#include <set>

#include "SLDefinition.h"


#include "SLDataCoreL0.h"
#include "SLDataCoreL1.h"
#include "SLDataCoreL2.h"
#include "SLDataCoreV.h"


/// <summary>
/// class for the data structure of storyline
/// </summary>
class SLData
{
public:
	SLData();
	~SLData();
private:
	/*
	* L0: raw data
	* 1.read data
	* 2.create valid data
	* 3.provide time range
	* 4.provide valid actor list
	*/
	SLDataCoreL0 _dataL0;
	/*
	* L1: valid data
	* 1.generate valid data from L0
	*/
	SLDataCoreL1 _dataL1;

	/*
	* L2: data after previsualization
	* generated from L1
	*/
	SLDataCoreL2 _dataL2;

	/*
	* V: final visualization data
	* generated from l2
	*/
	SLDataCoreV _dataV;

	/*
	* original L2 data
	*/
	SLDataCoreL2* _pDataL2_O = 0;

	/*
	* original LV data
	*/
	SLDataCoreV* _pDataV_O = 0;

	SLDataCoreL2 _dataL2_O;

	// list of valid actors is set by user, 
	// however, actors not in this list may also be visualized, if they are in the session of valid actors
	// this list is just used for the data processing
	std::vector<std::string> _listValidActors;		


	//  ========== setting ==========
	int _nWeightMethod = 0;						// 0: raw weight; 1: consider distance

	/*
	* if group actors
	* 0: no group
	* 1: group support actors
	* 2: group all actors
	*/
	int _nGrouping = 0;



private:

	/*
	* perform the data processing operation
	* called when data loaded (DataProcessing()), time ranged changed (SetTimeWindow()), and actors changed
	*/
	void dataProcessing();

	void preVisualization();

public:

	/*
		read a story file
	*/
	void ReadStory(QXmlStreamReader& reader, int nTimeRange=100000);


	/*
		get the first time
	*/
	int GetFirstTime();

	/*
		get the lase time
	*/
	int GetLastTime();

	/*
		set the display window
	*/
	void SetTimeWindow(int nStart, int nEnd);

	/*
	* set actor threshold
	*/
	void SetActorThreshold(int nThreshold);

	int GetStart();

	int GetEnd();

	int GetLocationNum();


	std::map<std::string, double> GetAWMap() ;
									
	std::map<std::string, double> GetAWOMap();
									
	std::map<std::string, double> GetLWMap() ;
									
	std::map<std::string, double> GetLWOMap();


	void SetWeightMethod(int nMethod);

	/*
	* move location by a set of ids and steps
	* passed locations will be eluded to avoid merging
	* steps: -n means n steps upwards, n means n steps downward
	*/
	void LocationMove(std::set<std::string> setL, int nSteps);

	/*
	* move location strL to a new sequence nSeq
	* if nSeq is same with the original one or add 1, do not move, and return false
	*/
	bool LocationMoveTo(std::string strL, int nSeq);

	// get the time step length
	int GetTimeLen();

	// get the timestep projected sequence
	int GetTimeProjection(int nTime);

	// get location id list ordered by pc0
	std::vector<std::string> GetDisplayedLocations();


	// get location id list ordered by pc0
	std::vector<std::string> GetDisplayedLocationsBySeq();

	// get location id list ordered by pc1
	std::vector<std::string> GetLocationsPC1();

	/*
	* get the weight of a session
	*/
	double GetSessionW1(std::string strS);

	double GetSessionWO(std::string strS);

	std::vector<std::string> GetSessionSecs(std::string strS) const;

	std::string GetSessionDsp(std::string strId);

	std::string GetSessionLocation(std::string strS) const;

	/*
	* get display location in current hierarchy
	*/
	std::string GetDisplayLocation(std::string strL);

	
	int GetSessionStart(std::string strS) const;

	std::vector<std::string> GetSessions();

	/*
	* get tips of location, used when select a location
	*/
	std::string GetLocationTips(std::string strId) const;
	/*
	* get tips of session, used when select a session
	*/
	std::string GetSessionTips(std::string strId) const;
	/*
	* user interactively modify the location weight
	*/
	void UpdateLocationWeight(std::string strL, int nW);

	/*
	* user interactively modify the session weight
	*/
	void UpdateSessionWeight(std::vector<std::string> listS, int nW);

	/*
	* get user set location weight
	*/
	double GetLWO(std::string strL);
	/*
	* get calculated location weight
	*/
	double GetLW1(std::string strL);
	/*
	* get final location weight
	*/
	double GetLW(std::string strL);
	/*
	* get the final actor weight
	*/
	double GetAW(std::string strA);
	double GetMaxAW() ;
	double GetMaxLW() ;
	double GetMaxAW1();
	double GetMaxLW1();
	double GetMaxAWO();
	double GetMaxLWO();
	double GetMaxSW1();

	// get the actor number of a session
	int GetSessionActorNumber(std::string strS);

	// get the hierarchical depth of the location
	int GetLocationDepth();
	/*
	* get the merge record
	*/
	std::vector<LocationMerge> GetMergeRecords();

	/*
	* set location depth
	*/
	void SetLocationDepth(int nD);

	/*
	* get the transition graph between locations
	*/
	void GetTransitionGraph(Eigen::MatrixXi& mStates,bool bO);

public:	// actor information

	int GetActorNum();

	std::vector<std::string> GetActors();

	void SetActorWeights(std::string strA, double dbW);

	/* 
	* hide or unhide an actor
	*/
	void HideActor(std::string strA, bool bHide);

	/*
	* uncheck all actors
	*/
	void UncheckAllActors();

	/*
	* check all actors
	*/
	void CheckAllActors();

	/*
	* keep only one actor
	*/
	void KeepOnlyOneActor(std::string strA);

	/*
	* set valid actors
	*/
	void SetValidActors(std::vector<std::string> vActors);

	/*
	* get sequence nSeq, group size nGroup, and sequence in group nGroupSeq of actor strA in time nT
	* a group means some actor moving together from the previous location to this location
	*/
	void GetActorSequence(int nT, std::string strA, int& nSeq, int& nGroup, int& nGroupSeq) const;

	/*
	* get the actor sequence in a session
	*/
	int GetActorSeqInSession(std::string strA, int nT, std::string strL);

	void GetActorPC(std::vector<double>& vActorPC0, std::vector<double>& vActorPC1);

	/*
	* get actor pcs and actor name list
	* new version, consider data overview and detail
	*/
	std::vector<std::string> GetActorPC(std::vector<double>& vActorPC0, std::vector<double>& vActorPC1, bool bO);

	// set grouping actors, return true if setting changed
	bool SetGroupingActors(int nG);

	/*
	* get tips of actor, used when select an actor
	*/
	std::string GetActorTips(std::string strId) const;

	/*
	* get the actor color
	*/
	const int* GetActorColor(std::string strA);

	/*
	* get time and location of spans of this actor
	*/
	void GetActorSpanInfo(std::string strA, std::vector<bool>& vDeath, std::vector<int>& vT, std::vector<std::string>& vL);

	/*
	* Get the display name of the actor
	*/
	std::string GetActorDisplayName(std::string strA);

public: // location information
	/*
	* generate the coordinates of the location pc view
	*/
	void GenerateLocationCoordinates(std::vector<double>& vCY
		, std::vector<double>& vCX
		, std::vector<double>& vSpanY
		, std::vector<double>& vSpanX
		, std::vector<std::string>& vPC0
		, std::vector<std::string>& vPC1
		, bool bO
	);

	/*
	* get the location max
	*/
	int GetLocationMax(std::string strL);

	/*
	* get the display name of the location
	*/
	std::string GetLocationDisplayName(std::string strL);

	/*
	* get the sequence of the location
	*/
	int GetLocationSeq(std::string strL);

	/*
	* Get location correlated actors
	*/
	std::vector<std::string> GetLocationActors(std::vector<std::string> vL);
	/*
	* Get session correlated actors
	*/
	std::vector<std::string> GetSessionActors(std::vector<std::string> vS);

	/*
	* Get actor correlated locations
	*/
	std::vector<std::string> GetActorLocations(std::vector<std::string> vA);

	/*
	* Get actor correlated sessions
	*/
	std::vector<std::string> GetActorSessions(std::vector<std::string> vA);
};

