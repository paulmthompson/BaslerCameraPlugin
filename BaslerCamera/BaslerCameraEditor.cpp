
#include "BaslerCameraEditor.h"
#include <stdio.h>
#include <fstream>
#include <iostream>
#include "../../Processors/SourceNode/SourceNode.h"

#include <pylon/PylonIncludes.h>
#include <pylon/usb/BaslerUsbInstantCamera.h>
//#ifdef PYLON_WIN_BUILD
//#    include <pylon/PylonGUI.h>
//#endif

// Namespace for using pylon objects.
using namespace Basler_UsbCameraParams;
using namespace Pylon;

MyCamera::MyCamera()
{
	Camera_t camera();
	attached = false;
	acquisitionActive = false;
	frameRate = 500.0;
	gain = 13.0;
	exposureTime = 1000;
}

MyCamera::~MyCamera()
{
}

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

BaslerCameraCanvas::BaslerCameraCanvas(GenericProcessor* n,MyCamera* mybasler)
{
	processor = (SourceNode*)n;

	basler = (MyCamera*) mybasler;

	cameraViewport = new Viewport();
	cameraViewer = new BaslerCameraViewer(processor, cameraViewport, this);
	cameraViewport->setViewedComponent(cameraViewer,false);
	cameraViewport->setScrollBarsShown(true,true);
	addAndMakeVisible(cameraViewport);
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
	basler->acquisitionActive=true;
	basler->camera.StartGrabbing();
	startTimer(40); 
}

void BaslerCameraCanvas::endAnimation()
{
	basler->acquisitionActive=false;
	basler->camera.StopGrabbing();
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

	if (basler->acquisitionActive != false && basler->attached) {
	int64 mytime = CoreServices::getGlobalTimestamp();
	float mysample = CoreServices::getGlobalSampleRate();    

        // This smart pointer will receive the grab result data.
        CGrabResultPtr ptrGrabResult;
	Image myImage(Image::ARGB, 640, 480, true);
	Image::BitmapData temp(myImage,Image::BitmapData::ReadWriteMode::readWrite);
	
	std::ofstream outbin("/mnt/data/Test/myfile.bin", std::ios::out | std::ios::binary | std::ios::app);

	//camera.RetrieveResult(0, ptrGrabResult, TimeoutHandling_ThrowException);
	int nBuffersInQueue = 0;
	char *mydata;
        while( basler->camera.RetrieveResult( 0, ptrGrabResult, TimeoutHandling_Return))
        {
            	nBuffersInQueue++;
	    	mydata = (char *) ptrGrabResult->GetBuffer();
		outbin.write(mydata,640*480);
        }
	outbin.close();
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

	desiredWidth = 400;
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

	frameRateLabel = new Label("frame rate label", "Frame Rate (fps)");
	frameRateLabel->setBounds(10,75,120,20);
	addAndMakeVisible(frameRateLabel);

	frameRateSlider = new Slider();
	frameRateSlider->setBounds(130,75,220,20);
	frameRateSlider->setRange(1.0, 500.0, 1.0);
	frameRateSlider->setValue(500.0);
	frameRateSlider->addListener(this);
	addAndMakeVisible(frameRateSlider);
	

	exposureTimeLabel = new Label("exposure time label", "Exposure Time (us)");
	exposureTimeLabel->setBounds(10,100,120,20);
	addAndMakeVisible(exposureTimeLabel);

	exposureTimeSlider = new Slider();
	exposureTimeSlider->setBounds(130,100,220,20);
	exposureTimeSlider->setRange(200.0,5000.0,1.0);
	exposureTimeSlider->setValue(1000.0);
	exposureTimeSlider->addListener(this);
	addAndMakeVisible(exposureTimeSlider);

	basler = new MyCamera();
}

BaslerCameraEditor::~BaslerCameraEditor()
{
}

void BaslerCameraEditor::comboBoxChanged(ComboBox* cb)
{

}

void BaslerCameraEditor::sliderEvent(Slider* slider)
{
	if (basler->attached) {
		if (slider == exposureTimeSlider)
		{
		
			double myvalue = slider->getValue();
			basler->camera.ExposureTime.SetValue(myvalue);

		} else if (frameRateSlider) {
		
			double myvalue = slider->getValue();
			basler->camera.AcquisitionFrameRate.SetValue(myvalue);

		}

		std::cout << "Resulting Frame Rate " << basler->camera.ResultingFrameRate.GetValue() << std::endl;
	}
}

void BaslerCameraEditor::buttonEvent(Button* button)
{
	if (button==connectButton)
	{
		// Before using any pylon methods, the pylon runtime must be initialized. 
		basler->camera.Attach(CTlFactory::GetInstance().CreateFirstDevice());
		if (basler->camera.IsPylonDeviceAttached())
		{
			std::cout << "Using device " << basler->camera.GetDeviceInfo().GetModelName() << std::endl;
			basler->attached = true;

			basler->camera.MaxNumBuffer = 35;
			basler->camera.Open(); // Need to access parameters

			std::cout << "Frame Rate " << basler->camera.AcquisitionFrameRate.GetValue() << std::endl;
			std::cout << "Exposure Time: " << basler->camera.ExposureTime.GetValue() << std::endl;
	
			basler->camera.AcquisitionFrameRateEnable.SetValue(true);
	
			basler->camera.AcquisitionFrameRate.SetValue(500.0);
			basler->camera.ExposureTime.SetValue(1000.0);

			//Resulting Frame Rate gives the real frame rate accounting for all of the camera
			//configuration parameters such as the desired sampling rate and exposure time
			std::cout << "Resulting Frame Rate " << basler->camera.ResultingFrameRate.GetValue() << std::endl;

		} else {
			std::cout << "Camera was not able to be initialized. Is one connected?" << std::endl;
		}
	}
}

Visualizer* BaslerCameraEditor::createNewCanvas()
{
	GenericProcessor* processor = (GenericProcessor*) getProcessor();
	canvas = new BaslerCameraCanvas(processor,basler);
	return canvas;

}

	
