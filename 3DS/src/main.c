#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <setjmp.h>
#include <3ds.h>
#include <arpa/inet.h>
#include <sys/dirent.h>
#include <sys/errno.h>
#include <sys/unistd.h>
#include <fcntl.h>
#include <stdbool.h>

#include "drawing.h"
#include "inet_pton.h"
#include "network.h"

static jmp_buf exitJmp;

void hang(char *message) {
    while (aptMainLoop()) {
        hidScanInput();

        clearScreen();
        drawString(10, 10, "%s", message);
        drawString(10, 20, "Press start to exit");

        u32 kHeld = hidKeysHeld();
        if (kHeld & KEY_START) longjmp(exitJmp, 1);

        gfxFlushBuffers();
        gspWaitForVBlank();
        gfxSwapBuffers();
    }
}

void cleanup() {
    enableBacklight();
    SOC_Shutdown();
    //svcCloseHandle(fileHandle);
    fsExit();
    gfxExit();
    acExit();
}

void displayMessage(int x, int y, char *msg) {
    clearScreen();
    drawString(x, y, msg);
    gfxFlushBuffers();
    gfxSwapBuffers();
}

void drawIP(int x, int y, int clear) {
    if(clear) {
        clearScreen();
    }
    char buffer[64];
    struct in_addr in;
    in.s_addr = gethostid();
    sprintf(buffer, "3DS IP: %s", inet_ntoa(in));
    drawString(10, 10, buffer);
    if(clear) {
        gfxFlushBuffers();
        gfxSwapBuffers();
    }
}

int main() {
    // Initializations
    acInit();
    gfxInitDefault();

    // Enable double buffering to remove screen tearing
    gfxSetDoubleBuffering(GFX_TOP, true);
    gfxSetDoubleBuffering(GFX_BOTTOM, true);

    // Save current stack frame for easy exit
    if(setjmp(exitJmp)) {
        cleanup();
        return 0;
    }

    // Set up file system
    displayMessage(10, 10, "Initializing file system");
    fsInit();

    // Set up Wi-Fi
    displayMessage(10, 10, "Initializing SOC");
    SOC_Initialize((u32*) memalign(0x1000, 0x100000), 0x100000);

    u32 wifiStatus = 0;
    ACU_GetWifiStatus(NULL, &wifiStatus);
    if(!wifiStatus) {
        hang("Wi-Fi is not enabled or not connected");
    }

    inet_pton4("10.101.3.155", (unsigned char *)&(saout.sin_addr));
    displayMessage(10, 10, "Connecting to server");
    if(!openSocket(50000)) {
        hang("Failed to connect!");
    }

    drawIP(10, 10, true);

    u32 kDown;
    u32 kHeld;
    //u32 kUp;
    circlePosition circlePad;
    circlePosition cStick;
    touchPosition touch;

    // Main loop
    while (aptMainLoop()) {

        // Read which buttons are currently pressed or not
        hidScanInput();
        kDown = hidKeysDown();
        kHeld = hidKeysHeld();
        //kUp = hidKeysUp();

        // Read circle pad and c stick positions
        hidCstickRead(&cStick);
        hidCircleRead(&circlePad);
        hidTouchRead(&touch);

        // If START button is pressed, break loop and quit
        if (kDown & KEY_START) {
            break;
        }

        clearScreen();
        drawIP(10, 10, false);

        if (kHeld & KEY_A) {
            drawString(20, 20, "A");
        }
        if (kHeld & KEY_B) {
            drawString(35, 20, "B");
        }
        if (kHeld & KEY_X) {
            drawString(50, 20, "X");
        }
        if (kHeld & KEY_Y) {
            drawString(65, 20, "Y");
        }
        if (kHeld & KEY_L) {
            drawString(80, 20, "L");
        }
        if (kHeld & KEY_R) {
            drawString(95, 20, "R");
        }
        if (kHeld & KEY_ZL) {
            drawString(110, 20, "ZL");
        }
        if (kHeld & KEY_ZR) {
            drawString(135, 20, "ZR");
        }

        if (kHeld & KEY_DUP) {
            drawString(20, 30, "UP");
        }
        if (kHeld & KEY_DDOWN) {
            drawString(45, 30, "DOWN");
        }
        if (kHeld & KEY_DLEFT) {
            drawString(90, 30, "LEFT");
        }
        if (kHeld & KEY_DRIGHT) {
            drawString(135, 30, "RIGHT");
        }

        char buffer[50];
        drawString(20, 50, itoa(circlePad.dx, buffer, 10));
        drawString(55, 50, itoa(circlePad.dy, buffer, 10));
        drawString(20, 60, itoa(cStick.dx, buffer, 10));
        drawString(55, 60, itoa(cStick.dy, buffer, 10));

        if(touch.px && touch.py) {
            drawBox(touch.px - 2, touch.py - 2, 5, 5, 255, 255, 255);
        }

        // Reset framebuffers
        //fbTopLeft = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);
        //fbTopRight = gfxGetFramebuffer(GFX_TOP, GFX_RIGHT, NULL, NULL);
        //fbBottom = gfxGetFramebuffer(GFX_BOTTOM, 0, NULL, NULL);
        //memset(fbTopLeft, 0, 240 * 400 * 3);
        //memset(fbTopRight, 0, 240 * 400 * 3);
        //memset(fbBottom, 0, 240 * 320 * 3);

        sendKeys(kHeld, circlePad, cStick, touch);

        // Flush and swap framebuffers
        gfxFlushBuffers();
        gspWaitForVBlank();
        gfxSwapBuffers();
    }

    // Exit
    cleanup();

    // Return to hbmenu
    return 0;
}
