/*
 * Problem Statement
 *
 * Create a video source with dynamic bit-rate change. This is the regular scenario encountered 
 * in the real world case.
 */
 
#include <gst/gst.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    GstElement *pipeline, *videoSrc, *capsFilter, *videoRate, *encoder, *muxer, *fileSink;
    GstBus *bus;
    GstMessage *msg;
    GstCaps *caps;
    
    gst_init(&argc, &argv);
    
    pipeline = gst_pipeline_new("pipeline");
    videoSrc = gst_element_factory_make("videotestsrc", "videosrc");
    videoRate = gst_element_factory_make("videorate", "videorate");
    capsFilter = gst_element_factory_make("capsfilter", "capsfilter");
    encoder = gst_element_factory_make("x264enc", "encoder");
    muxer = gst_element_factory_make("mpegtsmux", "muxer");
    fileSink = gst_element_factory_make("filesink", "filesink");
    
    if (!pipeline ||
        !videoSrc ||
        !videoRate ||
        !capsFilter ||
        !encoder ||
        !muxer ||
        !fileSink)
    {
        g_print("Failed to create elements\n");
        return -1;
    }
    
    gst_bin_add_many(GST_BIN(pipeline), videoSrc, videoRate, capsFilter, encoder, muxer, fileSink, NULL);
    caps = gst_caps_new_simple("video/x-raw",
                        "framerate", GST_TYPE_FRACTION, 30, 1,
                        NULL);
                        
    g_object_set(capsFilter, "caps", caps, NULL);
    gst_object_unref(caps);
   
    g_object_set(fileSink, "location", "/home/rakesh/Documents/GStreamerLearning/problem_3/test.ts", NULL);
    g_object_set(encoder, "bitrate", 500, NULL);
   
    if (!gst_element_link_many(videoSrc, videoRate, capsFilter, encoder, muxer, fileSink, NULL))
    {
        g_printerr("Failed to link the elements\n");
        return -1;
    }
   
    gst_element_set_state(pipeline, GST_STATE_PLAYING);
   
    sleep(10);
    
    g_object_set(encoder, "bitrate", 2000, NULL);
    
    bus = gst_element_get_bus(pipeline);
    msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS);
    
    if (msg)    
    {
        switch(GST_MESSAGE_TYPE(msg))
        {
            case GST_MESSAGE_ERROR:
                g_print("Error occurred. Try again with increased log level\n");
                break;
                
            case GST_MESSAGE_EOS:
                g_print("End of stream reached\n");
                 break;
                 
        }
    }
    
    gst_object_unref(bus);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    
    return 0;
}
