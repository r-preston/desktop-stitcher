	  ===============================================
	===============   DesktopStitcher   ===============
	  ===============================================

This is a program that randomly selects images from the directory it is executed in into a desktop background for multiple monitors. Images are selected at random

:: LIMITATIONS

> Can only deal with monitors that are arranged horizontally. It cannot cope with monitors stacked on top of each other
> All of the provided images must be the SAME SIZE, although this can be any size you want

:: REQUIREMENTS

> Install ImageMagick and add it to the system PATH (usually done automatically)
> Install the GNU GCC C/C++ compiler

:: INSTALLATION

> In main.cpp, set the SOURCE_IMAGE_WIDTH and SOURCE_IMAGE_HEIGHT macros to the dimensions of the images you are providing the program. (lines 27 & 28)
> Open Command Prompt to the directory main.cpp resides in and run the command 'mingw32-make'. This creates the executable DesktopStitcher.exe
> Place the DesktopStitcher.exe wherever you want. Create a shortcut to it.
> Set the working directory to the folder the images you want to use are. [right click on shortcut] > 'Properties' > 'Start in'
> Run the program once using the shortcut, this will create the folders 'Background' and 'Images' in the image folder specified. 

	IMPORTANT: make sure there aren't any folders called 'Background' or 'Images' in the folder before
	           running DesktopStitcher.exe, otherwise you will lose everything in those folders

> Go to Windows Settings and set up as follows:
	- set 'Background' to 'Slideshow'
	- set 'Choose albums for your slideshow' to the folder 'Background' in your chosen image folder
	- set 'Change picture every' to '1 minute'
	- set 'Choose a fit' to 'Span'
> The program should now work. To make it run every time you log in, copy the shortcut you created earlier to:

	C:/Users/[your name]/AppData/Roaming/Microsoft/Windows/Start menu/Programs/Startup