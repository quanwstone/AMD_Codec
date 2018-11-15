#include<stdio.h>
#include"common/AMFFactory.h"
#include"include/components/VideoDecoderUVD.h"

extern "C"
{
#include"libavformat/avformat.h"

}
class CDecode {
public:
	bool Init(int iW, int iH);
	bool Decode(unsigned char *data, int iLen, FILE *f);
	void WritePlane(amf::AMFPlane *plane, FILE *f);

	amf::AMFContextPtr      context;
	amf::AMFComponentPtr    decoder;
	amf::AMFBufferPtr		pictureBuffer;
	int		m_iFrames;
	
};
void CDecode::WritePlane(amf::AMFPlane *plane, FILE *f)
{
	// write NV12 surface removing offsets and alignments
	amf_uint8 *data = reinterpret_cast<amf_uint8*>(plane->GetNative());
	amf_int32 offsetX = plane->GetOffsetX();
	amf_int32 offsetY = plane->GetOffsetY();
	amf_int32 pixelSize = plane->GetPixelSizeInBytes();
	amf_int32 height = plane->GetHeight();
	amf_int32 width = plane->GetWidth();
	amf_int32 pitchH = plane->GetHPitch();

	for (amf_int32 y = 0; y < height; y++)
	{
		amf_uint8 *line = data + (y + offsetY) * pitchH;
		fwrite(reinterpret_cast<char*>(line) + offsetX * pixelSize, 1,pixelSize * width,f);
	}
}

bool CDecode::Decode(unsigned char *data, int iLen,FILE *f)
{
	amf::AMFDataPtr dTemp = NULL;
	AMF_RESULT re = AMF_OK;

	if (!pictureBuffer)
	{
		re = context->AllocBuffer(amf::AMF_MEMORY_HOST, iLen, &pictureBuffer);
		if (re != AMF_OK)
		{
			printf("AllocBuffer Failed\n");
		}
	}
	memcpy(pictureBuffer->GetNative(), data, iLen);
	
	dTemp = pictureBuffer.Detach();

	re = decoder->SubmitInput(dTemp);
	if (re == AMF_OK)
	{
		re = decoder->QueryOutput(&dTemp);
		if (re == AMF_OK)
		{
			if (dTemp)
			{
				re = dTemp->Convert(amf::AMF_MEMORY_HOST); // convert to system memory

				amf::AMFSurfacePtr surface(dTemp); // query for surface interface

				WritePlane(surface->GetPlane(amf::AMF_PLANE_Y), f);
				WritePlane(surface->GetPlane(amf::AMF_PLANE_UV), f);
				m_iFrames++;
			}
		}
	}
	return true;
}
bool CDecode::Init(int iW,int iH)
{
	AMF_RESULT re = AMF_OK;

	re = g_AMFFactory.Init();
	if (re != AMF_OK)
	{
		printf("Init Failed\n");
	}

	re = g_AMFFactory.GetFactory()->CreateContext(&context);
	if (re != AMF_OK)
	{
		printf("CreateContext Failed\n");
	}

	re = g_AMFFactory.GetFactory()->CreateComponent(context, AMFVideoDecoderUVD_H264_AVC, &decoder);
	if (re != AMF_OK)
	{
		printf("CreateComponent Failed\n");
	}
	
	re = decoder->SetProperty(AMF_VIDEO_DECODER_REORDER_MODE, AMF_VIDEO_DECODER_MODE_LOW_LATENCY);//如果不设置的话前几帧数据解析不会返回
	if (re != AMF_OK)
	{
		printf("SetProperty Failed\n");
	}

	re = decoder->Init(amf::AMF_SURFACE_NV12, iW, iH);
	if (re != AMF_OK)
	{
		printf("decode Init Failed\n");
	}
	m_iFrames = 0;
	pictureBuffer = nullptr;

	return true;
}
int main(int argc, char *argv[])
{
	printf("Begin\n");

	getchar();

	printf("End\n");
	
	av_register_all();

	AVFormatContext *pFormatContext = nullptr;
	FILE	*pFile = NULL;

	errno_t e = fopen_s(&pFile, "Test.yuv", "wb+");
	if (e != 0)
	{
		printf("fopen_s Failed\n");
	}
	int ire = avformat_open_input(&pFormatContext, "Test.264", NULL, NULL);
	if (ire != 0)
	{
		printf("avformat_open_input Failed\n");
	}

	int iIndexVideo = 0;

	ire = avformat_find_stream_info(pFormatContext, NULL);
	if (ire < 0)
	{
		printf("avformat_find_stream_info Failed\n");
	}
	AVCodec *pCodec = NULL;
	
	iIndexVideo = av_find_best_stream(pFormatContext, AVMEDIA_TYPE_VIDEO, 0, 0, &pCodec, 0);
	if (iIndexVideo < 0)
	{
		printf("av_find_best_stream Failed\n");
	}
	CDecode dlg;

	dlg.Init(pFormatContext->streams[iIndexVideo]->codec->width, pFormatContext->streams[iIndexVideo]->codec->height);

	AVPacket *pak = NULL;

	pak = av_packet_alloc();

	int iFrames = 0;
	while (av_read_frame(pFormatContext, pak) == 0)
	{
		if (pak->stream_index == iIndexVideo)
		{
			iFrames++;
			dlg.Decode((unsigned char *)pak->data, pak->size, pFile);
		}
		av_packet_unref(pak);
	}

	av_packet_free(&pak);

	fclose(pFile);
	
	printf("Frames=%d,All=%d\n",dlg.m_iFrames,iFrames);

	return 0;
}