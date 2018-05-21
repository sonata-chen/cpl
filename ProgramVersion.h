/*************************************************************************************

	cpl - cross-platform library - v. 0.1.0.

	Copyright (C) 2016 Janus Lynggaard Thorborg (www.jthorborg.com)

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

	file:ProgramVersion.h

		Utility class for managing versions

*************************************************************************************/

#ifndef CPL_PROGRAMVERSION_H
#define CPL_PROGRAMVERSION_H

#ifdef CPL_BIGENDIAN
#error Version routine does not yet support big-endianness
#endif

#include <cstdint>
#include <cstdio>
#include <string>
#include <algorithm>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4996)
#endif

namespace cpl
{
	union Version
	{
	public:

		typedef std::uint64_t BinaryStorage;

		Version()
			: compiled(0)
		{

		}

		explicit Version(BinaryStorage version)
			: compiled(version)
		{

		}

		Version(std::uint16_t major, std::uint16_t minor, std::uint32_t build)
		{
			*this = fromParts(major, minor, build);
		}

		static Version fromParts(std::uint16_t major, std::uint16_t minor, std::uint32_t build)
		{
			Version ret;
			ret.parts.major = major;
			ret.parts.minor = minor;
			ret.parts.build = build;
			return ret;
		}

		static Version fromString(const std::string & version)
		{
			Version ret;
			int parts[3];

			std::sscanf(version.c_str(), "%d.%d.%d", &parts[0], &parts[1], &parts[2]);

			ret.parts.major = (std::uint16_t) std::clamp<int>(parts[0], 0, std::numeric_limits<std::uint16_t>::max());
			ret.parts.minor = (std::uint16_t) std::clamp<int>(parts[1], 0, std::numeric_limits<std::uint16_t>::max());
			ret.parts.build = (std::uint32_t) std::clamp<int>(parts[2], 0, std::numeric_limits<std::uint32_t>::max());

			return ret;
		}

		std::string toString() const
		{
			return std::to_string(parts.major) + "." + std::to_string(parts.minor) + "." + std::to_string(parts.build);
		}

		bool operator < (const Version & other) const
		{
			// yes, endianness fucks us all.
			if (parts.major < other.parts.major)
				return true;
			else if (parts.major == other.parts.major)
			{
				if (parts.minor < other.parts.minor)
					return true;
				else if (parts.minor == other.parts.minor)
				{
					if (parts.build < other.parts.build)
						return true;
				}
			}



			return false;
		}

		bool operator > (const Version & other) const
		{
			return other < *this;
		}

		bool operator <= (const Version & other) const
		{
			return !(other < *this);
		}

		bool operator >= (const Version & other) const
		{
			return !(*this < other);
		}

		bool operator == (const Version & other) const
		{
			return compiled == other.compiled;
		}

		bool operator != (const Version & other) const
		{
			return compiled != other.compiled;
		}

		BinaryStorage compiled;
		struct parts_t
		{
			std::uint16_t major;
			std::uint16_t minor;
			std::uint32_t build;
		} parts;
	};
};

namespace std
{
	inline string to_string(const ::cpl::Version & p)
	{
		return p.toString();
	}
};


#ifdef _MSC_VER
#pragma warning(pop)
#endif
#endif