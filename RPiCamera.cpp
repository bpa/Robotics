/*
Copyright (c) 2013, Broadcom Europe Ltd
Copyright (c) 2013, James Hughes
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the copyright holder nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

// We use some GNU extensions (basename)
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <sysexits.h>

#include "bcm_host.h"
#include "interface/vcos/vcos.h"

#include "interface/mmal/mmal.h"
#include "interface/mmal/mmal_logging.h"
#include "interface/mmal/mmal_buffer.h"
#include "interface/mmal/util/mmal_util.h"
#include "interface/mmal/util/mmal_util_params.h"
#include "interface/mmal/util/mmal_default_components.h"
#include "interface/mmal/util/mmal_connection.h"

#include "RaspiCamControl.h"
#include "RaspiPreview.h"
#include "RaspiCLI.h"

#include <semaphore.h>

/// Camera number to use - we only have one camera, indexed from 0.
#define CAMERA_NUMBER 0

// Standard port setting for the camera component
#define MMAL_CAMERA_PREVIEW_PORT 0
#define MMAL_CAMERA_VIDEO_PORT 1
#define MMAL_CAMERA_CAPTURE_PORT 2

// Video format information
// 0 implies variable
#define VIDEO_FRAME_RATE_NUM 30
#define VIDEO_FRAME_RATE_DEN 1

/// Video render needs at least 2 buffers.
#define VIDEO_OUTPUT_BUFFERS_NUM 3

// Max bitrate we allow for recording
const int MAX_BITRATE = 25000000; // 25Mbits/s

/// Interval at which we check for an failure abort during capture
const int ABORT_INTERVAL = 100; // ms

int mmal_status_to_int(MMAL_STATUS_T status);

// Forward
typedef struct RASPIVID_STATE_S RASPIVID_STATE;

/** Struct used to pass information in encoder port userdata to callback
 */
typedef struct
{
   FILE *file_handle;                   /// File handle to write buffer data to.
   RASPIVID_STATE *pstate;              /// pointer to our state in case required in callback
   int abort;                           /// Set to 1 in callback if an error occurs to attempt to abort the capture
   char *cb_buff;                       /// Circular buffer
   int   cb_len;                        /// Length of buffer
   int   cb_wptr;                       /// Current write pointer
   int   cb_wrap;                       /// Has buffer wrapped at least once?
   int   cb_data;                       /// Valid bytes in buffer
#define IFRAME_BUFSIZE (60*1000)
   int   iframe_buff[IFRAME_BUFSIZE];          /// buffer of iframe pointers
   int   iframe_buff_wpos;
   int   iframe_buff_rpos;
   char  header_bytes[29];
   int  header_wptr;
} PORT_USERDATA;

/** Structure containing all state information for the current run
 */
struct RASPIVID_STATE_S
{
   int timeout;                        /// Time taken before frame is grabbed and app then shuts down. Units are milliseconds
   int width;                          /// Requested width of image
   int height;                         /// requested height of image
   int bitrate;                        /// Requested bitrate
   int framerate;                      /// Requested frame rate (fps)
   int intraperiod;                    /// Intra-refresh period (key frame rate)
   int quantisationParameter;          /// Quantisation parameter - quality. Set bitrate 0 and set this for variable bitrate
   int bInlineHeaders;                  /// Insert inline headers to stream (SPS, PPS)
   char *filename;                     /// filename of output file
   int verbose;                        /// !0 if want detailed run information
   int demoMode;                       /// Run app in demo mode
   int demoInterval;                   /// Interval between camera settings changes
   int immutableInput;                 /// Flag to specify whether encoder works in place or creates a new buffer. Result is preview can display either
                                       /// the camera output or the encoder output (with compression artifacts)
   int profile;                        /// H264 profile to use for encoding
   int waitMethod;                     /// Method for switching between pause and capture

   int onTime;                         /// In timed cycle mode, the amount of time the capture is on per cycle
   int offTime;                        /// In timed cycle mode, the amount of time the capture is off per cycle

   int segmentSize;                    /// Segment mode In timed cycle mode, the amount of time the capture is off per cycle
   int segmentWrap;                    /// Point at which to wrap segment counter
   int segmentNumber;                  /// Current segment counter
   int splitNow;                       /// Split at next possible i-frame if set to 1.
   int splitWait;                      /// Switch if user wants splited files 

   RASPIPREVIEW_PARAMETERS preview_parameters;   /// Preview setup parameters
   RASPICAM_CAMERA_PARAMETERS camera_parameters; /// Camera setup parameters

   MMAL_COMPONENT_T *camera_component;    /// Pointer to the camera component
   MMAL_COMPONENT_T *encoder_component;   /// Pointer to the encoder component
   MMAL_CONNECTION_T *preview_connection; /// Pointer to the connection from camera to preview
   MMAL_CONNECTION_T *encoder_connection; /// Pointer to the connection from camera to encoder

   MMAL_POOL_T *encoder_pool; /// Pointer to the pool of buffers used by encoder output port

