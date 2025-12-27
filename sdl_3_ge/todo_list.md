# TODO LIST 

## goals for the near future
* create_texture_from_path
* move non-window classes and structs to own file 


## window class

### non-SDL methods that should be implemented 
* draw_space_to_uv 
* uv_to_draw_space

### plot API functions 
* plot_to
* set_cursor_pos
* move_to


### functions that are still needing to be wrapped

#### from the *renderer API* 
* SDL_RenderCoordinatesFromWindow
* SDL_RenderCoordinatesToWindow
* SDL_GetRenderScale
* SDL_[Get/Set]RenderDrawBlendMode
* -SDL_RenderRects
* -SDL_RenderFillRects
* -SDL_RenderLines
* -SDL_RenderPoints

#### from the *window API*
* ***test*** SDL_DisableScreenSaver
* ***test*** SDL_EnableScreenSaver 
* SDL_SetWindowFullscreenMode
* SDL_SetWindowSize
* SDL_FlashWindow
* SDL_CreatePopupWindow
* SDL_GetClosestFullscreenDisplayMode
* SDL_GetCurrentDisplayOrientation
* SDL_GetWindowBordersSize 
* SDL_GetWindowFlags
* SDL_GetWindowFullscreenMode
* SDL_GetWindowPixelDensity
* SDL_GetWindowPixelFormat
* SDL_GetWindowPosition

## random number generator 
*function to choose from a span.  

## game_pad.hpp
* make function that returns the name of an axis/hat when pressed

## world class

## Message/File Box API

## LUA API stuff
* make C++ wrapper and general improvements to [LUA/C++] interface
* the ablity to handle events from LUA
* make an api for creating cut scenes in LUA
* trigger LUA Events that I can listen to on the native side.
* API for loading worlds via LUA 

## general to-dos 
* reformat and add comments to main.hpp

## resource mangigment and other stuff
* better API for CLI/Console settings
    * make the console accept input while the app is running
* file format for saving paramiters.
    * possibly get INI to work cross platform? 
* a file manager  
	* properties 
		* m_work_dir  (the base dir)
		* m_types     (a map of extensions to handler functions)
	* methods 
		* get_working_dir(void) returns m_work_dir 
		* operator/(path) returns the path with the string/path appended to it.
		* exists(path) returns if the entity exists in the working dir
		* open(path) calls the respective callbackc for the passed file
		* close(path) calls the close handle for the resource 
* audio play back (part of window?) 


#### general notes 
a - before an list item means that they are low proirety 
