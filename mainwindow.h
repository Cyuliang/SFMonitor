#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QDebug>
#include <QInputDialog>
#include <QDir>
#include <QDateTime>
#include <QTextStream>
#include <QTimer>
#include <QMenu>
#include <QMessageBox>
#include <QCloseEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void closeEvent(QCloseEvent *event)Q_DECL_OVERRIDE;

private slots:

    ///
    /// \brief on_actionAdd_triggered 添加项
    ///
    void on_actionAdd_triggered();

    ///
    /// \brief slot_item_checked 列表选中项
    /// \param row
    /// \param cot
    ///
    void slot_item_checked(int row,int cot);


    void RightClickSlot(QPoint pos);
    void RightClickDelete(QAction *act);
    void on_actionStart_triggered();

    void on_actionStop_triggered();

    void on_actionExit_triggered();

private:
    Ui::MainWindow *ui;

    QMenu *RightClick;                          //右键点击
    QAction *deleteAction;                      //删除事件（如需要其它事件，可新定义）
    int iDeletcRow;

    QTimer* timer;

private:

    ///
    /// \brief getProcess 拉去进程
    ///
    bool getProcess(QString name);
};
#endif // MAINWINDOW_H
