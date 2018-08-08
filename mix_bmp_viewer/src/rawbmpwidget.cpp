#include "rawbmpwidget.h"

#include <QVBoxLayout>
#include <QPainter>
#include <QMouseEvent>
#include <QMimeData>

#include <limits.h>

#include "mix_bmp.h"

ImageWidget::ImageWidget(QWidget *parent)
    :QWidget(parent)
{
    img = NULL;
    isDragging = false;
    isClicked = false;

    setAcceptDrops(true);

    color = QColor(200, 0, 200);
    color2 = QColor(120, 0, 120);
    isPoint2Show = false;

}

ImageWidget::~ImageWidget()
{

}

int ImageWidget::updateImage(QImage *img)
{
    this->img = img;
    setMinimumSize((float)img->width(), (float)img->height());
    update();
    return 0;
}

int ImageWidget::setMouseDrag(int x, int y, int w, int h)
{
    x_pressed = x;
    y_pressed = y;
    x_dragging = x+w;
    y_dragging = y+h;
    this->isClicked = false;
    this->isDragging = true;
    update();
    return 0;
}

int ImageWidget::setMouseClick(int x, int y)
{
    x_pressed = x;
    y_pressed = y;
    this->isClicked = true;
    this->isDragging = false;
    isPoint2Show = false;
    update();
    return 0;
}

int ImageWidget::setPoint2(int x, int y)
{
    isPoint2Show = true;
    x2 = x;
    y2 = y;
    update();
    return 0;
}

void ImageWidget::paintEvent(QPaintEvent *event)
{
    if(NULL == img)
        return;

    QPainter painter(this);
    painter.drawImage(0, 0, *img);



    painter.setPen(color);
    if(isDragging)
    {
        // Draw dragging rectangle.
        int xl = qMin(x_dragging, x_pressed);
        int yt = qMin(y_dragging, y_pressed);
        int w = qAbs(x_dragging - x_pressed);
        int h = qAbs(y_dragging - y_pressed);
        painter.drawRect(xl, yt, w, h);
    }

    if(isClicked)
    {
        int r=2,w=1;
        QBrush brush(color);
        painter.setPen(QPen(brush, w) );
        painter.drawLine(x_pressed-r,y_pressed, x_pressed+r, y_pressed);
        painter.drawLine(x_pressed,y_pressed-r, x_pressed, y_pressed+r);
    }

    if(isPoint2Show)
    {
        int r=2,w=1;
        QBrush brush(color2);
        painter.setPen(QPen(brush, w) );
        painter.drawLine(x2-r,y2, x2+r, y2);
        painter.drawLine(x2,y2-r, x2, y2+r);
    }

}

void ImageWidget::mousePressEvent(QMouseEvent *me)
{
    if(NULL == img)
        return;

    x_pressed = me->x();
    y_pressed = me->y();
//    is_pressed = true;
}

void ImageWidget::mouseMoveEvent(QMouseEvent *me)
{
    if(NULL == img)
        return;

    x_dragging = me->x();
    y_dragging = me->y();
    isDragging = true;
    isClicked = false;
    update();
}

void ImageWidget::mouseReleaseEvent(QMouseEvent *me)
{
    if(NULL == img)
        return;

    if(me->x() == x_pressed && me->y() == y_pressed)
    {
//        sendMouseClick(x_pressed, y_pressed);
        isClicked = true;
        isDragging = false;
        update();
        if(me->button() == Qt::LeftButton)
        {
            emit sendMouseClick(x_pressed, y_pressed);
        }
        else if(me->button() == Qt::RightButton)
        {
            emit sendMouseRightClick(x_pressed, y_pressed);
        }
    }
    else
    {
        int xl = qMin(me->x(), x_pressed);
        int yt = qMin(me->y(), y_pressed);
        int w = qAbs(me->x()-x_pressed);
        int h = qAbs(me->y()-y_pressed);
        isClicked = false;
        isDragging = true;
        emit sendMouseDrag(xl, yt, w, h);
    }
    isDragging = false;
}

void ImageWidget::dragEnterEvent(QDragEnterEvent *event)
{
//    setText(tr("<drop content>"));
    setBackgroundRole(QPalette::Highlight);

    event->acceptProposedAction();
//    emit changed(event->mimeData());
}

