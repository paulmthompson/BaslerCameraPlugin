
#include "BaslerCameraEditor.h"
#include <stdio.h>
#include "../../Processors/SourceNode/SourceNode.h"

#include <pylon/PylonIncludes.h>
#include <pylon/usb/BaslerUsbInstantCamera.h>
//#ifdef PYLON_WIN_BUILD
//#    include <pylon/PylonGUI.h>
//#endif

// Namespace for using pylon objects.
using namespace Basler_UsbCameraParams;
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
	Camera_t camera();

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
	camera.MaxNumBuffer = 35;
	camera.Open(); // Need to access parameters

	std::cout << "Frame Rate " << camera.AcquisitionFrameRate.GetValue() << std::endl;
	std::cout << "Exposure Time: " << camera.ExposureTime.GetValue() << std::endl;
	
	camera.AcquisitionFrameRateEnable.SetValue(true);
	
	camera.AcquisitionFrameRate.SetValue(500.0);
	camera.ExposureTime.SetValue(1000.0);

	//Resulting Frame Rate gives the real frame rate accounting for all of the camera
	//configuration parameters such as the desired sampling rate and exposure time
	std::cout << "Resulting Frame Rate " << camera.ResultingFrameRate.GetValue() << std::endl;

	camera.StartGrabbing();
	startTimer(40); 
}

void BaslerCameraCanvas::endAnimation()
{
	acquisitionActive=false;
	camera.StopGrabbing();
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
	Image myImage(Image::ARGB, 640, 480, true);
	Image::BitmapData temp(myImage,Image::BitmapData::ReadWriteMode::readWrite);
	
	//camera.RetrieveResult(0, ptrGrabResult, TimeoutHandling_ThrowException);
	int nBuffersInQueue = 0;
	uint8 *mydata;
        while( camera.RetrieveResult( 0, ptrGrabResult, TimeoutHandling_Return))
        {
            nBuffersInQueue++;
	    mydata = (uint8 *) ptrGrabResult->GetBuffer();
        }

        std::cout << "Retrieved " << nBuffersInQueue << " grab results from output queue." << std::endl;

	for (int y = 0; y< 480; y++)
	{
		for (int x = 0; x<640; x++)
		{
			temp.setPixelColour(x,y,Colour::fromRGBA(1,1,1,mydata[640*y + x]));
		}
	}
	
	g.drawImageAt(myImage,0,0);

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
