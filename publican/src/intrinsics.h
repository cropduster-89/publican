#pragma once
#ifndef PUB_INTRINSICS_H
#define PUB_INTRINSICS_H

#include <stdint.h>
#include <intrin.h>

#ifdef _WIN32
inline uint32_t AtomicCompareExchangeUInt32(int32_t volatile *val,
											uint32_t input,
											uint32_t expected)
{
	return _InterlockedCompareExchange((long *)val, input, expected);
}

inline uint64_t AtomicExchangeU64(uint64_t volatile *a,
								  uint64_t b)
{
	return _InterlockedExchange64((__int64 *)a, b);
}

inline uint32_t AtomicExchangeU32(uint32_t volatile *a,
								  uint32_t b)
{
	return _InterlockedExchange((long *)a, b);
}

inline uint64_t AtomicAddU64(uint64_t volatile *a,
							 uint64_t b)
{
	return _InterlockedExchangeAdd64((__int64 *)a, b);
}

inline void BeginTicketMutex(struct ticket_mutex *mutex)
{
	uint64_t ticket = AtomicAddU64(&mutex->ticket, 1);
	while(ticket != mutex->serving) {_mm_pause();}
}

inline void EndTicketMutex(struct ticket_mutex *mutex)
{
	AtomicAddU64(&mutex->serving, 1);
}
#endif

#endif