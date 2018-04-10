// VideoWnd.cpp : implementation file
//

#include "stdafx.h"
#include "VideoWnd.h"
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const static int dpixeladd = 24 / 8;
inline int GetDIBColor(int nWidth,int nHeight,byte *Buffer,int X, int Y)
{
	BYTE *pRgb=Buffer+ dpixeladd * (X + (nHeight - 1 - Y) * nWidth);
	return RGB(pRgb[2],pRgb[1],pRgb[0]);
}

inline int SetDIBColor(int nWidth,int nHeight,byte *Buffer, int X, int Y,int value)
{
	BYTE *ptr=Buffer+ dpixeladd*(X + (nHeight - 1 - Y) * nWidth);
	*ptr = value>>16;
	*(ptr + 1)=value>>8;
	*(ptr + 2)=value&0xFF;
	return 1;
}

VOID RGBRotate90(BYTE *pSrc,BYTE* pDst, int &nHeight,int& nWidth) 
{
	for(int w=0;w<nWidth;w++)
	{
		for(int h=0;h<nHeight;h++)
		{
			int SrcR=GetDIBColor(nWidth,nHeight,pSrc,w,h);
			SetDIBColor(nHeight,nWidth,pDst,nHeight-1-h,w,SrcR);
		}
	}
	std::swap(nHeight, nWidth);
}

VOID RGBRotate180_(BYTE *pSrc,BYTE* pDst, int nHeight,int nWidth) 
{
	memcpy(pDst, pSrc, nHeight * nWidth * 3);
	for(int h=0;h<nHeight/2;h++)  //(nHeight/2)
	{
		for( int w=0;w<nWidth;w++)//nWidth
		{
			int SrcR=GetDIBColor(nWidth,nHeight,pDst,w,h);
			int DesR=GetDIBColor(nWidth,nHeight,pDst,nWidth-w-1,nHeight-h-1);
			SetDIBColor(nWidth,nHeight,pDst,w,h,DesR);	
			SetDIBColor(nWidth,nHeight,pDst,nWidth-w-1,nHeight-h-1,SrcR);
		} 
	}
}

VOID RGBRotate180(BYTE *pSrc,BYTE* pDst, int nHeight,int nWidth) 
{
	for(int h=0;h<nHeight;h++)
	{
		for( int w=0;w<nWidth;w++)//nWidth
		{
			int SrcR=GetDIBColor(nWidth,nHeight,pSrc,w,h);
			SetDIBColor(nWidth,nHeight,pDst,nWidth-w-1,nHeight-h-1,SrcR);
		} 
	}
}

VOID RGBRotate270(BYTE *pSrc,BYTE* pDst,int &nHeight,int &nWidth) 
{
	for(int w=0;w<nWidth;w++)
	{
		for(int h=0;h<nHeight;h++)
		{
			int SrcR=GetDIBColor(nWidth,nHeight,pSrc,w,h);
			SetDIBColor(nHeight,nWidth,pDst,h,nWidth-w-1,SrcR);
		}
	}
	std::swap(nHeight, nWidth);
}

CVideoBuffer::CVideoBuffer()
{
	m_buf=NULL;
	m_w=0;
	m_h=0;
}

CVideoBuffer::~CVideoBuffer()
{
	CleanUpBuffer();
}

//分配符合视频大小的内存
BYTE *CVideoBuffer::GetBuffer(int w,int h)
{
	m_mutex.Lock();
	void *ptr=NULL;
	if(w==0||(w==m_w&&h==m_h))
	{
		//已经分配了要求大小的内存
		ptr=m_buf;
		goto RET;
	}
	if(m_buf)
	{
		//
		ptr=realloc(m_buf,w*h*3);		
		if(ptr==NULL)
		{
			CleanUpBuffer();
		}
	}
	else
	{
		ptr=malloc(w*h*3);
	}
	if(ptr==NULL)
		goto RET;
	
	m_buf=ptr;
	
	m_w=w;
	m_h=h;
	
	memset(m_buf,0,w*h*3);	

RET:
	if(!ptr)
		m_mutex.Unlock();
	return (BYTE*)ptr;
}

void CVideoBuffer::ReleaseBuffer()
{
	if(m_buf)
		m_mutex.Unlock();
}

void CVideoBuffer::CleanUpBuffer()
{
	m_mutex.Lock();
	if(m_buf)
	{
		free(m_buf);	
		m_buf=NULL;
		m_w=0;
		m_h=0;
	}
	m_mutex.Unlock();
}


/////////////////////////////////////////////////////////////////////////////
// CVideoWnd

