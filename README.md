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
