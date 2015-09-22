#include <algorithm>

#include "joystickiohandler.h"

#include <QtCore>


void JoystickIOHandler::onControllerAdd(const SDL_ControllerDeviceEvent sdlEvent)
{
    auto id = sdlEvent.which;

    if(SDL_IsGameController(id))
    {
        pad = SDL_GameControllerOpen(id);

        if(pad)
        {
            SDL_Joystick *joy = SDL_GameControllerGetJoystick(pad);
            int instanceID = SDL_JoystickInstanceID(joy);

            qDebug()<<"Joystick added: id"<<instanceID<<" "<<SDL_GameControllerName(pad);
        }
    }
}

void JoystickIOHandler::onControllerRemove(const SDL_ControllerDeviceEvent sdlEvent)
{
    SDL_GameControllerClose(pad);
    qDebug()<<"Gamepad id"<<sdlEvent.which<<" disconnected";
}

void JoystickIOHandler::onAxisChange(const SDL_ControllerAxisEvent sdlEvent)
{
//    qDebug()<<"Axis "<<(int)sdlEvent.axis<<" changed: "<<sdlEvent.value;

    int16_t old_3 = axis_val[3];

    axis_val[(size_t)sdlEvent.axis] = sdlEvent.value;

    float x;
    float y;
    int id;

    switch(sdlEvent.axis)
    {
    case 0:
    case 1:
        x = axis_val[0];
        y = -axis_val[1];
        id = 0;
        break;
    case 2:
    case 3:
        x = axis_val[5] - axis_val[4];
        if((axis_val[3]-old_3) * axis_val[3] > 0) // going forward
            throttle = std::min(std::max(throttle - (axis_val[3]-old_3)*2,-32768),32767);
        y = throttle;
        id = 1;
        break;
    case 4:
    case 5:
        x = axis_val[5] - axis_val[4];
        y = throttle;
        id = 1;
        break;
    }

    emit AxisChange(id,x/32768.0f,y/32768.0f);
}

void JoystickIOHandler::onButtonChange(const SDL_ControllerButtonEvent sdlEvent)
{
//    qDebug()<<"Button "<<(int)sdlEvent.button<<" changed: "<<(int)sdlEvent.state;

    if((int)sdlEvent.button == 5)
    {
        qDebug()<<"Exit";
        exit(0);
    }
    emit ButtonChange((int)sdlEvent.button,sdlEvent.state?"high":"low",true);
}

void JoystickIOHandler::run()
{
    bool done = false;
    SDL_Event event;

    qDebug()<<"Waiting for gamepad input events";
    while(!done&&SDL_WaitEvent(&event))
    {
        switch(event.type)
        {
        case SDL_CONTROLLERDEVICEADDED:
            onControllerAdd(event.cdevice);
            break;
        case SDL_CONTROLLERDEVICEREMOVED:
            onControllerRemove(event.cdevice);
            break;
        case SDL_CONTROLLERBUTTONDOWN:
        case SDL_CONTROLLERBUTTONUP:
            onButtonChange(event.cbutton);
            break;
        case SDL_CONTROLLERAXISMOTION:
            onAxisChange(event.caxis);
            break;

        case SDL_QUIT:
            done = true;
            break;
        }
    }
}

JoystickIOHandler::JoystickIOHandler()
{
    SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER);

    this->start();
}

JoystickIOHandler::~JoystickIOHandler()
{
    SDL_Event quit_event;
    quit_event.type = SDL_QUIT;
    SDL_PushEvent(&quit_event);
    this->wait();

    if(SDL_WasInit(SDL_INIT_GAMECONTROLLER))
        SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER);

    SDL_Quit();
}
