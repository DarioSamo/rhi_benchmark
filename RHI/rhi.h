//
// RHI
//

#pragma once

#include <vector>

struct RHI {
	enum class CommandType : uint32_t {
		Unknown = 0,
		DrawIndexed,
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

	struct CommandQueue {
		std::vector<uint8_t> commandBytes;

		template<typename T>
		inline void queue(const T &command) {
			const uint8_t *commandPtr = reinterpret_cast<const uint8_t *>(&command);
			commandBytes.insert(commandBytes.end(), commandPtr, commandPtr + sizeof(T));
		}

		inline CommandType peek(size_t &cursor) const {
			if (cursor >= commandBytes.size()) {
				return CommandType::End;
			}
			else {
				return *reinterpret_cast<const CommandType *>(commandBytes.data() + cursor);
			}
		}

		template<typename T>
		inline const T &read(size_t &cursor) const {
			const T *commandPtr = reinterpret_cast<const T *>(commandBytes.data() + cursor);
			cursor += sizeof(T);
			return *commandPtr;
		}

		void clear() {
			commandBytes.clear();
		}

		inline void drawIndexedInstanced(uint32_t indexCountPerInstance, uint32_t instanceCount, uint32_t startIndexLocation, int32_t baseVertexLocation, uint32_t startInstanceLocation) {
			CommandDrawIndexed cmd;
			cmd.indexCountPerInstance = indexCountPerInstance;
			cmd.instanceCount = instanceCount;
			cmd.startIndexLocation = startIndexLocation;
			cmd.baseVertexLocation = baseVertexLocation;
			cmd.startInstanceLocation = startInstanceLocation;
			queue(cmd);
		}
	};

	// Abstract interface for the implementation.
	virtual bool initialize() = 0;
	virtual bool executeCommandQueue(const CommandQueue &commandQueue) = 0;

	// Comparison interface that relies on virtual callbacks per command.
	virtual void drawIndexedInstancedVirtual(uint32_t indexCountPerInstance, uint32_t instanceCount, uint32_t startIndexLocation, int32_t baseVertexLocation, uint32_t startInstanceLocation) = 0;
};