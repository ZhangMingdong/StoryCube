#pragma once

#include "MyPCScene.h"

class SLGActorNode;

class MyActorPCScene : public MyPCScene
{
	Q_OBJECT

public:
	MyActorPCScene(qreal x, qreal y, qreal width, qreal height, bool bO = false);
	~MyActorPCScene();
protected:

	/*
	* generate according to a scale from 1 to 9
	*/
	virtual void generateScene(int nScale) noexcept;

	/*
	* generate the actor projection coordinated according to the given range
	* x and y mapping the PC0 and PC1 of the actors
	* the data maped to the given range
	*/
	std::vector<std::string> generateActorProjection(std::map<std::string, double>& mapX, std::map<std::string, double>& mapY, int nRange) noexcept;

	/*
	* generate graphics according to the calculated coordinates
	* nLen: number of points
	* vStrX: vector of names sort by pc1
	* vStrY: vector of names sort by pc0
	* vX: vector of x coordinates
	* vY: vector of y coordinates
	*/
	void generateGraphics(int nLen, const std::vector<std::string>& vStrX, const std::vector<std::string>& vStrY
		, const std::vector<double>& vX, const std::vector<double>& vY);

	// map form actor name to actor node
	std::map<std::string, SLGActorNode*> _mapNode;
public:
	// set cued actors
	void SetCued(std::vector<std::string> vCued);


protected:
	virtual void handleClearSelection();
};


