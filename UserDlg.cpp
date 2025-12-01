// UserDlg.cpp: 구현 파일
//

#include "pch.h"
#include "객체지향언어기말프로젝트.h"
#include "afxdialogex.h"
#include "UserDlg.h"

#include"객체지향언어기말프로젝트Doc.h"
#include"객체지향언어기말프로젝트View.h"
// UserDlg 대화 상자

IMPLEMENT_DYNAMIC(UserDlg, CDialogEx)

UserDlg::UserDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_User, pParent)
{
	userView = (C객체지향언어기말프로젝트View*)pParent;
}

UserDlg::~UserDlg()
{
}

void UserDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT5, userName);
	DDX_Control(pDX, IDC_EDIT8, birth);
	DDX_Control(pDX, IDC_EDIT9, maxLoan);
	DDX_Control(pDX, IDC_EDIT10, solveDate);
	DDX_Control(pDX, IDC_COMBO1, statusBox);
	DDX_Control(pDX, IDC_LIST1, user_lc);
}


BEGIN_MESSAGE_MAP(UserDlg, CDialogEx)
	ON_BN_CLICKED(ID_UserCancel, &UserDlg::OnBnClickedUsercancel)
	ON_BN_CLICKED(IDC_BUTTON5, &UserDlg::OnBnClickedButton5)
END_MESSAGE_MAP()


// UserDlg 메시지 처리기
BOOL UserDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  여기에 추가 초기화 작업을 추가합니다.
    statusBox.ResetContent();
    statusBox.AddString(_T("정상"));
    statusBox.AddString(_T("연체 정지"));
    statusBox.AddString(_T("자격 정지"));
    statusBox.AddString(_T("탈퇴"));
    statusBox.SetCurSel(0);

    user_lc.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

    
    user_lc.InsertColumn(0, _T("도서 ID"), LVCFMT_LEFT, 80); 
    user_lc.InsertColumn(1, _T("바코드"), LVCFMT_LEFT, 100);
    user_lc.InsertColumn(2, _T("대출일"), LVCFMT_LEFT, 120);
    user_lc.InsertColumn(3, _T("반납 예정일"), LVCFMT_LEFT, 120);
    user_lc.InsertColumn(4, _T("상태"), LVCFMT_LEFT, 80);
    user_lc.InsertColumn(5, _T("연체일수"), LVCFMT_RIGHT, 80);

    return TRUE;  // return TRUE unless you set the focus to a control
    // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void UserDlg::OnBnClickedUsercancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	userView->ud = NULL;
	DestroyWindow();
	delete this;
}


void UserDlg::OnBnClickedButton5()
{
    
    CString strName, strBirth;
    userName.GetWindowText(strName);
    birth.GetWindowText(strBirth);

   
    if (strName.IsEmpty() || strBirth.IsEmpty())
    {
        AfxMessageBox(_T("이름과 생년월일을 모두 입력해주세요."));
        return;
    }

    
    CDatabase* pDB = &userView->db;

    
    if (pDB == NULL || !pDB->IsOpen())
    {
        AfxMessageBox(_T("데이터베이스 연결 상태를 확인해주세요."));
        return;
    }

    
    CString strSQL;
    strSQL.Format(_T("SELECT userID, maxLoan, solveDate, status FROM mir9876.User WHERE name='%s' AND birth='%s'"),
        strName, strBirth);

    CRecordset rs(pDB);
    TRY
    {

        if (!rs.Open(CRecordset::forwardOnly, strSQL))
        {
            AfxMessageBox(_T("회원 조회 쿼리 실행에 실패했습니다."));
            return;
        }

    
    if (rs.IsEOF())
    {
        
        AfxMessageBox(_T("입력하신 이름과 생년월일로 등록된 회원이 없습니다."));

        
        maxLoan.SetWindowText(_T(""));
        solveDate.SetWindowText(_T(""));
        statusBox.SetCurSel(-1); 
    }
    else
    {
        
        CString strUserID, strMaxLoan, strSolveDate, strStatus;

        
        rs.GetFieldValue(_T("userID"), strUserID);
        rs.GetFieldValue(_T("maxLoan"), strMaxLoan);
        rs.GetFieldValue(_T("solveDate"), strSolveDate);
        rs.GetFieldValue(_T("status"), strStatus);

        
        maxLoan.SetWindowText(strMaxLoan);

        
        if (strSolveDate.IsEmpty() || strSolveDate == _T("0000-00-00"))
        {
            solveDate.SetWindowText(_T("없음"));
        }
        else
        {
            solveDate.SetWindowText(strSolveDate);
        }

        
        int index = statusBox.FindStringExact(-1, strStatus);
        if (index != CB_ERR)
        {
            statusBox.SetCurSel(index);
        }
        else
        {
            statusBox.SetCurSel(-1);
        }

  
        // LoadUserLoanList(strUserID); 
    }
    }
        CATCH(CDBException, e)
    {
        
        AfxMessageBox(e->m_strError);
        
        maxLoan.SetWindowText(_T(""));
        solveDate.SetWindowText(_T(""));
        statusBox.SetCurSel(-1);
    }
    END_CATCH

        rs.Close(); 
}


