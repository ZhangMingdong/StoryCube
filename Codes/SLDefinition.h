#pragma once
#include <vector>
#include <string>

// global definitions for the storyline

/*
* weight method
* 0: using adjacent timespan of the actor weight
* 1: using adjacent timespan of the actor weight multiply the timespace between the two pan
* 2: using each pair of timespan of the actor weight divide the span index distance
* 3: using each pair of timespan of the actor weight multiply the timespace and divide the span index distance
*/
#define WeightMethod_0	0
#define WeightMethod_1	1
#define WeightMethod_2	2
#define WeightMethod_3	3


/*
* define a number for the views: scene, location overview, location focused, actor overview, actor focused
*/
#define View_Null	 0
#define View_Scene	 1
#define View_LO		 2
#define View_LF		 3
#define View_AO		 4
#define View_AF		 5


class SLDataCoreInterface {
public:
	/*
	* try to swap actors at j and k in listA at time index nTimeIndex according to time index nRefTimeIndex
	*/
	virtual void tryToSwap(std::vector<std::string>& listA, int nTimeIndex, int nRefTimeIndex, int j, int k) const = 0;

	/*
	* merge two session to a new one
	* put into the list
	* return the id
	*/
	virtual std::string mergeSession(std::string strL, std::string strS1, std::string strS2) = 0;

	/*
	* sort actors in vA
	*/
	virtual void sortActors(std::vector<std::string>& vA) = 0;
};

struct LocationMerge {
	std::string _strL1;
	std::string _strL2;
	std::string _strC;
	int _nToRoot = -1;
};

/*
* generate a new id from time and location
*/
std::string GenerateSessionID(int nT, std::string strL);