
#include "BaslerCameraEditor.h"
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <errno.h>
#include "../../Processors/SourceNode/SourceNode.h"

#include <pylon/PylonIncludes.h>
#include <pylon/usb/BaslerUsbInstantCamera.h>
#include <pylon/AviCompressionOptions.h>

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
	saveData=false;
	frameRate = 500.0;
	gain = 13.0;
	exposureTime = 1000;
	saveFilePath = "/mnt/data/Test/myfile.bin";
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
	
	int nBuffersInQueue = 0;
	char *mydata;

	if (basler->saveData) {
		//std::ofstream outbin(const_cast<char*>(basler->saveFilePath.c_str()), std::ios::out | std::ios::binary | std::ios::app);	
		
        	while( basler->camera.RetrieveResult( 0, ptrGrabResult, TimeoutHandling_Return))
        	{
            	nBuffersInQueue++;
	    		mydata = (char *) ptrGrabResult->GetBuffer();
				fwrite(mydata, sizeof(char)*640*480, 1, basler->ffmpeg);
				//basler->aviWriter.Add(ptrGrabResult);
				//outbin.write(mydata,640*480);
        	}
			
		//outbin.close();
	} else {

		while( basler->camera.RetrieveResult( 0, ptrGrabResult, TimeoutHandling_Return))
        	{
            	nBuffersInQueue++;
				mydata = (char *) ptrGrabResult->GetBuffer();
        	}
		
	}
        //std::cout << "Retrieved " << nBuffersInQueue << " grab results from output queue." << std::endl;

	for (int y = 0; y< 480; y++)
	{
		for (int x = 0; x<640; x++)
		{
			temp.setPixelColour(x,y,Colour::fromRGBA(255,255,255,mydata[640*y + x]));
		}
	}
	
	g.drawImageAt(myImage,0,0);

	//std::cout << mytime / mysample << std::endl;
	}
}

BaslerCameraEditor::BaslerCameraEditor(GenericProcessor* parentNode,bool useDefaultParameterEditors)
	: VisualizerEditor(parentNode, useDefaultParameterEditors)

{
	std::ios_base::sync_with_stdio(false);
	PylonInitialize();

	desiredWidth = 400;
	canvas = nullptr;
	tabText = "Camera";

	sourceButton = new UtilityButton("Change Save Location",titleFont);
	sourceButton->setBounds(110,25,220,20);
	sourceButton->addListener(this);
	addAndMakeVisible(sourceButton);

	connectButton = new UtilityButton("Connect", titleFont);
	connectButton->setBounds(10,25,90,20);
	connectButton->addListener(this);
	addAndMakeVisible(connectButton);

	saveButton = new ToggleButton("Save");
	saveButton->setBounds(340,25,50,20);
	saveButton->addListener(this);
	addAndMakeVisible(saveButton);

	gainLabel = new Label("gain label", "Gain");
	gainLabel->setBounds(10,50,120,20);
	addAndMakeVisible(gainLabel);

	gainSlider = new Slider();
	gainSlider->setBounds(130,50,220,20);
	gainSlider->setRange(0.0,13.0,0.1);
	gainSlider->setValue(13.0);
	gainSlider->addListener(this);
	addAndMakeVisible(gainSlider);

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

		} else if (gainSlider) {
			
			/*
			double myvalue = slider->getValue();
			basler->camera.Gain.SetValue(myvalue);
			*/

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
	} else if (button == sourceButton) 
	{

		 FileChooser myChooser ("Save Location",File::getSpecialLocation (File::userHomeDirectory),"*");

		if (myChooser.browseForFileToSave(true))
    		{
        		File saveFileLocation (myChooser.getResult());
				String saveFile = saveFileLocation.getFullPathName();
				basler->saveFilePath = saveFile.toStdString();
    		}

	} else if (button == saveButton)
	{

		bool myState = button->getToggleState();
		if (myState) {
			const char* cmd = "C:/Users/Paul/Downloads/ffmpeg-3.4.1-win64-static/ffmpeg-3.4.1-win64-static/bin/ffmpeg -f rawvideo -pix_fmt gray -s 640x480 -r 500 -i - -y -pix_fmt nv12 -vcodec h264_qsv -preset veryfast -look_ahead 0 output.mp4";
			basler->ffmpeg = _popen(cmd, "wb");
			if (basler->ffmpeg == NULL) {
				printf("Error opening file unexist.ent: %s\n", strerror(errno));
			}
		}
		else {
			_pclose(basler->ffmpeg);
		}
		basler->saveData = myState;
	}
}

Visualizer* BaslerCameraEditor::createNewCanvas()
{
	GenericProcessor* processor = (GenericProcessor*) getProcessor();
	canvas = new BaslerCameraCanvas(processor,basler);
	return canvas;

}

	
