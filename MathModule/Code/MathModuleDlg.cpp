
// MathModuleDlg.cpp: 实现文件
//

#include <string>

#include "pch.h"
#include "framework.h"
#include "MathModule.h"
#include "MathModuleDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMathModuleDlg 对话框

// 样本数据
CMathModuleDlg::AxisPoint Sample_alcohol_concentration_points[23] = {
	{0.25f, 30.0f}, {0.5f, 68.0f},  {0.75f, 75.0f}, {1.0f, 82.0f},  {1.5f, 82.0f},
	{2.0f, 77.0f},  {2.5f, 68.0f},  {3.0f, 68.0f},  {3.5f, 58.0f},  {4.0f, 51.0f},
	{4.5f, 50.0f},  {5.0f, 41.0f},  {6.0f, 38.0f},  {7.0f, 35.0f},  {8.0f, 28.0f},
	{9.0f, 25.0f},  {10.0f, 18.0f}, {11.0f, 15.0f}, {12.0f, 10.0f}, {13.0f, 10.0f},
	{14.0f, 7.0f},  {15.0f, 7.0f},  {16.0f, 4.0f}
};

CMathModuleDlg::CMathModuleDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MATHMODULE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMathModuleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER_BOTTLE, sld_Bottle);
	DDX_Control(pDX, IDC_SLIDER_A1, sld_A);
	DDX_Control(pDX, IDC_SLIDER_A2, sld_a);
	DDX_Control(pDX, IDC_SLIDER_K, sld_k);
	DDX_Control(pDX, IDC_SLIDER_TIME, sld_time);
	DDX_Control(pDX, IDC_SLIDER_DRINKTIME, sld_drinktime);

	DDX_Control(pDX, IDC_STATIC_BOTTLE, lbl_Bottle);
	DDX_Control(pDX, IDC_STATIC_A1, lbl_A);
	DDX_Control(pDX, IDC_STATIC_A2, lbl_a);
	DDX_Control(pDX, IDC_STATIC_K, lbl_k);
	DDX_Control(pDX, IDC_STATIC_TIME, lbl_time);
	DDX_Control(pDX, IDC_STATIC_DRINKTIME, lbl_drinktime);

	DDX_Control(pDX, IDC_STATIC_YLABEL, lbl_ylabel);
	DDX_Control(pDX, IDC_LIST_PARAMETERS, lst_Parameters);
	DDX_Control(pDX, IDC_LIST_SAMPLE_DATA, lst_SampleData);
	DDX_Control(pDX, IDC_STATIC_OPTIMALCOEFFICIENTS, lbl_OptimalCoefficients);
}

