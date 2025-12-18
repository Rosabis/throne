#include "include/ui/profile/edit_juicity.h"

EditJuicity::EditJuicity(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::EditJuicity) {

    ui->setupUi(this);
}

EditJuicity::~EditJuicity() {
    delete ui;
}

void EditJuicity::onStart(std::shared_ptr<Configs::ProxyEntity> _ent) {
    this->ent = _ent;
    auto outbound = this->ent->Juicity();

    ui->uuid->setText(outbound->uuid);
    ui->password->setText(outbound->password);
    ui->congestion_control->setCurrentText(outbound->congestion_control);
    ui->udp_relay_mode->setCurrentText(outbound->udp_relay_mode);
    ui->udp_over_stream->setChecked(outbound->udp_over_stream);
    ui->zero_rtt_handshake->setChecked(outbound->zero_rtt_handshake);
    ui->heartbeat->setText(outbound->heartbeat);
}

bool EditJuicity::onEnd() {
    auto outbound = this->ent->Juicity();

    outbound->uuid = ui->uuid->text();
    outbound->password = ui->password->text();
    outbound->congestion_control = ui->congestion_control->currentText();
    outbound->udp_relay_mode = ui->udp_relay_mode->currentText();
    outbound->udp_over_stream = ui->udp_over_stream->isChecked();
    outbound->zero_rtt_handshake = ui->zero_rtt_handshake->isChecked();
    outbound->heartbeat = ui->heartbeat->text();
    
    return true;
}

QList<QPair<QPushButton *, QString>> EditJuicity::get_editor_cached() {
    return {};
}

