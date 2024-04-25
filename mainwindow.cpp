
#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QPixmap>
#include <QImage>
#include <QPainter>
#include <QFontDialog>
#include <QPushButton>
#include <QColorDialog>

#include <QFile>

#include <QByteArray>
#include <QTextStream>

#include <QTextCursor>
#include <QTextBlockFormat>
#include <QTextList>
#include <QTextListFormat>

#include <QDialog>
#include <QPlainTextEdit>
#include <QVBoxLayout>

#include <QFileDialog>
#include <QImage>
#include <QImageReader>

#include <QInputDialog>

#include <QFile>
#include <QByteArray>
#include <QTextStream>
#include <QFileInfo>

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  this->defaultTitle = "Кустарный текстовый процессор";
  this->curFileName = "Без имени";

  this->setWindowTitle(defaultTitle + " | " + curFileName);
  this->resize(854, 427);

  this->iconsSize = QSize(20, 20);
  // --- Раскрашиваем интерфейс

  QString icons = "#547BAB", text = "#4C566A", border = "#BEC7D8";
  // QSpinBox#spinBox & QFontComboBox#fontBox надо сделать маржины 2.5

  QColor iconsColor = QColor::fromString(icons);
  this->ui->toolBar->setIconSize(iconsSize);

  // Настройка элементов управления

  fontBox = new QFontComboBox();
  fontBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

  spinBox = new QSpinBox();
  spinBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
  spinBox->setMinimum(0); spinBox->setMaximum(144);
  spinBox->setValue(14);


  this->ui->toolBar->insertWidget(ui->toolBar->actions().at(ui->toolBar->actions().indexOf(ui->actionTextColor)), fontBox);
  this->ui->toolBar->insertWidget(ui->toolBar->actions().at(ui->toolBar->actions().indexOf(ui->actionTextColor)), spinBox);

  // Раскрашиваю иконки

  for (QAction* action : this->ui->toolBar->actions()) {
    action->setIcon(colourIcons(action->icon(), iconsColor));
  }
  this->ui->actionTextColor->setIcon(colourIcons(ui->actionTextColor->icon(), QColor::fromString(text)));

  // Настраиваю таблицы стилей

  QString boxButtonBorderRadius = "3px", boxBorderRadius = "5px", boxButtonMargin = "3px", boxButtonWidth = "16px";
  QString boxButtonColor = "#3B5472", toolBarBackground = "#D8DEE9";

  this->commonStyleSheet = "\
    QWidget {\
      border: none;\
    }\
    QTextEdit, QPlainTextEdit {\
      background: #ECEFF4;\
      color: " + text + ";\
    }";

  this->mainWndowStyleSheet = "    QToolBar {\
      background: #D8DEE9;\
      spacing: 9px;\
      padding-top: 5px; padding-bottom: 5px;\
      padding-right: 9px; padding-left: 9px;\
    }\
    QToolBar::separator {\
      background: " + border + ";\
      width: 2px;\
      margin-left: 5px; margin-right: 5px;\
    }\
    QSpinBox {\
      background: " + border + "; color: " + text + ";\
      border-radius: " + boxBorderRadius + ";\
      padding-left: 5px;\
      margin-left: 2.5px; margin-right: 2.5px;\
    }\
    QSpinBox::up-button, QSpinBox::down-button {\
      background: " + icons + ";\
      width: 16px; height: 10px;\
      margin-right: " + boxButtonMargin + ";\
    }\
    QSpinBox::up-button { \
      image: url(:/icons/arrow-up.svg);\
      margin-top: " + boxButtonMargin + ";\
      border-top-left-radius: " + boxButtonBorderRadius + "; border-top-right-radius: " + boxButtonBorderRadius + ";\
    }\
    QSpinBox::down-button {\
      image: url(:/icons/arrow-down.svg);\
      margin-bottom: " + boxButtonMargin + ";\
      border-bottom-left-radius: " + boxButtonBorderRadius + "; border-bottom-right-radius: " + boxButtonBorderRadius + ";\
    }\
    QFontComboBox, QFontComboBox QAbstractItemView {\
      background: " + border + "; color: " + text + ";\
      border-radius: " + boxBorderRadius + ";\
      padding-left: 5px;\
      margin-left: 2.5px; margin-right: 2.5px;\
    }\
    QFontComboBox {\
      \
    }\
    QFontComboBox::down-arrow {\
      image: url(:/icons/arrow-down.svg);\
      subcontrol-position: bottom center;\
    }\
    QFontComboBox::drop-down {\
      background: " + icons + ";\
      subcontrol-position: center right;\
      margin-top: " + boxButtonMargin + "; margin-bottom: " + boxButtonMargin + "; margin-right: " + boxButtonMargin + ";\
      width: 16px;\
      border-radius: " + boxButtonBorderRadius + "\
    }";

  this->setStyleSheet(this->commonStyleSheet + this->mainWndowStyleSheet);

  // Сигналы и слоты
  connect(this->fontBox, &QFontComboBox::currentFontChanged, this, &MainWindow::changeFont);
  connect(this->ui->actionTextColor, &QAction::triggered, this, &MainWindow::changeTextColor);
  connect(spinBox, &QSpinBox::valueChanged, this, &MainWindow::changeTextSize);

  connect(ui->actionBold, &QAction::toggled, this, &MainWindow::textBold);
  connect(ui->actionItalic, &QAction::toggled, this, &MainWindow::textItalic);
  connect(ui->actionUnderline, &QAction::toggled, this, &MainWindow::textUnderline);
  connect(ui->actionStrikeOut, &QAction::toggled, this, &MainWindow::textStrikeOut);
  // Списки
  connect(ui->actionBullet, &QAction::toggled, this, &MainWindow::on_bulletList);
  connect(ui->actionOrdered, &QAction::toggled, this, &MainWindow::on_numberedList);
  // Выравнивание текста
  connect(ui->actionLeft, &QAction::toggled, this, &MainWindow::on_alignLeft);
  connect(ui->actionRight, &QAction::toggled, this, &MainWindow::on_alignRight);
  connect(ui->actionCenter, &QAction::toggled, this, &MainWindow::on_alignCenter);
  connect(ui->actionJustify, &QAction::toggled, this, &MainWindow::on_alignJustify);
  // Отступы
  connect(ui->actionIncreaseIndent, &QAction::triggered, this, &MainWindow::on_increaseIndent);
  connect(ui->actionDecreseIndent, &QAction::triggered, this, &MainWindow::on_descreaseIndent);
  //
  connect(ui->actionEditHTML, &QAction::triggered, this, &MainWindow::on_editHTML);
  // Вставить ссылку или изображение
  connect(ui->actionImage, &QAction::triggered, this, &MainWindow::on_insertImage);
  // Открытие/закрытие файлов
  connect(ui->actionSaveDocument, &QAction::triggered, this, &MainWindow::on_saveFile);
  connect(ui->actionOpenDocument, &QAction::triggered, this, &MainWindow::on_openFile);
}

