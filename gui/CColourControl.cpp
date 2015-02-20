/*************************************************************************************
 
 cpl - cross-platform library - v. 0.1.0.
 
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
 
 file:ComponentContainers.cpp
 
	Source code for componentcontainers.h
 
 *************************************************************************************/

#include "CColourControl.h"
#include "../CCtrlEditSpace.h"

namespace cpl
{
	
	const char * argbTypes[] = 
	{
		"RGB",
		"ARGB",
		"GreyTone",
		"Red",
		"Green", 
		"Blue"

	};

	class ColourEditor 
	: 
		public CKnobSliderEditor
	{
	public:
		ColourEditor(CColourControl * parentControl)
			: CKnobSliderEditor(parentControl), parent(parentControl), selector(15, 5, 5), recursionFlagWeChanged(false), recursionFlagTheyChanged(false)
		{
			oldHeight = fullHeight;
			oldWidth = fullWidth;
			fullWidth = oldWidth + extraWidth;
			fullHeight = oldHeight + extraHeight;

			addAndMakeVisible(argbSelector);
			selector.addChangeListener(this);
			selector.setCurrentColour(juce::Colour(parent->getColour()));
			toolTip = "Colour editor space - adjust ARGB values of controls precisely.";

			juce::StringArray choices;
			for (const char * str : argbTypes)
			{
				choices.add(str);
			}
			argbSelector.addItemList(choices, 1);
			auto type = parent->getType();
			if (type != parent->SingleChannel)
			{
				argbSelector.setSelectedId(type + 1, juce::dontSendNotification);
			}
			else
			{
				argbSelector.setSelectedId(type + 1 + parent->getChannel(), juce::dontSendNotification);
			}
			argbSelector.addListener(this);
			setOpaque(false);
			//addAndMakeVisible(selector);
		}

		virtual void resized() override
		{
			argbSelector.setBounds(1, oldHeight, fullWidth - elementHeight - 3, elementHeight);
			auto bounds = argbSelector.getBounds();
			selector.setBounds(1, bounds.getBottom(), fullWidth - elementHeight - 3, extraHeight - bounds.getHeight());
			CKnobSliderEditor::resized();
		}

		virtual juce::String bGetToolTipForChild(const juce::Component * child) const override
		{
			if (child == &argbSelector || argbSelector.isParentOf(child))
			{
				return "Set which components of the colour the control adjusts.";
			}
			return CKnobSliderEditor::bGetToolTipForChild(child);
		}
		virtual void comboBoxChanged(juce::ComboBox * boxThatChanged) override
		{
			if (boxThatChanged == &argbSelector)
			{
				auto id = argbSelector.getSelectedId() - 1;
				if (id > 2)
				{
					id -= 2;
					parent->setChannel(id - 1);
					parent->setType(parent->SingleChannel);
					
				}
				else
					parent->setType((CColourControl::ColourType)(id));

				animateSucces(&argbSelector);
			}
			CKnobSliderEditor::comboBoxChanged(boxThatChanged);
		}
		virtual void changeListenerCallback(ChangeBroadcaster *source) override
		{
			if (recursionFlagTheyChanged || recursionFlagWeChanged)
			{
				recursionFlagWeChanged = recursionFlagTheyChanged = false;
			}
			else if (source == &selector)
			{
				recursionFlagWeChanged = true;
				parent->setColour(selector.getCurrentColour().getARGB());
			}
			CKnobSliderEditor::changeListenerCallback(source);
		}
		virtual void valueChanged(const CBaseControl * ctrl) override
		{
			if (recursionFlagTheyChanged || recursionFlagWeChanged)
			{
				recursionFlagWeChanged = recursionFlagTheyChanged = false;
			}
			else if (!recursionFlagWeChanged)
			{
				recursionFlagTheyChanged = true;
				selector.setCurrentColour(juce::Colour(parent->getColour()));
			}
			CKnobSliderEditor::valueChanged(ctrl);
		}
		virtual void setMode(bool newMode)
		{
			if (!newMode)
			{
				addAndMakeVisible(&selector);
			}
			else
			{
				removeChildComponent(&selector);
			}
			CKnobSliderEditor::setMode(newMode);
		}
	private:
		static const int extraHeight = 180;
		static const int extraWidth = 30;
		bool recursionFlagWeChanged;
		bool recursionFlagTheyChanged;
		int oldHeight, oldWidth;
		CColourControl * parent;
		juce::ColourSelector selector;
		juce::ComboBox argbSelector;
	};

	std::unique_ptr<CCtrlEditSpace> CColourControl::bCreateEditSpace()
	{
		if (isEditSpacesAllowed)
			return std::unique_ptr<CCtrlEditSpace>(new ColourEditor(this));
		else
			return nullptr;
	}