CVideoWnd::CVideoWnd()
{	
	memset(&m_bmi,0,sizeof(m_bmi));
	m_bmi.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
	m_bmi.bmiHeader.biPlanes=1;	
	m_bmi.bmiHeader.biBitCount=24;
	m_bSizeChanged=FALSE;
	m_bFit = TRUE;
	m_nDefOrgin = 0;
	m_clrBK = 0;
	m_bUseGDI=FALSE;
	m_nTextFmt = DT_BOTTOM|DT_SINGLELINE;
	m_hDrawDIB = NULL;
  m_bTracker = FALSE;
}

CVideoWnd::~CVideoWnd()
{
	if(m_hDrawDIB)
		DrawDibClose(m_hDrawDIB);
}


BEGIN_MESSAGE_MAP(CVideoWnd, CWnd)
	//{{AFX_MSG_MAP(CVideoWnd)
  ON_WM_ERASEBKGND()
  ON_WM_LBUTTONDBLCLK()
  ON_WM_CREATE()
	//}}AFX_MSG_MAP
  ON_WM_LBUTTONDOWN()
  ON_WM_MOUSEMOVE()
  ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CVideoWnd message handlers

BOOL CVideoWnd::OnEraseBkgnd(CDC* pDC) 
{
	BYTE *pBuffer=m_buffer.GetBuffer();
	if(pBuffer)
	{
		if(m_bSizeChanged)
		{
			m_bSizeChanged=FALSE;
			CRect rcWnd,rcClient;
			GetWindowRect(rcWnd); 
			GetClientRect(rcClient);
			//Make the client area fit video size
			SetWindowPos(0,0,0,
				m_bmi.bmiHeader.biWidth+rcWnd.Width()-rcClient.Width(),
				m_bmi.bmiHeader.biHeight+rcWnd.Height()-rcClient.Height(),SWP_NOMOVE);
		}
		else
		{
			COLORREF clr = m_clrBK;//pDC->GetBkColor();
			CRect rc;
			GetClientRect(&rc);
			if(!m_bFit){
				float fRate = m_bmi.bmiHeader.biWidth * 1.0 / m_bmi.bmiHeader.biHeight;
				int newW = rc.Height() * fRate;
				if(newW>rc.Width()){
					int newH = rc.Width() / fRate;
					int oldH = rc.Height();
					rc.top = (oldH - newH)/2;
					rc.bottom = rc.top + newH;
					pDC->FillSolidRect(0, 0, rc.Width(), rc.top, clr);
					pDC->FillSolidRect(0, rc.bottom, rc.Width(), rc.top, clr);
				}else{
					int oldW = rc.Width();
					rc.left = (oldW-newW)/2;
					rc.right = rc.left + newW;
					pDC->FillSolidRect(0, 0, rc.left, rc.Height(), clr);
					pDC->FillSolidRect(rc.right, 0, rc.left, rc.Height(), clr);
				}
			}
			if(m_bUseGDI){
				HDC hDC = pDC->GetSafeHdc();
				int oldMode = SetStretchBltMode(hDC, COLORONCOLOR);

				StretchDIBits(hDC, rc.left, rc.top, rc.Width(), rc.Height(), 
					0,0,m_bmi.bmiHeader.biWidth,m_bmi.bmiHeader.biHeight, 
					pBuffer, &m_bmi, DIB_RGB_COLORS, SRCCOPY);
				/*
				HDC hMemDc = CreateCompatibleDC(hDC);
				HBITMAP hBitmap = CreateDIBitmap(hDC, &m_bmi.bmiHeader, CBM_INIT, pBuffer, (BITMAPINFO*)&m_bmi, DIB_RGB_COLORS);
				HGDIOBJ hOldBmp = SelectObject(hMemDc, hBitmap);
				StretchBlt(hDC, rc.left, rc.top, rc.Width(), rc.Height(), 
					hMemDc, 0, 0, m_bmi.bmiHeader.biWidth,m_bmi.bmiHeader.biHeight, SRCCOPY);
				DeleteObject(SelectObject(hMemDc, hOldBmp));
				DeleteDC(hMemDc);
				*/
				SetStretchBltMode(hDC, oldMode);
			}else
			DrawDibDraw(m_hDrawDIB,pDC->GetSafeHdc(),
				rc.left,rc.top,rc.Width(),rc.Height(),
				&m_bmi.bmiHeader,pBuffer,
				0,0,m_bmi.bmiHeader.biWidth,m_bmi.bmiHeader.biHeight,
				DDF_NOTKEYFRAME);
			if(m_szText.GetLength()){ 
				pDC->SetBkMode(TRANSPARENT);
				clr = pDC->SetTextColor(m_clrText);
				pDC->DrawText(m_szText, rc, m_nTextFmt);
				pDC->SetTextColor(clr);
			}
		}
		m_buffer.ReleaseBuffer();
	}
	else
		CWnd::OnEraseBkgnd(pDC);

  if (m_rcTacker){
    HGDIOBJ pen = CreatePen(PS_DASH, 3, RGB(255, 0, 0));
    HGDIOBJ oldBush = pDC->SelectObject(GetStockObject(NULL_BRUSH));
    HGDIOBJ oldpen = pDC->SelectObject(pen);
    pDC->Rectangle(m_rcTacker);
    /*
    for (auto rc : rects) {
    //FrameRect(hDC, &rc, bush);
    rc.left *= xscale; rc.right *= xscale;
    rc.top *= yscale; rc.bottom *= yscale;
    Rectangle(hDC, rc.left, rc.top, rc.right, rc.bottom);
    }
    */
    pDC->SelectObject(oldpen);
    pDC->SelectObject(oldBush);
    DeleteObject(pen);
  }
  return TRUE;
}

