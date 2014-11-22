#include <QString>
#include <QRegExp>
#include <QRegExpValidator>
#include <QGridLayout>
#include <QLabel>
#include <QSize>
#include <QtDebug>
#include <QPair>
#include "widget.h"

static const QString STR_INPUT_REGEXP("\\d+");
static const int EACH_ELEMENT_SIZE = 20;
static const int TIME_SLOT = 500;//msec
static const QList<QPair<int, int> > DIRECTION = QList<QPair<int, int> >() <<
    qMakePair(-1, -1) << qMakePair(-1, 0) << qMakePair(-1, 1) <<
    qMakePair(0, -1) << qMakePair(0, 1) <<
    qMakePair(1, -1) << qMakePair(1, 0) << qMakePair(1, 1);

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
    connect(m_uiForm->startStopButton, SIGNAL(clicked()), this, SLOT(OnClickStartStop()));
    connect(this, SIGNAL(UpdateData()), this, SLOT(OnUpdateData()));
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(OnRunAlgorithm()));
    connect(m_uiForm->nextButton, SIGNAL(clicked()), this, SLOT(OnRunAlgorithm()));
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

    m_uiForm->startStopButton->setEnabled(false);

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

bool GameOfLifeWidget::IsItem(int row, int col)
{
    Q_ASSERT(0 <= row && row < m_itemMap.size());
    Q_ASSERT(0 <= col && col < m_itemMap[row].size());
    return 1 == m_itemMap[row][col].first || 2 == m_itemMap[row][col].first;
}

bool GameOfLifeWidget::IsRemoved(int row, int col)
{
    Q_ASSERT(0 <= row && row < m_itemMap.size());
    Q_ASSERT(0 <= col && col < m_itemMap[row].size());
    return 2 == m_itemMap[row][col].first;
}

void GameOfLifeWidget::ClearALL()
{
    for (int row = 0; row < m_itemMap.size(); ++row)
    {
        for (int col = 0; col < m_itemMap[row].size(); ++col)
        {
            m_gLayout->removeWidget(m_itemMap[row][col].second);
        }
        m_itemMap[row].clear();
    }
    m_itemMap.clear();
}

int GameOfLifeWidget::GetItemCount(int row, int col, const int widthSize)
{
    int itemCount = 0;
    for (QList<QPair<int, int> >::const_iterator cIter = DIRECTION.constBegin(); cIter != DIRECTION.constEnd(); ++cIter)
    {
        const int rowDirection = row + cIter->first;
        const int colDirection = col + cIter->second;
        if (0 <= rowDirection && widthSize > rowDirection &&
            0 <= colDirection && widthSize > colDirection)
        {
            if (IsItem(rowDirection, colDirection))
            {
                ++itemCount;
            }
        }
    }
    return itemCount;
}

void GameOfLifeWidget::OnClickOKbutton()
{
    m_uiForm->startStopButton->setEnabled(true);
    m_inputN = m_uiForm->borderWidth->text().toInt();
    m_initialItems = m_uiForm->initialItems->text().toInt();
    if (m_initialItems > (m_inputN * m_inputN))
    {
        m_initialItems = (m_inputN * m_inputN);
    }
    ClearALL();
    CreateItems();
    AddAnimalsWithRandom();
}

void GameOfLifeWidget::OnUpdateData()
{
    for (int r = 0; r < m_inputN; ++r)
    {
        for (int c = 0; c < m_inputN; ++c)
        {
            QColor color = Qt::green;
            if (1 == m_itemMap[r][c].first)
            {
                color = Qt::red;
            }
            else if (0 == m_itemMap[r][c].first)
            {
                color = Qt::blue;
            }
            QLabel* pTmp = m_itemMap[r][c].second;
            SetItemColor(pTmp, color);
        }
    }
}

void GameOfLifeWidget::OnRunAlgorithm()
{
    QList<QPair<int, int> >deadList;
    QList<QPair<int, int> >bornList;
    //record dead and born
    //4~8, 0~1 dead ->2
    //2,3 live
    //3 born ->3
    for (int r = 0; r < m_itemMap.size(); ++r)
    {
        for (int c = 0; c < m_itemMap[r].size(); ++c)
        {
            int itemCount = GetItemCount(r, c, m_itemMap[r].size());
            if (1 == m_itemMap[r][c].first)
            {
                if ((4 <= itemCount && 8 >= itemCount) || (0 == itemCount || 1 == itemCount))
                {
                    deadList.push_back(qMakePair<int, int>(r, c));
                }
            }
            else
            {
                if (3 == itemCount)
                {
                    bornList.push_back(qMakePair<int, int>(r, c));
                }
            }
        }
    }

    for (int i = 0; i < deadList.size(); ++i)
    {
        m_itemMap[deadList[i].first][deadList[i].second].first = 0;
    }

    for (int i = 0; i < bornList.size(); ++i)
    {
        m_itemMap[bornList[i].first][bornList[i].second].first = 1;
    }
    emit UpdateData();
}

void GameOfLifeWidget::OnClickStartStop()
{
    if (m_timer.isActive())
    {
        m_timer.stop();
    }
    else
    {
        m_timer.start();
    }
}
