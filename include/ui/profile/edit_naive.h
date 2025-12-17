#ifndef EDIT_NAIVE_H
#define EDIT_NAIVE_H

#include <QWidget>
#include "profile_editor.h"
#include "ui_edit_naive.h"

namespace Ui {
    class EditNaive;
}

class EditNaive : public QWidget, public ProfileEditor {
    Q_OBJECT

public:
    explicit EditNaive(QWidget *parent = nullptr);

    ~EditNaive() override;

    void onStart(std::shared_ptr<Configs::ProxyEntity> _ent) override;

    bool onEnd() override;

    QList<QPair<QPushButton *, QString>> get_editor_cached() override;

private:
    Ui::EditNaive *ui;
    std::shared_ptr<Configs::ProxyEntity> ent;

    struct {
        QString certificate;
        QString extra_headers;
    } CACHE;

private slots:
    void on_certificate_clicked();
    void on_extra_headers_clicked();
};

#endif // EDIT_NAIVE_H




