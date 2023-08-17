//
// RHI
//

#include "d3d12.h"

// D3D12

D3D12::D3D12() { }

bool D3D12::initialize() {
	HRESULT result;
	UINT dxgiFactoryFlags = 0;
	result = CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&dxgiFactory));
	if (FAILED(result)) {
		fprintf(stderr, "CreateDXGIFactory2 failed with result 0x%X.\n", result);
		return false;
	}

	DXGI_ADAPTER_DESC1 desc;
	for (UINT adapterIndex = 0; dxgiFactory->EnumAdapters1(adapterIndex, &d3dAdapter) != DXGI_ERROR_NOT_FOUND; ++adapterIndex) {
		d3dAdapter->GetDesc1(&desc);

		// Ignore software adapters.
		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
			d3dAdapter->Release();
			d3dAdapter = nullptr;
			continue;
		}

		result = D3D12CreateDevice(d3dAdapter, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&d3dDevice));
		if (SUCCEEDED(result)) {
			break;
		}
		else {
			d3dDevice->Release();
			d3dDevice = nullptr;

			d3dAdapter->Release();
			d3dAdapter = nullptr;
		}
	}

	if (d3dDevice == nullptr) {
		fprintf(stderr, "Unable to create a D3D device.\n");
		return false;
	}

	result = d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&d3dCommandAllocator));
	if (FAILED(result)) {
		fprintf(stderr, "CreateCommandAllocator failed with result 0x%X.\n", result);
		return false;
	}

	D3D12_COMMAND_QUEUE_DESC queueDesc = { };
	queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.NodeMask = 0;
	result = d3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&d3dCommandQueue));
	if (FAILED(result)) {
		fprintf(stderr, "CreateCommandQueue failed with result 0x%X.\n", result);
		return false;
	}

	result = d3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, d3dCommandAllocator, nullptr, IID_PPV_ARGS(&d3dGraphicsCommandList));
	if (FAILED(result)) {
		fprintf(stderr, "CreateCommandList failed with result 0x%X.\n", result);
		return false;
	}

	D3D12_VERSIONED_ROOT_SIGNATURE_DESC rootSigDesc = { };
	D3D12_ROOT_PARAMETER rootParameter = { };
	rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	rootParameter.Constants.Num32BitValues = 1;
	rootParameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootSigDesc.Desc_1_0.pParameters = &rootParameter;
	rootSigDesc.Desc_1_0.NumParameters = 1;
	rootSigDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1;

	result = D3D12SerializeVersionedRootSignature(&rootSigDesc, &d3dRootSignatureBlob, nullptr);
	if (FAILED(result)) {
		fprintf(stderr, "D3D12SerializeVersionedRootSignature failed with result 0x%X.\n", result);
		return false;
	}

	result = d3dDevice->CreateRootSignature(0, d3dRootSignatureBlob->GetBufferPointer(), d3dRootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&d3dRootSignature));
	if (FAILED(result)) {
		fprintf(stderr, "CreateRootSignature failed with result 0x%X.\n", result);
		return false;
	}

	return true;
}

void D3D12::resetCommandList() {
	d3dCommandAllocator->Reset();
	d3dGraphicsCommandList->Reset(d3dCommandAllocator, nullptr);

	///
	d3dGraphicsCommandList->SetGraphicsRootSignature(d3dRootSignature);
	///
}

void D3D12::closeCommandList() {
	d3dGraphicsCommandList->Close();
}

void D3D12::executeCommandDrawIndexed(const CommandDrawIndexed &command) {
	d3dGraphicsCommandList->DrawIndexedInstanced(command.indexCountPerInstance, command.instanceCount, command.startIndexLocation, command.baseVertexLocation, command.startInstanceLocation);
}

void D3D12::executeCommandSetGraphicsRoot32BitConstant(const CommandSetGraphicsRoot32BitConstant &command) {
	d3dGraphicsCommandList->SetGraphicsRoot32BitConstant(command.rootParameterIndex, command.srcData, command.destOffsetIn32BitValues);
}

bool D3D12::executeCommandQueue(const CommandQueue &commandQueue) {
	resetCommandList();

	CommandType commandType;
	size_t readCursor = 0;
	do {
		commandType = commandQueue.peek(readCursor);

		switch (commandType) {
		case CommandType::DrawIndexed:
			executeCommandDrawIndexed(commandQueue.read<CommandDrawIndexed>(readCursor));
			break;
		case CommandType::SetGraphicsRoot32BitConstant:
			executeCommandSetGraphicsRoot32BitConstant(commandQueue.read<CommandSetGraphicsRoot32BitConstant>(readCursor));
			break;
		default:
			break;
		}
	} while (commandType != CommandType::End);

	closeCommandList();

	return true;
}

void D3D12::drawIndexedInstanced(uint32_t indexCountPerInstance, uint32_t instanceCount, uint32_t startIndexLocation, int32_t baseVertexLocation, uint32_t startInstanceLocation) {
	d3dGraphicsCommandList->DrawIndexedInstanced(indexCountPerInstance, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
}

void D3D12::setGraphicsRoot32BitConstant(uint32_t rootParameterIndex, uint32_t srcData, uint32_t destOffsetIn32BitValues) {
	d3dGraphicsCommandList->SetGraphicsRoot32BitConstant(rootParameterIndex, srcData, destOffsetIn32BitValues);
}

void D3D12::drawIndexedInstancedVirtual(uint32_t indexCountPerInstance, uint32_t instanceCount, uint32_t startIndexLocation, int32_t baseVertexLocation, uint32_t startInstanceLocation) {
	d3dGraphicsCommandList->DrawIndexedInstanced(indexCountPerInstance, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
}

void D3D12::setGraphicsRoot32BitConstantVirtual(uint32_t rootParameterIndex, uint32_t srcData, uint32_t destOffsetIn32BitValues) {
	d3dGraphicsCommandList->SetGraphicsRoot32BitConstant(rootParameterIndex, srcData, destOffsetIn32BitValues);
}