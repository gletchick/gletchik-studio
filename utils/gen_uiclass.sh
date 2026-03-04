#!/bin/bash

# Параметры
CLASS_NAME=$1
SUB_PATH=$2
BASE_CLASS=${3:-QWidget}
NAMESPACE="gs"

if [ -z "$CLASS_NAME" ]; then
    echo "Использование: ./utils/gen_class.sh <ИмяКласса> <Подпуть> [БазовыйКласс]"
    exit 1
fi

# Корень проекта (на уровень выше от папки utils)
PROJECT_ROOT="$(cd "$(dirname "$0")/.." && pwd)"

# Обработка пути
if [ "$SUB_PATH" == "/" ] || [ -z "$SUB_PATH" ]; then
    CLEAN_PATH=""
else
    CLEAN_PATH="${SUB_PATH%/}/"
fi

# Пути к файлам (относительно корня проекта для CMake)
H_REL="include/core/${CLEAN_PATH}${CLASS_NAME,,}.h"
CPP_REL="src/core/${CLEAN_PATH}${CLASS_NAME}.cpp"
UI_REL="src/ui/${CLEAN_PATH}${CLASS_NAME}.ui"

# Полные пути для создания файлов системой
H_FULL="$PROJECT_ROOT/$H_REL"
CPP_FULL="$PROJECT_ROOT/$CPP_REL"
UI_FULL="$PROJECT_ROOT/$UI_REL"
CMAKE_FILE="$PROJECT_ROOT/CMakeLists.txt"

mkdir -p "$(dirname "$H_FULL")"
mkdir -p "$(dirname "$CPP_FULL")"
mkdir -p "$(dirname "$UI_FULL")"

# 1. Генерируем Header (.h) с pragma once
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

# 2. Генерируем Implementation (.cpp)
cat <<EOF > "$CPP_FULL"
#include "core/${CLEAN_PATH}${CLASS_NAME,,}.h"
#include "ui_$(basename ${CLASS_NAME,,}).h"

namespace $NAMESPACE {

$CLASS_NAME::$CLASS_NAME(QWidget *parent)
    : $BASE_CLASS(parent)
    , ui(new Ui::$CLASS_NAME) {
    ui->setupUi(this);
}

$CLASS_NAME::~$CLASS_NAME() {
    delete ui;
}

} // namespace $NAMESPACE
EOF

# 3. Генерируем UI файл
cat <<EOF > "$UI_FULL"
<?xml version="1.0" encoding="UTF-8"?>
<ui version="4.0">
 <class>$CLASS_NAME</class>
 <widget name="$BASE_CLASS" class="$CLASS_NAME">
  <property name="windowTitle">
   <string>$CLASS_NAME</string>
  </property>
 </widget>
 <resources/>
 <connections/>
</ui>
EOF

# 4. Обновление CMakeLists.txt
if [ -f "$CMAKE_FILE" ]; then
    echo ">> Обновляю CMakeLists.txt..."
    {
        echo ""
        echo "list(APPEND SOURCES \"src/core/${CLEAN_PATH}${CLASS_NAME}.cpp\")"
        echo "list(APPEND HEADERS \"include/core/${CLEAN_PATH}${CLASS_NAME,,}.h\")"
        echo "list(APPEND FORMS \"src/ui/${CLEAN_PATH}${CLASS_NAME}.ui\")"
    } >> "$CMAKE_FILE"
    echo ">> Готово. Файлы созданы и прописаны в CMake."
else
    echo "!! Предупреждение: CMakeLists.txt не найден в $PROJECT_ROOT"
fi

# 5. Интеграция с Git
if [ -d "$PROJECT_ROOT/.git" ]; then
    echo ">> Индексирую файлы в Git..."
    git -C "$PROJECT_ROOT" add "$H_REL" "$CPP_REL" "$UI_REL" "$CMAKE_FILE"
    echo ">> Готово! Файлы теперь отслеживаются (staged)."
else
    echo "?? Git-репозиторий не найден, пропускаю git add."
fi