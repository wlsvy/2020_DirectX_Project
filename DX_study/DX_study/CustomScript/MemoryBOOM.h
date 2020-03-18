#pragma once
#include "../Component/ScriptBehaviour.h"

class MemoryBOOM : public ScriptBehaviour{
public:
	NEVER_USE_CONSTRUCTOR(MemoryBOOM);

	~MemoryBOOM();
	void BOOM();
	void Delete();

	float *boomFLOAT = new float[10000000];
	int boomNum = 10000000;
};