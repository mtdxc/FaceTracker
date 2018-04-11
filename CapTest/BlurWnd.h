#pragma once
#include "VideoWnd.h"

class CBlurWnd :
  public CVideoWnd
{
  int type_;
  int arg1_, arg2_;
  double arg3_, arg4_;
  // 统计信息
  DWORD m_tick;
  int m_nFrame;
public:
  CBlurWnd();
  ~CBlurWnd();

  BOOL FillBuffer(BYTE* pRgb, int width, int height);
  BOOL SetBlurArgs(int type, int arg1, int arg2, double arg3, double arg4){
    type_ = type;
    arg1_ = arg1;
    arg2_ = arg2;
    arg3_ = arg3;
    arg4_ = arg4;
    Invalidate();
    return true;
  }

};

