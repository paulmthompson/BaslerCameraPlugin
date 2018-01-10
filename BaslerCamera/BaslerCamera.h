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

#ifndef __BASLERCAMERA_H_INCLUDED__
#define __BASLERCAMERA_H_INCLUDED__

#ifdef _WIN32
#include <Windows.h>
#endif

#include <ProcessorHeaders.h>
#include "BaslerCameraEditor.h"

class BaslerCamera : public GenericProcessor, public Thread
{
public:

	BaslerCamera();
	~BaslerCamera();

	bool isSource()
	{
		return true;
	}

	bool isSink()
	{
		return false;
	}

	void process(AudioSampleBuffer& buffer, MidiBuffer& events);

	void startRecording();
	void stopRecording();

	bool enable() override;
	bool disable() override;

	void run();

	AudioProcessorEditor* createEditor();



private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BaslerCamera);

};

#endif  // __BASLERCAMERA_H_INCLUDED__
