#pragma once
#include "MyPCScene.h"


class SLGNode;

class MyLocationPCScene : public MyPCScene
{
	Q_OBJECT

public:
	MyLocationPCScene(qreal x, qreal y, qreal width, qreal height, bool bO = false);
	~MyLocationPCScene();
protected:

	// generate according to a scale from 1 to 9
	virtual void generateScene(int nScale);


	// map form actor name to actor node
	std::map<std::string, SLGNode*> _mapNode;
public:
	// set cued actors
	void SetCued(std::vector<std::string> vCued);
protected:
	virtual void handleClearSelection();
};


