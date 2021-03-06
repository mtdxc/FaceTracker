
// RealTimeTestDlg.h : 头文件
//

#pragma once
#include "afxwin.h"

#include "CaptureVideo.h"
#include "DrawVideo.h"
#include "VideoWnd.h"
#include "BlurWnd.h"
#include "face_detection.h"
#include "face_alignment.h"
#include "FaceTracker.h"
#include "MagicBeautify.h"
#include <memory>
// CRealTimeTestDlg 对话框
class CRealTimeTestDlg : public CDialogEx
{
// 构造
public:
	CRealTimeTestDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CRealTimeTestDlg();
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_REALTIMETEST_DIALOG };
#endif
  void OnRgbData(BYTE* pRgb, int with, int heght);
	int FillPolyn(DWORD rgb, IFaceTracker::Feature feat);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
  TCHAR strfps[64];
  DWORD m_tick;
  int m_nFrame;
  HICON m_hIcon;
  CCaptureVideo m_capture;
  CDrawVideo m_draw;
  CBlurWnd m_preview;
  MagicBeautify m_deautify;

  std::unique_ptr<seeta::FaceDetection> face_detector;
  std::unique_ptr<seeta::FaceAlignment> point_detector;

	IFaceTracker* tracker;

  std::string yuv;
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnCbnSelchangeComboSmooth();
	afx_msg void OnBnClickedButtonStartPrev();
	afx_msg void OnBnClickedButtonReset();
	afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP()
public:
  CComboBox m_cbDevices;
  CComboBox m_cbCheckType;
  CButton m_btnBeatiful;
  CComboBox m_cbSmooth;
  double m_dbArg4;
  double m_dbArg3;
  int m_iArg2;
  int m_iArg1;
};
