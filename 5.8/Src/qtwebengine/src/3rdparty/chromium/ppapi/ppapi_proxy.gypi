# Copyright (c) 2012 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'target_defaults': {
    'variables': {
      'ppapi_proxy_target': 0,
    },
    'dependencies': [
      '<(DEPTH)/third_party/khronos/khronos.gyp:khronos_headers',
    ],
    'target_conditions': [
      # This part is shared between the targets defined below.
      ['ppapi_proxy_target==1', {
        'sources': [
          # Take some standalone files from the C++ wrapper allowing us to more
          # easily make async callbacks in the proxy. We can't depend on the
          # full C++ wrappers at this layer since the C++ wrappers expect
          # symbols defining the globals for "being a plugin" which we are not.
          # These callback files are standalone.
          'cpp/completion_callback.h',
          'utility/completion_callback_factory.h',

          'proxy/audio_buffer_resource.cc',
          'proxy/audio_buffer_resource.h',
          'proxy/audio_encoder_resource.cc',
          'proxy/audio_encoder_resource.h',
          'proxy/audio_input_resource.cc',
          'proxy/audio_input_resource.h',
          'proxy/broker_dispatcher.cc',
          'proxy/broker_dispatcher.h',
          'proxy/broker_resource.cc',
          'proxy/broker_resource.h',
          'proxy/browser_font_singleton_resource.cc',
          'proxy/browser_font_singleton_resource.h',
          'proxy/camera_capabilities_resource.cc',
          'proxy/camera_capabilities_resource.h',
          'proxy/camera_device_resource.cc',
          'proxy/camera_device_resource.h',
          'proxy/compositor_layer_resource.cc',
          'proxy/compositor_layer_resource.h',
          'proxy/compositor_resource.cc',
          'proxy/compositor_resource.h',
          'proxy/connection.h',
          'proxy/device_enumeration_resource_helper.cc',
          'proxy/device_enumeration_resource_helper.h',
          'proxy/dispatcher.cc',
          'proxy/dispatcher.h',
          'proxy/enter_proxy.h',
          'proxy/error_conversion.cc',
          'proxy/error_conversion.h',
          'proxy/file_chooser_resource.cc',
          'proxy/file_chooser_resource.h',
          'proxy/file_io_resource.cc',
          'proxy/file_io_resource.h',
          'proxy/file_ref_resource.cc',
          'proxy/file_ref_resource.h',
          'proxy/file_system_resource.cc',
          'proxy/file_system_resource.h',
          'proxy/flash_clipboard_resource.cc',
          'proxy/flash_clipboard_resource.h',
          'proxy/flash_drm_resource.cc',
          'proxy/flash_drm_resource.h',
          'proxy/flash_file_resource.cc',
          'proxy/flash_file_resource.h',
          'proxy/flash_font_file_resource.cc',
          'proxy/flash_font_file_resource.h',
          'proxy/flash_fullscreen_resource.cc',
          'proxy/flash_fullscreen_resource.h',
          'proxy/flash_menu_resource.cc',
          'proxy/flash_menu_resource.h',
          'proxy/flash_resource.cc',
          'proxy/flash_resource.h',
          'proxy/gamepad_resource.cc',
          'proxy/gamepad_resource.h',
          'proxy/graphics_2d_resource.cc',
          'proxy/graphics_2d_resource.h',
          'proxy/host_dispatcher.cc',
          'proxy/host_dispatcher.h',
          'proxy/host_resolver_private_resource.cc',
          'proxy/host_resolver_private_resource.h',
          'proxy/host_resolver_resource.cc',
          'proxy/host_resolver_resource.h',
          'proxy/host_resolver_resource_base.cc',
          'proxy/host_resolver_resource_base.h',
          'proxy/host_var_serialization_rules.cc',
          'proxy/host_var_serialization_rules.h',
          'proxy/interface_list.cc',
          'proxy/interface_list.h',
          'proxy/interface_proxy.cc',
          'proxy/interface_proxy.h',
          'proxy/isolated_file_system_private_resource.cc',
          'proxy/isolated_file_system_private_resource.h',
          'proxy/locking_resource_releaser.h',
          'proxy/media_stream_audio_track_resource.cc',
          'proxy/media_stream_audio_track_resource.h',
          'proxy/media_stream_track_resource_base.cc',
          'proxy/media_stream_track_resource_base.h',
          'proxy/media_stream_video_track_resource.cc',
          'proxy/media_stream_video_track_resource.h',
          'proxy/message_handler.cc',
          'proxy/message_handler.h',
          'proxy/net_address_resource.cc',
          'proxy/net_address_resource.h',
          'proxy/network_list_resource.cc',
          'proxy/network_list_resource.h',
          'proxy/network_monitor_resource.cc',
          'proxy/network_monitor_resource.h',
          'proxy/network_proxy_resource.cc',
          'proxy/network_proxy_resource.h',
          'proxy/output_protection_resource.cc',
          'proxy/output_protection_resource.h',
          'proxy/pdf_resource.cc',
          'proxy/pdf_resource.h',
          'proxy/platform_verification_private_resource.cc',
          'proxy/platform_verification_private_resource.h',
          'proxy/plugin_array_buffer_var.cc',
          'proxy/plugin_array_buffer_var.h',
          'proxy/plugin_dispatcher.cc',
          'proxy/plugin_dispatcher.h',
          'proxy/plugin_globals.cc',
          'proxy/plugin_globals.h',
          'proxy/plugin_message_filter.cc',
          'proxy/plugin_message_filter.h',
          'proxy/plugin_resource.cc',
          'proxy/plugin_resource.h',
          'proxy/plugin_resource_tracker.cc',
          'proxy/plugin_resource_tracker.h',
          'proxy/plugin_resource_var.cc',
          'proxy/plugin_resource_var.h',
          'proxy/plugin_var_serialization_rules.cc',
          'proxy/plugin_var_serialization_rules.h',
          'proxy/plugin_var_tracker.cc',
          'proxy/plugin_var_tracker.h',
          'proxy/ppapi_command_buffer_proxy.cc',
          'proxy/ppapi_command_buffer_proxy.h',
          'proxy/ppapi_message_utils.h',
          'proxy/ppapi_messages.h',
          'proxy/ppb_audio_proxy.cc',
          'proxy/ppb_audio_proxy.h',
          'proxy/ppb_broker_proxy.cc',
          'proxy/ppb_broker_proxy.h',
          'proxy/ppb_buffer_proxy.cc',
          'proxy/ppb_buffer_proxy.h',
          'proxy/ppb_core_proxy.cc',
          'proxy/ppb_core_proxy.h',
          'proxy/ppb_flash_message_loop_proxy.cc',
          'proxy/ppb_flash_message_loop_proxy.h',
          'proxy/ppb_graphics_3d_proxy.cc',
          'proxy/ppb_graphics_3d_proxy.h',
          'proxy/ppb_image_data_proxy.cc',
          'proxy/ppb_image_data_proxy.h',
          'proxy/ppb_instance_proxy.cc',
          'proxy/ppb_instance_proxy.h',
          'proxy/ppb_message_loop_proxy.cc',
          'proxy/ppb_message_loop_proxy.h',
          'proxy/ppb_testing_proxy.cc',
          'proxy/ppb_testing_proxy.h',
          'proxy/ppb_var_deprecated_proxy.cc',
          'proxy/ppb_var_deprecated_proxy.h',
          'proxy/ppb_video_decoder_proxy.cc',
          'proxy/ppb_video_decoder_proxy.h',
          'proxy/ppb_x509_certificate_private_proxy.cc',
          'proxy/ppb_x509_certificate_private_proxy.h',
          'proxy/ppp_class_proxy.cc',
          'proxy/ppp_class_proxy.h',
          'proxy/ppp_content_decryptor_private_proxy.cc',
          'proxy/ppp_content_decryptor_private_proxy.h',
          'proxy/ppp_find_proxy.cc',
          'proxy/ppp_find_proxy.h',
          'proxy/ppp_graphics_3d_proxy.cc',
          'proxy/ppp_graphics_3d_proxy.h',
          'proxy/ppp_input_event_proxy.cc',
          'proxy/ppp_input_event_proxy.h',
          'proxy/ppp_instance_private_proxy.cc',
          'proxy/ppp_instance_private_proxy.h',
          'proxy/ppp_instance_proxy.cc',
          'proxy/ppp_instance_proxy.h',
          'proxy/ppp_messaging_proxy.cc',
          'proxy/ppp_messaging_proxy.h',
          'proxy/ppp_mouse_lock_proxy.cc',
          'proxy/ppp_mouse_lock_proxy.h',
          'proxy/ppp_pdf_proxy.cc',
          'proxy/ppp_pdf_proxy.h',
          'proxy/ppp_printing_proxy.cc',
          'proxy/ppp_printing_proxy.h',
          'proxy/ppp_text_input_proxy.cc',
          'proxy/ppp_text_input_proxy.h',
          'proxy/ppp_video_decoder_proxy.cc',
          'proxy/ppp_video_decoder_proxy.h',
          'proxy/printing_resource.cc',
          'proxy/printing_resource.h',
          'proxy/proxy_array_output.cc',
          'proxy/proxy_array_output.h',
          'proxy/proxy_channel.cc',
          'proxy/proxy_channel.h',
          'proxy/proxy_completion_callback_factory.h',
          'proxy/proxy_module.cc',
          'proxy/proxy_module.h',
          'proxy/proxy_object_var.cc',
          'proxy/proxy_object_var.h',
          'proxy/resource_creation_proxy.cc',
          'proxy/resource_creation_proxy.h',
          'proxy/resource_message_filter.h',
          'proxy/resource_reply_thread_registrar.cc',
          'proxy/resource_reply_thread_registrar.h',
          'proxy/tcp_server_socket_private_resource.cc',
          'proxy/tcp_server_socket_private_resource.h',
          'proxy/tcp_socket_private_resource.cc',
          'proxy/tcp_socket_private_resource.h',
          'proxy/tcp_socket_resource.cc',
          'proxy/tcp_socket_resource.h',
          'proxy/tcp_socket_resource_base.cc',
          'proxy/tcp_socket_resource_base.h',
          'proxy/truetype_font_resource.cc',
          'proxy/truetype_font_resource.h',
          'proxy/truetype_font_singleton_resource.cc',
          'proxy/truetype_font_singleton_resource.h',
          'proxy/udp_socket_filter.cc',
          'proxy/udp_socket_filter.h',
          'proxy/udp_socket_private_resource.cc',
          'proxy/udp_socket_private_resource.h',
          'proxy/udp_socket_resource.cc',
          'proxy/udp_socket_resource.h',
          'proxy/udp_socket_resource_base.cc',
          'proxy/udp_socket_resource_base.h',
          'proxy/uma_private_resource.cc',
          'proxy/uma_private_resource.h',
          'proxy/url_loader_resource.cc',
          'proxy/url_loader_resource.h',
          'proxy/url_request_info_resource.cc',
          'proxy/url_request_info_resource.h',
          'proxy/url_response_info_resource.cc',
          'proxy/url_response_info_resource.h',
          'proxy/var_serialization_rules.h',
          'proxy/video_capture_resource.cc',
          'proxy/video_capture_resource.h',
          'proxy/video_decoder_resource.cc',
          'proxy/video_decoder_resource.h',
          'proxy/video_destination_resource.cc',
          'proxy/video_destination_resource.h',
          'proxy/video_encoder_resource.cc',
          'proxy/video_encoder_resource.h',
          'proxy/video_frame_resource.cc',
          'proxy/video_frame_resource.h',
          'proxy/video_source_resource.cc',
          'proxy/video_source_resource.h',
          'proxy/vpn_provider_resource.cc',
          'proxy/vpn_provider_resource.h',
          'proxy/websocket_resource.cc',
          'proxy/websocket_resource.h',
        ],
        'defines': [
          'PPAPI_PROXY_IMPLEMENTATION',
        ],
        'include_dirs': [
          '..',
        ],
        'target_conditions': [
          ['>(nacl_untrusted_build)==1', {
            'sources': [
              'nacl_irt/irt_interfaces.cc',
              'nacl_irt/irt_interfaces.h',
              'nacl_irt/irt_pnacl_translator_compile.cc',
              'nacl_irt/irt_pnacl_translator_link.cc',
              'nacl_irt/irt_ppapi.cc',
              'nacl_irt/irt_ppapi.h',
              'nacl_irt/irt_start.cc',
              'nacl_irt/manifest_service.cc',
              'nacl_irt/manifest_service.h',
              'nacl_irt/plugin_startup.cc',
              'nacl_irt/plugin_startup.h',
              'nacl_irt/ppapi_dispatcher.cc',
              'nacl_irt/ppapi_dispatcher.h',
            ],
            'sources!': [
              'proxy/audio_input_resource.cc',
              'proxy/broker_dispatcher.cc',
              'proxy/browser_font_singleton_resource.cc',
              'proxy/device_enumeration_resource_helper.cc',
              'proxy/flash_clipboard_resource.cc',
              'proxy/flash_drm_resource.cc',
              'proxy/flash_file_resource.cc',
              'proxy/flash_font_file_resource.cc',
              'proxy/flash_fullscreen_resource.cc',
              'proxy/flash_menu_resource.cc',
              'proxy/flash_resource.cc',
              'proxy/host_dispatcher.cc',
              'proxy/host_var_serialization_rules.cc',
              'proxy/pdf_resource.cc',
              'proxy/platform_verification_private_resource.cc',
              'proxy/platform_verification_private_resource.h',
              'proxy/ppb_broker_proxy.cc',
              'proxy/ppb_buffer_proxy.cc',
              'proxy/ppb_flash_message_loop_proxy.cc',
              'proxy/ppb_flash_proxy.cc',
              'proxy/ppb_pdf_proxy.cc',
              'proxy/ppb_var_deprecated_proxy.cc',
              'proxy/ppb_video_capture_proxy.cc',
              'proxy/ppb_video_decoder_proxy.cc',
              'proxy/ppp_content_decryptor_private_proxy.cc',
              'proxy/ppp_instance_private_proxy.cc',
              'proxy/ppp_video_decoder_proxy.cc',
              'proxy/serialized_flash_menu.cc',
              'proxy/video_capture_resource.cc',
            ],
          }],
        ],
      }],
    ],
  },
}
