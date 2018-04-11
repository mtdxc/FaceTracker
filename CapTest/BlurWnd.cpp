#include "stdafx.h"
#include "BlurWnd.h"

#include <stdint.h>
// ��ά���������
class Mat2{
  uint8_t* addr;
  int width_;
  int height_;
  int plane_;
  uint8_t bytes_;
public:
  Mat2(){ Clear(); }
  void Clear(){ addr = NULL; width_ = height_ = plane_ = bytes_ = 0; }
  bool Init(uint8_t* base, int width, int height, int plan = 0, int bytes = 3){
    if (!base){
      Clear();
    }
    else if (width > 0 && height > 0 && bytes <= 4){
      addr = base;
      width_ = width;
      height_ = height;
      bytes_ = bytes;
      if (plan){
        ASSERT(plan >= width*bytes);
        plane_ = plan;
      }
      else{
        plane_ = width * bytes;
      }
    }
    else{
      // args error
      return false;
    }
    return true;
  }
  int width() const { return width_; }
  int height() const { return height_; }
  int plane() const { return plane_; }

  uint8_t* at(int x, int y){
    //ASSERT( x>=0 && x < width_);
    //ASSERT( y>=0 && y < height_);
    if(x < width_ && y < height_)
      return addr + y * plane_ + x * bytes_;
    return NULL;
  }
  uint32_t get(int x, int y){
    uint32_t val = 0;
    uint8_t* p = at(x, y);
    if (p){
      uint8_t* r = (uint8_t*)&val;
      memcpy(r, p, bytes_);
    }
    return val;
  }
  bool set(int x, int y, uint32_t val){
    uint8_t* p = at(x, y);
    if (!p) return false;
    uint8_t* r = (uint8_t*)&val;
    memcpy(p, r, bytes_);
    return true;
  }
};

CBlurWnd::CBlurWnd()
{
}


CBlurWnd::~CBlurWnd()
{
}

void MSK(Mat2& mat, int r){
  for (int y = 0; y < mat.height(); y += r){
    for (int x = 0; x < mat.width(); x += r){
      uint32_t val = mat.get(x + r * rand() / RAND_MAX, y + r * rand() / RAND_MAX);
      // if ((y + r) > mat.height() || (x + r)>mat.height()) continue;
      for (int j = y; j < y + r; j++){
        for (int k = x; k < x + r; k++){
          mat.set(k, j, val);
        }
      }
    }
  }
}

void MBL(Mat2& mat, int r){
  for (int y = 0; y < mat.height() - r; y++){
    for (int x = 0; x < mat.width() - r; x++){
      uint32_t val = mat.get(x + r * rand() / RAND_MAX, y + r * rand() / RAND_MAX);
      mat.set(x, y, val);
    }
  }
}

#include "FaceTracker/ofxFaceTracker.h"
BOOL CBlurWnd::FillBuffer(BYTE* pRgb, int width, int height)
{
  BOOL bRet = FALSE;
  //cv::Mat img(height, width, CV_8UC3, pRgb, width*3);
  //return CVideoWnd::FillBuffer(pRgb, width, height, m_nDefOrgin);
  if (BYTE* pDst = GetBuffer(width, height)){
    memcpy(pDst, pRgb, width*height * 3);
    if (m_rcTacker && m_rcTacker.Width() > 3){
      CRect rc; GetClientRect(&rc);
      float scalex = width * 1.0 / rc.Width();
      float scaley = height * 1.0 / rc.Height();
      rc = m_rcTacker;
      rc.left *= scalex;
      rc.right *= scalex;
      rc.bottom *= scaley;
      rc.top *= scaley;
      Mat2 mat;
      mat.Init(pDst + 3 * (rc.left + rc.top * width),
        rc.Width(), rc.Height(), width * 3);
      //MSK(mat, 8);
      //MBL(mat, 8);
      Rgb24Blur(pDst + 3*(rc.left+ rc.top * width), rc.Width(), rc.Height(), width * 3, 
        2, 13, 0, 32, 32);
    }
    ReleaseBuffer();
    bRet = TRUE;
  }
  return bRet;
}

