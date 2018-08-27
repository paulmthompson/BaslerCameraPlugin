
#include "BaslerCameraEditor.h"
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <errno.h>
#include "../../Processors/SourceNode/SourceNode.h"

#include <pylon/PylonIncludes.h>
#include <pylon/usb/BaslerUsbInstantCamera.h>


// Namespace for using pylon objects.
using namespace Basler_UsbCameraParams;
using namespace Pylon;
using namespace std;

MyCamera::MyCamera()
{
	Camera_t camera();
	attached = false;
	acquisitionActive = false;
	saveData=false;
	frameRate = 500.0;
	saveFilePath = "./";
	saveFileName = "output.mp4";

	string file_path = __FILE__;
	string dir_path = file_path.substr(0, file_path.rfind("/"));

	configFileName = "./default.pfs";
	std::cout << configFileName << std::endl;
	framesGrabbed = false;
	totalFramesSaved = 0;
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
	if (basler->acquisitionActive != false && basler->attached) {

		int64 mytime = CoreServices::getGlobalTimestamp();
		float mysample = CoreServices::getGlobalSampleRate();

        	// This smart pointer will receive the grab result data.
        	CGrabResultPtr ptrGrabResult;

		int nBuffersInQueue = 0;
		basler ->framesGrabbed = false;

		if (basler->saveData) {

			while( basler->camera.RetrieveResult( 0, ptrGrabResult, TimeoutHandling_Return))
			{

				//For each frame we grab we will send it to
				//ffmpeg for saving
				//And write the total frames saved at the current time
				//stamp as a rough backup of the camera alignment
				//in case something goes wrong with the TTls
				nBuffersInQueue++;
				basler->mydata = (char *) ptrGrabResult->GetBuffer();
				fwrite(basler->mydata, sizeof(char)*640*480, 1, basler->ffmpeg);

				basler->totalFramesSaved+=1;
			}

			int64 mytime = CoreServices::getGlobalTimestamp();
			basler->saveTimeStamp.write(reinterpret_cast<const char *>(&mytime), sizeof(mytime));
			basler->saveTimeStamp.write(reinterpret_cast<const char *>(&basler->totalFramesSaved), sizeof(basler->totalFramesSaved));
			basler->saveTimeStamp.flush();
		} else {

			while( basler->camera.RetrieveResult( 0, ptrGrabResult, TimeoutHandling_Return))
			{
				nBuffersInQueue++;
				basler->mydata = (char *) ptrGrabResult->GetBuffer();
			}

		}
        //std::cout << "Retrieved " << nBuffersInQueue << " grab results from output queue." << std::endl;
		if (nBuffersInQueue) {
			basler->framesGrabbed = true;
		}
	}

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

	g.setColour(Colour(0,0,0));
	g.fillRect(0,0,getWidth(),getHeight());

	if (basler->framesGrabbed) {
		Image myImage(Image::ARGB, 640, 480, true);
		Image::BitmapData temp(myImage,Image::BitmapData::ReadWriteMode::readWrite);

		for (int y = 0; y< 480; y++)
		{
			for (int x = 0; x<640; x++)
			{
				temp.setPixelColour(x,y,Colour::fromRGBA(255,255,255,basler->mydata[640*y + x]));
			}
		}

		g.drawImageAt(myImage,0,0);
	}
}

BaslerCameraEditor::BaslerCameraEditor(GenericProcessor* parentNode,bool useDefaultParameterEditors)
	: VisualizerEditor(parentNode, useDefaultParameterEditors)

{
	std::ios_base::sync_with_stdio(false);
	PylonInitialize();

	desiredWidth = 500;
	canvas = nullptr;
	tabText = "Camera";

	sourceButton = new UtilityButton("Load Config File",titleFont);
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

	frameRateLabel = new Label("frame rate label", "Frame Rate (fps): ");
	frameRateLabel->setBounds(10,75,120,20);
	addAndMakeVisible(frameRateLabel);

	labelSaveFolder = new Label("save folder", "./");
	labelSaveFolder->setBounds(400, 50, 100, 20);
	labelSaveFolder->setFont(Font("Default", 15, Font::plain));
	labelSaveFolder->setEditable(true);
	labelSaveFolder->addListener(this);
	addAndMakeVisible(labelSaveFolder);

	labelFolder = new Label("save folder label", "Folder Path");
	labelFolder->setBounds(350, 50, 50, 20);
	addAndMakeVisible(labelFolder);

	labelSaveFile = new Label("save file", "output.mp4");
	labelSaveFile->setBounds(400, 75, 100, 20);
	labelSaveFile->setFont(Font("Default", 15, Font::plain));
	labelSaveFile->setEditable(true);
	labelSaveFile->addListener(this);
	addAndMakeVisible(labelSaveFile);

	labelFile = new Label("save file label", "File Name");
	labelFile->setBounds(350, 75, 50, 20);
	addAndMakeVisible(labelFile);

	basler = new MyCamera();
}

