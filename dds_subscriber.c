#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "dds/dds.h"
#include "dds_msg.h"

#define DOMAIN_ID DDS_DOMAIN_DEFAULT 
#define MAX_SAMPLES 100                 
#define POLL_INTERVAL 20              

int main(int argc, char **argv) {
    dds_entity_t participant, topic, reader;
    dds_return_t rc;
    dds_qos_t *qos;
    DDS_Msg_Module_DDS_Message *msg;
    
    // samples 数组初始化为 NULL
    void *samples[MAX_SAMPLES];       
    dds_sample_info_t infos[MAX_SAMPLES]; 
    
    (void)argc; (void)argv;

    // 1. 创建参与者
    participant = dds_create_participant(DOMAIN_ID, NULL, NULL);
    if (participant < 0)
        DDS_FATAL("创建参与者失败：%s\n", dds_strretcode(-participant));

    // 2. 创建话题
    topic = dds_create_topic(
        participant,
        &DDS_Msg_Module_DDS_Message_desc,
        "CycloneDDS_Demo_Topic",
        NULL, NULL);
    if (topic < 0)
        DDS_FATAL("创建话题失败：%s\n", dds_strretcode(-topic));

    // 3. 创建可靠QoS
    qos = dds_create_qos();
    dds_qset_reliability(qos, DDS_RELIABILITY_RELIABLE, DDS_SECS(10));

    // 4. 创建DataReader
    reader = dds_create_reader(participant, topic, qos, NULL);
    dds_delete_qos(qos);
    if (reader < 0)
        DDS_FATAL("创建DataReader失败：%s\n", dds_strretcode(-reader));

    printf("=== [Subscriber] 等待接收消息（按Ctrl+C退出）...\n");
    fflush(stdout);

    // --- 核心逻辑：循环接收 ---
    while (1) {
        // 每次读取前，确保 samples[0] 是 NULL，这样 DDS 才会分配内存
        samples[0] = NULL; 

        // 调用带 5 个参数的 dds_take
        rc = dds_take(reader, samples, infos, MAX_SAMPLES, MAX_SAMPLES);
        
        if (rc < 0) {
            fprintf(stderr, "读取消息失败：%s\n", dds_strretcode(-rc));
        } 
        else if (rc > 0) {
            // rc 返回的是实际读取到的消息数量
            if (infos[0].valid_data) {
                msg = (DDS_Msg_Module_DDS_Message *)samples[0];
                printf("接收消息：ID=%d, 内容=%s\n", msg->msg_id, msg->msg_content);
                fflush(stdout);
            }
            
            // 必须：将内存还给 DDS
            dds_return_loan(reader, samples, rc);
        }

        dds_sleepfor(DDS_MSECS(POLL_INTERVAL));
    }

    // 清理资源dds_take
    dds_delete(participant);
    return EXIT_SUCCESS;
}