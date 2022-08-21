#include "framework.h"
#include <windows.system.h>
#include <iostream>
#include <thread>

#include "mem.h"
#include "GaugeIncrementsAsm.h"

static double ten = 10;
static double total = 0;
static double Pgreat = 0;
static double Great = 0;
static double Good = 0;
static double Bad = 0;
static double Poor = 0;
static double MashPoor = 0;

int* vNotesNum = (int*)0x0CC27C;
int* vMagicNumber = (int*)0x0CC28C;

static int initialGauge = 0;
static int* gaugeType = (int*)0x0EF840;
int cycleNumber = 0;

static double notesNum = 0;
static double magicNumber = 0;

GaugeIncrements easy;
GaugeIncrements groove;
GaugeIncrements hard;

Gauge easyGauge;
Gauge grooveGauge;
Gauge hardGauge;

void Initialize()
{
	easy = GetIncrements::Easy();
	groove = GetIncrements::Groove();
	hard = GetIncrements::Hard();
	initialGauge = *gaugeType;
	cycleNumber = 0;

	std::cout << "Easy Increments: " << std::endl;
	std::cout << easy.pgreat << std::endl;
	std::cout << easy.great << std::endl;
	std::cout << easy.good << std::endl;
	std::cout << easy.bad << std::endl;
	std::cout << easy.missPoor << std::endl;
	std::cout << easy.mashPoor << std::endl;
	std::cout << "Groove Increments: " << std::endl;
	std::cout << groove.pgreat << std::endl;
	std::cout << groove.great << std::endl;
	std::cout << groove.good << std::endl;
	std::cout << groove.bad << std::endl;
	std::cout << groove.missPoor << std::endl;
	std::cout << groove.mashPoor << std::endl;
	std::cout << "Hard Increments: " << std::endl;
	std::cout << hard.pgreat << std::endl;
	std::cout << hard.great << std::endl;
	std::cout << hard.good << std::endl;
	std::cout << hard.bad << std::endl;
	std::cout << hard.missPoor << std::endl;
	std::cout << hard.mashPoor << std::endl;
}

void InitGaugesThread()
{
	easyGauge.InitializeGauge(20, 2, 100, easy);
	grooveGauge.InitializeGauge(20, 2, 100, groove);
	hardGauge.InitializeGauge(100, 0, 100, hard);

}

struct Graph
{
	int graphNode[1000];
};

Graph easyGraph;
Graph grooveGraph;
Graph hardGraph;

void WriteGraph()
{
	easyGraph.graphNode[cycleNumber] = easyGauge.vGauge;
	grooveGraph.graphNode[cycleNumber] = grooveGauge.vGauge;
	hardGraph.graphNode[cycleNumber] = hardGauge.vGauge;
	cycleNumber++;
}

