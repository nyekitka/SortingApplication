#include "sortvisualization.h"
#include "ui_sortvisualization.h"

void SortVisualization::change_diagram() {
    timer->stop();
    if (actions_number == actions.size()) {
        ui->runButton->setEnabled(true);
        ui->speedSlider->setEnabled(true);
        ui->sizeSlider->setEnabled(true);
        ui->sortingsListWidget->setEnabled(true);
        ui->typeComboBox->setEnabled(true);
        actions.clear();
        actions_number = 0;
        selected = -1;
        for (size_t i = 0; i < rectangles.length(); ++i) rectangles[i]->setBrush(Qt::white);
        compared.clear();
        ui->typeComboBox->setCurrentIndex(1);
        return;
    }
    else if (actions[actions_number][0] == 1) {
        QString text = ui->accessCount->text();
        ui->accessCount->setText(QString::number(text.toInt() + actions[actions_number][1]));
    } else if (actions[actions_number][0] == 2) {
        if (actions[actions_number][1]) {
            QString text = ui->cmpCount->text();
            ui->cmpCount->setText(QString::number(text.toInt() + 1));
        }
        for (auto i : compared) rectangles[i]->setBrush(Qt::white);
        compared.clear();
        for (size_t i = 2; i < actions[actions_number].size(); ++i) {
            compared.append(actions[actions_number][i]);
            rectangles[compared.last()]->setBrush(Qt::green);
        }
    } else if (actions[actions_number][0] == 3){
        QString text = ui->swapCount->text();
        ui->swapCount->setText(QString::number(text.toInt() + 1));
        rectangles[actions[actions_number][2]]->setRect(actions[actions_number][2]*(scene->width()/array.length()), scene->height() - actions[actions_number][4]*(scene->height()/array.length()), scene->width()/array.length(), scene->height() + 5);
        rectangles[actions[actions_number][1]]->setRect(actions[actions_number][1]*(scene->width()/array.length()), scene->height() - actions[actions_number][3]*(scene->height()/array.length()), scene->width()/array.length(), scene->height() + 5);
    } else if (actions[actions_number][0] == 4) {
        QString text = ui->swapCount->text();
        ui->swapCount->setText(QString::number(text.toInt() + 1));
        rectangles[actions[actions_number][1]]->setRect(actions[actions_number][1]*(scene->width()/array.length()), scene->height() - actions[actions_number][2]*(scene->height()/array.length()), scene->width()/array.length(), scene->height() + 5);
        rectangles[actions[actions_number][1]]->setBrush(Qt::green);
        if (selected != -1) rectangles[selected]->setBrush(Qt::white);
        selected = actions[actions_number][1];
    } else {
        int i = actions[actions_number][1];
        if (i) rectangles[i - 1]->setBrush(Qt::green);
        else {
            if (selected != -1) rectangles[selected]->setBrush(Qt::white);
            for (auto j : compared) rectangles[j]->setBrush(Qt::white);
        }
        if (i < rectangles.length()) rectangles[i]->setBrush(Qt::red);
    }
    ++actions_number;
    timer->start(multiplier/ui->speedSlider->value());
}


