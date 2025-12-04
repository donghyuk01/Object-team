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

	/*CString info = _T("제목: 데이터베이스 개론\n저자: 홍길동\n출판사: 한빛미디어\n대출 가능");

	mLc.InsertItem(0, info, imgIndex);*/

	db.OpenEx(_T("DSN=blob;UID=mir9876;PWD=rlaehdgur;"), CDatabase::noOdbcDialog);
	LoadBooksFromServer(_T("title"), _T(""));
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
	CString strKeyword;
	MainEc.GetWindowText(strKeyword); // Edit Control 내용

	int nSel = mCbs.GetCurSel(); // 콤보박스 (제목/저자)
	CString strType;
	if (nSel != LB_ERR) {
		mCbs.GetLBText(nSel, strType);
	}
	else {
		strType = _T("제목");
	}

	// 2. 서버 통신 함수 호출
	LoadBooksFromServer(strType, strKeyword);
}
void C객체지향언어기말프로젝트View::LoadBooksFromServer(CString type, CString keyword)
{
	CInternetSession session(_T("MyLibrarySession"));
	CHttpConnection* pConnection = NULL;
	CHttpFile* pFile = NULL;

	try {
		CString strServer = _T("localhost");
		INTERNET_PORT nPort = 8080;

		// 1. URL 생성
		CString strObject;
		// URL 인코딩을 고려하여 인코딩 되지 않은 상태의 문자열을 서버로 전달하는 것을 피합니다.
		// MFC CStringW(Unicode)가 CInternetSession을 통해 전달될 때 자동으로 UTF-8로 변환될 것이라고 가정합니다.
		strObject.Format(_T("/book/search?type=%s&keyword=%s"), type, keyword);
		pConnection = session.GetHttpConnection(strServer, nPort);

		// 2. GET 요청
		pFile = pConnection->OpenRequest(_T("GET"), strObject);
		pFile->SendRequest();

		DWORD dwRet;
		pFile->QueryInfoStatusCode(dwRet);

		if (dwRet == HTTP_STATUS_OK) {
			CString strResponse;

			// ⭐⭐⭐ 🛠️ UTF-8 인코딩 문제 해결 및 안정성 강화 ⭐⭐⭐
			DWORD dwTotalLength = 0;
			DWORD dwRead;

			// 1. 응답 데이터를 메모리로 모두 읽어옴
			CByteArray buffer;
			// 초기 크기를 명시적으로 설정 (1 바이트로 설정하면 Read 시 0을 요청하여 루프에 진입 못함)
			// 최소 4KB로 설정하거나, SetSize(0, 4096) 후 첫 Read 전에 SetSize(4096) 해주는 것이 좋습니다.
			buffer.SetSize(4096, 4096);

			// Read 함수의 반환값을 dwRead에 저장하며, 인자는 2개만 사용
			while (pFile != NULL && (dwRead = pFile->Read(buffer.GetData() + dwTotalLength, buffer.GetSize() - dwTotalLength)) > 0) {
				dwTotalLength += dwRead;

				// 버퍼 공간이 부족할 경우 확장
				if (dwTotalLength >= buffer.GetSize()) {
					buffer.SetSize(buffer.GetSize() + 4096, 4096);
				}
			}
			buffer.SetSize(dwTotalLength); // 실제 읽은 크기로 조정

			// 2. UTF-8 -> 유니코드(CString)로 변환
			if (dwTotalLength > 0) {
				int nRequiredLen = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)buffer.GetData(), dwTotalLength, NULL, 0);

				if (nRequiredLen > 0) {
					LPTSTR pszWideChar = strResponse.GetBuffer(nRequiredLen);
					MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)buffer.GetData(), dwTotalLength, pszWideChar, nRequiredLen);
					strResponse.ReleaseBuffer(nRequiredLen);
				}
			}
			// ⭐⭐⭐ 🛠️ UTF-8 인코딩 문제 해결 및 안정성 강화 끝 ⭐⭐⭐

			//... (이하 JSON 파싱 로직은 동일) ...

			ATLTRACE(_T("서버 응답 데이터: %s\n"), strResponse);

			// --- 데이터 파싱 및 리스트 업데이트 ---
			mLc.SetRedraw(FALSE);
			mLc.DeleteAllItems();

			// 이미지 리스트 초기화 (재생성)
			mImageList.DeleteImageList();
			const int FINAL_SIZE = 150;
			mImageList.Create(FINAL_SIZE, FINAL_SIZE, ILC_COLOR32 | ILC_MASK, 0, 1);
			mLc.SetImageList(&mImageList, LVSIL_SMALL);

			int nIndex = 0;

			// ⭐⭐⭐ JSON 파싱 로직 (기존과 동일) ⭐⭐⭐

			CString jsonArrayContent = strResponse.Trim();

			// 1. 배열 괄호 '[' 와 ']' 제거
			if (jsonArrayContent.GetLength() > 1 && jsonArrayContent[0] == _T('[') && jsonArrayContent[jsonArrayContent.GetLength() - 1] == _T(']')) {
				jsonArrayContent = jsonArrayContent.Mid(1, jsonArrayContent.GetLength() - 2);
			}
			else {
				if (jsonArrayContent.IsEmpty()) {
					ATLTRACE(_T("응답 데이터가 비어있습니다 (DB에 책 없음).\n"));
					goto cleanup;
				}
			}

			int nStart = 0;
			// 2. 객체별로 분리하여 루프 실행
			while (TRUE) {
				int nEnd = jsonArrayContent.Find(_T('}'), nStart);
				if (nEnd == -1) break;

				CString jsonObj = jsonArrayContent.Mid(nStart, nEnd - nStart + 1);
				jsonObj.TrimLeft(_T(" \t,"));

				// 람다 함수: 필드 추출
				auto GetField = [&](CString src, CString key) -> CString {
					CString token = key + _T("\":\"");
					int start = src.Find(token);
					if (start == -1) return _T("");
					start += token.GetLength();
					int end = src.Find(_T("\""), start);
					return src.Mid(start, end - start);
					};

				// 3. 데이터 추출 및 리스트 삽입
				CString title = GetField(jsonObj, _T("title"));
				CString author = GetField(jsonObj, _T("author"));
				CString publisher = GetField(jsonObj, _T("publisher"));
				CString status = GetField(jsonObj, _T("status"));
				CString imgPath = GetField(jsonObj, _T("imagePath"));

				// 이미지 로드 (기존과 동일)
				int imgIdx = -1;
				if (!imgPath.IsEmpty()) {
					CString fullImgUrl;
					fullImgUrl.Format(_T("http://localhost:8080%s"), imgPath);

					HBITMAP hBmpRaw = LoadImageFromURL(fullImgUrl);
					if (hBmpRaw) {
						HBITMAP hBmpResized = ResizeHBITMAP(hBmpRaw, FINAL_SIZE, FINAL_SIZE);
						CBitmap bmp;
						bmp.Attach(hBmpResized);
						imgIdx = mImageList.Add(&bmp, RGB(255, 255, 255));
						bmp.Detach();
						DeleteObject(hBmpRaw);
					}
				}

				// 리스트 컨트롤 텍스트 구성
				CString info;
				if (status.IsEmpty()) status = _T("대출 가능");

				info.Format(_T("제목: %s\n저자: %s\n출판사: %s\n%s"), title, author, publisher, status);

				mLc.InsertItem(nIndex++, info, imgIdx);

				// 다음 객체 시작점 설정
				nStart = nEnd + 1;
			}
			// ⭐⭐⭐ JSON 파싱 로직 끝 ⭐⭐⭐

		cleanup:
			mLc.SetRedraw(TRUE);
		}
	}
	catch (CInternetException* pEx) {
		// 오류가 발생하면 (예: 404 Not Found, 연결 끊김 등) 여기서 처리됩니다.
		pEx->ReportError();
		pEx->Delete();
	}

	// 3. 리소스 정리
	if (pFile) { pFile->Close(); delete pFile; }
	if (pConnection) { pConnection->Close(); delete pConnection; }
	session.Close();

	mLc.Invalidate();
}

