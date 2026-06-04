#include "inputdata.h"
#include "ui_inputdata.h"

#include <QFileDialog>
#include <QJsonArray>

void addJsonToItem(const QJsonValue &value, QStandardItem *parent);
QJsonValue modelItemToJson(QStandardItem *item);

InputData::InputData(QWidget *parent, GlobalDataTransition* data)
    : QWidget(parent)
    , ui(new Ui::InputData)
{
    ui->setupUi(this);
    this->data = data;
    filename = "";

    connect(ui->but_open_file, &QPushButton::clicked, this, &InputData::OpenFile);
    connect(ui->but_new_file, &QPushButton::clicked, this, &InputData::NewFile);
    connect(ui->but_save, &QPushButton::clicked, this, &InputData::SaveFile);
    connect(ui->but_cancel, &QPushButton::clicked, this, &InputData::ReopenFile);
}

InputData::~InputData()
{
    delete ui;
    delete data;
}

void InputData::NewFile() {
    ui->but_cancel->setEnabled(true);
    ui->but_save->setEnabled(true);
    ui->text_json->setEnabled(true);
    QString fileName = QFileDialog::getSaveFileName(this, "Создайте JSON файл", "", "*.json");
    if (fileName.isEmpty()) return;
    if (!fileName.endsWith(".json", Qt::CaseInsensitive)) fileName = fileName + ".json";

    QString textjson = "{\"default_count\":2,\"groups\":[\"A-02-30\",\"A-05-30\",\"A-08-30\",\"A-16-30\",\"A-18-30\"],\"subjects\":[\"math\",\"physics\",\"english\",\"IT\",\"economic\"],\"teachers\":[\"T1\",\"T2\",\"T3\",\"T4\",\"T5\",\"T6\",\"T7\"],\"rooms\":[\"m700\",\"m707\",\"m200\"],\"days\":[\"Monday\",\"Tuesday\",\"Wednesday\",\"Thursday\",\"Friday\"],\"times\":[\"9:20\",\"11:10\",\"13:45\",\"15:35\"],\"subject_teachers\":{\"math\":[\"T1\"],\"physics\":[\"T2\"],\"english\":[\"T3\"],\"economic\":[\"T4\"],\"IT\":[\"T5\",\"T6\",\"T7\"]},\"subject_count\":{\"A-18-30\":{\"math\":2},\"A-16-30\":{\"math\":1}}}";
    QJsonDocument doc = QJsonDocument::fromJson(textjson.toUtf8());
    this->data->SetData(doc.object());

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        ui->lab_info->setText("Ошибка открытия файла для записи:" + file.errorString());
        return;
    }
    file.write(doc.toJson());
    file.close();

    QJsonObject jsonObject = doc.object();
    this->data->SetData(jsonObject);
    ui->text_json->setPlainText(doc.toJson(QJsonDocument::Indented));
    this->filename = fileName;
    ui->lab_info->setText("Статус: Файл создан.");
    ui->lab_filename->setText("Файл: " + QFileInfo(this->filename).fileName());
}

void InputData::OpenFile(){
    ui->but_cancel->setEnabled(true);
    ui->but_save->setEnabled(true);
    ui->text_json->setEnabled(true);
    QString fileName = QFileDialog::getOpenFileName(this, "Выберите JSON файл", "", "*.json");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        ui->lab_info->setText("Невозможно открыть файл");
        return;
    }
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    if (!doc.isObject()) {
        ui->lab_info->setText("Ошибка фомата файла JSON");
        return;
    }

    QJsonObject jsonObject = doc.object();
    this->data->SetData(jsonObject);
    ui->text_json->setPlainText(doc.toJson(QJsonDocument::Indented));
    this->filename = fileName;
    ui->lab_info->setText("Статус: Файл открыт.");
    ui->lab_filename->setText("Файл: " + QFileInfo(this->filename).fileName());
}

void InputData::SaveFile() {
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(ui->text_json->toPlainText().toUtf8(), &error);
    if (error.error != QJsonParseError::NoError) {
        ui->lab_info->setText("Ошибка JSON: " + error.errorString());
        return;
    }

    if (filename.isEmpty()) {
        ui->lab_info->setText("Файл не выбран");
        return;
    };

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        ui->lab_info->setText("Ошибка открытия файла для записи:" + file.errorString());
        return;
    }
    file.write(doc.toJson());
    file.close();

    QJsonObject jsonObject = doc.object();
    this->data->SetData(jsonObject);
    ui->lab_info->setText("Статус: Файл сохранён.");
}

void InputData::ReopenFile() {
    if (filename.isEmpty()) {
        ui->lab_info->setText("Файл не выбран");
        return;
    };

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        ui->lab_info->setText("Невозможно открыть файл");
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    if (!doc.isObject()) {
        ui->lab_info->setText("Ошибка фомата файла JSON");
        return;
    }

    QJsonObject jsonObject = doc.object();
    this->data->SetData(jsonObject);
    ui->text_json->setPlainText(doc.toJson(QJsonDocument::Indented));
    ui->lab_info->setText("Статус: Изменения отменены.");
}

