/*
 * Problem Statement
 *
 * Query the caps of the source pad of videotestsrc plugin and print the same.
 *
 */

#include <gst/gst.h>

int main(int argc, char *argv[])
{
    GstElement *pipeline, *videoTestSrc, *videoSink;
    GstBus *bus;
    GstMessage *msg;
    GstPad *pad;
    GstCaps *caps;
    
    gst_init(&argc, &argv);
    
    pipeline = gst_pipeline_new("pipeline");
    videoTestSrc = gst_element_factory_make("videotestsrc", "videotestsrc");
    videoSink = gst_element_factory_make("autovideosink", "videosink");
    
    if (!pipeline ||
        !videoTestSrc ||
        !videoSink)
    {
        g_printerr("Failed to create the elements\n");
        return -1;
    }
    
    gst_bin_add_many(GST_BIN(pipeline), videoTestSrc, videoSink, NULL);    
    if (!gst_element_link(videoTestSrc, videoSink))
    {
        g_printerr("Failed to link the elements");
        return -1;
    }
    
    pad = gst_element_get_static_pad(videoTestSrc, "src");
    caps = gst_pad_query_caps(pad, NULL);
    
    g_print("caps supported by videotestsrc: %s", gst_caps_to_string(caps));
    
    gst_element_set_state(pipeline, GST_STATE_PLAYING);
    
    bus = gst_element_get_bus(pipeline);
    msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_EOS | GST_MESSAGE_ERROR);
    
    if (msg)
    {
        switch (GST_MESSAGE_TYPE(msg))
        {
            case GST_MESSAGE_ERROR:
                g_print("Error occurred. Increase the logs for more details\n");
                break;
                
            case GST_MESSAGE_EOS:
                g_print("End of stream reached\n");
                break;
        }
    }
    
    gst_caps_unref(caps);
    g_object_unref(bus);
    gst_message_unref(msg);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    g_object_unref(pipeline);
    
    return 0;
}
