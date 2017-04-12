/** Example 001 HelloWorld adapted to emscripten

This Tutorial shows how to run code with emscripten.
Emscripten compiles c++ to asm.js to allow it running inside a webbrowser.
You have to setup the emscripten environment on your system first to use this.
*/
#include <irrlicht.h>
#include "exampleHelper.h"
#include <emscripten.h>
#include <stdio.h>

/*
The code in here is mostly similar to the usual HelloWorld.
You can find more information about it there. Here we mainly document the 
differences needed for emscripten.
*/

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

/*
This part not necessary for emscripten, only useful to keep it in 
in case you want to run the same code on Windows with VS as well.
*/
#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
#endif


/*
Variables on the stack will stay intact between runs of one_iter()
*/
IrrlichtDevice *device = 0;
IVideoDriver* driver = 0;
ISceneManager* smgr = 0;
IGUIEnvironment* guienv = 0;

/*
	emscripten can't run things in an endless-loop or otherwise the browse will consider
	the script to hang.
*/
#ifdef __EMSCRIPTEN__
void one_iter()
{
    if(!device->run()) 
	{
		// Could clean up here in theory, but not sure if it makes a difference
		
		/*
		This tells emscripten to not run any further code.
		*/
        emscripten_cancel_main_loop();
        return;
    }
    driver->beginScene(ECBF_COLOR | ECBF_DEPTH, SColor(255,100,101,140));

    smgr->drawAll();
    guienv->drawAll();

    driver->endScene();
}
#endif //__EMSCRIPTEN__


/*
	The main method is also run on emscripten.
*/
int main()
{
	/*
	Create device flags for emscripten are still experimental 
	and might not all work.
	
	- deviceType: You have to use video::EDT_OGLES2 which will be translated 
	   to WebGL by emscripten.
	   It can be useful to have EDT_OGLES2 or EDT_OGLES1 for comparison for
	   testing on the desktop.
	*/
#ifndef __EMSCRIPTEN__
	device =
 		createDevice( video::EDT_OGLES1, dimension2d<u32>(640, 480), 16,
 			false, false, false, 0);
#else //__EMSCRIPTEN__
	device =
		createDevice(video::EDT_OGLES2, dimension2d<u32>(640, 480), 16,
    		false, false, false, 0);
#endif //__EMSCRIPTEN__

	if (!device)
		return 1;

	/*
	Window caption will set the title-text in the browser.
	*/
	device->setWindowCaption(L"Hello World! - Irrlicht Engine Demo");

	/*
	Get a pointer to the VideoDriver, the SceneManager and the graphical
	user interface environment, so that we do not always have to write
	device->getVideoDriver(), device->getSceneManager(), or
	device->getGUIEnvironment().
	*/
	driver = device->getVideoDriver();
	smgr = device->getSceneManager();
	guienv = device->getGUIEnvironment();

	/*
	We add a hello world label to the window, using the GUI environment.
	The text is placed at the position (10,10) as top left corner and
	(260,22) as lower right corner.
	*/
	guienv->addStaticText(L"Hello World! This is Irrlicht on emscripten!",
		rect<s32>(10,10,260,22), true);

	/*
	Get a media path dedicated for your platform. 
	We tell emscripten to copy the media folder in the Makefile with:
	"--preload-file ../../media@/media"
	That copies our ../../media folder in a .data 
	file which is loaded by the browser. It can then be accessed there
    by "/media" name (that's the parameter after the '@').
	Note that usually you would try to copy only as many files
	as absolutely necessary to reduce start-up times.
	*/
	const io::path mediaPath = getExampleMediaPath();

	/*
    Make a model called Sydney show up.
	*/
	IAnimatedMesh* mesh = smgr->getMesh(mediaPath + "sydney.md2");
	if (!mesh)
	{
		device->drop();
		return 1;
	}
	IAnimatedMeshSceneNode* node = smgr->addAnimatedMeshSceneNode( mesh );

	/*
	Disable lighting because we do not have a dynamic light in here, and	
	the mesh would be totally black otherwise. 
	Set the frame loop such that the predefined STAND animation is used. 
	Add a texture to the model. 
	*/
	if (node)
	{
		node->setMaterialFlag(EMF_LIGHTING, false);
		node->setMD2Animation(scene::EMAT_STAND);
		node->setMaterialTexture( 0, driver->getTexture(mediaPath + "sydney.bmp") );
	}

	/*
	To look at the mesh, we place a camera into 3d space at the position
	(0, 30, -40). The camera looks from there to (0,5,0), which is
	approximately the place where our md2 model is.
	*/
	smgr->addCameraSceneNode(0, vector3df(0,30,-40), vector3df(0,5,0));

#ifndef __EMSCRIPTEN__		// this part only so you can run the same code on desktop
	/*
	On desktop we run and endless loop until the user closes the window or 
	presses ALT+F4 (or whatever keycode closes a window).
	*/
	while(device->run())
	{
		driver->beginScene(ECBF_COLOR | ECBF_DEPTH, SColor(255,100,101,140));

		smgr->drawAll();
		guienv->drawAll();

		driver->endScene();
	}
	device->drop();
	
#else // __EMSCRIPTEN__
	
	/*
	Setting fps to 0 or a negative value will use the browser’s 
	requestAnimationFrame mechanism to call the main loop function. 
	Emscripten documentation recommends to do that, but you can also set 
	another fps value and the browser will try to call the main-loop 
	fps times per second.
	The simulate_infinite_loop tells emscripten that this is an application
	which will simulate an infinite loop. There is also a flag in the 
	Makefile about that: -s NO_EXIT_RUNTIME=1
	*/
	int fps = 0;	
	int simulate_infinite_loop = 1;
	emscripten_set_main_loop(one_iter, fps, simulate_infinite_loop);
#endif //__EMSCRIPTEN__

	return 0;
}

/*
That's it. Compile and run.
**/
