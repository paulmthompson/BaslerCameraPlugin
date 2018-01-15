/*
    ------------------------------------------------------------------

    This file is part of the Open Ephys GUI
    Copyright (C) 2013 Open Ephys

    ------------------------------------------------------------------

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

*/

#ifndef __BASLERCAMERAEDITOR_H__
#define __BASLERCAMERAEDITOR_H__

#include <EditorHeaders.h>
#include "BaslerCamera.h"
#include <stdio.h>
#include "../../Processors/Editors/GenericEditor.h"
#include "../../Processors/Editors/VisualizerEditor.h"

#include <pylon/PylonIncludes.h>
#include <pylon/usb/BaslerUsbInstantCamera.h>
typedef Pylon::CBaslerUsbInstantCamera Camera_t;
//#ifdef PYLON_WIN_BUILD
//#    include <pylon/PylonGUI.h>
//#endif

class BaslerCameraCanvas;

using namespace Pylon;

class MyCamera
{
public:
	MyCamera();
	~MyCamera();

	Camera_t camera;
	bool attached;
	bool acquisitionActive;
	float frameRate;
	float gain;
	int exposureTime;
	std::string saveFilePath;
};

class BaslerCameraViewer : public Component,
	Button::Listener, ComboBox::Listener
{
public:

	BaslerCameraViewer(GenericProcessor* proc, Viewport* p, BaslerCameraCanvas* c);
	~BaslerCameraViewer();
	void buttonClicked(Button* btn);
	void comboBoxChanged(ComboBox* b);
	SourceNode* proc;

private:
	Viewport* viewport;
	BaslerCameraCanvas* canvas;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BaslerCameraViewer);
};


class BaslerCameraCanvas : public Visualizer, public Button::Listener
{
public:
	BaslerCameraCanvas(GenericProcessor* n,MyCamera* mybasler);
	~BaslerCameraCanvas();

	void paint(Graphics& g);
	void update();
	void resized();
	void buttonClicked(Button* button);
	void refresh();
	void refreshState();
	void beginAnimation();
	void endAnimation();
	void setParameter(int, float);
	void setParameter(int, int, int, float);

	ScopedPointer<Viewport> cameraViewport;
	SourceNode* processor;
	ScopedPointer<BaslerCameraViewer> cameraViewer;

private:
	MyCamera* basler;
	
};



class BaslerCameraEditor : public VisualizerEditor, public ComboBox::Listener



{
public:
    BaslerCameraEditor(GenericProcessor* parentNode, bool useDefaultParameterEditors);
    ~BaslerCameraEditor();

	void comboBoxChanged(ComboBox* comboBoxThatHasChanged);
	Visualizer* createNewCanvas(void);
	void buttonEvent(Button* button);
	void sliderEvent(Slider* slider);
	
private:

	ScopedPointer<Label> sourceLabel;
	ScopedPointer<ComboBox> sourceCombo;
	ScopedPointer<UtilityButton> connectButton;
	ScopedPointer<UtilityButton> sourceButton;
	ScopedPointer<Slider> frameRateSlider;
	ScopedPointer<Label> frameRateLabel;
	ScopedPointer<Slider> exposureTimeSlider;
	ScopedPointer<Label> exposureTimeLabel;

	BaslerCameraCanvas* canvas;
	MyCamera* basler;
	

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BaslerCameraEditor);

};

#endif  // __BASLERCAMERAEDITOR_H__
		
