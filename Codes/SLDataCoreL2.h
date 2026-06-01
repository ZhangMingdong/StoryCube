#pragma once

#include "SLDataCore.h"

/*
* this class used to store the core information of storyline
*/
class SLDataCoreL2 :public SLDataCore {
public:

	/*
	* pre visualization
	*/
	void PreVisualization(int nWeightMethod);



private:

	/*
	* set weight for actors and locations, using pagerank
	*/
	void setWeight();

	/*
	* sort the locations
	*/
	void sortLocations(int nMethod);


	/*
	* generate weights for the sessions
	* generate weights for the timestep
	* generate the time projection
	*/
	void generateSessionWeight();

	/*
	* calculate the depth of the locations
	*/
	void calculateLocationDepth();

	/*
	* using actors as framework, add the weight of sessions.
	* used in the weight calculation loop
	*/
	void updateWeight(const std::vector<std::string>& listActors, const std::vector<std::string>& listLocations);


	/*
		sort using mds
	*/
	void sortMDS(Eigen::MatrixXd D);



	/*
	* build the distance matrix
	* and the maximum diatance
	*/
	void buildDistanceMatrix(int nMethod);

	/*
		build the distance matrix between actors
		given the maximum of location distance
	*/
	void buildActorDistanceMatrix();



};