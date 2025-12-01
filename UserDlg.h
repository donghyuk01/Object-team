#pragma once
#include "afxdialogex.h"


// UserDlg 대화 상자
class C객체지향언어기말프로젝트View;
class UserDlg : public CDialogEx
{
	DECLARE_DYNAMIC(UserDlg)

public:
	UserDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~UserDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_User };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedUsercancel();
	C객체지향언어기말프로젝트View* userView;
	CEdit userName;
	CEdit birth;
	CEdit maxLoan;
	CEdit solveDate;
	CComboBox statusBox;
	afx_msg void OnBnClickedButton5();
	CListCtrl user_lc;
	virtual BOOL OnInitDialog();
};