HBITMAP C객체지향언어기말프로젝트View::LoadImageFromURL(CString url)
{
	CInternetSession session;
	CHttpFile* pFile = NULL;
	HBITMAP hResult = NULL;

	try {
		pFile = (CHttpFile*)session.OpenURL(url);
		if (pFile) {
			// 파일을 메모리로 읽음
			DWORD dwFileSize = (DWORD)pFile->GetLength(); // 사이즈를 모를 수도 있음

			CFile tempFile;
			// 임시 경로에 저장했다가 로드하는 방식 (GDI+ Bitmap이 파일 경로를 좋아함)
			TCHAR szPath[MAX_PATH];
			GetTempPath(MAX_PATH, szPath);
			CString tempPath;
			tempPath.Format(_T("%stemp_img_%u.png"), szPath, GetTickCount());

			if (tempFile.Open(tempPath, CFile::modeCreate | CFile::modeWrite)) {
				const int BUF_SIZE = 4096;
				BYTE buffer[BUF_SIZE];
				UINT nRead;
				while ((nRead = pFile->Read(buffer, BUF_SIZE)) > 0) {
					tempFile.Write(buffer, nRead);
				}
				tempFile.Close();

				// 기존 LoadPngToHBITMAP 함수 재사용
				hResult = LoadPngToHBITMAP(tempPath);

				// 임시 파일 삭제
				DeleteFile(tempPath);
			}
		}
	}
	catch (...) {
		// 오류 처리
	}

	if (pFile) { pFile->Close(); delete pFile; }
	session.Close();

	return hResult;
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