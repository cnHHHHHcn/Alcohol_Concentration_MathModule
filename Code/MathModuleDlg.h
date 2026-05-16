
// MathModuleDlg.h: 头文件
//

#pragma once

// 定义 IN 和 OUT 宏，通常用于标识参数方向，实际编译时为空
#define IN
#define OUT

/*
 * Macro: GetAlcoholConcentrationFormula
 * Description:
 *   计算“短时间内快速饮酒”（瞬时摄入）模型下的血液酒精浓度。
 *   基于双指数模型：酒精先被吸收进入血液，随后被代谢排出。
 *
 * Formula:
 *   C(t) = (Bottle/2) * A * (e^(-k*t) - e^(-a*t))
 *
 * Parameters:
 *   IN int    Bottle    - 饮酒量（单位：瓶，基于参考数据中2瓶啤酒的标准进行缩放）
 *   IN float  A         - 综合系数（包含最大浓度潜力、分布容积等参数的拟合常数）
 *   IN float  a         - 吸收速率常数 (Absorption rate constant)，通常 a > k
 *   IN float  k         - 代谢速率常数 (Elimination rate constant)
 *   IN int    Time      - 饮酒后经过的时间，单位为小时
 *
 * Return Value:
 *   float - 计算得到的血液酒精浓度 (mg/100ml)
 */
#define GetAlcoholConcentrationFormula(Bottle, A, a, k, Time) ( \
	static_cast<float>((Bottle / 2.0f) *(A) * (exp(-(k) * (Time)) - exp(-(a) * (Time)))) \
)

 /*
  * Macro: GetAlcoholConcentrationFormulaWithUniformRate
  * Description:
  *   计算“较长时间内匀速饮酒”模型下的血液酒精浓度。
  *   该模型假设酒精在 DrinkTime 时间段内以恒定速率进入体内，同时伴随代谢过程。
  *   分为两个阶段：
  *   1. 饮酒阶段 (0 <= t <= DrinkTime)：酒精匀速进入，同时按一级动力学代谢。
  *   2. 代谢阶段 (t > DrinkTime)：酒精摄入停止，体内残留酒精按一级动力学指数衰减。
  *
  * Formula:
  *   If t <= DrinkTime:
  *     C(t) = ScaleFactor * A * (1 - e^(-k*t)) / (k * DrinkTime)
  *   Else:
  *     C(t) = C(DrinkTime) * e^(-k * (t - DrinkTime))
  *
  * Parameters:
  *   IN int    Bottle    - 饮酒量（单位：瓶）
  *   IN float  A         - 综合系数（与上一宏定义一致，基于2瓶酒拟合）
  *   IN float  a         - 吸收速率常数（注：在此匀速模型中，吸收过程被平滑化，主要受代谢率 k 影响，但保留参数以匹配接口）
  *   IN float  k         - 代谢速率常数
  *   IN int    Time      - 从开始饮酒起经过的总时间，单位为小时
  *   IN int    DrinkTime - 喝完这些酒所花费的总时长，单位为小时（例如：2小时）
  *
  * Return Value:
  *   float - 计算得到的血液酒精浓度 (mg/100ml)
  */
#define GetAlcoholConcentrationFormulaWithUniformRate(Bottle, A, a, k, Time, DrinkTime) ( \
    (Time) <= (DrinkTime) ? \
        /* 阶段1：正在喝酒。酒精持续匀速摄入，浓度随 (1 - e^-kt) 规律上升 */ \
        static_cast<float>((Bottle / 2.0f) * (A) * (1.0f - exp(-(k) * (Time))) / ((k) * (DrinkTime))) \
        : \
        /* 阶段2：喝酒结束。以喝完时刻的浓度为初始值，进行指数衰减 */ \
        static_cast<float>((Bottle / 2.0f) * (A) * (1.0f - exp(-(k) * (DrinkTime))) / ((k) * (DrinkTime)) * exp(-(k) * ((Time) - (DrinkTime)))) \
)

/*
 * Macro: TableFrameWorkToRect
 * Description:
 *   将自定义的表格框架结构体转换为 MFC 的 CRect 矩形对象。
 *   主要用于绘图 API 调用或矩形区域碰撞检测。
 *
 * Formula:
 *   Rect(Left, Top, Left + Width, Top + Height)
 *
 * Parameters:
 *   IN PtrTableFrameWork ptrTableFrameWork - 指向表格框架结构体的指针
 *
 * Return Value:
 *   CRect对象，表示表格框架的矩形区域 (Left, Top, Right, Bottom)
 */
