#ifndef CUSTOM_H
#define CUSTOM_H

#include <QDialog>
#include "sorts.hpp"

namespace Ui {
class custom;
}

class custom : public QDialog
{
    Q_OBJECT

public:
    explicit custom(QWidget *parent = nullptr);
    ~custom();

private slots:


    void on_openButton_clicked();

    void on_sortButton_clicked();

    void on_comboBox_currentIndexChanged(int index);

private:
    Ui::custom *ui;
    QString filePath;
    void (*sort) (Sequence<int, ArrayIterators<int> >*, int (*)(int, int)) = bubble_sort<int>;
};

#endif // CUSTOM_H
