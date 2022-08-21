#pragma once

struct GaugeIncrements final
{
	double pgreat = 0.0;
	double great = 0.0;
	double good = 0.0;
	double mashPoor = 0.0;
	double bad = 0.0;
	double missPoor = 0.0;
};

class Gauge final
{
public:
	Gauge() = default;

	Gauge(const double initialGauge, const double lowLimit, const double topLimit, const struct GaugeIncrements& gaugeIncrements) :
		vGauge(initialGauge),
		lowLimit(lowLimit),
		topLimit(topLimit),
		pgreatInc(gaugeIncrements.pgreat),
		greatInc(gaugeIncrements.great),
		goodInc(gaugeIncrements.good),
		badInc(gaugeIncrements.bad),
		missPoorInc(gaugeIncrements.missPoor),
		mashPoorInc(gaugeIncrements.mashPoor) {}

	Gauge(const Gauge&) = default;
	Gauge(Gauge&&) noexcept = default;

	Gauge& operator=(const Gauge&) = default;
	Gauge& operator=(Gauge&&) noexcept = default;

	~Gauge() = default;

	double getVGauge()
	{
		return vGauge;
	}

	// TODO: gaugeType should be an enum.
	void IncrementGauge(const int judgement, const int gaugeType)
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

private:
	double vGauge = 0.0;

	double pgreatInc = 0.0;
	double greatInc = 0.0;
	double goodInc = 0.0;
	double badInc = 0.0;
	double missPoorInc = 0.0;
	double mashPoorInc = 0.0;
	double lowLimit = 0.0;
	double topLimit = 0.0;
	int decideIncrement = 0;

	bool CheckLowLimit(const double increment)
	{
		if (vGauge + increment < lowLimit)
		{
			vGauge = lowLimit;
			return true;
		}
		return false;
	}

	bool CheckTopLimit(const double increment)
	{
		if (vGauge + increment > topLimit)
		{
			vGauge = topLimit;
			return true;
		}
		return false;
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