#define TableFrameWorkToRect(ptrTableFrameWork) CRect(\
	(ptrTableFrameWork)->Left, \
	(ptrTableFrameWork)->Top, \
	(ptrTableFrameWork)->Left + (ptrTableFrameWork)->Width, \
	(ptrTableFrameWork)->Top + (ptrTableFrameWork)->Height\
)

/*
 * Macro: GetStartPoint
 * Description:
 *   获取表格框架的起始点坐标（通常定义为矩形区域的左下角）。
 *   该点常作为坐标系转换或相对定位的基准原点。
 *
 * Formula:
 *   Point(Left, Top + Height)
 *
 * Parameters:
 *   IN PtrTableFrameWork ptrTableFrameWork - 指向表格框架结构体的指针
 *
 * Return Value:
 *   CPoint对象，表示表格框架的起始点坐标 (x, y)
 */
#define GetStartPoint(ptrTableFrameWork) CPoint(\
	(ptrTableFrameWork)->Left, \
	(ptrTableFrameWork)->Top + (ptrTableFrameWork)->Height\
)

 /*
 * Macro: GetRealPoint
 * Description:
 *   根据起始点、逻辑轴点、单元格尺寸和绘图精度，计算实际绘制时的物理坐标。
 *   实现了从逻辑坐标（如表格行列）到物理坐标（屏幕像素）的线性映射。
 *
 * Formula:
 *   X = StartX + (AxisX * UnitWidth) / PrecisionX
 *   Y = StartY + (AxisY * UnitHeight) / PrecisionY
 *
 * Parameters:
 *   IN TableFrameWork tableFrameWork - 表格框架结构体（包含 PrecisionX/Y 精度参数）
 *   IN CPoint         StartPoint     - 起始点坐标（基准原点）
 *   IN AxisPoint      axisPoint      - 轴点坐标（逻辑坐标值）
 *   IN Cell           cell           - 单元格尺寸（包含 Uint_Width/Height 单位宽高）
 *
 * Return Value:
 *   CPoint对象，表示经过比例缩放后的实际绘制点坐标
 */
#define GetRealPoint(tableFrameWork, StartPoint, axisPoint, cell) CPoint(\
	(StartPoint).x + int(((axisPoint).x * (cell).Uint_Width) / (tableFrameWork).PrecisionX),\
	(StartPoint).y + int(((axisPoint).y * (cell).Uint_Height) / (tableFrameWork).PrecisionY)\
)

/*
 * Macro: GetGUIPoint
 * Description:
 *   将实际绘制点坐标转换为 GUI 屏幕坐标系下的坐标。
 *   核心逻辑是进行 Y 轴的镜像翻转（因为数学坐标系 Y 轴向上，而屏幕坐标系 Y 轴向下）。
 *
 * Formula:
 *   GUI_X = Real_X
 *   GUI_Y = Start_Y * 2 - Real_Y  (以 StartPoint.y 为对称轴进行翻转)
 *
 * Parameters:
 *   IN CPoint StartPoint - 起始点坐标（作为 Y 轴镜像翻转的对称基准点）
 *   IN CPoint RealPoint  - 实际绘制点坐标（物理坐标）
 *
 * Return Value:
 *   CPoint对象，表示转换后的 GUI 屏幕坐标
 */
#define GetGUIPoint(StartPoint, RealPoint) CPoint(\
	((RealPoint).x), \
	((StartPoint).y * 2 - (RealPoint).y)\
)
// CMathModuleDlg 对话框
class CMathModuleDlg : public CDialogEx
{
// 构造
public:
	CMathModuleDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MATHMODULE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();

	// Windows 消息处理函数
	afx_msg void OnDestroy();														// 对话框销毁事件处理函数
	afx_msg void OnPaint();															// 绘制事件处理函数
	afx_msg HCURSOR OnQueryDragIcon();												// 鼠标悬停图标事件处理函数
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);		// 滑动条事件处理函数
	afx_msg void OnLbnSelchangeListParameters();									// 参数列表框事件处理函数
	afx_msg void OnBnClickedButtonSaveParameters();									// 保存参数按钮事件处理函数
	afx_msg void OnBnClickedButtonGetInformaion();									// 获取按钮事件处理函数
	afx_msg void OnBnClickedButtonAutocalcparameters();								// 自动计算参数按钮事件处理函数
	DECLARE_MESSAGE_MAP()

