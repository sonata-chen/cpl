/*************************************************************************************
 
	 Audio Programming Environment - Audio Plugin - v. 0.3.0.
	 
	 Copyright (C) 2014 Janus Lynggaard Thorborg [LightBridge Studios]
	 
	 This program is free software: you can redistribute it and/or modify
	 it under the terms of the GNU General Public License as published by
	 the Free Software Foundation, either version 3 of the License, or
	 (at your option) any later version.
	 
	 This program is distributed in the hope that it will be useful,
	 but WITHOUT ANY WARRANTY; without even the implied warranty of
	 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	 GNU General Public License for more details.
	 
	 You should have received a copy of the GNU General Public License
	 along with this program.  If not, see <http://www.gnu.org/licenses/>.
	 
	 See \licenses\ for additional details on licenses associated with this program.
 
 **************************************************************************************

	file:Common.h
	
		Implements common constants, types & macroes used throughout the program.
		Compilier specific tunings.
		Also #includes commonly used headers.

*************************************************************************************/

#ifndef _TYPES_H
	#define _TYPES_H

	#include <type_traits>
	#include <cstdint>
	#include "PlatformSpecific.h"
	#include <emmintrin.h>
	#include <immintrin.h>
	#include <xmmintrin.h>
	#include <emmintrin.h>

	namespace cpl
	{
		namespace Types
		{
			// the fastest possible integer of at least 32 bits
			typedef std::uint_fast32_t fint_t;
			// the fastest possible signed integer of at least 32 bits 
			typedef std::int_fast32_t fsint_t;
			// the fastest possible unsigned integer of at least 32 bits
			typedef std::uint_fast32_t fuint_t;
			// sse-vector of 4 floats
			typedef __m128 v4sf;
			// avx-vector of 8 floats
			typedef __m256 v8sf;
			// sse-vector of 2 doubles
			typedef __m128d v2sd;
			// avx-vector of 4 doubles
			typedef __m256d v4sd;

			// sse-vector of 16/8/4/2 ints
			typedef __m128i v128si;
			// sse-vector of 32/16/8/4 ints
			typedef __m256i v256si;

			#ifdef __WINDOWS__
				typedef DWORD OSError;
			#else
				typedef decltype(errno) OSError;
			#endif

			#ifdef __UNICODE__
				typedef std::wstring tstring;
				typedef wchar_t char_t;
			#else
				typedef char char_t;
				typedef std::string tstring;
			#endif

			template<typename T>
			struct mul_promotion
			{
				typedef T type;
			};

			template<> struct mul_promotion < std::uint8_t > { typedef std::uint16_t type; };
			template<> struct mul_promotion < std::uint16_t > { typedef std::uint32_t type; };
			template<> struct mul_promotion < std::uint32_t > { typedef std::uint64_t type; };

			template<> struct mul_promotion < std::int8_t > { typedef std::int16_t type; };
			template<> struct mul_promotion < std::int16_t > { typedef std::int32_t type; };
			template<> struct mul_promotion < std::int32_t > { typedef std::int64_t type; };
		};
	};
#endif