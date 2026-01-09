#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "dds/dds.h"
#include "dds_msg.h"  // 由IDL生成的头文件

#define DOMAIN_ID 0          // DDS域ID（通信双方需一致）
#define MAX_MSG_LEN 128      // 匹配IDL中string<128>的长度

int main(int argc, char *argv[]) {
    // 1. 创建DDS参与者（Domain Participant）
    dds_entity_t participant = dds_create_participant(DOMAIN_ID, NULL, NULL);
    if (participant < 0) {
        fprintf(stderr, "创建参与者失败！\n");
        return -1;
    }

    // 2. 创建话题（Topic）
    // 关键修改1：类型描述符加模块前缀 DDS_Msg_Module_
    dds_entity_t topic = dds_create_topic(
        participant,
        &DDS_Msg_Module_DDS_Message_desc,  // 带模块前缀的类型描述符
        "CycloneDDS_Demo_Topic",           // 话题名（通信双方需一致）
        NULL, NULL);
    if (topic < 0) {
        fprintf(stderr, "创建话题失败！\n");
        dds_delete(participant);
        return -1;
    }

    // 3. 创建发布者（Publisher）
    dds_entity_t publisher = dds_create_publisher(participant, NULL, NULL);
    if (publisher < 0) {
        fprintf(stderr, "创建发布者失败！\n");
        dds_delete(topic);
        dds_delete(participant);
        return -1;
    }

    // 4. 创建数据写入者（DataWriter）
    dds_entity_t writer = dds_create_writer(publisher, topic, NULL, NULL);
    if (writer < 0) {
        fprintf(stderr, "创建数据写入者失败！\n");
        dds_delete(publisher);
        dds_delete(topic);
        dds_delete(participant);
        return -1;
    }

    // 5. 循环发布消息
    // 关键修改2：结构体名加模块前缀 DDS_Msg_Module_
    DDS_Msg_Module_DDS_Message msg;
    msg.msg_id = 0;  // 字段名不变，正常赋值
    printf("发布者已启动，开始发送消息（按Ctrl+C退出）...\n");
    while (1) {
        // 构造消息内容（MAX_MSG_LEN匹配IDL中string<128>）
        snprintf(msg.msg_content, MAX_MSG_LEN, "Hello CycloneDDS! ID: %ld", msg.msg_id);
        
        // 发布消息
        dds_write(writer, &msg);
        printf("发送消息：%s\n", msg.msg_content);
        
        msg.msg_id++;
        dds_sleepfor(DDS_MSECS(5));
    }

    // 6. 清理资源（实际中Ctrl+C退出，此处仅为示例）
    dds_delete(writer);
    dds_delete(publisher);
    dds_delete(topic);
    dds_delete(participant);
    return 0;
}
