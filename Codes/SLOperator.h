#pragma once

#include <vector>
#include <string>

class SLData;

/// <summary>
/// class for the data structure of storyline operator
/// </summary>
class SLOperator
{
public:
	SLOperator(SLData* pData);
	~SLOperator();
private:
	SLData* _pData = NULL;
	// selected locations
	std::vector<std::string> _listSelectedL;
	// selected actors
	std::vector<std::string> _listSelectedA;
	// selected sessions
	std::vector<std::string> _listSelectedS;
private:
	void clearSelection();
public:
	void SelectLocations(std::vector<std::string> listL);
	void SelectActors(std::vector<std::string> listA);
	void SelectSessions(std::vector<std::string> listS);

	void SelectLocation(std::string strL);
	void SelectActor(std::string strA);
	void SelectSession(std::string strS);

	void ClearActorSelection();
	void ClearLocationSelection();

	std::string GetTips();

	std::vector<std::string> GetSelectedSessions() { return _listSelectedS; }

	std::vector<std::string> GetSelectedLocations() { return _listSelectedL; }

	std::vector<std::string> GetSelectedActors() { return _listSelectedA; }
};