   PORT_USERDATA callback_data;        /// Used to move data to the encoder callback

   int bCapturing;                     /// State of capture/pause
   int bCircularBuffer;                /// Whether we are writing to a circular buffer

};

/// Structure to cross reference H264 profile strings against the MMAL parameter equivalent
static XREF_T  profile_map[] =
{
   {"baseline",     MMAL_VIDEO_PROFILE_H264_BASELINE},
   {"main",         MMAL_VIDEO_PROFILE_H264_MAIN},
   {"high",         MMAL_VIDEO_PROFILE_H264_HIGH},
//   {"constrained",  MMAL_VIDEO_PROFILE_H264_CONSTRAINED_BASELINE} // Does anyone need this?
};

static int profile_map_size = sizeof(profile_map) / sizeof(profile_map[0]);

/**
 * Assign a default set of parameters to the state passed in
 *
 * @param state Pointer to state structure to assign defaults to
 */
static void default_status(RASPIVID_STATE *state)
{
   if (!state)
   {
      vcos_assert(0);
      return;
   }

   // Default everything to zero
   memset(state, 0, sizeof(RASPIVID_STATE));

   // Now set anything non-zero
   state->timeout = 5000;     // 5s delay before take image
   state->width = 1920;       // Default to 1080p
   state->height = 1080;
   state->bitrate = 17000000; // This is a decent default bitrate for 1080p
   state->framerate = VIDEO_FRAME_RATE_NUM;
   state->intraperiod = 0;    // Not set
   state->quantisationParameter = 0;
   state->demoMode = 0;
   state->demoInterval = 250; // ms
   state->immutableInput = 1;
   state->profile = MMAL_VIDEO_PROFILE_H264_HIGH;
   state->waitMethod = WAIT_METHOD_NONE;
   state->onTime = 5000;
   state->offTime = 5000;

   state->bCapturing = 0;
   state->bInlineHeaders = 0;

   state->segmentSize = 0;  // 0 = not segmenting the file.
   state->segmentNumber = 1;
   state->segmentWrap = 0; // Point at which to wrap segment number back to 1. 0 = no wrap
   state->splitNow = 0;
   state->splitWait = 0;

   // Setup preview window defaults
   raspipreview_set_defaults(&state->preview_parameters);

   // Set up the camera_parameters to default
   raspicamcontrol_set_defaults(&state->camera_parameters);
}

/**
 *  buffer header callback function for camera control
 *
 *  Callback will dump buffer data to the specific file
 *
 * @param port Pointer to port from which callback originated
 * @param buffer mmal buffer header pointer
 */
static void camera_control_callback(MMAL_PORT_T *port, MMAL_BUFFER_HEADER_T *buffer)
{
   if (buffer->cmd == MMAL_EVENT_PARAMETER_CHANGED)
   {
   }
   else
   {
      vcos_log_error("Received unexpected camera control callback event, 0x%08x", buffer->cmd);
   }

   mmal_buffer_header_release(buffer);
}

/**
 *  buffer header callback function for encoder
 *
 *  Callback will dump buffer data to the specific file
 *
 * @param port Pointer to port from which callback originated
 * @param buffer mmal buffer header pointer
 */
