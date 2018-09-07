#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define __STDC_CONSTANT_MACROS
#ifdef __cplusplus
extern "C"
{
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/opt.h>
#include <libavutil/pixdesc.h>
#ifdef __cplusplus
};
#endif

//'1': Use H.264 Bitstream Filter 
#define USE_H264BSF 1

AVFormatContext* ifmt_ctx = NULL;
AVFormatContext* ofmt_ctx_video = NULL;
void end_free()
{
	// Close an opened input AVFormatContext.Free it and all its contents and set *s to NULL.
	avformat_close_input(&ifmt_ctx);


	// Close the resource accessed by the AVIOContext s and free it.
	if (NULL != ofmt_ctx_video)
	avio_close(ofmt_ctx_video->pb);


	// Free an AVFormatContext and all its streams.
	avformat_free_context(ofmt_ctx_video);
}



int main(int argc, char* argv[])
{
	clock_t begin_all, end_all;
    double cost_all;
    begin_all = clock();
// Variables Definition

	const char* input_filepath = NULL; // File path of input stream

	int idx_video = -1; // index of input video stream


	AVStream* pVideoStream = NULL; // ... of input video stream
	AVCodecContext* pVideoCodecCtx = NULL; // 
	AVCodec* pVideoCodec = NULL; //


	//AVFormatContext* ofmt_ctx_video = NULL; // ... of output video stream
	AVStream* pVideoStream_out = NULL; // H264
	const char* output_filepath_video = NULL; // 


	AVPacket ipkt = { 0 }; // 
	// Variables Definition end


	// Initialize libavformat and register all the muxers, demuxers and *protocols
	av_register_all();


	// Do global initialization of network components
	avformat_network_init();


	/* INPUT STREAM */


	//input_filepath = "rtmp://live.hkstv.hk.lxdns.com/live/hks";//sample_720p
	input_filepath = "sample_720p.mp4";

	// Open an input stream and read the header. The codecs are not opened.
	if (avformat_open_input(&ifmt_ctx, input_filepath, NULL, NULL) != 0)
	{
		printf("error: avformat_open_input\n");
		end_free();
	}


	// Read packets of a media file to get stream information.
	if (avformat_find_stream_info(ifmt_ctx, NULL) < 0)
	{
		printf("error: avformat_find_stream_info\n");
		end_free();
	}


	// Find the "best" stream in the file.
	for (int i = 0; i < ifmt_ctx->nb_streams; i++)
	{
	// video stream
		if (AVMEDIA_TYPE_VIDEO == ifmt_ctx->streams[i]->codec->codec_type)//else 
		{
			idx_video = i;
		}
		else
		{
			break;
		}	
	}

	if (idx_video < 0 )
	{
		printf("error: can not find any  video stream\n");
		end_free();
	}


	// input video stream
	pVideoStream = ifmt_ctx->streams[idx_video];
	pVideoCodecCtx = pVideoStream->codec;
	pVideoCodec = avcodec_find_decoder(pVideoCodecCtx->codec_id);

	// Initialize the AVCodecContext to use the given AVCodec.
	if (avcodec_open2(pVideoCodecCtx, pVideoCodec, NULL) < 0)
	{
		printf("error: avcodec_open2, input video\n");
		end_free();
	}


	/* OUTPUT STREAM */
	output_filepath_video = "output.h264";

	// Allocate an AVFormatContext for an output format.
	avformat_alloc_output_context2(&ofmt_ctx_video, NULL, "h264", output_filepath_video); // video, h264
	if (NULL == ofmt_ctx_video)
	{
		printf("error: avformat_alloc_output_context2, output video\n");
		end_free();
	}


	// Add a new stream to a media file.
	pVideoStream_out = avformat_new_stream(ofmt_ctx_video, NULL);
	if (NULL == pVideoStream_out)
	{
		printf("error: avformat_new_stream, output video\n");
		end_free();
	}


	// Copy the settings of the source AVCodecContext into the destination AVCodecContext.
	if (0 != avcodec_copy_context(pVideoStream_out->codec, pVideoStream->codec))
	{
		printf("error: avcodec_copy_context, output video\n");
		end_free();
	}


	// Create and initialize a AVIOContext for accessing the resource.
	avio_open(&ofmt_ctx_video->pb, output_filepath_video, AVIO_FLAG_WRITE);
	if (NULL == ofmt_ctx_video->pb)
	{
		printf("error: avio_open, output video\n");
		end_free();
	}


	// Allocate the stream private data and write the stream header to an output media file.
	if (0 != avformat_write_header(ofmt_ctx_video, NULL))
	{
		printf("error: avformat_write_header, output video\n");
		end_free();
	}


	// Print detailed information about the input or output format.
	av_dump_format(ifmt_ctx, -1, input_filepath, 0);

	av_dump_format(ofmt_ctx_video, -1, output_filepath_video, 1);


	// Initialize optional fields of a packet with default values.
	av_init_packet(&ipkt);
	ipkt.data = NULL;
	ipkt.size = 0;


#if USE_H264BSF
	// Create and initialize a bitstream filter context given a bitstream filter name.
	AVBitStreamFilterContext* h264bsfc = av_bitstream_filter_init("h264_mp4toannexb");
#endif

    clock_t begin, end;
    double cost;
    begin = clock();
    int frame_number=0;
	while (1)
	{
	// Return the next frame of a stream.
		if (av_read_frame(ifmt_ctx, &ipkt) < 0)
			break;

		if (ipkt.stream_index == idx_video)
		{
#if USE_H264BSF
			// Filter bitstream.
			av_bitstream_filter_filter(h264bsfc, pVideoStream->codec, NULL, &ipkt.data, &ipkt.size, ipkt.data, ipkt.size, 0);
#endif
			// 因为 ofmt_ctx_video.nb_stream == 1，此处 stream_index 必须设置为0，否则可能因为ipkt.stream_index >= ofmt_ctx_video.nb_stream，导致出错！！
			// 详见 ffmpeg 源函数 check_packet 的具体实现。
			ipkt.stream_index = 0;


// Write a packet to an output media file ensuring correct interleaving.
			if (av_interleaved_write_frame(ofmt_ctx_video, &ipkt) < 0)
			{
				printf("error: av_interleaved_write_frame, output video\n");
				break;
			}
		}
		else
		{
			continue;
		}
		
		end = clock();
		cost = (double)(end - begin)/CLOCKS_PER_SEC;
		//printf("constant CLOCKS_PER_SEC is: %ld, time cost is: %lf secs", CLOCKS_PER_SEC, cost);
		printf("one frame time : %f\n",cost);
		begin=end;
		frame_number++;
		// Free a packet.
		av_free_packet(&ipkt);
	}
	
		
    printf("frame_number: %d\n",frame_number);

	// Write the stream trailer to an output media file and free the file private data.
	if (0 != av_write_trailer(ofmt_ctx_video))
	{
		printf("error: av_write_trailer\n");
		end_free();
	}


#if USE_H264BSF
		// Release bitstream filter context.
	av_bitstream_filter_close(h264bsfc);
#endif

		// Close an opened input AVFormatContext.Free it and all its contents and set *s to NULL.
	avformat_close_input(&ifmt_ctx);


// Close the resource accessed by the AVIOContext s and free it.
	if (NULL != ofmt_ctx_video)
		avio_close(ofmt_ctx_video->pb);

	avformat_free_context(ofmt_ctx_video);
	
	end_all = clock();
	cost_all = (double)(end_all - begin_all)/CLOCKS_PER_SEC;
	printf("constant CLOCKS_PER_SEC is: %ld, time cost is: %lf secs", CLOCKS_PER_SEC, cost_all);
    return 0;
}
