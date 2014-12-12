#pragma once

#include <malloc.h>

// Templated aligned allocator
// Based on https://gist.github.com/donny-dont/1471329
// and http://stackoverflow.com/questions/12942548/making-stdvector-allocate-aligned-memory

enum Alignment 
{
	Normal = sizeof(void*),
	SSE = 16,
	AVX = 32,
};

template <typename T, /*std::size_t Alignment*/ Alignment Align = Alignment::SSE>
class AlignedAllocator
{
public:
	// The following will be the same for virtually all allocators.
	typedef T*			pointer;
	typedef const T*	const_pointer;
	typedef T&			reference;
	typedef const T&	const_reference;
	typedef T			value_type;
	typedef size_t		size_type;
	typedef ptrdiff_t	difference_type;

	T * address(T& r) const
	{
		return &r;
	}

	const T * address(const T& s) const
	{
		return &s;
	}

	std::size_t max_size() const
	{
		// The following has been carefully written to be independent of
		// the definition of size_t and to avoid signed/unsigned warnings.
		return (static_cast<std::size_t>(~0) - static_cast<std::size_t>(Align)) / sizeof(T);
	}

	// The following must be the same for all allocators.
	template <typename U>
	struct rebind
	{
		typedef AlignedAllocator<U, Align> other;
	};

	bool operator!=(const AlignedAllocator& other) const
	{
		return !(*this == other);
	}

	void construct(T * const p, const T& t) const
	{
		void * const pv = static_cast<void *>(p);
		new (pv)T(t);
	}

	void destroy(T * const p) const
	{
		p->~T();
	}
	// Returns true if and only if storage allocated from *this
	// can be deallocated from other, and vice versa.
	// Always returns true for stateless allocators.
	bool operator==(const AlignedAllocator& other) const
	{
		return true;
	}
	// Default constructor, copy constructor, rebinding constructor, and destructor.
	// Empty for stateless allocators.
	AlignedAllocator() { }
	AlignedAllocator(const AlignedAllocator&) { }
	template <typename U> AlignedAllocator(const AlignedAllocator<U, Align>&) { }
	~AlignedAllocator() { }
	
	// The following will be different for each allocator.
	T * allocate(const std::size_t n) const
	{
		// The return value of allocate(0) is unspecified.
		// Mallocator returns NULL in order to avoid depending
		// on malloc(0)'s implementation-defined behavior
		// (the implementation can define malloc(0) to return NULL,
		// in which case the bad_alloc check below would fire).
		// All allocators can return NULL in this case.
		if (n == 0) 
		{
			return nullptr;
		}
		// All allocators should contain an integer overflow check.
		// The Standardization Committee recommends that std::length_error
		// be thrown in the case of integer overflow.
		if (n > max_size())
		{
			throw std::length_error("aligned_allocator<T>::allocate() - Integer overflow.");
		}
		// Mallocator wraps _mm_malloc().
		void * const pv = _mm_malloc(n * sizeof(T), Align);
		// Allocators should throw std::bad_alloc in the case of memory allocation failure.
		if (pv == nullptr)
		{
			throw std::bad_alloc();
		}
		return static_cast<T *>(pv);
	}
	void deallocate(T * const p, const std::size_t n) const
	{
		_mm_free(p);
	}

	// The following will be the same for all allocators that ignore hints.
	template <typename U>
	T * allocate(const std::size_t n, const U * /* const hint */) const
	{
		return allocate(n);
	}

	// Allocators are not required to be assignable, so
	// all allocators should have a private unimplemented
	// assignment operator. Note that this will trigger the
	// off-by-default (enabled under /Wall) warning C4626
	// "assignment operator could not be generated because a
	// base class assignment operator is inaccessible" within
	// the STL headers, but that warning is useless.
private:
	AlignedAllocator& operator=(const AlignedAllocator&);
};


