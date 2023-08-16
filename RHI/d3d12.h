//
// RHI
//

#pragma once

#include "rhi.h"

#include <d3d12.h>
#include <dxgi1_4.h>

struct D3D12 : public RHI {
	IDXGIAdapter1 *d3dAdapter;
	ID3D12Device8 *d3dDevice;
	IDXGIFactory4 *dxgiFactory;
	ID3D12CommandAllocator *d3dCommandAllocator;
	ID3D12GraphicsCommandList4 *d3dGraphicsCommandList;
	ID3D12CommandQueue *d3dCommandQueue;

	D3D12();
	bool initialize() override;
	inline void resetCommandList();
	inline void closeCommandList();
	inline void executeCommandDrawIndexed(const CommandDrawIndexed &command);
	bool executeCommandQueue(const CommandQueue &commandQueue) override;
	void drawIndexedInstancedVirtual(uint32_t indexCountPerInstance, uint32_t instanceCount, uint32_t startIndexLocation, int32_t baseVertexLocation, uint32_t startInstanceLocation) override;
};