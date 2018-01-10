
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

void BaslerCamera::process(AudioSampleBuffer& buffer)
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

	enable();
}

bool BaslerCamera::enable()
{

	BaslerCameraEditor* editor = (BaslerCameraEditor*) getEditor();
	editor->enable();
	return true;

}

bool BaslerCamera::disable()
{
	BaslerCameraEditor* editor = (BaslerCameraEditor*) getEditor();
	editor->disable();
	return true;
}

AudioProcessorEditor* BaslerCamera::createEditor()
{
	editor = new BaslerCameraEditor(this,true);
	return editor;
}
