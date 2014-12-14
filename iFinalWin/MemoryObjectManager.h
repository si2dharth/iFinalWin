#ifndef MEM_MANAGER
#define MEM_MANAGER
#include <map>
#include <stack>
using namespace std;

class ManagedObject
{
public:
	ManagedObject() {
	};

	virtual ~ManagedObject() {
	};

	void* operator new(size_t sz);

	void operator delete(void *mem, size_t sz);
};

class MemoryManager : public ManagedObject
{
	static size_t TotalUsed;
	map<ManagedObject *, size_t> MemMap;
	static stack<MemoryManager*> memStack;

	void reg(ManagedObject *BlockPointer, size_t size)
	{

		MemMap[BlockPointer] = size;
		TotalUsed += size;
	}

	void unreg(ManagedObject *BlockPointer)
	{
		TotalUsed -= MemMap[BlockPointer];
		MemMap.erase(BlockPointer);
	}

	static MemoryManager *memoryManager;
public:


	MemoryManager(bool first = false)
	{
		if (!first) memStack.push(memoryManager);
		memoryManager = this;
	}

	~MemoryManager()
	{
		while (MemMap.size() > 0)
			delete MemMap.begin()->first;
		if (memStack.size() == 0) return;
		memoryManager = memStack.top();
		memStack.pop();

	}

	static void Register(ManagedObject *BlockPointer,size_t size)
	{
		memoryManager->reg(BlockPointer, size);
	}

	static void Unregister(ManagedObject *BlockPointer)
	{
		memoryManager->unreg(BlockPointer);
	}

	static size_t GetTotalUsedMemory() {
		return TotalUsed;
	}
};
#endif