static void encoder_buffer_callback(MMAL_PORT_T *port, MMAL_BUFFER_HEADER_T *buffer)
{
   MMAL_BUFFER_HEADER_T *new_buffer;
   static int64_t base_time =  -1;

   // All our segment times based on the receipt of the first encoder callback
   if (base_time == -1)
      base_time = vcos_getmicrosecs64()/1000;

   // We pass our file handle and other stuff in via the userdata field.

   PORT_USERDATA *pData = (PORT_USERDATA *)port->userdata;

   if (pData)
   {
      int bytes_written = buffer->length;
      int64_t current_time = vcos_getmicrosecs64()/1000;

      vcos_assert(pData->file_handle);

      // For segmented record mode, we need to see if we have exceeded our time/size,
      // but also since we have inline headers turned on we need to break when we get one to
      // ensure that the new stream has the header in it. If we break on an I-frame, the
      // SPS/PPS header is actually in the previous chunk.
      if ((buffer->flags & MMAL_BUFFER_HEADER_FLAG_CONFIG) &&
          ((pData->pstate->segmentSize && current_time > base_time + pData->pstate->segmentSize) ||
           (pData->pstate->splitWait && pData->pstate->splitNow)))
      {
         FILE *new_handle;

         base_time = current_time;

         pData->pstate->splitNow = 0;
         pData->pstate->segmentNumber++;

         // Only wrap if we have a wrap point set
         if (pData->pstate->segmentWrap && pData->pstate->segmentNumber > pData->pstate->segmentWrap)
            pData->pstate->segmentNumber = 1;

         new_handle = open_filename(pData->pstate);

         if (new_handle)
         {
            fclose(pData->file_handle);
            pData->file_handle = new_handle;
         }
      }
      else if (pData->cb_buff)
      {
         int space_in_buff = pData->cb_len - pData->cb_wptr;
         int copy_to_end = space_in_buff > buffer->length ? buffer->length : space_in_buff;
         int copy_to_start = buffer->length - copy_to_end;

         if(buffer->flags & MMAL_BUFFER_HEADER_FLAG_CONFIG)
         {
            if(pData->header_wptr + buffer->length > sizeof(pData->header_bytes))
            {
               vcos_log_error("Error in header bytes\n");
            }
            else
            {
               // These are the header bytes, save them for final output
               mmal_buffer_header_mem_lock(buffer);
               memcpy(pData->header_bytes + pData->header_wptr, buffer->data, buffer->length);
               mmal_buffer_header_mem_unlock(buffer);
               pData->header_wptr += buffer->length;
            }
         }
         else
         {
            static int frame_start = -1;
            int i;

            if(frame_start == -1)
               frame_start = pData->cb_wptr;

            if(buffer->flags & MMAL_BUFFER_HEADER_FLAG_KEYFRAME)
            {
               pData->iframe_buff[pData->iframe_buff_wpos] = frame_start;
               pData->iframe_buff_wpos = (pData->iframe_buff_wpos + 1) % IFRAME_BUFSIZE;
            }

            if(buffer->flags & MMAL_BUFFER_HEADER_FLAG_FRAME_END)
               frame_start = -1;

            // If we overtake the iframe rptr then move the rptr along
            if((pData->iframe_buff_rpos + 1) % IFRAME_BUFSIZE != pData->iframe_buff_wpos)
               while(
                  (
                     pData->cb_wptr <= pData->iframe_buff[pData->iframe_buff_rpos] &&
                    (pData->cb_wptr + buffer->length) > pData->iframe_buff[pData->iframe_buff_rpos]
                  ) ||
                  (
                    (pData->cb_wptr > pData->iframe_buff[pData->iframe_buff_rpos]) &&
                    (pData->cb_wptr + buffer->length) > (pData->iframe_buff[pData->iframe_buff_rpos] + pData->cb_len)
                  )
               )
                  pData->iframe_buff_rpos = (pData->iframe_buff_rpos + 1) % IFRAME_BUFSIZE;

               mmal_buffer_header_mem_lock(buffer);
               // We are pushing data into a circular buffer
               memcpy(pData->cb_buff + pData->cb_wptr, buffer->data, copy_to_end);
               memcpy(pData->cb_buff, buffer->data + copy_to_end, copy_to_start);
               mmal_buffer_header_mem_unlock(buffer);

               if((pData->cb_wptr + buffer->length) > pData->cb_len)
                  pData->cb_wrap = 1;

               pData->cb_wptr = (pData->cb_wptr + buffer->length) % pData->cb_len;

               for(i = pData->iframe_buff_rpos; i != pData->iframe_buff_wpos; i = (i + 1) % IFRAME_BUFSIZE)
               {
                  int p = pData->iframe_buff[i];
                  if(pData->cb_buff[p] != 0 || pData->cb_buff[p+1] != 0 || pData->cb_buff[p+2] != 0 || pData->cb_buff[p+3] != 1)
                  {
                     vcos_log_error("Error in iframe list\n");
                  }
               }
            }
         }
      else if (buffer->length)
      {
         mmal_buffer_header_mem_lock(buffer);

         bytes_written = fwrite(buffer->data, 1, buffer->length, pData->file_handle);

         mmal_buffer_header_mem_unlock(buffer);

         if (bytes_written != buffer->length)
         {
            vcos_log_error("Failed to write buffer data (%d from %d)- aborting", bytes_written, buffer->length);
            pData->abort = 1;
         }
      }
   }
   else
   {
      vcos_log_error("Received a encoder buffer callback with no state");
   }

   // release buffer back to the pool
   mmal_buffer_header_release(buffer);

   // and send one back to the port (if still open)
   if (port->is_enabled)
   {
      MMAL_STATUS_T status;

      new_buffer = mmal_queue_get(pData->pstate->encoder_pool->queue);

      if (new_buffer)
         status = mmal_port_send_buffer(port, new_buffer);

      if (!new_buffer || status != MMAL_SUCCESS)
         vcos_log_error("Unable to return a buffer to the encoder port");
   }
}


/**
 * Create the camera component, set up its ports
 *
 * @param state Pointer to state control struct
 *
 * @return MMAL_SUCCESS if all OK, something else otherwise
 *
 */