/*
enum class Alignment : size_t
{
	Normal = sizeof(void*),
	SSE = 16,
	AVX = 32,
};


namespace detail 
{
	void*	allocate_aligned_memory(size_t align, size_t size);
	void	deallocate_aligned_memory(void* ptr) noexcept;
}


template <typename T, Alignment Align = Alignment::SSE>
class AlignedAllocator;


template <Alignment Align>
class AlignedAllocator<void, Align>
{
public:
	typedef void*             pointer;
	typedef const void*       const_pointer;
	typedef void              value_type;

	template <class U> struct rebind { typedef AlignedAllocator<U, Align> other; };
};


template <typename T, Alignment Align>
class AlignedAllocator
{
public:
	typedef T         value_type;
	typedef T*        pointer;
	typedef const T*  const_pointer;
	typedef T&        reference;
	typedef const T&  const_reference;
	typedef size_t    size_type;
	typedef ptrdiff_t difference_type;

	typedef std::true_type propagate_on_container_move_assignment;

	template <class U>
	struct rebind { typedef AlignedAllocator<U, Align> other; };

public:
	AlignedAllocator() noexcept
	{}

	template <class U>
	AlignedAllocator(const AlignedAllocator<U, Align>&) noexcept
	{}

	size_type max_size() const noexcept
	{ 
		return (size_type(~0) - size_type(Align)) / sizeof(T); 
	}

	pointer address(reference x) const noexcept
	{ 
		return std::addressof(x); 
	}

	const_pointer address(const_reference x) const noexcept
	{
		return std::addressof(x); 
	}

	pointer allocate(size_type n, typename AlignedAllocator<void, Align>::const_pointer = 0)
	{
		const size_type alignment = static_cast<size_type>(Align);
		void* ptr = detail::allocate_aligned_memory(alignment, n * sizeof(T));
		if (ptr == nullptr) {
			throw std::bad_alloc();
		}

		return reinterpret_cast<pointer>(ptr);
	}

	void deallocate(pointer p, size_type) noexcept
	{ 
		return detail::deallocate_aligned_memory(p); 
	}

	template <class U, class ...Args>
	void construct(U* p, Args&&... args)
	{
		::new(reinterpret_cast<void*>(p)) U(std::forward<Args>(args)...);
	}

	void destroy(pointer p)
	{
		p->~T();
	}
};


template <typename T, Alignment Align>
class AlignedAllocator<const T, Align>
{
public:
	typedef T         value_type;
	typedef const T*  pointer;
	typedef const T*  const_pointer;
	typedef const T&  reference;
	typedef const T&  const_reference;
	typedef size_t    size_type;
	typedef ptrdiff_t difference_type;

	typedef std::true_type propagate_on_container_move_assignment;

	template <class U>
	struct rebind { typedef AlignedAllocator<U, Align> other; };

public:
	AlignedAllocator() noexcept
	{}

	template <class U>
	AlignedAllocator(const AlignedAllocator<U, Align>&) noexcept
	{}

	size_type
	max_size() const noexcept
	{ return (size_type(~0) - size_type(Align)) / sizeof(T); }

	const_pointer
	address(const_reference x) const noexcept
	{ return std::addressof(x); }

	pointer
	allocate(size_type n, typename AlignedAllocator<void, Align>::const_pointer = 0)
	{
		const size_type alignment = static_cast<size_type>(Align);
		void* ptr = detail::allocate_aligned_memory(alignment, n * sizeof(T));
		if (ptr == nullptr) {
			throw std::bad_alloc();
		}

		return reinterpret_cast<pointer>(ptr);
	}

	void deallocate(pointer p, size_type) noexcept
	{ 
		return detail::deallocate_aligned_memory(p); 
	}

	template <class U, class ...Args>
	void construct(U* p, Args&&... args)
	{
		::new(reinterpret_cast<void*>(p)) U(std::forward<Args>(args)...);
	}

	void destroy(pointer p)
	{
		p->~T();
	}
};

template <typename T, Alignment TAlign, typename U, Alignment UAlign>
inline bool operator== (const AlignedAllocator<T, TAlign>&, const AlignedAllocator<U, UAlign>&) noexcept
{ 
	return TAlign == UAlign; 
}

template <typename T, Alignment TAlign, typename U, Alignment UAlign>
inline bool operator!= (const AlignedAllocator<T, TAlign>&, const AlignedAllocator<U, UAlign>&) noexcept
{ 
	return TAlign != UAlign; 
}*/