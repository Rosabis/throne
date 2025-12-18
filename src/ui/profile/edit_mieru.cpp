#include "include/ui/profile/edit_mieru.h"

#include <QInputDialog>

EditMieru::EditMieru(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::EditMieru) {

    ui->setupUi(this);
    
    connect(ui->port_bindings, &QPushButton::clicked, this, &EditMieru::on_port_bindings_clicked);
}

EditMieru::~EditMieru() {
    delete ui;
}

void EditMieru::onStart(std::shared_ptr<Configs::ProxyEntity> _ent) {
    this->ent = _ent;
    auto outbound = this->ent->Mieru();

    ui->username->setText(outbound->username);
    ui->password->setText(outbound->password);
    ui->profile_name->setText(outbound->profile_name);
    ui->mtu->setText(Int2String(outbound->mtu));
    ui->multiplexing->setCurrentText(outbound->multiplexing.isEmpty() ? "MULTIPLEXING_LOW" : outbound->multiplexing);
    ui->handshake_mode->setCurrentText(outbound->handshake_mode.isEmpty() ? "HANDSHAKE_STANDARD" : outbound->handshake_mode);
    
    // Store port_bindings and protocols as a formatted string for editing
    QStringList lines;
    for (int i = 0; i < outbound->port_bindings.size() && i < outbound->protocols.size(); i++) {
        lines << (outbound->port_bindings[i] + ":" + outbound->protocols[i]);
    }
    CACHE.port_bindings = lines.join("\n");
}

bool EditMieru::onEnd() {
    auto outbound = this->ent->Mieru();

    outbound->username = ui->username->text();
    outbound->password = ui->password->text();
    outbound->profile_name = ui->profile_name->text();
    outbound->mtu = ui->mtu->text().toInt();
    if (outbound->mtu < 1280 || outbound->mtu > 1400) outbound->mtu = 1400;
    outbound->multiplexing = ui->multiplexing->currentText();
    outbound->handshake_mode = ui->handshake_mode->currentText();
    
    // Parse port_bindings and protocols from formatted string
    QStringList lines = CACHE.port_bindings.split("\n", Qt::SkipEmptyParts);
    outbound->port_bindings.clear();
    outbound->protocols.clear();
    for (const auto &line : lines) {
        QString trimmed = line.trimmed();
        if (trimmed.isEmpty()) continue;
        if (trimmed.contains(":")) {
            auto parts = trimmed.split(":");
            if (parts.size() >= 2) {
                outbound->port_bindings << parts[0].trimmed();
                outbound->protocols << parts[1].trimmed();
            }
        } else {
            // Only port, default to TCP
            outbound->port_bindings << trimmed;
            outbound->protocols << "TCP";
        }
    }
    
    return true;
}

QList<QPair<QPushButton *, QString>> EditMieru::get_editor_cached() {
    return {
        {ui->port_bindings, CACHE.port_bindings},
    };
}

void EditMieru::on_port_bindings_clicked() {
    bool ok;
    QString placeholder = "Format: port:protocol (one per line)\n"
                         "Example:\n"
                         "2012-2022:TCP\n"
                         "2027:TCP\n"
                         "6489:UDP";
    auto txt = QInputDialog::getMultiLineText(this, tr("Port Bindings"), placeholder, CACHE.port_bindings, &ok);
    if (ok) {
        CACHE.port_bindings = txt;
        editor_cache_updated();
    }
}

