#include "include/ui/profile/edit_naive.h"

#include <QInputDialog>

EditNaive::EditNaive(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::EditNaive) {

    ui->setupUi(this);
    
    connect(ui->certificate, &QPushButton::clicked, this, &EditNaive::on_certificate_clicked);
    connect(ui->extra_headers, &QPushButton::clicked, this, &EditNaive::on_extra_headers_clicked);
}

EditNaive::~EditNaive() {
    delete ui;
}

void EditNaive::onStart(std::shared_ptr<Configs::ProxyEntity> _ent) {
    this->ent = _ent;
    auto outbound = this->ent->Naive();

    ui->username->setText(outbound->username);
    ui->password->setText(outbound->password);
    ui->protocol->setCurrentText(outbound->protocol);
    ui->sni->setText(outbound->sni);
    ui->insecure_concurrency->setText(Int2String(outbound->insecure_concurrency));
    ui->disable_log->setChecked(outbound->disable_log);
    
    CACHE.extra_headers = outbound->extra_headers;
    CACHE.certificate = outbound->certificate;
}

bool EditNaive::onEnd() {
    auto outbound = this->ent->Naive();

    outbound->username = ui->username->text();
    outbound->password = ui->password->text();
    outbound->protocol = ui->protocol->currentText();
    outbound->sni = ui->sni->text();
    outbound->insecure_concurrency = ui->insecure_concurrency->text().toInt();
    outbound->disable_log = ui->disable_log->isChecked();
    
    outbound->extra_headers = CACHE.extra_headers;
    outbound->certificate = CACHE.certificate;
    
    return true;
}

QList<QPair<QPushButton *, QString>> EditNaive::get_editor_cached() {
    return {
        {ui->certificate, CACHE.certificate},
        {ui->extra_headers, CACHE.extra_headers},
    };
}

void EditNaive::on_certificate_clicked() {
    bool ok;
    auto txt = QInputDialog::getMultiLineText(this, tr("Certificate"), "", CACHE.certificate, &ok);
    if (ok) {
        CACHE.certificate = txt;
        editor_cache_updated();
    }
}

void EditNaive::on_extra_headers_clicked() {
    bool ok;
    auto txt = QInputDialog::getMultiLineText(this, tr("Extra headers"), "", CACHE.extra_headers, &ok);
    if (ok) {
        CACHE.extra_headers = txt;
        editor_cache_updated();
    }
}