QIcon MainWindow::colourIcons(const QIcon& icon, const QColor& color) {
  QPixmap img = icon.pixmap(iconsSize);
  QPainter painter = QPainter(&img);
  painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
  painter.fillRect(img.rect(), color);
  return QIcon(img);
}

void MainWindow::colourToggledActions(QAction& action) {

}

void MainWindow::changeFont(const QFont& curFont) {
  this->ui->textEdit->setFont(curFont);
}

void MainWindow::changeTextColor() {
  QColor color = QColorDialog::getColor(Qt::white, this);
  if (color.isValid()) {
    ui->textEdit->setTextColor(color);

    ui->actionTextColor->setIcon(colourIcons(ui->actionTextColor->icon(), color));
  }
}

void MainWindow::changeTextSize() {
  QFont curFont = this->fontBox->currentFont();
  curFont.setPointSize(spinBox->value());
  this->ui->textEdit->setFont(curFont);
}

QString MainWindow::readQSS(const QString& path) {
  QFile file(path);
  if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QTextStream in(&file);
    return in.readAll();
  }
  return "";
}

// Стили текста
void MainWindow::mergeFormatOnWordOrSelection(const QTextCharFormat& fmt) {
  QTextCursor cursor = ui->textEdit->textCursor();
  if (!cursor.hasSelection()) {
    cursor.select(QTextCursor::WordUnderCursor);
  }
  cursor.mergeCharFormat(fmt);
  ui->textEdit->mergeCurrentCharFormat(fmt);
}

void MainWindow::textBold() {
  QTextCharFormat fmt;
  fmt.setFontWeight(ui->actionBold->isChecked() ? QFont::Bold : QFont::Normal);
  mergeFormatOnWordOrSelection(fmt);
}

void MainWindow::textItalic() {
  QTextCharFormat fmt;
  fmt.setFontItalic(ui->actionItalic->isChecked());
  mergeFormatOnWordOrSelection(fmt);
}

void MainWindow::textUnderline() {
  QTextCharFormat fmt;
  fmt.setFontUnderline(ui->actionUnderline->isChecked());
  mergeFormatOnWordOrSelection(fmt);
}

void MainWindow::textStrikeOut() {
  QTextCharFormat fmt;
  fmt.setFontStrikeOut(ui->actionStrikeOut->isChecked());
  mergeFormatOnWordOrSelection(fmt);
}

void MainWindow::on_genList(bool checked, QTextListFormat::Style style){
  QTextCursor cursor = ui->textEdit->textCursor();
  cursor.beginEditBlock();
  // Если мы убираем список
  if (!checked) {
    QTextBlockFormat obfmt = cursor.blockFormat(), bfmt;
    // Из старого форматирования оставляем только отступ.
    bfmt.setIndent(obfmt.indent());
    cursor.setBlockFormat(bfmt);
  }
  // если добавляем список
  else {
    QTextListFormat lfmt;
    // Такая ситуация может возникнуть если мы хотим поменять тип списка.
    if (cursor.currentList()) {
      lfmt = cursor.currentList()->format();
    }
    lfmt.setStyle(style);
    cursor.createList(lfmt);
  }
  cursor.endEditBlock();
}

