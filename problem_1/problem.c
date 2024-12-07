/*
 * Problem Statement
 *
 * Create a GStreamer program that takes a file as an input and plays both the 
 * audio and video on the screen.
 */
 
#include <gst/gst.h>

/* Implementation of the pad added signal */
void pad_added_handler(GstElement *src, GstPad *pad, gpointer data)
{
    GstElement *pipeline = (GstElement*) data;
    
    GstElement *videoSink = gst_bin_get_by_name(GST_BIN(pipeline), "videosink");
    GstElement *audioSink = gst_bin_get_by_name(GST_BIN(pipeline), "audioConvert");
    
    GstCaps *caps = gst_pad_query_caps(pad, NULL);
    const gchar *mediaType = gst_structure_get_name(gst_caps_get_structure(caps, 0));
    
    if (g_str_has_prefix(mediaType, "video/x-raw"))
    {
        GstPad *sinkPad = gst_element_get_static_pad(videoSink, "sink");
        if (!gst_pad_is_linked(sinkPad))
        {
            if (GST_PAD_LINK_OK != gst_pad_link(pad, sinkPad))
            {
                g_print("Failed to link decoderbin to the videosink\n");
            }
            else
            {
                g_print("Linked decodebin to the videosink\n");
            }
        }
        g_object_unref(sinkPad);
    }
    else if (g_str_has_prefix(mediaType, "audio/x-raw"))
    {
        GstPad *sinkPad = gst_element_get_static_pad(audioSink, "sink");
        if (!gst_pad_is_linked(sinkPad))
        {
            if (GST_PAD_LINK_OK != gst_pad_link(pad, sinkPad))
            {
                g_print("Failed to link decodebin to audioSink\n");
            }
            else
            {
                g_print("Linked decodebin to audioSink successfully\n");
            }
        }
        g_object_unref(sinkPad);
    }
    gst_caps_unref(caps);
    g_object_unref(videoSink);
    g_object_unref(audioSink);
}


int main(int argc, char* argv[])
{
    GstElement *pipeline, *source, *videoSink, *decoder, *audioSink, *audioConvert, *audioResample;
    GstBus *bus;
    GstMessage *msg;
    
    /* Initialize GStreamer*/
    gst_init(&argc, &argv);
    
    /* Create the required plugins*/
    pipeline = gst_pipeline_new("play-from-file");    
    source = gst_element_factory_make("filesrc", "source");
    decoder = gst_element_factory_make("decodebin", "decoder");
    videoSink = gst_element_factory_make("autovideosink", "videosink");
    audioSink = gst_element_factory_make("autoaudiosink", "audiosink");
    audioConvert = gst_element_factory_make("audioconvert", "audioConvert");
    audioResample = gst_element_factory_make("audioresample", "audioResample");
    
    /* Validate the created plugins*/
    if (!pipeline ||
        !source ||
        !decoder ||
        !videoSink ||
        !audioSink ||
        !audioConvert ||
        !audioResample)
    {
        g_printerr("Failed to create the GStreamer elements\n");
        return -1;
    }
    
    /* Add elements to the pipeline */
    gst_bin_add_many(GST_BIN(pipeline), source, decoder, videoSink, audioSink, audioConvert, audioResample, NULL);
    
    /* Set the plugin parameters*/
    g_object_set(source, "location", "/home/rakesh/Documents/GStreamerLearning/problem_1/test.mp4", NULL);
    
    /* Link the plugins*/
    if (!gst_element_link(source, decoder))
    {
        g_printerr("Failed to link elements filesrc and decoder\n");
        return -1;
    }
    
    if (!gst_element_link(audioConvert, audioResample))
    {
        g_printerr("Failed to link audioConvert to audioResample\n");
        return -1;
    }
    
    if (!gst_element_link(audioResample, audioSink))
    {
        g_printerr("Failed to link audioConvert to audioResample\n");
        return -1;
    }
    
    /* Link the plugin to a signal */
    g_signal_connect(decoder, "pad-added", G_CALLBACK(pad_added_handler), pipeline);
    
    /* Set the pipeline state */
    gst_element_set_state(pipeline, GST_STATE_PLAYING);
    
    /* Handle error messages */ 
    bus = gst_element_get_bus(pipeline);
    msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS);
    
    if (msg)
    {
        switch(GST_MESSAGE_TYPE(msg))
        {
            case GST_MESSAGE_ERROR:
                g_printerr("Error occurred. Run ./GST_DEBUG=4 ./<executable> for more info\n");
                break;
                
            case GST_MESSAGE_EOS:
                g_print("EOS receiver\n");
                break;
                
            default:
                g_printerr("Unknown message received");
                break;
        }
    }
    
    return 0;
}
