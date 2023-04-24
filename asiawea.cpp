#include "asiawea.h"
#include "ui_asiawea.h"

#include <QContextMenuEvent>
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
#include <QSslSocket>
#include <QThread>

#include "weatherData.h"

#define INCREMENT 2     //温度每升高或降低1度，y轴坐标的增量
#define POINT_RADIUS 3  //曲线秒点的大小
#define TEXT_OFFSET_X 9    //温度文本相对于点的偏移
#define TEXT_OFFSET_Y 7

AsiaWea::AsiaWea(QWidget *parent) : QMainWindow(parent),
    ui(new Ui::AsiaWea)
{
    ui->setupUi(this);
    //设置窗口属性
    setWindowFlag(Qt::FramelessWindowHint);  // 设置无边框
//    setFixedSize(width(), height());         // 设置固定窗口大小
    setWindowIcon(QIcon(":/res/WeaFor.png"));

    mExitMenu = new QMenu(this);
    mExitAct = new QAction();
    mExitAct->setText(tr("退出"));
    mExitAct->setIcon(QIcon(":/res/close.ico"));
    mExitMenu->addAction(mExitAct);
    //用lamba函数连接退出按钮，触发就退出该应用
    connect(mExitAct, &QAction::triggered, this, [=]() { qApp->exit(0); });

    //将控件添加到控件数组
    mWeekList << ui->lblWeek0 << ui->lblWeek1 << ui->lblWeek2 << ui->lblWeek3 << ui->lblWeek4 << ui->lblWeek5;
    mDateList << ui->lblDate0 << ui->lblDate1 << ui->lblDate2 << ui->lblDate3 << ui->lblDate4 << ui->lblDate5;

    mTypeList << ui->lblType0 << ui->lblType1 << ui->lblType2 << ui->lblType3 << ui->lblType4 << ui->lblType5;
    mTypeIconList << ui->lblTypeIcon0 << ui->lblTypeIcon1 << ui->lblTypeIcon2 << ui->lblTypeIcon3 << ui->lblTypeIcon4 << ui->lblTypeIcon5;

    mAqiList << ui->lblQuality0 << ui->lblQuality1 << ui->lblQuality2 << ui->lblQuality3 << ui->lblQuality4 << ui->lblQuality5;

    mFxList << ui->lblFx0 << ui->lblFx1 << ui->lblFx2 << ui->lblFx3 << ui->lblFx4 << ui->lblFx5;
    mFlList << ui->lblFl0 << ui->lblFl1 << ui->lblFl2 << ui->lblFl3 << ui->lblFl4 << ui->lblFl5;

    //网络请求
    mNetAccessManager = new QNetworkAccessManager(this);
    connect(mNetAccessManager,&QNetworkAccessManager::finished,this,&AsiaWea::onReplied);
    getWeatherInfo("新加坡");

    //给标签添加事件过滤器
//    ui->lblHighCurve->installEventFilter(this);
//    ui->lblLowCurve->installEventFilter(this);
    connect(ui->goBackBtn,&QPushButton::clicked,this,[=](){
        emit this->FirstBtn();
    });
    //back
    connect(ListBtn,&WorldListWea::BackButton,this,[=]{
        this->show();
        ListBtn->hide();
    });
    //list
    connect(ui->Jump,&QPushButton::clicked,this,[=](){
        ListBtn->show();
        this->hide();
    });
    //连接跟list页面的传参
    connect(ListBtn,SIGNAL(setData(QString)),this,SLOT(GetData(QString)));
}

AsiaWea::~AsiaWea()
{
    delete ui;
}

void AsiaWea::contextMenuEvent(QContextMenuEvent *event)
{
    //弹出右键菜单
    mExitMenu->exec(QCursor::pos());
    event->accept();
}

void AsiaWea::mousePressEvent(QMouseEvent *event)
{
    mOffset = event->globalPos() - this->pos();
}

void AsiaWea::mouseMoveEvent(QMouseEvent *event)
{
    this->move(event->globalPos() - mOffset);
}

void AsiaWea::getWeatherInfo(QString cityName)
{
    QUrl url("http://tianqiapi.com/api?version=v5&appid=95937852&appsecret=TP5lZbcz&city=" + cityName);
    mNetAccessManager->get(QNetworkRequest(url));
}

void AsiaWea::parseJson(QByteArray &byteArray)
{
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(byteArray,&err);
    if(err.error != QJsonParseError::NoError){
        return;
    }
    QJsonObject rootObj = doc.object();
//    qDebug() << rootObj.value("errmsg").toString();
    //解析日期和城市
    mToday.date = rootObj.value("update_time").toString().left(10);
    mToday.city = rootObj.value("city").toString();

    //解析日期
    QJsonArray objData = rootObj.value("data").toArray();
    QJsonObject objForecast = objData[0].toObject();
    mDay[1].high = objForecast.value("tem1").toString().toInt();
    mDay[1].low = objForecast.value("tem2").toString().toInt();

//    QJsonArray forecatArr = rootObj.value("month").toArray();
//    for(int j = 0;j < 6;j++){
//        QJsonObject objForecast = forecatArr[j].toObject();
//        mDay[j + 1].week = objForecast.value("dateOfWeek").toString();
//        mDay[j + 1].date = objForecast.value("date").toString();
//    }

    mToday.wendu = (mDay[1].high + mDay[1].low) / 2;

    mToday.high = mDay[1].high;
    mToday.low = mDay[1].low;

    //更新UI
    updateUI();

}

