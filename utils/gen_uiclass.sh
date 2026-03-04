#!/bin/bash

CLASS_INPUT=$1
SUB_PATH=$2
BASE_CLASS=${3:-QWidget}
NAMESPACE="gs"

if [ -z "$CLASS_INPUT" ]; then
    echo "Использование: ./utils/gen_class.sh <ИмяКласса> <Подпуть> [БазовыйКласс]"
    exit 1
fi

# Имя класса всегда с большой буквы для C++ (MainWindow)
CLASS_NAME="$(tr '[:lower:]' '[:upper:]' <<< ${CLASS_INPUT:0:1})${CLASS_INPUT:1}"

# Имя файла всегда в нижнем регистре (mainwindow)
FILE_NAME_LOWER="$(tr '[:upper:]' '[:lower:]' <<< "$CLASS_NAME")"

PROJECT_ROOT="$(cd "$(dirname "$0")/.." && pwd)"

# Очистка пути
CLEAN_PATH=$(echo "$SUB_PATH" | sed 's|^/||; s|/$||')
if [ -n "$CLEAN_PATH" ]; then
    CLEAN_PATH="$CLEAN_PATH/"
fi

# Пути для файлов (H теперь в нижнем регистре)
H_REL="include/core/${CLEAN_PATH}${FILE_NAME_LOWER}.h"
CPP_REL="src/core/${CLEAN_PATH}${CLASS_NAME}.cpp"
UI_REL="src/ui/${CLEAN_PATH}${CLASS_NAME}.ui"

H_FULL="$PROJECT_ROOT/$H_REL"
CPP_FULL="$PROJECT_ROOT/$CPP_REL"
UI_FULL="$PROJECT_ROOT/$UI_REL"
CMAKE_FILE="$PROJECT_ROOT/CMakeLists.txt"

mkdir -p "$(dirname "$H_FULL")"
mkdir -p "$(dirname "$CPP_FULL")"
mkdir -p "$(dirname "$UI_FULL")"

# --- Генерация .h (Имя файла строчными, класс с большой) ---
cat <<EOF > "$H_FULL"
#pragma once
#include <$BASE_CLASS>

QT_BEGIN_NAMESPACE
namespace Ui { class $CLASS_NAME; }
QT_END_NAMESPACE

namespace $NAMESPACE {
class $CLASS_NAME : public $BASE_CLASS {
    Q_OBJECT
public:
    explicit $CLASS_NAME(QWidget *parent = nullptr);
    ~$CLASS_NAME();
private:
    Ui::$CLASS_NAME *ui;
};
} // namespace $NAMESPACE
EOF

# --- Генерация .cpp (Инклюдит строчный .h) ---
cat <<EOF > "$CPP_FULL"
#include "core/${CLEAN_PATH}${FILE_NAME_LOWER}.h"
#include "ui_${CLASS_NAME}.h"

namespace $NAMESPACE {
$CLASS_NAME::$CLASS_NAME(QWidget *parent)
    : $BASE_CLASS(parent)
    , ui(new Ui::$CLASS_NAME)
{
    ui->setupUi(this);
}

$CLASS_NAME::~$CLASS_NAME() {
    delete ui;
}
} // namespace $NAMESPACE
EOF

# --- Генерация .ui ---
cat <<EOF > "$UI_FULL"
<?xml version="1.0" encoding="UTF-8"?>
<ui version="4.0">
 <class>$CLASS_NAME</class>
 <widget class="$BASE_CLASS" name="$CLASS_NAME">
  <property name="windowTitle">
   <string>$CLASS_NAME</string>
  </property>
 </widget>
 <resources/>
 <connections/>
</ui>
EOF

# --- Вставка в CMakeLists.txt ---
if [ -f "$CMAKE_FILE" ]; then
    sed -i "/add_executable(gletchik-studio/,/)/ s|)|    $CPP_REL\n        $H_REL\n        $UI_REL\n    )|" "$CMAKE_FILE"
    echo ">> Созданы файлы:"
    echo "   - $H_REL"
    echo "   - $CPP_REL"
    echo "   - $UI_REL"
else
    echo "!! Ошибка: CMakeLists.txt не найден"
fi