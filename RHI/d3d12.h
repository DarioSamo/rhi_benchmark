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
	ID3D12RootSignature *d3dRootSignature;
	ID3DBlob *d3dRootSignatureBlob;

	D3D12();
	bool initialize() override;
	inline void resetCommandList();
	inline void closeCommandList();
	inline void executeCommandDrawIndexed(const CommandDrawIndexed &command);
	inline void executeCommandSetGraphicsRoot32BitConstant(const CommandSetGraphicsRoot32BitConstant &command);
	bool executeCommandQueue(const CommandQueue &commandQueue) override;
	void drawIndexedInstanced(uint32_t indexCountPerInstance, uint32_t instanceCount, uint32_t startIndexLocation, int32_t baseVertexLocation, uint32_t startInstanceLocation);
	void setGraphicsRoot32BitConstant(uint32_t rootParameterIndex, uint32_t srcData, uint32_t destOffsetIn32BitValues);
	void drawIndexedInstancedVirtual(uint32_t indexCountPerInstance, uint32_t instanceCount, uint32_t startIndexLocation, int32_t baseVertexLocation, uint32_t startInstanceLocation) override;
	void setGraphicsRoot32BitConstantVirtual(uint32_t rootParameterIndex, uint32_t srcData, uint32_t destOffsetIn32BitValues) override;
};