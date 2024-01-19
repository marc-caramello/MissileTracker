#ifndef MYCUSTOMDELEGATE_H
#define MYCUSTOMDELEGATE_H

class MyCustomDelegate : public QStyledItemDelegate {
public:
    using QStyledItemDelegate::QStyledItemDelegate;

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        QStyleOptionViewItem centeredOption(option);
        centeredOption.displayAlignment = Qt::AlignCenter;
        QStyledItemDelegate::paint(painter, centeredOption, index);
    }
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        return nullptr;
    }
};

#endif // MYCUSTOMDELEGATE_H
