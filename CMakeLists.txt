cmake_minimum_required(VERSION 3.10)
project(dds_publisher)

# 自动获取当前用户主目录，拼接CycloneDDS安装路径（无需改任何内容）
set(CYCLONEDDS_INSTALL_DIR "/home/jn/software/cyclonedds_install")

set(CMAKE_PREFIX_PATH ${CYCLONEDDS_INSTALL_DIR} ${CMAKE_PREFIX_PATH})

find_package(CycloneDDS REQUIRED)

idlc_generate(TARGET dds_msg_lib FILES "dds_msg.idl" WARNINGS no-implicit-extensibility)
add_executable(dds_publisher dds_publisher.c)
target_link_libraries(dds_publisher CycloneDDS::ddsc dds_msg_lib)
add_executable(dds_subscriber dds_subscriber.c)
target_link_libraries(dds_subscriber CycloneDDS::ddsc dds_msg_lib)
