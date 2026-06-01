#pragma once

#include "SLDataCore.h"


/*
* L0 data is the raw data read from the data file
*/
class SLDataCoreL0:public SLDataCore {
public:
	~SLDataCoreL0();
public:

	/*
	* read data
	* L0
	*/
	void ReadStory(QXmlStreamReader& reader, int nTimeRange = 100000);

private:
	/*
	read the characters
	*/
	void readActors(QXmlStreamReader& reader);

	/*
		read a character
	*/
	void readActor(QXmlStreamReader& reader);

	/*
		read location names
	*/
	void readLocationNames(QXmlStreamReader& reader);

	/*
		read sessions
	*/
	void readSessions(QXmlStreamReader& reader);
};