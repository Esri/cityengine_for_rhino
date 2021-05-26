# ![Puma Icon](doc/puma_icon_32px.png)  Puma - CityEngine Plugin for Rhino3D and Grasshopper

Puma is a plugin for [Rhino3D and Grasshopper](https://www.rhino3d.com). It provides a Rhino command and Grasshopper components which enable the execution of [CityEngine](https://www.esri.com/software/cityengine) ‘rules’ within a Rhino scene. Therefore, a Rhino artist or designer does not have to leave their familiar Rhino environment anymore to make use of CityEngine’s procedural modeling power. Complicated export-import steps are no longer needed, which also means that the procedural models do not need to be “baked” anymore. The building or street models stay procedural during the entire design or planning workflow. Consequently, the user can change any attributes of the building or street models easily by connecting them to other Grasshopper components.

Puma requires so-called rule packages (RPK) as input, which are authored in CityEngine. An RPK includes assets and a CGA rule file which encodes an architectural style. Comprehensive RPK examples are available below.

## Quick Start

1. Install Puma from the food4rhino market place.
1. Start Rhino and open Grasshopper
1. In Grasshopper go to the "Special" tab and find the "Esri" section, drag the Puma component into the sheet.
1. Create a "File Path" component and use the "Select one existing file" context menu entry to locate an RPK file from the examples directory.
1. Create a "Surface" component and use the "Set one surface" context menu entry to draw a surface in Rhino with the "Surface from 3 or 4 corner points" tool.
1. Connect the "File Path" component to the "RPK" input of Puma and "Surface" to the "Shape" input. Now Puma will generate the model in the Rhino viewport.

## Table of Contents

* [User Manual](#user-manual)
* [Developer Manual](#developer-manual)
* [Licensing Information](#licensing-information)

## User Manual

### Starting the plugins

* To start the Rhino plugin, run the command _ApplyRulePackage_.
* After starting Grasshopper, the new component is located in the _Special_ tab.

## Developer Manual

### Software Requirements

* Rhino 6 or 7 (<https://www.rhino3d.com/download>)
* Microsoft Visual Studio 2019 with MSVC 14.27 and C#
* Optional: Python 3.7 or later

### Build Instructions

1. Install the tools from the [Software Requirements](#software-requirements) section.
1. Follow the instructions from the [Rhino documentation](https://developer.rhino3d.com/guides/cpp/installing-tools-windows/) to install the Rhino SDK and related tools.
1. Follow the instructions from the [Rhino documentation](https://developer.rhino3d.com/guides/grasshopper/installing-tools-windows/) to install the Grasshoper SDK.
1. Checkout this Git repository.
1. Open the Visual Studio solution.
1. Build the `MasterBuild` project to download PRT and build the 3 other projects. **It is necessary to do this step at least once.**
1. To build the Rhino Plugin, select the `Release` configuration with target platform `x64`.
1. To build the Grasshopper Plugin, select the `Release_gh` configuration with target platform `x64`.

### Installing locally built plugins

After having built the plugins, they have to be installed in Rhino and Grasshopper respectively.

1. Start Rhino. In the menu bar, go to _Tools -> Options -> Rhino Options -> Plug-ins_.
1. Click on `install` and select the `RhinoPRT.rhp` file located in `path-to-project-dir/build/RhinoPRT.rhp`.
1. To install the grasshopper plugin, run the commpand `GrasshopperDeveloperSettings` in Rhino.
1. In the window that opens, add the folder containing `GrasshopperPRT.gha`: `path-to-project-dir/build/RhinoPRT.gha`. Make sure the `Memory load`.GHA ...\* box is unticked.
1. Confirm, then restart Rhino.

### Create installation packages

Once both plugins are built, it is possible to create a RHI (Rhino Installer Engine) package and a YAK package using the create_package.py python script. A RHI package is simply a zip archive containing all files required to run a plugin. If Rhino is installed, the plugin can be installed by double-clicking the package. It will extract the files and Rhino/Grasshopper will load them when started. The YAK package is the archive that can be uploaded to the Rhino servers in order to publish the plug-in.

1. Open a console, navigate to the project root and run the command `python create_package.py <option>`. Valid values for `<option>` are `both`, `rhi`, or `yak` to choose which package type to build.
1. The resulting `rhi` and `yak` packages will be created in a folder named `package_output` located in the project root directory.

### Install locally built packages

1. In order for the plugin to be correctly loaded, it is needed to tick the "Ask to load disabled plug-ins" box located in Rhino's _Tools -> Options -> Plug-ins_.
1. Close Rhino if it is open.
1. Run the rhi package by double-clicking it.
1. The package installer will open. Follow the instructions.
1. The plugin will be loaded at the next start of Rhino/Grasshopper.

## Licensing Information

Puma is free for personal, educational, and non-commercial use. Commercial use requires at least one commercial license of the latest CityEngine version installed in the organization. Redistribution or web service offerings are not allowed unless expressly permitted.

Puma is under the same license as the included [CityEngine SDK](https://github.com/Esri/esri-cityengine-sdk#licensing). An exception is the Puma source code (without CityEngine SDK, binaries, or object code), which is licensed under the Apache License, Version 2.0 (the “License”); you may not use this work except in compliance with the License. You may obtain a copy of the License at <https://www.apache.org/licenses/LICENSE-2.0>.

All content in the "Examples" directory/section is licensed under the APACHE 2.0 license as well.

For questions or enquiries, please contact the Esri CityEngine team (cityengine-info@esri.com).
