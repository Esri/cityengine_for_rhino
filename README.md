# rhino-plugin-prototype
This is a prototype of RhinoPRT. It enables the execution of CityEngine CGA rules within Rhino/Grasshopper.

## Installation
To install and build the Plugins:
1. Install Rhino from [here](https://www.rhino3d.com/download) if not installed.
2. Follow the instructions from the [Rhino documentation](https://developer.rhino3d.com/guides/cpp/installing-tools-windows/) to install the Rhino SDK, Visual Studio and its required component.
3. Follow the instructions from the [Rhino documentation](https://developer.rhino3d.com/guides/grasshopper/installing-tools-windows/) to install the Grasshoper SDK.
4. Checkout this repository.
5. Open the solution with vs2017.
6. To build the Rhino Plugin, select the *Release* configuration with target platform *x64*.
7. To build the Grasshopper Plugin, select the *Release_gh* configuration with target platform *x64*.

#### Plugin Installation
After having built the plugins, they have to be installed in Rhino and Grasshopper respectively.

8. Start Rhino. In the menu bar, go to *Tools -> Options -> Rhino Options -> Plug-ins*.
9. Click on *install* and select the **RhinoPRT.rhp** file located in *path-to-project-dir/x64/Release*.
10. To install the grasshopper plugin, run the commpand *GrasshopperDeveloperSettings* in Rhino.
11. In the window that opens, add the folder containing **GrasshopperPRT.gha**: *path-to-project-dir/x64/Release_gh*. Make sure the *Memory load *.GHA ...* box is unticked.
12. Confirm, then close Rhino.

#### Starting the plugins
- To start the Rhino plugin, run the command *ApplyRulePackage*.
- After starting Grasshopper, the new component is located in the *Special* tab.

## How to run the packaging script
Once both plugins are built, it is possible to create a RHI (Rhino Installer Engine) package using the CreateRHIPackage.py python script. A RHI package is simply a zip archive containing all files required to run a plugin. If Rhino is installed, the plugin can be installed by double-clicking the package. It will extract the files and Rhino/Grasshopper will load them when started.

To run the script:
- Open the console, navigate to the project's root and run the command _"\<path-to-your-python-executable> CreateRHIPackage.py"_.
- The resulting rhi package will be created in a folder named "Package_output" located in the project's root directory.

Install the package:
- In order for the plugin to be correctly loaded, it is needed to tick the "Ask to load disabled plug-ins" box located in Rhino's Tools -> Options -> Plug-ins.
- Close Rhino if it is open.
- Run the rhi package by double-clicking it.
- The package installer will open. Follow the instructions.
- The plugin will be loaded at the next start of Rhino/Grasshopper.