static MMAL_STATUS_T create_camera_component(RASPIVID_STATE *state)
{
   MMAL_COMPONENT_T *camera = 0;
   MMAL_ES_FORMAT_T *format;
   MMAL_PORT_T *preview_port = NULL, *video_port = NULL, *still_port = NULL;
   MMAL_STATUS_T status;

   /* Create the component */
   status = mmal_component_create(MMAL_COMPONENT_DEFAULT_CAMERA, &camera);

   if (status != MMAL_SUCCESS)
   {
      vcos_log_error("Failed to create camera component");
      goto error;
   }

   if (!camera->output_num)
   {
      status = MMAL_ENOSYS;
      vcos_log_error("Camera doesn't have output ports");
      goto error;
   }

   preview_port = camera->output[MMAL_CAMERA_PREVIEW_PORT];
   video_port = camera->output[MMAL_CAMERA_VIDEO_PORT];
   still_port = camera->output[MMAL_CAMERA_CAPTURE_PORT];

   // Enable the camera, and tell it its control callback function
   status = mmal_port_enable(camera->control, camera_control_callback);

   if (status != MMAL_SUCCESS)
   {
      vcos_log_error("Unable to enable control port : error %d", status);
      goto error;
   }

   //  set up the camera configuration
   {
      MMAL_PARAMETER_CAMERA_CONFIG_T cam_config =
      {
         { MMAL_PARAMETER_CAMERA_CONFIG, sizeof(cam_config) },
         .max_stills_w = state->width,
         .max_stills_h = state->height,
         .stills_yuv422 = 0,
         .one_shot_stills = 0,
         .max_preview_video_w = state->width,
         .max_preview_video_h = state->height,
         .num_preview_video_frames = 3,
         .stills_capture_circular_buffer_height = 0,
         .fast_preview_resume = 0,
         .use_stc_timestamp = MMAL_PARAM_TIMESTAMP_MODE_RESET_STC
      };
      mmal_port_parameter_set(camera->control, &cam_config.hdr);
   }

   // Now set up the port formats

   // Set the encode format on the Preview port
   // HW limitations mean we need the preview to be the same size as the required recorded output

   format = preview_port->format;

   format->encoding = MMAL_ENCODING_OPAQUE;
   format->encoding_variant = MMAL_ENCODING_I420;

   format->encoding = MMAL_ENCODING_OPAQUE;
   format->es->video.width = VCOS_ALIGN_UP(state->width, 32);
   format->es->video.height = VCOS_ALIGN_UP(state->height, 16);
   format->es->video.crop.x = 0;
   format->es->video.crop.y = 0;
   format->es->video.crop.width = state->width;
   format->es->video.crop.height = state->height;
   format->es->video.frame_rate.num = PREVIEW_FRAME_RATE_NUM;
   format->es->video.frame_rate.den = PREVIEW_FRAME_RATE_DEN;

   status = mmal_port_format_commit(preview_port);

   if (status != MMAL_SUCCESS)
   {
      vcos_log_error("camera viewfinder format couldn't be set");
      goto error;
   }

   // Set the encode format on the video  port

   format = video_port->format;
   format->encoding_variant = MMAL_ENCODING_I420;

   format->encoding = MMAL_ENCODING_OPAQUE;
   format->es->video.width = VCOS_ALIGN_UP(state->width, 32);
   format->es->video.height = VCOS_ALIGN_UP(state->height, 16);
   format->es->video.crop.x = 0;
   format->es->video.crop.y = 0;
   format->es->video.crop.width = state->width;
   format->es->video.crop.height = state->height;
   format->es->video.frame_rate.num = state->framerate;
   format->es->video.frame_rate.den = VIDEO_FRAME_RATE_DEN;

   status = mmal_port_format_commit(video_port);

   if (status != MMAL_SUCCESS)
   {
      vcos_log_error("camera video format couldn't be set");
      goto error;
   }

   // Ensure there are enough buffers to avoid dropping frames
   if (video_port->buffer_num < VIDEO_OUTPUT_BUFFERS_NUM)
      video_port->buffer_num = VIDEO_OUTPUT_BUFFERS_NUM;


   // Set the encode format on the still  port

   format = still_port->format;

   format->encoding = MMAL_ENCODING_OPAQUE;
   format->encoding_variant = MMAL_ENCODING_I420;

   format->es->video.width = VCOS_ALIGN_UP(state->width, 32);
   format->es->video.height = VCOS_ALIGN_UP(state->height, 16);
   format->es->video.crop.x = 0;
   format->es->video.crop.y = 0;
   format->es->video.crop.width = state->width;
   format->es->video.crop.height = state->height;
   format->es->video.frame_rate.num = 0;
   format->es->video.frame_rate.den = 1;

   status = mmal_port_format_commit(still_port);

   if (status != MMAL_SUCCESS)
   {
      vcos_log_error("camera still format couldn't be set");
      goto error;
   }

   /* Ensure there are enough buffers to avoid dropping frames */
   if (still_port->buffer_num < VIDEO_OUTPUT_BUFFERS_NUM)
      still_port->buffer_num = VIDEO_OUTPUT_BUFFERS_NUM;

   /* Enable component */
   status = mmal_component_enable(camera);

   if (status != MMAL_SUCCESS)
   {
      vcos_log_error("camera component couldn't be enabled");
      goto error;
   }

   raspicamcontrol_set_all_parameters(camera, &state->camera_parameters);

   state->camera_component = camera;

   if (state->verbose)
      fprintf(stderr, "Camera component done\n");

   return status;

error:

   if (camera)
      mmal_component_destroy(camera);

   return status;
}

