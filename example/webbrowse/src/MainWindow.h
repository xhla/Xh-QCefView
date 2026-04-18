#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCefView.h>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

/**
 * @brief 主窗口类
 *
 * 实现一个简单的浏览器窗口，包含：
 * - 顶部工具栏：地址输入框 + 加载按钮
 * - 中央区域：QCefView 网页浏览器组件
 */
class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget* parent = nullptr);
  ~MainWindow();

private slots:
  /**
   * @brief 加载 URL 的槽函数
   *
   * 当用户点击按钮时触发，从输入框获取 URL 并加载到 QCefView 中
   */
  void onLoadButtonClicked();

private:
  /**
   * @brief 设置 UI 布局
   *
   * 创建并组织所有控件
   */
  void setupUI();

  /**
   * @brief 设置信号槽连接
   *
   * 连接按钮点击信号到对应的槽函数
   */
  void setupConnections();

private slots:
  

  void onInvokeMethodNotify(const QCefBrowserId& browserId,
                    const QCefFrameId& frameId,
                    const QString& method,
                    const QVariantList& arguments);

  
  /// @brief 
  /// @param browserId 
  /// @param frameId 
  /// @param query 
  void onQCefQueryRequest(const QCefBrowserId& browserId, const QCefFrameId& frameId, const QCefQuery& query);

private:
  QWidget* m_centralWidget;     ///< 中央窗口部件
  QVBoxLayout* m_mainLayout;    ///< 主布局（垂直布局）
  QHBoxLayout* m_toolbarLayout; ///< 工具栏布局（水平布局）

  QLineEdit* m_urlEdit;      ///< 地址输入框
  QPushButton* m_loadButton; ///< 加载按钮
  QCefView* m_cefView = nullptr;
  QShortcut* m_shortcut = nullptr; ///< QCefView 浏览器组件
};

#endif // MAINWINDOW_H