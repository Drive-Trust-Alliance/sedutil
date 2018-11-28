#pragma once
#include <vector>
#ifdef __linux__
#include <memory>
#include <sys/mman.h>

/**
  This allocator zeros the memory before deallocating
  The memory allocated is locked into RAM, preventing that memory from being paged to the swap area
  */
template <class T> class SecureAllocator : public std::allocator<T>
{
    public:
        template<class U> struct rebind { typedef SecureAllocator<U> other; };

        SecureAllocator() throw() {}
        SecureAllocator(const SecureAllocator&) throw() {}
        template <class U> SecureAllocator(const SecureAllocator<U>&) throw() {}

        typename std::allocator<T>::pointer allocate(typename std::allocator<T>::size_type n, std::allocator<void>::const_pointer hint=0)
        {
            typename std::allocator<T>::pointer p = std::allocator<T>::allocate(n, hint);
            mlock(p, n*sizeof(T));
            return p;
        }

        void deallocate(typename std::allocator<T>::pointer p, typename std::allocator<T>::size_type n)
        {
            std::fill_n((volatile char*)p, n*sizeof(T), 0);
            munlock(p, n*sizeof(T));
            std::allocator<T>::deallocate(p, n);
        }
};

typedef std::basic_string<char, std::char_traits<char>, SecureAllocator<char>> SecureString;
typedef std::vector<uint8_t, SecureAllocator<uint8_t>> SecureByteVector;
#else
#include <string>
typedef std::string SecureString;
typedef std::vector<uint8_t> SecureByteVector;
#endif //__linux__
