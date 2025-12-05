
// 객체지향언어기말프로젝트View.h: C객체지향언어기말프로젝트View 클래스의 인터페이스
//

#pragma once
#include <afxinet.h>
#include <afxdb.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <gdiplus.h>
#include <algorithm>	
#include <cctype>
using namespace Gdiplus;
#pragma comment(lib, "gdiplus.lib")

class bookDIg;
class UserDlg;
class C객체지향언어기말프로젝트View : public CFormView
{
protected: // serialization에서만 만들어집니다.
	C객체지향언어기말프로젝트View() noexcept;
	DECLARE_DYNCREATE(C객체지향언어기말프로젝트View)

public:
#ifdef AFX_DESIGN_TIME
	enum{ IDD = IDD_MY_FORM };
#endif

// 특성입니다.
public:
	C객체지향언어기말프로젝트Doc* GetDocument() const;

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual void OnInitialUpdate(); // 생성 후 처음 호출되었습니다.
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);

// 구현입니다.
public:
	virtual ~C객체지향언어기말프로젝트View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedUserform();
	afx_msg void OnBnClickedBookform();
	afx_msg void OnBnClickedLoanform();
	afx_msg void OnBnClickedMainsearch();
	CEdit MainEc; //메인 에디트 컨트롤 검색창
	CComboBox mCbs; //메인 콤보박스 검색용 (제목/저자)
	CComboBox mCbc; //메인 콤보박스 정렬
	CListCtrl mLc; //메인 리스트 컨트롤
	CDatabase db;
	CImageList mImageList;
	UserDlg* ud;
	bookDIg* bd;
	CComboBox mCb3;
	ULONG_PTR m_gdiplusToken;
	Gdiplus::Bitmap* m_pBookBitmap;
	afx_msg void OnNMCustomdrawList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	void LoadBooksFromServer(CString type, CString keyword, CString sortOrder, CString category);
	HBITMAP LoadImageFromURL(CString url);
	afx_msg void OnCbnSelchangeCombo3();
	afx_msg void OnCbnSelchangeCombo2();
};

HBITMAP LoadPngToHBITMAP(CString path);
HBITMAP ResizeHBITMAP(HBITMAP hSrcBmp, int nNewWidth, int nNewHeight);
#ifndef _DEBUG  // 객체지향언어기말프로젝트View.cpp의 디버그 버전
inline C객체지향언어기말프로젝트Doc* C객체지향언어기말프로젝트View::GetDocument() const
   { return reinterpret_cast<C객체지향언어기말프로젝트Doc*>(m_pDocument); }
#endif