void ImageWidget::dragMoveEvent(QDragMoveEvent *event)
{
    event->acceptProposedAction();
}

void ImageWidget::dropEvent(QDropEvent *event)
{

    const QMimeData *mimeData = event->mimeData();

    if (mimeData->hasUrls())
    {
        emit fileDroped(mimeData->urls().at(0).path());
    }

    setBackgroundRole(QPalette::Dark);
    event->acceptProposedAction();
}

void ImageWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
//    clear();
    event->accept();
}

RawBmpWidget::RawBmpWidget(QWidget *parent)
    :QWidget(parent)
{
    factor = 0.0625;
    bf = 21.6;
    currX = 0;
    currY = 0;
    currW = 0;
    currH = 0;
    isSecMapOpen = false;

    setAcceptDrops(true);

//    this->leftOrRight = leftOrRight;
    iwImage = new ImageWidget(this);
//    ivImage->setAlignment(Qt::AlignCenter);
    lbInfo1 = new QLabel("none", this);
    lbInfo2 = new QLabel("none", this);


    lbXl = new QLabel("xl:", this);
    lbYt = new QLabel("yt:", this);
    leX = new QLineEdit(this);
    leY = new QLineEdit(this);
    lbFac = new QLabel("Fac:", this);
    lbBf = new QLabel("Bf:", this);
    leFac = new QLineEdit(this);
    leFac->setText(QString::number(factor));
    leBf = new QLineEdit(this);
    leBf->setText(QString::number(bf));
    lbXrlc = new QLabel("xrlc:",this);
    leXrlc = new QLineEdit(this);
    lbDrlc = new QLabel("drlc:",this);
    leDrlc = new QLineEdit(this);
    QGridLayout * lytXY = new QGridLayout();
    lytXY->addWidget(lbXl, 0, 0, 1, 1);
    lytXY->addWidget(leX, 0, 1, 1, 1);
    lytXY->addWidget(lbYt, 0, 2, 1, 1);
    lytXY->addWidget(leY, 0, 3, 1, 1);
    lytXY->addWidget(lbFac, 1, 0, 1, 1);
    lytXY->addWidget(leFac, 1, 1, 1, 1);
    lytXY->addWidget(lbBf, 1, 2, 1, 1);
    lytXY->addWidget(leBf, 1, 3, 1, 1);
    lytXY->addWidget(lbXrlc, 2, 0, 1, 1);
    lytXY->addWidget(leXrlc, 2, 1, 1, 1);
    lytXY->addWidget(lbDrlc, 2, 2, 1, 1);
    lytXY->addWidget(leDrlc, 2, 3, 1, 1);

    QVBoxLayout *lytMain = new QVBoxLayout(this);


    lytMain->addWidget(iwImage, 1);
    lytMain->addWidget(lbInfo1, 1);
    lytMain->addWidget(lbInfo2, 1);
    lytMain->addLayout(lytXY);
    lytMain->addStretch(999);

    connect(iwImage, SIGNAL(sendMouseClick(int,int)), this,
            SLOT(onIWMouseClick(int,int)));
    connect(iwImage, SIGNAL(sendMouseDrag(int,int,int,int)), this,
            SLOT(onIWMouseDrag(int,int,int,int)));
    connect(iwImage, SIGNAL(sendMouseRightClick(int,int)), this,
            SLOT(onIWMouseRightClick(int,int)));

    connect(leX, SIGNAL(textChanged(QString)), this, SLOT(onXYChanged(QString)));
    connect(leY, SIGNAL(textChanged(QString)), this, SLOT(onXYChanged(QString)));

    connect(leBf, SIGNAL(textChanged(QString)), this, SLOT(onBfChanged(QString)));
    connect(leFac, SIGNAL(textChanged(QString)), this, SLOT(onFacChanged(QString)));

}

RawBmpWidget::~RawBmpWidget()
{

}

