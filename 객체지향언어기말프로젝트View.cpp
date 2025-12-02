// 객체지향언어기말프로젝트View.cpp: C객체지향언어기말프로젝트View 클래스의 구현
//

#include "pch.h"
#include "framework.h"
#ifndef SHARED_HANDLERS
#include "객체지향언어기말프로젝트.h"
#endif

#include "객체지향언어기말프로젝트Doc.h"
#include "객체지향언어기말프로젝트View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "bookDIg.h"
#include "UserDlg.h"
// C객체지향언어기말프로젝트View

IMPLEMENT_DYNCREATE(C객체지향언어기말프로젝트View, CFormView)

BEGIN_MESSAGE_MAP(C객체지향언어기말프로젝트View, CFormView)
	ON_COMMAND(ID_FILE_PRINT, &CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CFormView::OnFilePrintPreview)
	ON_BN_CLICKED(IDC_Userform, &C객체지향언어기말프로젝트View::OnBnClickedUserform)
	ON_BN_CLICKED(IDC_Bookform, &C객체지향언어기말프로젝트View::OnBnClickedBookform)
	ON_BN_CLICKED(IDC_Loanform, &C객체지향언어기말프로젝트View::OnBnClickedLoanform)
	ON_BN_CLICKED(IDC_MainSearch, &C객체지향언어기말프로젝트View::OnBnClickedMainsearch)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST1, &C객체지향언어기말프로젝트View::OnNMCustomdrawList1)
	ON_WM_MEASUREITEM()
END_MESSAGE_MAP()

// C객체지향언어기말프로젝트View 생성/소멸

C객체지향언어기말프로젝트View::C객체지향언어기말프로젝트View() noexcept
	: CFormView(IDD_MY_FORM)
{
	bd = NULL;
	ud = NULL;
	m_pBookBitmap = NULL;
}

C객체지향언어기말프로젝트View::~C객체지향언어기말프로젝트View()
{
	if (m_pBookBitmap) {
		delete m_pBookBitmap;
	}
}

void C객체지향언어기말프로젝트View::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, MainEc);
	DDX_Control(pDX, IDC_COMBO1, mCbs);
	DDX_Control(pDX, IDC_COMBO3, mCbc);
	DDX_Control(pDX, IDC_LIST1, mLc);
	DDX_Control(pDX, IDC_COMBO2, mCb3);
}

BOOL C객체지향언어기말프로젝트View::PreCreateWindow(CREATESTRUCT& cs)
{
	return CFormView::PreCreateWindow(cs);
}

void C객체지향언어기말프로젝트View::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	mCbs.ResetContent();
	mCbc.ResetContent();

	mCbs.AddString(_T("제목"));
	mCbs.AddString(_T("저자"));
	mCbs.SetCurSel(0);

	mCbc.AddString(_T("최신순"));
	mCbc.AddString(_T("인기순"));
	mCbc.AddString(_T("제목순"));
	mCbc.SetCurSel(0);

	mCb3.AddString(_T("철학"));
	mCb3.AddString(_T("종교"));
	mCb3.AddString(_T("사회과학"));
	mCb3.AddString(_T("자연과학"));
	mCb3.AddString(_T("기술과학"));
	mCb3.AddString(_T("예술"));
	mCb3.AddString(_T("언어"));
	mCb3.AddString(_T("문학"));
	mCb3.AddString(_T("역사"));
	mCb3.AddString(_T("기타"));
	mCb3.SetCurSel(0);

	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

	mLc.ModifyStyle(0, LVS_REPORT | LVS_OWNERDRAWFIXED);
	mLc.SetExtendedStyle(0);

	mLc.InsertColumn(0, _T("도서 정보"), LVCFMT_LEFT, 500);

	// ⭐ 이미지 로드 및 축소 로직 시작 ⭐
	const int FINAL_SIZE = 150;

	// 1. CImageList 크기는 최종 크기와 일치시켜야 합니다.
	mImageList.Create(FINAL_SIZE, FINAL_SIZE, ILC_COLOR32 | ILC_MASK, 1, 1);
	mLc.SetImageList(&mImageList, LVSIL_SMALL);

	// 2. 원본 HBITMAP 로드
	HBITMAP hBmpOriginal = LoadPngToHBITMAP(_T("1.png"));

	int imgIndex = -1;
	if (hBmpOriginal)
	{
		// 3. HBITMAP을 최종 크기로 직접 축소 
		HBITMAP hBmpResized = ResizeHBITMAP(hBmpOriginal, FINAL_SIZE, FINAL_SIZE);

		CBitmap bmp;
		bmp.Attach(hBmpResized);
		imgIndex = mImageList.Add(&bmp, RGB(255, 255, 255));
		bmp.Detach();

		// 원본 HBITMAP 해제
		DeleteObject(hBmpOriginal);
	}
	// ⭐ 이미지 로드 및 축소 로직 끝 ⭐


	/*mLc.SetIconSpacing(300, 220);*/

	CString info = _T("제목: 데이터베이스 개론\n저자: 홍길동\n출판사: 한빛미디어\n대출 가능");

	mLc.InsertItem(0, info, imgIndex);
	db.OpenEx(_T("DSN=blob;UID=mir9876;PWD=rlaehdgur;"), CDatabase::noOdbcDialog);
}