void MainWindow::on_bulletList(bool checked) {
  on_genList(checked, QTextListFormat::ListDisc);
}

void MainWindow::on_numberedList(bool checked) {
  on_genList(checked, QTextListFormat::ListDecimal);

}

// Выравнивание текста
void MainWindow::on_genAlign(bool checked, Qt::Alignment alignment) {
  QTextCursor cursor = ui->textEdit->textCursor();
  cursor.beginEditBlock();
  QTextBlockFormat bfmt = cursor.blockFormat();
  bfmt.setIndent(0);
  if (!checked) {
    bfmt.setAlignment(Qt::AlignLeft);
  } else {
    bfmt.setAlignment(alignment);
  }
  cursor.setBlockFormat(bfmt);
  cursor.endEditBlock();
}
void MainWindow::on_alignLeft(bool checked) {
  on_genAlign(checked, Qt::AlignLeft);
}

void MainWindow::on_alignRight(bool checked) {
    on_genAlign(checked, Qt::AlignRight);
}

void MainWindow::on_alignCenter(bool checked) {
  on_genAlign(checked, Qt::AlignCenter);
}

void MainWindow::on_alignJustify(bool checked) {
  on_genAlign(checked, Qt::AlignJustify);
}

// Отступы
void MainWindow::on_genIndent(int delta) {
  QTextCursor cursor = ui->textEdit->textCursor();
  cursor.beginEditBlock();
  QTextBlockFormat bfmt = cursor.blockFormat();
  int ind = bfmt.indent();
  if (ind + delta >= 0) {
    bfmt.setIndent(ind + delta);
  }
  cursor.setBlockFormat(bfmt);
  cursor.endEditBlock();

}

void MainWindow::on_increaseIndent(int delta) {
  on_genIndent(+1);
}

void MainWindow::on_descreaseIndent(int delta) {
  on_genIndent(-1);
}

void MainWindow::on_editHTML() {
  QDialog* dialog = new QDialog(this);
  dialog->setStyleSheet(this->commonStyleSheet);
  QPlainTextEdit* editor = new QPlainTextEdit();
  editor->setPlainText(ui->textEdit->toHtml());
  QVBoxLayout* layout = new QVBoxLayout(dialog);
  layout->setContentsMargins(0, 0, 0, 0);

  layout->addWidget(editor);
  dialog->setWindowTitle("Исходный HTML документ");
  dialog->setMinimumWidth (400);
  dialog->setMinimumHeight(600);
  dialog->exec();

  ui->textEdit->setHtml(editor->toPlainText());

  delete dialog;
}

void MainWindow::on_insertImage() {
  QString file = QFileDialog::getOpenFileName(this, "Выберите изображение", "", "JPEG (*.jpg);; GIF (*.gif);; PNG (*.png);; BMP (*.bmp);; All (*)");

  QImage image;
  image = QImageReader(file).read();
  if (!image.isNull()) {
    QTextCursor cursor = ui->textEdit->textCursor();
    cursor.insertImage(image);
  }

}

void MainWindow::on_saveFile() {
  QString fileName = QFileDialog::getSaveFileName(this,
    "Сохранить как", "", "MD (*.md);; HTML (*.html);; TXT (*.txt);; All (*)"
  );
  if (fileName.isEmpty()) {
    qDebug() << "Не выбран файл для записи"; return;
  }

  QFile file(fileName);

  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    qDebug() << "Ошибка при открытии файла"; return;
  }

  QTextStream out(&file);
  QString ext = QFileInfo(file).suffix().toLower();

  if (ext == "md") {
    out << ui->textEdit->toMarkdown();
  } else if (ext == "html") {
    out << ui->textEdit->toHtml();
  } else {
    out << ui->textEdit->toPlainText();
  }

  this->setWindowTitle(this->defaultTitle + " | " + QFileInfo(file).fileName());

  file.close();
}

void MainWindow::on_openFile() {
  QString fileName = QFileDialog::getOpenFileName(this,
    "Выберите файл", "", "MD (*.md);; HTML (*.html);; TXT (*.txt);;     All (*)");
  if (fileName.isEmpty()) {
    qDebug() << "Не выбран файл для чтения"; return;
  }

  QFile file(fileName);

  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qDebug() << "Ошибка при открытии файла"; return;
  }

  QByteArray data = file.readAll();
  this->setWindowTitle(this->defaultTitle + " | " + QFileInfo(file).fileName());
  file.close();

  QTextStream in(&data); QString line;

  ui->textEdit->setPlainText("");

  while (!in.atEnd()){
    line = in.readLine();
    ui->textEdit->setText(ui->textEdit->toPlainText() + line + "\n");
  }

}

MainWindow::~MainWindow() { delete ui; }

