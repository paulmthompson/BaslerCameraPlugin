
#include "BaslerCameraEditor.h"
#include <stdio.h>
#include "../../Processors/SourceNode/SourceNode.h"

#include <pylon/PylonIncludes.h>
//#ifdef PYLON_WIN_BUILD
//#    include <pylon/PylonGUI.h>
//#endif

// Namespace for using pylon objects.
using namespace Pylon;

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
	CInstantCamera camera();
	cameraViewport = new Viewport();
	cameraViewer = new BaslerCameraViewer(processor, cameraViewport, this);
	cameraViewport->setViewedComponent(cameraViewer,false);
	cameraViewport->setScrollBarsShown(true,true);
	addAndMakeVisible(cameraViewport);
	acquisitionActive = false;
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
	repaint();
}

void BaslerCameraCanvas::refreshState()
{
}

void BaslerCameraCanvas::beginAnimation()
{
	//startCallbacks();
	//This specifies the period in ms that the timer will get called
	camera.Attach(CTlFactory::GetInstance().CreateFirstDevice());
	std::cout << "Using device " << camera.GetDeviceInfo().GetModelName() << std::endl;
	acquisitionActive=true;
	camera.StartGrabbing( GrabStrategy_LatestImageOnly);
	startTimer(10);
}

void BaslerCameraCanvas::endAnimation()
{
	acquisitionActive=false;
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

	if (acquisitionActive != false && camera.IsPylonDeviceAttached()) {
	int64 mytime = CoreServices::getGlobalTimestamp();
	float mysample = CoreServices::getGlobalSampleRate();    

        // This smart pointer will receive the grab result data.
        CGrabResultPtr ptrGrabResult;

	camera.RetrieveResult(0, ptrGrabResult, TimeoutHandling_ThrowException);

        // Image grabbed successfully?
        if (ptrGrabResult->GrabSucceeded())
        {

		Image myImage(Image::ARGB, 640, 480, true);
		Image::BitmapData temp(myImage,Image::BitmapData::ReadWriteMode::readWrite);
		uint8 *mydata = (uint8 *) ptrGrabResult->GetBuffer();
		for (int y = 0; y< 480; y++)
		{
			for (int x = 0; x<640; x++)
			{
				temp.setPixelColour(x,y,Colour::fromRGBA(0,0,0,mydata[640*y + x]));
			}
		}
		g.drawImageAt(myImage,0,0);
        }

	//Save data
	//CImagePersistence::Save( ImageFileFormat_Png, "/home/wanglab/Pictures/GrabbedImage.png", ptrGrabResult);

	std::cout << mytime / mysample << std::endl;
	}
}

BaslerCameraEditor::BaslerCameraEditor(GenericProcessor* parentNode,bool useDefaultParameterEditors)
	: VisualizerEditor(parentNode, useDefaultParameterEditors)

{
	PylonInitialize();

	desiredWidth = 340;
	canvas = nullptr;
	tabText = "Camera";

	sourceLabel = new Label("video source", "Source");
	sourceLabel->setBounds(10,25,90,20);
	addAndMakeVisible(sourceLabel);

	sourceCombo = new ComboBox();
	sourceCombo->setBounds(110,25,220,20);
	addAndMakeVisible(sourceCombo);

	connectButton = new UtilityButton("Connect", titleFont);
	connectButton->setBounds(10,50,90,20);
	connectButton->addListener(this);
	addAndMakeVisible(connectButton);

}

BaslerCameraEditor::~BaslerCameraEditor()
{
}

void BaslerCameraEditor::comboBoxChanged(ComboBox* cb)
{

}

void BaslerCameraEditor::buttonEvent(Button* button)
{
	if (button==connectButton)
	{
		// Before using any pylon methods, the pylon runtime must be initialized. 
		std::cout << "Hello" << std::endl;
	}
}

Visualizer* BaslerCameraEditor::createNewCanvas()
{
	GenericProcessor* processor = (GenericProcessor*) getProcessor();
	canvas = new BaslerCameraCanvas(processor);
	return canvas;

}
