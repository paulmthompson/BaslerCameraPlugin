
#include <stdio.h>
#include "BaslerCamera.h"


class BaslerCameraEditor;

BaslerCamera::BaslerCamera()
	: GenericProcessor("Basler Camera"), Thread("BaslerGrabberThread")
{

}

BaslerCamera::~BaslerCamera()
{

}

void BaslerCamera::process(AudioSampleBuffer& buffer, MidiBuffer& events)
{
}

void BaslerCamera::startRecording()
{
}

void BaslerCamera::stopRecording()
{
}

void BaslerCamera::run()
{
}

AudioProcessorEditor* BaslerCamera::createEditor()
{
	editor = new BaslerCameraEditor(this,true);
	return editor;
}
