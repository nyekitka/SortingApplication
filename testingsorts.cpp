#include "testingsorts.h"
#include "ui_testingsorts.h"



TestingSorts::TestingSorts(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TestingSorts)
{
    ui->setupUi(this);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->stopButton->setDisabled(true);
    setLayout(ui->horizontalLayout_3);
}
void SortWorker::run(const QStringList& rows, size_t firstCount, size_t increment) {
    srand(time(NULL));
    isProcessing = true;
    auto f = [&](){
        std::map<QString, void (*)(Sequence<int, ArrayIterators<int> >* , int (*) (int, int))> sorts = {
            {"Пузырьковая сортировка", bubble_sort<int>},
            {"Шейкерная сортировка", shaker_sort<int>},
            {"Сортировка вставками", insertion_sort<int>},
            {"Сортировка выбором", selection_sort<int>},
            {"Сортировка подсчётом", counting_sort<int>},
            {"Сортировка двоичными вставками", bin_insertion_sort<int>},
            {"Двусторонняя сортировка выбором", double_selection_sort<int>},
            {"Сортировка деревом", tree_sort<int>},
            {"Сортировка слиянием", merge_sort<int>},
            {"Пирамидальная сортировка", heap_sort<int>},
            {"Быстрая сортировка", quick_sort<int>},
            {"Сортировка Шелла", [](Sequence<int, ArrayIterators<int> >* seq, int (*cmp) (int, int)) {shell_sort(seq, cmp);}},
            {"Сортировка Шелла (последовательность Хиббарда)", [](Sequence<int, ArrayIterators<int> >* seq, int (*cmp) (int, int)) {shell_sort(seq, cmp, HibbardSequence);}},
            {"Сортировка Шелла (последовательность Седжвика)", [](Sequence<int, ArrayIterators<int> >* seq, int (*cmp) (int, int)) {shell_sort(seq, cmp, SedgewickSequence);}},
            {"Сортировка Шелла (последовательность Пратта)", [](Sequence<int, ArrayIterators<int> >* seq, int (*cmp) (int, int)) {shell_sort(seq, cmp, PrattSequence);}},
            {"Сортировка Шелла (последовательность Фибоначчи)", [](Sequence<int, ArrayIterators<int> >* seq, int (*cmp) (int, int)) {shell_sort(seq, cmp, FibonacciSequence);}},
            {"Битонная сортировка", bitonic_sort<int>}
        };
        ArraySequence<ArraySequence<int>> arrays;
        for (size_t i = 0; i < 10; ++i) {
            ArraySequence<int> arr;
            for (size_t j = 0; j < firstCount + i*increment; ++j) arr.append(rand());
            arrays.append(arr);
        }
        QStringListIterator it(rows);
        for (size_t i = 0; it.hasNext(); ++i) {
            QString name = it.next();
            for (size_t j = 0; j < arrays.length(); ++j) {
                if (!isProcessing) return;
                clock_t begin = clock();
                sorts[name](&arrays[j], [](int a, int b){return a - b;});
                size_t result = clock() - begin;
                emit sortCompleted(i, j, result);
                if (!isProcessing) return;
                for (size_t k = 0; k < arrays[j].length(); ++k) arrays[j][k] = rand();
            }
        }
    };
    f();
    emit sortFinished();
}
void SortWorker::stop() {
    isProcessing = false;
}
TestingSorts::~TestingSorts()
{
    delete ui;
}



void TestingSorts::on_startButton_clicked()
{
    setEnabledCheckBoxPanel(false);
    ui->firstCountSlider->setEnabled(false);
    ui->firstCountSpinBox->setEnabled(false);
    ui->stepSlider->setEnabled(false);
    ui->stepSpinBox->setEnabled(false);
    if (!isTableClear) {
        for (size_t i = 0; i < ui->tableWidget->rowCount(); ++i) {
            for (size_t j = 0; j < ui->tableWidget->columnCount(); ++j) {
                if (ui->tableWidget->item(i, j)) ui->tableWidget->item(i, j)->setText("");
            }
        }
        isTableClear = true;
    }
    thread = new QThread;
    worker = new SortWorker;
    connect(thread, &QThread::started, [&](){
        worker->run(activated_sorts, firstCount, increment);
    });
    connect(worker, &SortWorker::sortCompleted, this, &TestingSorts::updateCell);
    connect(worker, &SortWorker::sortFinished, thread, &QThread::quit);
    connect(thread, &QThread::finished, [&]() {
        ui->startButton->setEnabled(true);
        ui->startButton->setText("Старт");
        setEnabledCheckBoxPanel(true);
        ui->firstCountSlider->setEnabled(true);
        ui->firstCountSpinBox->setEnabled(true);
        ui->stepSlider->setEnabled(true);
        ui->stepSpinBox->setEnabled(true);
        thread->deleteLater();
        worker->deleteLater();
    });
    worker->moveToThread(thread);
    ui->startButton->setDisabled(true);
    ui->startButton->setText("В процессе...");
    ui->stopButton->setEnabled(true);
    thread->start();
}
void TestingSorts::updateCell(size_t row, size_t column, size_t result) {
    if (ui->tableWidget->item(row, column)) {
        ui->tableWidget->item(row, column)->setText(tr("%1 мс").arg(result));
    } else {
        QTableWidgetItem* item = new QTableWidgetItem(tr("%1 мс").arg(result));
        ui->tableWidget->setItem(row, column, item);
    }
    isTableClear = false;
}

