﻿
// ChartGameDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "ChartGame.h"
#include "ChartGameDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

// 캔들 조작용
CChartCandlestickSerie* pCandle = nullptr;
SChartCandlestickPoint pCandlePoint[600];
int cnt;

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
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


// CChartGameDlg 대화 상자



CChartGameDlg::CChartGameDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CHARTGAME_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CChartGameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHARTCTRL, m_ChartCtrl);
	DDX_Control(pDX, IDC_NEXT, CNext);
	DDX_Control(pDX, IDC_COUNT, mCount);
}

BEGIN_MESSAGE_MAP(CChartGameDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_NEXT, &CChartGameDlg::OnBnClickedNext)
	ON_BN_CLICKED(IDC_GO, &CChartGameDlg::OnBnClickedGo)
END_MESSAGE_MAP()


// CChartGameDlg 메시지 처리기

BOOL CChartGameDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	/// 초기화
	CChartDateTimeAxis* pBottomAxis = m_ChartCtrl.CreateDateTimeAxis(CChartCtrl::BottomAxis);
	CChartStandardAxis* pLeftAxis = m_ChartCtrl.CreateStandardAxis(CChartCtrl::LeftAxis);
	pLeftAxis->SetAutomaticMode(CChartAxis::FullAutomatic);
	pBottomAxis->SetAutomaticMode(CChartAxis::FullAutomatic);

	m_ChartCtrl.ShowMouseCursor(false);
	CChartCrossHairCursor* pCrossHair = m_ChartCtrl.CreateCrossHairCursor();

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CChartGameDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CChartGameDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

void CChartGameDlg::ReadData(SChartCandlestickPoint(&pCandlePoint)[600])
{
	UpdateData(TRUE);

	//@ 파일에서 데이터 불러오기
	// 파일열기
	CStringA strCSVfileName = (CStringA)theApp.m_sAppPath + L"\\data\\testdata.csv";
	char* BufOfFileName = strCSVfileName.GetBuffer(strCSVfileName.GetLength());
	FILE* f = nullptr;
	errno_t err;

	err = fopen_s(&f, BufOfFileName, "rt");	//읽기모드
	if (err || f == NULL)
	{
		AfxMessageBox(L"파일열기 실패");
		return;
	}

	double temp;
	int year, month, day;
	for (int i = 0; i < 600; i++)
	{
		fscanf_s(f, "%4d%2d%2d,%lf,%lf,%lf,%lf,%lf\n",	//  날짜, 시가, 종가, 고가, 저가, 거래량
			&year, &month, &day,
			&pCandlePoint[i].Open,
			&pCandlePoint[i].Close,
			&pCandlePoint[i].High,
			&pCandlePoint[i].Low,
			&temp);

		COleDateTime date(year, month, day, 0, 0, 0);
		pCandlePoint[i].XVal = CChartCtrl::DateToValue(date);
	}
	fclose(f);				//파일 닫기
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CChartGameDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CChartGameDlg::OnBnClickedNext()
{
	CString str;
	str.Format(_T("%d"), ++cnt);

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	pCandle->SetPoints(pCandlePoint, 100+(cnt));
	mCount.SetWindowTextW(str);
}

void CChartGameDlg::OnBnClickedGo()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	/// 봉차트 파트
	pCandle = m_ChartCtrl.CreateCandlestickSerie();
	
	ReadData(pCandlePoint);

	pCandle->SetPoints(pCandlePoint, 100);
	pCandle->SetColor(RGB(0, 255, 0));

	cnt = 0;
	// pCandle->CreateBalloonLabel(5, _T("This is a candle"));
}