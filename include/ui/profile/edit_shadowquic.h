#ifndef EDIT_SHADOWQUIC_H
#define EDIT_SHADOWQUIC_H

#include <QWidget>
#include "profile_editor.h"
#include "ui_edit_shadowquic.h"

namespace Ui {
class EditShadowquic;
}

class EditShadowquic : public QWidget, public ProfileEditor {
    Q_OBJECT

public:
    explicit EditShadowquic(QWidget *parent = nullptr);
    ~EditShadowquic() override;

    void onStart(std::shared_ptr<Configs::ProxyEntity> _ent) override;
    bool onEnd() override;
    QList<QPair<QPushButton *, QString>> get_editor_cached() override;

private:
    Ui::EditShadowquic *ui;
    std::shared_ptr<Configs::ProxyEntity> ent;
};

#endif // EDIT_SHADOWQUIC_H

