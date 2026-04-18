#include "MainWindow.h"
#include <QCefContext.h>
#include <QCefView.h> // QCefView 核心头文件
#include <QDebug>
#include <QShortcut>
#include <QUrl>
#include <qmessagebox.h>

/**
 * @brief 构造函数
 *
 * 初始化窗口，设置默认 URL，创建 QCefView 实例
 *
 * @param parent 父窗口指针
 */
MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent)
  , m_centralWidget(nullptr)
  , m_mainLayout(nullptr)
  , m_toolbarLayout(nullptr)
  , m_urlEdit(nullptr)
  , m_loadButton(nullptr)
  , m_cefView(nullptr)
{
  // 设置窗口标题和初始大小
  setWindowTitle(tr("QCefView Demo - 混合浏览器"));
  resize(1200, 800);

  // 设置 UI 布局
  setupUI();

  // 连接信号槽
  setupConnections();

  m_urlEdit->setText("https://www.baidu.com");
}

/**
 * @brief 析构函数
 *
 * 注意：QCefView 会在销毁时自动清理相关的 CEF 资源
 */
MainWindow::~MainWindow()
{
  // QCefView 会自行清理，无需手动处理
}

/**
 * @brief 设置 UI 布局
 *
 * 创建控件并组织布局：
 * - 顶部工具栏：水平布局，包含地址输入框和加载按钮
 * - 中央区域：QCefView 组件占据剩余空间
 */
void
MainWindow::setupUI()
{
  // 创建中央窗口部件
  m_centralWidget = new QWidget(this);
  setCentralWidget(m_centralWidget);

  // 创建主布局（垂直布局）
  m_mainLayout = new QVBoxLayout(m_centralWidget);
  m_mainLayout->setContentsMargins(0, 0, 0, 0); // 移除边距
  m_mainLayout->setSpacing(0);                  // 设置间距为 0

  // === 创建工具栏 ===
  m_toolbarLayout = new QHBoxLayout();
  m_toolbarLayout->setContentsMargins(8, 8, 8, 8);

  // 创建地址输入框
  m_urlEdit = new QLineEdit(this);
  m_urlEdit->setPlaceholderText(tr("请输入网址 (例如: https://www.baidu.com)"));
  m_urlEdit->setClearButtonEnabled(true); // 显示清空按钮

  // 创建加载按钮
  m_loadButton = new QPushButton(tr("加载"), this);
  m_loadButton->setDefault(true); // 设置为默认按钮，支持回车触发

  // 将控件添加到工具栏布局
  m_toolbarLayout->addWidget(m_urlEdit, 1); // 1 表示拉伸因子，让输入框占据主要空间
  m_toolbarLayout->addWidget(m_loadButton);

  // 将工具栏布局添加到主布局
  m_mainLayout->addLayout(m_toolbarLayout);

  // Build settings for per QCefView
  QCefSetting setting;
  setting.setOffScreenRenderingEnabled(true);
  setting.setHardwareAccelerationEnabled(false);
  setting.setWindowlessFrameRate(60);
  // 获取网页文件路径
  QString webPath = QCoreApplication::applicationDirPath() + "/webres/index.html";
  QUrl webUrl = QUrl::fromLocalFile(webPath);
  // m_cefView = new QCefView("https://www.baidu.com", &setting, this);
  m_cefView = new QCefView(webUrl.toString(), &setting, this);
  // 连接消息接收信号
  connect(m_cefView, &QCefView::invokeMethod, this, &MainWindow::onInvokeMethodNotify);
  connect(m_cefView, &QCefView::cefQueryRequest, this, &MainWindow::onQCefQueryRequest);
  // 1. 创建一个 QShortcut，绑定 F12 键
  m_shortcut = new QShortcut(QKeySequence(Qt::Key_F12), this);

  // 2. 连接快捷键的 activated 信号
  connect(m_shortcut, &QShortcut::activated, [this]() {
    // 调用 QCefView 的调试接口 (假设你的版本支持)
    m_cefView->showDevTools();
  });

  // 将 QCefView 添加到主布局（占据剩余所有空间）
  m_mainLayout->addWidget(m_cefView, 1);
}

/**
 * @brief 设置信号槽连接
 *
 * 连接按钮的 clicked 信号到 onLoadButtonClicked 槽函数
 * 同时支持输入框按回车键触发加载
 */
void
MainWindow::setupConnections()
{
  // 连接按钮点击信号
  connect(m_loadButton, &QPushButton::clicked, this, &MainWindow::onLoadButtonClicked);

  // 连接输入框的回车信号（按回车键时触发加载）
  connect(m_urlEdit, &QLineEdit::returnPressed, this, &MainWindow::onLoadButtonClicked);
}

void
MainWindow::onQCefQueryRequest(const QCefBrowserId& browserId, const QCefFrameId& frameId, const QCefQuery& query)
{
 qDebug() << "MainWindow::onQCefQueryRequest:" << query.request() << "from browserId:" << browserId << "frameId:" << frameId;
  QMessageBox::information(this, "onQCefQueryRequest", "请求: " + query.request());
 query.setResponseResult(true, "Query received and processed successfully");
}

void
MainWindow::onInvokeMethodNotify(const QCefBrowserId& browserId,
                                 const QCefFrameId& frameId,
                                 const QString& method,
                                 const QVariantList& arguments)
{
   
// 信息提示框
  QMessageBox::information(this, "onInvokeMethodNotify", "方法名: " + method + "\n参数: " + QVariant(arguments).toString());
   qDebug() << "MainWindow::onInvokeMethodNotify:" << method << arguments;
}

/**
 * @brief 加载按钮点击的槽函数实现
 *
 * 从输入框获取 URL，进行基本验证后加载到 QCefView 中
 */
void
MainWindow::onLoadButtonClicked()
{
  // 获取输入的 URL 字符串
  QString urlString = m_urlEdit->text().trimmed();

  // 验证 URL 是否为空
  if (urlString.isEmpty()) {
    qWarning() << "URL 不能为空";
    return;
  }

  // 如果 URL 没有协议前缀，自动添加 https://
  if (!urlString.startsWith("http://") && !urlString.startsWith("https://")) {
    urlString = "https://" + urlString;
    m_urlEdit->setText(urlString); // 更新输入框显示
  }

  QUrl url(urlString);

  // 验证 URL 有效性
  if (!url.isValid()) {
    qWarning() << "无效的 URL:" << urlString;
    return;
  }

  qDebug() << "正在加载 URL:" << url.toString();

  // 调用 QCefView 的导航方法加载网页
  m_cefView->navigateToUrl(urlString);
}