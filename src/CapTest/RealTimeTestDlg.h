
// RealTimeTestDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"

#include "CaptureVideo.h"
#include "DrawVideo.h"
#include "face_detection.h"
#include "FaceTracker/Tracker.h"
#include "../lib/ofxFaceTracker.h"

// CRealTimeTestDlg �Ի���
class CRealTimeTestDlg : public CDialogEx
{
// ����
public:
	CRealTimeTestDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_REALTIMETEST_DIALOG };
#endif
  void OnRgbData(BYTE* pRgb, int with, int heght);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

// ʵ��
protected:
  TCHAR strfps[32];
  DWORD m_tick;
  int m_nFrame;
  HICON m_hIcon;
  CCaptureVideo m_capture;
  CDrawVideo m_draw;
  seeta::FaceDetection detector;
#ifdef OFX_EXPORTS
	ofxFaceTracker tracker;
#else
	FACETRACKER::Tracker model;
	bool failed;
#endif

  std::string yuv;
	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedButtonStartPrev();
  CComboBox m_cbDevices;
  afx_msg void OnBnClickedButtonReset();
};
