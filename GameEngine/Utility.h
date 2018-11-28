#pragma once

#include <iostream>
#include <stdarg.h>
#include <algorithm>
#include <string>
#include <sstream>
#include <DirectXMath.h>

#include "dxerr.h"

using namespace DirectX;

/*
Utility is for all the somewhat miscellaneous and low level stuff
that we may need access to globally
*/

//Release a DirectX object 
#ifndef SafeRelease
	#define SafeRelease(x) { if(x){ x->Release(); x = 0;} }
#endif

#ifndef SafeReleaseMap
	#define SafeReleaseMap(x) {	for (auto it = x.begin(); it != x.end(); ++it) { if(it->second) {it->second->Release(); it->second = 0;}}}
#endif

//Safely delete a dynamically allocated object (created with 'new')
#ifndef SafeDelete
	#define SafeDelete(x) { if(x != nullptr) { delete x; x = nullptr; } }
#endif

#ifndef SafeDeleteMap
	#define SafeDeleteMap(x) {	for (auto it = x.begin(); it != x.end(); ++it) { if(it->second) {delete it->second; it->second = nullptr;}}}
#endif

#ifndef LERP
	#define LERP(a, b, t) (1 - t)*a + t*b
#endif

#ifndef CLAMP
	#define CLAMP(a,b,v) max(a, min(v,b))
#endif

