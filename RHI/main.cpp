//
// RHI
//

#include "d3d12.h"
#include "timer.h"

int main(int argc, char** argv) {
	Timer::initialize();
	ElapsedTimer timer;

	RHI::CommandQueue commandQueue;
	
	D3D12 *d3d12 = new D3D12();
	RHI *rhi = d3d12;
	if (!rhi->initialize()) {
		return 1;
	}
	
	const int IterationCount = 10;
	for (int iter = 0; iter < IterationCount; iter++) {
		const int CommandCount = 1000000;
		
		timer.reset();
		commandQueue.clear();
		for (int i = 0; i < CommandCount; i++) {
			commandQueue.drawIndexedInstanced(3, 1, 0, 0, 0);
		}

		rhi->executeCommandQueue(commandQueue);
		int64_t elapsedQueue = timer.elapsedMicroseconds();

		timer.reset();
		d3d12->resetCommandList();
		for (int i = 0; i < CommandCount; i++) {
			rhi->drawIndexedInstancedVirtual(3, 1, 0, 0, 0);
		}

		d3d12->closeCommandList();
		int64_t elapsedVirtual = timer.elapsedMicroseconds();
		fprintf(stdout, "Queue microseconds #%d: %llu\n", iter, elapsedQueue);
		fprintf(stdout, "Virtual microseconds #%d: %llu\n", iter, elapsedVirtual);
	}

	return 0;
}