int RawBmpWidget::open(QString path)
{
    uint8_t *data_rgb;
    read_raw_bmp_file(path.toStdString(), &mapW, &mapH, &type, &buf, &data_rgb,
                      &cmap);
    cvt_rgb_to_bgr(data_rgb, mapW, mapH);
    QImage *qimg_rgb = new QImage(data_rgb, mapW, mapH, QImage::Format_RGB888);
    iwImage->updateImage(qimg_rgb);
    update();
    return 0;
}

int RawBmpWidget::clean()
{
    delete buf;
    return 0;
}

int RawBmpWidget::setInfoClickNorBmp(int x, int y)
{
//    uint8_t
    return 0;
}

int RawBmpWidget::setInfoClickI16(int x, int y)
{
    int16_t v = ((int16_t*)buf)[y*mapW+x];
    float vf = (float)v * factor;
    QString info = "x:"+QString::number(x) + ", y:"+QString::number(y) +
            ", v:"+QString::number(vf, 'g', 4);

    if(abs(bf)>0.001 && abs(vf)>0.001)
    {
        info += ", dist:"+QString::number(bf/vf, 'g', 4);
    }
    lbInfo1->setText(info);
    lbInfo2->setVisible(false);
    currX = x;
    currY = y;
    currW = 0;
    currH = 0;

    return 0;
}

int RawBmpWidget::setInfoClickFloat(int x, int y)
{
    float v = ((float*)buf)[y*mapW+x];
    float vf = v * factor;
    QString info = "x:"+QString::number(x) + ", y:"+QString::number(y) +
            ", v:"+QString::number(vf, 'g', 4);

    if(abs(bf)>0.001 && abs(vf)>0.001)
    {
        info += ", dist:"+QString::number(bf/vf, 'g', 4);
    }
    lbInfo1->setText(info);
    lbInfo2->setVisible(false);
    currX = x;
    currY = y;
    currW = 0;
    currH = 0;

    return 0;
}

int RawBmpWidget::setInfoDragI16(int x, int y, int w, int h)
{
    double sum = 0.0, N= 0.0, avr = 0.0;
    int16_t vmin = SHRT_MAX, vmax = SHRT_MIN;
    for(int r=y; r<y+h; r++)
    {
        for(int c=x; c<x+w; c++)
        {
            int16_t vi = ((int16_t*)buf)[r*mapW+c];

            // Invalid value.
            if((double)vi < cmap.min || (double)vi > cmap.max)
            {
                continue;
            }

            if(vi < vmin)
                vmin = vi;
            if(vi > vmax)
                vmax = vi;

            sum += (double)vi;
            N += 1.0;
        }
    }
    avr = (float)(sum/N);
//    *avr = (float)((int)(*avr*100))/100;  // set precision.

    float vminf = (float)vmin*factor;
    float vmaxf = (float)vmax*factor;
    avr *= factor;

    QString  info = "xl:"+QString::number(x) + ", yt:"+QString::number(y)
            +", w:" + QString::number(w) + ", h:" + QString::number(h)
            +", xr:"+ QString::number(x+w) + ", yb:" +QString::number(y+h);
    lbInfo1->setText(info);
    info = "min:" + QString::number(vminf, 'g', 4)
            + ", max:"+QString::number(vmaxf, 'g', 4)
            + ", avr:"+QString::number(avr, 'g', 4);

    if(abs(bf)>0.001 && abs(avr)>0.001)
    {
        info += ", dist:"+QString::number(bf/avr, 'g', 4);
    }

    lbInfo2->setText(info);
    lbInfo2->setVisible(true);

    currX  = x;
    currY = y;
    currW = w;
    currH = h;
    return 0;
}

int RawBmpWidget::cvt_rgb_to_bgr(uint8_t *data, int W, int H)
{
    for(int r=0; r<H; r++)
    {
        for(int c= 0; c<W; c++)
        {
            uint8_t tmp;
            tmp = data[3*(r*W+c+0)];
            data[3*(r*W+c)+0] = data[3*(r*W+c)+2];
            data[3*(r*W+c)+2] = tmp;
        }
    }
    return 0;
}

void RawBmpWidget::onIWMouseClick(int x, int y)
{

    switch(type)
    {
        case NOT_RAW_BMP:
            setInfoClickNorBmp(x, y);
        break;
        case RAW_BMP_INT16 :
            setInfoClickI16(x, y);
        break;
        case RAW_BMP_FLOAT :
            setInfoClickFloat(x, y);
        break;
    }
}

