/*
 * Problem Statement
 *
 * Create a program that outputs video of custom resolution and frame-rate
 */

#include <gst/gst.h>

#define WIDTH 1920
#define HEIGHT 1080
#define FRAME_RATE 30

int main(int argc, char* argv[])
{
    GstElement *pipeline, *videoTestSrc, *videoConvert, *videoSink, *capsFilter;
    GstBus *bus;
    GstMessage *msg;
    GstCaps *caps;
    
    gst_init(&argc, &argv);
    
    pipeline = gst_pipeline_new("pipeline");
    
    videoTestSrc = gst_element_factory_make("videotestsrc", "videosrc");
    videoConvert = gst_element_factory_make("videoconvert", "videoconvert");
    videoSink = gst_element_factory_make("autovideosink", "videosink");
    capsFilter = gst_element_factory_make("capsfilter", "capsfilter");
    
    if (!pipeline ||
        !videoTestSrc ||
        !videoConvert ||
        !videoSink ||
        !capsFilter)
    {
        g_printerr("Failed to create elements\n");
        return -1;
    }
    
    gst_bin_add_many(GST_BIN(pipeline), videoTestSrc, videoConvert, videoSink, capsFilter, NULL);
    
    caps = gst_caps_new_simple("video/x-raw",
                               "width", G_TYPE_INT, WIDTH,
                               "height", G_TYPE_INT, HEIGHT,
                               "framerate", GST_TYPE_FRACTION, FRAME_RATE, 1,
                               NULL);
                               
    g_object_set(capsFilter, "caps", caps, NULL);
    
    gst_element_link(videoTestSrc, capsFilter);
    gst_element_link(capsFilter, videoConvert);
    gst_element_link(videoConvert, videoSink);
    
    gst_element_set_state(pipeline, GST_STATE_PLAYING);
    
    bus = gst_element_get_bus(pipeline);
    msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_EOS | GST_MESSAGE_ERROR);
    
    if (msg)
    {
        switch(GST_MESSAGE_TYPE(msg))
        {
            case GST_MESSAGE_EOS:
                g_print("End of stream reached. Thankyou...\n");
                break;
                
            case GST_MESSAGE_ERROR:
                g_print("Error occurred. Increase the log level for more info\n");
                break;
        }
    }
    
    gst_caps_unref(caps);
    gst_object_unref(bus);
    gst_message_unref(msg);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    
    return 0;
}
