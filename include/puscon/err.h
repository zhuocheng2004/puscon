#ifndef PUSCON_ERR_H
#define PUSCON_ERR_H

#include <errno.h>

#include <puscon/types.h>

#define MAX_ERRNO	4095

/**
 * IS_ERR_VALUE - Detect an error pointer.
 * @x: The pointer to check.
 *
 * Like IS_ERR(), but does not generate a compiler warning if result is unused.
 */
#define IS_ERR_VALUE(x) ((unsigned long)(void *)(x) >= (unsigned long)-MAX_ERRNO)


/**
 * ERR_PTR - Create an error pointer.
 * @error: A negative error code.
 *
 * Encodes @error into a pointer value. Users should consider the result
 * opaque and not assume anything about how the error is encoded.
 *
 * Return: A pointer with @error encoded within its value.
 */
static inline void* ERR_PTR(long error)
{
	return (void*) error;
}

/**
 * PTR_ERR - Extract the error code from an error pointer.
 * @ptr: An error pointer.
 * Return: The error code within @ptr.
 */
static inline long PTR_ERR(const void* ptr)
{
	return (long) ptr;
}

/**
 * IS_ERR - Detect an error pointer.
 * @ptr: The pointer to check.
 * Return: true if @ptr is an error pointer, false otherwise.
 */
static inline bool IS_ERR(const void* ptr)
{
	return IS_ERR_VALUE((unsigned long) ptr);
}

#endif
