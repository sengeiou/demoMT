/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "sys_init.h"
#include "wifi_nvdm_config.h"
#include "wifi_lwip_helper.h"
#if defined(MTK_MINICLI_ENABLE)
#include "cli_def.h"
#endif
#ifdef MTK_HOMEKIT_ENABLE
#include "homekit_init.h"
#endif

#include "bsp_gpio_ept_config.h"
#include "hal_sleep_manager.h"

#if defined(MTK_AUDIO_MP3_ENABLED)
#include "mp3_codec_ut.h"
#include "task_def.h"
#endif

#if defined(MTK_AUDIO_I2S_REC_ENABLED)
#include "i2s_rec.h"
#include "task_def.h"
#endif

#if defined(MTK_AUDIO_SPEEX_ENABLED)
#include "speex_codec_ut.h"
#include "speex_codec_demo.h"
#include "task_def.h"
#endif

#if defined(MTK_AUDIO_AMR_ENABLED)
#include "amr_decoder.h"
#include "amr_decoder_demo.h"
#include "amr_encoder.h"
#include "amr_encoder_demo.h"
#include "task_def.h"
#include "audip_exp.h"
IPCOMMON_PLUS
#endif

log_create_module(audio_ref_design, PRINT_LEVEL_INFO);
#define LOGE(fmt,arg...)   LOG_E(audio_ref_design, "[MAIN]: "fmt,##arg)
#define LOGW(fmt,arg...)   LOG_W(audio_ref_design, "[MAIN]: "fmt,##arg)
#define LOGI(fmt,arg...)   LOG_I(audio_ref_design, "[MAIN]: "fmt,##arg)

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
    /* Do system initialization, eg: hardware, nvdm, logging and random seed. */
    system_init();

    /* bsp_ept_gpio_setting_init() under driver/board/mt76x7_hdk/ept will initialize the GPIO settings
     * generated by easy pinmux tool (ept). ept_*.c and ept*.h are the ept files and will be used by
     * bsp_ept_gpio_setting_init() for GPIO pinumux setup.
     */
    bsp_ept_gpio_setting_init();

    //hal_audio_init();

//#if defined(MTK_MINICLI_ENABLE)
//    /* Initialize cli task to enable user input cli command from uart port.*/
//    cli_def_create();
//    cli_task_create();
//#endif

    LOGI("welcom to main\n");

#if defined(MTK_AUDIO_MP3_ENABLED)
        LOGI("mp3_codec_ut\n");
        xTaskCreate(mp3_codec_ut, MP3_PLAY_EXAMPLE_TASK_NAME, MP3_PLAY_EXAMPLE_TASK_STACKSIZE, NULL, MP3_PLAY_EXAMPLE_TASK_PRIO, NULL);
#endif

#if defined(MTK_AUDIO_I2S_REC_ENABLED)
      LOGI("i2s_rec_demo\n");
      xTaskCreate(i2s_rec_demo, I2S_REC_DEMO_TASK_NAME, I2S_REC_DEMO_TASK_STACKSIZE, NULL, I2S_REC_DEMO_TASK_PRIO, NULL);
#endif

#if defined(MTK_AUDIO_SPEEX_ENABLED)
      LOGI("SPEEX_CODEC_demo\n");
      //speex_en_demo();
      //speex_dec_demo();
      xTaskCreate(speex_codec_ut, SPEEX_CODEC_DEMO_NAME, SPEEX_CODEC_DEMO_STACKSIZE, NULL, SPEEX_CODEC_DEMO_PRIO, NULL);
#endif

#if defined(MTK_AUDIO_AMR_ENABLED)
      /*Demo :Encode pcm stream from sd card and output it to sd card*/
      //LOGI("AMR_encoder_demo\n");
      //xTaskCreate(amr_encoder_demo, AMR_ENCODER_DEMO_NAME, AMR_ENCODER_DEMO_STACKSIZE, NULL, AMR_ENCODER_DEMO_PRIO, NULL);

      /*Demo :Decoder amr stream from sd card and output it to nau8810*/
      LOGI("AMR_decoder_demo\n");
      xTaskCreate(amr_decoder_demo, AMR_DECODER_DEMO_NAME, AMR_DECODER_DEMO_STACKSIZE, NULL, AMR_DECODER_DEMO_PRIO, NULL);
#endif
    LOGI("vTaskStartScheduler\n");

    /* Call this function to indicate the system initialize done. */
    SysInitStatus_Set();


    vTaskStartScheduler();

    /* If all is well, the scheduler will now be running, and the following line
    will never be reached.  If the following line does execute, then there was
    insufficient FreeRTOS heap memory available for the idle and/or timer tasks
    to be created.  See the memory management section on the FreeRTOS web site
    for more details. */
    for ( ;; );
}

