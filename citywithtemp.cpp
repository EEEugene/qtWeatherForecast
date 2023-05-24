#include "citywithtemp.h"
#include "ui_citywithtemp.h"
#include <QDebug>

QString cityName;

cityWithTemp::cityWithTemp(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::cityWithTemp)
{
    ui->setupUi(this);
}

cityWithTemp::~cityWithTemp()
{
    delete ui;
}

void cityWithTemp::SetData(QString str1, QString str2,const QString& pic)
{
    ui->citylbl->setText(str1);
    ui->templbl->setText(str2);

    QPixmap pixmapPic(pic);
    int iWidth = ui->piclbl->width();
    int iHeight = ui->piclbl->height();
    QPixmap pixmapPicFit = pixmapPic.scaled(iWidth, iHeight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);//饱满填充

    ui->piclbl->setPixmap(pixmapPicFit);
}

void cityWithTemp::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);
    cityName = ui->citylbl->text();
//    qDebug() << cityName;
}
