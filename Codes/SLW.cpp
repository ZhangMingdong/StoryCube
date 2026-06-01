#pragma once

#include "SLW.h"

double SLW::GetW1()const { return _dbW1; }
void SLW::SetW1(double dbW) { 
	_dbW1 = dbW;
}
double SLW::GetWO()const { return _dbWO; }
void SLW::SetWO(double dbW) { _dbWO = dbW; }
double SLW::GetW()const { return _dbWO * _dbW1; }
