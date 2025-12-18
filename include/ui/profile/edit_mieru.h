#ifndef EDIT_MIERU_H
#define EDIT_MIERU_H

#include <QWidget>
#include "profile_editor.h"
#include "ui_edit_mieru.h"

namespace Ui {
    class EditMieru;
}

class EditMieru : public QWidget, public ProfileEditor {
    Q_OBJECT

public:
    explicit EditMieru(QWidget *parent = nullptr);

    ~EditMieru() override;

    void onStart(std::shared_ptr<Configs::ProxyEntity> _ent) override;

    bool onEnd() override;

    QList<QPair<QPushButton *, QString>> get_editor_cached() override;

private:
    Ui::EditMieru *ui;
    std::shared_ptr<Configs::ProxyEntity> ent;

    struct {
        QString port_bindings;
    } CACHE;

private slots:
    void on_port_bindings_clicked();
};

#endif // EDIT_MIERU_H

