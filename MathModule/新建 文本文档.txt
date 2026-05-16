###  酒精代谢模拟与拟合分析工具 - Readme

**开发环境**：Visual Studio (MFC App)
**核心功能**：基于药代动力学模型，模拟人体血液酒精浓度（BAC）随时间的变化，并支持基于样本数据的参数自动拟合。

---

#### 1. 核心算法模型
本工具提供了两种饮酒模式下的酒精浓度计算模型，基于**双指数模型**（吸收+代谢）构建。

- **短时间内快速饮酒（瞬时摄入）**
  - 适用于一次性快速饮酒后的浓度预测。
  - **宏定义**：`GetAlcoholConcentrationFormula`
  - **计算公式**：$C(t) = (Bottle/2) \times A \times (e^{-k \cdot t} - e^{-a \cdot t})$

- **较长时间内匀速饮酒**
  - 适用于在一段时间内持续饮酒的场景，分为饮酒阶段和代谢阶段。
  - **宏定义**：`GetAlcoholConcentrationFormulaWithUniformRate`
  - **分段函数逻辑**：
    1. **饮酒中** ($t \le T_{drink}$)：浓度按 $(1 - e^{-kt})$ 规律上升。
    2. **饮酒后** ($t > T_{drink}$)：以结束时刻的浓度为初始值，按 $e^{-k(t-T_{drink})}$ 规律衰减。

#### 2. 系统架构与绘图逻辑
程序利用 GDI 绘图技术将数学模型可视化，坐标系处理逻辑如下：

- **坐标转换流程**
  1. **逻辑坐标**：算法计算出的时间-浓度点。
  2. **实际绘制点**：通过 `GetRealPoint` 将逻辑值映射到像素坐标。
  3. **GUI 屏幕坐标**：通过 `GetGUIPoint` 进行 Y 轴翻转（数学坐标系原点在左下角，屏幕坐标系原点在左上角）。

- **绘图组件**
  - `TableFrameWorkToRect`：定义绘图区域的矩形范围。
  - `DrawTableFramework`：负责绘制拟合曲线。
  - `Display_Alcohol_Concentration_SampleList`：在图中标记原始样本数据点（蓝色椭圆）。

#### 3. 用户界面 (UI) 功能
程序界面包含多个交互式控件，用于参数调节与结果显示。

| 控件类型 | 功能描述 | 对应变量 |
| :--- | :--- | :--- |
| **滑动条 (Slider)** | 调节饮酒量、A系数、a常数、k常数、时间范围及饮酒时长。 | `sld_Bottle`, `sld_A`, `sld_a`, `sld_k` 等 |
| **列表框 (ListBox)** | 显示历史保存的参数组合；显示原始样本数据。 | `lst_Parameters`, `lst_SampleData` |
| **静态文本 (Static)** | 显示当前参数值、坐标轴标签、最优拟合系数结果。 | `lbl_A`, `lbl_OptimalCoefficients` 等 |
| **按钮 (Button)** | 保存参数、获取特定阈值信息、自动计算最优参数。 | `IDC_BUTTON_SAVEPARAMETERS`, `IDC_BUTTON_AUTOCALCPARAMETERS` |

#### 4. 智能优化模块 (自动拟合)
程序内置了强大的**参数自动寻优**功能，用于寻找最符合给定样本数据的 $A, a, k$ 参数组合。

- **实现原理**
  - **多线程并行计算**：点击“自动计算参数”按钮后，程序会创建 **16 个线程** (`CalcOptimalCoefficientsCore`)。
  - **穷举搜索策略**：每个线程负责 `A` 系数的一个特定区间（如 105-121），在区间内对 $A, a, k$ 进行步进式穷举。
  - **评价指标**：
    - **RMSE (均方根误差)**：衡量预测值与样本值的偏差。
    - **R-Squared ($R^2$)**：拟合优度，越接近 1 说明模型拟合越好。

- **样本数据**
  - 程序内置了 **23 组**标准样本数据点（时间 vs 浓度），存储在 `Sample_alcohol_concentration_points` 数组中，作为拟合的基准。

#### 5. 数据持久化
- **文件读写**：程序启动时 (`OnInitDialog`) 会自动读取目录下的 `Alcohol_Concentration_Data.txt` 文件，加载历史参数到列表框。
- **保存机制**：关闭程序 (`OnDestroy`) 或点击“保存参数”按钮时，会将当前列表框中的所有参数组合写回该文本文件，实现数据持久化。

#### 6. 编译与运行
1. 使用 Visual Studio 打开工程。
2. 确保包含 `MathModule.h` 和 `MathModuleDlg.h` 等头文件。
3. 编译链接后直接运行。
4. **注意**：运行目录下若存在 `Alcohol_Concentration_Data.txt`，数据将被自动加载；若不存在，程序将正常启动但列表框为空。