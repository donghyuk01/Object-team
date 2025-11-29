// mpictureView.cpp: CmpictureView 클래스의 구현
//
#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "mpicture.h"
#endif
#include "mpictureDoc.h"
#include "mpictureView.h"
#include <atlimage.h>          // ← CImage 사용을 위해 추가

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CmpictureView
IMPLEMENT_DYNCREATE(CmpictureView, CFormView)

BEGIN_MESSAGE_MAP(CmpictureView, CFormView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CFormView::OnFilePrintPreview)
	ON_BN_CLICKED(IDC_BUTTON1, &CmpictureView::OnBnClickedButton1)
END_MESSAGE_MAP()

// CmpictureView 생성/소멸
CmpictureView::CmpictureView() noexcept
	: CFormView(IDD_MPICTURE_FORM)
{
	// TODO: 여기에 생성 코드를 추가합니다.
}

CmpictureView::~CmpictureView()
{
	if (!m_image.IsNull())
		m_image.Destroy();           // ← 메모리 누수 방지
}

void CmpictureView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_PICTURE, m_wndPicture);   // ← Picture Control 연결 (ID 확인!)
}

BOOL CmpictureView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	// Window 클래스 또는 스타일을 수정합니다.

	return CFormView::PreCreateWindow(cs);
}

void CmpictureView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();
}

// CmpictureView 인쇄
BOOL CmpictureView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CmpictureView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CmpictureView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}

void CmpictureView::OnPrint(CDC* pDC, CPrintInfo* /*pInfo*/)
{
	// TODO: 여기에 사용자 지정 인쇄 코드를 추가합니다.
}

// CmpictureView 진단
#ifdef _DEBUG
void CmpictureView::AssertValid() const
{
	CFormView::AssertValid();
}

void CmpictureView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CmpictureDoc* CmpictureView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CmpictureDoc)));
	return (CmpictureDoc*)m_pDocument;
}
#endif //_DEBUG

// CmpictureView 메시지 처리기
void CmpictureView::OnBnClickedButton1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strFilter = L"이미지 파일 (*.bmp;*.jpg;*.jpeg;*.png;*.gif)|*.bmp;*.jpg;*.jpeg;*.png;*.gif||";

	CFileDialog dlg(TRUE, nullptr, nullptr, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, strFilter);
	if (dlg.DoModal() == IDOK)
	{
		LoadAndDisplayImage(dlg.GetPathName());
	}
}

void CmpictureView::LoadAndDisplayImage(const CString& strPath)
{
	// 기존 이미지 정리
	if (!m_image.IsNull())
		m_image.Destroy();

	if (FAILED(m_image.Load(strPath)))
	{
		AfxMessageBox(L"이미지를 열 수 없습니다.");
		return;
	}

	// Picture Control 크기 얻기
	CRect rc;
	m_wndPicture.GetClientRect(&rc);

	// Picture Control에 그리기
	CDC* pDC = m_wndPicture.GetDC();
	if (pDC)
	{
		m_image.Draw(pDC->m_hDC, 0, 0, rc.Width(), rc.Height());
		m_wndPicture.ReleaseDC(pDC);
	}

	AfxMessageBox(L"이미지가 성공적으로 추가되었습니다.", MB_ICONINFORMATION);
}
