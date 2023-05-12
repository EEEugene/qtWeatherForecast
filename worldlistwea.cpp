#include "worldlistwea.h"
#include "ui_worldlistwea.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMenu>
#include <QMessageBox>
#include <QNetworkReply>
#include <QPainter>
#include <QTimer>
#include <QPainter>
#include <QPen>
#include <QMouseEvent>
#include <QThread>

extern QString cityName;

WorldListWea::WorldListWea(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WorldListWea)
{
    ui->setupUi(this);
    setWindowFlag(Qt::FramelessWindowHint);  // 设置无边框
    setFixedSize(width(), height());         // 设置固定窗口大小

    //设置图标
    setWindowIcon(QIcon(":/res/WeaFor.png"));

    //构建右键菜单
    mExitMenu = new QMenu(this);
    mExitAct = new QAction();
    mExitAct->setText(tr("退出"));
    mExitAct->setIcon(QIcon(":/res/close.ico"));
    mExitMenu->addAction(mExitAct);
    //用lamba函数连接退出按钮，触发就退出该应用
    connect(mExitAct, &QAction::triggered, this, [=]() { qApp->exit(0); });

    connect(ui->backBtn,&QPushButton::clicked,this,[=](){
        on_listBtn_clicked();
        emit this->BackButton();
    });

    setState();
}

WorldListWea::~WorldListWea()
{
    delete ui;
}

void WorldListWea::contextMenuEvent(QContextMenuEvent *event)
{
    //弹出右键菜单
    mExitMenu->exec(QCursor::pos());
    event->accept();
}

void WorldListWea::mousePressEvent(QMouseEvent *event)
{
    mOffset = event->globalPos() - this->pos();
}

//重写鼠标移动事件
void WorldListWea::mouseMoveEvent(QMouseEvent *event)
{
    this->move(event->globalPos() - mOffset);
}

//默认情况下先选择五大洲
void WorldListWea::setState()
{
    ui->listWidget->hide();
    QStringList stateList;
    stateList << "亚洲" << "欧洲" << "非洲" << "美洲" << "大洋洲";
    ui->stateWidget->addItems(stateList);
}

void WorldListWea::asiaCity()
{
    ui->listWidget->show();
    ui->listWidget->clear();
    AddItem("东京","13°~19°","://res/type/DuoYun.png");
    AddItem("香港","21°~26°","://res/type/Yin.png");
    AddItem("广州","25°~28°","://res/type/DuoYun.png");
    AddItem("新加坡","26°~31","://res/type/DaYu.png");
    AddItem("吉隆坡","18°~21°","://res/type/XiaoYu.png");
    AddItem("大阪","13°~17°","://res/type/DuoYun.png");
    AddItem("首尔","11°~22°","://res/type/DuoYun.png");
    AddItem("上海","16°~27°","://res/type/Qing.png");
    AddItem("北京","12°~22°","://res/type/DuoYun.png");
    AddItem("深圳","21°~28°","://res/type/DuoYun.png");
}

void WorldListWea::euCity()
{
    ui->listWidget->clear();
    ui->listWidget->show();
    AddItem("伦敦","4°~13°","://res/type/Yin.png");
    AddItem("巴黎","7°~13°","://res/type/Yin.png");
    AddItem("莫斯科","8°~20°","://res/type/Yu.png");
    AddItem("罗马","9°~20°","://res/type/Qing.png");
    AddItem("马德里","10°~28°","://res/type/Qing.png");
    AddItem("柏林","6°~10°","://res/type/Yin.png");
    AddItem("威尼斯","10°~15°","://res/type/Yin.png");
    AddItem("汉堡","4°~8°","://res/type/Yu.png");
    AddItem("维也纳","9°~14°","://res/type/Yin.png");
    AddItem("米兰","16°~21°","://res/type/Yu.png");
}

void WorldListWea::afCity()
{
    ui->listWidget->clear();
    ui->listWidget->show();
    AddItem("开罗","17°~28°","://res/type/Qing.png");
    AddItem("坎帕拉","11°~25°","://res/type/Qing.png");
    AddItem("开普敦","12°~22°","://res/type/Qing.png");
    AddItem("内罗毕","17°~24°","://res/type/Yin.png");
    AddItem("阿尔及尔","19°~28°","://res/type/Qing.png");
    AddItem("拉各斯","28°~30°","://res/type/Yin.png");
    AddItem("金沙萨","20°~29°","://res/type/Yu.png");
    AddItem("阿比让","20°~25°","://res/type/Qing.png");
    AddItem("达喀尔","21°~25°","://res/type/Qing.png");
    AddItem("突尼斯","24°~30°","://res/type/Yu.png");
}

