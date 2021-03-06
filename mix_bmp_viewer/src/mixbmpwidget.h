#ifndef mixBMPWIDGET_H
#define mixBMPWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QMessageBox>
#include <cstdint>

#include "mix_bmp.h"


#define MSG_REPORT(x)   \
    do{                 \
    QMessageBox msgBox; \
    msgBox.setText(x);  \
    msgBox.exec();      \
    }while(0)           \

class ImageWidget : public QWidget
{
    Q_OBJECT
public:
    ImageWidget(QWidget *parent);
    ~ImageWidget();
    int updateImage(QImage *img);

    int setMouseDrag(int x, int y, int w, int h);
    int setMouseClick(int x, int y);
    int setPoint2(int x, int y);

signals:
    void sendMouseClick(int x, int y);
    void sendMouseDrag(int xl, int yt, int w, int h);
    void sendMouseRightClick(int x, int y);
    void fileDroped(QString path);

protected:
    void mousePressEvent(QMouseEvent *me);
    void mouseReleaseEvent(QMouseEvent *me);
    void mouseMoveEvent(QMouseEvent *me);
    void paintEvent(QPaintEvent *event);


    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;
    void dropEvent(QDropEvent *event) override;


private:
    int x_pressed, y_pressed, x_dragging, y_dragging;
    int x2, y2; // Secondary show point.
    bool isPoint2Show;

    QImage *img;

    bool isDragging;
    bool isClicked;
    QColor color, color2;

};

class mixBmpWidget : public QWidget
{
    Q_OBJECT
public:
    mixBmpWidget(QWidget *parent = 0);
    ~mixBmpWidget();
    int open(QString path);
    int clean();
    ImageWidget *iwImage;

protected:
    int setInfoClickNorBmp(int x, int y);   // Normal bmp file.
    int setInfoClickI16(int x, int y);
    int setInfoDragI16(int x, int y, int w, int h);
    int setInfoClickFloat(int x, int y);
    int cvt_rgb_to_bgr(uint8_t *data, int W, int H);
    int mapW, mapH;
    color_map_t cmap;
    mix_bmp_type_t type;    // If open a mix bmp, record its type.
    int bmpNumChn;  // If open a normal bmp, record its channel numbers.
    void *buf;
    QLabel *lbXl, *lbYt, *lbXr, *lbYb;
    QLineEdit *leX,*leY;
    QLabel *lbFac, *lbBf;
    QLineEdit *leFac,*leBf;
    QLabel *lbXrlc, *lbDrlc;    // x,d of right to left check,
    QLineEdit *leXrlc, *leDrlc;
    QLabel *lbChanVal[3];
    QLineEdit *leChnVal[3];

    float factor;
    float bf;

    int currX,currY,currW,currH;
    bool isSecMapOpen;
    int rlcX, rlcY;   // x,y of Right to left check

protected:
    QLabel *lbInfo1, *lbInfo2;

signals:
    void acqRightToLeftCheck(int x, int y);
    void ackRightToLeftCheck(int x, int y);

protected slots:
    void onIWMouseClick(int x, int y);
    void onIWMouseDrag(int x, int y, int w, int h);
    void dropEvent(QDropEvent *event);

    void onXYChanged(QString s);
    void onFacChanged(QString s);
    void onBfChanged(QString s);
    void onIWMouseRightClick(int x, int y);

public slots:
    void onExtMouseClick(int x, int y);
    void onAcqRightToLeftCheck(int x, int y);
    void onAckRightToLeftCHeck(int x, int y);


};

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow();
    ~MainWindow();

    mixBmpWidget *rbwMainMap, *rbwSecMap;

private:
    bool isSecMapOpen;

protected slots:
    void onFileDroped(QString path);
};

#endif // mixBMPWIDGET_H
