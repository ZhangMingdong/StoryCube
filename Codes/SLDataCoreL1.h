#pragma once

#include "SLDataCore.h"


/*
* L1 data is the data after validation tripping and connected the actor and locations
*/
class SLDataCoreL1 :public SLDataCore {
public:
	~SLDataCoreL1();
private:

	/*
	check if two actors are company in current display level
	*/
	bool checkCompany(std::string strA1, std::string strA2);
	/*
	* add actor to location, considering parent
	*/
	void addActorToLocation(std::string strL, std::string strA, int nT);
public:

	/*
	* group the support actors with same behaviors
	* use for L1
	*/
	void GroupSupportActors(std::vector<std::string> listValidActors);

	/*
	* group all the actors with the same behavior
	* prioritize valid actors
	*/
	void GroupActors(std::vector<std::string> listValidActors);
	/*
	* bind locations and actors
	*/
	void BindLocationActor();

	/*
	* build the graph of the sessions
	*/
	void BuildSessionGraph();

	/*
	* Filter actors according to the thredhold
	*/
	void FilterActors();

};