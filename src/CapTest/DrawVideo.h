//����ͼ����
//JC 2013-07-31
#ifndef AVLIB_DRAWVIDEO_H_
#define AVLIB_DRAWVIDEO_H_
#include <Vfw.h>
#include <vector>

class CDrawVideo
{
public:
  std::vector<RECT> rects;
  std::vector<POINT> points;
//Attributes
protected:
	HWND m_hWnd;
	HDRAWDIB m_hDib;
	BITMAPINFO bih;
	CRect m_rect;
	BOOL m_bUseGDI;
public:
	CDrawVideo();
	~CDrawVideo();
	BOOL Init(HWND hWnd);
	void Release(){}
	//����RGB����  ���ݳ���Ϊwidth*height*3
	void Draw(BYTE *pBuf,int width, int height, LPCTSTR szText=NULL);
};

#endif //AVLIB_DRAWVIDEO_H_