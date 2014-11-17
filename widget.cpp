#include <QString>
#include <QRegExp>
#include <QRegExpValidator>
#include <QGridLayout>
#include <QLabel>
#include <QSize>
#include <QtDebug>
#include "widget.h"

static const QString STR_INPUT_REGEXP("\\d+");
static const int EACH_ELEMENT_SIZE = 40;
static const int TIME_SLOT = 500;//msec

GameOfLifeWidget::GameOfLifeWidget(QWidget *parent) :
    QWidget(parent),
    m_uiForm(new Ui::Widget),
    m_gLayout(NULL),
    m_inputN(0),
    m_initialItems(0)
{
    InitialUI();

    m_timer.setInterval(TIME_SLOT);
    m_timer.setSingleShot(false);

    connect(m_uiForm->okButton, SIGNAL(clicked()), this, SLOT(OnClickOKbutton()));
    connect(this, SIGNAL(UpdateData()), this, SLOT(OnUpdateData()));
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(OnRunAlgorithm()));
}

GameOfLifeWidget::~GameOfLifeWidget()
{
    delete m_uiForm;
}

void GameOfLifeWidget::InitialUI()
{
    m_uiForm->setupUi(this);

    QRegExp regExp(STR_INPUT_REGEXP);
    m_uiForm->borderWidth->setValidator(new QRegExpValidator(regExp));
    m_uiForm->initialItems->setValidator(new QRegExpValidator(regExp));

    if (NULL == m_gLayout)
    {
       m_gLayout = new QGridLayout(m_uiForm->displayWindow);
    }

    m_uiForm->displayWindow->setMinimumSize(100, 100);
    m_uiForm->displayWindow->setLayout(m_gLayout);
}

void GameOfLifeWidget::CreateItems()
{
    for (int r = 0; r < m_inputN; ++r)
    {
        OneDimension d1;
        for (int c = 0; c < m_inputN; ++c)
        {
            QLabel* pItem = new QLabel();
            SetItemColor(pItem, Qt::blue);
            m_gLayout->addWidget(pItem, r, c);
            OneDimension::value_type tmp;
            tmp.first = 0;
            tmp.second = pItem;
            d1.push_back(tmp);
        }
        m_itemMap.push_back(d1);
    }
    int widthSize = EACH_ELEMENT_SIZE * m_inputN;
    m_uiForm->displayWindow->setMinimumSize(widthSize, widthSize);
}

void GameOfLifeWidget::AddAnimalsWithRandom()
{
    if (0 == m_initialItems)
    {
        m_initialItems = qrand() % (m_inputN * (m_inputN - 1)) + 3;
    }
    for (int count = 0; count < m_initialItems; ++count)
    {
        int rowIndex = qrand() % m_inputN;
        int colIndex = qrand() % m_inputN;
        m_itemMap[rowIndex][colIndex].first = 1;
    }
    emit UpdateData();
}

void GameOfLifeWidget::SetItemColor(QLabel* pItem, const QColor& color)
{
    QPalette pal;
    pal.setColor(QPalette::Background, color);
    if (false == pItem->autoFillBackground())
    {
        pItem->setAutoFillBackground(true);
    }
    pItem->setPalette(pal);
}

void GameOfLifeWidget::OnClickOKbutton()
{
    m_inputN = m_uiForm->borderWidth->text().toInt();
    m_initialItems = m_uiForm->initialItems->text().toInt();
    if (m_initialItems > (m_inputN * m_inputN))
    {
        m_initialItems = (m_inputN * m_inputN);
    }
    CreateItems();
    AddAnimalsWithRandom();
    m_timer.start();
}

void GameOfLifeWidget::OnUpdateData()
{
    for (int r = 0; r < m_inputN; ++r)
    {
        for (int c = 0; c < m_inputN; ++c)
        {
            if (1 == m_itemMap[r][c].first)
            {
                QLabel* pTmp = m_itemMap[r][c].second;
                SetItemColor(pTmp, Qt::yellow);
            }
        }
    }
}

void GameOfLifeWidget::OnRunAlgorithm()
{

}
