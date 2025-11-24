
// 객체지향언어기말프로젝트View.cpp: C객체지향언어기말프로젝트View 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "객체지향언어기말프로젝트.h"
#endif

#include "객체지향언어기말프로젝트Doc.h"
#include "객체지향언어기말프로젝트View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// C객체지향언어기말프로젝트View

IMPLEMENT_DYNCREATE(C객체지향언어기말프로젝트View, CFormView)

BEGIN_MESSAGE_MAP(C객체지향언어기말프로젝트View, CFormView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CFormView::OnFilePrintPreview)
END_MESSAGE_MAP()

// C객체지향언어기말프로젝트View 생성/소멸

C객체지향언어기말프로젝트View::C객체지향언어기말프로젝트View() noexcept
	: CFormView(IDD_MY_FORM)
{
	// TODO: 여기에 생성 코드를 추가합니다.

}

C객체지향언어기말프로젝트View::~C객체지향언어기말프로젝트View()
{
}

void C객체지향언어기말프로젝트View::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BOOL C객체지향언어기말프로젝트View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CFormView::PreCreateWindow(cs);
}

void C객체지향언어기말프로젝트View::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

}


// C객체지향언어기말프로젝트View 인쇄

BOOL C객체지향언어기말프로젝트View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void C객체지향언어기말프로젝트View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void C객체지향언어기말프로젝트View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}

void C객체지향언어기말프로젝트View::OnPrint(CDC* pDC, CPrintInfo* /*pInfo*/)
{
	// TODO: 여기에 사용자 지정 인쇄 코드를 추가합니다.
}


// C객체지향언어기말프로젝트View 진단

#ifdef _DEBUG
void C객체지향언어기말프로젝트View::AssertValid() const
{
	CFormView::AssertValid();
}

void C객체지향언어기말프로젝트View::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

C객체지향언어기말프로젝트Doc* C객체지향언어기말프로젝트View::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(C객체지향언어기말프로젝트Doc)));
	return (C객체지향언어기말프로젝트Doc*)m_pDocument;
}
#endif //_DEBUG


// C객체지향언어기말프로젝트View 메시지 처리기
