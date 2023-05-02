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
    //网络请求
    mNetAccessManager = new QNetworkAccessManager(this);
    connect(mNetAccessManager,&QNetworkAccessManager::finished,this,&WorldListWea::onReplied);
    getWeatherInfo("101320101");

//    connect(dlg,SIGNAL(setData(QString)),this,SLOT(GetData(QString)));
//    connect(ListBtn,SIGNAL(setData(QString)),this,SLOT(GetData(QString)));
    ui->listWidget->setResizeMode(QListView::Adjust);
    updateUI();
//    on_EuBTn_clicked();
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

void WorldListWea::getWeatherInfo(QString cityName)
{
    QUrl url("http://t.weather.itboy.net/api/weather/city/" + cityName);
    mNetAccessManager->get(QNetworkRequest(url));
}

void WorldListWea::getWorldWeather(QString wcity)
{
    QUrl url("https://tianqiapi.com/api?version=v5&appid=95937852&appsecret=TP5lZbcz&city=" + wcity);
    mNetAccessManager->get(QNetworkRequest(url));
}

void WorldListWea::parseJson(QByteArray &byteArray)
{
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(byteArray,&err);
    if(err.error != QJsonParseError::NoError){
        return;
    }

    QJsonObject rootObj = doc.object();
//    qDebug() << rootObj.value("errmsg").toString();

    //解析日期
    mToday.city = rootObj.value("cityInfo").toObject().value("city").toString();
    //解析今天的最高最低气温
    QJsonObject objData = rootObj.value("data").toObject();
    QJsonArray forecatArr =  objData.value("forecast").toArray();
    QJsonObject objForecast = forecatArr[0].toObject();
    QString s;
    s = objForecast.value("high").toString().split(" ").at(1);
    s = s.left(s.length() - 1);
    mDay[0].high = s.toInt();

    s = objForecast.value("low").toString().split(" ").at(1);
    s = s.left(s.length() - 1);
    mDay[0].low = s.toInt();

    mToday.high = mDay[0].high;
    mToday.low = mDay[0].low;

    //更新UI
//    updateUI();
}

//默认情况为亚洲
void WorldListWea::updateUI()
{
    AddItem("东京","13°~19°","://res/type/DuoYun.png");
    AddItem("香港","21°~27°","://res/type/Yin.png");
    AddItem("广州","25°~28°","://res/type/DuoYun.png");
    AddItem("新加坡","26°~31","://res/type/DaYu.png");
    AddItem("吉隆坡","18°~21°","://res/type/XiaoYu.png");
    AddItem("大阪","13°~17°","://res/type/DuoYun.png");
    AddItem("首尔","11°~22°","://res/type/DuoYun.png");
    AddItem("上海","16°~27°","://res/type/Qing.png");
    AddItem("北京","12°~22°","://res/type/Mai.png");
    AddItem("深圳","21°~28°","://res/type/DuoYun.png");
}

//默认情况为全透明，当点击了亚洲开始之后才开始有边框和文字
void WorldListWea::appearButton()
{
    ui->pushButton_2->setStyleSheet("QPushButton{background:rgb(85,85,85);}");
}

void WorldListWea::on_hide_clicked()
{
    ui->pushButton_2->setStyleSheet("QPushButton{border:none;background:transparent;}");
    ui->pushButton_2->setText("");
}

void WorldListWea::onReplied(QNetworkReply *reply)
{
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if(reply->error() != QNetworkReply::NoError || statusCode != 200){
        qDebug() << reply->errorString().toLatin1().data();
        QMessageBox::warning(this,"天气","请求数据失败",QMessageBox::Ok);
    }
    else{
        QByteArray  byteArray = reply->readAll();
//        qDebug() << "World读所有：" << byteArray.data();
        parseJson(byteArray);
    }
    reply->deleteLater();
}

//创建自定义的QWidget并和QListWidget的Item关联起来
void WorldListWea::AddItem(QString str1, QString str2,const QString& pic)
{
    cityWithTemp* pItemWidget = new cityWithTemp(this);  //新建widget对象
    pItemWidget->SetData(str1, str2,pic);   //设置要写入的城市和天气
    QListWidgetItem* pItem = new QListWidgetItem();  //新建listwidget的一个item
    pItem->setSizeHint(QSize(30, 50));  //设置一项的大小
    ui->listWidget->addItem(pItem);  //加入listwidget
    ui->listWidget->setItemWidget(pItem, pItemWidget);      //将新建的widget加入到item中
    connect(pItemWidget,&cityWithTemp::sendlbl,this,&WorldListWea::getCity);    //连接发送数据
}

void WorldListWea::on_Back_clicked()
{
    emit this->BackButton();
}