int SortVisualization::binary_search(int value, int begin, int end) {
    int middle = begin + (end - begin) / 2;
    actions.append({1, 1});
    actions.append({2, 1, middle});
    int result = array[middle] - value;
    if (middle == begin) {
        if (result > 0) return begin;
        else return end;
    }
    if (result > 0) return binary_search(value, begin, middle);
    else if (result == 0) return middle;
    else return binary_search(value, middle, end);
}
void SortVisualization::heapify(int index) {
    if (index == 0) return;
    actions.append({1, 2});
    actions.append({2, 1, (index - 1)/2, index});
    if (array[index] > array[(index - 1) / 2]) {
        std::swap(array[index], array[(index - 1) / 2]);
        actions.append({3, (index - 1)/2, index, array[(index - 1)/2], array[index]});
        heapify((index - 1) / 2);
    }
}
void SortVisualization::stable_heap(int size, int index = 0) {
    if (size <= 1) return;
    int child1 = 2 * index + 1, child2 = 2 * index + 2;
    actions.append({1, 1});
    int max_value = array[index];
    if (child1 < size) {
        actions.append({1, 1});
        actions.append({2, 1, index, child1});
        max_value = std::max(array[child1], array[index]);
        if (child2 < size) {
            array[child1]> array[index] ? actions.append({2, 1, child1, child2}) : actions.append({2, 1, index, child2});
            max_value = std::max(array[child2], max_value);
        }
    }
    actions.append({1, 1});
    if (child1 < size && max_value == array[child1]) {
        actions.append({1, 2});
        std::swap(array[child1], array[index]);
        actions.append({3, index, child1, array[index], array[child1]});
        stable_heap(size, child1);
    }
    else if (child2 < size && max_value == array[child2]) {
        actions.append({1, 2});
        std::swap(array[child2], array[index]);
        actions.append({3, child1, child2, array[child1], array[child2]});
        stable_heap(size, child2);
    }
}
void SortVisualization::make_bitonic(DynamicArray<int>& cont, int begin, int end) {
    auto middle = begin + (end - begin) / 2;
    if (middle == begin) return;
    make_bitonic(cont, begin, middle);
    btsort(cont, begin, middle, true);
    make_bitonic(cont, middle, end);
    btsort(cont, middle, end, false);
}
void SortVisualization::btsort(DynamicArray<int>& cont, int begin, int end, bool less) {
    auto middle = begin + (end - begin) / 2;
    if (middle == begin) return;
    for (int it1 = begin, it2 = middle; it1 != middle && it2 != end; ++it1, ++it2) {
        if (it1 < array.length()) {
            actions.append({1, 2});
            if (it2 >= array.length())actions.append({2, 1, it1});
            else actions.append({2, 1, it1, it2});
        }
        if ((cont[it1] > cont[it2]) == less) {
            std::swap(cont[it1], cont[it2]);
            if (it1 < array.length()) {
                actions.append({1, 2});
                if (it2 >= array.length()) actions.append({4, it1, cont[it1]});
                else actions.append({3, it1, it2, cont[it1], cont[it2]});
            }
        }
    }
    btsort(cont, begin, middle, less);
    btsort(cont, middle, end, less);
}


