
// RealTimeTestDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "RealTimeTest.h"
#include "RealTimeTestDlg.h"
#include "afxdialogex.h"
#include "convert.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma comment(lib, "FaceDetection.lib");
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CRealTimeTestDlg 对话框
CRealTimeTestDlg::CRealTimeTestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_REALTIMETEST_DIALOG, pParent), detector("seeta_fd_frontal_v1.0.bin")
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRealTimeTestDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialogEx::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_COMBO_DEVICES, m_cbDevices);
}

BEGIN_MESSAGE_MAP(CRealTimeTestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
  ON_BN_CLICKED(IDC_BUTTON_START_PREV, &CRealTimeTestDlg::OnBnClickedButtonStartPrev)
  ON_BN_CLICKED(IDC_BUTTON_RESET, &CRealTimeTestDlg::OnBnClickedButtonReset)
END_MESSAGE_MAP()


// CRealTimeTestDlg 消息处理程序

BOOL CRealTimeTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
  if (m_capture.EnumDevices(m_cbDevices.GetSafeHwnd()))
    m_cbDevices.SetCurSel(0);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CRealTimeTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CRealTimeTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CRealTimeTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void VideCallBack(BYTE* data, int width, int height, void* user) {
  CRealTimeTestDlg* pDlg = (CRealTimeTestDlg*)user;
  if (pDlg) pDlg->OnRgbData(data, width, height);
}

void CRealTimeTestDlg::OnRgbData(BYTE* pRgb, int width, int height)
{
  int nyuv = width*height * 3 / 2;
  if (yuv.size() < nyuv)
    yuv.resize(nyuv);
  BYTE* pyuv = (BYTE*)yuv.data();
  // 这边要进行翻转(否则识别不对)！
  RGBtoYUV420P(pRgb, pyuv, width, height, 3, TRUE);
  m_draw.rects.clear();
  m_draw.points.clear();
#if 1
  if (tracker.updateYUV(pyuv, width, height)) {
    vector<ofVec2f> pts = tracker.getImagePoints();
    TRACE("Got %d pts\n", pts.size());
    for (auto p : pts){
      POINT pt = { p.x, p.y };
      m_draw.points.push_back(pt);
    }
    /* 把右上眼给描点出来
    tracker.getGesture(ofxFaceTracker::MOUTH_WIDTH);
    ofPolyline polyEye = tracker.getImageFeature(ofxFaceTracker::LEFT_EYE_TOP);
    for (auto p : polyEye.points) {
      POINT pt = { p.x, p.y };
      m_draw.points.push_back(pt);
    }*/
  }
#elif 0
  //set other tracking parameters
  std::vector<int> wSize1(1); wSize1[0] = 7;
  std::vector<int> wSize2(3); wSize2[0] = 11; wSize2[1] = 9; wSize2[2] = 7;
  int fpd = -1; int nIter = 5; double clamp = 3, fTol = 0.01;
  //track this image
  std::vector<int> wSize;
  if (failed)
    wSize = wSize2;
  else
    wSize = wSize1;
#if 0
  cv::Mat gray,img(height, width, CV_8UC3, pRgb);
  //cv::flip(img, img, 1);
  cv::cvtColor(img, gray, CV_RGB2GRAY);
#else
  cv::Mat gray(height, width, CV_8UC1, pyuv);
#endif

  // 识别
  if (model.Track(gray, wSize, fpd, nIter, clamp, fTol, false) == 0) {
    failed = false;
    cv::Mat& shape = model._shape;
    cv::Mat& visi = model.GetVisiMat();
    int i, n = shape.rows / 2;
    for (i = 0; i < n; i++) {
      if (visi.at<int>(i, 0) == 0)
        continue;
      POINT pt = { shape.at<double>(i, 0), shape.at<double>(i + n, 0) };
      m_draw.points.push_back(pt);
    }
    /* 这个不准
    m_draw.rects.push_back(RECT{ model._rect.x, model._rect.y,
      model._rect.x + model._rect.width, model._rect.y + model._rect.height });
    */
  }
  else {
    //if (show) { cv::Mat R(im, cvRect(0, 0, 150, 50)); R = cv::Scalar(0, 0, 255); }
    model.FrameReset(); 
    failed = true;
  }
#else
  seeta::ImageData img_data;
  // yuv通道数据
  img_data.data = pyuv;
  img_data.width = width;
  img_data.height = height;
  img_data.num_channels = 1;

  long t0 = GetTickCount();
  std::vector<seeta::FaceInfo> faces = detector.Detect(img_data);
  long t1 = GetTickCount();
  m_draw.rects.resize(faces.size());
  for (int i =0; i<faces.size(); i++)
  {
    m_draw.rects[i] = RECT{ faces[i].bbox.x, faces[i].bbox.y,
      faces[i].bbox.x + faces[i].bbox.width, faces[i].bbox.y + faces[i].bbox.height };
  }
#endif
  m_nFrame++;
  if (GetTickCount() - m_tick > 1000) {
    _stprintf(strfps, _T("FPS:%d"), m_nFrame);
    m_nFrame = 0;
    m_tick = GetTickCount();
  }
  m_draw.Draw(pRgb, width, height, strfps);
}


void CRealTimeTestDlg::OnBnClickedButtonStartPrev()
{
  CWnd* btn = GetDlgItem(IDC_BUTTON_START_PREV);
  // TODO: 在此添加控件通知处理程序代码
  if (m_capture.started()) {
    m_capture.Stop();
    btn->SetWindowText(_T("Start Test"));
  }
  else {
    m_capture.SetSize(640, 480);
    m_capture.SetCallBack(&VideCallBack, this);
    HWND hVideo = NULL;
    GetDlgItem(IDC_VIDEO, &hVideo);
    m_capture.Start(m_cbDevices.GetCurSel(), NULL);// hVideo);
    m_draw.Init(hVideo);
#ifdef OFX_EXPORTS
    /* 参数设置
    tracker.setRescale(1.0);
    tracker.setIterations(5);
    tracker.setClamp(3.0);
    tracker.setTolerance(0.01);
    tracker.setAttempts(1);
    */
    tracker.setup();
#else
    model.Load("face2.tracker");
    failed = true;
#endif
    m_nFrame = 0;
    strfps[0] = 0;
    m_tick = GetTickCount();
    /*
    * Set minimum and maximum size of faces to detect (Default: 20, Not Limited)
    - `face_detector.SetMinFaceSize(size);`
    - `face_detector.SetMaxFaceSize(size);`
    * Set step size of sliding window (Default: 4)
    - `face_detector.SetWindowStep(step_x, step_y);`
    * Set scaling factor of image pyramid (0 < `factor` < 1, Default: 0.8)
    - `face_detector.SetImagePyramidScaleFactor(factor);`
    * Set score threshold of detected faces (Default: 2.0)
    - `face_detector.SetScoreThresh(thresh);`
    */
    detector.SetMinFaceSize(80);
    detector.SetScoreThresh(2.f);
    detector.SetImagePyramidScaleFactor(0.8f);
    detector.SetWindowStep(4, 4);

    btn->SetWindowText(_T("Stop Test"));
  }
}

void CRealTimeTestDlg::OnBnClickedButtonReset(){
  // TODO: 在此添加控件通知处理程序代码
#ifdef OFX_EXPORTS
  tracker.reset();
#else
  model.FrameReset();
#endif
}
#pragma comment(lib,"FaceTracker.lib")

#ifndef OFX_EXPORTS
//=============================================================================
void Draw(cv::Mat &image, cv::Mat &shape, cv::Mat &con, cv::Mat &tri, cv::Mat &visi)
{
  int i, n = shape.rows / 2; 
  cv::Point p1, p2; cv::Scalar c;

  //draw triangulation 三角测量
  c = CV_RGB(0, 0, 0);
  for (i = 0; i < tri.rows; i++) {
    if (visi.at<int>(tri.at<int>(i, 0), 0) == 0 ||
      visi.at<int>(tri.at<int>(i, 1), 0) == 0 ||
      visi.at<int>(tri.at<int>(i, 2), 0) == 0)continue;
    p1 = cv::Point(shape.at<double>(tri.at<int>(i, 0), 0),
      shape.at<double>(tri.at<int>(i, 0) + n, 0));
    p2 = cv::Point(shape.at<double>(tri.at<int>(i, 1), 0),
      shape.at<double>(tri.at<int>(i, 1) + n, 0));
    cv::line(image, p1, p2, c);
    p1 = cv::Point(shape.at<double>(tri.at<int>(i, 0), 0),
      shape.at<double>(tri.at<int>(i, 0) + n, 0));
    p2 = cv::Point(shape.at<double>(tri.at<int>(i, 2), 0),
      shape.at<double>(tri.at<int>(i, 2) + n, 0));
    cv::line(image, p1, p2, c);
    p1 = cv::Point(shape.at<double>(tri.at<int>(i, 2), 0),
      shape.at<double>(tri.at<int>(i, 2) + n, 0));
    p2 = cv::Point(shape.at<double>(tri.at<int>(i, 1), 0),
      shape.at<double>(tri.at<int>(i, 1) + n, 0));
    cv::line(image, p1, p2, c);
  }
  //draw connections
  c = CV_RGB(0, 0, 255);
  for (i = 0; i < con.cols; i++) {
    if (visi.at<int>(con.at<int>(0, i), 0) == 0 ||
      visi.at<int>(con.at<int>(1, i), 0) == 0)continue;
    p1 = cv::Point(shape.at<double>(con.at<int>(0, i), 0),
      shape.at<double>(con.at<int>(0, i) + n, 0));
    p2 = cv::Point(shape.at<double>(con.at<int>(1, i), 0),
      shape.at<double>(con.at<int>(1, i) + n, 0));
    cv::line(image, p1, p2, c, 1);
  }
  //draw points
  for (i = 0; i < n; i++) {
    if (visi.at<int>(i, 0) == 0)continue;
    p1 = cv::Point(shape.at<double>(i, 0), shape.at<double>(i + n, 0));
    c = CV_RGB(255, 0, 0); cv::circle(image, p1, 2, c);
  }return;
}

void TrackerFunc() {
  char ftFile[256] = "../model/face2.tracker";
  char conFile[256] = "../model/face.con";
  char triFile[256] = "../model/face.tri";
  bool fcheck = false;// double scale = 1;  bool show = true;

  //set other tracking parameters
  std::vector<int> wSize1(1); wSize1[0] = 7;
  std::vector<int> wSize2(3); wSize2[0] = 11; wSize2[1] = 9; wSize2[2] = 7;
  int fpd = -1; int nIter = 5; double clamp = 3, fTol = 0.01;

  FACETRACKER::Tracker model(ftFile);
  cv::Mat tri = FACETRACKER::IO::LoadTri(triFile);
  cv::Mat con = FACETRACKER::IO::LoadCon(conFile);

  bool failed = true;
  cv::Mat frame, gray, im; double fps = 0;
  while (1) {
    // 获取图像，并转成灰度图
    cv::flip(im, im, 1); 
    cv::cvtColor(im, gray, CV_BGR2GRAY);

    //track this image
    std::vector<int> wSize; 
    if (failed)
      wSize = wSize2; 
    else 
      wSize = wSize1;
    // 识别
    if (model.Track(gray, wSize, fpd, nIter, clamp, fTol, fcheck) == 0) {
      failed = false;
      Draw(im, model._shape, con, tri, model.GetVisiMat());
    }
    else {
      //if (show) { cv::Mat R(im, cvRect(0, 0, 150, 50)); R = cv::Scalar(0, 0, 255); }
      model.FrameReset(); failed = true;
    }
  }
}
#endif