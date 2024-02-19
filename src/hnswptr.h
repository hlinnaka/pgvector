/*
 * HnswPtr* macros provides an abstraction over memory management
 *
 * Inspired by PostgreSQL relptr.h, but built on top of DSA allocator.
 */
#ifndef HNSWPTR_H
#define HNSWPTR_H

#define HnswPtrDeclare(type, ptrtype) \
	typedef union { type *ptr; dsa_pointer dsaptr; } ptrtype; \
	static inline ptrtype hnsw_##ptrtype##_from_voidptr(dsa_area *base, VoidPtr vp) { \
		ptrtype p; \
		if (base == NULL) \
			p.ptr = vp.ptr; \
		else \
			p.dsaptr = vp.dsaptr; \
		return p; \
	}

/* Pointer macros */
#define HnswPtrAccess(base, hp) \
	(AssertVariableIsOfTypeMacro(base, dsa_area *), \
	 ((base) == NULL ? (hp).ptr : ((hp).dsaptr == InvalidDsaPointer ? NULL : dsa_get_address((base), (hp).dsaptr))))
#define HnswPtrIsNull(base, hp) ((base) == NULL ? (hp).ptr == NULL : ((hp).dsaptr == InvalidDsaPointer))
#define HnswPtrEqual(base, hp1, hp2) ((base) == NULL ? (hp1).ptr == (hp2).ptr : (hp1).dsaptr == (hp2).dsaptr)
#define HnswPtrStoreNull(base, hp) do { \
		if ((base) == NULL) \
			(hp).ptr = NULL; \
		else \
			(hp).dsaptr = InvalidDsaPointer; \
} while(0)

#define HnswPtrPointer(hp) (hp).ptr
#define HnswPtrOffset(hp) (hp).dsaptr

/* VoidPtr is needed by the HnswAlloc macro, so declare it here */
HnswPtrDeclare(void, VoidPtr)

/* Allocator */

#define HnswAlloc(allocator, base, ptrtype, size) \
	hnsw_##ptrtype##_from_voidptr((base), (allocator)->alloc((size), (allocator)->state))

typedef struct HnswAllocator
{
	VoidPtr	  (*alloc) (Size size, void *state);
	void	   *state;
}			HnswAllocator;

/* LOCAL_ALLOC uses palloc() */
extern const HnswAllocator LOCAL_ALLOC;

#endif
