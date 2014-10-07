#include "joystickiohandler.h"


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
    qDebug()<<"Gamepad disconnected";
}

void JoystickIOHandler::onAxisChange(const SDL_ControllerAxisEvent sdlEvent)
{
    qDebug()<<"Axis "<<(int)sdlEvent.axis<<" changed: "<<sdlEvent.value;
}

void JoystickIOHandler::onButtonChange(const SDL_ControllerButtonEvent sdlEvent)
{
    qDebug()<<"Button "<<(int)sdlEvent.button<<" changed: "<<(int)sdlEvent.state;
}

void JoystickIOHandler::run()
{
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
        }
    }
}

JoystickIOHandler::JoystickIOHandler()
{
    done = false;

    SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER);

    this->start();
}

JoystickIOHandler::~JoystickIOHandler()
{
    done = true;
    this->wait();

    if(SDL_WasInit(SDL_INIT_GAMECONTROLLER))
        SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER);

    SDL_Quit();
}
