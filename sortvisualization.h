#ifndef SORTVISUALIZATION_H
#define SORTVISUALIZATION_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QTimer>
#include "sorts.hpp"

namespace Ui {
class SortVisualization;
}

class SortVisualization : public QWidget
{
    Q_OBJECT

public:
    explicit SortVisualization(QWidget *parent = nullptr);
    ~SortVisualization();
    enum Mix {
        mixed, ascending, descending
    };
private:
    QGraphicsScene* scene;
    QVector<QGraphicsRectItem*> rectangles;
    ArraySequence<int> array;
    Mix mix = mixed;
private slots:

    void change_diagram();

    int binary_search(int, int, int);
    void heapify(int);
    void stable_heap(int, int);
    void make_bitonic(DynamicArray<int>&, int, int);
    void btsort(DynamicArray<int>&, int, int, bool);


    void bubble_sort();
    void shaker_sort();
    void insertion_sort();
    void selection_sort();
    void counting_sort();
    void bin_insertion_sort();
    void double_selection_sort();
    void tree_sort();
    void merge_sort(int, int);
    void heap_sort();
    void quick_sort(int, int);
    void shell_sort(ShellSortSequence seq);
    void bitonic_sort();

    void on_typeComboBox_currentIndexChanged(int index);

    void on_sizeSlider_sliderReleased();

    void on_sizeSlider_sliderMoved(int position);

    void on_sizeSpinBox_editingFinished();

    void on_runButton_clicked();

    void on_pauseButton_clicked();

private:
    Ui::SortVisualization *ui;
    QVector<int> compared;
    int selected = -1;
    QVector< QVector<int> > actions;
    size_t actions_number = 0;
    QTimer* timer;
    bool waspaused = false;
    int multiplier = 50;
};

#endif // SORTVISUALIZATION_H
