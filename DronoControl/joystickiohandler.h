#ifndef JOYSTICKIOHANDLER_H
#define JOYSTICKIOHANDLER_H

#include <SDL2/SDL.h>

#include <QtCore>

class JoystickIOHandler: public QThread
{
    Q_OBJECT

protected:
    SDL_GameController *pad;
    bool done;

    void onControllerAdd(const SDL_ControllerDeviceEvent sdlEvent);
    void onControllerRemove(const SDL_ControllerDeviceEvent sdlEvent);
    void onAxisChange(const SDL_ControllerAxisEvent sdlEvent);
    void onButtonChange(const SDL_ControllerButtonEvent sdlEvent);

    // sdl Event Loop
    void run();

public:
    JoystickIOHandler();

    ~JoystickIOHandler();
};

#endif // JOYSTICKIOHANDLER_H
