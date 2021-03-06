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

	file:CTransformWidget.h

		A widget that can design a transform.

*************************************************************************************/

#ifndef CPL_CTRANSFORMWIDGET_H
#define CPL_CTRANSFORMWIDGET_H

#include "WidgetBase.h"
#include "../../rendering/Graphics.h"

namespace cpl
{
	class CTransformWidget
		:
		public ValueControl<TransformValue, CompleteTransformValue>,
		public juce::Component,
		public juce::TextEditor::Listener
	{

	public:
		typedef std::pair<juce::Point<int>, juce::TextEditor *> LabelDescriptor;

		CTransformWidget(TransformValue * value = nullptr, bool takeOwnership = false);

		juce::String bGetToolTipForChild(const Component *) const override;
		// coordinates are boxes
		void inputCommand(int x, int y, const String & data);

	protected:
		void syncEditor();
		// coordinates are mouse/graphical
		LabelDescriptor getDraggableLabelAt(int x, int y);
		void resized() override;
		void paint(juce::Graphics & g) override;
		void textEditorTextChanged(juce::TextEditor &) override;
		void textEditorFocusLost(TextEditor &) override;
		void textEditorReturnKeyPressed(TextEditor &) override;
		void mouseDown(const juce::MouseEvent & e) override;
		void mouseUp(const juce::MouseEvent & e) override;
		void mouseMove(const juce::MouseEvent & e) override;
		void mouseDrag(const juce::MouseEvent & e) override;

		void onControlSerialization(CSerializer::Archiver & ar, Version version) override;
		void onControlDeserialization(CSerializer::Builder & ar, Version version) override;
		virtual void onValueObjectChange(ValueEntityListener * sender, ValueEntityBase * value) override;

		juce::TextEditor labels[3][3];
		juce::MouseCursor horizontalDragCursor;
		LabelDescriptor currentlyDraggedLabel;
		Utility::ConditionalSwap cursorSwap;
		bool isAnyLabelBeingDragged;
		juce::Point<float> lastMousePos;
	};
};
#endif
