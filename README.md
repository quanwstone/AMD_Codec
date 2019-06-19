## 说明

AMF:Advanced Media Framework 

SDK路径：<https://github.com/GPUOpen-LibrariesAndSDKs/AMF>

该doc文档里有详细说明.



支持MPEG-2，MPEG-4，AVC,HEVC,AAC等编解码.

## 流程

解码

1：创建一个**AMF Context**并初始化

2：利用**AMFFactory::CreateComponent** 方法创建解码器

3：利用**AMFPropertyStorage::SetProperty**设置属性

4：利用 **AMFComponent::Init** 初始化解码器，格式属性设置必须为**AMF_SURFACE_NV12**

5：利用**AMFComponent::SubmitInpu**t填充解码数据

6：利用**AMFComponent::QueryOutput**获取解码数据.



### 编码

1:创建一个AMF Context并初始化

CreateContext(AMFContext** ppContext) = 0;

2：创建一个编码器

CreateComponent(AMFContext* pContext, const wchar_t* id, AMFComponent** ppComponent) = 0;

第一个参数为创建的Context，第二个参数为编码器类型，第三个参数为创建的编码器对象

3：利用**AMFPropertyStorage::SetProperty**设置属性

AMF_VIDEO_ENCODER_USAGE 编码模式 分为四种Transcoding：用于转换和视频编辑. Ultra-low latency：用于视频游戏流. Low Latency：用于视频协作远程桌面. Webcam：用于视频会议,目前使用第一个.

AMF_VIDEO_ENCODER_PROFILE :分为Baseline，Main，High

AMF_VIDEO_ENCODER_PROFILE_LEVEL 

AMF_VIDEO_ENCODER_FRAMESIZE:设置像素宽高.

AMF_VIDEO_ENCODER_ASPECT_RATIO ：像素纵横比默认1：1

AMF_VIDEO_ENCODER_TARGET_BITRATE ：编码目标码率 bps

AMF_VIDEO_ENCODER_PEAK_BITRATE：设置峰值码率

AMF_VIDEO_ENCODER_RATE_CONTROL_METHOD：设置编码方式 CQP，CBR,VBR,

AMF_VIDEO_ENCODER_RATE_CONTROL_SKIP_FRAME_ENABLE:跳过帧用于码率控制,

AMF_VIDEO_ENCODER_MIN_QP ：0-51

AMF_VIDEO_ENCODER_MAX_QP ：0-51

AMF_VIDEO_ENCODER_QP_I ：设置CQP模式下I帧QP,仅仅在CQP模式下有用

AMF_VIDEO_ENCODER_QP_P ：设置CQP模式下P帧QP，仅仅在CQP模式下有用

AMF_VIDEO_ENCODER_QP_B ：设置CQP模式下B帧QP，仅仅在CQP模式下有用

AMF_VIDEO_ENCODER_FRAMERATE ：帧率

AMF_VIDEO_ENCODER_VBV_BUFFER_SIZE ：VBV(video buffer virifier )为了处理编码后各帧大小不一与CBR矛盾的问题，由于CBR规定编码输出为码率恒定，但是IPB帧大小不固定，故需要在encoder后面加入VBV Buffer，可以想象VBV Buffer为一个蓄水池，入口连接编码，出口连接输出,所以encoder在编码一帧时会参考vbv buffer的情况，来计算出当前帧编码多少bit不会造成上溢和下溢。

AMF_VIDEO_ENCODER_INITIAL_VBV_BUFFER_FULLNESS：设置VBV buffer的填充程度

AMF_VIDEO_ENCODER_B_PIC_PATTERN ：设置一个GOP中B帧数量，0表示不使用B帧

AMF_VIDEO_ENCODER_B_REFERENCE_ENABLE :是否启用B帧作为参考帧

AMF_VIDEO_ENCODER_CABAC_ENABLE ：编码算法类型



AMF_VIDEO_ENCODER_INSERT_PPS ：是否需要插入pps

AMF_VIDEO_ENCODER_INSERT_SPS：是否需要插入sps

AMF_VIDEO_ENCODER_FORCE_PICTURE_TYPE ：编码时生成的帧类型

以下图表为默认的参数设置，profile为main

![1560927989991](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\1560927989991.png)