BEGIN_MESSAGE_MAP(CMathModuleDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_HSCROLL()
	ON_WM_DESTROY()
	ON_LBN_SELCHANGE(IDC_LIST_PARAMETERS, &CMathModuleDlg::OnLbnSelchangeListParameters)
	ON_BN_CLICKED(IDC_BUTTON_SAVEPARAMETERS, &CMathModuleDlg::OnBnClickedButtonSaveParameters)
	ON_BN_CLICKED(IDC_BUTTON_GETINFORMATION, &CMathModuleDlg::OnBnClickedButtonGetInformaion)
	ON_BN_CLICKED(IDC_BUTTON_AUTOCALCPARAMETERS, &CMathModuleDlg::OnBnClickedButtonAutocalcparameters)
END_MESSAGE_MAP()


// CMathModuleDlg 消息处理程序

BOOL CMathModuleDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// 读取文件中的参数值并显示在列表框中
	HANDLE hFile = CreateFile(_T("Alcohol_Concentration_Data.txt"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (hFile != INVALID_HANDLE_VALUE) {
		DWORD dwFileSize = GetFileSize(hFile, nullptr);
		if (dwFileSize > 0) {
			WCHAR* Buffer = (WCHAR*)calloc(dwFileSize / sizeof(WCHAR) + 1, sizeof(WCHAR)); // +1 用于 null 结尾
			if(Buffer != nullptr) {
				DWORD dwBytesRead = 0;
				BOOL RTN = ReadFile(hFile, Buffer, dwFileSize, &dwBytesRead, nullptr);
				if (!RTN) {
					MessageBox(_T("Failed to read file!"), _T("Error"), MB_ICONERROR);
					free(Buffer);
				}else {
					Buffer[dwBytesRead / sizeof(WCHAR)] = '\0'; // 确保字符串以 null 结尾
					CString strData(Buffer, dwBytesRead / sizeof(WCHAR));
					free(Buffer);
					int pos = 0;
					CString strLine;
					while ((pos = strData.Find(_T("\r\n"))) != -1) {
						strLine = strData.Left(pos);
						lst_Parameters.AddString(strLine); // 将每行数据添加到列表框中
						strData = strData.Mid(pos + 2); // 移除已处理的行
					}
					if (!strData.IsEmpty()) {
						lst_Parameters.AddString(strData); // 添加最后一行（如果存在）
					}
				}
			}else {
				MessageBox(_T("Memory allocation failed!"), _T("Error"), MB_ICONERROR);
			}
		}
		CloseHandle(hFile);
	}else {
		MessageBox(_T("Failed to create or open file!"), _T("Error"), MB_ICONERROR);
	}

	/* 设置滑动条的范围(参数范围)
	* Bottle: (1 -- 20)
	* A: (105 -- 120)
	* a: (1.5 -- 2.5)
	* k: (0.1 -- 0.25)
	*/
	sld_Bottle.SetRange(1, 20);
	sld_Bottle.SetPos(2);
	sld_A.SetRange(10500, 12000);	// 设置酒精计量系数的范围 (要除以 100)
	sld_a.SetRange(150, 250);		// 设置吸收速率常数的范围 (要除以 100)
	sld_k.SetRange(100, 250);		// 设置代谢消除速率常数的范围 (要除以 1000)
	sld_time.SetRange(1, 50);		
	sld_time.SetPos(30);
	sld_drinktime.SetRange(0, 4);
	for (int Index = 0;	 Index < 23; Index++){
		CString content;
		content.Format(L"%.2f  %.2f", Sample_alcohol_concentration_points[Index].x, Sample_alcohol_concentration_points[Index].y);
		lst_SampleData.AddString(content); // 将结果添加到列表框中
	}

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMathModuleDlg::OnDestroy() {
	CDialogEx::OnDestroy();
	HANDLE hFile = CreateFile(_T("Alcohol_Concentration_Data.txt"), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (hFile == INVALID_HANDLE_VALUE) {
		MessageBox(_T("Failed to create or open file!"), _T("Error"), MB_ICONERROR);
		return;
	}
	for(int i = 0; i < lst_Parameters.GetCount(); i++) {
		CString strParameter;
		lst_Parameters.GetText(i, strParameter);
		strParameter += _T("\r\n"); // 添加换行符
		DWORD dwBytesWritten;
		WriteFile(hFile, strParameter.GetBuffer(), strParameter.GetLength() * sizeof(TCHAR), &dwBytesWritten, nullptr);
	}
	CloseHandle(hFile);
}

void CMathModuleDlg::OnPaint()
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
		//pPaintDC = new CPaintDC(this); // 用于绘制的设备上下文
		CFont font;
		font.CreateFont(
			20, 0, -900, -900, FW_NORMAL, FALSE, FALSE, 0,
			DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial")
		);
		lbl_ylabel.SetFont(&font); // 设置 y 轴标签的字体
		Display_Alcohol_Concentration(2, 105, 1.2f, 0.1f); // 绘制酒精浓度表格
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMathModuleDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMathModuleDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) {
	UINT SliderID = pScrollBar->GetDlgCtrlID();
	CString content;
	int Bottle = 0;
	// 将整数值转换为小数
	float alcohol_dosage_coefficient = 0.0f;
	float absorption_rate_constant = 0.0f; 
	float elimination_rate_constant = 0.0f;
	int hour = 0;
	int drinktime = 0;
	Get_Parameters_From_SbliderBox(Bottle, alcohol_dosage_coefficient, absorption_rate_constant, elimination_rate_constant, hour, drinktime);
	if (SliderID == IDC_SLIDER_BOTTLE) {
		content.Format(L"m(%d)", Bottle);
		lbl_Bottle.SetWindowTextW(content);
	}
	else if (SliderID == IDC_SLIDER_A1) {
		content.Format(L"A(%.2f)", alcohol_dosage_coefficient);
		lbl_A.SetWindowTextW(content);
	}
	else if (SliderID == IDC_SLIDER_A2) {
		content.Format(L"a(%.2f)", absorption_rate_constant);
		lbl_a.SetWindowTextW(content);
	}
	else if (SliderID == IDC_SLIDER_K) {
		content.Format(L"k(%.2f)", elimination_rate_constant);
		lbl_k.SetWindowTextW(content);
	}
	else if (SliderID == IDC_SLIDER_TIME) {
		content.Format(L"time(%d)", hour);
		lbl_time.SetWindowTextW(content);
	}
	else if (SliderID == IDC_SLIDER_DRINKTIME) {
		content.Format(L"drink(%d)", drinktime);
		lbl_drinktime.SetWindowTextW(content);
	}
	else {
		MessageBox(_T("Unknown slider control!"), _T("Error"), MB_ICONERROR);
		return;
	}
	if (HScroll_IsPaint) {
		Display_Alcohol_Concentration(
			Bottle,
			alcohol_dosage_coefficient,
			absorption_rate_constant,
			elimination_rate_constant
		);
	}
}

void CMathModuleDlg::OnLbnSelchangeListParameters() {
	int selIndex = lst_Parameters.GetCurSel();
	if (selIndex == LB_ERR) {
		MessageBox(_T("Please select a set of parameters!"), _T("Warning"), MB_ICONWARNING);
		return;
	}
	CString selectedParameter;
	lst_Parameters.GetText(selIndex, selectedParameter);
	int Bottle = 0;
	float alcohol_dosage_coefficient = 0.0f;
	float absorption_rate_constant = 0.0f;
	float elimination_rate_constant = 0.0f;
	int hour = 0;
	int drinktime = 0;

	int c = swscanf_s(
		selectedParameter,
		L"m:%d, A: %f, a: %f, k: %f, time: %d, drink: %d",
		&Bottle,
		&alcohol_dosage_coefficient,
		&absorption_rate_constant,
		&elimination_rate_constant,
		&hour,
		&drinktime);
	if (swscanf_s(
		selectedParameter,
		L"m:%d, A: %f, a: %f, k: %f, time: %d, drink: %d",
		&Bottle,
		&alcohol_dosage_coefficient,
		&absorption_rate_constant,
		&elimination_rate_constant,
		&hour,
		&drinktime) != 6) {
		MessageBox(_T("Parameter format error!"), _T("Error"), MB_ICONERROR);
		return;
	}
	CString content;
	int A = int(alcohol_dosage_coefficient * 100),
		a = int(absorption_rate_constant * 100),
		k = int(elimination_rate_constant * 1000);
	content.Format(L"m(%d)", Bottle);
	lbl_Bottle.SetWindowTextW(content);
	content.Format(L"A(%.2f)", alcohol_dosage_coefficient);
	lbl_A.SetWindowTextW(content);
	content.Format(L"a(%.2f)", absorption_rate_constant);
	lbl_a.SetWindowTextW(content);
	content.Format(L"k(%.2f)", elimination_rate_constant);
	lbl_k.SetWindowTextW(content);
	sld_Bottle.SetPos(Bottle);
	sld_A.SetPos(A);
	sld_a.SetPos(a);
	sld_k.SetPos(k);
	Display_Alcohol_Concentration(
		Bottle,
		alcohol_dosage_coefficient,
		absorption_rate_constant,
		elimination_rate_constant
	);
}

void CMathModuleDlg::OnBnClickedButtonSaveParameters() {
	CString content;
	int Bottle = 0;
	float alcohol_dosage_coefficient = 0.0f;
	float absorption_rate_constant = 0.0f;
	float elimination_rate_constant = 0.0f;
	int hour = 0;
	int divCount = 0;
	Get_Parameters_From_SbliderBox(Bottle, alcohol_dosage_coefficient, absorption_rate_constant, elimination_rate_constant, hour, divCount);
	content.Format(L"m:%d, A: %0.2f, a: %.2f, k: %.2f, time: %d, drink: %d", Bottle, alcohol_dosage_coefficient, absorption_rate_constant, elimination_rate_constant, hour, divCount);
	lst_Parameters.AddString(content);

}

void CMathModuleDlg::OnBnClickedButtonGetInformaion(){
	int Bottle = sld_Bottle.GetPos();
	int Count = sld_time.GetPos() * 200 + 1;
	PtrAxisPoint pAxisPoint = reinterpret_cast<PtrAxisPoint>(pResultData);
	CString content; bool IsDrink = false;
	content.Format(L"喝 %d 瓶3.3%%vol啤酒\n", Bottle);
	content.AppendFormat(L"酒精含量在 20 mg / 100 ml 左右结果如下\n");
	for (int i = 0; i <= Count; i++) {
		if (pAxisPoint[i].y > 20 && IsDrink == false) {
			content.AppendFormat(L"起始为 t = %0.4f hour, C = %0.4f mg / 100ml \n", pAxisPoint[i - 1].x, pAxisPoint[i - 1].y);
			IsDrink = true;
		}
		if (pAxisPoint[i].y < 20 && IsDrink == true) {
			content.AppendFormat(L"结束为 t = %0.4f hour, C = %0.4f mg / 100ml \n", pAxisPoint[i - 1].x, pAxisPoint[i - 1].y);
			IsDrink = false;
			break;
		}
	}
	MessageBox(content, _T("Result"), MB_ICONINFORMATION);
}

void CMathModuleDlg::OnBnClickedButtonAutocalcparameters() {
	sld_drinktime.SetPos(0);
	const int threadCount = 16; // 比如 A 从 105 到 121，跨度16，开16个线程
	HANDLE* hThreads = new HANDLE[threadCount];
	OptimalParameter* pParams = new OptimalParameter[threadCount]; // 在堆上分配，防止被销毁

	// 1. 创建并启动所有子线程
	for (int i = 0; i < threadCount; ++i) {
		pParams[i].A = 105.0f + i * 1.0f; // 给每个线程分配 A 的区间起点
		pParams[i].pClassBase = this; // 传入当前类的指针，供线程函数调用成员函数
		pParams[i].parameters.error = FLT_MAX;

		hThreads[i] = CreateThread(
			nullptr,
			0,
			CalcOptimalCoefficientsCore, // 线程函数
			&pParams[i],                 // 传入堆上分配的结构体地址
			0,
			nullptr
		);
	}

	// 2. 主线程在这里死等，直到所有子线程全部计算完毕
	WaitForMultipleObjects(threadCount, hThreads, TRUE, INFINITE);

	// 3. 所有线程结束后，汇总结果并更新界面（此时更新界面绝对安全）
	double globalMinError = FLT_MAX;
	OptimalParameter finalResult = { 0 };

	for (int i = 0; i < threadCount; ++i) {
		// 找出所有线程里误差最小的那个
		if (pParams[i].parameters.error < globalMinError) {
			globalMinError = pParams[i].parameters.error;
			finalResult = pParams[i];
			CString content;
			content.Format(L"Optimal Coefficients: m=2, A=%.2f, a=%.2f, k=%.2f \nRMSE(Error) : %0.2f mg / 100ml   R - Squared : %0.4f",
							pParams[i].parameters.A, pParams[i].parameters.a, pParams[i].parameters.k, pParams[i].parameters.error, pParams[i].parameters.R_Squared);
			lbl_OptimalCoefficients.SetWindowTextW(content);
			Display_Alcohol_Concentration(2, pParams[i].parameters.A, pParams[i].parameters.a, pParams[i].parameters.k);
			Sleep(50); // 每个结果显示50毫秒，形成动态更新的效果
		}
		CloseHandle(hThreads[i]); // 记得关闭句柄释放资源
	}
	sld_A.SetPos(int(finalResult.parameters.A * 100));
	sld_a.SetPos(int(finalResult.parameters.a * 100));
	sld_k.SetPos(int(finalResult.parameters.k * 1000));
	HScroll_IsPaint = false;
	SendMessage(WM_HSCROLL, MAKEWPARAM(SB_THUMBPOSITION, int(0)), (LPARAM)sld_Bottle.m_hWnd);
	SendMessage(WM_HSCROLL, MAKEWPARAM(SB_THUMBPOSITION, int(finalResult.parameters.A * 100)), (LPARAM)sld_A.m_hWnd);
	SendMessage(WM_HSCROLL, MAKEWPARAM(SB_THUMBPOSITION, int(finalResult.parameters.a * 100)), (LPARAM)sld_a.m_hWnd);
	SendMessage(WM_HSCROLL, MAKEWPARAM(SB_THUMBPOSITION, int(finalResult.parameters.k * 1000)), (LPARAM)sld_k.m_hWnd);
	SendMessage(WM_HSCROLL, MAKEWPARAM(SB_THUMBPOSITION, int(0)), (LPARAM)sld_time.m_hWnd);
	SendMessage(WM_HSCROLL, MAKEWPARAM(SB_THUMBPOSITION, int(0)), (LPARAM)sld_drinktime.m_hWnd);
	HScroll_IsPaint = true;
	// 5. 释放堆内存
	delete[] hThreads;
	delete[] pParams;
}

void CMathModuleDlg::Get_Parameters_From_SbliderBox(OUT int& Bottle, OUT float& A, OUT float& a, OUT float& k, OUT int& hour, OUT int& drinktime){
	Bottle = sld_Bottle.GetPos();
	A = sld_A.GetPos() / 100.0f;
	a = sld_a.GetPos() / 100.0f; // 将整数值转换为小数
	k = sld_k.GetPos() / 1000.0f;
	hour = sld_time.GetPos();
	drinktime = sld_drinktime.GetPos();
}

/*
* 自变量（时间）：time_hour（单位：小时）或 time_minute（单位：分钟）。
* 因变量（酒精含量）：alcohol_concentration（酒精浓度）。
* 公式函数名：calculate_alcohol_concentration（计算酒精浓度的函数）
* 
* A：酒精计量系数（看你喝酒喝了多少）				Name : alcohol_dosage_coefficient
* a：吸收速率常数（判定酒精在你体内吸收的快慢）     Name : absorption_rate_constant
* k：代谢消除速率常数（看酒精在你体内代谢的快慢）   Name : elimination_rate_constant
*/
int CMathModuleDlg::Calculate_Alcohol_Concentration(
	IN int Bottle,
	IN float alcohol_dosage_coefficient,
	IN float absorption_rate_constant,
	IN float elimination_rate_constant,
	OUT PtrAxisPoint& ptr_alcohol_concentration
) {
	if (!Bottle) {
		MessageBox(_T("Invalid Parameters: Bottle!"), _T("Warning"), MB_ICONWARNING);
		return 0;
	}
	if (alcohol_dosage_coefficient <= 0 || absorption_rate_constant <= 0 || elimination_rate_constant <= 0) {
		MessageBox(_T("Alcohol dosage coefficient, absorption rate constant, and elimination rate constant must be positive numbers!"), _T("Error"), MB_ICONERROR);
		return 0;
	}
	if ((absorption_rate_constant <= elimination_rate_constant)) {
		MessageBox(_T("Absorption rate constant must be greater than elimination rate constant!"), _T("Warning"), MB_ICONWARNING);
		return 0;
	}

	ptr_alcohol_concentration = static_cast<PtrAxisPoint>(calloc(sld_time.GetPos() * 200 + 1, sizeof(AxisPoint))); // 存储 0 到 16 小时的酒精浓度数据
	if (ptr_alcohol_concentration == nullptr) {
		MessageBox(_T("Memory allocation failed!"), _T("Error"), MB_ICONERROR);
		return 0;
	}
	int count = 0;
	int Max_hour = sld_time.GetPos();
	int drinktime = sld_drinktime.GetPos();
	for (float time_hour = 0; time_hour <= Max_hour; time_hour += 0.005f) {

		float alcohol_concentration = 0.0f;
		if (drinktime) {
			alcohol_concentration = GetAlcoholConcentrationFormulaWithUniformRate(
				Bottle,
				alcohol_dosage_coefficient,
				absorption_rate_constant,
				elimination_rate_constant,
				time_hour,
				drinktime
			);
		}else{
			alcohol_concentration = GetAlcoholConcentrationFormula(
				Bottle,
				alcohol_dosage_coefficient,
				absorption_rate_constant,
				elimination_rate_constant,
				time_hour
			);
		}
		ptr_alcohol_concentration[count].x = time_hour; // 时间（小时）
		ptr_alcohol_concentration[count++].y = alcohol_concentration; // 酒精浓度
	}
	return count;
}

void CMathModuleDlg::InitTableFramework(IN CDC* pDC) {
	if (pDC == nullptr) {
		MessageBox(_T("Device context pointer must not be NULL!"), _T("Error"), MB_ICONERROR);
		return;
	}

	CRect rect = TableFrameWorkToRect(&tableFrameWork);
	CBrush BlackBrush(RGB(0, 0, 0)); // 黑色刷子
	pDC->DrawEdge(&rect, EDGE_BUMP, BF_RECT); // 填充背景
	pDC->SelectObject(&BlackBrush); // 选择刷子
	
	CString Precision;
	for(int Row = 0; Row <= 10; Row++) {
		pDC->MoveTo(rect.left, rect.top + Row * cell.Uint_Height); // 水平线
		pDC->LineTo(rect.right, rect.top + Row * cell.Uint_Height);
		float MaxY = static_cast<float>(Bottle_one_Max_Concentration * sld_Bottle.GetPos());
		float stepY = MaxY / 10.0f;
		tableFrameWork.PrecisionY = static_cast<int>(stepY);
		Precision.Format(L"%.0f",  (MaxY - Row * stepY));
		pDC->TextOutW(50, rect.top + Row * cell.Uint_Height - 10, Precision); // y 轴标签
	}
	for(int col = 0; col <= 16; col++) {
		pDC->MoveTo(rect.left + col * cell.Uint_Width, rect.top); // 垂直线
		pDC->LineTo(rect.left + col * cell.Uint_Width, rect.bottom);
		float MaxX = static_cast<float>(sld_time.GetPos());
		float stepX = MaxX / 16.0f;
		tableFrameWork.PrecisionX = stepX;
		Precision.Format(L"%.2f", col * stepX);
		pDC->TextOutW(90 + col * cell.Uint_Width - 25, 560, Precision); // x 轴标签
	}
}

void CMathModuleDlg::ClearTableFramework(IN CDC* pDC) {
	if (pDC == nullptr) {
		MessageBox(_T("Device context pointer must not be NULL!"), _T("Error"), MB_ICONERROR);
		return;
	}
	CRect tableFrameWorkRect = TableFrameWorkToRect(&tableFrameWork);
	tableFrameWorkRect.left -= 40;
	tableFrameWorkRect.top -= 10;
	tableFrameWorkRect.right += 30;
	tableFrameWorkRect.bottom += 50;
	pDC->FillSolidRect(tableFrameWorkRect, RGB(240, 240, 240)); // 清空表格框架
}

void CMathModuleDlg::DrawTableFramework(IN CDC* pDC, IN PtrAxisPoint alcohol_concentration_points, IN int pointCount) {
	if(pDC == nullptr) {
		MessageBox(_T("Device context pointer must not be NULL!"), _T("Error"), MB_ICONERROR);
		return;
	}
	if (alcohol_concentration_points == nullptr || pointCount <= 0) {
		MessageBox(_T("Failed to retrieve alcohol concentration data! "), _T("Error"), MB_ICONERROR);
		return;
	}
	CPen RedPen(PS_SOLID, 1, RGB(255, 0, 0)); // 红色画笔
	pDC->SelectObject(&RedPen);
	CPoint StartPoint = GetStartPoint(&tableFrameWork);
	CPoint MaxPoint = StartPoint;
	pDC->MoveTo(StartPoint);
	for(int Index = 0; Index < pointCount; Index++) {
		CPoint Point = GetRealPoint(tableFrameWork, StartPoint, alcohol_concentration_points[Index], cell);
		if (MaxPoint.y < Point.y)
			memcpy(&MaxPoint, &Point, sizeof(CPoint));
		Point = GetGUIPoint(StartPoint, Point);

		pDC->LineTo(Point);
		if (Index < pointCount - 1) pDC->MoveTo(Point);
		if (Point.x > tableFrameWork.Left + tableFrameWork.Width) break;
	}
	CBrush BlueBrush(RGB(0, 255, 0)); // 绿色刷子
	pDC->SelectObject(&BlueBrush);
	int Size = 5;
	MaxPoint = GetGUIPoint(StartPoint, MaxPoint);
	pDC->Ellipse(MaxPoint.x - Size, MaxPoint.y - Size, MaxPoint.x + Size, MaxPoint.y + Size);
}


DWORD WINAPI CMathModuleDlg::CalcOptimalCoefficientsCore(LPVOID pParam) {
	PtrOptimalParameter pData = (PtrOptimalParameter)pParam;

	double MinError = FLT_MAX;
	float best_A = 0, best_a = 0, best_k = 0;
		
	// 假设每个线程负责 A 的 1.0 跨度，内部进行精细搜索
	float A_start = pData->A;
	for (float A = A_start; A < A_start + 1.0f; A += 0.05f) {
		for (float a = 1.5f; a <= 2.5f; a += 0.05f) {
			for (float k = 0.1f; k <= 0.25f; k += 0.01f) {
				double error = static_cast<CMathModuleDlg*>(pData->pClassBase)->FindOptimalCoefficients(A, a, k);

				if (error < MinError) {
					MinError = error;
					best_A = A;
					best_a = a;
					best_k = k;
				}
			}
		}
	}

	double sum = 0.0f;
	for(int i = 0; i < 23; i++) 
		sum += Sample_alcohol_concentration_points[i].y;
	double SST = 0;
	for (int i = 0; i < 23; i++) {
		double diff = Sample_alcohol_concentration_points[i].y - (sum / 23.0f);
		SST += (diff * diff);
	}
	// 将计算出的最优结果写回结构体
	pData->pClassBase = nullptr; // 线程结束前清空指针，防止误用
	pData->parameters.A = best_A;
	pData->parameters.a = best_a;
	pData->parameters.k = best_k;
	pData->parameters.error = sqrt(MinError / 23.0f);		// 计算 均方根误差
	pData->parameters.R_Squared = 1 - (MinError / SST);	// 计算 拟合优度
	return 0;
}

double CMathModuleDlg::FindOptimalCoefficients(IN float A, IN float a, IN float k){
	double totalError = 0.0f;
	for (int i = 0; i < 23; i++) {
		float time_hour = Sample_alcohol_concentration_points[i].x;
		float actual_concentration = Sample_alcohol_concentration_points[i].y;

		float predicted_concentration = GetAlcoholConcentrationFormula(2, A, a, k, time_hour);
		totalError += pow(predicted_concentration - actual_concentration, 2); // 均方误差
	}
	return totalError;
}

void CMathModuleDlg::Display_Alcohol_Concentration_SampleList(IN CDC* pDC) {
	CBrush BlueBrush(RGB(0, 0, 255)); // 蓝色刷子
	pDC->SelectObject(&BlueBrush);
	for (int i = 0; i < 23; i++) {
		CPoint StartPoint = GetStartPoint(&tableFrameWork);
		CPoint Point = GetRealPoint(tableFrameWork, StartPoint, Sample_alcohol_concentration_points[i], cell);
		Point = GetGUIPoint(StartPoint, Point);
		int Size = 5; // 方块半径
		if(tableFrameWork.Top < Point.y && Point.x < (tableFrameWork.Left + tableFrameWork.Width))
			pDC->Ellipse(Point.x - Size, Point.y - Size, Point.x + Size, Point.y + Size);
		
	}
}

void CMathModuleDlg::Display_Alcohol_Concentration(IN int Bottle, IN float A, IN float a, IN float k){
	int PointCount = 0;
	if (pResultData) free(pResultData);
	PtrAxisPoint alcohol_concentration_points = nullptr;
	PointCount = Calculate_Alcohol_Concentration(Bottle, A, a, k, alcohol_concentration_points);
	if (alcohol_concentration_points != nullptr) {
		CClientDC dc(this);
		ClearTableFramework(&dc);
		InitTableFramework(&dc);
		Display_Alcohol_Concentration_SampleList(&dc);
		DrawTableFramework(&dc, alcohol_concentration_points, PointCount);
	}
	pResultData = alcohol_concentration_points;
}
