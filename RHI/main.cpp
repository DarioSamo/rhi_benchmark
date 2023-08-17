//
// RHI
//

#include "d3d12.h"
#include "timer.h"

#include <list>

//#define UNRELATED_CODE_EACH_ITERATION

struct DrawParams {
	uint32_t srcData;
	uint32_t indexCount;
};

#ifdef UNRELATED_CODE_EACH_ITERATION

void do_stuff() {
	// Calculate pi a bunch of times
	for (int i = 0; i < 1; i++) {
		volatile double sum = 0.0;
		int sign = 1;
		for (int i = 0; i < 100; ++i) {
			sum += sign / (2.0 * i + 1.0);
			sign *= -1;
		}
	}
}

#endif

int main(int argc, char** argv) {
	Timer::initialize();
	ElapsedTimer timer;

	RHI::CommandQueue commandQueue;
	
	D3D12 *d3d12 = new D3D12();
	RHI *rhi = d3d12;
	if (!rhi->initialize()) {
		return 1;
	}
	
	const int CommandCount = 100000;
	std::list<DrawParams> drawParamsList;
	for (int i = 0; i < CommandCount; i++) {
		DrawParams params;
		params.indexCount = 3;
		params.srcData = 1;
		drawParamsList.push_back(params);
	}

	int64_t elapsedQueueTotal = 0;
	int64_t elapsedVirtualTotal = 0;
	int64_t elapsedDirectTotal = 0;
	const int IterationCount = 25;
	for (int iter = 0; iter < IterationCount; iter++) {
		timer.reset();
		commandQueue.clear();
		for (const DrawParams &params : drawParamsList) {
			commandQueue.setGraphicsRoot32BitConstant(0, params.srcData, 0);
			commandQueue.drawIndexedInstanced(params.indexCount, 1, 0, 0, 0);

#		ifdef UNRELATED_CODE_EACH_ITERATION
			do_stuff();
#		endif
		}

		rhi->executeCommandQueue(commandQueue);
		int64_t elapsedQueue = timer.elapsedMicroseconds();

		timer.reset();
		d3d12->resetCommandList();
		for (const DrawParams &params : drawParamsList) {
			rhi->setGraphicsRoot32BitConstantVirtual(0, params.srcData, 0);
			rhi->drawIndexedInstancedVirtual(params.indexCount, 1, 0, 0, 0);

#		ifdef UNRELATED_CODE_EACH_ITERATION
			do_stuff();
#		endif
		}

		d3d12->closeCommandList();
		int64_t elapsedVirtual = timer.elapsedMicroseconds();

		timer.reset();
		d3d12->resetCommandList();
		for (const DrawParams &params : drawParamsList) {
			d3d12->setGraphicsRoot32BitConstant(0, params.srcData, 0);
			d3d12->drawIndexedInstanced(params.indexCount, 1, 0, 0, 0);

#		ifdef UNRELATED_CODE_EACH_ITERATION
			do_stuff();
#		endif
		}

		d3d12->closeCommandList();
		int64_t elapsedDirect = timer.elapsedMicroseconds();

		fprintf(stdout, "Queue microseconds #%d: %llu\n", iter, elapsedQueue);
		fprintf(stdout, "Virtual microseconds #%d: %llu\n", iter, elapsedVirtual);
		fprintf(stdout, "Direct microseconds #%d: %llu\n", iter, elapsedDirect);
		elapsedQueueTotal += elapsedQueue;
		elapsedVirtualTotal += elapsedVirtual;
		elapsedDirectTotal += elapsedDirect;
	}

	fprintf(stdout, "\n");
	fprintf(stdout, "Queue microseconds Total: %llu\n", elapsedQueueTotal);
	fprintf(stdout, "Virtual microseconds Total: %llu\n", elapsedVirtualTotal);
	fprintf(stdout, "Direct microseconds Total: %llu\n", elapsedDirectTotal);

	return 0;
}