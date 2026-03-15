#!/bin/bash

CLASS_INPUT=$1
SUB_PATH=$2
BASE_CLASS=${3:-QWidget}
NAMESPACE="gs"

if [ -z "$CLASS_INPUT" ]; then
    echo "Использование: ./utils/gen_class.sh <ИмяКласса> <Подпуть> [БазовыйКласс]"
    exit 1
fi

CLASS_NAME="$(tr '[:lower:]' '[:upper:]' <<< ${CLASS_INPUT:0:1})${CLASS_INPUT:1}"
FILE_NAME_BASE="$(tr '[:upper:]' '[:lower:]' <<< "$CLASS_NAME")"

PROJECT_ROOT="$(cd "$(dirname "$0")/.." && pwd)"

CLEAN_PATH=$(echo "$SUB_PATH" | sed 's|^/||; s|/$||')
if [ -n "$CLEAN_PATH" ]; then
    CLEAN_PATH="$CLEAN_PATH/"
fi

# Пути к файлам (Приводим .h и .ui к нижнему регистру для корректной работы AutoUic)
H_REL="include/core/${CLEAN_PATH}${FILE_NAME_BASE}.h"
UI_REL="src/ui/${CLEAN_PATH}${FILE_NAME_BASE}.ui"
# .cpp оставляем с большой буквы, если вам так привычнее для исходников
CPP_REL="src/core/${CLEAN_PATH}${CLASS_NAME}.cpp"

H_FULL="$PROJECT_ROOT/$H_REL"
CPP_FULL="$PROJECT_ROOT/$CPP_REL"
UI_FULL="$PROJECT_ROOT/$UI_REL"
CMAKE_FILE="$PROJECT_ROOT/CMakeLists.txt"

mkdir -p "$(dirname "$H_FULL")"
mkdir -p "$(dirname "$CPP_FULL")"
mkdir -p "$(dirname "$UI_FULL")"

# --- Генерация .h ---
cat <<EOF > "$H_FULL"
#pragma once
#include <$BASE_CLASS>
#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui { class $CLASS_NAME; }
QT_END_NAMESPACE

namespace $NAMESPACE {
class $CLASS_NAME : public $BASE_CLASS {
    Q_OBJECT
public:
    explicit $CLASS_NAME(QWidget *parent = nullptr);
    ~$CLASS_NAME() override;
private:
    std::unique_ptr<Ui::$CLASS_NAME> ui;
};
} // namespace $NAMESPACE
EOF

# --- Генерация .cpp ---
cat <<EOF > "$CPP_FULL"
#include "core/${CLEAN_PATH}${FILE_NAME_BASE}.h"
#include "ui_${FILE_NAME_BASE}.h"

namespace $NAMESPACE {
$CLASS_NAME::$CLASS_NAME(QWidget *parent)
    : $BASE_CLASS(parent)
    , ui(std::make_unique<Ui::$CLASS_NAME>())
{
    ui->setupUi(this);
}

$CLASS_NAME::~$CLASS_NAME() = default;
} // namespace $NAMESPACE
EOF

# --- Генерация .ui (Важно: имя класса внутри XML должно совпадать с C++) ---
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

# --- Обновление CMakeLists.txt ---
if [ -f "$CMAKE_FILE" ]; then
    if grep -q "$CPP_REL" "$CMAKE_FILE"; then
        echo "!! Файлы уже зарегистрированы в CMakeLists.txt"
    else
        # Вставляем пути. Используем $FILE_NAME_BASE для .h и .ui
        # Теперь AutoUic увидит mainwindow.ui и поймет, что делать с ui_mainwindow.h
        sed -i "/add_executable(gletchik-studio/,/)/ s|)|    $CPP_REL\n        $H_REL\n        $UI_REL\n    )|" "$CMAKE_FILE"
        echo ">> Файлы успешно добавлены в CMakeLists.txt"

        # --- Обновление путей поиска AUTOUIC ---
        if [ -n "$CLEAN_PATH" ]; then
            # Убираем лишний слеш в конце для красоты пути
            UI_SEARCH_DIR="\${CMAKE_CURRENT_SOURCE_DIR}/src/ui/${CLEAN_PATH%/}"

            if grep -q "$UI_SEARCH_DIR" "$CMAKE_FILE"; then
                echo "!! Директория UI поиска уже есть в CMakeLists.txt"
            else
                # Вставляем новый путь перед закрывающей скобкой блока CMAKE_AUTOUIC_SEARCH_PATHS
                sed -i "/set(CMAKE_AUTOUIC_SEARCH_PATHS/,/)/ s|)|        \"$UI_SEARCH_DIR\"\n    )|" "$CMAKE_FILE"
                echo ">> Директория $UI_SEARCH_DIR добавлена в CMAKE_AUTOUIC_SEARCH_PATHS"
            fi
        fi
    fi

    echo ">> Созданы файлы:"
    echo "   - $H_REL"
    echo "   - $CPP_REL"
    echo "   - $UI_REL"
else
    echo "!! Ошибка: CMakeLists.txt не найден"
fi