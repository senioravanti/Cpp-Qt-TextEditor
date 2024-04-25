#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QColor>
#include <QSpinBox>
#include <QFontComboBox>
#include <QTextCharFormat>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

public slots:
  void textBold();
  void textItalic();
  void textUnderline();
  void textStrikeOut();

  void on_bulletList(bool);
  void on_numberedList(bool);

  void changeFont(const QFont&);
  void changeTextSize();
  void changeTextColor();

  void on_alignCenter(bool);
  void on_alignJustify(bool);
  void on_alignRight(bool);
  void on_alignLeft(bool);

  void on_increaseIndent(int);
  void on_descreaseIndent(int);

  void on_insertImage();
  void on_editHTML();

  void on_saveFile();
  void on_openFile();

private:
  // Свойства
  Ui::MainWindow *ui;
  QSize iconsSize;
  QSpinBox* spinBox;
  QFontComboBox* fontBox;


  QString defaultTitle, curFileName, mainWndowStyleSheet, commonStyleSheet;

  // Методы
  void modifySVGFile(QString);
  QIcon colourIcons(const QIcon& icon, const QColor& color);
  void colourToggledActions(QAction&);
  QString readQSS(const QString&);


  void mergeFormatOnWordOrSelection(const QTextCharFormat&);

  void on_genList(bool, QTextListFormat::Style);

  void on_genAlign(bool, Qt::Alignment);

  void on_genIndent(int);
};
#endif // MAINWINDOW_H
