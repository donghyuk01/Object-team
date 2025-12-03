// bookDIg.cpp: 구현 파일
//

#include "pch.h"
#include "객체지향언어기말프로젝트.h"
#include "afxdialogex.h"
#include "bookDIg.h"

#include "객체지향언어기말프로젝트Doc.h"
#include "객체지향언어기말프로젝트View.h"

#include <afxinet.h>
#include <string>
#include <afxdlgs.h>

// 서버 주소
#define SERVER_IP _T("127.0.0.1")
#define SERVER_PORT 8080

IMPLEMENT_DYNAMIC(bookDIg, CDialogEx)

bookDIg::bookDIg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_Book_reg, pParent)
{
	bookView = (C객체지향언어기말프로젝트View*)pParent;
}

bookDIg::~bookDIg()
{
}

void bookDIg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, bec1);
	DDX_Control(pDX, IDC_EDIT2, bec2);
	DDX_Control(pDX, IDC_EDIT3, bec3);
	DDX_Control(pDX, IDC_COMBO1, bcb1);
}

BEGIN_MESSAGE_MAP(bookDIg, CDialogEx)
	ON_BN_CLICKED(ID_BookReg, &bookDIg::OnBnClickedBookreg)
	ON_BN_CLICKED(IDC_BookUpdate, &bookDIg::OnBnClickedBookupdate)
	ON_BN_CLICKED(IDC_BookDel, &bookDIg::OnBnClickedBookdel)
	ON_BN_CLICKED(IDC_BookImg, &bookDIg::OnBnClickedBookimg)
	ON_BN_CLICKED(IDC_BookClose, &bookDIg::OnBnClickedBookclose)
END_MESSAGE_MAP()


// -----------------------------
// 도서 등록 (POST /book)
// -----------------------------
void bookDIg::OnBnClickedBookreg()
{


	CString strTitle, strAuthor, strPublisher, strCategory;

	USES_CONVERSION;

	// JSON 문자열 생성 시 UTF-8 변환 적용
	// strCategory, strTitle, strAuthor, strPublisher가 CStringW (유니코드)라고 가정
	

	bec1.GetWindowText(strTitle);
	bec2.GetWindowText(strAuthor);
	bec3.GetWindowText(strPublisher);

	int nCurSel = bcb1.GetCurSel();
	if (nCurSel != CB_ERR)
		bcb1.GetLBText(nCurSel, strCategory);

	if (strTitle.IsEmpty() || strAuthor.IsEmpty() || strCategory.IsEmpty())
	{
		AfxMessageBox(_T("제목, 저자, 카테고리는 필수 입력 사항입니다."));
		return;
	}

	// ---------------------------------------------------------
	// 이미지 파일 → Base64 변환
	// ---------------------------------------------------------
	CStringA base64Image = "";
	if (!m_strImagePath.IsEmpty())
	{
		CFile file;
		if (file.Open(m_strImagePath, CFile::modeRead | CFile::typeBinary))
		{
			DWORD fsize = file.GetLength();
			BYTE* buffer = new BYTE[fsize];

			file.Read(buffer, fsize);
			file.Close();

			base64Image = Base64Encode(buffer, fsize);
			delete[] buffer;
		}
	}

	// ---------------------------------------------------------
	// JSON 본문 생성
	// ---------------------------------------------------------
	CStringA category_utf8 = CW2A(strCategory, CP_UTF8);
	CStringA title_utf8 = CW2A(strTitle, CP_UTF8);
	CStringA author_utf8 = CW2A(strAuthor, CP_UTF8);
	CStringA publisher_utf8 = CW2A(strPublisher, CP_UTF8);

	// JSON 본문 생성
	CStringA json;
	json += "{";
	json += "\"title\":\"" + title_utf8 + "\",";
	json += "\"author\":\"" + author_utf8 + "\",";
	json += "\"category\":\"" + category_utf8 + "\","; // UTF-8 변환된 카테고리 사용
	json += "\"publisher\":\"" + publisher_utf8 + "\",";
	json += "\"imageBase64\":\"" + base64Image + "\"";
	json += "}";

	// ---------------------------------------------------------
	// HTTP POST 요청
	// ---------------------------------------------------------
	CInternetSession session(_T("BookClientJSON"));
	CHttpConnection* pConnection = nullptr;
	CHttpFile* pFile = nullptr;

	try
	{
		pConnection = session.GetHttpConnection((LPCTSTR)SERVER_IP, (INTERNET_PORT)SERVER_PORT);

		pFile = pConnection->OpenRequest(
			CHttpConnection::HTTP_VERB_POST,
			_T("/book"),
			NULL, 1, NULL, NULL,
			INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE
		);

		CString strHeaders = _T("Content-Type: application/json\r\n");

		pFile->SendRequest(strHeaders, (DWORD)strHeaders.GetLength(),
			(LPVOID)(LPCSTR)json, json.GetLength());

		DWORD dwStatus = 0;
		pFile->QueryInfoStatusCode(dwStatus);

		if (dwStatus == 200 || dwStatus == 201)
		{
			AfxMessageBox(_T("도서 등록 성공!"));
			OnBnClickedBookclose();
		}
		else
		{
			CString msg;
			msg.Format(_T("서버 응답 코드 : %u"), dwStatus);
			AfxMessageBox(msg);
		}
	}
	catch (CInternetException* e)
	{
		TCHAR err[512];
		e->GetErrorMessage(err, 512);
		AfxMessageBox(err);
		e->Delete();
	}

	if (pFile) { pFile->Close(); delete pFile; }
	if (pConnection) { pConnection->Close(); delete pConnection; }
	session.Close();
}