	union ARGBPixel
	{
		struct
		{
			// components
			//std::uint8_t a, r, g, b; <- big endian
			std::uint8_t b, g, r, a;
		} c;
		// data access
		std::uint8_t d[4];
		// complete pixel
		std::uint32_t p;
		ARGBPixel() : p(0) {}
		ARGBPixel(std::uint32_t pixel) : p(pixel) {}
		ARGBPixel(std::uint8_t red, std::uint8_t green, std::uint8_t blue) : c{ red, green, blue, 0} {}
		ARGBPixel(std::uint8_t alpha, std::uint8_t red, std::uint8_t green, std::uint8_t blue) : c{ red, green, blue, alpha} {}
	};

	/*********************************************************************************************

	CKnobEx

	*********************************************************************************************/
	CColourControl::CColourControl(const std::string & name, ColourType typeToUse, int channel)
		: CKnobSlider(name), colourType(typeToUse), colour(ARGBPixel(0xFF, 0, 0, 0).p), channel(channel % 3)
	{
		//bForceEvent();
		isEditSpacesAllowed = true;
		setVisible(true);
		onValueChange();
	}
	/*********************************************************************************************/

	void CColourControl::save(CSerializer::Archiver & ar, long long int version)
	{
		CKnobSlider::save(ar, version);
		ar << getColour();
		ar << getType();
	}
	void CColourControl::load(CSerializer::Builder & ar, long long int version)
	{
		CKnobSlider::load(ar, version);
		decltype(getColour()) newColour(0);
		decltype(getType()) newType;
		ar >> newColour;
		ar >> newType;
		setType(newType);
		setColour(newColour);
	}

	void CColourControl::onValueChange()
	{
		colour = floatToInt(bGetValue());
	}
	void CColourControl::setType(ColourType typeToUse)
	{
		colourType = typeToUse;
		bSetValue(intToFloat(colour));
	}
	CColourControl::ColourType CColourControl::getType() const
	{
		return colourType;
	}
	void CColourControl::setChannel(int newChannel)
	{
		channel = newChannel % 3;
	}
	int CColourControl::getChannel() const
	{
		return channel;
	}
	std::uint32_t CColourControl::floatToInt(iCtrlPrec_t val) const
	{
		ARGBPixel ownColour;
		ownColour.p = colour;
		switch (colourType) 
		{
		case RGB:
		{
			// set alpha to max anyway
			ownColour.c.r = ownColour.c.b = ownColour.c.g = 0;
			auto a1 = static_cast<decltype(colour)>(val * 0x00FFFFFF);
			auto a2 = ownColour.p;
			auto a3 = a1 | a2;
			return a3;
		}
		case ARGB:
			return static_cast<decltype(colour)>(val * 0xFFFFFFFF);
		case SingleChannel:
			ownColour.d[2 - channel] = static_cast<std::uint8_t>(val * 0xFF);
			return ownColour.p;
		case GreyTone:

			ownColour.c.r = ownColour.c.g = ownColour.c.b = static_cast<decltype(ownColour.c.r)>(val * 0xFF);
			return ownColour.p;
		default:
			return static_cast<decltype(colour)>(val * 0xFFFFFFFF);
		};
	}

	iCtrlPrec_t CColourControl::intToFloat(std::uint32_t val) const
	{
		switch (colourType)
		{
		case RGB:
			return double(val & 0x00FFFFFF) / (double)0xFFFFFF;
		case ARGB:
			return double(val) / (double)0xFFFFFFFF;
		case SingleChannel:
		{
			ARGBPixel pixel;
			pixel.p = val;
			return double(pixel.d[2 -channel]) / (double)0xFF;
		}
		case GreyTone:
		{
			ARGBPixel pixel;
			pixel.p = val;
			double sum = pixel.c.r + pixel.c.g + pixel.c.b;
			return sum / (3 * 0xFF);
		}
		default:
			return 0;
		};
	}
	std::uint32_t CColourControl::getColour()
	{

		return colour;
	}

	void CColourControl::setColour(std::uint32_t newColour)
	{
		bSetValue(intToFloat(newColour));
	}
	bool CColourControl::bStringToValue(const std::string & valueString, iCtrlPrec_t & value) const
	{
		std::uint32_t result(0);
		char * endPtr = nullptr;
		result = static_cast<std::uint32_t>(strtoul(valueString.c_str(), &endPtr, 0));
		if (endPtr > valueString.c_str())
		{
			value = intToFloat(result);
			return true;
		}
		return false;
	}

	bool CColourControl::bValueToString(std::string & valueString, iCtrlPrec_t value) const
	{
		char text[30];
		sprintf_s(text, "0x%.8X", floatToInt(value));
		valueString = text;
		return true;
	}

	void CColourControl::paint(juce::Graphics & g)
	{
		CKnobSlider::paint(g);
		auto c = juce::Colour(colour); 
		g.setColour(c);
		auto b = getTextRect().toFloat();

		g.fillRoundedRectangle(b.withTrimmedRight(5).withTrimmedBottom(2), 5.f);




	}

};
