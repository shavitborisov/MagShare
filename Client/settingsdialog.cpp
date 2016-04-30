#include "settingsdialog.h"
#include "ui_settings.h"
#include "settings.h"
#include <QtGui>
#include <QFileDialog>
#include <QFileInfo>


SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);

    // fill Log levels
    ui->comboLevel->addItem("INFO");
    ui->comboLevel->addItem("TRACE");
    ui->comboLevel->addItem("DEBUG");
    ui->comboLevel->addItem("WARN");
    ui->comboLevel->addItem("ERROR");
    ui->comboLevel->addItem("FATAL");

    assignSettings();
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::assignSettings()
{
    ui->lineHost->setText(Settings::trackerHost());
    ui->linePort->setText(QString::number(Settings::trackerPort()));
    ui->lineLogFile->setText(Settings::logFile());
    int idx = ui->comboLevel->findText(Settings::logLevel());
    if (idx != -1)
    {
        ui->comboLevel->setCurrentIndex(idx);
    }
    ui->lineDestFolder->setText(Settings::destinationFolder());
    ui->lineMSFFolder->setText(Settings::msfFolder());
}

void SettingsDialog::on_buttonBox_accepted()
{
    keepSettings();
    close();
}

void SettingsDialog::keepSettings(){
    Settings::assign(ui->lineHost->text(), ui->linePort->text().toInt(), ui->lineLogFile->text(),ui->comboLevel->currentText(),ui->lineDestFolder->text(),ui->lineMSFFolder->text());
    Settings::save();
}

void SettingsDialog::on_pushDestFolder_clicked()
{
    QString destDir = QFileDialog::getExistingDirectory(this,"Choose Destination Folder",ui->lineDestFolder->text(),QFileDialog::ShowDirsOnly);
    if (!destDir.isEmpty())
        ui->lineDestFolder->setText(destDir);
}

void SettingsDialog::on_pushMSFfolder_clicked()
{

    QString magDir = QFileDialog::getExistingDirectory(this,"Choose MagShare Files Folder",ui->lineMSFFolder->text(),QFileDialog::ShowDirsOnly);
    if (!magDir.isEmpty())
        ui->lineMSFFolder->setText(magDir);

}
