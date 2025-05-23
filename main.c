/*
 * PSP Software Development Kit - https://github.com/pspdev
 * -----------------------------------------------------------------------
 * Licensed under the BSD license, see LICENSE in PSPSDK root for details.
 *
 * Copyright (c) 2005 Jesper Svennevid
 */

#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspdebug.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include <pspgu.h>
#include <pspgum.h>

#include "callbacks.h"

PSP_MODULE_INFO("gu Triangle", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);

static unsigned int __attribute__((aligned(16))) list[262144];

struct Vertex
{
  unsigned int color;
  float x,y,z;
};

struct Vertex __attribute__((aligned(16))) vertices[3] =
{
  {0xff0000ff,-0.5, -0.5,0}, // 0
  {0xff00ff00, 0.5, -0.5,0}, // 1
  {0xffff0000, 0.0,  0.5,0}, // 2
};

#define BUF_WIDTH (512)
#define SCR_WIDTH (480)
#define SCR_HEIGHT (272)

int main(int argc, char* argv[])
{
  setupCallbacks();

  // setup GU

  void* fbp0 = guGetStaticVramBuffer(BUF_WIDTH,SCR_HEIGHT,GU_PSM_8888);
  void* fbp1 = guGetStaticVramBuffer(BUF_WIDTH,SCR_HEIGHT,GU_PSM_8888);
  void* zbp = guGetStaticVramBuffer(BUF_WIDTH,SCR_HEIGHT,GU_PSM_4444);

  sceGuInit();

  sceGuStart(GU_DIRECT,list);
  sceGuDrawBuffer(GU_PSM_8888,fbp0,BUF_WIDTH);
  sceGuDispBuffer(SCR_WIDTH,SCR_HEIGHT,fbp1,BUF_WIDTH);
  sceGuDepthBuffer(zbp,BUF_WIDTH);
  sceGuOffset(2048 - (SCR_WIDTH/2),2048 - (SCR_HEIGHT/2));
  sceGuViewport(2048,2048,SCR_WIDTH,SCR_HEIGHT);
  sceGuDepthRange(65535,0);
  sceGuScissor(0,0,SCR_WIDTH,SCR_HEIGHT);
  sceGuEnable(GU_SCISSOR_TEST);
  // sceGuDepthFunc(GU_GEQUAL);
  // sceGuEnable(GU_DEPTH_TEST);
  // sceGuFrontFace(GU_CW);
  sceGuShadeModel(GU_SMOOTH);
  // sceGuEnable(GU_CULL_FACE);
  // sceGuEnable(GU_TEXTURE_2D);
  sceGuEnable(GU_CLIP_PLANES);
  sceGuFinish();
  sceGuSync(GU_SYNC_FINISH, GU_SYNC_WHAT_DONE);

  sceDisplayWaitVblankStart();
  sceGuDisplay(GU_TRUE);

  // run sample

  int val = 0;

  while(running())
  {
    sceGuStart(GU_DIRECT,list);

    // clear screen

    sceGuClearColor(0xff554433);
    sceGuClearDepth(0);
    sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);

    // setup matrices for cube

    sceGumMatrixMode(GU_PROJECTION);
    sceGumLoadIdentity();
    sceGumPerspective(75.0f,16.0f/9.0f,0.5f,1000.0f);

    sceGumMatrixMode(GU_VIEW);
    sceGumLoadIdentity();

    sceGumMatrixMode(GU_MODEL);
    sceGumLoadIdentity();
    {
      ScePspFVector3 pos = { 0, 0, -1.5f };
      ScePspFVector3 rot = { 0, val * 0.98f * (GU_PI/180.0f), 0 };
      sceGumTranslate(&pos);
      sceGumRotateXYZ(&rot);
    }


    // draw triangle
    sceGumDrawArray(GU_TRIANGLES,GU_COLOR_8888|GU_VERTEX_32BITF|GU_TRANSFORM_3D,3,0,vertices);

    sceGuFinish();
    sceGuSync(GU_SYNC_FINISH, GU_SYNC_WHAT_DONE);

    sceDisplayWaitVblankStart();
    sceGuSwapBuffers();

    val++;
  }

  sceGuTerm();

  sceKernelExitGame();
  return 0;
}
