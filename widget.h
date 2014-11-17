#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QList>
#include <QTimer>
#include <QPair>
#include "ui_widget.h"

typedef QList<QPair<int, QLabel*> > OneDimension;
typedef QList<OneDimension> TwoDimension;

class GameOfLifeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GameOfLifeWidget(QWidget *parent = 0);
    ~GameOfLifeWidget();
private:
    void InitialUI();
    void CreateItems();
    void AddAnimalsWithRandom();
    void SetItemColor(QLabel* pItem, const QColor& color);

private slots:
    void OnClickOKbutton();
    void OnUpdateData();
    void OnRunAlgorithm();

signals:
    void UpdateData();

private:
    Ui::Widget* m_uiForm;
    QGridLayout* m_gLayout;
    int m_inputN;
    int m_initialItems;
    QTimer m_timer;
    TwoDimension m_itemMap;
};

#endif // WIDGET_H
