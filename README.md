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

## Installation

To install and build the Plugins:

1. Install Rhino from [here](https://www.rhino3d.com/download) if not installed.
2. Follow the instructions from the [Rhino documentation](https://developer.rhino3d.com/guides/cpp/installing-tools-windows/) to install the Rhino SDK, Visual Studio and its required component.
3. Follow the instructions from the [Rhino documentation](https://developer.rhino3d.com/guides/grasshopper/installing-tools-windows/) to install the Grasshoper SDK.
4. Checkout this repository.
5. Open the solution with vs2017.
6. Build the _MasterBuild_ project to download PRT and build the 3 other projects. **It is necessary to do this step at least once. When PRT is installed, projects can be built independently(7. and 8.).**
7. To build the Rhino Plugin, select the _Release_ configuration with target platform _x64_.
8. To build the Grasshopper Plugin, select the _Release_gh_ configuration with target platform _x64_.

#### Plugin Installation

After having built the plugins, they have to be installed in Rhino and Grasshopper respectively.

8. Start Rhino. In the menu bar, go to _Tools -> Options -> Rhino Options -> Plug-ins_.
9. Click on _install_ and select the **RhinoPRT.rhp** file located in _path-to-project-dir/build/RhinoPRT.rhp_.
10. To install the grasshopper plugin, run the commpand _GrasshopperDeveloperSettings_ in Rhino.
11. In the window that opens, add the folder containing **GrasshopperPRT.gha**: _path-to-project-dir/build/RhinoPRT.gha_. Make sure the _Memory load _.GHA ...\* box is unticked.
12. Confirm, then close Rhino.

#### Starting the plugins

- To start the Rhino plugin, run the command _ApplyRulePackage_.
- After starting Grasshopper, the new component is located in the _Special_ tab.

## How to run the packaging script

Once both plugins are built, it is possible to create a RHI (Rhino Installer Engine) package and a YAK package using the create_package.py python script. A RHI package is simply a zip archive containing all files required to run a plugin. If Rhino is installed, the plugin can be installed by double-clicking the package. It will extract the files and Rhino/Grasshopper will load them when started. The YAK package is the archive that can be uploaded to the Rhino servers in order to publish the plug-in.

To run the script:

- Open the console, navigate to the project's root and run the command _"\<path-to-your-python-executable> create_package.py <option>"_. Replace _<option>_ by _both_, _rhi_, or _yak_, to choose which package to build.
- The resulting rhi and yak packages will be created in a folder named "Package_output" located in the project's root directory.

Install the package:

- In order for the plugin to be correctly loaded, it is needed to tick the "Ask to load disabled plug-ins" box located in Rhino's Tools -> Options -> Plug-ins.
- Close Rhino if it is open.
- Run the rhi package by double-clicking it.
- The package installer will open. Follow the instructions.
- The plugin will be loaded at the next start of Rhino/Grasshopper.

## Licensing Information

Puma is free for personal, educational, and non-commercial use. Commercial use requires at least one commercial license of the latest CityEngine version installed in the organization. Redistribution or web service offerings are not allowed unless expressly permitted.

Puma is under the same license as the included [CityEngine SDK](https://github.com/Esri/esri-cityengine-sdk#licensing). An exception is the Puma source code (without CityEngine SDK, binaries, or object code), which is licensed under the Apache License, Version 2.0 (the “License”); you may not use this work except in compliance with the License. You may obtain a copy of the License at https://www.apache.org/licenses/LICENSE-2.0.

All content in the "Examples" directory/section is licensed under the APACHE 2.0 license as well.

For questions or enquiries, please contact the Esri CityEngine team (cityengine-info@esri.com).