void WorldListWea::on_AsiaBtn_clicked()
{
    ui->listWidget->clear();
    QStringList city;
    city << "东京" << "香港" << "广州" << "新加坡" << "吉隆坡"
         << "大阪" << "首尔" <<  "上海"<< "北京" << "深圳";
//    ui->listWidget->addItems(city);
    AddItem("东京","13°~19°","://res/type/DuoYun.png");
    AddItem("香港","21°~27°","://res/type/Yin.png");
    AddItem("广州","25°~28°","://res/type/DuoYun.png");
    AddItem("新加坡","26°~31","://res/type/DaYu.png");
    AddItem("吉隆坡","18°~21°","://res/type/XiaoYu.png");
    AddItem("大阪","13°~17°","://res/type/DuoYun.png");
    AddItem("首尔","11°~22°","://res/type/DuoYun.png");
    AddItem("上海","16°~27°","://res/type/Qing.png");
    AddItem("北京","12°~22°","://res/type/Mai.png");
    AddItem("深圳","21°~28°","://res/type/DuoYun.png");
}

void WorldListWea::on_EuBTn_clicked()
{
    ui->listWidget->clear();
    QStringList city;
    city << "伦敦" << "巴黎" << "莫斯科" << "罗马" << "马德里"
         << "柏林" << "威尼斯" << "汉堡" << "维也纳" << "米兰";
//    ui->listWidget->addItems(city);
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

void WorldListWea::on_AfBtn_clicked()
{
    ui->listWidget->clear();
    QStringList city;
    city << "开罗" << "约翰内斯堡" << "开普敦" << "内罗毕" << "阿尔及尔"
         << "拉各斯" << "金沙萨" << "德班" << "达喀尔" << "达累斯萨拉姆";
//    ui->listWidget->addItems(city);
    AddItem("开罗","17°~28°","://res/type/Qing.png");
    AddItem("约翰内斯堡","11°~25°","://res/type/Qing.png");
    AddItem("开普敦","12°~22°","://res/type/Qing.png");
    AddItem("内罗毕","17°~24°","://res/type/Yin.png");
    AddItem("阿尔及尔","19°~28°","://res/type/Qing.png");
    AddItem("拉各斯","28°~30°","://res/type/Yin.png");
    AddItem("金沙萨","20°~29°","://res/type/Yu.png");
    AddItem("德班","20°~25°","://res/type/Qing.png");
    AddItem("达喀尔","21°~25°","://res/type/Qing.png");
    AddItem("达累斯萨拉姆","24°~30°","://res/type/Yu.png");
}

void WorldListWea::on_NaBTn_clicked()
{
    ui->listWidget->clear();
    QStringList city;
    city << "墨西哥城" << "纽约" << "洛杉矶" << "多伦多" << "芝加哥"
         << "休斯顿" << "费城" << "里约热内卢" << "圣保罗" << "圣地亚哥";
//    ui->listWidget->addItems(city);
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

void WorldListWea::on_OaBTn_clicked()
{
//    appearButton();
//    ui->pushButton->setText("悉尼");
//    ui->pushButton_2->setText("墨尔本");
//    ui->pushButton_3->setText("布里斯班");
    ui->listWidget->clear();
    QStringList city;
    city << "悉尼" << "墨尔本" << "布里斯班" << "珀斯" << "阿德莱德"
         << "奥克兰" << "惠灵顿" << "皇后镇" << "堪培拉" << "哈密尔顿";
//    ui->listWidget->addItems(city);
    AddItem("悉尼","14°~23°","://res/type/Qing.png");
    AddItem("墨尔本","13°~25°","://res/type/Yin.png");
    AddItem("布里斯班","14°~23°","://res/type/Qing.png");
    AddItem("珀斯","7°~20°","://res/type/Qing.png");
    AddItem("阿德莱德","18°~23°","://res/type/Yu.png");
    AddItem("奥克兰","13°~17°","://res/type/Yin.png");
    AddItem("惠灵顿","9°~18°","://res/type/Qing.png");
    AddItem("皇后镇","1°~9°","://res/type/Yu.png");
    AddItem("堪培拉","7°~21°","://res/type/Yin.png");
    AddItem("哈密尔顿","9°~17°","://res/type/Yin.png");
}

//当列表中项目被双击时
void WorldListWea::on_listWidget_itemDoubleClicked()
{
//     QString str = cityName;
//     qDebug() << cityName.toUtf8().data();
     this->hide();
     emit setData(cityName);
}

void WorldListWea::on_listWidget_itemClicked()
{
//    emit setData(cityName);
}

void WorldListWea::on_pushButton_2_clicked()
{
    emit setData(ui->pushButton_2->text());
}

void WorldListWea::getCity(QString)
{
    //    qDebug() << cityName;

}

void WorldListWea::GetData(QString str1)
{
//    dlg->hide();
//    this->show();
//      qDebug() << str1;
    if(str1 == "欧洲"){
        on_EuBTn_clicked();
    }else if(str1 == "非洲"){
        on_AfBtn_clicked();
    }else if(str1 == "美洲"){
        on_NaBTn_clicked();
    }else if(str1 == "大洋洲"){
        on_OaBTn_clicked();
    }else{
        on_AsiaBtn_clicked();
    }

}