/**
 * Destroy the camera component
 *
 * @param state Pointer to state control struct
 *
 */
static void destroy_camera_component(RASPIVID_STATE *state)
{
   if (state->camera_component)
   {
      mmal_component_destroy(state->camera_component);
      state->camera_component = NULL;
   }
}

/**
 * Create the encoder component, set up its ports
 *
 * @param state Pointer to state control struct
 *
 * @return MMAL_SUCCESS if all OK, something else otherwise
 *
 */
static MMAL_STATUS_T create_encoder_component(RASPIVID_STATE *state)
{
   MMAL_COMPONENT_T *encoder = 0;
   MMAL_PORT_T *encoder_input = NULL, *encoder_output = NULL;
   MMAL_STATUS_T status;
   MMAL_POOL_T *pool;

   status = mmal_component_create(MMAL_COMPONENT_DEFAULT_VIDEO_ENCODER, &encoder);

   if (status != MMAL_SUCCESS)
   {
      vcos_log_error("Unable to create video encoder component");
      goto error;
   }

   if (!encoder->input_num || !encoder->output_num)
   {
      status = MMAL_ENOSYS;
      vcos_log_error("Video encoder doesn't have input/output ports");
      goto error;
   }

   encoder_input = encoder->input[0];
   encoder_output = encoder->output[0];

   // We want same format on input and output
   mmal_format_copy(encoder_output->format, encoder_input->format);

   // Only supporting H264 at the moment
   encoder_output->format->encoding = MMAL_ENCODING_H264;

   encoder_output->format->bitrate = state->bitrate;

   encoder_output->buffer_size = encoder_output->buffer_size_recommended;

   if (encoder_output->buffer_size < encoder_output->buffer_size_min)
      encoder_output->buffer_size = encoder_output->buffer_size_min;

   encoder_output->buffer_num = encoder_output->buffer_num_recommended;

   if (encoder_output->buffer_num < encoder_output->buffer_num_min)
      encoder_output->buffer_num = encoder_output->buffer_num_min;

   // We need to set the frame rate on output to 0, to ensure it gets
   // updated correctly from the input framerate when port connected
   encoder_output->format->es->video.frame_rate.num = 0;
   encoder_output->format->es->video.frame_rate.den = 1;

   // Commit the port changes to the output port
   status = mmal_port_format_commit(encoder_output);

   if (status != MMAL_SUCCESS)
   {
      vcos_log_error("Unable to set format on video encoder output port");
      goto error;
   }

   // Set the rate control parameter
   if (0)
   {
      MMAL_PARAMETER_VIDEO_RATECONTROL_T param = {{ MMAL_PARAMETER_RATECONTROL, sizeof(param)}, MMAL_VIDEO_RATECONTROL_DEFAULT};
      status = mmal_port_parameter_set(encoder_output, &param.hdr);
      if (status != MMAL_SUCCESS)
      {
         vcos_log_error("Unable to set ratecontrol");
         goto error;
      }

   }

   if (state->intraperiod)
   {
      MMAL_PARAMETER_UINT32_T param = {{ MMAL_PARAMETER_INTRAPERIOD, sizeof(param)}, state->intraperiod};
      status = mmal_port_parameter_set(encoder_output, &param.hdr);
      if (status != MMAL_SUCCESS)
      {
         vcos_log_error("Unable to set intraperiod");
         goto error;
      }
   }

   if (state->quantisationParameter)
   {
      MMAL_PARAMETER_UINT32_T param = {{ MMAL_PARAMETER_VIDEO_ENCODE_INITIAL_QUANT, sizeof(param)}, state->quantisationParameter};
      status = mmal_port_parameter_set(encoder_output, &param.hdr);
      if (status != MMAL_SUCCESS)
      {
         vcos_log_error("Unable to set initial QP");
         goto error;
      }

      MMAL_PARAMETER_UINT32_T param2 = {{ MMAL_PARAMETER_VIDEO_ENCODE_MIN_QUANT, sizeof(param)}, state->quantisationParameter};
      status = mmal_port_parameter_set(encoder_output, &param2.hdr);
      if (status != MMAL_SUCCESS)
      {
         vcos_log_error("Unable to set min QP");
         goto error;
      }

      MMAL_PARAMETER_UINT32_T param3 = {{ MMAL_PARAMETER_VIDEO_ENCODE_MAX_QUANT, sizeof(param)}, state->quantisationParameter};
      status = mmal_port_parameter_set(encoder_output, &param3.hdr);
      if (status != MMAL_SUCCESS)
      {
         vcos_log_error("Unable to set max QP");
         goto error;
      }

   }

   {
      MMAL_PARAMETER_VIDEO_PROFILE_T  param;
      param.hdr.id = MMAL_PARAMETER_PROFILE;
      param.hdr.size = sizeof(param);

      param.profile[0].profile = state->profile;
      param.profile[0].level = MMAL_VIDEO_LEVEL_H264_4; // This is the only value supported

      status = mmal_port_parameter_set(encoder_output, &param.hdr);
      if (status != MMAL_SUCCESS)
      {
         vcos_log_error("Unable to set H264 profile");
         goto error;
      }
   }

   if (mmal_port_parameter_set_boolean(encoder_input, MMAL_PARAMETER_VIDEO_IMMUTABLE_INPUT, state->immutableInput) != MMAL_SUCCESS)
   {
      vcos_log_error("Unable to set immutable input flag");
      // Continue rather than abort..
   }

   //set INLINE HEADER flag to generate SPS and PPS for every IDR if requested
   if (mmal_port_parameter_set_boolean(encoder_output, MMAL_PARAMETER_VIDEO_ENCODE_INLINE_HEADER, state->bInlineHeaders) != MMAL_SUCCESS)
   {
      vcos_log_error("failed to set INLINE HEADER FLAG parameters");
      // Continue rather than abort..
   }

   //  Enable component
   status = mmal_component_enable(encoder);

   if (status != MMAL_SUCCESS)
   {
      vcos_log_error("Unable to enable video encoder component");
      goto error;
   }

   /* Create pool of buffer headers for the output port to consume */
   pool = mmal_port_pool_create(encoder_output, encoder_output->buffer_num, encoder_output->buffer_size);

   if (!pool)
   {
      vcos_log_error("Failed to create buffer header pool for encoder output port %s", encoder_output->name);
   }

   state->encoder_pool = pool;
   state->encoder_component = encoder;

   if (state->verbose)
      fprintf(stderr, "Encoder component done\n");

   return status;

   error:
   if (encoder)
      mmal_component_destroy(encoder);

   state->encoder_component = NULL;

   return status;
}

