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

	file:CBaseControl.h
		
		CBaseControl - base class for all implementable controls.
		all methods prefixed with 'b' to avoid future nameclashing.
		This class provides a common interface for all 'controls' to support
		normalized value-, string and position get/set.
		Also encapsulates listeners into a single one.
		Provides a cheap RAII mutex lock as well, as well as optional reference counting.
		Does not derive from the system's base class for graphics to avoid the diamond
		problem (classes deriving from this should set the base class in their constructor)

*************************************************************************************/

#ifndef _CFFT_H
	#define _CFFT_H

		
		
		
		
		
		
		
		
		
		
		
		
		
		
#endif