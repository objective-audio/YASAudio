//
//  yas_audio.h
//

#pragma once

#include "yas_audio_types.h"
#include "yas_stl_utils.h"
#include "yas_cf_utils.h"
#include "yas_observing.h"
#include "yas_property.h"
#include "yas_exception.h"
#include "yas_result.h"
#include "yas_audio_math.h"
#include "yas_flex_ptr.h"
#include "yas_operation.h"

#include "yas_audio_time.h"
#include "yas_audio_format.h"
#include "yas_audio_pcm_buffer.h"
#include "yas_audio_enumerator.h"
#include "yas_audio_file.h"
#include "yas_audio_file_utils.h"
#include "yas_audio_exception.h"

#if (TARGET_OS_MAC && !TARGET_OS_IPHONE)

#include "yas_audio_route.h"
#include "yas_audio_device.h"
#include "yas_audio_device_stream.h"
#include "yas_audio_device_io.h"
#include "yas_audio_device_io_node.h"

#endif

#include "yas_audio_graph.h"
#include "yas_audio_unit.h"
#include "yas_audio_unit_parameter.h"

#include "yas_audio_engine.h"
#include "yas_audio_node.h"
#include "yas_audio_unit_node.h"
#include "yas_audio_unit_mixer_node.h"
#include "yas_audio_unit_io_node.h"
#include "yas_audio_offline_output_node.h"
#include "yas_audio_tap_node.h"
#include "yas_audio_connection.h"
#include "yas_audio_route_node.h"