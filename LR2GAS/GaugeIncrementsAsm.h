#pragma once

#include <cmath>

#include "framework.h"
#include "mem.h"

//static double* hkGauge = (double*)0x187200;

static double* hkGauge = (double*)0x187200;
static double* hkPgreat = (double*)0x187258;
static double* hkGreat = (double*)0x187250;
static double* hkGood = (double*)0x187248;
static double* hkBad = (double*)0x187240;
static double* hkPoor = (double*)0x187238;
static double* hkMashPoor = (double*)0x187230;

struct GaugeIncrements
{
	double pgreat;
	double great;
	double good;
	double mashPoor;
	double bad;
	double missPoor;
};

class Gauge
{
private:
	double pgreatInc;
	double greatInc;
	double goodInc;
	double badInc;
	double missPoorInc;
	double mashPoorInc;
	double lowLimit;
	double topLimit;
	int decideIncrement;

	bool CheckLowLimit(double increment)
	{
		if (vGauge + increment < lowLimit)
		{
			vGauge = lowLimit;
			return 1;
		}
		return 0;
	}
	bool CheckTopLimit(double increment)
	{
		if (vGauge + increment > topLimit)
		{
			vGauge = topLimit;
			return 1;
		}
		return 0;
	}
public:
	double vGauge;
	void InitializeGauge(double initialGauge, double lowLimit, double topLimit, const struct GaugeIncrements& GaugeIncrements)
	{
		this->vGauge = initialGauge;
		this->lowLimit = lowLimit;
		this->topLimit = topLimit;
		pgreatInc = GaugeIncrements.pgreat;
		greatInc = GaugeIncrements.great;
		goodInc = GaugeIncrements.good;
		badInc = GaugeIncrements.bad;
		missPoorInc = GaugeIncrements.missPoor;
		mashPoorInc = GaugeIncrements.mashPoor;
	}
	void IncrementGauge(int judgement, int gaugeType)
	{
		switch (judgement)
		{
		case 0:
			if (CheckLowLimit(mashPoorInc) != 0)
			{
				break;
			}
			if (gaugeType != 1)
			{
				vGauge += mashPoorInc;
				break;
			}
			if (vGauge <= 32)
			{
				vGauge += mashPoorInc * 0.6;
			}
			else
			{
				vGauge += mashPoorInc;
			}
			break;
		case 1:
			if (CheckLowLimit(missPoorInc) != 0)
			{
				break;
			}
			if (gaugeType != 1)
			{
				vGauge += missPoorInc;
				break;
			}
			if (vGauge <= 32)
			{
				vGauge += missPoorInc * 0.6;
			}
			else
			{
				vGauge += missPoorInc;
			}
			break;
		case 2:
			if (CheckLowLimit(badInc) != 0)
			{
				break;
			}
			if (gaugeType != 1)
			{
				vGauge += badInc;
				break;
			}
			if (vGauge <= 32)
			{
				vGauge += badInc * 0.6;
			}
			else
			{
				vGauge += badInc;
			}
			break;
		case 3:
			if (CheckTopLimit(goodInc) != 0)
			{
				break;
			}
			vGauge += goodInc;
			break;
		case 4:
			if (CheckTopLimit(greatInc) != 0)
			{
				break;
			}
			vGauge += greatInc;
			break;
		case 5:
			if (CheckTopLimit(pgreatInc) != 0)
			{
				break;
			}
			vGauge += pgreatInc;
			break;
		default:
			break;
		}
	}
};

namespace GetIncrements
{
	void HookIncrements();
	double Total();
	GaugeIncrements Easy();
	GaugeIncrements Groove();
	GaugeIncrements Hard();
}