/**
 * Destroy the encoder component
 *
 * @param state Pointer to state control struct
 *
 */
static void destroy_encoder_component(RASPIVID_STATE *state)
{
   // Get rid of any port buffers first
   if (state->encoder_pool)
   {
      mmal_port_pool_destroy(state->encoder_component->output[0], state->encoder_pool);
   }

   if (state->encoder_component)
   {
      mmal_component_destroy(state->encoder_component);
      state->encoder_component = NULL;
   }
}

/**
 * Connect two specific ports together
 *
 * @param output_port Pointer the output port
 * @param input_port Pointer the input port
 * @param Pointer to a mmal connection pointer, reassigned if function successful
 * @return Returns a MMAL_STATUS_T giving result of operation
 *
 */
static MMAL_STATUS_T connect_ports(MMAL_PORT_T *output_port, MMAL_PORT_T *input_port, MMAL_CONNECTION_T **connection)
{
   MMAL_STATUS_T status;

   status =  mmal_connection_create(connection, output_port, input_port, MMAL_CONNECTION_FLAG_TUNNELLING | MMAL_CONNECTION_FLAG_ALLOCATION_ON_INPUT);

   if (status == MMAL_SUCCESS)
   {
      status =  mmal_connection_enable(*connection);
      if (status != MMAL_SUCCESS)
         mmal_connection_destroy(*connection);
   }

   return status;
}

/**
 * Checks if specified port is valid and enabled, then disables it
 *
 * @param port  Pointer the port
 *
 */
static void check_disable_port(MMAL_PORT_T *port)
{
   if (port && port->is_enabled)
      mmal_port_disable(port);
}