void WorldListWea::naCity()
{
    ui->listWidget->clear();
    ui->listWidget->show();
    AddItem("墨西哥城","13°~26°","://res/type/Qing.png");
    AddItem("纽约","6°~11°","://res/type/Yin.png");
    AddItem("洛杉矶","7°~18°","://res/type/Qing.png");
    AddItem("多伦多","3°~10°","://res/type/Yin.png");
    AddItem("芝加哥","4°~9°","://res/type/Yin.png");
    AddItem("休斯顿","15°~23°","://res/type/Qing.png");
    AddItem("费城","8°~17°","://res/type/DuoYun.png");
    AddItem("里约热内卢","16°~24°","://res/type/Qing.png");
    AddItem("圣地亚哥","10°~23°","://res/type/Qing.png");
    AddItem("圣保罗","4°~13°","://res/type/Qing.png");
}

void WorldListWea::oaCity()
{
    ui->listWidget->clear();
    ui->listWidget->show();
    AddItem("悉尼","14°~23°","://res/type/Qing.png");
    AddItem("墨尔本","13°~25°","://res/type/Yin.png");
    AddItem("布里斯班","14°~23°","://res/type/Qing.png");
    AddItem("珀斯","7°~20°","://res/type/Qing.png");
    AddItem("克赖斯特彻奇","18°~23°","://res/type/Yu.png");
    AddItem("奥克兰","13°~17°","://res/type/Yin.png");
    AddItem("惠灵顿","9°~18°","://res/type/Qing.png");
    AddItem("皇后镇","1°~9°","://res/type/Yu.png");
    AddItem("堪培拉","7°~21°","://res/type/Yin.png");
    AddItem("哈密尔顿","9°~17°","://res/type/Yin.png");
}

//创建自定义的QWidget并和QListWidget的Item关联起来
void WorldListWea::AddItem(QString city, QString temp,QString pic)
{
    if(city == updateCity){
        //将已获取数据的放入list中
        cityList.append(updateCity);
        tempList.append(updateTemp);
        typeList.append(updateType);
        //更新数据
        city = updateCity;
        temp = updateTemp;
        pic = updateType;
        cityWithTemp* pItemWidget = new cityWithTemp(this);  //新建widget对象
        pItemWidget->SetData(city, temp, pic);   //设置要写入的城市和天气

        QListWidgetItem* pItem = new QListWidgetItem();  //新建listwidget的一个item
        pItem->setData(Qt::UserRole,city);    //保存城市数据

        pItem->setSizeHint(QSize(30, 55));  //设置一项的大小
        ui->listWidget->addItem(pItem);  //加入listwidget
        ui->listWidget->setItemWidget(pItem, pItemWidget);   //将新建的widget加入到item中
    }
    else{
        //遍历数据，若已存在，直接更新数据
        for (auto iCity = cityList.begin(),iTemp = tempList.begin(),iPic = typeList.begin();
                  iCity != cityList.end(),iTemp != tempList.end(),iPic != typeList.end();
                  ++iCity,++iTemp,++iPic){
            if(city == *iCity){
                city = *iCity;
                temp = *iTemp;
                pic = *iPic;
//                qDebug() << *iCity << *iTemp << *iPic;
            }
        }
        cityWithTemp* pItemWidget = new cityWithTemp(this);
        pItemWidget->SetData(city, temp, pic);
        QListWidgetItem* pItem = new QListWidgetItem();
        pItem->setData(Qt::UserRole,city);
        pItem->setSizeHint(QSize(30, 55));
        ui->listWidget->addItem(pItem);
        ui->listWidget->setItemWidget(pItem, pItemWidget);
    }
}

void WorldListWea::on_stateWidget_itemDoubleClicked()
{
    QString state = ui->stateWidget->currentItem()->text();
    ui->stateWidget->hide();
    if(state == "欧洲"){
        euCity();
    }else if(state == "非洲"){
        afCity();
    }else if(state == "美洲"){
        naCity();
    }else if(state == "大洋洲"){
        oaCity();
    }else{
        asiaCity();
    }
}

//回到默认选择大洲状态
void WorldListWea::on_listBtn_clicked()
{
    ui->listWidget->hide();
    ui->stateWidget->show();
}

void WorldListWea::getTemp(QString city,QString temp,QString type)
{
    updateCity = city;
    updateTemp = temp;
    updateType = type;
}

//当列表中项目被双击时
void WorldListWea::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    QString cityData = item->data(Qt::UserRole).toString();
    emit setData(cityData);
//    qDebug() << cityData;
//    qDebug() << cityName;
    on_listBtn_clicked();
}

void WorldListWea::on_pushButton_2_clicked()
{
    emit setData(ui->pushButton_2->text());
}