void TestingSorts::on_stopButton_clicked() {
    worker->stop();
}

void TestingSorts::setEnabledCheckBoxPanel(bool arg) {
    ui->bubbleSortBox->setEnabled(arg);
    ui->shakerSortBox->setEnabled(arg);
    ui->insertionSortBox->setEnabled(arg);
    ui->binInsertSortBox->setEnabled(arg);
    ui->selectionSortBox->setEnabled(arg);
    ui->doubleSelectionSortBox->setEnabled(arg);
    ui->quickSortBox->setEnabled(arg);
    ui->mergeSortBox->setEnabled(arg);
    ui->shellSortBox->setEnabled(arg);
    ui->hibbardSortBox->setEnabled(arg);
    ui->prattSortBox->setEnabled(arg);
    ui->sedgewickSortBox->setEnabled(arg);
    ui->countingSortBox->setEnabled(arg);
    ui->treeSortBox->setEnabled(arg);
    ui->fiSortBox->setEnabled(arg);
    ui->bitonicSortBox->setEnabled(arg);
    ui->heapSortBox->setEnabled(arg);
    ui->stopButton->setDisabled(arg);
    ui->chooseAllBox->setEnabled(arg);
}

#define CHECKBOX_CLICK(name)                                                        \
void TestingSorts::on_##name##SortBox_stateChanged(int state) {                     \
    if (state == Qt::Checked) {                                                     \
        ui->tableWidget->setRowCount(ui->tableWidget->rowCount() + 1);              \
        activated_sorts.append(ui->name##SortBox->text());                          \
        ui->tableWidget->setVerticalHeaderLabels(activated_sorts);                  \
    }                                                                               \
    else {                                                                          \
        size_t index = activated_sorts.indexOf(ui->name##SortBox->text());          \
        ui->tableWidget->removeRow(index);                                          \
        activated_sorts.remove(index);                                              \
    }                                                                               \
}

void TestingSorts::on_firstCountSlider_sliderMoved(int position) {
    ui->firstCountSpinBox->setValue(position);
}


void TestingSorts::on_firstCountSpinBox_valueChanged(int arg1) {
    ui->firstCountSlider->setValue(arg1);
    firstCount = arg1;
    QStringList labels;
    for (size_t i = 0; i < 10; ++i) {
        labels << tr("%1 эл.").arg(firstCount + i * increment);
    }
    ui->tableWidget->setHorizontalHeaderLabels(labels);
}


void TestingSorts::on_stepSlider_sliderMoved(int position) {
    ui->stepSpinBox->setValue(position);
}


void TestingSorts::on_stepSpinBox_valueChanged(int arg1) {
    ui->stepSlider->setValue(arg1);
    increment = arg1;
    QStringList labels;
    for (size_t i = 0; i < 10; ++i) {
        labels << tr("%1 эл.").arg(firstCount + i * increment);
    }
    ui->tableWidget->setHorizontalHeaderLabels(labels);
}


void TestingSorts::on_chooseAllBox_stateChanged(int state)
{
    if (state == Qt::Checked) {
        ui->bubbleSortBox->setCheckState(Qt::Checked);
        ui->shakerSortBox->setCheckState(Qt::Checked);
        ui->insertionSortBox->setCheckState(Qt::Checked);
        ui->selectionSortBox->setCheckState(Qt::Checked);
        ui->countingSortBox->setCheckState(Qt::Checked);
        ui->binInsertSortBox->setCheckState(Qt::Checked);
        ui->doubleSelectionSortBox->setCheckState(Qt::Checked);
        ui->treeSortBox->setCheckState(Qt::Checked);
        ui->heapSortBox->setCheckState(Qt::Checked);
        ui->mergeSortBox->setCheckState(Qt::Checked);
        ui->quickSortBox->setCheckState(Qt::Checked);
        ui->shellSortBox->setCheckState(Qt::Checked);
        ui->hibbardSortBox->setCheckState(Qt::Checked);
        ui->sedgewickSortBox->setCheckState(Qt::Checked);
        ui->prattSortBox->setCheckState(Qt::Checked);
        ui->fiSortBox->setCheckState(Qt::Checked);
        ui->bitonicSortBox->setCheckState(Qt::Checked);
    }
}




CHECKBOX_CLICK(bubble)
CHECKBOX_CLICK(shaker)
CHECKBOX_CLICK(insertion)
CHECKBOX_CLICK(binInsert)
CHECKBOX_CLICK(selection)
CHECKBOX_CLICK(doubleSelection)
CHECKBOX_CLICK(quick)
CHECKBOX_CLICK(merge)
CHECKBOX_CLICK(shell)
CHECKBOX_CLICK(hibbard)
CHECKBOX_CLICK(pratt)
CHECKBOX_CLICK(sedgewick)
CHECKBOX_CLICK(counting)
CHECKBOX_CLICK(tree)
CHECKBOX_CLICK(fi)
CHECKBOX_CLICK(bitonic)
CHECKBOX_CLICK(heap)