void RawBmpWidget::onIWMouseDrag(int x, int y, int w, int h)
{
    switch(type)
    {
        case RAW_BMP_INT16 :
            setInfoDragI16(x, y, w, h);
        break;
    }
}

void RawBmpWidget::dropEvent(QDropEvent *event)
{
    leX->setText(event->mimeData()->text());
}


void RawBmpWidget::onXYChanged(QString s)
{
    int x = leX->text().toInt();
    int y = leY->text().toInt();
    setInfoClickI16(x, y);
    iwImage->setMouseClick(x, y);
}

void RawBmpWidget::onFacChanged(QString s)
{
    factor = s.toFloat();
    if(0 >= currW && 0 >= currH)
    {
        setInfoClickI16(currX, currY);
    }
    else
    {
        setInfoDragI16(currX, currY, currW, currH);
    }
}

void RawBmpWidget::onBfChanged(QString s)
{
    bf = s.toFloat();

    if(0 >= currW && 0 >= currH)
    {
        setInfoClickI16(currX, currY);
    }
    else
    {
        setInfoDragI16(currX, currY, currW, currH);
    }
}

void RawBmpWidget::onIWMouseRightClick(int x, int y)
{
    int16_t v = ((int16_t*)buf)[y*mapW+x];
//    v = (int)((float)v * factor+0.5);
    v = (int)((float)v * factor);
    setInfoClickI16(x, y);
    emit acqRightToLeftCheck(x-v, y);

}

void RawBmpWidget::onExtMouseClick(int x, int y)
{
    iwImage->setMouseClick(x, y);
    setInfoClickI16(x, y);
}

void RawBmpWidget::onAcqRightToLeftCheck(int x, int y)
{
    int16_t v = ((int16_t*)buf)[y*mapW+x];
    v = (int)((float)v * factor /*+0.5*/);
    iwImage->setMouseClick(x, y);
    setInfoClickI16(x, y);
    emit ackRightToLeftCheck(x+v, y);
}

void RawBmpWidget::onAckRightToLeftCHeck(int x, int y)
{
    rlcX = x;
    rlcY = y;
    leXrlc->setText(QString::number(rlcX));
    leDrlc->setText(QString::number(rlcX-currX));
    iwImage->setPoint2(x, y);
}


MainWindow::MainWindow()
{
    rbwMainMap = new RawBmpWidget(this);
    rbwSecMap = new RawBmpWidget(this);

    QWidget *centerWidget = new QWidget(this);
    QHBoxLayout * centerLayout = new QHBoxLayout();
    centerLayout->addWidget(rbwMainMap);
    centerLayout->addWidget(rbwSecMap);
    centerWidget->setLayout(centerLayout);
    setCentralWidget(centerWidget);

    isSecMapOpen = false;
    rbwSecMap->setVisible(isSecMapOpen);


    connect(rbwMainMap->iwImage, SIGNAL(fileDroped(QString)), this, SLOT(onFileDroped(QString)));


}


MainWindow::~MainWindow()
{

}

void MainWindow::onFileDroped(QString path)
{
    path = path.right(path.length()-1); // Remove the first '/' in the string.
    rbwSecMap->open(path);
    isSecMapOpen = true;
    rbwSecMap->setVisible(isSecMapOpen);

    connect(rbwMainMap->iwImage, SIGNAL(sendMouseClick(int,int)), rbwSecMap,
            SLOT(onExtMouseClick(int,int)));
    connect(rbwSecMap->iwImage, SIGNAL(sendMouseClick(int,int)), rbwMainMap,
            SLOT(onExtMouseClick(int,int)));

    connect(rbwMainMap, SIGNAL(acqRightToLeftCheck(int,int)), rbwSecMap,
            SLOT(onAcqRightToLeftCheck(int,int)));
    connect(rbwSecMap, SIGNAL(ackRightToLeftCheck(int,int)), rbwMainMap,
            SLOT(onAckRightToLeftCHeck(int,int)));
}
