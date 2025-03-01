#include "GaugeIncrementsAsm.h"

#include "framework.h"

#include <windows.system.h>
#include <iostream>
#include <thread>

#include "mem.h"
#include "winver.h"

// TODO: Everything should be done using RAII approach, Initialize-Finalize is C style and is not recommended.

namespace
{
	unsigned int g_win10Offset = 0;

	double* hkGauge;
	double* hkPgreat;
	double* hkGreat;
	double* hkGood;
	double* hkBad;
	double* hkPoor;
	double* hkMashPoor;

	int* vNotesNum;
	int* vMagicNumber;

	int initialGauge = 0;

	bool isCourse = 0;

	// TODO: This should be an enum.
	int* gaugeType;
	int* battleType;

	int cycleNumber = 0;

	double notesNum = 0.0;
	double magicNumber = 0.0;

	GaugeIncrements easy = {};
	GaugeIncrements groove = {};
	GaugeIncrements hard = {};

	Gauge easyGauge = {};
	Gauge grooveGauge = {};
	Gauge hardGauge = {};

	struct Graph
	{
		int graphNode[1000] = {};
	};

	Graph easyGraph = {};
	Graph grooveGraph = {};
	Graph hardGraph = {};

	void LogIncrementsToCout() {
		// TODO: test with std::cout.sync_with_stdio(false).
		std::cout << "Easy Increments:" << '\n';
		std::cout << easy.pgreat << '\n';
		std::cout << easy.great << '\n';
		std::cout << easy.good << '\n';
		std::cout << easy.bad << '\n';
		std::cout << easy.missPoor << '\n';
		std::cout << easy.mashPoor << '\n';
		std::cout << "Groove Increments:" << '\n';
		std::cout << groove.pgreat << '\n';
		std::cout << groove.great << '\n';
		std::cout << groove.good << '\n';
		std::cout << groove.bad << '\n';
		std::cout << groove.missPoor << '\n';
		std::cout << groove.mashPoor << '\n';
		std::cout << "Hard Increments:" << '\n';;
		std::cout << hard.pgreat << '\n';
		std::cout << hard.great << '\n';
		std::cout << hard.good << '\n';
		std::cout << hard.bad << '\n';
		std::cout << hard.missPoor << '\n';
		std::cout << hard.mashPoor << std::endl;
	}

	void Initialize()
	{
		initialGauge = *gaugeType;

		if (isCourse == 1 || *battleType == 4 || *gaugeType == 2 || *gaugeType > 3)
		{
			if (isCourse == 1)
			{
				std::cout << "Course detected, GAS deactivated" << std::endl;
			}
			else if (*battleType == 4)
			{
				std::cout << "G-Battle detected, GAS deactivated" << std::endl;
			}
			else
			{
				std::cout << "Gimmick gauge type detected, GAS deactivated" << std::endl;
			}
			return;
		}

		easy = GetIncrements::Easy();
		groove = GetIncrements::Groove();
		hard = GetIncrements::Hard();

		switch (*gaugeType)
		{
		case 0:
		{
			*hkPgreat = groove.pgreat;
			*hkGreat = groove.great;
			*hkGood = groove.good;
			*hkBad = groove.bad;
			*hkPoor = groove.missPoor;
			*hkMashPoor = groove.mashPoor;
		}
		break;
		case 1:
		{
			*hkPgreat = hard.pgreat;
			*hkGreat = hard.great;
			*hkGood = hard.good;
			*hkBad = hard.bad;
			*hkPoor = hard.missPoor;
			*hkMashPoor = hard.mashPoor;
		}
		break;
		case 3:
		{
			*hkPgreat = easy.pgreat;
			*hkGreat = easy.great;
			*hkGood = easy.good;
			*hkBad = easy.bad;
			*hkPoor = easy.missPoor;
			*hkMashPoor = easy.mashPoor;
		}
		break;
		}

		cycleNumber = 0;

		LogIncrementsToCout();
	}

	void InitGaugesThread()
	{
		easyGauge = Gauge(20, 2, 100, easy);
		grooveGauge = Gauge(20, 2, 100, groove);
		hardGauge = Gauge(100, 0, 100, hard);
	}

