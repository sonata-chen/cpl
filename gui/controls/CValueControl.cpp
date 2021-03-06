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

	file:CValueControl.cpp

		Source code for CValueControl.h

*************************************************************************************/

#include "CValueControl.h"
#include "../../Mathext.h"

namespace cpl
{
	/*********************************************************************************************

		CValueControl - constructor

	*********************************************************************************************/
	CValueControl::CValueControl(std::string name, const std::string_view inputValues, std::string unit)
		: unit(std::move(unit))
	{
		bSetTitle(std::move(name));
		setValues(inputValues);

	}

	CValueControl::CValueControl(std::string name, std::vector<std::string> inputValues, std::string unit)
		: unit(std::move(unit)), values(std::move(inputValues))
	{
		bSetTitle(std::move(name));
	}

	/*********************************************************************************************

		CValueControl - constructor

	*********************************************************************************************/
	CValueControl::CValueControl()
	{

	}
	/*********************************************************************************************/
	void CValueControl::setValues(const std::string_view inputValues)
	{
		values.clear();
		std::size_t iter_pos(0);
		auto len = inputValues.size();
		for (std::size_t i = 0; i < len; i++)
		{
			if (inputValues[i] == '|')
			{
				values.emplace_back(inputValues.substr(iter_pos, i));
				iter_pos = i + 1;
			}
			else if (i == (len - 1))
			{
				values.emplace_back(inputValues.substr(iter_pos, i + 1));

			}
		}
	}
	/*********************************************************************************************/
	void CValueControl::setValues(std::vector<std::string> inputValues)
	{
		values = std::move(inputValues);
	}
	/*********************************************************************************************/
	void CValueControl::setUnit(std::string newUnit)
	{
		unit = std::move(newUnit);
	}
	/*********************************************************************************************/
	bool CValueControl::bValueToString(std::string & valueString, iCtrlPrec_t val) const
	{
		std::size_t idx = Math::round<std::size_t>(Math::confineTo<iCtrlPrec_t>(val, 0.0, 1.0) * (values.size() ? values.size() - 1 : 0));
		valueString = values[idx] + " " + unit;
		return true;
	}

	/*********************************************************************************************/
	bool CValueControl::bStringToValue(const string_ref valueString, iCtrlPrec_t & val) const
	{
		for (std::size_t i = 0; i < values.size(); ++i)
		{
			if (values[i] == valueString)
			{
				// can never divide by zero here, since the for loop won't enter in that case
				val = iCtrlPrec_t(i) / (values.size() > 1 ? values.size() - 1 : 1);
				return true;
			}
		}
		return false;
	}
};
