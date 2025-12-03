#pragma once
#include "afxdialogex.h"


// bookDIg 대화 상자
class C객체지향언어기말프로젝트View;
class bookDIg : public CDialogEx
{
	DECLARE_DYNAMIC(bookDIg)

public:
	bookDIg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~bookDIg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_Book_reg };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	C객체지향언어기말프로젝트View* bookView;
	DECLARE_MESSAGE_MAP()
	
public:
	afx_msg void OnBnClickedBookreg();
	afx_msg void OnBnClickedBookupdate();
	afx_msg void OnBnClickedBookdel();
	CEdit bec1;
	CEdit bec2;
	CEdit bec3;
	CComboBox bcb1;
	afx_msg void OnBnClickedBookimg();
	afx_msg void OnBnClickedBookclose();
	virtual BOOL OnInitDialog();
	CStringA Base64Encode(const BYTE* pData, DWORD length);
	CString m_strImagePath; // 선택된 로컬 이미지 파일 경로 (업로드 시 사용)
};
