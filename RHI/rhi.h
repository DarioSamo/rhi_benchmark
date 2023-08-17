//
// RHI
//

#pragma once

#include <vector>

struct RHI {
	enum class CommandType : uint32_t {
		Unknown = 0,
		DrawIndexed,
		SetGraphicsRoot32BitConstant,
		End = 0xFFFFFFFF
	};

	struct CommandDrawIndexed {
		CommandType commandType = CommandType::DrawIndexed;
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

		void drawIndexedInstanced(uint32_t indexCountPerInstance, uint32_t instanceCount, uint32_t startIndexLocation, int32_t baseVertexLocation, uint32_t startInstanceLocation) {
			CommandDrawIndexed cmd;
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
	};

	// Abstract interface for the implementation.
	virtual bool initialize() = 0;
	virtual bool executeCommandQueue(const CommandQueue &commandQueue) = 0;

	// Comparison interface that relies on virtual callbacks per command.
	virtual void drawIndexedInstancedVirtual(uint32_t indexCountPerInstance, uint32_t instanceCount, uint32_t startIndexLocation, int32_t baseVertexLocation, uint32_t startInstanceLocation) = 0;
	virtual void setGraphicsRoot32BitConstantVirtual(uint32_t rootParameterIndex, uint32_t srcData, uint32_t destOffsetIn32BitValues) = 0;
};