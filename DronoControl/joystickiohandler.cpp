//#include "joystickiohandler.h"

//JoystickIOHandler::JoystickIOHandler()
//{
//    SDL_Init(SDL_INIT_GAMECONTROLLER);
//}

//void JoystickIOHandler::addController(int id)
//{
//    if(SDL_IsGameController(id))
//    {
//        SDL_GameController *pad = SDL_GameControllerOpen(id);

//        if(pad)
//        {
//            SDL_Joystick *joy = SDL_GameControllerGetJoystick(pad);
//            int instanceID = SDL_JoystickInstanceID(joy);

//            // You can add to your own map of joystick IDs to controllers here.
//            YOUR_FUNCTION_THAT_CREATES_A_MAPPING(id,pad);
//        }
//    }
//}

//void JoystickIOHandler::removeController(int id)
//{
//    SDL_GameController *pad = YOUR_FUNCTION_THAT_RETRIEVES_A_MAPPING(id);
//    SDL_GameControllerClose(pad);
//}

//void JoystickIOHandler::onControllerButton(const SDL_ControllerButtonEvent sdlEvent)
//{
//    // Button presses and axis movements both sent here as SDL_ControllerButtonEvent structures
//}

//void JoystickIOHandler::onControllerAxis(const SDL_ControllerAxisEvent sdlEvent)
//{
//    // Axis movements and button presses both sent here as SDL_ControllerAxisEvent structures
//}

//void JoystickIOHandler::eventLoop()
//{
//    SDL_Event sdlEvent;

//    while(SDL_PollEvent(&sdlEvent))
//    {
//        switch( sdlEvent.type )
//        {
//        case SDL_CONTROLLERDEVICEADDED:
//            addController(sdlEvent.cdevice);
//            break;

//        case SDL_CONTROLLERDEVICEREMOVED:
//            removeController(sdlEvent.cdevice);
//            break;

//        case SDL_CONTROLLERBUTTONDOWN:
//        case SDL_CONTROLLERBUTTONUP:
//            onControllerButton(sdlEvent.cbutton);
//            break;

//        case SDL_CONTROLLERAXISMOTION:
//            onControllerAxis(sdlEvent.caxis);
//            break;
//        // YOUR OTHER EVENT HANDLING HERE
//        }
//    }
//}