void SortVisualization::bubble_sort() {
    actions.append({1, 1});
    for (int i = array.length() - 1; i > 0; --i) {
        for (int j = 0; j != i; ++j) {
            actions.append({1, 2});
            actions.append({2, 1, j, j + 1});
            if (array[j] > array[j + 1]) {
                std::swap(array[j], array[j + 1]);
                actions.append({3, j, j + 1, array[j], array[j + 1]});
                actions.append({1, 2});
            }
        }
    }
}
void SortVisualization::shaker_sort() {
    int start = 0, end = array.length() - 1, border = end;
    actions.append({1, 1});
    do {
        for (int i = start; i != end; ++i) {
            actions.append({1, 2});
            actions.append({2, 1, i, i + 1});
            if (array[i] > array[i + 1]) {
                std::swap(array[i], array[i + 1]);
                actions.append({3, i, i + 1, array[i], array[i + 1]});
                border = i;
            }
        }
        end = border;
        for (int i = end; i != start; --i) {
            actions.append({1, 2});
            actions.append({2, 1, i - 1, i});
            if (array[i - 1] > array[i]) {
                std::swap(array[i - 1], array[i]);
                actions.append({3, i - 1, i, array[i - 1], array[i]});
                border = i;
            }
        }
        start = border;
    } while (end - start > 0);
}
void SortVisualization::insertion_sort() {
    actions.append({1, 1});
    for (int i = 1; i != array.length(); ++i) {
        int j = i;
        while (j != 0 && array[j - 1] > array[j]) {
            actions.append({1, 2});
            actions.append({2, 1, j - 1, j});
            std::swap(array[j - 1], array[j]);
            actions.append({3, j - 1, j, array[j - 1], array[j]});
            --j;
        }
        if (j != 0) {
            actions.append({1, 2});
            actions.append({2, 1, j - 1, j});
        }
    }
}
void SortVisualization::selection_sort() {
    actions.append({1, 1});
    for (int i = 0; i < array.length(); ++i) {
        int min = i;
        actions.append({1, 1});
        for (int j = i; j <array.length(); ++j) {
            actions.append({1, 2});
            actions.append({2, 1, min, j});
            if (array[min] > array[j]) min = j;
        }
        std::swap(array[min], array[i]);
        actions.append({3, min, i, array[min], array[i]});
    }
}
void SortVisualization::counting_sort() {
    DynamicArray<int> counter(array.length() + 1);
    actions.append({1,2});
    for (int i = 0; i < array.length(); ++i) {
        actions.append({1,1});
        counter[array[i]]++;
    }
    int k = 0;
    for (int i = 0; i <= array.length(); ++i) {
       for (size_t j = 0; j < counter[i]; ++j) {
           actions.append({1,1});
           actions.append({4, k, i});
           array[k++] = i;
       }
    }
}
void SortVisualization::bin_insertion_sort() {
    for (int i = 1; i < array.length(); ++i) {
        actions.append({1, 2});
        int value = array[i];
        int place = binary_search(value, 0, i);
        for (int j = i; j != place; --j) {
            actions.append({1, 2});
            array[j] = array[j - 1];
            actions.append({4, j, array[j]});
        }
        array[place] = value;
        actions.append({1, 1});
        actions.append({4, place, value});
    }
}
void SortVisualization::double_selection_sort () {
    int end = array.length();
    actions.append({1, 1});
    for (auto i = 0; i < end; ++i) {
        int min = i;
        int max = i;
        for (int j = i; j != end; ++j) {
            actions.append({1, 2});
            actions.append({2, 1, min, j});
            if (array[min] > array[j]) min = j;
            actions.append({1, 2});
            actions.append({2, 1, max, j});
            if (array[max] < array[j]) max = j;
        }
        if (min == --end && max == i) {
            std::swap(array[min], array[max]);
            actions.append({3, min, max, array[min], array[max]});
        }
        else if (max == i) {
            std::swap(array[max], array[end]);
            actions.append({3, max, end, array[max], array[end]});
            std::swap(array[min], array[i]);
            actions.append({3, min, i, array[min], array[i]});
        }
        else {
            std::swap(array[min], array[i]);
            actions.append({3, min, i, array[min], array[i]});
            std::swap(array[max], array[end]);
            actions.append({3, max, end, array[max], array[end]});
        }
    }
}
void SortVisualization::tree_sort() {
    std::multiset<int> s;
    for (auto i : array) {
        actions.append({1, 1});
        s.insert(i);
    }
    int it = 0;
    for (auto i : s) {
        array[it] = i;
        actions.append({4, it, array[it++]});
    }
}
void SortVisualization::merge_sort(int begin, int end) {
    actions.append({1, 1});
    if (end - begin <= 1) return;
    int difference = end - begin;
    int middle = begin + difference / 2;
    merge_sort(begin, middle);
    merge_sort(middle, end);
    int ptr1 = begin, ptr2 = middle;
    DynamicArray<int> v;
    while (v.length() != difference) {
        actions.append({1, 1});
        if (ptr2 == end || ptr1 != middle && ptr2 != end && array[ptr1] < array[ptr2]) {
            if (ptr2 != end) {
                actions.append({1, 2});
                actions.append({2, 1, ptr1, ptr2});
            }
            v.append(array[ptr1++]);
        }
        else {
            actions.append({1, 2});
            v.append(array[ptr2++]);
        }
    }
    for (int it1 = begin, it2 = 0; it1 != end; ++it1, ++it2) {
        array[it1] = v[it2];
        actions.append({4, it1, array[it1]});
    }
}
void SortVisualization::heap_sort() {
    actions.append({1, 1});
    int size = array.length();
    if (size == 0) return;
    for (int i = 0; i < size; ++i) {
        heapify(i);
    }
    for (int i = 0; i < size; ++i) {
        std::swap(array[0], array[size - i - 1]);
        actions.append({3, 0, size - i - 1, array[0], array[size - i - 1]});
        stable_heap(size - i - 1);
    }
}
void SortVisualization::quick_sort(int begin, int end) {
    int size = end - begin;
    int difference = size - 1;
    if (size <= 1) return;
    actions.append({1, 1});
    int base = array[begin + rand() % size];
    int left = begin, right = end - 1;
    do {
        while (array[left] < base && difference > 0) {
            actions.append({1, 1});
            actions.append({2, 1, left, right});
            ++left;
            --difference;
        }
        actions.append({2, 1, left, right});
        actions.append({1, 1});
        while (array[right] > base && difference > 0) {
            actions.append({1, 1});
            actions.append({2, 1, left, right});
            --right;
            --difference;
        }
        actions.append({2, 1, left, right});
        actions.append({1, 1});
        if (difference > 0) {
            actions.append({1, 2});
            std::swap(array[left], array[right]);
            actions.append({3, left, right, array[left], array[right]});
            ++left;
            difference -= 1;
        }
    } while (difference > 0);
    if (left == begin) ++left;
    quick_sort(begin, left);
    quick_sort(left, end);
}
void SortVisualization::shell_sort(ShellSortSequence seq = DefaultSequence) {
    actions.append({1, 1});
    size_t size = array.length();
    DynamicArray<size_t> shell_seq = seq(size);
    for (auto step : shell_seq) {
        for (int i = step; i < size; ++i) {
            actions.append({1, 1});
            int t = array[i];
            int j = i;
            for (; j >= step; j -= step) {
                auto k = j - step;
                actions.append({2, 1, i, j - (int)step});
                if (t < array[j - step]) {
                    array[j] = array[j - step];
                    actions.append({4, j, array[j]});
                }
                else break;
            }
            array[j] = t;
            actions.append({4, j, array[j]});
        }
    }
}
void SortVisualization::bitonic_sort() {
    size_t size = array.length(), newsize = 1;
    while (newsize < size) newsize <<= 1;
    DynamicArray<int> arr(newsize, size);
    for (int i = 0; i != size; ++i) {
        arr[i] = array[i];
        actions.append({1, 1});
    }
    make_bitonic(arr, 0, newsize);
    btsort(arr, 0, newsize, true);
    for (size_t i = 0; i < size; ++i) array[i] = arr[i];
}


