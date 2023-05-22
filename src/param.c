#include "param.h"
#include "flash.h"
#include "voice.h"
#include "lfo.h"

paramUnion_t tmp;
int ret[16];

// Write program to memory no. [num]
void WriteProgram(int8_t num) {
	if (num < 0) {
		return;
	}
	
	tmp.param.ratio = GetRatio();
	tmp.param.index = GetIndex();
	tmp.param.ratio2 = GetRatio2();
	tmp.param.index2 = GetIndex2();
	tmp.param.index1Offset = GetIndexOffset();
	tmp.param.attack = GetEnv(0)->a;
	tmp.param.decay = GetEnv(0)->d;
	tmp.param.sustain = GetEnv(0)->s;
	tmp.param.release = GetEnv(0)->r;
	tmp.param.ampLFODpt = GetAmpLfoDepth();
	tmp.param.indexLFODepth = GetIndexLFODepth();
	tmp.param.indexEnvDepth = GetIndexEnvDepth();
	tmp.param.lfo1Period = GetLFO(0)->period;
	tmp.param.algorithm = GetRouteConfig();
	
	for (uint32_t i = 0; i < 16; i++) {
		ret[i] = WriteWord((uint32_t)num * 64 + i * 4, tmp.mem[i]);
	}
}

void LoadProgram(int8_t num) {
	uint32_t* ptr = (uint32_t*)(SECTOR7_BASE + num * 64);
	for (uint32_t i = 0; i < 16; i++, ptr++) {
		tmp.mem[i] = *ptr;
	}
	
	SetRatioRaw(tmp.param.ratio);
	SetIndexRaw((uint16_t)tmp.param.index);
	SetRatio2Raw(tmp.param.ratio2);
	SetIndex2Raw((uint16_t)tmp.param.index2);
	SetIndexOffsetRaw(tmp.param.index1Offset);
	SetAttack(tmp.param.attack);
	SetDecay(tmp.param.decay);
	SetSustain(tmp.param.sustain);
	SetRelease(tmp.param.release);
	SetAmpLfoDepth(tmp.param.ampLFODpt);
	SetIndexLFODepthRaw(tmp.param.indexLFODepth);
	SetIndexEnvDepthRaw(tmp.param.indexEnvDepth);
	GetLFO(0)->period = tmp.param.lfo1Period;
	SetRouteConfigRaw(tmp.param.algorithm);
}

void EraseAllPrograms(void) {
	EraseSector7();
}
