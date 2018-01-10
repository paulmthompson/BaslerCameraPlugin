
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
	//g.setColour(Colour(r.nextInt(),r.nextInt(),r.nextInt()));
	//g.fillAll();
	g.setColour(Colours::black);
	g.setFont(24.0f);

	int64 mytime = CoreServices::getGlobalTimestamp();
	float mysample = CoreServices::getGlobalSampleRate();

	g.drawText(String(mytime/mysample),getLocalBounds(),Justification::centred,true);

	// Create an instant camera object with the camera device found first.
        CInstantCamera camera( CTlFactory::GetInstance().CreateFirstDevice());

        // Print the model name of the camera.
        std::cout << "Using device " << camera.GetDeviceInfo().GetModelName() << std::endl;

        // The parameter MaxNumBuffer can be used to control the count of buffers
        // allocated for grabbing. The default value of this parameter is 10.
        camera.MaxNumBuffer = 5;

        // Start the grabbing of c_countOfImagesToGrab images.
        // The camera device is parameterized with a default configuration which
        // sets up free-running continuous acquisition.
        camera.StartGrabbing(1);

        // This smart pointer will receive the grab result data.
        CGrabResultPtr ptrGrabResult;

	camera.RetrieveResult( 5000, ptrGrabResult, TimeoutHandling_ThrowException);

        // Image grabbed successfully?
        if (ptrGrabResult->GrabSucceeded())
        {
                // Access the image data.
        	std::cout << "SizeX: " << ptrGrabResult->GetWidth() << std::endl;
		std::cout << "SizeY: " << ptrGrabResult->GetHeight() << std::endl;
		std::cout << "Payload Size: " << ptrGrabResult->GetPayloadSize() << std::endl;
        }

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
	std::cout << mydata[1] << std::endl;
	//CImagePersistence::Save( ImageFileFormat_Png, "/home/wanglab/Pictures/GrabbedImage.png", ptrGrabResult);
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

	// Before using any pylon methods, the pylon runtime must be initialized. 
    	PylonInitialize();
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