void bookDIg::OnBnClickedBookupdate()
{
	// TODO
}

void bookDIg::OnBnClickedBookdel()
{
	// TODO
}

void bookDIg::OnBnClickedBookimg()
{
	CFileDialog dlg(TRUE, _T("png"), NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		_T("Image Files (*.png;*.jpg;*.jpeg)|*.png;*.jpg;*.jpeg|All Files (*.*)|*.*||"),
		this);

	if (dlg.DoModal() == IDOK)
	{
		m_strImagePath = dlg.GetPathName();
		AfxMessageBox(_T("이미지 파일 경로가 성공적으로 저장되었습니다:\n") + m_strImagePath);
	}
}

void bookDIg::OnBnClickedBookclose()
{
	bookView->bd = NULL;
	DestroyWindow();
	delete this;
}

BOOL bookDIg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	bcb1.AddString(_T("철학"));
	bcb1.AddString(_T("종교"));
	bcb1.AddString(_T("사회과학"));
	bcb1.AddString(_T("자연과학"));
	bcb1.AddString(_T("기술과학"));
	bcb1.AddString(_T("예술"));
	bcb1.AddString(_T("언어"));
	bcb1.AddString(_T("문학"));
	bcb1.AddString(_T("역사"));
	bcb1.AddString(_T("기타"));
	bcb1.SetCurSel(0);

	return TRUE;
}
CStringA bookDIg::Base64Encode(const BYTE* pData, DWORD length)
{
	static const char base64Chars[] =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	CStringA encoded;
	DWORD outLen = 4 * ((length + 2) / 3);
	encoded.Preallocate(outLen);

	DWORD i = 0;
	while (i < length)
	{
		DWORD triple = 0;
		int bytes = 0;

		for (int j = 0; j < 3; j++)
		{
			triple <<= 8;
			if (i < length)
			{
				triple |= pData[i++];
				bytes++;
			}
		}

		for (int j = 0; j < 4; j++)
		{
			if (j <= (bytes))
			{
				int idx = (triple >> (18 - 6 * j)) & 0x3F;
				encoded += base64Chars[idx];
			}
			else
			{
				encoded += '=';
			}
		}
	}

	return encoded;
}