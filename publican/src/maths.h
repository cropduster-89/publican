#pragma once
#ifndef PUB_MATH_H
#define PUB_MATH_H

#include <stdint.h>

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

inline uintptr_t AlignPow2(size_t value, uint32_t alignment)
{
	return (value + ((alignment) - 1)) & ~((value - value) + (alignment) - 1);
}

#define Align4(Value) ((Value + 3) & ~3)
#define Align8(Value) ((Value + 7) & ~7)
#define Align16(Value) ((Value + 15) & ~15)

union vec2 {
	struct {
		float x;
		float y;
	};
	float all[2];
};

union vec3 {
	struct {
		float x, y, z;
	};
	struct {
		float u, v, w;
	};
	struct {
		float r, g ,b;
	};
	struct {
		union vec2 xy;
		float ignored0;
	};
	struct {
		float ignored1;
		union vec2 yz;
	};
	float all[3];
};

union vec4 {
	struct {
		union {
			union vec3 xyz;
			struct {
				float x, y, z;
			};
		};
		float w;
	};
	struct {
		union {
			union vec3 rgb;
			struct {
				float r, g, b;
			};
		};
		float a;
	};
	struct {
		union vec2 xy;
		float ignored0;
		float ignored1;
	};
	struct {
		float ignored2;
		union vec2 yz;
		float ignored3;
	};
	struct {
		float ignored4;
		float ignored5;
		union vec2 zw;
	};
	float all[4];
};

inline union vec2 RealToVec2(float a,
							 float b)
{
	union vec2 result = {
		.x = a,
		.y = b
	};

	return result;
}

inline union vec3 RealToVec3(float a,
							 float b,
							 float c)
{
	union vec3 result = {
		.x = a,
		.y = b,
		.z = c
	};

	return result;
}

inline union vec4 RealToVec4(float a,
							 float b,
							 float c,
							 float d)
{
	union vec4 result = {
		.x = a,
		.y = b,
		.z = c,
		.w = d
	};

	return result;
}

#define GET_FLOAT_TO_VEC(_1,_2,_3,_4,name,...) name
#define VEC(...) GET_FLOAT_TO_VEC(__VA_ARGS__, \
RealToVec4, RealToVec3, RealToVec2)(__VA_ARGS__)

inline union vec2 AddVec2(union vec2 a,
						  union vec2 b)
{
	union vec2 result = {
		.x = a.x + b.x,
		.y = a.y + b.y
	};

	return result;
}

inline union vec3 AddVec3(union vec3 a,
						  union vec3 b)
{
	union vec3 result = {
		.x = a.x + b.x,
		.y = a.y + b.y,
		.z = a.z + b.z
	};

	return result;
}

inline union vec4 AddVec4(union vec4 a,
						  union vec4 b)
{
	union vec4 result = {
		.x = a.x + b.x,
		.y = a.y + b.y,
		.z = a.z + b.z,
		.w = a.w + b.w
	};

	return result;
}

#define ADD_VEC(a, b) _Generic((a), \
union vec2: AddVec2, \
union vec3: AddVec3, \
union vec4: AddVec4) ((a), (b))

inline union vec2 SubVec2(union vec2 a,
						  union vec2 b)
{
	union vec2 result = {
		.x = a.x - b.x,
		.y = a.y - b.y
	};

	return result;
}

inline union vec3 SubVec3(union vec3 a,
						  union vec3 b)
{
	union vec3 result = {
		.x = a.x - b.x,
		.y = a.y - b.y,
		.z = a.z - b.z
	};

	return result;
}

inline union vec4 SubVec4(union vec4 a,
						  union vec4 b)
{
	union vec4 result = {
		.x = a.x - b.x,
		.y = a.y - b.y,
		.z = a.z - b.z,
		.w = a.w - b.w
	};

	return result;
}

#define SUB_VEC(a, b) _Generic((a), \
union vec2: SubVec2, \
union vec3: SubVec3, \
union vec4: SubVec4) ((a), (b))

inline union vec2 MultVec2(union vec2 a,
						   float b)
{
	union vec2 result = {
		.x = a.x * b,
		.y = a.y * b
	};

	return result;
}

inline union vec3 MultVec3(union vec3 a,
						   float b)
{
	union vec3 result = {
		.x = a.x * b,
		.y = a.y * b,
		.z = a.z * b
	};

	return result;
}

inline union vec4 MultVec4(union vec4 a,
						   float b)
{
	union vec4 result = {
		.x = a.x * b,
		.y = a.y * b,
		.z = a.z * b,
		.w = a.a * b
	};

	return result;
}

#define MULT_VEC(a, b) _Generic((a), \
union vec2: MultVec2, \
union vec3: MultVec3, \
union vec4: MultVec4) ((a), (b))

#endif