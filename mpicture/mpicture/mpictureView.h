
// mpictureView.h: CmpictureView 클래스의 인터페이스
//

#pragma once


class CmpictureView : public CFormView
{
protected: // serialization에서만 만들어집니다.
	CmpictureView() noexcept;
	DECLARE_DYNCREATE(CmpictureView)

public:
#ifdef AFX_DESIGN_TIME
	enum{ IDD = IDD_MPICTURE_FORM };
#endif

// 특성입니다.
public:
	CmpictureDoc* GetDocument() const;

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
	virtual ~CmpictureView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()
public:
	// Picture Control 변수 (리소스 ID: IDC_STATIC_PICTURE 라고 가정)
	CStatic m_wndPicture;

	// 현재 로드된 이미지 보관 (메모리 누수 방지 + 재사용 용이)
	CImage m_image;

	// 이미지 로드 및 Picture Control에 표시하는 함수
	void LoadAndDisplayImage(const CString& strPath);

	afx_msg void OnBnClickedButton1();
};

#ifndef _DEBUG  // mpictureView.cpp의 디버그 버전
inline CmpictureDoc* CmpictureView::GetDocument() const
   { return reinterpret_cast<CmpictureDoc*>(m_pDocument); }
#endif

