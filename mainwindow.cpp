#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->textEdit->document()->setMaximumBlockCount(100);

    ui->actionStart->setEnabled(true);
    ui->actionStop->setEnabled(false);

    QFile INI("Sys.ini");
    if(INI.open(QIODevice::ReadOnly)){
        while (!INI.atEnd())
        {
            QByteArray line = INI.readLine();
            QStringList cmd=QString::fromLocal8Bit(line).split(",");
            QString name=cmd[0];
            int ind=name.lastIndexOf("\\");

            int row=ui->tableWidget->rowCount();
            ui->tableWidget->insertRow(row);
            QTableWidgetItem* item= new QTableWidgetItem(name.mid(ind+1,name.length()));
            item->setCheckState(cmd.at(1).toInt()?Qt::Checked:Qt::Unchecked);
            ui->tableWidget->setItem(row,0,item);
            ui->tableWidget->setItem(row,1,new QTableWidgetItem(name.mid(0,ind+1)));
        }
    }
    INI.close();

    connect(ui->tableWidget,&QTableWidget::cellChanged,this,&MainWindow::slot_item_checked);

    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    RightClick = new QMenu(ui->tableWidget);                        //右键点击菜单
    deleteAction = new QAction(QString::fromLocal8Bit("Del"),this);     //删除事件
    RightClick->addAction(deleteAction);                                //将action添加到菜单内

    //connect(ui->tableWidget,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(RightClickSlot(QPoint)));
    //connect(RightClick,SIGNAL(triggered(QAction*)),this,SLOT(RightClickDelete(QAction*)));

    timer=new QTimer(this);
    timer->setSingleShot(false);
    connect(timer,&QTimer::timeout,this,&MainWindow::on_actionStart_triggered);
    timer->start(5000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QString pas = QInputDialog::getText(this,"Password","Enter the exit password");
    if(pas=="168"){
        event->accept();
    }
    else {
        event->ignore();
    }
}

bool MainWindow::getProcess(QString exe)
{
    QProcess process;
    process.start("tasklist",QStringList()<<"/FI"<<"imagename eq "+exe);
    if(process.waitForFinished()){
        QString state = QString::fromLocal8Bit(process.readAllStandardOutput());
        if(state.contains(exe)){
            return true;
        }
        else {
            return false;
        }
    }
    return false;
}


void MainWindow::on_actionAdd_triggered()
{
    QString name = QInputDialog::getText(this,"Monitor the process","Please enter a process name");
    int ind=name.lastIndexOf("\\");

    int row=ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(row);
    QTableWidgetItem* item= new QTableWidgetItem(name.mid(ind+1,name.length()));
    item->setCheckState(Qt::Unchecked);
    ui->tableWidget->setItem(row,0,item);
    ui->tableWidget->setItem(row,1,new QTableWidgetItem(name.mid(0,ind+1)));

    QFile INI("Sys.ini");
    if(INI.open(QIODevice::ReadWrite | QIODevice::Append)){
        QTextStream txt(&INI);
        txt<<QString("%1,%2").arg(name,QString::number(1));
        txt<<QString("\r\n");
    }
    INI.close();
}

void MainWindow::slot_item_checked(int row, int cot)
{
    if(ui->tableWidget->item(row,cot)->checkState()==Qt::Checked){
        if(!getProcess(ui->tableWidget->item(row,0)->text())){
            QProcess Process;
            QString program=ui->tableWidget->item(row,0)->text();
            QString path=QDir::toNativeSeparators(ui->tableWidget->item(row,1)->text());
            Process.setWorkingDirectory(path);
            Process.startDetached(path.append(program));
            ui->textEdit->append(QString("[%1][%2]  Start the startup program").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"),ui->tableWidget->item(row,0)->text()));
        }
        else {
            ui->textEdit->append(QString("[%1][%2]  The program is running").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"),ui->tableWidget->item(row,0)->text()));
        }
    }
}

void MainWindow::RightClickSlot(QPoint pos)
{
    QModelIndex index = ui->tableWidget->indexAt(pos);    //找到tableview当前位置信息
    iDeletcRow = index.row();    //获取到了当前右键所选的行数

    if(index.isValid())        //如果行数有效，则显示菜单
    {
        RightClick->exec(QCursor::pos());
    }
}

void MainWindow::RightClickDelete(QAction *act)
{
    //弹出提示框，看是否删除数据
    QMessageBox message(QMessageBox::NoIcon, QString("提示"),
                                QString("是否删除本行数据?"),
    QMessageBox::Yes | QMessageBox::No, NULL);

    //如确认删除
    if(message.exec() == QMessageBox::Yes)
    {
        if(act->text() == QString("删除"))   //看选中了删除这个菜单
        {
            ui->tableWidget->removeRow(iDeletcRow);  //删除掉了表格信息
        }
    }
}

void MainWindow::on_actionStart_triggered()
{
    ui->actionStart->setEnabled(false);
    ui->actionStop->setEnabled(true);

    for(int row=0;row<ui->tableWidget->rowCount();row++){
        if(ui->tableWidget->item(row,0)->checkState()!=Qt::Checked){
            continue;
        }

        if(!getProcess(ui->tableWidget->item(row,0)->text())){
            QProcess Process;
            QString program=ui->tableWidget->item(row,0)->text();
            QString path=QDir::toNativeSeparators(ui->tableWidget->item(row,1)->text());
            Process.setWorkingDirectory(path);
            Process.startDetached(path.append(program));
            if(Process.waitForFinished(3000)){
                ui->textEdit->append(QString("[%1][%2]  Program startup failure").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"),ui->tableWidget->item(row,0)->text()));
            }
            else {
                ui->textEdit->append(QString("[%1][%2]  Start the startup program").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"),ui->tableWidget->item(row,0)->text()));
            }
        }
//        else {
//            ui->textEdit->append(QString("[%1][%2]  The program is running").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"),ui->tableWidget->item(row,0)->text()));
//        }
    }
}

void MainWindow::on_actionStop_triggered()
{
    ui->actionStart->setEnabled(true);
    ui->actionStop->setEnabled(false);

    if(timer && timer->isActive()){
        timer->stop();
    }
}

void MainWindow::on_actionExit_triggered()
{
    QString pas = QInputDialog::getText(this,"Password","Enter the exit password");
    if(pas=="168"){
        this->close();
    }
}
