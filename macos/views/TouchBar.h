//
//  TouchBar.h
//  mkxp-z
//
//  Created by ゾロア on 1/14/22.
//

#ifndef MKXPZ_TOUCHBAR_H
#define MKXPZ_TOUCHBAR_H

#include <stdio.h>
#include <SDL_events.h>
#include <SDL_video.h>

#include "config.h"

#ifdef __OBJC__
API_AVAILABLE(macos(10.12.2))
@interface MKXPZTouchBar : NSTouchBar <NSTouchBarDelegate>
+(MKXPZTouchBar *)sharedTouchBar;
@end
#endif

void initTouchBar(SDL_Window *win, Config &conf);
void updateTouchBarFPSDisplay(uint32_t value);

#endif // MKXPZ_TOUCHBAR_H
