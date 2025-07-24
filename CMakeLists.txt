cmake_minimum_required(VERSION 3.28)
project(qt_programming_2024)

# 配置C++标准和Qt自动工具
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)

# 查找Qt依赖
find_package(Qt6 COMPONENTS
    Core
    Gui
    Widgets
    REQUIRED)

# 添加所有源文件和资源文件
add_executable(qt_programming_2024 WIN32
    src/main.cpp
    src/MyGame.cpp
    src/MyGame.h
    src/Scenes/Scene.cpp
    src/Scenes/Scene.h
    src/Scenes/BattleScene.cpp
    src/Scenes/BattleScene.h
    src/Items/Characters/Character.cpp
    src/Items/Characters/Character.h
    src/Items/Characters/Link.cpp
    src/Items/Characters/Link.h
    assets/assets.qrc  # 修正：assets与src同级，非Resources下
    src/Items/Maps/Battlefield.cpp
    src/Items/Maps/Battlefield.h
    src/Items/Maps/Map.cpp
    src/Items/Maps/Map.h
    src/Items/Mountable.cpp
    src/Items/Mountable.h
)

# 链接Qt库
target_link_libraries(qt_programming_2024 PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)
