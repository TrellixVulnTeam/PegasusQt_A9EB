# Copyright (c) 2012 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'variables': {
    'use_system_libwebp%': 0,
  },
  'target_defaults': {
    'conditions': [
      ['os_posix==1 and (target_arch=="arm" or target_arch=="arm64")', {
        'cflags!': [ '-Os' ],
        'cflags': [ '-O2' ],
      }],
    ],
  },
  'conditions' : [
    ['use_system_libwebp == 0', {
  'targets': [
    {
      'target_name': 'libwebp_dec',
      'type': 'static_library',
      'dependencies' : [
        'libwebp_dsp',
        'libwebp_dsp_neon',
        'libwebp_utils',
      ],
      'include_dirs': ['.'],
      'sources': [
        'dec/alpha.c',
        'dec/buffer.c',
        'dec/frame.c',
        'dec/idec.c',
        'dec/io.c',
        'dec/quant.c',
        'dec/tree.c',
        'dec/vp8.c',
        'dec/vp8l.c',
        'dec/webp.c',
      ],
    },
    {
      'target_name': 'libwebp_demux',
      'type': 'static_library',
      'include_dirs': ['.'],
      'sources': [
        'demux/demux.c',
      ],
      'dependencies' : [
        'libwebp_utils',
      ],
    },
    {
      'target_name': 'libwebp_dsp',
      'type': 'static_library',
      'include_dirs': ['.'],
      'sources': [
        'dsp/alpha_processing.c',
        'dsp/alpha_processing_mips_dsp_r2.c',
        'dsp/argb.c',
        'dsp/argb_mips_dsp_r2.c',
        'dsp/cost.c',
        'dsp/cost_mips32.c',
        'dsp/cost_mips_dsp_r2.c',
        'dsp/cpu.c',
        'dsp/dec.c',
        'dsp/dec_clip_tables.c',
        'dsp/dec_mips32.c',
        'dsp/dec_mips_dsp_r2.c',
        'dsp/enc.c',
        'dsp/enc_avx2.c',
        'dsp/enc_mips32.c',
        'dsp/enc_mips_dsp_r2.c',
        'dsp/filters.c',
        'dsp/filters_mips_dsp_r2.c',
        'dsp/lossless.c',
        'dsp/lossless_enc.c',
        'dsp/lossless_enc_mips32.c',
        'dsp/lossless_enc_mips_dsp_r2.c',
        'dsp/lossless_mips_dsp_r2.c',
        'dsp/rescaler.c',
        'dsp/rescaler_mips32.c',
        'dsp/rescaler_mips_dsp_r2.c',
        'dsp/upsampling.c',
        'dsp/upsampling_mips_dsp_r2.c',
        'dsp/yuv.c',
        'dsp/yuv_mips32.c',
        'dsp/yuv_mips_dsp_r2.c',
      ],
      'dependencies' : [
        'libwebp_dsp_sse2',
        'libwebp_dsp_sse41',
        'libwebp_utils',
      ],
      'conditions': [
        ['OS == "android"', {
          'dependencies': [ '../../build/android/ndk.gyp:cpu_features' ],
        }],
        # iOS uses the same project to generate build project for both device
        # and simulator and do not use "target_arch" variable. Other platform
        # set it correctly.
        ['OS!="ios" and (target_arch=="ia32" or target_arch=="x64")', {
          'defines': [ 'WEBP_HAVE_SSE2', 'WEBP_HAVE_SSE41' ],
        }],
      ],
    },
    {
      'target_name': 'libwebp_dsp_sse2',
      'type': 'static_library',
      'include_dirs': ['.'],
      'sources': [
        'dsp/alpha_processing_sse2.c',
        'dsp/argb_sse2.c',
        'dsp/cost_sse2.c',
        'dsp/dec_sse2.c',
        'dsp/enc_sse2.c',
        'dsp/filters_sse2.c',
        'dsp/lossless_enc_sse2.c',
        'dsp/lossless_sse2.c',
        'dsp/rescaler_sse2.c',
        'dsp/upsampling_sse2.c',
        'dsp/yuv_sse2.c',
      ],
      'conditions': [
        # iOS uses the same project to generate build project for both device
        # and simulator and do not use "target_arch" variable. Other platform
        # set it correctly.
        ['OS!="ios" and (target_arch=="ia32" or target_arch=="x64") and msan==0', {
          'cflags': [ '-msse2', ],
          'xcode_settings': { 'OTHER_CFLAGS': [ '-msse2' ] },
        }],
      ],
    },
    {
      'target_name': 'libwebp_dsp_sse41',
      'type': 'static_library',
      'include_dirs': ['.'],
      'sources': [
        'dsp/alpha_processing_sse41.c',
        'dsp/dec_sse41.c',
        'dsp/enc_sse41.c',
        'dsp/lossless_enc_sse41.c',
      ],
      'conditions': [
        ['OS=="win" and clang==1', {
          # cl.exe's /arch flag doesn't have a setting for SSSE3/4, and cl.exe
          # doesn't need it for intrinsics. clang-cl does need it, though.
          'msvs_settings': {
            'VCCLCompilerTool': { 'AdditionalOptions': [ '-msse4.1' ] },
          },
        }],
        # iOS uses the same project to generate build project for both device
        # and simulator and do not use "target_arch" variable. Other platform
        # set it correctly.
        ['OS!="ios" and (target_arch=="ia32" or target_arch=="x64") and msan==0', {
          'cflags': [ '-msse4.1', ],
          'xcode_settings': { 'OTHER_CFLAGS': [ '-msse4.1' ] },
        }],
      ],
    },
    {
      'target_name': 'libwebp_dsp_neon',
      'includes' : [
        # Disable LTO due to Neon issues.
        # crbug.com/408997
        '../../build/android/disable_gcc_lto.gypi',
      ],
      'conditions': [
        # iOS uses the same project to generate build project for both device
        # and simulator and do not use "target_arch" variable. Other platform
        # set it correctly.
        ['OS == "ios" or (target_arch == "arm" and arm_version >= 7 and (arm_neon == 1 or arm_neon_optional == 1)) or (target_arch == "arm64")', {
          'type': 'static_library',
          'include_dirs': ['.'],
          'sources': [
            'dsp/dec_neon.c',
            'dsp/enc_neon.c',
            'dsp/lossless_enc_neon.c',
            'dsp/lossless_neon.c',
            'dsp/rescaler_neon.c',
            'dsp/upsampling_neon.c',
          ],
          'conditions': [
            ['target_arch == "arm" and arm_version >= 7 and (arm_neon == 1 or arm_neon_optional == 1)', {
              # behavior similar to *.c.neon in an Android.mk
              'cflags!': [ '-mfpu=vfpv3-d16' ],
              'cflags': [ '-mfpu=neon' ],
            }],
            ['target_arch == "arm64" and clang != 1', {
              # avoid an ICE with gcc-4.9: b/15574841
              'cflags': [ '-frename-registers' ],
            }],
          ]
        }, {
          'type': 'none',
        }],
      ],
    },
    {
      'target_name': 'libwebp_enc',
      'type': 'static_library',
      'include_dirs': ['.'],
      'sources': [
        'enc/alpha.c',
        'enc/analysis.c',
        'enc/backward_references.c',
        'enc/config.c',
        'enc/cost.c',
        'enc/delta_palettization.c',
        'enc/filter.c',
        'enc/frame.c',
        'enc/histogram.c',
        'enc/iterator.c',
        'enc/near_lossless.c',
        'enc/picture.c',
        'enc/picture_csp.c',
        'enc/picture_psnr.c',
        'enc/picture_rescale.c',
        'enc/picture_tools.c',
        'enc/quant.c',
        'enc/syntax.c',
        'enc/token.c',
        'enc/tree.c',
        'enc/vp8l.c',
        'enc/webpenc.c',
      ],
      'dependencies' : [
        'libwebp_utils',
      ],
    },
    {
      'target_name': 'libwebp_utils',
      'type': 'static_library',
      'include_dirs': ['.'],
      'sources': [
        'utils/bit_reader.c',
        'utils/bit_writer.c',
        'utils/color_cache.c',
        'utils/filters.c',
        'utils/huffman.c',
        'utils/huffman_encode.c',
        'utils/quant_levels.c',
        'utils/quant_levels_dec.c',
        'utils/random.c',
        'utils/rescaler.c',
        'utils/thread.c',
        'utils/utils.c',
      ],
      'variables': {
        'clang_warning_flags': [
          # See https://code.google.com/p/webp/issues/detail?id=253.
          '-Wno-incompatible-pointer-types',
        ]
      },
    },
    {
      'target_name': 'libwebp',
      'type': 'none',
      'dependencies' : [
        'libwebp_dec',
        'libwebp_demux',
        'libwebp_dsp',
        'libwebp_dsp_neon',
        'libwebp_enc',
        'libwebp_utils',
      ],
      'direct_dependent_settings': {
        'include_dirs': ['.'],
      },
      'conditions': [
        ['OS!="win"', {'product_name': 'webp'}],
      ],
    },
  ],
  }, #  'use_system_libwebp == 0'
  {
  'targets': [
    {
      'target_name': 'libwebp',
      'type': 'none',
      'direct_dependent_settings': {
        'defines': [
          'ENABLE_WEBP',
        ],
        'cflags': [
          '<!@(<(pkg-config) --cflags libwebp libwebpdemux)',
        ],
      },
      'link_settings': {
        'ldflags': [
          '<!@(<(pkg-config) --libs-only-L --libs-only-other libwebp libwebpdemux)',
        ],
        'libraries': [
          '<!@(<(pkg-config) --libs-only-l libwebp libwebpdemux)',
        ],
      },
    }
  ],
  }
  ]]
}
