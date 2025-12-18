#ifndef EDIT_JUICITY_H
#define EDIT_JUICITY_H

#include <QWidget>
#include "profile_editor.h"
#include "ui_edit_juicity.h"

namespace Ui {
    class EditJuicity;
}

class EditJuicity : public QWidget, public ProfileEditor {
    Q_OBJECT

public:
    explicit EditJuicity(QWidget *parent = nullptr);

    ~EditJuicity() override;

    void onStart(std::shared_ptr<Configs::ProxyEntity> _ent) override;

    bool onEnd() override;

    QList<QPair<QPushButton *, QString>> get_editor_cached() override;

private:
    Ui::EditJuicity *ui;
    std::shared_ptr<Configs::ProxyEntity> ent;
};

#endif // EDIT_JUICITY_H

