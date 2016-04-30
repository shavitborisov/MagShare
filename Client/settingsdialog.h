#ifndef SETTINGS_DIALOG_H
#define SETTINGS_DIALOG_H

#include <QDialog>

namespace Ui {
class Settings;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();

private slots:
    void on_buttonBox_accepted();

    void on_pushDestFolder_clicked();
    void on_pushMSFfolder_clicked();

private:
    Ui::Settings *ui;
    void assignSettings();
    void keepSettings();
};

#endif // SETTINGS_DIALOG_H
