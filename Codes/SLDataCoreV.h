#pragma once

#include "SLDataCore.h"
#include <set>

/*
* this class used to store the core information of storyline
*/
class SLDataCoreV :public SLDataCore {
private:
	// list of hidden locations according to the visualization hierarchy
	std::vector<std::string> _listLHidden;
public:
	SLDataCoreV() {};
	/*
	* generate from l2 data
	*/
	SLDataCoreV(const SLDataCore& data);
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

	/*
	* get the merge record
	*/
	std::vector<LocationMerge> GetMergeRecords() { return _listLocationMerge; }

	/*
	* set location depth
	*/
	void SetLocationDepth(int nD);

	/*
	* get the pcs of actors
	*/
	void GetActorPC(std::vector<double>& vActorPC0, std::vector<double>& vActorPC1);

	/*
	* get the transition graph between locations
	*/
	void GetTransitionGraph(Eigen::MatrixXi& mStates);
private:
	void removeLowLevelNodes(int depthThreshold);
	void showLowLeaves(int depthThreshold);
	void hideHigherClusters(int depthThreshold);
	void addClustersForNewDepth(int targetDepth);
public:
	// actor operation


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
	void GenerateLocationCoordinates(std::vector<double>& vCY, std::vector<double>& vCX, std::vector<double>& vSpanY, std::vector<double>& vSpanX);


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