SortVisualization::SortVisualization(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SortVisualization)
{
    ui->setupUi(this);
    timer = new QTimer;
    scene = new QGraphicsScene(0, 0, 1024, 768, this);
    scene->addRect(scene->sceneRect());
    scene->setBackgroundBrush(Qt::black);
    for (size_t i = 0; i < 100; ++i) array.append(i + 1);
    for (size_t i = 0; i < 100; ++i) std::swap(array[i], array[rand()%100]);
    for (size_t i = 0; i < 100; ++i) {
        QGraphicsRectItem* rect = new QGraphicsRectItem(i*(scene->width()/100), scene->height() - array[i]*(scene->height()/100), scene->width()/100, scene->height() + 5);
        rect->setBrush(Qt::white);
        rectangles.append(rect);
        scene->addItem(rectangles[i]);
    }
    ui->graphicsView->setScene(scene);
    setLayout(ui->horizontalLayout_4);
    ui->sortingsListWidget->setCurrentItem(ui->sortingsListWidget->item(0));
    connect(timer, &QTimer::timeout, this, &SortVisualization::change_diagram);
}

SortVisualization::~SortVisualization()
{
    delete ui;
}



void SortVisualization::on_typeComboBox_currentIndexChanged(int index)
{
    waspaused = false;
    if (index == 0) {
        mix = mixed;
        for (size_t i = 0; i < array.length(); ++i) {
            std::swap(array[i], array[rand() % array.length()]);
        }
    }
    else if (index == 1) {
        mix = ascending;
        for (size_t i = 0; i < array.length(); ++i) {
            array[i] = i + 1;
        }
    }
    else {
        mix = descending;
        for (size_t i = 0; i < array.length(); ++i) {
            array[i] = array.length() - i;
        }
    }
    for (size_t i = 0; i < array.length(); ++i) {
        rectangles[i]->setRect(i*(scene->width()/array.length()), scene->height() - array[i]*(scene->height()/array.length()), scene->width()/array.length(), scene->height() + 5);
    }

}


