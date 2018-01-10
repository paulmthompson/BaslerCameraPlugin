
#include "BaslerCameraEditor.h"
#include <stdio.h>
#include "../../Processors/SourceNode/SourceNode.h"

#include <pylon/PylonIncludes.h>
#ifdef PYLON_WIN_BUILD
#    include <pylon/PylonGUI.h>
#endif

BaslerCameraViewer::BaslerCameraViewer(GenericProcessor* proc_, Viewport* p, BaslerCameraCanvas* c) : viewport(p), canvas(c)
{
	proc = (SourceNode*)proc_;
}

BaslerCameraViewer::~BaslerCameraViewer()
{
}

void BaslerCameraViewer::buttonClicked(Button* btn)
{
}

void BaslerCameraViewer::comboBoxChanged(ComboBox* b)
{
}

BaslerCameraCanvas::BaslerCameraCanvas(GenericProcessor* n)
{
	processor = (SourceNode*)n;
	cameraViewport = new Viewport();
	cameraViewer = new BaslerCameraViewer(processor, cameraViewport, this);
	cameraViewport->setViewedComponent(cameraViewer,false);
	cameraViewport->setScrollBarsShown(true,true);
	addAndMakeVisible(cameraViewport);
	elapsedFrames = 0;
	resized();
	update();
}

BaslerCameraCanvas::~BaslerCameraCanvas()
{
}

void BaslerCameraCanvas::update()
{
	repaint();
}

void BaslerCameraCanvas::resized()
{
}

void BaslerCameraCanvas::buttonClicked(Button* btn)
{
}

void BaslerCameraCanvas::refresh()
{
	elapsedFrames += 1;
	repaint();
}

void BaslerCameraCanvas::refreshState()
{
}

void BaslerCameraCanvas::beginAnimation()
{
	//startCallbacks();
	//This specifies the period in ms that the timer will get called
	startTimer(1000);
}

void BaslerCameraCanvas::endAnimation()
{
	stopCallbacks();
}

void BaslerCameraCanvas::setParameter(int x, float f)
{
}

void BaslerCameraCanvas::setParameter(int a, int b, int c, float d)
{
}

void BaslerCameraCanvas::paint(Graphics& g)
{
	Random& r(Random::getSystemRandom());
	g.setColour(Colour(r.nextInt(),r.nextInt(),r.nextInt()));
	g.fillAll();
	g.setColour(Colours::black);
	g.setFont(24.0f);

	int64 mytime = CoreServices::getGlobalTimestamp();
	float mysample = CoreServices::getGlobalSampleRate();

	g.drawText(String(mytime/mysample),getLocalBounds(),Justification::centred,true);
}

BaslerCameraEditor::BaslerCameraEditor(GenericProcessor* parentNode,bool useDefaultParameterEditors)
	: VisualizerEditor(parentNode, useDefaultParameterEditors)

{
	desiredWidth = 340;
	canvas = nullptr;
	tabText = "Camera";

	sourceLabel = new Label("video source", "Source");
	sourceLabel->setBounds(10,25,90,20);
	addAndMakeVisible(sourceLabel);

	sourceCombo = new ComboBox();
	sourceCombo->setBounds(110,25,220,20);
	addAndMakeVisible(sourceCombo);
}

BaslerCameraEditor::~BaslerCameraEditor()
{
}

void BaslerCameraEditor::comboBoxChanged(ComboBox* cb)
{

}

Visualizer* BaslerCameraEditor::createNewCanvas()
{
	GenericProcessor* processor = (GenericProcessor*) getProcessor();
	canvas = new BaslerCameraCanvas(processor);
	return canvas;

}