void AsiaWea::updateUI()
{
    //更新日期和城市
    ui->lblDate->setText(QDateTime::fromString(mToday.date,"yyyy-MM-dd").toString("yyyy/MM/dd"));
    ui->lblCity->setText(mToday.city);

    ui->lblTemp->setText(QString::number(mToday.wendu) +"°");
    ui->lblLowHigh->setText(QString::number(mToday.low) + "~" + QString::number(mToday.high) + "°C");

//    //更新六天的数据
    for(int j = 0;j <= 5;j++){
        ui->lblWeek0->setText("昨天");
        ui->lblWeek1->setText("今天");
        ui->lblWeek2->setText("明天");
    }
}

bool AsiaWea::eventFilter(QObject *watched, QEvent *event)
{
    //绘制最高温度曲线
    if(watched == ui->lblHighCurve && event->type() == QEvent::Paint){
        paintHighCurve();
    }
    //绘制最低温度曲线
    if(watched == ui->lblLowCurve && event->type() == QEvent::Paint){
        paintLowCurve();
    }
    return QWidget::eventFilter(watched,event);
}

void AsiaWea::paintHighCurve()
{
    QPainter painter(ui->lblHighCurve);

    // 抗锯齿
    painter.setRenderHint(QPainter::Antialiasing,true);

    //获取x坐标
    int pointX[6] = {0};
    for(int j = 0;j < 6;j++){
        pointX[j] = mWeekList[j]->pos().x() + mWeekList[j]->width() / 2;
    }
    //获取y坐标
    int tmpSum = 0;
    int tmpAvg = 0;
    for(int i = 0;i < 6;i++){
        tmpSum += mDay[i].high;
    }
    tmpAvg = tmpSum / 6;

    int pointY[6] = {0};
    int yCenter = ui->lblHighCurve->height() / 2;
    for(int i = 0;i < 6;i++){
        pointY[i] = yCenter - ((mDay[i].high - tmpAvg) * INCREMENT);
    }
    //绘制
    QPen pen = painter.pen();
    pen.setWidth(1);
    pen.setColor(QColor(255,170,0));

    painter.setPen(pen);
    painter.setBrush(QColor(255,170,0));    //设置画刷内部填充的颜色
    //画点、写文本
    for(int i = 0;i < 6;i++){
        painter.drawEllipse(QPoint(pointX[i],pointY[i]),POINT_RADIUS,POINT_RADIUS);

        //显示温度文本
        painter.drawText(pointX[i] - TEXT_OFFSET_X,pointY[i] - TEXT_OFFSET_Y,QString::number(mDay[i].high) + "°");
    }
    for(int i = 0;i < 5;i++){
        if(i == 0){
            pen.setStyle(Qt::DashLine);
            painter.setPen(pen);
        }else{
            pen.setStyle(Qt::SolidLine);
            painter.setPen(pen);
        }
        painter.drawLine(pointX[i],pointY[i],pointX[i + 1],pointY[i + 1]);
    }
    painter.restore();
}

void AsiaWea::paintLowCurve()
{
    QPainter painter(ui->lblLowCurve);

    // 抗锯齿
    painter.setRenderHint(QPainter::Antialiasing,true);

    //获取x坐标
    int pointX[6] = {0};
    for(int i = 0;i < 6;i++){
        pointX[i] = mWeekList[i]->pos().x() + mWeekList[i]->width() / 2;
    }
    //获取y坐标
    int tmpSum = 0;
    int tmpAvg = 0;
    for(int i = 0;i < 6;i++){
        tmpSum += mDay[i].low;
    }
    tmpAvg = tmpSum / 6;

    int pointY[6] = {0};
    int yCenter = ui->lblLowCurve->height() / 2;
    for(int i = 0;i < 6;i++){
        pointY[i] = yCenter - ((mDay[i].low - tmpAvg) * INCREMENT);
    }
    //绘制
    QPen pen = painter.pen();
    pen.setWidth(1);
    pen.setColor(QColor(0,255,255));

    painter.setPen(pen);
    painter.setBrush(QColor(0,255,255));    //设置画刷内部填充的颜色
    //画点、写文本
    for(int i = 0;i < 6;i++){
        painter.drawEllipse(QPoint(pointX[i],pointY[i]),POINT_RADIUS,POINT_RADIUS);

        //显示温度文本
        painter.drawText(pointX[i] - TEXT_OFFSET_X,pointY[i] - TEXT_OFFSET_Y,QString::number(mDay[i].low) + "°");
    }
    for(int i = 0;i < 5;i++){
        if(i == 0){
            pen.setStyle(Qt::DashLine);
            painter.setPen(pen);
        }else{
            pen.setStyle(Qt::SolidLine);
            painter.setPen(pen);
        }
        painter.drawLine(pointX[i],pointY[i],pointX[i + 1],pointY[i + 1]);
    }

    painter.restore();
}

void AsiaWea::GetData(QString city)
{
    if(city == ""){
        //QMessageBox::warning(this,"提示","没有选中城市，请点击五大洲进行选择",QMessageBox::Ok);
        return;
    }
    else{
        getWeatherInfo(city);
        ListBtn->hide();
        this->show();
        qDebug() << city;
    }
}

void AsiaWea::onReplied(QNetworkReply *reply){
//    qDebug()<< QSslSocket::sslLibraryBuildVersionString();

//    qDebug() << QStringLiteral("OpenSSL支持情况:") << QSslSocket::supportsSsl();
//    qDebug()<<"QSslSocket="<<QSslSocket::sslLibraryBuildVersionString();
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if(reply->error() != QNetworkReply::NoError || statusCode != 200){
        qDebug() << reply->errorString().toLatin1().data();
        QMessageBox::warning(this,"天气","请求数据失败",QMessageBox::Ok);
    }else{
        QByteArray  byteArray = reply->readAll();
        qDebug() << "Asia读所有：" << byteArray.data();
        parseJson(byteArray);
    }
    reply->deleteLater();
}
