#include "SC_PlugIn.h"

// InterfaceTable contains pointers to functions in the host
static InterfaceTable *ft;

struct VoxRes : public Unit
{
	float* resampled;
	float* local_buf;
	int resampledFrames;
	float* autocor_buffer;
	float* lpc;
	float* lpc_work;
	float* res;
	float* res_work;
	int lpc_size;
	int max_resonances;
	SndBuf *m_buf;
	float m_fbufnum;
	int m_framepos;
	int offset;
};

static void VoxRes_next_k(VoxRes *unit, int inNumSamples);
static void VoxRes_Ctor(VoxRes *unit);

extern "C" 
{
	float* vox_box_autocorrelate_mut_f32(const float*, size_t, size_t, float*);
	void vox_box_resample_mut_f32(const float*, size_t, size_t, float*);
	void vox_box_normalize_f32(void*, size_t);
	void vox_box_lpc_mut_f32(const float*, size_t, size_t, float*, float*);
	float* vox_box_resonances_mut_f32(float*, size_t, float, int*, float*, float*);
}

void VoxRes_Ctor(VoxRes *unit) 
{
	SETCALC(VoxRes_next_k);
	unit->lpc_size = ZIN0(1);
	unit->resampledFrames = ZIN0(2);
	unit->m_buf = unit->mWorld->mSndBufs;
	unit->m_fbufnum = -1.f;
	unit->m_framepos = 0;
	GET_BUF_SHARED;
	unit->max_resonances = 4;
	unit->offset = 0;
	unit->local_buf = (float*)RTAlloc(unit->mWorld, sizeof(float) * bufFrames);
	unit->autocor_buffer = (float*)RTAlloc(unit->mWorld, sizeof(float) * unit->resampledFrames);
	unit->resampled = (float*)RTAlloc(unit->mWorld, sizeof(float) * unit->resampledFrames);
	unit->lpc = (float*)RTAlloc(unit->mWorld, sizeof(float) * unit->lpc_size + 1);
	unit->lpc_work = (float*)RTAlloc(unit->mWorld, sizeof(float) * unit->lpc_size * 4);
	unit->res = (float*)RTAlloc(unit->mWorld, sizeof(float) * unit->lpc_size + 1);
	unit->res_work = (float*)RTAlloc(unit->mWorld, sizeof(float) * unit->lpc_size * 16);
	for (int i = 0; i < 4; i++) {
		*OUT(i) = 0.f;
	}
	VoxRes_next_k(unit, 1);
}

void VoxRes_next_k(VoxRes *unit, int inNumSamples)
{
	GET_BUF_SHARED;
	for (int i = 0; i < bufFrames; i++) {
		unit->local_buf[i] = bufData[(i + unit->offset) & (bufFrames - 1)];
	}
	vox_box_resample_mut_f32(unit->local_buf, (size_t)bufFrames, unit->resampledFrames, unit->resampled);
	vox_box_autocorrelate_mut_f32(unit->resampled, (size_t)unit->resampledFrames, unit->lpc_size + 2, unit->autocor_buffer);
	vox_box_normalize_f32(unit->autocor_buffer, unit->lpc_size + 2);
	vox_box_lpc_mut_f32(unit->autocor_buffer, unit->lpc_size + 2, unit->lpc_size, unit->lpc, unit->lpc_work); 
	int resonance_count = 0;
	vox_box_resonances_mut_f32(unit->lpc, unit->lpc_size + 1, FULLRATE * unit->resampledFrames / (float)bufFrames, &resonance_count, unit->res_work, unit->res);
	for (int i = 0; (i < resonance_count) && (i < unit->max_resonances); i++) {
		*OUT(i) = unit->res[i];
	}
	unit->offset += (int)BUFRATE;
}

void VoxRes_Dtor(VoxRes *unit) 
{
	RTFree(unit->mWorld, unit->local_buf);
	RTFree(unit->mWorld, unit->autocor_buffer);
	RTFree(unit->mWorld, unit->lpc);
	RTFree(unit->mWorld, unit->lpc_work);
	RTFree(unit->mWorld, unit->res);
	RTFree(unit->mWorld, unit->res_work);
}

PluginLoad(VoxRes)
{
	ft = inTable;
	DefineSimpleUnit(VoxRes);
}