	void WriteGraph()
	{
		if (isCourse == 1 || *battleType == 4 || *gaugeType == 2 || *gaugeType > 3)
		{
			return;
		}
		int* pointerToArrayForGraph = nullptr;
		__asm
		{
			MOV pointerToArrayForGraph, ESI
		};
		if (pointerToArrayForGraph != (int*)(0x1873F4 + g_win10Offset))
		{
			return;
		}
		easyGraph.graphNode[cycleNumber] = static_cast<int>(easyGauge.getVGauge());
		grooveGraph.graphNode[cycleNumber] = static_cast<int>(grooveGauge.getVGauge());
		hardGraph.graphNode[cycleNumber] = static_cast<int>(hardGauge.getVGauge());
		cycleNumber++;
	}

	void SetGraph()
	{
		if (isCourse == 1 || *battleType == 4 || *gaugeType == 2 || *gaugeType > 3)
		{
			return;
		}

		for (int i = 0; i < 1000; i++)
		{
			int* hkGraphNode = (int*)(0x1873F4 + g_win10Offset + i * sizeof(int));
			switch (*gaugeType)
			{
			case 0:
				*hkGraphNode = grooveGraph.graphNode[i];
				break;
			case 1:
				*hkGraphNode = hardGraph.graphNode[i];
				break;
			case 3:
				*hkGraphNode = easyGraph.graphNode[i];
				break;
			}
			grooveGraph.graphNode[i] = 0;
			hardGraph.graphNode[i] = 0;
			easyGraph.graphNode[i] = 0;
		}


	}

	void GaugeRestore()
	{
		int* opType = nullptr;
		__asm
		{
			PUSH[ESP + 0x48]
			POP opType
		};
		if (opType != (int*)0x6F90)
		{
			return;
		}
		*gaugeType = initialGauge;
	}

	void SwitchForCourses()
	{
		isCourse = 1;
	}

	void SwitchForNormal()
	{
		isCourse = 0;
	}

	void IncrementGaugesThread(int judgement)
	{
		easyGauge.IncrementGauge(judgement, *gaugeType);
		grooveGauge.IncrementGauge(judgement, *gaugeType);
		hardGauge.IncrementGauge(judgement, *gaugeType);

		switch (*gaugeType)
		{
		case 0:
			if (*hkGauge < 80)
			{
				*hkGauge = easyGauge.getVGauge();
				*hkPgreat = easy.pgreat;
				*hkGreat = easy.great;
				*hkGood = easy.good;
				*hkBad = easy.bad;
				*hkPoor = easy.missPoor;
				*hkMashPoor = easy.mashPoor;
				*gaugeType = 3;
			}
			break;
		case 1:
			if (*hkGauge < 2)
			{
				*hkGauge = grooveGauge.getVGauge();
				*hkPgreat = groove.pgreat;
				*hkGreat = groove.great;
				*hkGood = groove.good;
				*hkBad = groove.bad;
				*hkPoor = groove.missPoor;
				*hkMashPoor = groove.mashPoor;
				*gaugeType = 0;
			}
			break;
		case 3:
			if (grooveGauge.getVGauge() >= 80)
			{
				*hkGauge = grooveGauge.getVGauge();
				*hkPgreat = groove.pgreat;
				*hkGreat = groove.great;
				*hkGood = groove.good;
				*hkBad = groove.bad;
				*hkPoor = groove.missPoor;
				*hkMashPoor = groove.mashPoor;
				*gaugeType = 0;
			}
			break;
		}

		std::cout << "Easy: " << easyGauge.getVGauge() << '\n';
		std::cout << "Groove: " << grooveGauge.getVGauge() << '\n';
		std::cout << "Hard: " << hardGauge.getVGauge() << '\n';
		std::cout << "Hook Gauge: " << *hkGauge << std::endl;
	}

	void IncrementGauges()
	{
		if (isCourse == 1 || *battleType == 4 || *gaugeType == 2 || *gaugeType > 3)
		{
			return;
		}

		int judgement;
		int isGhost;
		__asm
		{
			PUSH[EBP + 0x1C]
			POP judgement
			PUSH[EBP + 0x24]
			POP isGhost
		};
		if (isGhost == 1)
		{
			return;
		}
		IncrementGaugesThread(judgement);
	}

	void ThreadStarter()
	{
		notesNum = *vNotesNum;
		magicNumber = *vMagicNumber;
		std::thread initialize(&Initialize);
		initialize.detach();

		if (isCourse == 1)
		{
			return;
		}
		std::thread initGauges(&InitGaugesThread);
		initGauges.detach();
	}
}