Camera::Camera()
{
   // Our main data storage vessel..
   RASPIVID_STATE state;
   int exit_code = EX_OK;

   MMAL_STATUS_T status = MMAL_SUCCESS;
   MMAL_PORT_T *camera_preview_port = NULL;
   MMAL_PORT_T *camera_video_port = NULL;
   MMAL_PORT_T *camera_still_port = NULL;
   MMAL_PORT_T *preview_input_port = NULL;
   MMAL_PORT_T *encoder_input_port = NULL;
   MMAL_PORT_T *encoder_output_port = NULL;

   bcm_host_init();

   // Register our application with the logging system
   vcos_log_register("RaspiVid", VCOS_LOG_CATEGORY);

   default_status(&state);

   // OK, we have a nice set of parameters. Now set up our components
   // We have three components. Camera, Preview and encoder.

   if ((status = create_camera_component(&state)) != MMAL_SUCCESS)
   {
      vcos_log_error("%s: Failed to create camera component", __func__);
      exit_code = EX_SOFTWARE;
   }
   else if ((status = raspipreview_create(&state.preview_parameters)) != MMAL_SUCCESS)
   {
      vcos_log_error("%s: Failed to create preview component", __func__);
      destroy_camera_component(&state);
      exit_code = EX_SOFTWARE;
   }
   else if ((status = create_encoder_component(&state)) != MMAL_SUCCESS)
   {
      vcos_log_error("%s: Failed to create encode component", __func__);
      raspipreview_destroy(&state.preview_parameters);
      destroy_camera_component(&state);
      exit_code = EX_SOFTWARE;
   }
   else
   {
      if (state.verbose)
         fprintf(stderr, "Starting component connection stage\n");

      camera_preview_port = state.camera_component->output[MMAL_CAMERA_PREVIEW_PORT];
      camera_video_port   = state.camera_component->output[MMAL_CAMERA_VIDEO_PORT];
      camera_still_port   = state.camera_component->output[MMAL_CAMERA_CAPTURE_PORT];
      preview_input_port  = state.preview_parameters.preview_component->input[0];
      encoder_input_port  = state.encoder_component->input[0];
      encoder_output_port = state.encoder_component->output[0];

      if (state.preview_parameters.wantPreview )
      {
         if (state.verbose)
         {
            fprintf(stderr, "Connecting camera preview port to preview input port\n");
            fprintf(stderr, "Starting video preview\n");
         }

         // Connect camera to preview
         status = connect_ports(camera_preview_port, preview_input_port, &state.preview_connection);

         if (status != MMAL_SUCCESS)
            state.preview_connection = NULL;
      }
      else
      {
         status = MMAL_SUCCESS;
      }

      if (status == MMAL_SUCCESS)
      {
         if (state.verbose)
            fprintf(stderr, "Connecting camera stills port to encoder input port\n");

         // Now connect the camera to the encoder
         status = connect_ports(camera_video_port, encoder_input_port, &state.encoder_connection);

         if (status != MMAL_SUCCESS)
         {
            state.encoder_connection = NULL;
            vcos_log_error("%s: Failed to connect camera video port to encoder input", __func__);
            goto error;
         }

         state.callback_data.file_handle = NULL;

         if (state.filename)
         {
            if (state.filename[0] == '-')
            {
               state.callback_data.file_handle = stdout;

               // Ensure we don't upset the output stream with diagnostics/info
               state.verbose = 0;
            }
            else
            {
               state.callback_data.file_handle = open_filename(&state);
            }

            if (!state.callback_data.file_handle)
            {
               // Notify user, carry on but discarding encoded output buffers
               vcos_log_error("%s: Error opening output file: %s\nNo output file will be generated\n", __func__, state.filename);
            }
         }

         if(state.bCircularBuffer)
         {
            if(state.bitrate == 0)
            {
               vcos_log_error("%s: Error circular buffer requires constant bitrate and small intra period\n", __func__);
               goto error;
            }
            else if(state.timeout == 0)
            {
               vcos_log_error("%s: Error, circular buffer size is based on timeout must be greater than zero\n", __func__);
               goto error;
            }
            else if(state.waitMethod != WAIT_METHOD_KEYPRESS && state.waitMethod != WAIT_METHOD_SIGNAL)
            {
               vcos_log_error("%s: Error, Circular buffer mode requires either keypress (-k) or signal (-s) triggering\n", __func__);
               goto error;
            }
            else if(!state.callback_data.file_handle)
            {
               vcos_log_error("%s: Error require output file (or stdout) for Circular buffer mode\n", __func__);
               goto error;
            }
            else
            {
               int count = state.bitrate * (state.timeout / 1000) / 8;

               state.callback_data.cb_buff = (char *) malloc(count);
               if(state.callback_data.cb_buff == NULL)
               {
                  vcos_log_error("%s: Unable to allocate circular buffer for %d seconds at %.1f Mbits\n", __func__, state.timeout / 1000, (double)state.bitrate/1000000.0);
                  goto error;
               }
               else
               {
                  state.callback_data.cb_len = count;
                  state.callback_data.cb_wptr = 0;
                  state.callback_data.cb_wrap = 0;
                  state.callback_data.cb_data = 0;
                  state.callback_data.iframe_buff_wpos = 0;
                  state.callback_data.iframe_buff_rpos = 0;
                  state.callback_data.header_wptr = 0;
               }
            }
         }

         // Set up our userdata - this is passed though to the callback where we need the information.
         state.callback_data.pstate = &state;
         state.callback_data.abort = 0;

         encoder_output_port->userdata = (struct MMAL_PORT_USERDATA_T *)&state.callback_data;

         if (state.verbose)
            fprintf(stderr, "Enabling encoder output port\n");

         // Enable the encoder output port and tell it its callback function
         status = mmal_port_enable(encoder_output_port, encoder_buffer_callback);

         if (status != MMAL_SUCCESS)
         {
            vcos_log_error("Failed to setup encoder output");
            goto error;
         }

         if (state.demoMode)
         {
            // Run for the user specific time..
            int num_iterations = state.timeout / state.demoInterval;
            int i;

            if (state.verbose)
               fprintf(stderr, "Running in demo mode\n");

            for (i=0;state.timeout == 0 || i<num_iterations;i++)
            {
               raspicamcontrol_cycle_test(state.camera_component);
               vcos_sleep(state.demoInterval);
            }
         }
         else
         {
            // Only encode stuff if we have a filename and it opened
            // Note we use the copy in the callback, as the call back MIGHT change the file handle
            if (state.callback_data.file_handle)
            {
               int running = 1;

               // Send all the buffers to the encoder output port
               {
                  int num = mmal_queue_length(state.encoder_pool->queue);
                  int q;
                  for (q=0;q<num;q++)
                  {
                     MMAL_BUFFER_HEADER_T *buffer = mmal_queue_get(state.encoder_pool->queue);

                     if (!buffer)
                        vcos_log_error("Unable to get a required buffer %d from pool queue", q);

                     if (mmal_port_send_buffer(encoder_output_port, buffer)!= MMAL_SUCCESS)
                        vcos_log_error("Unable to send a buffer to encoder output port (%d)", q);
                  }
               }
               
               int initialCapturing=state.bCapturing;
               while (running)
               {
                  // Change state

                  state.bCapturing = !state.bCapturing;

                  if (mmal_port_parameter_set_boolean(camera_video_port, MMAL_PARAMETER_CAPTURE, state.bCapturing) != MMAL_SUCCESS)
                  {
                     // How to handle?
                  }

                  // In circular buffer mode, exit and save the buffer (make sure we do this after having paused the capture
                  if(state.bCircularBuffer && !state.bCapturing)
                  {
                     break;
                  }

                  if (state.verbose)
                  {
                     if (state.bCapturing)
                        fprintf(stderr, "Starting video capture\n");
                     else
                        fprintf(stderr, "Pausing video capture\n");
                  }
                  
                  if(state.splitWait)
                  {
                     if(state.bCapturing)
                     {
                        if (mmal_port_parameter_set_boolean(encoder_output_port, MMAL_PARAMETER_VIDEO_REQUEST_I_FRAME, 1) != MMAL_SUCCESS)
                        {
                           vcos_log_error("failed to request I-FRAME");
                        }
                     }
                     else
                     {
                        if(!initialCapturing)
                           state.splitNow=1;   
                     }
                     initialCapturing=0;
                  }
                  running = wait_for_next_change(&state);
               }

               if (state.verbose)
                  fprintf(stderr, "Finished capture\n");
            }
            else
            {
               if (state.timeout)
                  vcos_sleep(state.timeout);
               else
               {
                  // timeout = 0 so run forever
                  while(1)
                     vcos_sleep(ABORT_INTERVAL);
               }
            }
         }
      }
      else
      {
         mmal_status_to_int(status);
         vcos_log_error("%s: Failed to connect camera to preview", __func__);
      }

      if(state.bCircularBuffer)
      {
         int copy_from_end, copy_from_start;

         copy_from_end = state.callback_data.cb_len - state.callback_data.iframe_buff[state.callback_data.iframe_buff_rpos];
         copy_from_start = state.callback_data.cb_len - copy_from_end;
         copy_from_start = state.callback_data.cb_wptr < copy_from_start ? state.callback_data.cb_wptr : copy_from_start;
         if(!state.callback_data.cb_wrap)
         {
            copy_from_start = state.callback_data.cb_wptr;
            copy_from_end = 0;
         }

         fwrite(state.callback_data.header_bytes, 1, state.callback_data.header_wptr, state.callback_data.file_handle);
         // Save circular buffer
         fwrite(state.callback_data.cb_buff + state.callback_data.iframe_buff[state.callback_data.iframe_buff_rpos], 1, copy_from_end, state.callback_data.file_handle);
         fwrite(state.callback_data.cb_buff, 1, copy_from_start, state.callback_data.file_handle);
      }

error:

      mmal_status_to_int(status);

      if (state.verbose)
         fprintf(stderr, "Closing down\n");

      // Disable all our ports that are not handled by connections
      check_disable_port(camera_still_port);
      check_disable_port(encoder_output_port);

      if (state.preview_parameters.wantPreview && state.preview_connection)
         mmal_connection_destroy(state.preview_connection);

      if (state.encoder_connection)
         mmal_connection_destroy(state.encoder_connection);

      // Can now close our file. Note disabling ports may flush buffers which causes
      // problems if we have already closed the file!
      if (state.callback_data.file_handle && state.callback_data.file_handle != stdout)
         fclose(state.callback_data.file_handle);

      /* Disable components */
      if (state.encoder_component)
         mmal_component_disable(state.encoder_component);

      if (state.preview_parameters.preview_component)
         mmal_component_disable(state.preview_parameters.preview_component);

      if (state.camera_component)
         mmal_component_disable(state.camera_component);

      destroy_encoder_component(&state);
      raspipreview_destroy(&state.preview_parameters);
      destroy_camera_component(&state);

      if (state.verbose)
         fprintf(stderr, "Close down completed, all components disconnected, disabled and destroyed\n\n");
   }

   if (status != MMAL_SUCCESS)
      raspicamcontrol_check_configuration(128);

   return exit_code;
}
