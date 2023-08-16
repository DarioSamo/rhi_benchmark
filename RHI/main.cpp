//
// RHI
//

#include "d3d12.h"
#include "timer.h"

int main(int argc, char** argv) {
	ElapsedTimer timer;
	Timer::initialize();

	RHI::CommandQueue commandQueue;
	
	D3D12 *d3d12 = new D3D12();
	RHI *rhi = d3d12;
	if (!rhi->initialize()) {
		return 1;
	}
	
	timer.reset();
	const int CommandCount = 100000;
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
	fprintf(stdout, "Queue microseconds: %llu\n", elapsedQueue);
	fprintf(stdout, "Virtual microseconds: %llu\n", elapsedVirtual);

	return 0;
}