BOOL CVideoWnd::Create(CWnd *pParentWnd, DWORD dwExStyle, LPCTSTR lpszWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight)
{
	CString ClassName;
	ClassName=AfxRegisterWndClass(
			CS_VREDRAW | CS_HREDRAW|CS_DBLCLKS,
			::LoadCursor(NULL, IDC_ARROW),
			(HBRUSH) ::GetStockObject(BLACK_BRUSH),
			NULL);
    if(ClassName.IsEmpty())
		return FALSE;
	HWND parent=NULL;
	if(pParentWnd)
		parent=pParentWnd->m_hWnd;
	return CWnd::CreateEx(dwExStyle,ClassName,lpszWindowName,dwStyle,x,y,nWidth,nHeight,parent,NULL);
}

BYTE * CVideoWnd::GetBuffer(int w, int h)
{
	BYTE *ptr=NULL;
	BOOL bVisible = (GetWindowLong(m_hWnd, GWL_STYLE) & WS_VISIBLE);
	if(m_hWnd&&bVisible)//IsWindowVisible())
	{
		ptr=m_buffer.GetBuffer(w,h);
		if(ptr)
		{
			if(w!=m_bmi.bmiHeader.biWidth||h!=m_bmi.bmiHeader.biHeight)
			{
				m_bmi.bmiHeader.biWidth=w;
				m_bmi.bmiHeader.biHeight=h;
				// m_bSizeChanged=TRUE;
			}			
			//We don't call ReleaseBuffer() here, and call it later
		}
	}
	return ptr;
	
}

BOOL CVideoWnd::ReleaseBuffer()
{
	m_buffer.ReleaseBuffer();
	if(m_hWnd)
		InvalidateRect(NULL);
	return TRUE;
}


void CVideoWnd::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	if(m_bmi.bmiHeader.biWidth>0)
	{
		CRect rcWnd,rcClient;
		GetWindowRect(rcWnd); 
		GetClientRect(rcClient);
		//make the client area fit video size
		SetWindowPos(0,0,0,
			m_bmi.bmiHeader.biWidth+rcWnd.Width()-rcClient.Width(),
			m_bmi.bmiHeader.biHeight+rcWnd.Height()-rcClient.Height(),
			SWP_NOMOVE);
	}
	CWnd::OnLButtonDblClk(nFlags, point);
}


int CVideoWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CRect rcWnd,rcClient;
	GetWindowRect(rcWnd); 
	GetClientRect(rcClient);
	//make the client rectangle fit video size
	SetWindowPos(0,0,0,176+rcWnd.Width()-rcClient.Width(),144+rcWnd.Height()-rcClient.Height(),SWP_NOMOVE);

	memset(&m_bmi,0,sizeof(m_bmi));
	m_bmi.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
	m_bmi.bmiHeader.biPlanes=1;	
	m_bmi.bmiHeader.biBitCount=24;

	m_hDrawDIB=DrawDibOpen();
	return 0;
}

BOOL CVideoWnd::FillBuffer( BYTE* pRgb, int width, int height, int orgin )
{
	BYTE* pDst = NULL;
	if(orgin%2)
		pDst = GetBuffer(height, width);
	else
		pDst = GetBuffer(width, height);
	if(pDst)
	{
		switch(orgin%4){
		case 0:
			memcpy(pDst, pRgb, width*height*3);
			break;
		case 1:
			RGBRotate90(pRgb, pDst, height, width);
			break;
		case 2:
			RGBRotate180(pRgb, pDst, height, width);
			break;
		case 3:
			RGBRotate270(pRgb, pDst, height, width);
			break;
		}
		ReleaseBuffer();
		return TRUE;
	}
	return FALSE;
}

