#ifndef TESTINGSORTS_H
#define TESTINGSORTS_H

#include <QWidget>
#include <QThread>
#include <QLayout>
#include <QDebug>
#include <map>
#include "sorts.hpp"

class SortWorker : public QObject {
    Q_OBJECT
public slots:
    void run(const QStringList&, size_t, size_t);
    void stop();
signals:
    void sortCompleted(size_t row, size_t column, size_t time);
    void sortFinished();
private:
    bool isProcessing = false;
};

namespace Ui {
class TestingSorts;
}

class TestingSorts : public QWidget
{
    Q_OBJECT
private slots:
    void updateCell(size_t row, size_t column, size_t result);
    void on_startButton_clicked();
    void on_stopButton_clicked();
    void on_bubbleSortBox_stateChanged(int);
    void on_shakerSortBox_stateChanged(int);
    void on_insertionSortBox_stateChanged(int);
    void on_selectionSortBox_stateChanged(int);
    void on_countingSortBox_stateChanged(int);
    void on_binInsertSortBox_stateChanged(int);
    void on_doubleSelectionSortBox_stateChanged(int);
    void on_treeSortBox_stateChanged(int);
    void on_heapSortBox_stateChanged(int);
    void on_mergeSortBox_stateChanged(int);
    void on_quickSortBox_stateChanged(int);
    void on_shellSortBox_stateChanged(int);
    void on_hibbardSortBox_stateChanged(int);
    void on_sedgewickSortBox_stateChanged(int);
    void on_prattSortBox_stateChanged(int);
    void on_fiSortBox_stateChanged(int);
    void on_bitonicSortBox_stateChanged(int);
    void setEnabledCheckBoxPanel(bool);
    void on_firstCountSlider_sliderMoved(int position);

    void on_firstCountSpinBox_valueChanged(int arg1);

    void on_stepSlider_sliderMoved(int position);

    void on_stepSpinBox_valueChanged(int arg1);

    void on_chooseAllBox_stateChanged(int arg1);

public:
    explicit TestingSorts(QWidget *parent = nullptr);
    ~TestingSorts();

private:
    Ui::TestingSorts *ui;
    QThread* thread;
    SortWorker* worker;
    QStringList activated_sorts;
    bool isTableClear = true;
    int firstCount = 1000;
    int increment = 1000;
};

#endif // TESTINGSORTS_H
