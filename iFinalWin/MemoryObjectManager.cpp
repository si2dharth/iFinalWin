#include "MemoryObjectManager.h"

MemoryManager MemManager(true);
MemoryManager *MemoryManager::memoryManager = &MemManager;
stack<MemoryManager*> MemoryManager::memStack;
size_t MemoryManager::TotalUsed = 0;

void* ManagedObject::operator new(size_t sz)
{
	void* newObject = malloc(sz);
	MemoryManager::Register((ManagedObject*)newObject, sz);
	return newObject;
};

void ManagedObject::operator delete(void *mem, size_t sz)
{
	MemoryManager::Unregister((ManagedObject*)mem);
	free(mem);
}