HBITMAP LoadPngToHBITMAP(CString path)
{
	Bitmap bmp(path);
	HBITMAP hBmp = NULL;
	bmp.GetHBITMAP(Color(255, 255, 255), &hBmp);
	return hBmp;
}
HBITMAP ResizeHBITMAP(HBITMAP hSrcBmp, int nNewWidth, int nNewHeight)
{
	// 1. 원본 HBITMAP을 GDI+ Bitmap 객체로 변환
	Bitmap* pSrcBitmap = Bitmap::FromHBITMAP(hSrcBmp, NULL);

	// 2. 새 크기의 GDI+ Bitmap 객체 생성
	Bitmap destBitmap(nNewWidth, nNewHeight);

	// 3. Graphics 객체를 사용하여 고품질로 이미지 그리기 (축소)
	Graphics graphics(&destBitmap);
	graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic); // 고품질 축소 설정

	graphics.DrawImage(pSrcBitmap, 0, 0, nNewWidth, nNewHeight);

	// 4. 축소된 GDI+ Bitmap을 다시 HBITMAP으로 변환
	HBITMAP hDestBmp = NULL;
	destBitmap.GetHBITMAP(Color(255, 255, 255), &hDestBmp);

	delete pSrcBitmap;
	return hDestBmp;
}
// C객체지향언어기말프로젝트View 인쇄

BOOL C객체지향언어기말프로젝트View::OnPreparePrinting(CPrintInfo* pInfo)
{
	return DoPreparePrinting(pInfo);
}

void C객체지향언어기말프로젝트View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

void C객체지향언어기말프로젝트View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

void C객체지향언어기말프로젝트View::OnPrint(CDC* pDC, CPrintInfo* /*pInfo*/)
{
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

C객체지향언어기말프로젝트Doc* C객체지향언어기말프로젝트View::GetDocument() const
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(C객체지향언어기말프로젝트Doc)));
	return (C객체지향언어기말프로젝트Doc*)m_pDocument;
}
#endif //_DEBUG


// C객체지향언어기말프로젝트View 메시지 처리기

void C객체지향언어기말프로젝트View::OnBnClickedUserform()
{
	if (ud == NULL) {
		ud = new UserDlg(this);
		ud->Create(IDD_User);
		ud->ShowWindow(SW_SHOW);
	}
	else {
		ud->SetFocus();
	}
}

void C객체지향언어기말프로젝트View::OnBnClickedBookform()
{
	if (bd == NULL) {
		bd = new bookDIg(this);
		bd->Create(IDD_Book_reg);
		bd->ShowWindow(SW_SHOW);
	}
	else {
		bd->SetFocus();
	}
}

void C객체지향언어기말프로젝트View::OnBnClickedLoanform()
{
}

void C객체지향언어기말프로젝트View::OnBnClickedMainsearch()
{
}

void C객체지향언어기말프로젝트View::OnNMCustomdrawList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLVCUSTOMDRAW pLVCD = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);
	*pResult = CDRF_DODEFAULT;

	switch (pLVCD->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:
		*pResult = CDRF_NOTIFYITEMDRAW;
		return;

	case CDDS_ITEMPREPAINT:
	{
		CDC* pDC = CDC::FromHandle(pLVCD->nmcd.hdc);
		int index = static_cast<int>(pLVCD->nmcd.dwItemSpec);

		CRect rc = pLVCD->nmcd.rc;

		pDC->FillSolidRect(rc, RGB(255, 255, 255));

		LVITEM lvi = { 0 };
		lvi.iItem = index;
		lvi.iSubItem = 0;
		lvi.mask = LVIF_IMAGE;
		mLc.GetItem(&lvi);
		int imgIndex = lvi.iImage;

		const int TARGET_IMG_SIZE = 120;
		const int PADDING = 40;

		int itemHeight = rc.Height();
		int imageTop = rc.top + (itemHeight - TARGET_IMG_SIZE) / 2;

		CRect imageRc(
			rc.left + PADDING,
			imageTop,
			rc.left + PADDING + TARGET_IMG_SIZE,
			imageTop + TARGET_IMG_SIZE
		);


		if (imgIndex >= 0)
		{
			mImageList.Draw(
				pDC,
				imgIndex,
				CPoint(imageRc.left, imageRc.top),
				ILD_NORMAL
			);
		}

		CString text = mLc.GetItemText(index, 0);

		CRect textRc = rc;
		textRc.left = imageRc.right + PADDING;
		textRc.right = rc.right - PADDING;
		textRc.top = rc.top; // 항목 상단에서 시작
		textRc.bottom = rc.bottom;


		// CFont* pOldFont = pDC->SelectObject(AfxGetFont(IDS_FONT_BOLD));

		pDC->DrawText(text, textRc, DT_LEFT | DT_WORDBREAK | DT_NOPREFIX | DT_TOP);

		// pDC->SelectObject(pOldFont);

		*pResult = CDRF_SKIPDEFAULT;
		return;
	}
	}

	*pResult = CDRF_DODEFAULT;
}

void C객체지향언어기말프로젝트View::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	if (nIDCtl == IDC_LIST1)
	{
		lpMeasureItemStruct->itemHeight = 240;
		return;
	}
	CFormView::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}