#pragma once

/*
* define the weight system of the storyline
*/
class SLW {
protected:
	double _dbWO = 1.0;						// user set weight
	double _dbW1 = 0;					    // calculated weight
public:

	double GetW1()const;
	void SetW1(double dbW);
	double GetWO()const;
	void SetWO(double dbW);
	double GetW()const;
};