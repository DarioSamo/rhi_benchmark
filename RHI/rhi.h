//
// RHI
//

#pragma once

#include <vector>

struct RHI {
	enum class CommandType : uint32_t {
		Unknown = 0,
		DrawInstanced,
		DrawIndexedInstanced,
		SetGraphicsRoot32BitConstant,
		SetViewport,
		SetScissorRect,
		End = 0xFFFFFFFF
	};

	struct CommandDrawInstanced {
		CommandType commandType = CommandType::DrawInstanced;
		uint32_t vertexCountPerInstance;
		uint32_t instanceCount;
		uint32_t startVertexLocation;
		uint32_t startInstanceLocation;
	};

	struct CommandDrawIndexedInstanced {
		CommandType commandType = CommandType::DrawIndexedInstanced;
		uint32_t indexCountPerInstance;
		uint32_t instanceCount;
		uint32_t startIndexLocation;
		int32_t baseVertexLocation;
		uint32_t startInstanceLocation;
	};

	struct CommandSetGraphicsRoot32BitConstant {
		CommandType commandType = CommandType::SetGraphicsRoot32BitConstant;
		uint32_t rootParameterIndex;
		uint32_t srcData;
		uint32_t destOffsetIn32BitValues;
	};

	struct CommandSetViewport {
		CommandType commandType = CommandType::SetViewport;
		float topLeftX;
		float topLeftY;
		float width;
		float height;
		float minDepth;
		float maxDepth;
	};

	struct CommandSetScissorRect {
		CommandType commandType = CommandType::SetScissorRect;
		int left;
		int top;
		int right;
		int bottom;
	};

	struct CommandQueue {
		std::vector<uint8_t> commandBytes;

		template<typename T>
		void queue(const T &command) {
			size_t currentSize = commandBytes.size();
			commandBytes.resize(currentSize + sizeof(T));
			memcpy(commandBytes.data() + currentSize, reinterpret_cast<const uint8_t *>(&command), sizeof(T));
		}

		CommandType peek(size_t &cursor) const {
			if (cursor >= commandBytes.size()) {
				return CommandType::End;
			}
			else {
				return *reinterpret_cast<const CommandType *>(commandBytes.data() + cursor);
			}
		}

		template<typename T>
		const T &read(size_t &cursor) const {
			const T *commandPtr = reinterpret_cast<const T *>(commandBytes.data() + cursor);
			cursor += sizeof(T);
			return *commandPtr;
		}

		void clear() {
			commandBytes.clear();
		}

		void drawInstanced(uint32_t vertexCountPerInstance, uint32_t instanceCount, uint32_t startVertexLocation, uint32_t startInstanceLocation) {
			CommandDrawInstanced cmd;
			cmd.vertexCountPerInstance = vertexCountPerInstance;
			cmd.instanceCount = instanceCount;
			cmd.startVertexLocation = startVertexLocation;
			cmd.startInstanceLocation = startInstanceLocation;
			queue(cmd);
		}

		void drawIndexedInstanced(uint32_t indexCountPerInstance, uint32_t instanceCount, uint32_t startIndexLocation, int32_t baseVertexLocation, uint32_t startInstanceLocation) {
			CommandDrawIndexedInstanced cmd;
			cmd.indexCountPerInstance = indexCountPerInstance;
			cmd.instanceCount = instanceCount;
			cmd.startIndexLocation = startIndexLocation;
			cmd.baseVertexLocation = baseVertexLocation;
			cmd.startInstanceLocation = startInstanceLocation;
			queue(cmd);
		}

		void setGraphicsRoot32BitConstant(uint32_t rootParameterIndex, uint32_t srcData, uint32_t destOffsetIn32BitValues) {
			CommandSetGraphicsRoot32BitConstant cmd;
			cmd.rootParameterIndex = rootParameterIndex;
			cmd.srcData = srcData;
			cmd.destOffsetIn32BitValues = destOffsetIn32BitValues;
			queue(cmd);
		}

		void setViewport(float topLeftX, float topLeftY, float width, float height, float minDepth, float maxDepth) {
			CommandSetViewport cmd;
			cmd.topLeftX = topLeftX;
			cmd.topLeftY = topLeftY;
			cmd.width = width;
			cmd.height = height;
			cmd.minDepth = minDepth;
			cmd.maxDepth = maxDepth;
			queue(cmd);
		}

		void setScissorRect(int left, int top, int right, int bottom) {
			CommandSetScissorRect cmd;
			cmd.left = left;
			cmd.top = top;
			cmd.right = right;
			cmd.bottom = bottom;
			queue(cmd);
		}
	};

	// Abstract interface for the implementation.
	virtual bool initialize() = 0;
	virtual bool executeCommandQueue(const CommandQueue &commandQueue) = 0;

	// Comparison interface that relies on virtual callbacks per command.
	virtual void drawInstancedVirtual(uint32_t vertexCountPerInstance, uint32_t instanceCount, uint32_t startVertexLocation, uint32_t startInstanceLocation) = 0;
	virtual void drawIndexedInstancedVirtual(uint32_t indexCountPerInstance, uint32_t instanceCount, uint32_t startIndexLocation, int32_t baseVertexLocation, uint32_t startInstanceLocation) = 0;
	virtual void setGraphicsRoot32BitConstantVirtual(uint32_t rootParameterIndex, uint32_t srcData, uint32_t destOffsetIn32BitValues) = 0;
	virtual void setViewportVirtual(float topLeftX, float topLeftY, float width, float height, float minDepth, float maxDepth) = 0;
	virtual void setScissorRectVirtual(int left, int top, int right, int bottom) = 0;
};