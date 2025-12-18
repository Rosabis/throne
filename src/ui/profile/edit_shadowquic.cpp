#include "include/ui/profile/edit_shadowquic.h"

EditShadowquic::EditShadowquic(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::EditShadowquic) {
    ui->setupUi(this);
}

EditShadowquic::~EditShadowquic() {
    delete ui;
}

void EditShadowquic::onStart(std::shared_ptr<Configs::ProxyEntity> _ent) {
    this->ent = _ent;
    auto outbound = this->ent->ShadowQUIC();

    ui->server->setText(outbound->server);
    ui->port->setValue(outbound->server_port > 0 ? outbound->server_port : 1443);
    ui->username->setText(outbound->username);
    ui->password->setText(outbound->password);
    ui->server_name->setText(outbound->server_name);
    ui->alpn->setText(outbound->alpn.join(","));
    ui->initial_mtu->setValue(outbound->initial_mtu <= 0 ? 1300 : outbound->initial_mtu);
    ui->congestion_control->setText(outbound->congestion_control.isEmpty() ? "bbr" : outbound->congestion_control);
    ui->zero_rtt->setChecked(outbound->zero_rtt);
    ui->over_stream->setChecked(outbound->over_stream);
}

bool EditShadowquic::onEnd() {
    auto outbound = this->ent->ShadowQUIC();
    outbound->server = ui->server->text().trimmed();
    outbound->server_port = ui->port->value();
    outbound->username = ui->username->text();
    outbound->password = ui->password->text();
    outbound->server_name = ui->server_name->text();
    outbound->alpn = ui->alpn->text().split(",", Qt::SkipEmptyParts);
    for (auto &s : outbound->alpn) s = s.trimmed();
    outbound->initial_mtu = ui->initial_mtu->value();
    outbound->congestion_control = ui->congestion_control->text();
    outbound->zero_rtt = ui->zero_rtt->isChecked();
    outbound->over_stream = ui->over_stream->isChecked();
    return true;
}

QList<QPair<QPushButton *, QString>> EditShadowquic::get_editor_cached() {
    return {};
}

