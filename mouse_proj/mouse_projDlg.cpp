
// mouse_projDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "mouse_proj.h"
#include "mouse_projDlg.h"
#include "afxdialogex.h"

#include"ext_func.h"
#include<time.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

char filename[100] = "data.txt";
//WCHAR* procname = NULL;
bool start_log = false;
bool first_click = false;
int time_count;
bool is_time_count = false;
HWND hWnd;

bool slow_flag = false;


inline char* UnicodeToAnsi(const wchar_t* szStr)
{
	int nLen = WideCharToMultiByte(CP_ACP, 0, szStr, -1, NULL, 0, NULL, NULL);
	if (nLen == 0)
	{
		return NULL;
	}
	char* pResult = new char[nLen];
	WideCharToMultiByte(CP_ACP, 0, szStr, -1, pResult, nLen, NULL, NULL);
	return pResult;
}
inline wchar_t* AnsiToUnicode(const char* szStr)
{
	int nLen = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szStr, -1, NULL, 0);
	if (nLen == 0)
	{
		return NULL;
	}
	wchar_t* pResult = new wchar_t[nLen];
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szStr, -1, pResult, nLen);
	return pResult;
}

BOOL InstallKbHook(){
	if (mouse_Hook)
		UninstallHook(); //键盘为：WH_KEYBOARD_LL 鼠标为：WH_MOUSE_LL
	mouse_Hook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)(LowLevelMouseProc), AfxGetApp()->m_hInstance, NULL);
	return(mouse_Hook != NULL);
}
BOOL UninstallHook()
{
	BOOL jud = FALSE;
	if (mouse_Hook) {
		jud = UnhookWindowsHookEx(mouse_Hook);
		mouse_Hook = NULL;  //set NULL  
	}
	return jud;
}


#include<vector>
typedef struct EnumHWndsArg
{
	std::vector<HWND> *vecHWnds;
	DWORD dwProcessId;
}EnumHWndsArg, *LPEnumHWndsArg;

HANDLE GetProcessHandleByID(int nID)//通过进程ID获取进程句柄
{
	return OpenProcess(PROCESS_ALL_ACCESS, FALSE, nID);
}
#include <tlhelp32.h>
//https://www.cnblogs.com/tangxin-blog/p/6077746.html
DWORD GetProcessIDByName(WCHAR* pName)
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot) {
		return NULL;
	}
	PROCESSENTRY32 pe = { sizeof(pe) };
	for (BOOL ret = Process32First(hSnapshot, &pe); ret; ret = Process32Next(hSnapshot, &pe)) {
		if (wcscmp(pe.szExeFile, pName) == 0) {
			CloseHandle(hSnapshot);
			return pe.th32ProcessID;
		}
		//printf("%-6d %s\n", pe.th32ProcessID, pe.szExeFile);
	}
	CloseHandle(hSnapshot);
	return 0;
}

LRESULT CALLBACK LowLevelMouseProc(INT nCode, WPARAM wParam, LPARAM lParam){
	MSLLHOOKSTRUCT *pkbhs = (MSLLHOOKSTRUCT *)lParam;
	if ((nCode == HC_ACTION || nCode==HC_NOREMOVE) && start_log){
		HWND curhwnd = ::GetForegroundWindow();
		HWND dsthwnd = ::FindWindowA(NULL, "KeyLogTest");
		FILE*fp;
		if (wParam == WM_KEYUP || wParam==WM_SYSKEYUP){/* || wParam == WM_LBUTTONUP*/
			if (curhwnd == dsthwnd){
				fopen_s(&fp, "debug.txt", "a");
				//fprintf(fp, "%d,%d, \n", curhwnd, dsthwnd);
				fprintf(fp, "%d,\n", wParam);
				fclose(fp);
			}
			if (dsthwnd && curhwnd == dsthwnd){
				FILE*fp;
				BYTE ks[256];
				GetKeyboardState(ks);
				WORD w;
				UINT scan = 0;
				fopen_s(&fp, filename, "a");

				TCHAR szBuf[MAXBYTE] = { 0 };
				PKBDLLHOOKSTRUCT pKeyboardHookStruct = (PKBDLLHOOKSTRUCT)lParam;
				long Flag = pKeyboardHookStruct->scanCode << 16;
				Flag |= pKeyboardHookStruct->flags << 24;
				int i = GetKeyNameText(Flag, szBuf, MAXBYTE);

				for (int j = 0; j < i; j++){
					fprintf(fp,"%c",szBuf[j]);
				}

				fclose(fp);
			}	
		}
	}

	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// Cmouse_projDlg 对话框



Cmouse_projDlg::Cmouse_projDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(Cmouse_projDlg::IDD, pParent)
	//, m_outfile(_T(""))
	, m_infile(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Cmouse_projDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_List);
	//DDX_Text(pDX, IDC_EDIT1, m_outfile);
	DDX_Text(pDX, IDC_EDIT2, m_infile);
}

BEGIN_MESSAGE_MAP(Cmouse_projDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &Cmouse_projDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &Cmouse_projDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON1, &Cmouse_projDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &Cmouse_projDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &Cmouse_projDlg::OnBnClickedButton3)
END_MESSAGE_MAP()


// Cmouse_projDlg 消息处理程序

BOOL Cmouse_projDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// 将“关于...”菜单项添加到系统菜单中。
	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码


	mouse_Hook = NULL;

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void Cmouse_projDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void Cmouse_projDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR Cmouse_projDlg::OnQueryDragIcon(){
	return static_cast<HCURSOR>(m_hIcon);
}



void Cmouse_projDlg::OnBnClickedOk(){
	InstallKbHook();

	start_log = true;
	UpdateData(true);
	char*p = UnicodeToAnsi(m_outfile.GetBuffer());

	//MessageBox(m_outfile, NULL, MB_OK);

	strcpy_s(filename, p);
	m_List.AddString(L"开始！");
	UpdateData(false);
	// TODO:  在此添加控件通知处理程序代码
	//CDialogEx::OnOK();
}

void Cmouse_projDlg::OnBnClickedCancel()
{
	UninstallHook();
	// TODO:  在此添加控件通知处理程序代码
	CDialogEx::OnCancel();
}


#include<fstream>

bool play_flag = false;

void Cmouse_projDlg::OnBnClickedButton1(){
	using namespace std;
	UpdateData(true);
	InstallKbHook();
	start_log = true;
	char*p = UnicodeToAnsi(m_infile.GetBuffer());
	strcpy_s(filename, p);
	m_List.AddString(L"开始！");
	//procname = m_outfile.GetBuffer();

	if (slow_flag == false){
		FILE*fp;
		fopen_s(&fp, filename, "w");
		fclose(fp);
		fopen_s(&fp, "debug.txt", "w");
		fclose(fp);
	}
	slow_flag = false;
	UpdateData(false);
	return;

}


void Cmouse_projDlg::OnBnClickedButton2()
{
	play_flag = false;
	m_List.AddString(L"暂停执行！");
	slow_flag = true;
	// TODO:  在此添加控件通知处理程序代码
}


void Cmouse_projDlg::OnBnClickedButton3()
{
	play_flag = false;
	Sleep(1000);//等当前动作结束
	//act_player.clear();
	m_List.AddString(L"终止执行！");
	// TODO:  在此添加控件通知处理程序代码
}