static void set_global_addresses()
{
	const double g_winver = getSysOpType();
	if (g_winver >= 10)
		g_win10Offset = 0x10000;
	else
		g_win10Offset = 0;

	std::cout << "winver: " << g_winver << '\n'
		<< "win10Offset: " << g_win10Offset << '\n';

	hkGauge = (double*)(0x187200 + g_win10Offset);
	hkPgreat = (double*)(0x187258 + g_win10Offset);
	hkGreat = (double*)(0x187250 + g_win10Offset);
	hkGood = (double*)(0x187248 + g_win10Offset);
	hkBad = (double*)(0x187240 + g_win10Offset);
	hkPoor = (double*)(0x187238 + g_win10Offset);
	hkMashPoor = (double*)(0x187230 + g_win10Offset);

	vNotesNum = (int*)(0x0CC27C + g_win10Offset);
	vMagicNumber = (int*)(0x0CC28C + g_win10Offset);

	gaugeType = (int*)(0x0EF840 + g_win10Offset);
	battleType = (int*)(0x0EF884 + g_win10Offset);
}

void GetIncrements::HookIncrements()
{
	uintptr_t moduleBase;
	// TODO: check for errors.
	// https://docs.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getmodulehandlea#return-value
	if ((moduleBase = (uintptr_t)GetModuleHandle("LR2body.exe")) == 0)
	{
		moduleBase = (uintptr_t)GetModuleHandle("LRHbody.exe");
	}

	set_global_addresses();

	mem::Detour32((void*)(moduleBase + 0x0B59FF), (void*)&ThreadStarter, 6);
	mem::Detour32((void*)(moduleBase + 0x0AD669), (void*)&ThreadStarter, 5);
	mem::Detour32((void*)(moduleBase + 0x006308), (void*)&IncrementGauges, 5);
	mem::Detour32((void*)(moduleBase + 0x01F2EF), (void*)&SetGraph, 6);
	mem::Detour32((void*)(moduleBase + 0x005C45), (void*)&WriteGraph, 6);
	mem::Detour32((void*)(moduleBase + 0x031A74), (void*)&GaugeRestore, 5);
	mem::Detour32((void*)(moduleBase + 0x0B5A88), (void*)&SwitchForCourses, 6);
	mem::Detour32((void*)(moduleBase + 0x0B5C02), (void*)&SwitchForNormal, 6);
}

double GetIncrements::Total()
{
	if (notesNum < 20)
	{
		return 10.0;
	}
	if (notesNum < 30)
	{
		return 14.0 - (notesNum / 5.0);
	}
	if (notesNum < 60)
	{
		return 9.0 - (notesNum / 15.0);
	}
	if (notesNum < 125)
	{
		return 5.0 - ((notesNum - 60.0) / 65.0);
	}
	if (notesNum < 250)
	{
		return 5.0 - (notesNum / 125.0);
	}
	if (notesNum < 500)
	{
		return 4.0 - (notesNum / 250.0);
	}
	if (notesNum < 1000)
	{
		return 3.0 - (notesNum / 500.0);
	}
	return 1;
}

GaugeIncrements GetIncrements::Easy()
{
	constexpr double easyConst = 1.2;

	GaugeIncrements result;
	result.pgreat = easyConst * (magicNumber / notesNum);
	result.great = easyConst * (magicNumber / notesNum);
	result.good = easyConst * (magicNumber / (notesNum * 2.0));
	result.mashPoor = -1.6;
	result.bad = -3.2;
	result.missPoor = -4.8;

	return result;
}

GaugeIncrements GetIncrements::Groove()
{
	GaugeIncrements result;
	result.pgreat = magicNumber / notesNum;
	result.great = magicNumber / notesNum;
	result.good = magicNumber / (notesNum * 2.0);
	result.mashPoor = -2.0;
	result.bad = -4.0;
	result.missPoor = -6.0;

	return result;
}

GaugeIncrements GetIncrements::Hard()
{
	double total = GetIncrements::Total();

	GaugeIncrements result;
	result.pgreat = 0.1;
	result.great = 0.1;
	result.good = 0.05;
	result.mashPoor = total * -2.0;
	result.bad = total * -6.0;
	result.missPoor = total * -10.0;

	return result;
}
