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
 
	file:CTransformWidget.cpp
 
		Source code for CTransformWidget.h
	
*************************************************************************************/

#include "CTransformWidget.h"
#include "../Mathext.h"
#include "../LexicalConversion.h"

namespace cpl
{
	auto elementHeight = 15;
	auto elementWidth = 50;

	CTransformWidget::CTransformWidget()
	: 
		CBaseControl(this), 
		transform(1.0f),
		horizontalDragCursor(juce::MouseCursor::LeftRightResizeCursor),
		isAnyLabelBeingDragged(false),
		cursorSwap([&]() { setMouseCursor(juce::MouseCursor::ParentCursor); }, [&]() { setMouseCursor(horizontalDragCursor); })
	{

		bSetDescription("Controls an object's transformation in a visual 3D space.");
		enableTooltip(true);
		// otherwise, user can click anywhere in the program and focus will be given to the first label
		setMouseClickGrabsKeyboardFocus(false);
		for (auto & type : labels)
		{
			for (auto & label : type)
			{
				label.addListener(this);
				label.setSelectAllWhenFocused(true);
				label.setText("1", true);
				label.setColour(TextEditor::backgroundColourId, GetColour(ColourEntry::Deactivated));
				label.setColour(TextEditor::outlineColourId, GetColour(ColourEntry::Separator));
				label.setColour(TextEditor::textColourId, GetColour(ColourEntry::AuxillaryText));
				label.setScrollToShowCursor(false);
				addAndMakeVisible(label);
			}
		}
		syncEditor();
		setSize((elementWidth + 15) * 3 , elementHeight * 6);
		bSetIsDefaultResettable(true);
	}

	void CTransformWidget::onControlSerialization(CSerializer::Archiver & ar, Version version)
	{
		ar << transform;
	}

	void CTransformWidget::onControlDeserialization(CSerializer::Builder & ar, Version version)
	{
		ar >> transform;
		syncEditor();
	}

	void CTransformWidget::syncEditor()
	{
		char textBuf[200];
		for (unsigned x = 0; x < 3; ++x)
		{
			for (unsigned y = 0; y < 3; ++y)
			{
				sprintf_s(textBuf, "%.2f", transform.element(x, y));
				labels[x][y].setText(textBuf, false);
			}
		}
	}

	juce::String CTransformWidget::bGetToolTipForChild(const Component * c) const
	{
		juce::String ret = "Set the object's ";
		const char * params[] = { "position (where {0, 0, 0} is the center, and {1, 1, 1} is upper right back corner)", 
								  "rotation (in degrees)", "scale (where 1 is identity)" };
		const char * property[] = { "x-", "y-", "z-" };
		for (unsigned y = 0; y < 3; ++y)
		{

			for (unsigned x = 0; x < 3; ++x)
			{
				if (c == &labels[y][x])
				{
					ret += property[x];
					ret += params[y];
					return ret;
				}
			}
		}
		return juce::String::empty;
	}

	void CTransformWidget::inputCommand(int x, int y, const String & data)
	{
		float input;
		if (lexicalConversion(data, input))
		{
			transform.element(x, y) = (float)input;
			bForceEvent();
		}
	}

	void CTransformWidget::textEditorTextChanged(juce::TextEditor & t)
	{

	}


	void CTransformWidget::textEditorFocusLost(TextEditor & t)
	{
		for (unsigned x = 0; x < 3; ++x)
		{
			for (unsigned y = 0; y < 3; ++y)
			{
				if (&t == &labels[x][y])
				{
					inputCommand(x, y, t.getText());
					char textBuf[200];
					sprintf_s(textBuf, "%.2f", transform.element(x, y));
					t.setText(textBuf, false);
					break;
				}
			}
		}
	}
	void CTransformWidget::textEditorReturnKeyPressed(TextEditor & t)
	{
		t.unfocusAllComponents();
	}

	void CTransformWidget::mouseDown(const juce::MouseEvent & e)
	{
		if (e.eventComponent == this)
		{
			unfocusAllComponents();
			if (e.mods.isLeftButtonDown())
			{
				currentlyDraggedLabel = getDraggableLabelAt(e.x, e.y);
				if (currentlyDraggedLabel.second)
				{
					isAnyLabelBeingDragged = true;
				}
			}

		}
		lastMousePos = e.position;
	}

	void CTransformWidget::mouseUp(const juce::MouseEvent & e)
	{
		isAnyLabelBeingDragged = false;
	}


	void CTransformWidget::mouseDrag(const juce::MouseEvent & e)
	{
		if (isAnyLabelBeingDragged)
		{
			auto deltaDifference = e.position - lastMousePos;
			float scale = 0.005f;
			if (e.mods.isCommandDown())
			{
				scale *= 0.05f;
			}


			auto & now = transform.element(currentlyDraggedLabel.first.x, currentlyDraggedLabel.first.y);

			if (currentlyDraggedLabel.first.x == 1) // rotations
			{
				scale *= 0.3f * 360;
				now = std::fmod(now + scale * deltaDifference.x, 360.0f);
			}
			else
			{
				now += scale * deltaDifference.x;
			}
			lastMousePos = e.position;
			bForceEvent();
			syncEditor();
		}
	}

	void CTransformWidget::mouseMove(const juce::MouseEvent & e)
	{
		cursorSwap.setCondition(isAnyLabelBeingDragged || getDraggableLabelAt(e.x, e.y).second);
	}


	CTransformWidget::LabelDescriptor CTransformWidget::getDraggableLabelAt(int mouseX, int mouseY)
	{
		LabelDescriptor ret{ {0, 0}, nullptr };

		juce::Rectangle<int> currentLabelBounds;
		for (int y = 0; y < 3; ++y)
		{
			for (int x = 0; x < 3; ++x)
			{
				currentLabelBounds.setBounds(x * (elementWidth + 15), elementHeight + y * (elementHeight * 2), 10, elementHeight);
				if (currentLabelBounds.contains(mouseX, mouseY))
				{
					return LabelDescriptor{ {y, x}, &labels[y][x] };
				}
			}
		}
		return ret;
	}

	void CTransformWidget::resized()
	{

		for (unsigned x = 0; x < 3; ++x)
		{
			for (unsigned y = 0; y < 3; ++y)
			{
				labels[y][x].setBounds(
					10 + x * (elementWidth + 15), 
					elementHeight + y * (elementHeight * 2), 
					elementWidth,
					elementHeight
				);
			}
		}
	}
	void CTransformWidget::paint(juce::Graphics & g)
	{

		const char * texts[] = { "x:", "y:", "z:" };
		const char * titles[] = { " - Position - ", " - Rotation - ", " - Scale - " };

		g.setFont(TextSize::normalText);
		g.setColour(GetColour(ColourEntry::AuxillaryText));
		for (unsigned y = 0; y < 3; ++y)
		{
			g.drawText(titles[y], 0, y * (elementHeight * 2), getWidth(), elementHeight - 1, juce::Justification::centred);
		}
		g.setColour(GetColour(ColourEntry::AuxillaryText));
		g.setFont(TextSize::smallText);
		for (unsigned y = 0; y < 3; ++y)
		{
			for (unsigned x = 0; x < 3; ++x)
			{
				g.drawText(texts[x], x * (elementWidth + 15), elementHeight + y * (elementHeight * 2), 10, elementHeight, juce::Justification::centred);
			}
		}
	}

};