void SortVisualization::on_sizeSlider_sliderReleased()
{
    waspaused = false;
    size_t size = ui->sizeSlider->value();
    if (size > array.length()) {
        for (size_t i = array.length(); i < size; ++i) array.append(i + 1);
        if (mix == mixed) {
            for (size_t i = 0; i < array.length(); ++i) std::swap(array[i], array[rand() % array.length()]);
        }
        else if (mix == ascending){
            for (size_t i = 0; i < rectangles.size(); ++i) array[i] = i + 1;
        }
        else {
            for (size_t i = 0; i < array.length(); ++i) array[i] = array.length() - i;
        }
        for (size_t i = 0; i < rectangles.size(); ++i) {
            rectangles[i]->setRect(i*(scene->width()/array.length()), scene->height() - array[i]*(scene->height()/array.length()), scene->width()/array.length(), scene->height() + 5);
            if (size > 340) rectangles[i]->setPen(QPen(Qt::white, 0, Qt::NoPen));
            else rectangles[i]->setPen(QPen(Qt::black, 1));
        }
        for (size_t i = rectangles.size(); i < array.length(); ++i) {
            QGraphicsRectItem* rect = new QGraphicsRectItem(i*(scene->width()/array.length()), scene->height() - array[i]*(scene->height()/array.length()), scene->width()/array.length(), scene->height() + 5);
            rect->setBrush(Qt::white);
            if (size > 340) rect->setPen(QPen(Qt::white, 0, Qt::NoPen));
            else rect->setPen(QPen(Qt::black, 1));
            rectangles.append(rect);
            scene->addItem(rect);
        }
    }
    else if (size < array.length()) {
        for (size_t i = 0; i < size; ++i) array[i] = i + 1;
        while (size < array.length()) array.erase(array.length() - 1);
        if (mix == mixed) {
            for (size_t i = 0; i < size; ++i) std::swap(array[i], array[rand() % size]);
        }
        else if (mix == descending) {
            for (size_t i = 0; i < size; ++i) array[i] = size - i;
        }
        for (size_t i = 0; i < array.length(); ++i) {
            rectangles[i]->setRect(i*(scene->width()/array.length()), scene->height() - array[i]*(scene->height()/array.length()), scene->width()/array.length(), scene->height() + 5);
            if (size > 340) rectangles[i]->setPen(QPen(Qt::white, 0, Qt::NoPen));
            else rectangles[i]->setPen(QPen(Qt::black, 1));
        }
        while (rectangles.length() != array.length()) {
            scene->removeItem(rectangles[rectangles.length() - 1]);
            rectangles.removeLast();
        }
    }
}


void SortVisualization::on_sizeSlider_sliderMoved(int position)
{
    ui->sizeSpinBox->setValue(position);
}


void SortVisualization::on_sizeSpinBox_editingFinished()
{
    ui->sizeSlider->setValue(ui->sizeSpinBox->value());
    on_sizeSlider_sliderReleased();
}


