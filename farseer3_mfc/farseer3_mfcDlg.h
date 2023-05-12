#pragma once



class Cfarseer3mfcDlg : public CDialogEx
{

public:
	TFarseer3 * appRef;
	Cfarseer3mfcDlg(CWnd* pParent = nullptr);


#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FARSEER3_MFC_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	



protected:
	HICON m_hIcon;


	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	void updateOglState();
	void outpMeasJourn();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedCancel();
	CEdit vEdScaleX;
	CEdit vEdScaleY;
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton6();
	CEdit vEdJournal;
	CButton vCbShowMessage;
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedButton8();
	CEdit vEdTolerance;
	CEdit vEdBuffSize;
};
