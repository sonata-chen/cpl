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

#include "CPresetWidget.h"
#include "../CPresetManager.h"
namespace cpl
{
	CPresetWidget::CPresetWidget(CSerializer::Serializable * content, const std::string & uName)
	: 
		CBaseControl(this), 
		name(uName), 
		parent(content),
		ext(uName + "." + programInfo.programAbbr)

	{
		initControls();
		enableTooltip();
		updatePresetList();

		addAndMakeVisible(layout);
	}

	std::string CPresetWidget::presetWithoutExtension(juce::File preset)
	{
		if (preset.existsAsFile() && preset.hasFileExtension(ext.c_str()))
		{
			auto fname = preset.getFileName();
			auto end = fname.lastIndexOfIgnoreCase(ext.c_str());

			auto str = fname.toStdString();
			return{ str.begin(), str.begin() + (end == -1 ? fname.length() : (end - 1)) };
		}
		return "";
	}

	std::string CPresetWidget::fullPathToPreset(const std::string & name)
	{
		return CPresetManager::instance().getPresetDirectory() + name + "." + ext;
	}

	void CPresetWidget::valueChanged(const CBaseControl * c)
	{
		if (c == &ksavePreset)
		{
			CCheckedSerializer serializer(name);
			parent->save(serializer.getArchiver(), programInfo.versionInteger);
			juce::File location;
			bool result = CPresetManager::instance().savePresetAs(serializer, location, name);
			// update list anyway; user may delete files in dialog etc.
			updatePresetList();
			if (result)
			{
				setSelectedPreset(location);
			}

		}
		else if (c == &kloadPreset)
		{
			CCheckedSerializer serializer(name);
			juce::File location;
			bool result = CPresetManager::instance().loadPresetAs(serializer, location, name);
			updatePresetList();
			if (result)
			{
				parent->load(serializer.getBuilder(), serializer.getBuilder().getMasterVersion());
				setSelectedPreset(location);
			}
		}
		else if (c == &kpresetList)
		{
			auto presetName = kpresetList.valueFor(kpresetList.getZeroBasedSelIndex());

			if (presetName.size())
			{
				juce::File location;
				CCheckedSerializer serializer(name);
				if (CPresetManager::instance().loadPreset(fullPathToPreset(presetName), serializer, location))
				{
					parent->load(serializer.getBuilder(), serializer.getBuilder().getMasterVersion());
					setSelectedPreset(location);
				}
			}


		}
	}

	void CPresetWidget::onValueChange()
	{
	}

	void CPresetWidget::onObjectDestruction(const ObjectProxy & object)
	{
	}

	const std::string & CPresetWidget::getName() const noexcept
	{
		return name;
	}

	bool CPresetWidget::setSelectedPreset(juce::File location)
	{
		std::string newValue = presetWithoutExtension(location);
		return kpresetList.bInterpretAndSet(newValue, true);
	}

	const std::vector<std::string>& CPresetWidget::getPresets()
	{
		// TODO: insert return statement here
		return{};
	}

	void CPresetWidget::updatePresetList()
	{
		auto & presetList = CPresetManager::instance().getPresets();
		std::vector<std::string> shortList;
		for (auto & preset : presetList)
		{
			auto name = presetWithoutExtension(preset);
			if(name.length())
				shortList.push_back(name);
		}

		kpresetList.setValues(shortList);


	}

	void CPresetWidget::initControls()
	{

		kloadPreset.bAddPassiveChangeListener(this);
		ksavePreset.bAddPassiveChangeListener(this);
		kpresetList.bAddPassiveChangeListener(this);

		kloadPreset.bSetTitle("Load preset");
		ksavePreset.bSetTitle("Save preset");
		kpresetList.bSetTitle("Preset list");


		bSetDescription("The preset widget allows you to save and load the state of the current local parent view.");
		kloadPreset.bSetDescription("Load a preset from a location.");
		ksavePreset.bSetDescription("Save the current state to a location.");

		layout.addControl(&kpresetList, 0, false);
		layout.addControl(&kloadPreset, 1, false);
		layout.addControl(&ksavePreset, 2, false);

		setSize(layout.getSuggestedSize().first, layout.getSuggestedSize().second);
	}
};