void CVideoWnd::SetFitMode( BOOL bFit )
{
	m_bFit = bFit;
	if(m_hWnd)
		InvalidateRect(NULL);
}

void CVideoWnd::SetDefOrgin( int orgin )
{
	m_nDefOrgin = orgin;
	if(m_hWnd)
		InvalidateRect(NULL);
}

void CVideoWnd::SetText( LPCTSTR text, COLORREF cl, DWORD mode )
{
	m_szText = text;
	m_nTextFmt = mode;
	m_clrText = cl;
	if(m_hWnd)
		InvalidateRect(NULL);
}

void CVideoWnd::Clear()
{
	m_buffer.CleanUpBuffer();
	if(m_hWnd)
		InvalidateRect(NULL);
}

//创建位图文件
BOOL SaveBitmap(LPCTSTR bmpPath ,BYTE * pBuffer, int lWidth, int lHeight)
{
	// 生成bmp文件
	HANDLE hf = CreateFile(
		bmpPath, GENERIC_WRITE, FILE_SHARE_READ, NULL,
		CREATE_ALWAYS, NULL, NULL );
	if( hf == INVALID_HANDLE_VALUE )return 0;
	// 写文件头 
	BITMAPFILEHEADER bfh;
	memset( &bfh, 0, sizeof( bfh ) );
	bfh.bfType = 'MB';
	bfh.bfSize = sizeof( bfh ) + lWidth*lHeight*3 + sizeof( BITMAPINFOHEADER );
	bfh.bfOffBits = sizeof( BITMAPINFOHEADER ) + sizeof( BITMAPFILEHEADER );
	DWORD dwWritten = 0;
	WriteFile( hf, &bfh, sizeof( bfh ), &dwWritten, NULL );
	// 写位图格式
	BITMAPINFOHEADER bih;
	memset( &bih, 0, sizeof( bih ) );
	bih.biSize = sizeof( bih );
	bih.biWidth = lWidth;
	bih.biHeight = lHeight;
	bih.biPlanes = 1;
	bih.biBitCount = 24;
	WriteFile( hf, &bih, sizeof( bih ), &dwWritten, NULL );
	// 写位图数据
	WriteFile( hf, pBuffer, lWidth*lHeight*3, &dwWritten, NULL );
	CloseHandle( hf );
	return 0;
}

#include <afxole.h>
#include <atlimage.h>
BOOL CVideoWnd::TaskSnap( LPCTSTR path )
{
	BOOL bRet = FALSE;
	COleStreamFile stm;
	BYTE* b = m_buffer.GetBuffer();
	if(b&&stm.CreateMemoryStream()){
		DWORD size = m_bmi.bmiHeader.biWidth * m_bmi.bmiHeader.biHeight * 3;
		// 写文件头 
		BITMAPFILEHEADER bfh;
		memset( &bfh, 0, sizeof( bfh ) );
		bfh.bfType = 'MB';
		bfh.bfSize = sizeof( bfh ) + size + sizeof( BITMAPINFOHEADER );
		bfh.bfOffBits = sizeof( BITMAPINFOHEADER ) + sizeof( BITMAPFILEHEADER );
		stm.Write(&bfh, sizeof bfh);
		stm.Write(&m_bmi.bmiHeader, sizeof BITMAPINFOHEADER);
		stm.Write(b, size);
		CImage img;
		if(SUCCEEDED(img.Load(stm.GetStream())))
			bRet = SUCCEEDED(img.Save(path));
	}
	if(b) m_buffer.ReleaseBuffer();
	return bRet;
}


void CVideoWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
  // TODO: Add your message handler code here and/or call default
  SetCapture();
  m_bTracker = true;
  m_rcTacker.BottomRight() = m_rcTacker.TopLeft() = point;
  Invalidate();
  CWnd::OnLButtonDown(nFlags, point);
}


void CVideoWnd::OnMouseMove(UINT nFlags, CPoint point)
{
  // TODO: Add your message handler code here and/or call default
  if (m_bTracker){
    m_rcTacker.BottomRight() = point;
    InvalidateRect(m_rcTacker);
  }
  CWnd::OnMouseMove(nFlags, point);
}


void CVideoWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
  // TODO: Add your message handler code here and/or call default
  if (m_bTracker){
    m_rcTacker.BottomRight() = point;
    ReleaseCapture();
    Invalidate();
    m_bTracker = false;
  }
  CWnd::OnLButtonUp(nFlags, point);
}
