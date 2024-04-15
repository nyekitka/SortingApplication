#include "custom.h"
#include "ui_custom.h"
#include <QFileDialog>

custom::custom(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::custom)
{
    ui->setupUi(this);
    ui->warningLabel->setVisible(false);
    ui->lineEdit->setReadOnly(true);
}

custom::~custom()
{
    delete ui;
}



void custom::on_openButton_clicked()
{
    filePath = QFileDialog::getOpenFileName(this, "Выбрать текстовый файл", "C:\\", "Текстовый документ (*.txt);");
    ui->lineEdit->setText(filePath);
    ui->warningLabel->setVisible(false);
}


void custom::on_sortButton_clicked()
{
    QFile file(filePath);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        ui->warningLabel->setText("Не удалось открыть файл по указаному пути (попробуйте выбрать ещё раз)");
        ui->warningLabel->setVisible(true);
        ui->lineEdit->setText("");
        return;
    }
    QTextStream input(&file);
    ArraySequence<int> arr;
    while (!input.atEnd() && arr.length() <= 1E6) {
        int t;
        input >> t;
        arr.append(t);
    }
    file.close();
    sort(&arr, [](int a, int b) {return a - b;});
    QString out = QFileDialog::getExistingDirectory(this, "Выбрать папку для сохранения", "C:\\") + "/sorted_array.txt";
    QFile outFile(out);
    outFile.open(QFile::WriteOnly | QFile::Text);
    QTextStream output(&outFile);
    for (auto i : arr) output << i << '\n';
}


void custom::on_comboBox_currentIndexChanged(int index)
{
    void (*list_of_sortings[])(Sequence<int, ArrayIterators<int>>*, int (*)(int, int))  = {
        bubble_sort<int>,
        shaker_sort<int>,
        insertion_sort<int>,
        selection_sort<int>,
        counting_sort<int>,
        bin_insertion_sort<int>,
        double_selection_sort<int>,
        tree_sort<int>,
        merge_sort<int>,
        heap_sort<int>,
        quick_sort<int>,
        bitonic_sort<int>
    };
    sort = list_of_sortings[index];
}