#if defined(_DEBUG)
	#define _CRTDBG_MAP_ALLOC//Track memory leaks and stuff
	#include <crtdbg.h> 

	//Get human readable error from a DirectX function
	#ifndef HR
	#define HR(x)														\
		{																\
			{															\
				HRESULT hr = (x);										\
				if (FAILED(hr))											\
				{														\
					DXTrace(__FILEW__, (DWORD)__LINE__, hr, L#x, true);	\
					PostQuitMessage(0);									\
				}														\
			}															\
		}
	#endif
#else
	#ifndef HR
	#define HR(x)(x)
	#endif 
#endif 

/*
Utility class for miscellaneous functions we need to work with DirectX
*/
class Utility
{
public:
	static XMFLOAT4 RandomColor(bool randomAlpha = false)
	{
		return XMFLOAT4((float)rand() / RAND_MAX,
			(float)rand() / RAND_MAX,
			(float)rand() / RAND_MAX,
			randomAlpha ? (float)rand() / RAND_MAX : 1);
	}
	static XMFLOAT3 RandomPosition(float radius)
	{
		return XMFLOAT3((float)rand() / RAND_MAX * radius - (radius / 2.0f),
			(float)rand() / RAND_MAX * radius - (radius / 2.0f),
			(float)rand() / RAND_MAX * radius - (radius / 2.0f));
	}

	//Returns a random value between 0 and 1
	static float RandomFloat()
	{
		return rand() / (float)RAND_MAX;
	}

	//Return a a value that is the next multiple of 16 from val
	inline static int SizeAlignedTo16(int val)
	{
		return 16 * ((val - 1) / 16 + 1);
	}

	//Returns a number between 0 and max going back and forth based on x, slowing near 0 and max
	inline static float PingPongSpherical(float x, float max)
	{
		return (cos(XM_PI * x / max) + 1) / 2.0f;
	}

	//Returns a number between 0 and max going back and forth based on x
	inline static float PingPongLinear(float x, float max)
	{
		float div = x / max;
		int divint = (int)div;
		float dx = div - divint;
		return divint % 2 == 0 ? max * dx : max * (1 - dx);
	}

	//Converts a matrix to a wstring for debugging
	inline static std::wstring MatrixToWString(const XMMATRIX &m)
	{
		std::wstringstream stream;
		stream << "(" << m.r[0].m128_f32[0] << L", " << m.r[0].m128_f32[1] << L", " << m.r[0].m128_f32[2] << L", " << m.r[0].m128_f32[3] << L" | "
			<< m.r[1].m128_f32[0] << L", " << m.r[1].m128_f32[1] << L", " << m.r[1].m128_f32[2] << L", " << m.r[1].m128_f32[3] << L" | "
			<< m.r[2].m128_f32[0] << L", " << m.r[2].m128_f32[1] << L", " << m.r[2].m128_f32[2] << L", " << m.r[2].m128_f32[3] << L" | "
			<< m.r[3].m128_f32[0] << L", " << m.r[3].m128_f32[1] << L", " << m.r[3].m128_f32[2] << L", " << m.r[3].m128_f32[3] << L")";

		return stream.str();
	}

	//Converts a matrix to a string for debugging
	inline static std::string MatrixToString(const XMMATRIX &m)
	{
		std::ostringstream stream;
		stream << "(" << m.r[0].m128_f32[0] << ", " << m.r[0].m128_f32[1] << ", " << m.r[0].m128_f32[2] << ", " << m.r[0].m128_f32[3] << " | "
			<< m.r[1].m128_f32[0] << ", " << m.r[1].m128_f32[1] << ", " << m.r[1].m128_f32[2] << ", " << m.r[1].m128_f32[3] << " | "
			<< m.r[2].m128_f32[0] << ", " << m.r[2].m128_f32[1] << ", " << m.r[2].m128_f32[2] << ", " << m.r[2].m128_f32[3] << " | "
			<< m.r[3].m128_f32[0] << ", " << m.r[3].m128_f32[1] << ", " << m.r[3].m128_f32[2] << ", " << m.r[3].m128_f32[3] << ")";

		return stream.str();
	}

	//Converts a vector to a wstring for debugging
	inline static std::wstring VectorToWString(const XMVECTOR &v)
	{
		std::wstringstream stream;
		stream << L"(" << v.m128_f32[0] << L", " << v.m128_f32[1] << L", " << v.m128_f32[2] << L", " << v.m128_f32[3] << L")";
		return stream.str();
	}

	//Converts a vector to a string for debugging
	inline static std::string VectorToString(const XMVECTOR &v)
	{
		std::stringstream stream;
		stream << "(" << v.m128_f32[0] << ", " << v.m128_f32[1] << ", " << v.m128_f32[2] << ", " << v.m128_f32[3] << ")";
		return stream.str();
	}

	//determines if one float is ALMOST equal to another (remember .00000001 is not == 0)
	inline static bool Near(float v, float g, float e = .0001f)//FLT_EPSILON)
	{
		return fabs(g - v) <= e;
	}

	//Provides a smooth curved interpolation instead of a linear interpolation
	inline static XMVECTOR SmoothStep(XMVECTOR V0, XMVECTOR V1, float t)//From MSDN
	{
		t = (t > 1.0f) ? 1.0f : ((t < 0.0f) ? 0.0f : t);  // Clamp value to 0 to 1
		t = t * t*(3.f - 2.f*t);
		return XMVectorLerp(V0, V1, t);
	}

	//Linear interpolation
	inline static float Lerp(float v1, float v2, float t)
	{
		return v1 + (v2 - v1) * t;
	}

	//Provides a smooth curved interpolation instead of a linear interpolation
	inline static float SmoothStep(float v1, float v2, float t)//From MSDN
	{
		t = (t > 1.0f) ? 1.0f : ((t < 0.0f) ? 0.0f : t);  // Clamp value to 0 to 1
		t = t * t*(3.f - 2.f*t);
		return Lerp(v1, v2, t);
	}

	//Quick function to subtract two XMFLOAT3's
	inline static XMFLOAT3 Subtract(XMFLOAT3 x, XMFLOAT3 y)
	{
		XMFLOAT3 temp;
		XMStoreFloat3(&temp, XMLoadFloat3(&x) - XMLoadFloat3(&y));
		return temp;
	}

	//Quick function to subtract two XMFLOAT2's
	inline static XMFLOAT2 Subtract(XMFLOAT2 x, XMFLOAT2 y)
	{
		XMFLOAT2 temp;
		XMStoreFloat2(&temp, XMLoadFloat2(&x) - XMLoadFloat2(&y));
		return temp;
	}

	//Quick function to Add two XMFLOAT3's
	inline static XMFLOAT3 Add(XMFLOAT3 x, XMFLOAT3 y)
	{
		XMFLOAT3 temp;
		XMStoreFloat3(&temp, XMLoadFloat3(&x) + XMLoadFloat3(&y));
		return temp;
	}
	
	//Quick function to Add two XMFLOAT2's
	inline static XMFLOAT2 Add(XMFLOAT2 x, XMFLOAT2 y)
	{
		XMFLOAT2 temp;
		XMStoreFloat2(&temp, XMLoadFloat2(&x) + XMLoadFloat2(&y));
		return temp;
	}
};