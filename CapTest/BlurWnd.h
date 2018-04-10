#pragma once
#include "VideoWnd.h"

class CBlurWnd :
  public CVideoWnd
{
public:
  CBlurWnd();
  ~CBlurWnd();

  BOOL FillBuffer(BYTE* pRgb, int width, int height);
};

