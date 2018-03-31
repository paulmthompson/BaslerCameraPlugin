# BaslerCameraPlugin

## Overview

This is a plugin for OpenEphys that allows you to view the output of a Basler high speed camera directly in Open Ephys during neural data acquisition. 

## Installation 

This Plugin was designed with Pylon 5 from Basler and v0.4.3 Open Ephys (note that this is not the version with readily available binaries and will probably need to be compiled from source).

### Windows

Follow the instructions for creating Open Ephys from source https://open-ephys.atlassian.net/wiki/spaces/OEW/pages/491621/Windows

Download the pylon software. Install as a Developer.

In the BaslerCameraPlugin folder, inside "./Builds/VisualStudio2013" copy the "BaslerCamera" folder and paste it in the Open Ephys plugins folder with the other plugins (./Builds/VisualStudio2013/Plugins). Then copy "BaslerCamera" folder in the highest level directory which includes the source files into the Open Ephys source folder containing the plugin source code (./Source/Plugins).

Load the "Plugins" Solution into Visual Studio. This should be located in the Open Ephys director at "{Install Location}\Builds\VisualStudio2013\Plugins\Plugins.sln"  The Solution Explorer Window should appear and be populated with all of the native plugins that come with Open Ephys. Build these plugins for "Release" (Make sure Release rather than Debug is selected in the "Solutions Configuration" Box in the top center area of the screen). After this has finished, in the solutions explorer menu on the right side of the program, right click on the " Solution 'Plugins' " menu item at the top. Go to Add -> New Project. Navigate to the Basler Camera Project file you previously copied.

Right click on the Basler Camera Projectin the Solution Explorer and go to "Properties." Click on C/C++ in the left hand menu and the top menu box on the right side should be "Additional Include Directories. Click the drop down menu in this box and go to edit. Here you should add the file path to the include directories of the Basler Camera pylon software. For me, this is "C:\Program Files\Basler\pylon 5\Development\include"

Now click on the "Linker" menu item in the left hand menu. In about the middle of the right hand options you should see one labeled "Additional Library Directories." Again, click on the drop down menu and click edit. Now enter the file path to the 64-bit libraries associated with the Basler Camera Development package. For me, this filepath was "C:\Program Files\Basler\pylon 5\Development\lib\x64" Click apply and exit.

Now right click on the Solution 'Plugins' Menu Item again in the Solutions Explorer and go to "Configuration Manager". Find the entry for Basler Camera and make sure the platform is listed as x64. Now you can right click on the Basler Camera project item in the Solution Explorer and select build. Once this has finished, you should be able to run Open Ephys from .\Builds\VisualStudio2013\x64\Release64\bin\OpenEphys.exe with the Basler Plugin.

### Linux

## How to Use