BaslerCameraEditor::~BaslerCameraEditor()
{
}

void BaslerCameraEditor::comboBoxChanged(ComboBox* cb)
{

}

void BaslerCameraEditor::labelTextChanged(juce::Label *label)
{
	if (label == labelSaveFolder)
	{
		Value val = label->getTextValue();

		basler->saveFilePath = val.toString().toStdString();
	}
	else if (label == labelSaveFile) {
		Value val = label->getTextValue();

		basler->saveFileName = val.toString().toStdString();
	}
}


void BaslerCameraEditor::buttonEvent(Button* button)
{
	if (button==connectButton)
	{

		//Check if camera is already attached
		if (!basler->attached) {

			// Before using any pylon methods, the pylon runtime must be initialized.
			basler->camera.Attach(CTlFactory::GetInstance().CreateFirstDevice());
			if (basler->camera.IsPylonDeviceAttached())
			{
				std::cout << "Using device " << basler->camera.GetDeviceInfo().GetModelName() << std::endl;
				basler->attached = true;

				basler->camera.MaxNumBuffer = 35;
				basler->camera.Open(); // Need to access parameters

				//Load values from configuration file
				CFeaturePersistence::Load(basler->configFileName.c_str(), &basler->camera.GetNodeMap(), true);

				std::cout << "Frame Rate " << basler->camera.AcquisitionFrameRate.GetValue() << std::endl;
				std::cout << "Exposure Time: " << basler->camera.ExposureTime.GetValue() << std::endl;

				//basler->camera.AcquisitionFrameRateEnable.SetValue(true);

				//basler->camera.AcquisitionFrameRate.SetValue(500.0);
				//basler->camera.ExposureTime.SetValue(1000.0);

				//Resulting Frame Rate gives the real frame rate accounting for all of the camera
				//configuration parameters such as the desired sampling rate and exposure time
				std::cout << "Resulting Frame Rate " << basler->camera.ResultingFrameRate.GetValue() << std::endl;
			}
		} else {
			std::cout << "Camera was not able to be initialized. Is one connected?" << std::endl;
		}
	} else if (button == sourceButton)
	{

		FileChooser myChooser ("Configuration Location",File::getSpecialLocation (File::userHomeDirectory),"*");

		if (myChooser.browseForFileToOpen())
		{
			File configFileLocation (myChooser.getResult());
			String configFile = configFileLocation.getFullPathName();
			basler->configFileName = configFile.toStdString();
		}

	} else if (button == saveButton)
	{

		bool myState = button->getToggleState();
		if (myState) {
			char full_cmd[1000];
			//Should have a configuration file or something to state location of ffmpeg along with
			//other default parameters, and probably the location of the basler camera features
			char ffmpeg_filepath[] = "ffmpeg";
			char ffmpeg_cmd[] = "-f rawvideo -pix_fmt gray -s 640x480 -i - -y -pix_fmt nv12 -vcodec h264_nvenc";
			int len;

			len = std::snprintf(full_cmd, sizeof(full_cmd), "%s %s %s%s", ffmpeg_filepath, ffmpeg_cmd, basler->saveFilePath.c_str(), basler->saveFileName.c_str());

			#ifdef _WIN32
				basler->ffmpeg = _popen(full_cmd, "wb");
			#else
				basler->ffmpeg = popen(full_cmd, "w");
			#endif
			if (basler->ffmpeg == NULL) {
				printf("Error opening file unexist.ent: %s\n", strerror(errno));
			}

			len = std::snprintf(full_cmd, sizeof(full_cmd), "%s%s%s",  basler->saveFilePath.c_str(), basler->saveFileName.c_str(),"_tsbackup.bin");

			//Create fstream for backup timestamp saving
			basler->saveTimeStamp = std::fstream(full_cmd,std::ios::out | std::ios::binary);
		}
		else {
			//Uncheck the save button, close ffmpeg and time stamp backup stream
			#ifdef _WIN32
				_pclose(basler->ffmpeg);
			#else
				pclose(basler->ffmpeg);
			#endif

			//close backup timestamp stream
			basler->saveTimeStamp.close();

			//reset the number of total frames saved
			basler->totalFramesSaved = 0;
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
