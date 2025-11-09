//
// Created by bismarck on 12/8/22.
//

#ifndef MSG_SERIALIZE_LISTENER_H
#define MSG_SERIALIZE_LISTENER_H

#include <string>
#include <cstring>

#include "callback_manager.h"
#include "header_process.h"
#include "check.h"

class Listener {
private:
    CallbackManager callbackManager;
    std::string txBuffer{100};

public:
    template<typename T>
    bool registerCallback(int id, std::function<void(T)> userCallback) {
        return callbackManager.registerCallback(id, [userCallback](const char* data) {
            T t;
            memcpy(&t, data, sizeof(T));
            userCallback(t);
        });
    }

    bool append(const char c) {
        txBuffer.push_back(c);// 将字符 c 添加到缓冲区
        //std::cout<<"Hello!"<<std::endl;
        int eraseSize = 0; // 记录要删除的缓冲区大小    
         // 遍历缓冲区，尝试解析数据包
        for (int i = 0; i < (long)txBuffer.size()-headerSize-checkSize-tailSize; i++) {
            int id, size;
            // 尝试从头部解析消息 ID 和大小
            if (parseFromHeader((uint8_t*)txBuffer.data()+i, (int)txBuffer.size()-i, id, size)) 
            //  size = (int)data[1];
            //  id = (int)data[2];
            {
                size -= headerSize + checkSize + tailSize;
                // 检查缓冲区中是否有完整的数据包
                if (txBuffer.size() - i >= size + headerSize + checkSize + tailSize) {
                    //std::cout<<"Head Right!"<<" "<<id<<" "<<size<<std::endl;
                    // 校验数据包的校验码是否正确
                    int check_flag = 0;
                    check_flag = (checkFunc(((uint8_t*)txBuffer.data() + i), size+headerSize) ==
                            ((uint8_t*)txBuffer.data())[size+headerSize+i]);
                     // 处理特定的消息 (size == 0x18, id == 0x12)
                    if ((size == 0x18) and (id == 0x12)) {
                        //std::cout<<std::hex<<"size == 0x1c and ..."<<std::endl;
                        // 调用与 ID 相关联的回调函数
                        callbackManager[id](txBuffer.data()+i+headerSize);
                        txBuffer.erase(0, i+size);
                        return true;
                    } else {
                        eraseSize = i+1;
                    }
                }
            }
        }
        txBuffer.erase(0, eraseSize);
        return false;
    }
};

#endif //MSG_SERIALIZE_LISTENER_H
