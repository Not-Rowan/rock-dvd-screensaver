#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>

/*
 ** Description
 * Creates a window to be displayed on the screen
 ** Parameters
 * window_title: the title that is displayed on the menu bar of the window
 * window_pos_x: the x position on the screen to display the window
 * window_pos_y: the y position on the screen to display the window
 * window_size_x: the x size of the displayed window
 * window_size_y: the y size of the displayed window
 * flags: additional flags for things like look of window, functionality, etc...
 ** Return Value
 * window: returns a pointer to the window that has been displayed or null if error
 */
SDL_Window *create_window(const char *window_title, int window_pos_x, int window_pos_y, int window_size_x, int window_size_y, int flags) {
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        fprintf(stderr, "Failed to initialize the SDL2 library\n");
        return NULL;
    }

    SDL_Window *window = SDL_CreateWindow(window_title, window_pos_x, window_pos_y, window_size_x, window_size_y, flags);

    if (!window) {
        fprintf(stderr, "Failed to create window\n");
        return NULL;
    }

    return window;
}


/*
 ** Description
 * Creates the surface of the window so the window can actually be interacted with
 ** Parameters
 * window: the window variable returned by createWindow();
 ** Return Value
 * window_surface: pointer to the window surface or null if error
 */
SDL_Surface *create_window_surface(SDL_Window *window) {
    SDL_Surface *window_surface = SDL_GetWindowSurface(window);

    if (!window_surface) {
        fprintf(stderr, "Failed to get the surface from the window\n");
        return NULL;
    }

    return window_surface;
}


/*
 ** Description
 * Loads the surface of the bitmap to be displayed later
 ** Parameters
 * path: path to bitmap (.bmp) file
 ** Return Value
 * image_surface: the pointer to the image surface or null if error
 */
SDL_Surface *load_bmp_surface(char const *path) {
    SDL_Surface *image_surface = SDL_LoadBMP(path);

    if (!image_surface) {
        fprintf(stderr, "Failed to load image\n");
        fprintf(stderr, "SDL2 Error: %s\n", SDL_GetError());
        return NULL;
    }
    
    return image_surface;
}


/*
 ** Description
 * Changes the colour of an SDL surface
 ** Parameters
 * surface: pointer to the surface we want to change
 * R: red colour value
 * G: green colour value
 * B: blue colour value
 ** Return Value
 * N/A
 */
void change_colour(SDL_Surface* surface, Uint8 R, Uint8 G, Uint8 B) {
    // Check if the surface exists
    if (surface == NULL) {
        return;
    }

    // Lock the surface to access pixel data
    SDL_LockSurface(surface);

    // Get the pixel format
    SDL_PixelFormat* format = surface->format;

    // Iterate through each pixel
    for (int y = 0; y < surface->h; ++y) {
        for (int x = 0; x < surface->w; ++x) {
            // Get the pixel at (x, y)
            Uint32* pixel = (Uint32*)((Uint8*)surface->pixels + y * surface->pitch + x * sizeof(Uint32));

            // Extract the color components (RGBA)
            Uint8 r, g, b, a;
            SDL_GetRGBA(*pixel, format, &r, &g, &b, &a);

            // Check if the pixel's color matches the target color
            if (r == R && g == G && b == B) {
                // Modify the color (replace with your desired color)
                Uint8 newR = 255;
                Uint8 newG = 0;
                Uint8 newB = 0;

                // Set the new pixel value
                *pixel = SDL_MapRGBA(format, newR, newG, newB, a);
            }
        }
    }

    // Unlock the surface
    SDL_UnlockSurface(surface);
}



int main() {
    int is_game_running = 0;

    int SCREEN_WIDTH = 640; // 1280 // 640
    int SCREEN_HEIGHT = 480; // 720 // 480

    // create a window
    SDL_Window *window = create_window("hey :)", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);
    SDL_Surface *window_surface = create_window_surface(window);

    is_game_running = 1;

    // load all "therock" images into an array
    SDL_Surface *images[8];

    images[0] = load_bmp_surface("resources/therock.bmp");
    images[1] = load_bmp_surface("resources/therockblue.bmp");
    images[2] = load_bmp_surface("resources/therockgreen.bmp");
    images[3] = load_bmp_surface("resources/therockpurple.bmp");
    images[4] = load_bmp_surface("resources/therockdarkblue.bmp");
    images[5] = load_bmp_surface("resources/therockpink.bmp");
    images[6] = load_bmp_surface("resources/therockturquoise.bmp");
    images[7] = load_bmp_surface("resources/therockyellow.bmp");

    // create surface for "therock" images
    SDL_Surface *image_surface = images[0];

    SDL_Rect rect_surface;
    rect_surface.x = 0;
    rect_surface.y = 0;

    // load vineboom.wav
    SDL_AudioSpec wav_spec;
    Uint32 wav_length;
    Uint8 *wav_buffer;
 
    SDL_LoadWAV("resources/vineboom.wav", &wav_spec, &wav_buffer, &wav_length);

    // open audio device (speakers, headphones, etc)
    SDL_AudioDeviceID device_id = SDL_OpenAudioDevice(NULL, 0, &wav_spec, NULL, 0);

    // these variables are for "therock"
    int speed = 2;
    int dir_x = 1;
    int dir_y = -1;

    // init random number stuffs
    srand(time(NULL));

    while (is_game_running == 1) {
        SDL_Event e;
        while(SDL_PollEvent(&e) > 0) {
            if (e.type == SDL_QUIT) {
                is_game_running = 0;
            } else if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_RESIZED) {
                SDL_GetWindowSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);
                rect_surface.x = 0;
                rect_surface.y = 0;
            }
        }

        // boundary checking
        if ((rect_surface.x+rect_surface.w) >= SCREEN_WIDTH || rect_surface.x <= 0) {
            dir_x *= -1;
            image_surface = images[rand() % 8];

            // clear audio queue
            SDL_ClearQueuedAudio(device_id);

            // play audio
            SDL_QueueAudio(device_id, wav_buffer, wav_length);
            SDL_PauseAudioDevice(device_id, 0);
        } else if ((rect_surface.y+rect_surface.h) >= SCREEN_HEIGHT || rect_surface.y <= 0) {
            dir_y *= -1;
            image_surface = images[rand() % 8];

            // clear audio queue
            SDL_ClearQueuedAudio(device_id);

            // play audio
            SDL_QueueAudio(device_id, wav_buffer, wav_length);
            SDL_PauseAudioDevice(device_id, 0);
        }

        // apply values to x and y of therock
        rect_surface.x += dir_x * speed;
        rect_surface.y += dir_y * speed;

        // fill background, resize rock based on window size, and update window
        window_surface = SDL_GetWindowSurface(window);
        SDL_FillRect(window_surface, NULL, SDL_MapRGB(window_surface->format, 0, 0, 0));
        SDL_BlitSurface(image_surface, NULL, window_surface, &rect_surface);
        SDL_UpdateWindowSurface(window);
        SDL_Delay(16);
    }

    SDL_CloseAudioDevice(device_id);
    SDL_FreeWAV(wav_buffer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

