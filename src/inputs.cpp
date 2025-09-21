#include <SDL3/SDL.h>
#include <stdio.h>

#include "main.h"

/* #### Gives out the pressed key
   A function for using the keyboard as a "controller"

   outputs: (what they would be on QWERTY)
    - -1, -2, -3, -4: ^, <, v, >
    - 0: Not defined
    - 1, 2, 3, 4: W, A, S, D
    - 5: [Space]
    - 6: E
    - 7: Q
*/
//int controls_keyboard(const SDL_Event& e) {  // FIXME make coustom keybinds
//    if (e.type == SDL_EVENT_KEY_DOWN) {
//        switch (e.key.scancode) {
//            case SDL_SCANCODE_W:
//                return 1;
//            case SDL_SCANCODE_A:
//                return 2;
//            case SDL_SCANCODE_S:
//                return 3;
//            case SDL_SCANCODE_D:
//                return 4;
//            default:
//                return 0;
//        }
//    }
//    return 0;
//}