public:
	// 定义结构体
	// 表格框架结构体
	typedef struct _TableFrameWork {
		int Left;
		int Top;
		int Width;
		int Height;
		float PrecisionX;
		int PrecisionY;
	} TableFrameWork, * PtrTableFrameWork;

	// 坐标结构体
	typedef struct _AxisPoint {
		float x;
		float y;
	}AxisPoint, *PtrAxisPoint;

	// 表格单元格结构体
	typedef struct _Cell {
		int Uint_Width;
		int Uint_Height;
	} Cell, *PtrCell;

	typedef struct _AlgorithmParameters {
		float A;
		float a;
		float k;
		double error;
		double R_Squared;
	} AlgorithmParameters, * PtrAlgorithmParameters;

	BOOL HScroll_IsPaint = true;
	int Bottle_one_Max_Concentration = 50;
	void* pResultData = nullptr;

	// 图形成员变量
	CSliderCtrl sld_Bottle;
	CSliderCtrl sld_A;
	CSliderCtrl sld_a;
	CSliderCtrl sld_k;
	CSliderCtrl sld_time;
	CSliderCtrl sld_drinktime;

	CStatic lbl_Bottle;
	CStatic lbl_A;
	CStatic lbl_a;
	CStatic lbl_k;
	CStatic lbl_time;
	CStatic lbl_drinktime;

	CStatic lbl_ylabel;
	CListBox lst_Parameters;
	CListBox lst_SampleData;
	CStatic lbl_OptimalCoefficients;

	// 线图成员变量
	TableFrameWork tableFrameWork = {
		80,		// Left
		50,		// Top
		800,	// Width
		500,	// Height
		0,		// PrecisionX
		0		// PrecisionY
	};	// 表格框架

	Cell cell = {
		50,	// Uint_Width
		50	// Uint_Height
	};	// 表格单元格

	// 从参数列表框获取参数值
	void Get_Parameters_From_SbliderBox(OUT int& Bottle, OUT float& A, OUT float& a, OUT float& k, OUT int& hour, OUT int& drinktime);	

	// 计算酒精浓度的函数
	int CMathModuleDlg::Calculate_Alcohol_Concentration(
		IN int Bottle, 
		IN float alcohol_dosage_coefficient,
		IN float absorption_rate_constant,
		IN float elimination_rate_constant,
		OUT PtrAxisPoint& ptr_alcohol_concentration
	);

	// 手动绘制表格函数
	void InitTableFramework(IN CDC* pDC);		// 初始化表格框架
	void ClearTableFramework(IN CDC* pDC);		// 清除表格框架
	void DrawTableFramework(
		IN CDC* pDC, 
		IN PtrAxisPoint alcohol_concentration_points,
		IN int pointCount
	);											// 绘制表格框架(酒精浓度拟合曲线)

	/* 计算拟合曲线函数
	* 问题: 使用穷举法过于耗时，是否有更高效的算法来计算最优参数？
	* 
	* 目前解决办法:
	* 创建多线程来并行计算不同参数组合的误差，利用多核CPU的计算能力来加速寻找最优参数的过程。
	* 最后将所有线程的结果汇总，找到误差最小的参数组合作为最优参数。
	*/ 

	typedef struct _OptimalParameter {
		IN	float A = 0.0f;
		IN  void* pClassBase = nullptr;	// 指向CMathModuleDlg类的指针，用于在线程函数中调用成员函数
		OUT	AlgorithmParameters parameters = { 0 };
	} OptimalParameter, * PtrOptimalParameter;

	// 计算最优参数线程核心函数
	static DWORD WINAPI CMathModuleDlg::CalcOptimalCoefficientsCore(LPVOID Paramters);

	// 计算最优参数函数
	double FindOptimalCoefficients(IN float A, IN float a, IN float k);

	// 显示酒精浓度结果样本列表函数
	void Display_Alcohol_Concentration_SampleList(IN CDC* pDC);

	// 显示酒精浓度拟合曲线，样本数据函数
	void Display_Alcohol_Concentration(IN int Bottle, IN float A, IN float a, IN float k);
};
