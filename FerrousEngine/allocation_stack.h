#include "allocation.h"

class StackAllocator : public FerrousAllocator {
public:
	StackAllocator(FerrousAllocator* parent, size_t num_bytes);
	~StackAllocator();

	/* Allocates the specified amount of memory.*/
	void* alloc(size_t num_bytes, uint8_t alignment) override;

	/* De-allocate all allocated memory in the stack to the specified pointer.*/
	void revertTo(void* p);

	void dealloc(void* p) override;

	/* Resets the stack back to it's starting address.*/
	void reset(void);

	void ref(void* p) override;

	void deref(void* p) override;

	size_t getCapacity() override;
private:
	FerrousAllocator* _parent;
	void* _mem;
	void* _pos;
	size_t _capacity;
};