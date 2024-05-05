#include "pch.h"
#include "framework.h"
#include "farseer3_mfc.h"
#include "farseer3_mfcDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);  

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



Cfarseer3mfcDlg::Cfarseer3mfcDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FARSEER3_MFC_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Cfarseer3mfcDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, edScaleX, vEdScaleX);
	DDX_Control(pDX, edScaleY, vEdScaleY);
	DDX_Control(pDX, edJournal, vEdJournal);
	DDX_Control(pDX, cbShowMessage, vCbShowMessage);
	DDX_Control(pDX, edTolerance, vEdTolerance);
	DDX_Control(pDX, edBuffSize, vEdBuffSize);
	DDX_Control(pDX, edNSteps, vEdNSteps);
	DDX_Control(pDX, cbUsePntMgr, vCbUsePntMgr);
	DDX_Control(pDX, cbUseDblFltr, vCbUseDblFltr);
	DDX_Control(pDX, edMTresh, vEdMTresh);
	DDX_Control(pDX, edFTresh, vEdFTresh);
}

BEGIN_MESSAGE_MAP(Cfarseer3mfcDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &Cfarseer3mfcDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDOK, &Cfarseer3mfcDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON4, &Cfarseer3mfcDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDCANCEL, &Cfarseer3mfcDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON2, &Cfarseer3mfcDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &Cfarseer3mfcDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON5, &Cfarseer3mfcDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &Cfarseer3mfcDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7, &Cfarseer3mfcDlg::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON8, &Cfarseer3mfcDlg::OnBnClickedButton8)
	ON_EN_CHANGE(edBuffSize2, &Cfarseer3mfcDlg::OnEnChangeedbuffsize2)
	ON_BN_CLICKED(IDC_BUTTON9, &Cfarseer3mfcDlg::OnBnClickedButton9)
	ON_BN_CLICKED(IDC_BUTTON10, &Cfarseer3mfcDlg::OnBnClickedButton10)
END_MESSAGE_MAP()


BOOL Cfarseer3mfcDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

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

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	GetDlgItem(edScaleX)->SetWindowText(L"100");
	GetDlgItem(edScaleY)->SetWindowText(L"100");
	GetDlgItem(edTolerance)->SetWindowText(L"0.6");
	GetDlgItem(edBuffSize)->SetWindowText(L"20");
	GetDlgItem(edNSteps)->SetWindowText(L"20");
	GetDlgItem(edMTresh)->SetWindowText(L"0.05");
	GetDlgItem(edFTresh)->SetWindowText(L"0.25");
	GetDlgItem(edNSteps)->SetWindowText(L"20");

	vCbShowMessage.SetWindowText(_T("Show message"));

	return TRUE;  
}

void Cfarseer3mfcDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void Cfarseer3mfcDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR Cfarseer3mfcDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void Cfarseer3mfcDlg::OnBnClickedButton1()
{
	CString szt;
	vEdScaleX.GetWindowText(szt);
	appRef->nomSizeX = _ttof(szt);
	appRef->demandOp = 1;
}


void Cfarseer3mfcDlg::OnBnClickedOk()
{
	CDialogEx::OnOK();
}


void Cfarseer3mfcDlg::OnBnClickedButton4()
{
	CStatic *pStatic = (CStatic *)GetDlgItem(IDC_STATIC1);
	if (pStatic != NULL)
	{
		CString outp;
		outp.Format(L"%d; %d", appRef->ww, appRef->wh);
		pStatic->SetWindowText(outp);
	}
}

void Cfarseer3mfcDlg::updateOglState()
{
	CStatic *pStatic = (CStatic *)GetDlgItem(IDC_STATIC1);
	if (pStatic != NULL)
	{
		CString outp;
		CString inp; 
		pStatic->GetWindowText(inp);
		outp.Format(L"%d; %d - %d; %d - %.1f; %.1f", appRef->ww, appRef->wh, appRef->oglCurX, appRef->oglCurY, appRef->fScaleX, appRef->fScaleY);
		
		if (inp != outp) pStatic->SetWindowText(outp);
	}
}


void Cfarseer3mfcDlg::OnBnClickedCancel()
{
	const TCHAR szFilter[] = _T("JPG Files (*.jpg)|*.jpg|PNG Files (*.png)|*.png|All Files (*.*)|*.*||");
	CFileDialog dlg(TRUE, _T("jpg"), NULL, OFN_ENABLESIZING | OFN_FILEMUSTEXIST, szFilter, this);
	if (dlg.DoModal() == IDOK)
	{
		CString sFilePath = dlg.GetPathName();
		appRef->file_target = sFilePath;
	}
}


void Cfarseer3mfcDlg::OnBnClickedButton2()
{
	CString szt;
	vEdScaleY.GetWindowText(szt);
	appRef->nomSizeY = _ttof(szt);
	appRef->demandOp = 2;
}


void Cfarseer3mfcDlg::OnBnClickedButton3()
{
	appRef->demandOp = 3;
}


void Cfarseer3mfcDlg::OnBnClickedButton5()
{

	appRef->fScaleY = appRef->fScaleX;
	appRef->forceUpdateOglScales(false);
}


void Cfarseer3mfcDlg::OnBnClickedButton6()
{
	appRef->fScaleX = appRef->fScaleY;
	appRef->forceUpdateOglScales(true);
}

void Cfarseer3mfcDlg::outpMeasJourn()
{
	CString txt = _T("");
	for (int i = 0; i < appRef->getMeasJournal().size(); i++)
	{
		CString jval;
		jval.Format(_T("%.2f\r\n"), appRef->getMeasJournal()[i]);
		txt += jval;
	}
	vEdJournal.SetWindowText(txt);
}

void Cfarseer3mfcDlg::logStuff(CString txt)
{
	CString currText;
	vEdJournal.GetWindowText(currText);
	CString fullText;
	fullText.Format(_T("%s\n%s"),currText,txt);
	vEdJournal.SetWindowText(fullText);
}

void Cfarseer3mfcDlg::OnBnClickedButton7()
{
	appRef->demandOp = 4;
}


void Cfarseer3mfcDlg::OnBnClickedButton8()
{
	CString tol;
	vEdTolerance.GetWindowText(tol);
	appRef->expTolerance = _ttof(tol);

	CString bufs;
	vEdBuffSize.GetWindowText(bufs);
	appRef->expBufSize = _ttoi(bufs);

	vEdNSteps.GetWindowText(bufs);
	appRef->expNSteps = _ttoi(bufs);

	appRef->demandOp = 5;
}


void Cfarseer3mfcDlg::OnEnChangeedbuffsize2()
{

}


void Cfarseer3mfcDlg::OnBnClickedButton9()
{
	appRef->clearMeasJournal();
	outpMeasJourn();
}


void Cfarseer3mfcDlg::OnBnClickedButton10()
{
	CString tol;
	vEdTolerance.GetWindowText(tol);
	appRef->expTolerance = _ttof(tol);

	CString bufs;
	vEdBuffSize.GetWindowText(bufs);
	appRef->expBufSize = _ttoi(bufs);

	vEdNSteps.GetWindowText(bufs);
	appRef->expNSteps = _ttoi(bufs);

	vEdMTresh.GetWindowText(bufs);
	appRef->mTresh = _ttof(bufs);

	vEdFTresh.GetWindowText(bufs);
	appRef->fTresh = _ttof(bufs);

	//new op for full image scan
	appRef->demandOp = 6;
}