void SetGraph()
{
	for (int i = 0; i < 1000; i++)
	{
		int* hkGraphNode = (int*)(0x1873F4 + i * 4);
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
	}
	*gaugeType = initialGauge;
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
			*hkGauge = easyGauge.vGauge;
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
			*hkGauge = grooveGauge.vGauge;
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
		if (grooveGauge.vGauge >= 80)
		{
			*hkGauge = grooveGauge.vGauge;
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

	std::cout << "Easy: " << easyGauge.vGauge << std::endl;
	std::cout << "Groove: " << grooveGauge.vGauge << std::endl;
	std::cout << "Hard: " << hardGauge.vGauge << std::endl;
	std::cout << "Hook Gauge: " << *hkGauge << std::endl;
}

void IncrementGauges()
{
	int judgement;
	__asm 
	{
		PUSH EAX
		MOV EAX,[EBP + 0x1C]
		MOV judgement,EAX
		POP EAX
	};
	IncrementGaugesThread(judgement);
}

void ThreadStarter()
{
	notesNum = *vNotesNum;
	magicNumber = *vMagicNumber;
	std::thread initialize(Initialize);
	initialize.detach();
	std::thread initGauges(InitGaugesThread);
	initGauges.detach();
}

void GetIncrements::HookIncrements()
{
	uintptr_t moduleBase;
	if ((moduleBase = (uintptr_t)GetModuleHandle("LR2body.exe")) == 0)
	{
		moduleBase = (uintptr_t)GetModuleHandle("LRHbody.exe");
	}
	mem::Detour32((void*)(moduleBase + 0x0B59FF), (void*)&ThreadStarter, 6);
	mem::Detour32((void*)(moduleBase + 0x006308), (void*)&IncrementGauges, 5);
	mem::Detour32((void*)(moduleBase + 0x01F2EF), (void*)&SetGraph, 6);
	mem::Detour32((void*)(moduleBase + 0x005C45), (void*)&WriteGraph, 6);
	//mem::Detour32((void*)(moduleBase + 0x00632E), (void*)&GaugeShiftThread, 5);

}

double GetIncrements::Total()
{
	static constexpr double CONST_2 = 2;
	static constexpr double CONST_3 = 3;
	static constexpr double CONST_4 = 4;
	static constexpr double CONST_5 = 5;
	static constexpr double CONST_6 = 6;
	static constexpr double CONST_7 = 7;
	static constexpr double CONST_10 = 10;
	static constexpr double CONST_15 = 15;
	static constexpr double CONST_20 = 20;
	static constexpr double CONST_30 = 30;
	static constexpr double CONST_45 = 45;
	static constexpr double CONST_60 = 60;
	static constexpr double CONST_65 = 65;
	static constexpr double CONST_125 = 125;
	static constexpr double CONST_250 = 250;
	static constexpr double CONST_500 = 500;

	if (notesNum <= 20)
	{
		total = 10;
		return total;
	}
	if (notesNum <= 30)
	{
		__asm
		{
			FLD notesNum
			FSUB CONST_20
			FDIV CONST_10
			FMUL CONST_2
			FSUBR CONST_10
			FSTP total
		};
		return total;
	}
	if (notesNum <= 45)
	{
		__asm
		{
			FLD notesNum
			FSUB CONST_30
			FDIV CONST_15
			FSUBR CONST_7
			FSTP total
		};
		return total;
	}
	if (notesNum <= 60)
	{
		__asm
		{
			FLD notesNum
			FSUB CONST_45
			FDIV CONST_15
			FSUBR CONST_6
			FSTP total
		};
		return total;
	}
	if (notesNum <= 125)
	{
		__asm
		{
			FLD notesNum
			FSUB CONST_60
			FDIV CONST_65
			FSUBR CONST_5
			FSTP total
		};
		return total;
	}
	if (notesNum <= 250)
	{
		__asm
		{
			FLD notesNum
			FSUB CONST_125
			FDIV CONST_125
			FSUBR CONST_4
			FSTP total
		};
		return total;
	}
	if (notesNum <= 500)
	{
		__asm
		{
			FLD notesNum
			FSUB CONST_250
			FDIV CONST_250
			FSUBR CONST_3
			FSTP total
		};
		return total;
	}
	if (notesNum <= 1000)
	{
		__asm
		{
			FLD CONST_500
			FLD notesNum
			FSUB CONST_500
			FDIVRP ST(1),ST
			FLD CONST_2
			FSUBRP ST(1),ST
			FSTP total
		};
		return total;
	}
	total = 1;
	return total;
}

GaugeIncrements GetIncrements::Easy()
{
	double total = GetIncrements::Total();
	constexpr double easyConst = 1.2;
	constexpr double bad = -3.2;
	constexpr double poor = -4.8;
	constexpr double mashPoor = -1.6;

	__asm
	{
		FLD notesNum
		FLD magicNumber
		FDIV ST,ST(1)
		FLD easyConst
		FMUL ST,ST(1)
		FST Pgreat
		FSTP Great
		FLD notesNum
		FADD ST,ST
		FLD magicNumber
		FDIV ST,ST(1)
		FMUL easyConst
		FSTP Good
		FLD mashPoor
		FLD poor
		FLD bad
		FSTP Bad
		FSTP Poor
		FSTP MashPoor
	};
	return { Pgreat, Great, Good, MashPoor, Bad, Poor };
}

GaugeIncrements GetIncrements::Groove()
{
	double total = GetIncrements::Total();
	constexpr double bad = -4;
	constexpr double poor = -6;
	constexpr double mashPoor = -2;

	__asm
	{
		FLD notesNum
		FLD magicNumber
		FDIV ST,ST(1)
		FST Great
		FSTP Pgreat
		FLD magicNumber
		FLD notesNum
		FADD ST,ST
		FDIVP ST(1), ST
		FSTP Good
		FLD mashPoor
		FLD poor
		FLD bad
		FSTP Bad
		FSTP Poor
		FSTP MashPoor
	};
	return { Pgreat, Great, Good, MashPoor, Bad, Poor };
}

GaugeIncrements GetIncrements::Hard()
{
	double total = GetIncrements::Total();
	constexpr double pgreat = 0.1;
	constexpr double good = 0.05;
	constexpr double bad = -6;
	constexpr double poor = -10;
	constexpr double mashPoor = -2;

	__asm
	{
		FLD good
		FLD pgreat
		FST Pgreat
		FSTP Great
		FSTP Good
		FLD total
		FLD bad
		FSTP ST(5)
		FMUL ST, ST(4)
		FSTP Bad
		FLD total
		FMUL poor
		FSTP Poor
		FLD total
		FMUL mashPoor
		FSTP MashPoor
	};
	return { Pgreat, Great, Good, MashPoor, Bad, Poor };
}