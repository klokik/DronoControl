#ifndef JOYSTICKIOHANDLER_H
#define JOYSTICKIOHANDLER_H

#include <SDL2/SDL.h>

#include <QtCore>

class JoystickIOHandler: public QThread
{
    Q_OBJECT

protected:
    SDL_GameController *pad;

    int16_t axis_val[10] = {0};
    int16_t throttle = -32768;

    void onControllerAdd(const SDL_ControllerDeviceEvent sdlEvent);
    void onControllerRemove(const SDL_ControllerDeviceEvent sdlEvent);
    void onAxisChange(const SDL_ControllerAxisEvent sdlEvent);
    void onButtonChange(const SDL_ControllerButtonEvent sdlEvent);

    // sdl Event Loop
    void run();

signals:
    void AxisChange(QVariant id,QVariant x,QVariant y);
    void ButtonChange(QVariant id,QVariant state,QVariant is_button);

public:
    JoystickIOHandler();

    ~JoystickIOHandler();
};

#endif // JOYSTICKIOHANDLER_H
