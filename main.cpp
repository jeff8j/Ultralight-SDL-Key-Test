#include <iostream>       // std::cout
#include <thread>         // std::thread
#include <sstream>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_syswm.h>
#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2main.lib")
#pragma comment(lib, "SDL2_image.lib")

# if SDL_BYTEORDER == SDL_BIG_ENDIAN
#  define RMASK 0xff000000
#  define GMASK 0x00ff0000
#  define BMASK 0x0000ff00
#  define AMASK 0x000000ff
# else
#  define RMASK 0x000000ff
#  define GMASK 0x0000ff00
#  define BMASK 0x00ff0000
#  define AMASK 0xff000000
# endif



#include <Ultralight/CAPI.h>
#include <AppCore/CAPI.h>
#pragma comment(lib, "Ultralight.lib")
#pragma comment(lib, "AppCore.lib")

bool running = true;

//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
int main(int argc, char** argv) {


    SDL_Window* window;
    SDL_Surface* window_surface;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Failed to initialize the SDL2 library\n";
        return EXIT_FAILURE;
    }

    window = SDL_CreateWindow("Simple test",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800, 600,
        0);


    if (!window) {
        std::cerr << "Failed to create window\n";
        return EXIT_FAILURE;
    }

    window_surface = SDL_GetWindowSurface(window);

    if (!window_surface) {
        std::cerr << "Failed to get the surface from the window\n";
        return EXIT_FAILURE;
    }

    // now we should have a window




    ULConfig config = ulCreateConfig();
    ulEnablePlatformFontLoader();

    ULString base_dir = ulCreateString("./assets/");
    ulEnablePlatformFileSystem(base_dir);
    ulDestroyString(base_dir);


    ULString log_path = ulCreateString("./ultralight.log");
    ulEnableDefaultLogger(log_path);
    ulDestroyString(log_path);



    //Create our renderer using the Config we just set up.
    // You must set up your platform handlers before calling this. At a minimum, you must call
    // ulPlatformSetFileSystem() and  ulPlatformSetFontLoader() before calling this.
    ULRenderer renderer = ulCreateRenderer(config);

    ulDestroyConfig(config);




    //Create our View.
    /// Views are sized containers for loading and displaying web content.
    /// Let's set a 2x DPI scale and disable GPU acceleration so we can render to a bitmap.
    ULViewConfig view_config = ulCreateViewConfig();
    ulViewConfigSetInitialDeviceScale(view_config, 1.0); //2.0
    ulViewConfigSetIsAccelerated(view_config, false);


    ULView view = ulCreateView(renderer, 800, 600, view_config, NULL);

    ulDestroyViewConfig(view_config);


    // Get our View's rendering surface.
    ULSurface surface = ulViewGetSurface(view);



    //Should have a window and a ulrtralight now
    //lets load a url
    ULString url_string = ulCreateString("https://keyjs.dev/");
    ulViewLoadURL(view, url_string);
    ulDestroyString(url_string);



    //start sdl loop
    bool running = true;
    SDL_Event event;
    while ( running ) {

        //handle sdl events
        while (SDL_PollEvent(&event) ) {
            switch (event.type) {
                case SDL_MOUSEMOTION:
                    {
                        int x, y;
                        SDL_GetMouseState(&x, &y);
                        ULMouseEvent evt = ulCreateMouseEvent(kMouseEventType_MouseMoved, x, y, kMouseButton_None);
                        ulViewFireMouseEvent(view, evt);
                        ulDestroyMouseEvent(evt);
                    }
                    break;

                case SDL_MOUSEBUTTONDOWN:
                    if ((int)event.button.button == 1) { // left = 1
                        int x, y;
                        SDL_GetMouseState(&x, &y);
                        ULMouseEvent evt = ulCreateMouseEvent(kMouseEventType_MouseDown, x, y, kMouseButton_Left);
                        ulViewFireMouseEvent(view, evt);
                        ulDestroyMouseEvent(evt);
                    }
                    break;

                case SDL_MOUSEBUTTONUP:
                    if ((int)event.button.button == 1) { // left = 1
                        int x, y;
                        SDL_GetMouseState(&x, &y);
                        ULMouseEvent evt = ulCreateMouseEvent(kMouseEventType_MouseUp, x, y, kMouseButton_Left);
                        ulViewFireMouseEvent(view, evt);
                        ulDestroyMouseEvent(evt);
                    }
                    break;


                case SDL_KEYDOWN:
                    std::cout << "Key press detected " << event.key.keysym.sym << " " << event.key.keysym.scancode << " " << SDL_GetKeyName(event.key.keysym.sym) << " " << event.key.keysym.mod << std::endl;
                    {
                        int keycode = event.key.keysym.sym; // 101; // 69;
                        int scancode = event.key.keysym.scancode; // 8;
                        ULString keytext = ulCreateString(SDL_GetKeyName(event.key.keysym.sym) ); //ULString keytext = ulCreateString("E");
                        ULKeyEvent evt = ulCreateKeyEvent(kKeyEventType_RawKeyDown, 0, keycode, scancode, keytext, keytext, false, false, false);
                        ulViewFireKeyEvent(view, evt);
                        ulDestroyKeyEvent(evt);
                    }
                    break;

                case SDL_KEYUP:
                    std::cout << "Key release detected " << event.key.keysym.sym << std::endl;
                    {
                        int keycode = event.key.keysym.sym; // 101; // 69;
                        int scancode = event.key.keysym.scancode; // 8;
                        ULString keytext = ulCreateString(SDL_GetKeyName(event.key.keysym.sym) ); //ULString keytext = ulCreateString("E");
                        ULKeyEvent evt = ulCreateKeyEvent(kKeyEventType_KeyUp, 0, keycode, scancode, keytext, keytext, false, false, false);
                        ulViewFireKeyEvent(view, evt);
                        ulDestroyKeyEvent(evt);
                    }
                    break;


                case SDL_QUIT:
                    running = false;
                    break;
            }
        }


        //ultralight update
        ulUpdate(renderer);
        //draw ultralight if dirty
        if ( ulViewGetNeedsPaint(view) ) {
            std::cout << "paint" << std::endl;
            ulRender(renderer);
            ULBitmap bitmap = ulBitmapSurfaceGetBitmap(surface);
            //Lock pixels for reading/writing, returns pointer to pixel buffer.
            void* pixels = ulBitmapLockPixels(bitmap);
            //get width
            unsigned int w = ulBitmapGetWidth(bitmap);
            //get height
            unsigned int h = ulBitmapGetHeight(bitmap);

            ULBitmapFormat format = ulBitmapGetFormat(bitmap);
            if (format != kBitmapFormat_BGRA8_UNORM_SRGB) {
                std::cerr << "unhandled webkit image format" << std::endl;
                return EXIT_FAILURE;
            }

            //copy
            //img->surface = SDL_CreateRGBSurfaceFrom(pixels, w, h,  32, 4*w, RMASK, GMASK, BMASK, AMASK);
            SDL_Surface* imgsurface = SDL_CreateRGBSurfaceFrom(pixels, w, h, 32, 4 * w, BMASK, GMASK, RMASK, AMASK);
            SDL_BlitSurface(imgsurface, NULL, window_surface, NULL);
            SDL_FreeSurface(imgsurface);

            //unlock
            ulBitmapUnlockPixels(bitmap);


            //update the screen
            SDL_UpdateWindowSurface(window);
        }



    }


    return EXIT_SUCCESS;
}