void SortVisualization::on_runButton_clicked()
{
    ui->runButton->setEnabled(false);
    ui->speedSlider->setEnabled(false);
    ui->sizeSlider->setEnabled(false);
    ui->sortingsListWidget->setEnabled(false);
    ui->typeComboBox->setEnabled(false);
    ui->pauseButton->setEnabled(true);
    if (!waspaused) {
        ui->swapCount->setText("0");
        ui->accessCount->setText("0");
        ui->cmpCount->setText("0");
        actions.clear();
        actions_number = 0;
        for (size_t i = 0; i < rectangles.length(); ++i) rectangles[i]->setBrush(Qt::white);
        compared.clear();
        if (ui->sortingsListWidget->currentItem()->text() == "Пузырьковая сортировка") bubble_sort(), multiplier = 100;
        else if (ui->sortingsListWidget->currentItem()->text() == "Шейкерная сортировка") shaker_sort(), multiplier = 100;
        else if (ui->sortingsListWidget->currentItem()->text() == "Сортировка вставками") insertion_sort(), multiplier = 100;
        else if (ui->sortingsListWidget->currentItem()->text() == "Сортировка выбором") selection_sort(), multiplier = 100;
        else if (ui->sortingsListWidget->currentItem()->text() == "Сортировка подсчётом") counting_sort(), multiplier = 400;
        else if (ui->sortingsListWidget->currentItem()->text() == "Сортировка двоичными вставками") bin_insertion_sort(), multiplier = 100;
        else if (ui->sortingsListWidget->currentItem()->text() == "Двусторонняя сортировка выбором") double_selection_sort(), multiplier = 100;
        else if (ui->sortingsListWidget->currentItem()->text() == "Сортировка деревом") tree_sort(), multiplier = 400;
        else if (ui->sortingsListWidget->currentItem()->text() == "Сортировка слиянием") merge_sort(0, array.length()), multiplier = 200;
        else if (ui->sortingsListWidget->currentItem()->text() == "Пирамидальная сортировка") heap_sort(), multiplier = 200;
        else if (ui->sortingsListWidget->currentItem()->text() == "Быстрая сортировка") quick_sort(0, array.length()), multiplier = 200;
        else if (ui->sortingsListWidget->currentItem()->text() == "Сортировка Шелла") shell_sort(), multiplier = 200;
        else if (ui->sortingsListWidget->currentItem()->text() == "Сортировка Шелла (последовательность Хиббарда)") shell_sort(HibbardSequence), multiplier = 200;
        else if (ui->sortingsListWidget->currentItem()->text() == "Сортировка Шелла (последовательность Седжвика)") shell_sort(SedgewickSequence), multiplier = 200;
        else if (ui->sortingsListWidget->currentItem()->text() == "Сортировка Шелла (последовательность Пратта)") shell_sort(PrattSequence), multiplier = 200;
        else if (ui->sortingsListWidget->currentItem()->text() == "Сортировка Шелла (последовательность Фибоначчи)") shell_sort(FibonacciSequence), multiplier = 200;
        else if (ui->sortingsListWidget->currentItem()->text() == "Битонная сортировка") bitonic_sort(), multiplier = 200;
        for (int i = 0; i <= rectangles.length(); ++i) actions.append({5, i});
    }
    timer->start(multiplier/ui->speedSlider->value());
    waspaused = false;
}


void SortVisualization::on_pauseButton_clicked()
{
    waspaused = true;
    timer->stop();
    ui->runButton->setEnabled(true);
    ui->speedSlider->setEnabled(true);
    ui->sizeSlider->setEnabled(true);
    ui->sortingsListWidget->setEnabled(true);
    ui->typeComboBox->setEnabled(true);
    ui->typeComboBox->setCurrentIndex(0);
    ui->pauseButton->setEnabled(false);
}

