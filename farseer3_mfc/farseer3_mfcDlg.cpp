
// farseer3_mfcDlg.cpp: файл реализации
//

#include "pch.h"
#include "framework.h"
#include "farseer3_mfc.h"
#include "farseer3_mfcDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Диалоговое окно CAboutDlg используется для описания сведений о приложении

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // поддержка DDX/DDV

// Реализация
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


// Диалоговое окно Cfarseer3mfcDlg



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
END_MESSAGE_MAP()


// Обработчики сообщений Cfarseer3mfcDlg

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

// При добавлении кнопки свертывания в диалоговое окно нужно воспользоваться приведенным ниже кодом,
//  чтобы нарисовать значок.  Для приложений MFC, использующих модель документов или представлений,
//  это автоматически выполняется рабочей областью.

void Cfarseer3mfcDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // контекст устройства для рисования

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Выравнивание значка по центру клиентского прямоугольника
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Нарисуйте значок
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// Система вызывает эту функцию для получения отображения курсора при перемещении
//  свернутого окна.
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
	// TODO: добавьте свой код обработчика уведомлений
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
	CFileDialog dlg(FALSE, _T("jpg"), NULL, OFN_ENABLESIZING | OFN_FILEMUSTEXIST, szFilter, this);
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

void Cfarseer3mfcDlg::OnBnClickedButton7()
{
	appRef->demandOp = 4;
}
