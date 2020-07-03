/*  RetroArch - A frontend for libretro.
 *  Copyright (C) 2011-2017 - Daniel De Matteis
 *
 *  RetroArch is free software: you can redistribute it and/or modify it under the terms
 *  of the GNU General Public License as published by the Free Software Found-
 *  ation, either version 3 of the License, or (at your option) any later version.
 *
 *  RetroArch is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *  PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with RetroArch.
 *  If not, see <http://www.gnu.org/licenses/>.
 */

#include <compat/strl.h>

#include <string.h>
#include <string/stdstring.h>
#include <file/file_path.h>

#include "../menu_driver.h"
#include "../menu_cbs.h"

#include "../../retroarch.h"
#include "../../managers/core_option_manager.h"

#ifdef HAVE_CHEEVOS
#include "../../cheevos/cheevos.h"
#endif
#include "../../core_info.h"
#include "../../verbosity.h"
#include "../../bluetooth/bluetooth_driver.h"

#ifdef HAVE_NETWORKING
#include "../../network/netplay/netplay.h"
#include "../../network/netplay/netplay_discovery.h"
#endif

#include "../../retroarch.h"
#include "../../content.h"
#include "../../dynamic.h"
#include "../../configuration.h"
#ifdef HAVE_NETWORKING
#include "../../core_updater_list.h"
#endif
#ifdef HAVE_CHEATS
#include "../../managers/cheat_manager.h"
#endif
#include "../../tasks/tasks_internal.h"

#include "../../playlist.h"
#include "../../runtime_file.h"

#ifndef BIND_ACTION_SUBLABEL
#define BIND_ACTION_SUBLABEL(cbs, name) (cbs)->action_sublabel = (name)
#endif

#define DEFAULT_SUBLABEL_MACRO(func_name, lbl) \
  static int (func_name)(file_list_t *list, unsigned type, unsigned i, const char *label, const char *path, char *s, size_t len) \
{ \
   strlcpy(s, msg_hash_to_str(lbl), len); \
   return 1; \
}

static int menu_action_sublabel_file_browser_core(file_list_t *list, unsigned type, unsigned i, const char *label, const char *path, char *s, size_t len)
{
   core_info_ctx_find_t core_info;

   /* Set sublabel prefix */
   strlcpy(s, msg_hash_to_str(MENU_ENUM_LABEL_VALUE_CORE_INFO_LICENSES), len);
   strlcat(s, ": ", len);

   /* Search for specified core */
   core_info.inf  = NULL;
   core_info.path = path;

   if (core_info_find(&core_info) &&
       core_info.inf->licenses_list)
   {
      char tmp[MENU_SUBLABEL_MAX_LENGTH];
      tmp[0]  = '\0';

      /* Add license text */
      string_list_join_concat(tmp, sizeof(tmp),
            core_info.inf->licenses_list, ", ");
      strlcat(s, tmp, len);
      return 1;
   }

   /* No license found - set to N/A */
   strlcat(s, msg_hash_to_str(MENU_ENUM_LABEL_VALUE_NOT_AVAILABLE), len);
   return 1;
}

#ifdef HAVE_AUDIOMIXER
DEFAULT_SUBLABEL_MACRO(menu_action_sublabel_setting_audio_mixer_add_to_mixer_and_play,
      MENU_ENUM_SUBLABEL_ADD_TO_MIXER_AND_PLAY)
DEFAULT_SUBLABEL_MACRO(menu_action_sublabel_setting_audio_mixer_add_to_mixer,
      MENU_ENUM_SUBLABEL_ADD_TO_MIXER)
DEFAULT_SUBLABEL_MACRO(menu_action_sublabel_setting_audio_mixer_stream_play,
      MENU_ENUM_SUBLABEL_MIXER_ACTION_PLAY)
DEFAULT_SUBLABEL_MACRO(menu_action_sublabel_setting_audio_mixer_stream_play_looped,
      MENU_ENUM_SUBLABEL_MIXER_ACTION_PLAY_LOOPED)
DEFAULT_SUBLABEL_MACRO(menu_action_sublabel_setting_audio_mixer_stream_play_sequential,
      MENU_ENUM_SUBLABEL_MIXER_ACTION_PLAY_SEQUENTIAL)
DEFAULT_SUBLABEL_MACRO(menu_action_sublabel_setting_audio_mixer_stream_stop,
      MENU_ENUM_SUBLABEL_MIXER_ACTION_STOP)
DEFAULT_SUBLABEL_MACRO(menu_action_sublabel_setting_audio_mixer_stream_remove,
      MENU_ENUM_SUBLABEL_MIXER_ACTION_REMOVE)
DEFAULT_SUBLABEL_MACRO(menu_action_sublabel_setting_audio_mixer_stream_volume,
      MENU_ENUM_SUBLABEL_MIXER_ACTION_VOLUME)
#endif
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_accessibility_enabled, MENU_ENUM_SUBLABEL_ACCESSIBILITY_ENABLED
         )
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_accessibility_narrator_speech_speed, MENU_ENUM_SUBLABEL_ACCESSIBILITY_NARRATOR_SPEECH_SPEED
         )
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_reset_to_default_config,             MENU_ENUM_SUBLABEL_RESET_TO_DEFAULT_CONFIG)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_quick_menu_override_options,             MENU_ENUM_SUBLABEL_QUICK_MENU_OVERRIDE_OPTIONS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_quick_menu_start_streaming,             MENU_ENUM_SUBLABEL_QUICK_MENU_START_STREAMING)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_quick_menu_start_recording,             MENU_ENUM_SUBLABEL_QUICK_MENU_START_RECORDING)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_quick_menu_stop_streaming,             MENU_ENUM_SUBLABEL_QUICK_MENU_STOP_STREAMING)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_quick_menu_stop_recording,             MENU_ENUM_SUBLABEL_QUICK_MENU_STOP_RECORDING)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_crt_switchres,             MENU_ENUM_SUBLABEL_CRT_SWITCH_RESOLUTION)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_crt_switchres_super,       MENU_ENUM_SUBLABEL_CRT_SWITCH_RESOLUTION_SUPER)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_crt_switchres_x_axis_centering,       MENU_ENUM_SUBLABEL_CRT_SWITCH_X_AXIS_CENTERING)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_crt_switchres_use_custom_refresh_rate,       MENU_ENUM_SUBLABEL_CRT_SWITCH_RESOLUTION_USE_CUSTOM_REFRESH_RATE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_driver_settings_list,             MENU_ENUM_SUBLABEL_DRIVER_SETTINGS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_retro_achievements_settings_list, MENU_ENUM_SUBLABEL_RETRO_ACHIEVEMENTS_SETTINGS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_saving_settings_list,          MENU_ENUM_SUBLABEL_SAVING_SETTINGS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_logging_settings_list,         MENU_ENUM_SUBLABEL_LOGGING_SETTINGS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_user_interface_settings_list,  MENU_ENUM_SUBLABEL_USER_INTERFACE_SETTINGS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_ai_service_settings_list,  MENU_ENUM_SUBLABEL_AI_SERVICE_SETTINGS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_accessibility_settings_list,  MENU_ENUM_SUBLABEL_ACCESSIBILITY_SETTINGS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_ai_service_mode,  MENU_ENUM_SUBLABEL_AI_SERVICE_MODE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_ai_service_pause,  MENU_ENUM_SUBLABEL_AI_SERVICE_PAUSE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_ai_service_target_lang,  MENU_ENUM_SUBLABEL_AI_SERVICE_TARGET_LANG)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_ai_service_source_lang,  MENU_ENUM_SUBLABEL_AI_SERVICE_SOURCE_LANG)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_ai_service_url,  MENU_ENUM_SUBLABEL_AI_SERVICE_URL)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_ai_service_enable,  MENU_ENUM_SUBLABEL_AI_SERVICE_ENABLE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_power_management_settings_list,  MENU_ENUM_SUBLABEL_POWER_MANAGEMENT_SETTINGS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_privacy_settings_list,         MENU_ENUM_SUBLABEL_PRIVACY_SETTINGS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_midi_settings_list,            MENU_ENUM_SUBLABEL_MIDI_SETTINGS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_directory_settings_list,       MENU_ENUM_SUBLABEL_DIRECTORY_SETTINGS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_playlist_settings_list,        MENU_ENUM_SUBLABEL_PLAYLIST_SETTINGS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_playlist_manager_list,         MENU_ENUM_SUBLABEL_PLAYLIST_MANAGER_LIST)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_playlist_manager_default_core, MENU_ENUM_SUBLABEL_PLAYLIST_MANAGER_DEFAULT_CORE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_playlist_manager_reset_cores,  MENU_ENUM_SUBLABEL_PLAYLIST_MANAGER_RESET_CORES)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_playlist_manager_label_display_mode, MENU_ENUM_SUBLABEL_PLAYLIST_MANAGER_LABEL_DISPLAY_MODE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_playlist_manager_sort_mode, MENU_ENUM_SUBLABEL_PLAYLIST_MANAGER_SORT_MODE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_playlist_manager_clean_playlist, MENU_ENUM_SUBLABEL_PLAYLIST_MANAGER_CLEAN_PLAYLIST)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_delete_playlist,               MENU_ENUM_SUBLABEL_DELETE_PLAYLIST)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_network_settings_list,         MENU_ENUM_SUBLABEL_NETWORK_SETTINGS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_network_on_demand_thumbnails,  MENU_ENUM_SUBLABEL_NETWORK_ON_DEMAND_THUMBNAILS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_user_settings_list,            MENU_ENUM_SUBLABEL_USER_SETTINGS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_recording_settings_list,       MENU_ENUM_SUBLABEL_RECORDING_SETTINGS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_frame_throttle_settings_list,  MENU_ENUM_SUBLABEL_FRAME_THROTTLE_SETTINGS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_frame_time_counter_settings_list,  MENU_ENUM_SUBLABEL_FRAME_TIME_COUNTER_SETTINGS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_frame_time_counter_reset_after_fastforwarding,  MENU_ENUM_SUBLABEL_FRAME_TIME_COUNTER_RESET_AFTER_FASTFORWARDING)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_frame_time_counter_reset_after_load_state,  MENU_ENUM_SUBLABEL_FRAME_TIME_COUNTER_RESET_AFTER_LOAD_STATE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_frame_time_counter_reset_after_save_state,  MENU_ENUM_SUBLABEL_FRAME_TIME_COUNTER_RESET_AFTER_SAVE_STATE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_onscreen_display_settings_list,MENU_ENUM_SUBLABEL_ONSCREEN_DISPLAY_SETTINGS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_core_settings_list,            MENU_ENUM_SUBLABEL_CORE_SETTINGS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_information_list_list,         MENU_ENUM_SUBLABEL_INFORMATION_LIST_LIST)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_cheevos_enable,                MENU_ENUM_SUBLABEL_CHEEVOS_ENABLE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_cheevos_test_unofficial,       MENU_ENUM_SUBLABEL_CHEEVOS_TEST_UNOFFICIAL)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_cheevos_hardcore_mode_enable,  MENU_ENUM_SUBLABEL_CHEEVOS_HARDCORE_MODE_ENABLE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_cheevos_leaderboards_enable,   MENU_ENUM_SUBLABEL_CHEEVOS_LEADERBOARDS_ENABLE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_cheevos_richpresence_enable,   MENU_ENUM_SUBLABEL_CHEEVOS_RICHPRESENCE_ENABLE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_cheevos_badges_enable,         MENU_ENUM_SUBLABEL_CHEEVOS_BADGES_ENABLE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_cheevos_verbose_enable,        MENU_ENUM_SUBLABEL_CHEEVOS_VERBOSE_ENABLE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_cheevos_auto_screenshot,       MENU_ENUM_SUBLABEL_CHEEVOS_AUTO_SCREENSHOT)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_cheevos_start_active,          MENU_ENUM_SUBLABEL_CHEEVOS_START_ACTIVE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_views_settings_list,      MENU_ENUM_SUBLABEL_MENU_VIEWS_SETTINGS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_quick_menu_views_settings_list, MENU_ENUM_SUBLABEL_QUICK_MENU_VIEWS_SETTINGS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_settings_views_settings_list, MENU_ENUM_SUBLABEL_SETTINGS_VIEWS_SETTINGS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_settings_list,            MENU_ENUM_SUBLABEL_MENU_SETTINGS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_video_settings_list,           MENU_ENUM_SUBLABEL_VIDEO_SETTINGS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_video_synchronization_settings_list,           MENU_ENUM_SUBLABEL_VIDEO_SYNCHRONIZATION_SETTINGS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_video_fullscreen_mode_settings_list,           MENU_ENUM_SUBLABEL_VIDEO_FULLSCREEN_MODE_SETTINGS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_video_windowed_mode_settings_list,           MENU_ENUM_SUBLABEL_VIDEO_WINDOWED_MODE_SETTINGS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_video_scaling_settings_list,           MENU_ENUM_SUBLABEL_VIDEO_SCALING_SETTINGS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_video_output_settings_list,           MENU_ENUM_SUBLABEL_VIDEO_OUTPUT_SETTINGS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_crt_switchres_settings_list,           MENU_ENUM_SUBLABEL_CRT_SWITCHRES_SETTINGS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_suspend_screensaver_enable,    MENU_ENUM_SUBLABEL_SUSPEND_SCREENSAVER_ENABLE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_video_window_scale,            MENU_ENUM_SUBLABEL_VIDEO_WINDOW_SCALE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_audio_settings_list,           MENU_ENUM_SUBLABEL_AUDIO_SETTINGS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_audio_resampler_settings_list,           MENU_ENUM_SUBLABEL_AUDIO_RESAMPLER_SETTINGS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_audio_output_settings_list,           MENU_ENUM_SUBLABEL_AUDIO_OUTPUT_SETTINGS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_audio_synchronization_settings_list,           MENU_ENUM_SUBLABEL_AUDIO_SYNCHRONIZATION_SETTINGS)
#ifdef HAVE_AUDIOMIXER
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_mixer_settings_list,           MENU_ENUM_SUBLABEL_AUDIO_MIXER_SETTINGS)
#endif
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_settings_list,           MENU_ENUM_SUBLABEL_INPUT_SETTINGS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_menu_settings_list,           MENU_ENUM_SUBLABEL_INPUT_MENU_SETTINGS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_haptic_feedback_settings_list,           MENU_ENUM_SUBLABEL_INPUT_HAPTIC_FEEDBACK_SETTINGS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_latency_settings_list,         MENU_ENUM_SUBLABEL_LATENCY_SETTINGS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_bluetooth_settings_list,       MENU_ENUM_SUBLABEL_BLUETOOTH_SETTINGS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_wifi_settings_list,            MENU_ENUM_SUBLABEL_WIFI_SETTINGS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_netplay_lan_scan_settings_list,MENU_ENUM_SUBLABEL_NETPLAY_LAN_SCAN_SETTINGS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_help_list,                     MENU_ENUM_SUBLABEL_HELP_LIST)
#ifdef HAVE_LAKKA
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_services_settings_list,        MENU_ENUM_SUBLABEL_SERVICES_SETTINGS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_ssh_enable,                    MENU_ENUM_SUBLABEL_SSH_ENABLE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_samba_enable,                  MENU_ENUM_SUBLABEL_SAMBA_ENABLE )
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_bluetooth_enable,              MENU_ENUM_SUBLABEL_BLUETOOTH_ENABLE )
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_localap_enable,                MENU_ENUM_SUBLABEL_LOCALAP_ENABLE )
#endif
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_user_language,                 MENU_ENUM_SUBLABEL_USER_LANGUAGE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_max_swapchain_images,          MENU_ENUM_SUBLABEL_VIDEO_MAX_SWAPCHAIN_IMAGES )
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_online_updater,                MENU_ENUM_SUBLABEL_ONLINE_UPDATER)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_fps_show,                      MENU_ENUM_SUBLABEL_FPS_SHOW)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_fps_update_interval,                      MENU_ENUM_SUBLABEL_FPS_UPDATE_INTERVAL)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_framecount_show,               MENU_ENUM_SUBLABEL_FRAMECOUNT_SHOW)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_memory_show,                   MENU_ENUM_SUBLABEL_MEMORY_SHOW)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_statistics_show,               MENU_ENUM_SUBLABEL_STATISTICS_SHOW)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_netplay_settings,              MENU_ENUM_SUBLABEL_NETPLAY)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_user_bind_settings,            MENU_ENUM_SUBLABEL_INPUT_USER_BINDS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_hotkey_settings,         MENU_ENUM_SUBLABEL_INPUT_HOTKEY_BINDS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_meta_fast_forward_key,      MENU_ENUM_SUBLABEL_INPUT_META_FAST_FORWARD_KEY)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_meta_fast_forward_hold_key, MENU_ENUM_SUBLABEL_INPUT_META_FAST_FORWARD_HOLD_KEY)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_meta_slowmotion_key,        MENU_ENUM_SUBLABEL_INPUT_META_SLOWMOTION_KEY)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_meta_slowmotion_hold_key,   MENU_ENUM_SUBLABEL_INPUT_META_SLOWMOTION_HOLD_KEY)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_meta_load_state_key,        MENU_ENUM_SUBLABEL_INPUT_META_LOAD_STATE_KEY)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_meta_save_state_key,        MENU_ENUM_SUBLABEL_INPUT_META_SAVE_STATE_KEY)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_meta_fullscreen_toggle_key, MENU_ENUM_SUBLABEL_INPUT_META_FULLSCREEN_TOGGLE_KEY)
#ifdef HAVE_LAKKA
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_meta_restart_key,           MENU_ENUM_SUBLABEL_INPUT_META_RESTART_KEY)
#else
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_meta_quit_key,              MENU_ENUM_SUBLABEL_INPUT_META_QUIT_KEY)
#endif
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_meta_state_slot_plus,       MENU_ENUM_SUBLABEL_INPUT_META_STATE_SLOT_PLUS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_meta_state_slot_minus,      MENU_ENUM_SUBLABEL_INPUT_META_STATE_SLOT_MINUS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_meta_rewind_hotkey,         MENU_ENUM_SUBLABEL_INPUT_META_REWIND_HOTKEY)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_meta_bsv_record_toggle,     MENU_ENUM_SUBLABEL_INPUT_META_BSV_RECORD_TOGGLE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_meta_pause_toggle,          MENU_ENUM_SUBLABEL_INPUT_META_PAUSE_TOGGLE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_meta_frameadvance,          MENU_ENUM_SUBLABEL_INPUT_META_FRAMEADVANCE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_meta_reset,                 MENU_ENUM_SUBLABEL_INPUT_META_RESET)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_meta_shader_next,           MENU_ENUM_SUBLABEL_INPUT_META_SHADER_NEXT)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_meta_shader_prev,           MENU_ENUM_SUBLABEL_INPUT_META_SHADER_PREV)
#ifdef HAVE_CHEATS
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_meta_cheat_index_plus,      MENU_ENUM_SUBLABEL_INPUT_META_CHEAT_INDEX_PLUS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_meta_cheat_index_minus,     MENU_ENUM_SUBLABEL_INPUT_META_CHEAT_INDEX_MINUS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_meta_cheat_toggle,          MENU_ENUM_SUBLABEL_INPUT_META_CHEAT_TOGGLE)
#endif
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_meta_screenshot,            MENU_ENUM_SUBLABEL_INPUT_META_SCREENSHOT)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_meta_mute,                  MENU_ENUM_SUBLABEL_INPUT_META_MUTE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_meta_osk,                   MENU_ENUM_SUBLABEL_INPUT_META_OSK)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_meta_fps_toggle,            MENU_ENUM_SUBLABEL_INPUT_META_FPS_TOGGLE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_meta_send_debug_info,       MENU_ENUM_SUBLABEL_INPUT_META_SEND_DEBUG_INFO)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_meta_netplay_host_toggle,   MENU_ENUM_SUBLABEL_INPUT_META_NETPLAY_HOST_TOGGLE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_meta_netplay_game_watch,    MENU_ENUM_SUBLABEL_INPUT_META_NETPLAY_GAME_WATCH)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_meta_enable_hotkey,         MENU_ENUM_SUBLABEL_INPUT_META_ENABLE_HOTKEY)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_meta_volume_up,             MENU_ENUM_SUBLABEL_INPUT_META_VOLUME_UP)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_meta_volume_down,           MENU_ENUM_SUBLABEL_INPUT_META_VOLUME_DOWN)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_meta_overlay_next,          MENU_ENUM_SUBLABEL_INPUT_META_OVERLAY_NEXT)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_meta_disk_eject_toggle,     MENU_ENUM_SUBLABEL_INPUT_META_DISK_EJECT_TOGGLE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_meta_disk_next,             MENU_ENUM_SUBLABEL_INPUT_META_DISK_NEXT)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_meta_disk_prev,             MENU_ENUM_SUBLABEL_INPUT_META_DISK_PREV)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_meta_grab_mouse_toggle,     MENU_ENUM_SUBLABEL_INPUT_META_GRAB_MOUSE_TOGGLE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_meta_game_focus_toggle,     MENU_ENUM_SUBLABEL_INPUT_META_GAME_FOCUS_TOGGLE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_meta_ui_companion_toggle,   MENU_ENUM_SUBLABEL_INPUT_META_UI_COMPANION_TOGGLE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_meta_recording_toggle,      MENU_ENUM_SUBLABEL_INPUT_META_RECORDING_TOGGLE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_meta_streaming_toggle,      MENU_ENUM_SUBLABEL_INPUT_META_STREAMING_TOGGLE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_meta_ai_service,            MENU_ENUM_SUBLABEL_INPUT_META_AI_SERVICE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_meta_menu_toggle,           MENU_ENUM_SUBLABEL_INPUT_META_MENU_TOGGLE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_hotkey_block_delay,         MENU_ENUM_SUBLABEL_INPUT_HOTKEY_BLOCK_DELAY)
#ifdef HAVE_MATERIALUI
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_materialui_icons_enable,       MENU_ENUM_SUBLABEL_MATERIALUI_ICONS_ENABLE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_materialui_landscape_layout_optimization, MENU_ENUM_SUBLABEL_MATERIALUI_LANDSCAPE_LAYOUT_OPTIMIZATION)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_materialui_show_nav_bar,        MENU_ENUM_SUBLABEL_MATERIALUI_SHOW_NAV_BAR)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_materialui_auto_rotate_nav_bar, MENU_ENUM_SUBLABEL_MATERIALUI_AUTO_ROTATE_NAV_BAR)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_materialui_dual_thumbnail_list_view_enable, MENU_ENUM_SUBLABEL_MATERIALUI_DUAL_THUMBNAIL_LIST_VIEW_ENABLE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_materialui_thumbnail_background_enable, MENU_ENUM_SUBLABEL_MATERIALUI_THUMBNAIL_BACKGROUND_ENABLE)
#endif
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_add_content_list,              MENU_ENUM_SUBLABEL_ADD_CONTENT_LIST)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_video_frame_delay,             MENU_ENUM_SUBLABEL_VIDEO_FRAME_DELAY)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_video_shader_delay,            MENU_ENUM_SUBLABEL_VIDEO_SHADER_DELAY)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_video_black_frame_insertion,   MENU_ENUM_SUBLABEL_VIDEO_BLACK_FRAME_INSERTION)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_systeminfo_cpu_cores,          MENU_ENUM_SUBLABEL_CPU_CORES)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_toggle_gamepad_combo,          MENU_ENUM_SUBLABEL_INPUT_MENU_ENUM_TOGGLE_GAMEPAD_COMBO)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_show_hidden_files,             MENU_ENUM_SUBLABEL_SHOW_HIDDEN_FILES)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_log_verbosity,                 MENU_ENUM_SUBLABEL_LOG_VERBOSITY)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_log_to_file,                   MENU_ENUM_SUBLABEL_LOG_TO_FILE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_log_to_file_timestamp,         MENU_ENUM_SUBLABEL_LOG_TO_FILE_TIMESTAMP)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_log_dir,                       MENU_ENUM_SUBLABEL_LOG_DIR)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_video_monitor_index,           MENU_ENUM_SUBLABEL_VIDEO_MONITOR_INDEX)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_video_refresh_rate_auto,       MENU_ENUM_SUBLABEL_VIDEO_REFRESH_RATE_AUTO)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_video_hard_sync,               MENU_ENUM_SUBLABEL_VIDEO_HARD_SYNC)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_video_hard_sync_frames,        MENU_ENUM_SUBLABEL_VIDEO_HARD_SYNC_FRAMES)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_video_threaded,                MENU_ENUM_SUBLABEL_VIDEO_THREADED)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_settings,                      MENU_ENUM_SUBLABEL_SETTINGS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_config_save_on_exit,           MENU_ENUM_SUBLABEL_CONFIG_SAVE_ON_EXIT)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_configuration_settings_list,   MENU_ENUM_SUBLABEL_CONFIGURATION_SETTINGS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_configurations_list_list,      MENU_ENUM_SUBLABEL_CONFIGURATIONS_LIST)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_video_shared_context,          MENU_ENUM_SUBLABEL_VIDEO_SHARED_CONTEXT)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_driver_switch_enable,          MENU_ENUM_SUBLABEL_DRIVER_SWITCH_ENABLE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_audio_latency,                 MENU_ENUM_SUBLABEL_AUDIO_LATENCY)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_audio_rate_control_delta,      MENU_ENUM_SUBLABEL_AUDIO_RATE_CONTROL_DELTA)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_audio_mute,                    MENU_ENUM_SUBLABEL_AUDIO_MUTE)
#ifdef HAVE_AUDIOMIXER
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_audio_mixer_mute,              MENU_ENUM_SUBLABEL_AUDIO_MIXER_MUTE)
#endif
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_audio_fastforward_mute,        MENU_ENUM_SUBLABEL_AUDIO_FASTFORWARD_MUTE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_camera_allow,                  MENU_ENUM_SUBLABEL_CAMERA_ALLOW)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_location_allow,                MENU_ENUM_SUBLABEL_LOCATION_ALLOW)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_max_users,               MENU_ENUM_SUBLABEL_INPUT_MAX_USERS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_poll_type_behavior,      MENU_ENUM_SUBLABEL_INPUT_POLL_TYPE_BEHAVIOR)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_all_users_control_menu,  MENU_ENUM_SUBLABEL_INPUT_ALL_USERS_CONTROL_MENU)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_bind_timeout,            MENU_ENUM_SUBLABEL_INPUT_BIND_TIMEOUT)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_bind_hold,               MENU_ENUM_SUBLABEL_INPUT_BIND_HOLD)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_audio_volume,                  MENU_ENUM_SUBLABEL_AUDIO_VOLUME)
#ifdef HAVE_AUDIOMIXER
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_audio_mixer_volume,            MENU_ENUM_SUBLABEL_AUDIO_MIXER_VOLUME)
#endif
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_audio_sync,                    MENU_ENUM_SUBLABEL_AUDIO_SYNC)
#if defined(GEKKO)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_mouse_scale, MENU_ENUM_SUBLABEL_INPUT_MOUSE_SCALE)
#endif
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_axis_threshold,                MENU_ENUM_SUBLABEL_INPUT_BUTTON_AXIS_THRESHOLD)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_turbo_period,            MENU_ENUM_SUBLABEL_INPUT_TURBO_PERIOD)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_duty_cycle,              MENU_ENUM_SUBLABEL_INPUT_DUTY_CYCLE)
   DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_turbo_mode,              MENU_ENUM_SUBLABEL_INPUT_TURBO_MODE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_turbo_default_button,    MENU_ENUM_SUBLABEL_INPUT_TURBO_DEFAULT_BUTTON)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_video_vertical_sync,           MENU_ENUM_SUBLABEL_VIDEO_VSYNC)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_video_adaptive_vsync,          MENU_ENUM_SUBLABEL_VIDEO_ADAPTIVE_VSYNC)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_core_allow_rotate,             MENU_ENUM_SUBLABEL_VIDEO_ALLOW_ROTATE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_dummy_on_core_shutdown,        MENU_ENUM_SUBLABEL_DUMMY_ON_CORE_SHUTDOWN)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_dummy_check_missing_firmware,  MENU_ENUM_SUBLABEL_CHECK_FOR_MISSING_FIRMWARE)
#ifndef HAVE_DYNAMIC
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_always_reload_core_on_run_content, MENU_ENUM_SUBLABEL_ALWAYS_RELOAD_CORE_ON_RUN_CONTENT)
#endif
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_video_refresh_rate,            MENU_ENUM_SUBLABEL_VIDEO_REFRESH_RATE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_video_refresh_rate_polled,     MENU_ENUM_SUBLABEL_VIDEO_REFRESH_RATE_POLLED)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_audio_enable,                  MENU_ENUM_SUBLABEL_AUDIO_ENABLE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_audio_enable_menu,             MENU_ENUM_SUBLABEL_AUDIO_ENABLE_MENU)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_audio_max_timing_skew,         MENU_ENUM_SUBLABEL_AUDIO_MAX_TIMING_SKEW)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_pause_nonactive,               MENU_ENUM_SUBLABEL_PAUSE_NONACTIVE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_video_disable_composition,     MENU_ENUM_SUBLABEL_VIDEO_DISABLE_COMPOSITION)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_video_smooth,                  MENU_ENUM_SUBLABEL_VIDEO_SMOOTH)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_add_null_drivers,                  MENU_ENUM_SUBLABEL_ADD_NULL_DRIVERS)
#ifdef HAVE_ODROIDGO2
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_video_ctx_scaling,             MENU_ENUM_SUBLABEL_VIDEO_RGA_SCALING)
#else
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_video_ctx_scaling,             MENU_ENUM_SUBLABEL_VIDEO_CTX_SCALING)
#endif
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_history_list_enable,           MENU_ENUM_SUBLABEL_HISTORY_LIST_ENABLE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_content_history_size,          MENU_ENUM_SUBLABEL_CONTENT_HISTORY_SIZE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_content_favorites_size,        MENU_ENUM_SUBLABEL_CONTENT_FAVORITES_SIZE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_input_unified_controls,   MENU_ENUM_SUBLABEL_INPUT_UNIFIED_MENU_CONTROLS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_quit_press_twice,              MENU_ENUM_SUBLABEL_QUIT_PRESS_TWICE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_onscreen_notifications_enable, MENU_ENUM_SUBLABEL_VIDEO_FONT_ENABLE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_video_crop_overscan,           MENU_ENUM_SUBLABEL_VIDEO_CROP_OVERSCAN)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_video_filter,                  MENU_ENUM_SUBLABEL_VIDEO_FILTER)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_netplay_nickname,              MENU_ENUM_SUBLABEL_NETPLAY_NICKNAME)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_cheevos_username,              MENU_ENUM_SUBLABEL_CHEEVOS_USERNAME)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_cheevos_password,              MENU_ENUM_SUBLABEL_CHEEVOS_PASSWORD)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_video_post_filter_record,      MENU_ENUM_SUBLABEL_VIDEO_POST_FILTER_RECORD)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_start_core,                    MENU_ENUM_SUBLABEL_START_CORE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_core_list,                     MENU_ENUM_SUBLABEL_CORE_LIST)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_download_core,                 MENU_ENUM_SUBLABEL_DOWNLOAD_CORE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_update_installed_cores,        MENU_ENUM_SUBLABEL_UPDATE_INSTALLED_CORES)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_sideload_core_list,            MENU_ENUM_SUBLABEL_SIDELOAD_CORE_LIST)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_core_manager_list,             MENU_ENUM_SUBLABEL_CORE_MANAGER_LIST)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_load_disc,                  MENU_ENUM_SUBLABEL_LOAD_DISC)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_dump_disc,                  MENU_ENUM_SUBLABEL_DUMP_DISC)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_content_list,                  MENU_ENUM_SUBLABEL_LOAD_CONTENT_LIST)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_content_special,               MENU_ENUM_SUBLABEL_LOAD_CONTENT_SPECIAL)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_load_content_history,          MENU_ENUM_SUBLABEL_LOAD_CONTENT_HISTORY)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_network_information,           MENU_ENUM_SUBLABEL_NETWORK_INFORMATION)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_system_information,            MENU_ENUM_SUBLABEL_SYSTEM_INFORMATION)
#ifdef HAVE_LAKKA
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_quit_retroarch,                MENU_ENUM_SUBLABEL_RESTART_RETROARCH)
#else
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_quit_retroarch,                MENU_ENUM_SUBLABEL_QUIT_RETROARCH)
#endif
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_restart_retroarch,             MENU_ENUM_SUBLABEL_RESTART_RETROARCH)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_widgets,             MENU_ENUM_SUBLABEL_MENU_WIDGETS_ENABLE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_video_window_width,            MENU_ENUM_SUBLABEL_VIDEO_WINDOW_WIDTH)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_video_window_height,           MENU_ENUM_SUBLABEL_VIDEO_WINDOW_HEIGHT)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_video_fullscreen_x,            MENU_ENUM_SUBLABEL_VIDEO_FULLSCREEN_X)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_video_fullscreen_y,            MENU_ENUM_SUBLABEL_VIDEO_FULLSCREEN_Y)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_video_save_window_position,    MENU_ENUM_SUBLABEL_VIDEO_WINDOW_SAVE_POSITION)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_video_message_pos_x,           MENU_ENUM_SUBLABEL_VIDEO_MESSAGE_POS_X)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_video_message_pos_y,           MENU_ENUM_SUBLABEL_VIDEO_MESSAGE_POS_Y)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_video_font_size,               MENU_ENUM_SUBLABEL_VIDEO_FONT_SIZE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_overlay_hide_in_menu,    MENU_ENUM_SUBLABEL_INPUT_OVERLAY_HIDE_IN_MENU)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_overlay_show_mouse_cursor,       MENU_ENUM_SUBLABEL_INPUT_OVERLAY_SHOW_MOUSE_CURSOR)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_overlay_auto_rotate,     MENU_ENUM_SUBLABEL_INPUT_OVERLAY_AUTO_ROTATE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_content_collection_list,       MENU_ENUM_SUBLABEL_PLAYLISTS_TAB)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_video_scale_integer,           MENU_ENUM_SUBLABEL_VIDEO_SCALE_INTEGER)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_video_gpu_screenshot,          MENU_ENUM_SUBLABEL_VIDEO_GPU_SCREENSHOT)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_video_rotation,                MENU_ENUM_SUBLABEL_VIDEO_ROTATION)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_screen_orientation,            MENU_ENUM_SUBLABEL_SCREEN_ORIENTATION)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_video_force_srgb_enable,       MENU_ENUM_SUBLABEL_VIDEO_FORCE_SRGB_DISABLE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_video_fullscreen,              MENU_ENUM_SUBLABEL_VIDEO_FULLSCREEN)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_video_windowed_fullscreen,     MENU_ENUM_SUBLABEL_VIDEO_WINDOWED_FULLSCREEN)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_video_gpu_record,              MENU_ENUM_SUBLABEL_VIDEO_GPU_RECORD)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_savestate_auto_index,          MENU_ENUM_SUBLABEL_SAVESTATE_AUTO_INDEX)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_block_sram_overwrite,          MENU_ENUM_SUBLABEL_BLOCK_SRAM_OVERWRITE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_fastforward_ratio,             MENU_ENUM_SUBLABEL_FASTFORWARD_RATIO)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_vrr_runloop_enable,            MENU_ENUM_SUBLABEL_VRR_RUNLOOP_ENABLE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_slowmotion_ratio,              MENU_ENUM_SUBLABEL_SLOWMOTION_RATIO)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_run_ahead_enabled,             MENU_ENUM_SUBLABEL_RUN_AHEAD_ENABLED)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_run_ahead_secondary_instance,  MENU_ENUM_SUBLABEL_RUN_AHEAD_SECONDARY_INSTANCE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_run_ahead_hide_warnings,       MENU_ENUM_SUBLABEL_RUN_AHEAD_HIDE_WARNINGS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_run_ahead_frames,              MENU_ENUM_SUBLABEL_RUN_AHEAD_FRAMES)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_block_timeout,           MENU_ENUM_SUBLABEL_INPUT_BLOCK_TIMEOUT)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_rewind,                        MENU_ENUM_SUBLABEL_REWIND_ENABLE)
#ifdef HAVE_CHEATS
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_cheat_apply_after_toggle,      MENU_ENUM_SUBLABEL_CHEAT_APPLY_AFTER_TOGGLE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_cheat_apply_after_load,        MENU_ENUM_SUBLABEL_CHEAT_APPLY_AFTER_LOAD)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_cheat_idx,                     MENU_ENUM_SUBLABEL_CHEAT_IDX)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_cheat_match_idx,               MENU_ENUM_SUBLABEL_CHEAT_MATCH_IDX)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_cheat_big_endian,              MENU_ENUM_SUBLABEL_CHEAT_BIG_ENDIAN)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_cheat_start_or_cont,           MENU_ENUM_SUBLABEL_CHEAT_START_OR_CONT)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_cheat_start_or_restart,        MENU_ENUM_SUBLABEL_CHEAT_START_OR_RESTART)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_cheat_search_exact,            MENU_ENUM_SUBLABEL_CHEAT_SEARCH_EXACT)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_cheat_search_lt,               MENU_ENUM_SUBLABEL_CHEAT_SEARCH_LT)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_cheat_search_gt,               MENU_ENUM_SUBLABEL_CHEAT_SEARCH_GT)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_cheat_search_eq,               MENU_ENUM_SUBLABEL_CHEAT_SEARCH_EQ)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_cheat_search_neq,              MENU_ENUM_SUBLABEL_CHEAT_SEARCH_NEQ)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_cheat_search_eqplus,           MENU_ENUM_SUBLABEL_CHEAT_SEARCH_EQPLUS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_cheat_search_eqminus,          MENU_ENUM_SUBLABEL_CHEAT_SEARCH_EQMINUS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_cheat_repeat_count,            MENU_ENUM_SUBLABEL_CHEAT_REPEAT_COUNT)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_cheat_repeat_add_to_address,   MENU_ENUM_SUBLABEL_CHEAT_REPEAT_ADD_TO_ADDRESS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_cheat_repeat_add_to_value,     MENU_ENUM_SUBLABEL_CHEAT_REPEAT_ADD_TO_VALUE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_cheat_add_matches,             MENU_ENUM_SUBLABEL_CHEAT_ADD_MATCHES)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_cheat_view_matches,            MENU_ENUM_SUBLABEL_CHEAT_VIEW_MATCHES)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_cheat_create_option,           MENU_ENUM_SUBLABEL_CHEAT_CREATE_OPTION)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_cheat_delete_option,           MENU_ENUM_SUBLABEL_CHEAT_DELETE_OPTION)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_cheat_add_new_top,             MENU_ENUM_SUBLABEL_CHEAT_ADD_NEW_TOP)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_cheat_add_new_bottom,          MENU_ENUM_SUBLABEL_CHEAT_ADD_NEW_BOTTOM)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_cheat_reload_cheats,           MENU_ENUM_SUBLABEL_CHEAT_RELOAD_CHEATS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_cheat_address_bit_position,    MENU_ENUM_SUBLABEL_CHEAT_ADDRESS_BIT_POSITION)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_cheat_delete_all,              MENU_ENUM_SUBLABEL_CHEAT_DELETE_ALL)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_meta_cheat_details,              MENU_ENUM_SUBLABEL_INPUT_META_CHEAT_DETAILS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_meta_cheat_search,               MENU_ENUM_SUBLABEL_INPUT_META_CHEAT_SEARCH)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_core_cheat_options,                    MENU_ENUM_SUBLABEL_CORE_CHEAT_OPTIONS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_quick_menu_show_cheats,                MENU_ENUM_SUBLABEL_QUICK_MENU_SHOW_CHEATS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_cheatfile_directory,                   MENU_ENUM_SUBLABEL_CHEAT_DATABASE_PATH)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_cheat_apply_changes,                   MENU_ENUM_SUBLABEL_CHEAT_APPLY_CHANGES)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_cheat_num_passes,                      MENU_ENUM_SUBLABEL_CHEAT_NUM_PASSES)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_cheat_file_load,                       MENU_ENUM_SUBLABEL_CHEAT_FILE_LOAD)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_cheat_file_load_append,                MENU_ENUM_SUBLABEL_CHEAT_FILE_LOAD_APPEND)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_cheat_file_save_as,                    MENU_ENUM_SUBLABEL_CHEAT_FILE_SAVE_AS)
#endif
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_rewind_granularity,            MENU_ENUM_SUBLABEL_REWIND_GRANULARITY)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_rewind_buffer_size,            MENU_ENUM_SUBLABEL_REWIND_BUFFER_SIZE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_rewind_buffer_size_step,       MENU_ENUM_SUBLABEL_REWIND_BUFFER_SIZE_STEP)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_libretro_log_level,            MENU_ENUM_SUBLABEL_LIBRETRO_LOG_LEVEL)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_frontend_log_level,            MENU_ENUM_SUBLABEL_FRONTEND_LOG_LEVEL)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_perfcnt_enable,                MENU_ENUM_SUBLABEL_PERFCNT_ENABLE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_savestate_auto_save,           MENU_ENUM_SUBLABEL_SAVESTATE_AUTO_SAVE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_savestate_auto_load,           MENU_ENUM_SUBLABEL_SAVESTATE_AUTO_LOAD)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_savestate_thumbnail_enable,    MENU_ENUM_SUBLABEL_SAVESTATE_THUMBNAIL_ENABLE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_save_file_compression,         MENU_ENUM_SUBLABEL_SAVE_FILE_COMPRESSION)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_savestate_file_compression,    MENU_ENUM_SUBLABEL_SAVESTATE_FILE_COMPRESSION)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_autosave_interval,             MENU_ENUM_SUBLABEL_AUTOSAVE_INTERVAL)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_remap_binds_enable,      MENU_ENUM_SUBLABEL_INPUT_REMAP_BINDS_ENABLE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_autodetect_enable,       MENU_ENUM_SUBLABEL_INPUT_AUTODETECT_ENABLE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_swap_ok_cancel,          MENU_ENUM_SUBLABEL_MENU_INPUT_SWAP_OK_CANCEL)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_pause_libretro,                MENU_ENUM_SUBLABEL_PAUSE_LIBRETRO)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_savestate_resume,         MENU_ENUM_SUBLABEL_MENU_SAVESTATE_RESUME)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_insert_disk_resume,       MENU_ENUM_SUBLABEL_MENU_INSERT_DISK_RESUME)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_video_driver,                  MENU_ENUM_SUBLABEL_VIDEO_DRIVER)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_audio_driver,                  MENU_ENUM_SUBLABEL_AUDIO_DRIVER)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_driver,                  MENU_ENUM_SUBLABEL_INPUT_DRIVER)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_joypad_driver,                 MENU_ENUM_SUBLABEL_JOYPAD_DRIVER)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_audio_resampler_driver,        MENU_ENUM_SUBLABEL_AUDIO_RESAMPLER_DRIVER)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_camera_driver,                 MENU_ENUM_SUBLABEL_CAMERA_DRIVER)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_location_driver,               MENU_ENUM_SUBLABEL_LOCATION_DRIVER)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_driver,                   MENU_ENUM_SUBLABEL_MENU_DRIVER)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_record_driver,                 MENU_ENUM_SUBLABEL_RECORD_DRIVER)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_midi_driver,                   MENU_ENUM_SUBLABEL_MIDI_DRIVER)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_bluetooth_driver,              MENU_ENUM_SUBLABEL_BLUETOOTH_DRIVER)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_wifi_driver,                   MENU_ENUM_SUBLABEL_WIFI_DRIVER)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_filter_supported_extensions,   MENU_ENUM_SUBLABEL_NAVIGATION_BROWSER_FILTER_SUPPORTED_EXTENSIONS_ENABLE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_wallpaper,                     MENU_ENUM_SUBLABEL_MENU_WALLPAPER)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_dynamic_wallpaper,             MENU_ENUM_SUBLABEL_DYNAMIC_WALLPAPER)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_audio_device,                  MENU_ENUM_SUBLABEL_AUDIO_DEVICE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_audio_output_rate,             MENU_ENUM_SUBLABEL_AUDIO_OUTPUT_RATE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_audio_dsp_plugin,              MENU_ENUM_SUBLABEL_AUDIO_DSP_PLUGIN)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_audio_wasapi_exclusive_mode,   MENU_ENUM_SUBLABEL_AUDIO_WASAPI_EXCLUSIVE_MODE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_audio_wasapi_float_format,     MENU_ENUM_SUBLABEL_AUDIO_WASAPI_FLOAT_FORMAT)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_audio_wasapi_sh_buffer_length, MENU_ENUM_SUBLABEL_AUDIO_WASAPI_SH_BUFFER_LENGTH)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_overlay_opacity,               MENU_ENUM_SUBLABEL_OVERLAY_OPACITY)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_overlay_scale,                 MENU_ENUM_SUBLABEL_OVERLAY_SCALE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_overlay_center_x,                 MENU_ENUM_SUBLABEL_OVERLAY_CENTER_X)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_overlay_center_y,                 MENU_ENUM_SUBLABEL_OVERLAY_CENTER_Y)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_overlay_enable,                MENU_ENUM_SUBLABEL_INPUT_OVERLAY_ENABLE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_overlay_preset,                MENU_ENUM_SUBLABEL_OVERLAY_PRESET)
#ifdef HAVE_VIDEO_LAYOUT
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_video_layout_enable,           MENU_ENUM_SUBLABEL_VIDEO_LAYOUT_ENABLE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_video_layout_path,             MENU_ENUM_SUBLABEL_VIDEO_LAYOUT_PATH)
#endif
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_netplay_public_announce,       MENU_ENUM_SUBLABEL_NETPLAY_PUBLIC_ANNOUNCE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_netplay_ip_address,            MENU_ENUM_SUBLABEL_NETPLAY_IP_ADDRESS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_netplay_tcp_udp_port,          MENU_ENUM_SUBLABEL_NETPLAY_TCP_UDP_PORT)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_netplay_password,              MENU_ENUM_SUBLABEL_NETPLAY_PASSWORD)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_netplay_spectate_password,     MENU_ENUM_SUBLABEL_NETPLAY_SPECTATE_PASSWORD)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_netplay_start_as_spectator,    MENU_ENUM_SUBLABEL_NETPLAY_START_AS_SPECTATOR)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_netplay_allow_slaves,          MENU_ENUM_SUBLABEL_NETPLAY_ALLOW_SLAVES)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_netplay_require_slaves,        MENU_ENUM_SUBLABEL_NETPLAY_REQUIRE_SLAVES)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_netplay_stateless_mode,        MENU_ENUM_SUBLABEL_NETPLAY_STATELESS_MODE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_netplay_check_frames,          MENU_ENUM_SUBLABEL_NETPLAY_CHECK_FRAMES)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_netplay_nat_traversal,         MENU_ENUM_SUBLABEL_NETPLAY_NAT_TRAVERSAL)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_stdin_cmd_enable,              MENU_ENUM_SUBLABEL_STDIN_CMD_ENABLE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_mouse_enable,                  MENU_ENUM_SUBLABEL_MOUSE_ENABLE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_pointer_enable,                MENU_ENUM_SUBLABEL_POINTER_ENABLE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_thumbnails,                    MENU_ENUM_SUBLABEL_THUMBNAILS)
#ifdef HAVE_MATERIALUI
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_thumbnails_materialui,         MENU_ENUM_SUBLABEL_THUMBNAILS_MATERIALUI)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_left_thumbnails_materialui,    MENU_ENUM_SUBLABEL_LEFT_THUMBNAILS_MATERIALUI)
#endif
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_left_thumbnails,               MENU_ENUM_SUBLABEL_LEFT_THUMBNAILS)
#ifdef HAVE_RGUI
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_left_thumbnails_rgui,          MENU_ENUM_SUBLABEL_LEFT_THUMBNAILS_RGUI)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_thumbnails_rgui,               MENU_ENUM_SUBLABEL_THUMBNAILS_RGUI)
#endif
#ifdef HAVE_OZONE
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_left_thumbnails_ozone,         MENU_ENUM_SUBLABEL_LEFT_THUMBNAILS_OZONE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_ozone_menu_color_theme,                MENU_ENUM_SUBLABEL_OZONE_MENU_COLOR_THEME)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_ozone_collapse_sidebar,                MENU_ENUM_SUBLABEL_OZONE_COLLAPSE_SIDEBAR)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_ozone_truncate_playlist_name,          MENU_ENUM_SUBLABEL_OZONE_TRUNCATE_PLAYLIST_NAME)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_ozone_sort_after_truncate_playlist_name, MENU_ENUM_SUBLABEL_OZONE_SORT_AFTER_TRUNCATE_PLAYLIST_NAME)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_ozone_scroll_content_metadata,         MENU_ENUM_SUBLABEL_OZONE_SCROLL_CONTENT_METADATA)
#endif
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_thumbnail_upscale_threshold, MENU_ENUM_SUBLABEL_MENU_THUMBNAIL_UPSCALE_THRESHOLD)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_timedate_enable,               MENU_ENUM_SUBLABEL_TIMEDATE_ENABLE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_timedate_style,                MENU_ENUM_SUBLABEL_TIMEDATE_STYLE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_timedate_date_separator,       MENU_ENUM_SUBLABEL_TIMEDATE_DATE_SEPARATOR)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_battery_level_enable,          MENU_ENUM_SUBLABEL_BATTERY_LEVEL_ENABLE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_show_sublabels,           MENU_ENUM_SUBLABEL_MENU_SHOW_SUBLABELS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_navigation_wraparound,         MENU_ENUM_SUBLABEL_NAVIGATION_WRAPAROUND)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_audio_resampler_quality,       MENU_ENUM_SUBLABEL_AUDIO_RESAMPLER_QUALITY)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_netplay_enable_host,           MENU_ENUM_SUBLABEL_NETPLAY_ENABLE_HOST)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_netplay_enable_client,         MENU_ENUM_SUBLABEL_NETPLAY_ENABLE_CLIENT)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_netplay_disconnect,            MENU_ENUM_SUBLABEL_NETPLAY_DISCONNECT)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_scan_file,                     MENU_ENUM_SUBLABEL_SCAN_FILE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_scan_directory,                MENU_ENUM_SUBLABEL_SCAN_DIRECTORY)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_video_swap_interval,           MENU_ENUM_SUBLABEL_VIDEO_SWAP_INTERVAL)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_sort_savefiles_enable,         MENU_ENUM_SUBLABEL_SORT_SAVEFILES_ENABLE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_sort_savestates_enable,        MENU_ENUM_SUBLABEL_SORT_SAVESTATES_ENABLE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_core_updater_buildbot_url,     MENU_ENUM_SUBLABEL_CORE_UPDATER_BUILDBOT_URL)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_overlay_show_physical_inputs,    MENU_ENUM_SUBLABEL_INPUT_OVERLAY_SHOW_PHYSICAL_INPUTS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_overlay_show_physical_inputs_port,    MENU_ENUM_SUBLABEL_INPUT_OVERLAY_SHOW_PHYSICAL_INPUTS_PORT)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_core_updater_buildbot_assets_url,      MENU_ENUM_SUBLABEL_BUILDBOT_ASSETS_URL)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_core_updater_auto_extract_archive,     MENU_ENUM_SUBLABEL_CORE_UPDATER_AUTO_EXTRACT_ARCHIVE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_core_updater_show_experimental_cores,  MENU_ENUM_SUBLABEL_CORE_UPDATER_SHOW_EXPERIMENTAL_CORES)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_core_updater_auto_backup,              MENU_ENUM_SUBLABEL_CORE_UPDATER_AUTO_BACKUP)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_core_updater_auto_backup_history_size, MENU_ENUM_SUBLABEL_CORE_UPDATER_AUTO_BACKUP_HISTORY_SIZE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_netplay_refresh_rooms,                 MENU_ENUM_SUBLABEL_NETPLAY_REFRESH_ROOMS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_rename_entry,                          MENU_ENUM_SUBLABEL_RENAME_ENTRY)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_delete_entry,                          MENU_ENUM_SUBLABEL_DELETE_ENTRY)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_information,                           MENU_ENUM_SUBLABEL_INFORMATION)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_run,                                   MENU_ENUM_SUBLABEL_RUN)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_add_to_favorites,                      MENU_ENUM_SUBLABEL_ADD_TO_FAVORITES)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_download_pl_entry_thumbnails,          MENU_ENUM_SUBLABEL_DOWNLOAD_PL_ENTRY_THUMBNAILS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_goto_favorites,                        MENU_ENUM_SUBLABEL_GOTO_FAVORITES)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_goto_images,                           MENU_ENUM_SUBLABEL_GOTO_IMAGES)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_goto_music,                            MENU_ENUM_SUBLABEL_GOTO_MUSIC)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_goto_video,                            MENU_ENUM_SUBLABEL_GOTO_VIDEO)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_filebrowser_settings,             MENU_ENUM_SUBLABEL_MENU_FILE_BROWSER_SETTINGS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_filebrowser_open_uwp_permissions, MENU_ENUM_SUBLABEL_FILE_BROWSER_OPEN_UWP_PERMISSIONS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_filebrowser_open_picker,          MENU_ENUM_SUBLABEL_FILE_BROWSER_OPEN_PICKER)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_auto_remaps_enable,                    MENU_ENUM_SUBLABEL_AUTO_REMAPS_ENABLE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_auto_overrides_enable,                 MENU_ENUM_SUBLABEL_AUTO_OVERRIDES_ENABLE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_game_specific_options,                 MENU_ENUM_SUBLABEL_GAME_SPECIFIC_OPTIONS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_global_core_options,                   MENU_ENUM_SUBLABEL_GLOBAL_CORE_OPTIONS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_core_enable,                           MENU_ENUM_SUBLABEL_CORE_ENABLE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_database_manager,                      MENU_ENUM_SUBLABEL_DATABASE_MANAGER)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_cursor_manager,                        MENU_ENUM_SUBLABEL_CURSOR_MANAGER)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_take_screenshot,                       MENU_ENUM_SUBLABEL_TAKE_SCREENSHOT)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_close_content,                         MENU_ENUM_SUBLABEL_CLOSE_CONTENT)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_load_state,                            MENU_ENUM_SUBLABEL_LOAD_STATE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_save_state,                            MENU_ENUM_SUBLABEL_SAVE_STATE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_resume_content,                        MENU_ENUM_SUBLABEL_RESUME_CONTENT)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_state_slot,                            MENU_ENUM_SUBLABEL_STATE_SLOT)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_undo_load_state,                       MENU_ENUM_SUBLABEL_UNDO_LOAD_STATE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_undo_save_state,                       MENU_ENUM_SUBLABEL_UNDO_SAVE_STATE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_accounts_retro_achievements,           MENU_ENUM_SUBLABEL_ACCOUNTS_RETRO_ACHIEVEMENTS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_accounts_list,                         MENU_ENUM_SUBLABEL_ACCOUNTS_LIST)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_meta_rewind,                     MENU_ENUM_SUBLABEL_INPUT_META_REWIND)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_restart_content,                       MENU_ENUM_SUBLABEL_RESTART_CONTENT)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_save_current_config_override_core,     MENU_ENUM_SUBLABEL_SAVE_CURRENT_CONFIG_OVERRIDE_CORE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_save_current_config_override_content_dir,
                                                                                   MENU_ENUM_SUBLABEL_SAVE_CURRENT_CONFIG_OVERRIDE_CONTENT_DIR)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_save_current_config_override_game,     MENU_ENUM_SUBLABEL_SAVE_CURRENT_CONFIG_OVERRIDE_GAME)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_shader_options,                        MENU_ENUM_SUBLABEL_SHADER_OPTIONS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_core_input_remapping_options,          MENU_ENUM_SUBLABEL_CORE_INPUT_REMAPPING_OPTIONS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_core_options,                          MENU_ENUM_SUBLABEL_CORE_OPTIONS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_show_advanced_settings,                MENU_ENUM_SUBLABEL_SHOW_ADVANCED_SETTINGS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_threaded_data_runloop_enable,          MENU_ENUM_SUBLABEL_THREADED_DATA_RUNLOOP_ENABLE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_playlist_entry_rename,                 MENU_ENUM_SUBLABEL_PLAYLIST_ENTRY_RENAME)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_playlist_entry_remove,                 MENU_ENUM_SUBLABEL_PLAYLIST_ENTRY_REMOVE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_system_directory,                      MENU_ENUM_SUBLABEL_SYSTEM_DIRECTORY)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_rgui_browser_directory,                MENU_ENUM_SUBLABEL_RGUI_BROWSER_DIRECTORY)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_content_dir,                           MENU_ENUM_SUBLABEL_CONTENT_DIR)
DEFAULT_SUBLABEL_MACRO(action_bind_dynamic_wallpapers_directory,                   MENU_ENUM_SUBLABEL_DYNAMIC_WALLPAPERS_DIRECTORY)
DEFAULT_SUBLABEL_MACRO(action_bind_thumbnails_directory,                           MENU_ENUM_SUBLABEL_THUMBNAILS_DIRECTORY)
DEFAULT_SUBLABEL_MACRO(action_bind_rgui_config_directory,                          MENU_ENUM_SUBLABEL_RGUI_CONFIG_DIRECTORY)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_latency_frames,                  MENU_ENUM_SUBLABEL_NETPLAY_INPUT_LATENCY_FRAMES_MIN)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_latency_frames_range,            MENU_ENUM_SUBLABEL_NETPLAY_INPUT_LATENCY_FRAMES_RANGE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_disk_tray_eject,                       MENU_ENUM_SUBLABEL_DISK_TRAY_EJECT)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_disk_tray_insert,                      MENU_ENUM_SUBLABEL_DISK_TRAY_INSERT)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_disk_index,                            MENU_ENUM_SUBLABEL_DISK_INDEX)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_disk_options,                          MENU_ENUM_SUBLABEL_DISK_OPTIONS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_throttle_framerate,               MENU_ENUM_SUBLABEL_MENU_ENUM_THROTTLE_FRAMERATE)
#ifdef HAVE_XMB
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_xmb_layout,                            MENU_ENUM_SUBLABEL_XMB_LAYOUT)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_xmb_icon_theme,                        MENU_ENUM_SUBLABEL_XMB_THEME)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_xmb_shadows_enable,                    MENU_ENUM_SUBLABEL_XMB_SHADOWS_ENABLE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_xmb_vertical_thumbnails,               MENU_ENUM_SUBLABEL_XMB_VERTICAL_THUMBNAILS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_xmb_thumbnail_scale_factor,       MENU_ENUM_SUBLABEL_MENU_XMB_THUMBNAIL_SCALE_FACTOR)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_xmb_animation_horizontal_higlight,             MENU_ENUM_SUBLABEL_MENU_XMB_ANIMATION_HORIZONTAL_HIGHLIGHT)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_xmb_animation_move_up_down,             MENU_ENUM_SUBLABEL_MENU_XMB_ANIMATION_MOVE_UP_DOWN)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_xmb_animation_opening_main_menu,             MENU_ENUM_SUBLABEL_MENU_XMB_ANIMATION_OPENING_MAIN_MENU)
#endif
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_color_theme,                      MENU_ENUM_SUBLABEL_MATERIALUI_MENU_COLOR_THEME)
#ifdef HAVE_MATERIALUI
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_materialui_menu_transition_animation, MENU_ENUM_SUBLABEL_MATERIALUI_MENU_TRANSITION_ANIMATION)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_materialui_menu_thumbnail_view_portrait, MENU_ENUM_SUBLABEL_MATERIALUI_MENU_THUMBNAIL_VIEW_PORTRAIT)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_materialui_menu_thumbnail_view_landscape, MENU_ENUM_SUBLABEL_MATERIALUI_MENU_THUMBNAIL_VIEW_LANDSCAPE)
#endif
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_use_preferred_system_color_theme, MENU_ENUM_SUBLABEL_MENU_USE_PREFERRED_SYSTEM_COLOR_THEME)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_scale_factor,                     MENU_ENUM_SUBLABEL_MENU_SCALE_FACTOR)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_widget_scale_auto,                MENU_ENUM_SUBLABEL_MENU_WIDGET_SCALE_AUTO)
#if (defined(RARCH_CONSOLE) || defined(RARCH_MOBILE))
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_widget_scale_factor,              MENU_ENUM_SUBLABEL_MENU_WIDGET_SCALE_FACTOR)
#else
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_widget_scale_factor,              MENU_ENUM_SUBLABEL_MENU_WIDGET_SCALE_FACTOR_FULLSCREEN)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_widget_scale_factor_windowed,     MENU_ENUM_SUBLABEL_MENU_WIDGET_SCALE_FACTOR_WINDOWED)
#endif
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_wallpaper_opacity,                MENU_ENUM_SUBLABEL_MENU_WALLPAPER_OPACITY)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_framebuffer_opacity,              MENU_ENUM_SUBLABEL_MENU_FRAMEBUFFER_OPACITY)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_horizontal_animation,             MENU_ENUM_SUBLABEL_MENU_HORIZONTAL_ANIMATION)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_ribbon_enable,                    MENU_ENUM_SUBLABEL_XMB_RIBBON_ENABLE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_font,                             MENU_ENUM_SUBLABEL_XMB_FONT)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_settings_show_drivers,                 MENU_ENUM_SUBLABEL_SETTINGS_SHOW_DRIVERS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_settings_show_video,                   MENU_ENUM_SUBLABEL_SETTINGS_SHOW_VIDEO)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_settings_show_audio,                   MENU_ENUM_SUBLABEL_SETTINGS_SHOW_AUDIO)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_settings_show_input,                   MENU_ENUM_SUBLABEL_SETTINGS_SHOW_INPUT)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_settings_show_latency,                 MENU_ENUM_SUBLABEL_SETTINGS_SHOW_LATENCY)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_settings_show_core,                    MENU_ENUM_SUBLABEL_SETTINGS_SHOW_CORE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_settings_show_configuration,           MENU_ENUM_SUBLABEL_SETTINGS_SHOW_CONFIGURATION)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_settings_show_saving,                  MENU_ENUM_SUBLABEL_SETTINGS_SHOW_SAVING)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_settings_show_logging,                 MENU_ENUM_SUBLABEL_SETTINGS_SHOW_LOGGING)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_settings_show_frame_throttle,          MENU_ENUM_SUBLABEL_SETTINGS_SHOW_FRAME_THROTTLE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_settings_show_recording,               MENU_ENUM_SUBLABEL_SETTINGS_SHOW_RECORDING)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_settings_show_onscreen_display,        MENU_ENUM_SUBLABEL_SETTINGS_SHOW_ONSCREEN_DISPLAY)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_settings_show_user_interface,          MENU_ENUM_SUBLABEL_SETTINGS_SHOW_USER_INTERFACE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_settings_show_ai_service,              MENU_ENUM_SUBLABEL_SETTINGS_SHOW_AI_SERVICE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_settings_show_power_management,        MENU_ENUM_SUBLABEL_SETTINGS_SHOW_POWER_MANAGEMENT)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_settings_show_achievements,            MENU_ENUM_SUBLABEL_SETTINGS_SHOW_ACHIEVEMENTS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_settings_show_network,                 MENU_ENUM_SUBLABEL_SETTINGS_SHOW_NETWORK)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_settings_show_playlists,               MENU_ENUM_SUBLABEL_SETTINGS_SHOW_PLAYLISTS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_settings_show_user,                    MENU_ENUM_SUBLABEL_SETTINGS_SHOW_USER)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_settings_show_directory,               MENU_ENUM_SUBLABEL_SETTINGS_SHOW_DIRECTORY)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_quick_menu_show_take_screenshot,       MENU_ENUM_SUBLABEL_QUICK_MENU_SHOW_TAKE_SCREENSHOT)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_quick_menu_show_resume_content,        MENU_ENUM_SUBLABEL_QUICK_MENU_SHOW_RESUME_CONTENT)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_quick_menu_show_restart_content,       MENU_ENUM_SUBLABEL_QUICK_MENU_SHOW_RESTART_CONTENT)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_quick_menu_show_close_content,         MENU_ENUM_SUBLABEL_QUICK_MENU_SHOW_CLOSE_CONTENT)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_quick_menu_show_save_load_state,       MENU_ENUM_SUBLABEL_QUICK_MENU_SHOW_SAVE_LOAD_STATE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_quick_menu_show_undo_save_load_state,  MENU_ENUM_SUBLABEL_QUICK_MENU_SHOW_UNDO_SAVE_LOAD_STATE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_quick_menu_show_add_to_favorites,      MENU_ENUM_SUBLABEL_QUICK_MENU_SHOW_ADD_TO_FAVORITES)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_quick_menu_show_start_recording,       MENU_ENUM_SUBLABEL_QUICK_MENU_SHOW_START_RECORDING)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_quick_menu_show_start_streaming,       MENU_ENUM_SUBLABEL_QUICK_MENU_SHOW_START_STREAMING)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_quick_menu_show_set_core_association,  MENU_ENUM_SUBLABEL_QUICK_MENU_SHOW_SET_CORE_ASSOCIATION)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_quick_menu_show_reset_core_association, MENU_ENUM_SUBLABEL_QUICK_MENU_SHOW_RESET_CORE_ASSOCIATION)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_quick_menu_show_options,               MENU_ENUM_SUBLABEL_QUICK_MENU_SHOW_OPTIONS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_quick_menu_show_controls,              MENU_ENUM_SUBLABEL_QUICK_MENU_SHOW_CONTROLS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_quick_menu_show_shaders,               MENU_ENUM_SUBLABEL_QUICK_MENU_SHOW_SHADERS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_content_show_overlays,                 MENU_ENUM_SUBLABEL_CONTENT_SHOW_OVERLAYS)
#ifdef HAVE_VIDEO_LAYOUT
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_content_show_video_layout,             MENU_ENUM_SUBLABEL_CONTENT_SHOW_VIDEO_LAYOUT)
#endif
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_content_show_rewind,                   MENU_ENUM_SUBLABEL_CONTENT_SHOW_REWIND)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_content_show_latency,                  MENU_ENUM_SUBLABEL_CONTENT_SHOW_LATENCY)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_quick_menu_show_save_core_overrides,   MENU_ENUM_SUBLABEL_QUICK_MENU_SHOW_SAVE_CORE_OVERRIDES)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_quick_menu_show_save_game_overrides,   MENU_ENUM_SUBLABEL_QUICK_MENU_SHOW_SAVE_GAME_OVERRIDES)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_quick_menu_show_information,           MENU_ENUM_SUBLABEL_QUICK_MENU_SHOW_INFORMATION)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_quick_menu_show_download_thumbnails,   MENU_ENUM_SUBLABEL_QUICK_MENU_SHOW_DOWNLOAD_THUMBNAILS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_enable_kiosk_mode,                MENU_ENUM_SUBLABEL_MENU_ENABLE_KIOSK_MODE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_disable_kiosk_mode,               MENU_ENUM_SUBLABEL_MENU_DISABLE_KIOSK_MODE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_kiosk_mode_password,              MENU_ENUM_SUBLABEL_MENU_KIOSK_MODE_PASSWORD)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_favorites_tab,                    MENU_ENUM_SUBLABEL_CONTENT_SHOW_FAVORITES)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_images_tab,                       MENU_ENUM_SUBLABEL_CONTENT_SHOW_IMAGES)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_show_load_core,                   MENU_ENUM_SUBLABEL_MENU_SHOW_LOAD_CORE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_show_load_content,                MENU_ENUM_SUBLABEL_MENU_SHOW_LOAD_CONTENT)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_show_load_disc,                   MENU_ENUM_SUBLABEL_MENU_SHOW_LOAD_DISC)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_show_dump_disc,                   MENU_ENUM_SUBLABEL_MENU_SHOW_DUMP_DISC)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_show_information,                 MENU_ENUM_SUBLABEL_MENU_SHOW_INFORMATION)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_show_configurations,              MENU_ENUM_SUBLABEL_MENU_SHOW_CONFIGURATIONS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_show_help,                        MENU_ENUM_SUBLABEL_MENU_SHOW_HELP)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_show_quit_retroarch,              MENU_ENUM_SUBLABEL_MENU_SHOW_QUIT_RETROARCH)
#ifndef HAVE_LAKKA
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_show_restart_retroarch,           MENU_ENUM_SUBLABEL_MENU_SHOW_RESTART_RETROARCH)
#endif
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_show_reboot,                      MENU_ENUM_SUBLABEL_MENU_SHOW_REBOOT)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_show_shutdown,                    MENU_ENUM_SUBLABEL_MENU_SHOW_SHUTDOWN)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_show_online_updater,              MENU_ENUM_SUBLABEL_MENU_SHOW_ONLINE_UPDATER)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_show_core_updater,                MENU_ENUM_SUBLABEL_MENU_SHOW_CORE_UPDATER)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_show_legacy_thumbnail_updater,    MENU_ENUM_SUBLABEL_MENU_SHOW_LEGACY_THUMBNAIL_UPDATER)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_scroll_fast,                      MENU_ENUM_SUBLABEL_MENU_SCROLL_FAST)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_music_tab,                        MENU_ENUM_SUBLABEL_CONTENT_SHOW_MUSIC)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_video_tab,                        MENU_ENUM_SUBLABEL_CONTENT_SHOW_VIDEO)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_netplay_tab,                      MENU_ENUM_SUBLABEL_CONTENT_SHOW_NETPLAY)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_settings_tab,                     MENU_ENUM_SUBLABEL_CONTENT_SHOW_SETTINGS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_settings_tab_enable_password,     MENU_ENUM_SUBLABEL_CONTENT_SHOW_SETTINGS_PASSWORD)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_history_tab,                      MENU_ENUM_SUBLABEL_CONTENT_SHOW_HISTORY)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_import_content_tab,               MENU_ENUM_SUBLABEL_CONTENT_SHOW_ADD)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_playlist_tabs,                    MENU_ENUM_SUBLABEL_CONTENT_SHOW_PLAYLISTS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_main_menu_enable_settings,             MENU_ENUM_SUBLABEL_XMB_MAIN_MENU_ENABLE_SETTINGS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_rgui_show_start_screen,                MENU_ENUM_SUBLABEL_RGUI_SHOW_START_SCREEN)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_header_opacity,                   MENU_ENUM_SUBLABEL_MATERIALUI_MENU_HEADER_OPACITY)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_footer_opacity,                   MENU_ENUM_SUBLABEL_MATERIALUI_MENU_FOOTER_OPACITY)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_core_assets_directory,                 MENU_ENUM_SUBLABEL_CORE_ASSETS_DIRECTORY)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_input_remapping_directory,             MENU_ENUM_SUBLABEL_INPUT_REMAPPING_DIRECTORY)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_core_directory,                        MENU_ENUM_SUBLABEL_LIBRETRO_DIR_PATH)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_core_info_directory,                   MENU_ENUM_SUBLABEL_LIBRETRO_INFO_PATH)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_joypad_autoconfig_directory,           MENU_ENUM_SUBLABEL_JOYPAD_AUTOCONFIG_DIR)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_playlists_directory,                   MENU_ENUM_SUBLABEL_PLAYLIST_DIRECTORY)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_runtime_log_directory,                 MENU_ENUM_SUBLABEL_RUNTIME_LOG_DIRECTORY)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_cache_directory,                       MENU_ENUM_SUBLABEL_CACHE_DIRECTORY)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_database_directory,                    MENU_ENUM_SUBLABEL_CONTENT_DATABASE_DIRECTORY)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_cursor_directory,                      MENU_ENUM_SUBLABEL_CURSOR_DIRECTORY)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_assets_directory,                      MENU_ENUM_SUBLABEL_ASSETS_DIRECTORY)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_savefile_directory,                    MENU_ENUM_SUBLABEL_SAVEFILE_DIRECTORY)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_savestate_directory,                   MENU_ENUM_SUBLABEL_SAVESTATE_DIRECTORY)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_screenshot_directory,                  MENU_ENUM_SUBLABEL_SCREENSHOT_DIRECTORY)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_overlay_directory,                     MENU_ENUM_SUBLABEL_OVERLAY_DIRECTORY)
#ifdef HAVE_VIDEO_LAYOUT
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_video_layout_directory,                MENU_ENUM_SUBLABEL_VIDEO_LAYOUT_DIRECTORY)
#endif
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_audio_filter_directory,                MENU_ENUM_SUBLABEL_AUDIO_FILTER_DIR)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_video_filter_directory,                MENU_ENUM_SUBLABEL_VIDEO_FILTER_DIR)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_video_shader_directory,                MENU_ENUM_SUBLABEL_VIDEO_SHADER_DIR)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_recording_output_directory,            MENU_ENUM_SUBLABEL_RECORDING_OUTPUT_DIRECTORY)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_recording_config_directory,            MENU_ENUM_SUBLABEL_RECORDING_CONFIG_DIRECTORY)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_video_font_path,                       MENU_ENUM_SUBLABEL_VIDEO_FONT_PATH)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_shader_apply_changes,                  MENU_ENUM_SUBLABEL_SHADER_APPLY_CHANGES)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_shader_watch_for_changes,              MENU_ENUM_SUBLABEL_SHADER_WATCH_FOR_CHANGES)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_shader_num_passes,                     MENU_ENUM_SUBLABEL_VIDEO_SHADER_NUM_PASSES)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_shader_preset,                         MENU_ENUM_SUBLABEL_VIDEO_SHADER_PRESET)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_shader_preset_save,                    MENU_ENUM_SUBLABEL_VIDEO_SHADER_PRESET_SAVE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_shader_preset_remove,                    MENU_ENUM_SUBLABEL_VIDEO_SHADER_PRESET_REMOVE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_shader_preset_save_as,                 MENU_ENUM_SUBLABEL_VIDEO_SHADER_PRESET_SAVE_AS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_shader_preset_save_global,             MENU_ENUM_SUBLABEL_VIDEO_SHADER_PRESET_SAVE_GLOBAL)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_shader_preset_save_core,               MENU_ENUM_SUBLABEL_VIDEO_SHADER_PRESET_SAVE_CORE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_shader_preset_save_parent,             MENU_ENUM_SUBLABEL_VIDEO_SHADER_PRESET_SAVE_PARENT)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_shader_preset_save_game,               MENU_ENUM_SUBLABEL_VIDEO_SHADER_PRESET_SAVE_GAME)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_shader_preset_remove_global,             MENU_ENUM_SUBLABEL_VIDEO_SHADER_PRESET_REMOVE_GLOBAL)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_shader_preset_remove_core,               MENU_ENUM_SUBLABEL_VIDEO_SHADER_PRESET_REMOVE_CORE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_shader_preset_remove_parent,             MENU_ENUM_SUBLABEL_VIDEO_SHADER_PRESET_REMOVE_PARENT)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_shader_preset_remove_game,               MENU_ENUM_SUBLABEL_VIDEO_SHADER_PRESET_REMOVE_GAME)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_shader_parameters,                     MENU_ENUM_SUBLABEL_VIDEO_SHADER_PARAMETERS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_shader_preset_parameters,              MENU_ENUM_SUBLABEL_VIDEO_SHADER_PRESET_PARAMETERS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_quick_menu,                            MENU_ENUM_SUBLABEL_CONTENT_SETTINGS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_core_information,                      MENU_ENUM_SUBLABEL_CORE_INFORMATION)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_disc_information,                      MENU_ENUM_SUBLABEL_DISC_INFORMATION)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_video_aspect_ratio,                    MENU_ENUM_SUBLABEL_VIDEO_ASPECT_RATIO)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_video_viewport_custom_height,          MENU_ENUM_SUBLABEL_VIDEO_VIEWPORT_CUSTOM_HEIGHT)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_video_viewport_custom_width,           MENU_ENUM_SUBLABEL_VIDEO_VIEWPORT_CUSTOM_WIDTH)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_video_viewport_custom_x,               MENU_ENUM_SUBLABEL_VIDEO_VIEWPORT_CUSTOM_X)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_video_viewport_custom_y,               MENU_ENUM_SUBLABEL_VIDEO_VIEWPORT_CUSTOM_Y)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_netplay_use_mitm_server,               MENU_ENUM_SUBLABEL_NETPLAY_USE_MITM_SERVER)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_netplay_mitm_server,                   MENU_ENUM_SUBLABEL_NETPLAY_MITM_SERVER)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_core_lock,                             MENU_ENUM_SUBLABEL_CORE_LOCK)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_core_delete,                           MENU_ENUM_SUBLABEL_CORE_DELETE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_pause_hardcode_mode,                   MENU_ENUM_SUBLABEL_ACHIEVEMENT_PAUSE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_resume_hardcode_mode,                  MENU_ENUM_SUBLABEL_ACHIEVEMENT_RESUME)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_midi_input,                            MENU_ENUM_SUBLABEL_MIDI_INPUT)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_midi_output,                           MENU_ENUM_SUBLABEL_MIDI_OUTPUT)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_midi_volume,                           MENU_ENUM_SUBLABEL_MIDI_VOLUME)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_onscreen_overlay_settings_list,        MENU_ENUM_SUBLABEL_ONSCREEN_OVERLAY_SETTINGS)
#ifdef HAVE_VIDEO_LAYOUT
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_onscreen_video_layout_settings_list,   MENU_ENUM_SUBLABEL_ONSCREEN_VIDEO_LAYOUT_SETTINGS)
#endif
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_onscreen_notifications_settings_list,  MENU_ENUM_SUBLABEL_ONSCREEN_NOTIFICATIONS_SETTINGS)
#ifdef HAVE_QT
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_show_wimp,                             MENU_ENUM_SUBLABEL_SHOW_WIMP)
#endif
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_discord_allow,                         MENU_ENUM_SUBLABEL_DISCORD_ALLOW)

#if defined(HAVE_LAKKA_SWITCH) || defined(HAVE_LIBNX)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_switch_cpu_profile,             MENU_ENUM_SUBLABEL_SWITCH_CPU_PROFILE)
#endif

#ifdef HAVE_LAKKA_SWITCH
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_switch_gpu_profile,             MENU_ENUM_SUBLABEL_SWITCH_GPU_PROFILE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_switch_backlight_control,       MENU_ENUM_SUBLABEL_SWITCH_BACKLIGHT_CONTROL)
#endif

#if defined(_3DS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_video_3ds_lcd_bottom,           MENU_ENUM_SUBLABEL_VIDEO_3DS_LCD_BOTTOM)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_video_3ds_display_mode,         MENU_ENUM_SUBLABEL_VIDEO_3DS_DISPLAY_MODE)
#endif

#if defined(GEKKO)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_video_overscan_correction_top,    MENU_ENUM_SUBLABEL_VIDEO_OVERSCAN_CORRECTION_TOP)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_video_overscan_correction_bottom, MENU_ENUM_SUBLABEL_VIDEO_OVERSCAN_CORRECTION_BOTTOM)
#endif

DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_playlist_show_sublabels,                       MENU_ENUM_SUBLABEL_PLAYLIST_SHOW_SUBLABELS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_rgui_border_filler_enable,                MENU_ENUM_SUBLABEL_MENU_RGUI_BORDER_FILLER_ENABLE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_rgui_border_filler_thickness_enable,      MENU_ENUM_SUBLABEL_MENU_RGUI_BORDER_FILLER_THICKNESS_ENABLE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_rgui_background_filler_thickness_enable,  MENU_ENUM_SUBLABEL_MENU_RGUI_BACKGROUND_FILLER_THICKNESS_ENABLE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_linear_filter,                            MENU_ENUM_SUBLABEL_MENU_LINEAR_FILTER)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_rgui_aspect_ratio_lock,                   MENU_ENUM_SUBLABEL_MENU_RGUI_ASPECT_RATIO_LOCK)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_rgui_menu_color_theme,                         MENU_ENUM_SUBLABEL_RGUI_MENU_COLOR_THEME)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_rgui_menu_theme_preset,                        MENU_ENUM_SUBLABEL_RGUI_MENU_THEME_PRESET)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_rgui_shadows,                             MENU_ENUM_SUBLABEL_MENU_RGUI_SHADOWS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_rgui_particle_effect,                     MENU_ENUM_SUBLABEL_MENU_RGUI_PARTICLE_EFFECT)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_rgui_particle_effect_speed,               MENU_ENUM_SUBLABEL_MENU_RGUI_PARTICLE_EFFECT_SPEED)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_rgui_inline_thumbnails,                   MENU_ENUM_SUBLABEL_MENU_RGUI_INLINE_THUMBNAILS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_rgui_swap_thumbnails,                     MENU_ENUM_SUBLABEL_MENU_RGUI_SWAP_THUMBNAILS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_rgui_thumbnail_downscaler,                MENU_ENUM_SUBLABEL_MENU_RGUI_THUMBNAIL_DOWNSCALER)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_rgui_thumbnail_delay,                     MENU_ENUM_SUBLABEL_MENU_RGUI_THUMBNAIL_DELAY)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_content_runtime_log,                           MENU_ENUM_SUBLABEL_CONTENT_RUNTIME_LOG)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_content_runtime_log_aggregate,                 MENU_ENUM_SUBLABEL_CONTENT_RUNTIME_LOG_AGGREGATE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_scan_without_core_match,                 MENU_ENUM_SUBLABEL_SCAN_WITHOUT_CORE_MATCH)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_playlist_sublabel_runtime_type,                MENU_ENUM_SUBLABEL_PLAYLIST_SUBLABEL_RUNTIME_TYPE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_playlist_sublabel_last_played_style,           MENU_ENUM_SUBLABEL_PLAYLIST_SUBLABEL_LAST_PLAYED_STYLE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_rgui_internal_upscale_level,              MENU_ENUM_SUBLABEL_MENU_RGUI_INTERNAL_UPSCALE_LEVEL)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_rgui_aspect_ratio,                        MENU_ENUM_SUBLABEL_MENU_RGUI_ASPECT_RATIO)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_ticker_type,                              MENU_ENUM_SUBLABEL_MENU_TICKER_TYPE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_ticker_speed,                             MENU_ENUM_SUBLABEL_MENU_TICKER_SPEED)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_ticker_smooth,                            MENU_ENUM_SUBLABEL_MENU_TICKER_SMOOTH)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_playlist_show_inline_core_name,                MENU_ENUM_SUBLABEL_PLAYLIST_SHOW_INLINE_CORE_NAME)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_playlist_sort_alphabetical,                    MENU_ENUM_SUBLABEL_PLAYLIST_SORT_ALPHABETICAL)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_playlist_fuzzy_archive_match,                  MENU_ENUM_SUBLABEL_PLAYLIST_FUZZY_ARCHIVE_MATCH)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_playlist_use_old_format,                       MENU_ENUM_SUBLABEL_PLAYLIST_USE_OLD_FORMAT)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_playlist_compression,                          MENU_ENUM_SUBLABEL_PLAYLIST_COMPRESSION)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_rgui_full_width_layout,                   MENU_ENUM_SUBLABEL_MENU_RGUI_FULL_WIDTH_LAYOUT)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_menu_rgui_extended_ascii,                      MENU_ENUM_SUBLABEL_MENU_RGUI_EXTENDED_ASCII)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_thumbnails_updater_list,                       MENU_ENUM_SUBLABEL_THUMBNAILS_UPDATER_LIST)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_pl_thumbnails_updater_list,                    MENU_ENUM_SUBLABEL_PL_THUMBNAILS_UPDATER_LIST)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_help_send_debug_info,                          MENU_ENUM_SUBLABEL_HELP_SEND_DEBUG_INFO)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_rdb_entry_detail,                              MENU_ENUM_SUBLABEL_RDB_ENTRY_DETAIL)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_manual_content_scan_list,                      MENU_ENUM_SUBLABEL_MANUAL_CONTENT_SCAN_LIST)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_manual_content_scan_dir,                       MENU_ENUM_SUBLABEL_MANUAL_CONTENT_SCAN_DIR)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_manual_content_scan_system_name,               MENU_ENUM_SUBLABEL_MANUAL_CONTENT_SCAN_SYSTEM_NAME)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_manual_content_scan_system_name_custom,        MENU_ENUM_SUBLABEL_MANUAL_CONTENT_SCAN_SYSTEM_NAME_CUSTOM)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_manual_content_scan_core_name,                 MENU_ENUM_SUBLABEL_MANUAL_CONTENT_SCAN_CORE_NAME)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_manual_content_scan_file_exts,                 MENU_ENUM_SUBLABEL_MANUAL_CONTENT_SCAN_FILE_EXTS)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_manual_content_scan_search_recursively,        MENU_ENUM_SUBLABEL_MANUAL_CONTENT_SCAN_SEARCH_RECURSIVELY)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_manual_content_scan_search_archives,           MENU_ENUM_SUBLABEL_MANUAL_CONTENT_SCAN_SEARCH_ARCHIVES)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_manual_content_scan_dat_file,                  MENU_ENUM_SUBLABEL_MANUAL_CONTENT_SCAN_DAT_FILE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_manual_content_scan_dat_file_filter,           MENU_ENUM_SUBLABEL_MANUAL_CONTENT_SCAN_DAT_FILE_FILTER)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_manual_content_scan_overwrite,                 MENU_ENUM_SUBLABEL_MANUAL_CONTENT_SCAN_OVERWRITE)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_manual_content_scan_start,                     MENU_ENUM_SUBLABEL_MANUAL_CONTENT_SCAN_START)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_core_create_backup,                            MENU_ENUM_SUBLABEL_CORE_CREATE_BACKUP)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_core_restore_backup_list,                      MENU_ENUM_SUBLABEL_CORE_RESTORE_BACKUP_LIST)
DEFAULT_SUBLABEL_MACRO(action_bind_sublabel_core_delete_backup_list,                       MENU_ENUM_SUBLABEL_CORE_DELETE_BACKUP_LIST)

static int action_bind_sublabel_systeminfo_controller_entry(
      file_list_t *list,
      unsigned type, unsigned i,
      const char *label, const char *path,
      char *s, size_t len)
{
   char tmp[4096];
   unsigned controller;

   for(controller = 0; controller < MAX_USERS; controller++)
   {
      if (input_is_autoconfigured(controller))
      {
            snprintf(tmp, sizeof(tmp), "%s #%d device name: %s (#%d)",
               msg_hash_to_str(MENU_ENUM_LABEL_VALUE_PORT),
               controller,
               input_config_get_device_name(controller),
               input_autoconfigure_get_device_name_index(controller));

            if (string_is_equal(path, tmp))
               break;
      }
   }
   snprintf(tmp, sizeof(tmp), "Device display name: %s\nDevice config name: %s\nDevice identifiers: %d/%d",
      input_config_get_device_display_name(controller) ? input_config_get_device_display_name(controller) : msg_hash_to_str(MENU_ENUM_LABEL_VALUE_NOT_AVAILABLE),
      input_config_get_device_display_name(controller) ? input_config_get_device_config_name(controller) : msg_hash_to_str(MENU_ENUM_LABEL_VALUE_NOT_AVAILABLE),
      input_config_get_vid(controller), input_config_get_pid(controller));
   strlcpy(s, tmp, len);

   return 0;
}

static int action_bind_sublabel_bluetooth_list(
      file_list_t *list,
      unsigned type, unsigned i,
      const char *label, const char *path,
      char *s, size_t len)
{
   driver_bluetooth_device_get_sublabel(s, i, len);
   return 0;
}

static int action_bind_sublabel_cheevos_entry(
      file_list_t *list,
      unsigned type, unsigned i,
      const char *label, const char *path,
      char *s, size_t len)
{
#ifdef HAVE_CHEEVOS
   rcheevos_ctx_desc_t desc_info;
   unsigned new_id;
   char fetched_sublabel[MENU_SUBLABEL_MAX_LENGTH];

   fetched_sublabel[0] = '\0';

   new_id              = type - MENU_SETTINGS_CHEEVOS_START;
   desc_info.idx       = new_id;
   desc_info.s         = fetched_sublabel;
   desc_info.len       = len;

   rcheevos_get_description((rcheevos_ctx_desc_t*) &desc_info);

   strlcpy(s, desc_info.s, len);
#endif
   return 0;
}

static int action_bind_sublabel_subsystem_add(
      file_list_t *list,
      unsigned type, unsigned i,
      const char *label, const char *path,
      char *s, size_t len)
{
   rarch_system_info_t *system                  = runloop_get_system_info();
   const struct retro_subsystem_info *subsystem;

   /* Core fully loaded, use the subsystem data */
   if (system->subsystem.data)
      subsystem = system->subsystem.data + (type - MENU_SETTINGS_SUBSYSTEM_ADD);
   /* Core not loaded completely, use the data we peeked on load core */
   else
      subsystem = subsystem_data + (type - MENU_SETTINGS_SUBSYSTEM_ADD);

   if (subsystem && subsystem_current_count > 0)
   {
      if (content_get_subsystem_rom_id() < subsystem->num_roms)
         snprintf(s, len, " Current Content: %s",
            content_get_subsystem() == type - MENU_SETTINGS_SUBSYSTEM_ADD
            ? subsystem->roms[content_get_subsystem_rom_id()].desc
            : subsystem->roms[0].desc);
   }

   return 0;
}

static int action_bind_sublabel_subsystem_load(
      file_list_t *list,
      unsigned type, unsigned i,
      const char *label, const char *path,
      char *s, size_t len)
{
   unsigned j = 0;
   char buf[4096];

   buf[0] = '\0';

   for (j = 0; j < content_get_subsystem_rom_id(); j++)
   {
      strlcat(buf, "   ", sizeof(buf));
      strlcat(buf, path_basename(content_get_subsystem_rom(j)), sizeof(buf));
      if (j != content_get_subsystem_rom_id() - 1)
         strlcat(buf, "\n", sizeof(buf));
   }

   if (!string_is_empty(buf))
      strlcpy(s, buf, len);

   return 0;
}

static int action_bind_sublabel_disk_image_append(
      file_list_t *list,
      unsigned type, unsigned i,
      const char *label, const char *path,
      char *s, size_t len)
{
   rarch_system_info_t *sys_info = runloop_get_system_info();
   enum msg_hash_enums enum_idx  = MENU_ENUM_SUBLABEL_DISK_IMAGE_APPEND;

   /* Check whether disk is currently ejected */
   if (sys_info &&
       disk_control_get_eject_state(&sys_info->disk_control))
      enum_idx = MENU_ENUM_SUBLABEL_DISK_IMAGE_APPEND_TRAY_OPEN;

   strlcpy(s, msg_hash_to_str(enum_idx), len);

   return 1;
}

static int action_bind_sublabel_remap_kbd_sublabel(
      file_list_t *list,
      unsigned type, unsigned i,
      const char *label, const char *path,
      char *s, size_t len)
{
   unsigned user_idx = (type - MENU_SETTINGS_INPUT_DESC_KBD_BEGIN) / RARCH_FIRST_CUSTOM_BIND;

   snprintf(s, len, "%s #%d: %s",
         msg_hash_to_str(MENU_ENUM_LABEL_VALUE_PORT),
         user_idx + 1,
         input_config_get_device_display_name(user_idx) ?
         input_config_get_device_display_name(user_idx) :
         (input_config_get_device_name(user_idx) ?
            input_config_get_device_name(user_idx) :
            msg_hash_to_str(MENU_ENUM_LABEL_VALUE_NOT_AVAILABLE)));
   return 0;
}

#ifdef HAVE_AUDIOMIXER
static int action_bind_sublabel_audio_mixer_stream(
      file_list_t *list,
      unsigned type, unsigned i,
      const char *label, const char *path,
      char *s, size_t len)
{
   char msg[64];
   unsigned              offset = (type - MENU_SETTINGS_AUDIO_MIXER_STREAM_BEGIN);
   audio_mixer_stream_t *stream = audio_driver_mixer_get_stream(offset);

   if (!stream)
      return -1;

   switch (stream->state)
   {
      case AUDIO_STREAM_STATE_NONE:
         strlcpy(msg,
               msg_hash_to_str(MENU_ENUM_LABEL_VALUE_NOT_AVAILABLE),
               sizeof(msg));
         break;
      case AUDIO_STREAM_STATE_STOPPED:
         strlcpy(msg, "Stopped", sizeof(msg));
         break;
      case AUDIO_STREAM_STATE_PLAYING:
         strlcpy(msg, "Playing", sizeof(msg));
         break;
      case AUDIO_STREAM_STATE_PLAYING_LOOPED:
         strlcpy(msg, "Playing (Looped)", sizeof(msg));
         break;
      case AUDIO_STREAM_STATE_PLAYING_SEQUENTIAL:
         strlcpy(msg, "Playing (Sequential)", sizeof(msg));
         break;
   }

   snprintf(s, len, "State : %s | %s: %.2f dB", msg,
         msg_hash_to_str(MENU_ENUM_LABEL_VALUE_MIXER_ACTION_VOLUME),
         stream->volume);
   return 0;
}
#endif

static int action_bind_sublabel_remap_sublabel(
      file_list_t *list,
      unsigned type, unsigned i,
      const char *label, const char *path,
      char *s, size_t len)
{
   unsigned offset = (type - MENU_SETTINGS_INPUT_DESC_BEGIN)
      / (RARCH_FIRST_CUSTOM_BIND + 8);

   snprintf(s, len, "%s #%d: %s",
         msg_hash_to_str(MENU_ENUM_LABEL_VALUE_PORT),
         offset + 1,
         input_config_get_device_display_name(offset) ?
         input_config_get_device_display_name(offset) :
         (input_config_get_device_name(offset) ?
          input_config_get_device_name(offset) :
          msg_hash_to_str(MENU_ENUM_LABEL_VALUE_NOT_AVAILABLE)));
   return 0;
}

#ifdef HAVE_CHEATS
static int action_bind_sublabel_cheat_desc(
      file_list_t *list,
      unsigned type, unsigned i,
      const char *label, const char *path,
      char *s, size_t len)
{
   unsigned offset = (type - MENU_SETTINGS_CHEAT_BEGIN);

   if (cheat_manager_state.cheats)
   {
      if (cheat_manager_state.cheats[offset].handler == CHEAT_HANDLER_TYPE_EMU)
         strlcpy(s, "Emulator-Handled", len);
      else
         strlcpy(s, "RetroArch-Handled", len);
   }

   return 0;
}
#endif

#ifdef HAVE_NETWORKING
static int action_bind_sublabel_netplay_room(
      file_list_t *list,
      unsigned type, unsigned i,
      const char *label, const char *path,
      char *s, size_t len)
{
   uint32_t gamecrc       = 0;
   const char *ra_version = NULL;
   const char *corename   = NULL;
   const char *gamename   = NULL;
   const char *core_ver   = NULL;
   const char *frontend   = NULL;
   const char *na         = NULL;
   const char *subsystem  = NULL;
   unsigned room_index    = type - MENU_SETTINGS_NETPLAY_ROOMS_START;

   if (room_index >= (unsigned)netplay_room_count)
      return menu_cbs_exit();

   ra_version = netplay_room_list[room_index].retroarch_version;
   corename   = netplay_room_list[room_index].corename;
   gamename   = netplay_room_list[room_index].gamename;
   core_ver   = netplay_room_list[room_index].coreversion;
   gamecrc    = netplay_room_list[room_index].gamecrc;
   frontend   = netplay_room_list[room_index].frontend;
   subsystem  = netplay_room_list[room_index].subsystem_name;
   na         = msg_hash_to_str(MENU_ENUM_LABEL_VALUE_NOT_AVAILABLE);

   if (string_is_empty(subsystem) || string_is_equal(subsystem,
            msg_hash_to_str(MENU_ENUM_LABEL_VALUE_NOT_AVAILABLE)))
   {
      snprintf(s, len,
         "%s: %s (%s)\n%s: %s (%s)\nGame: %s (%08x)",
         msg_hash_to_str(MSG_PROGRAM),
         string_is_empty(ra_version)    ? na : ra_version,
         string_is_empty(frontend)      ? na : frontend,
         msg_hash_to_str(MENU_ENUM_LABEL_VALUE_CONTENT_INFO_CORE_NAME),
         corename, core_ver,
         !string_is_equal(gamename, na) ? gamename : na,
         gamecrc);
   }
   else
   {
      if (strstr(gamename, "|"))
      {
         char buf[4096];
         unsigned i               = 0;
         struct string_list *list = string_split(gamename, "|");

         buf[0] = '\0';
         for (i = 0; i < list->size; i++)
         {
            strlcat(buf, "   ", sizeof(buf));
            strlcat(buf, list->elems[i].data, sizeof(buf));
            /* Never terminate a UI string with a newline */
            if (i != list->size - 1)
               strlcat(buf, "\n", sizeof(buf));
         }
         snprintf(s, len,
            "%s: %s (%s)\n%s: %s (%s)\nSubsystem: %s\nGames:\n%s",
            msg_hash_to_str(MSG_PROGRAM),
            string_is_empty(ra_version)    ? na : ra_version,
            string_is_empty(frontend)      ? na : frontend,
            msg_hash_to_str(MENU_ENUM_LABEL_VALUE_CONTENT_INFO_CORE_NAME),
            corename, core_ver, subsystem,
            !string_is_equal(gamename, na) ? buf : na
            );
         string_list_free(list);
      }
      else
      {
         snprintf(s, len,
            "%s: %s (%s)\n%s: %s (%s)\nSubsystem: %s\nGame: %s (%08x)",
            msg_hash_to_str(MSG_PROGRAM),
            string_is_empty(ra_version)    ? na : ra_version,
            string_is_empty(frontend)      ? na : frontend,
            msg_hash_to_str(MENU_ENUM_LABEL_VALUE_CONTENT_INFO_CORE_NAME),
            corename, core_ver, subsystem,
            !string_is_equal(gamename, na) ? gamename : na,
            gamecrc);
      }
   }
   return 0;
}
#endif

static int action_bind_sublabel_playlist_entry(
      file_list_t *list,
      unsigned type, unsigned i,
      const char *label, const char *path,
      char *s, size_t len)
{
   playlist_t *playlist                      = NULL;
   const struct playlist_entry *entry        = NULL;
#ifdef HAVE_OZONE
   const char *menu_ident                    = menu_driver_ident();
#endif
   settings_t *settings                      = config_get_ptr();
   bool playlist_show_sublabels              = settings->bools.playlist_show_sublabels;
   unsigned playlist_sublabel_runtime_type   = settings->uints.playlist_sublabel_runtime_type;
   bool content_runtime_log                  = settings->bools.content_runtime_log;
   bool content_runtime_log_aggregate        = settings->bools.content_runtime_log_aggregate;
   const char *directory_runtime_log         = settings->paths.directory_runtime_log;
   const char *directory_playlist            = settings->paths.directory_playlist;
   enum playlist_sublabel_last_played_style_type
         playlist_sublabel_last_played_style =
               (enum playlist_sublabel_last_played_style_type)
                     settings->uints.playlist_sublabel_last_played_style;
   enum playlist_sublabel_last_played_date_separator_type
         menu_timedate_date_separator        =
               (enum playlist_sublabel_last_played_date_separator_type)
                     settings->uints.menu_timedate_date_separator;

   if (!playlist_show_sublabels)
      return 0;
#ifdef HAVE_OZONE
   if (string_is_equal(menu_ident, "ozone"))
      return 0;
#endif

   /* Get current playlist */
   playlist = playlist_get_cached();

   if (!playlist)
      return 0;

   if (i >= playlist_get_size(playlist))
      return 0;

   /* Read playlist entry */
   playlist_get_index(playlist, i, &entry);

   /* Only add sublabel if a core is currently assigned
    * > Both core name and core path must be valid */
   if (  string_is_empty(entry->core_name) ||
         string_is_equal(entry->core_name, "DETECT") ||
         string_is_empty(entry->core_path) ||
         string_is_equal(entry->core_path, "DETECT"))
      return 0;

   /* Add core name */
   strlcpy(s,
         msg_hash_to_str(MENU_ENUM_LABEL_VALUE_PLAYLIST_SUBLABEL_CORE), len);
   strlcat(s, " ", len);
   strlcat(s, entry->core_name, len);

   /* Get runtime info *if* required runtime log is enabled
    * *and* this is a valid playlist type */
   if (((playlist_sublabel_runtime_type == PLAYLIST_RUNTIME_PER_CORE) &&
         !content_runtime_log) ||
       ((playlist_sublabel_runtime_type == PLAYLIST_RUNTIME_AGGREGATE) &&
         !content_runtime_log_aggregate))
      return 0;

   /* Note: This looks heavy, but each string_is_equal() call will
    * return almost immediately */
   if (!string_is_equal(label, msg_hash_to_str(MENU_ENUM_LABEL_LOAD_CONTENT_HISTORY)) &&
       !string_is_equal(label, msg_hash_to_str(MENU_ENUM_LABEL_HISTORY_TAB)) &&
       !string_is_equal(label, msg_hash_to_str(MENU_ENUM_LABEL_DEFERRED_FAVORITES_LIST)) &&
       !string_is_equal(label, msg_hash_to_str(MENU_ENUM_LABEL_FAVORITES_TAB)) &&
       !string_is_equal(label, msg_hash_to_str(MENU_ENUM_LABEL_DEFERRED_PLAYLIST_LIST)) &&
       !string_is_equal(label, msg_hash_to_str(MENU_ENUM_LABEL_HORIZONTAL_MENU)))
      return 0;

   /* Check whether runtime info should be loaded from log file */
   if (entry->runtime_status == PLAYLIST_RUNTIME_UNKNOWN)
      runtime_update_playlist(
            playlist, i,
            directory_runtime_log,
            directory_playlist,
            (playlist_sublabel_runtime_type == PLAYLIST_RUNTIME_PER_CORE),
            playlist_sublabel_last_played_style,
            menu_timedate_date_separator);

   /* Check whether runtime info is valid */
   if (entry->runtime_status == PLAYLIST_RUNTIME_VALID)
   {
      int n = 0;
      char tmp[64];

      tmp[0  ] = '\n';
      tmp[1  ] = '\0';

      n        = strlcat(tmp, entry->runtime_str, sizeof(tmp));

      tmp[n  ] = '\n';
      tmp[n+1] = '\0';

      /* Runtime/last played strings are now cached in the
       * playlist, so we can add both in one go */
      n = strlcat(tmp, entry->last_played_str, sizeof(tmp));

      if ((n < 0) || (n >= 64))
         n = 0; /* Silence GCC warnings... */

      if (!string_is_empty(tmp))
         strlcat(s, tmp, len);
   }

   return 0;
}

static int action_bind_sublabel_core_option(
      file_list_t *list,
      unsigned type, unsigned i,
      const char *label, const char *path,
      char *s, size_t len)
{
   core_option_manager_t *opt = NULL;
   const char *info           = NULL;

   if (!rarch_ctl(RARCH_CTL_CORE_OPTIONS_LIST_GET, &opt))
      return 0;

   info = core_option_manager_get_info(opt, type - MENU_SETTINGS_CORE_OPTION_START);

   if (!string_is_empty(info))
      strlcpy(s, info, len);

   return 0;
}

#ifdef HAVE_NETWORKING
static int action_bind_sublabel_core_updater_entry(
      file_list_t *list,
      unsigned type, unsigned i,
      const char *label, const char *path,
      char *s, size_t len)
{
   core_updater_list_t *core_list         = core_updater_list_get_cached();
   const core_updater_list_entry_t *entry = NULL;

   /* Set sublabel prefix */
   strlcpy(s, msg_hash_to_str(MENU_ENUM_LABEL_VALUE_CORE_INFO_LICENSES), len);
   strlcat(s, ": ", len);

   /* Search for specified core */
   if (core_list &&
       core_updater_list_get_filename(core_list, path, &entry) &&
       entry->licenses_list)
   {
      char tmp[MENU_SUBLABEL_MAX_LENGTH];
      tmp[0] = '\0';

      /* Add license text */
      string_list_join_concat(tmp, sizeof(tmp),
            entry->licenses_list, ", ");
      strlcat(s, tmp, len);
      return 1;
   }

   /* No license found - set to N/A */
   strlcat(s, msg_hash_to_str(MENU_ENUM_LABEL_VALUE_NOT_AVAILABLE), len);
   return 1;
}
#endif

static int action_bind_sublabel_core_backup_entry(
      file_list_t *list,
      unsigned type, unsigned i,
      const char *label, const char *path,
      char *s, size_t len)
{
   const char *crc = NULL;

   /* crc is entered as 'alt' text */
   menu_entries_get_at_offset(list, i, NULL,
         NULL, NULL, NULL, &crc);

   /* Set sublabel prefix */
   strlcpy(s, msg_hash_to_str(MENU_ENUM_LABEL_VALUE_CORE_BACKUP_CRC), len);

   /* Add crc string */
   strlcat(s, (string_is_empty(crc) ? "00000000" : crc), len);

   return 1;
}

static int action_bind_sublabel_generic(
      file_list_t *list,
      unsigned type, unsigned i,
      const char *label, const char *path,
      char *s, size_t len)
{
   return 0;
}

int menu_cbs_init_bind_sublabel(menu_file_list_cbs_t *cbs,
      const char *path, const char *label, unsigned type, size_t idx)
{
   unsigned i;
   typedef struct info_range_list
   {
      unsigned min;
      unsigned max;
      int (*cb)(file_list_t *list,
            unsigned type, unsigned i,
            const char *label, const char *path,
            char *s, size_t len);
   } info_range_list_t;

   info_range_list_t info_list[] = {
      {
         MENU_SETTINGS_INPUT_DESC_KBD_BEGIN,
         MENU_SETTINGS_INPUT_DESC_KBD_END,
         action_bind_sublabel_remap_kbd_sublabel
      },
#ifdef HAVE_AUDIOMIXER
      {
         MENU_SETTINGS_AUDIO_MIXER_STREAM_ACTIONS_PLAY_BEGIN,
         MENU_SETTINGS_AUDIO_MIXER_STREAM_ACTIONS_PLAY_END,
         menu_action_sublabel_setting_audio_mixer_stream_play
      },
      {
         MENU_SETTINGS_AUDIO_MIXER_STREAM_ACTIONS_PLAY_LOOPED_BEGIN,
         MENU_SETTINGS_AUDIO_MIXER_STREAM_ACTIONS_PLAY_LOOPED_END,
         menu_action_sublabel_setting_audio_mixer_stream_play_looped
      },
      {
         MENU_SETTINGS_AUDIO_MIXER_STREAM_ACTIONS_PLAY_SEQUENTIAL_BEGIN,
         MENU_SETTINGS_AUDIO_MIXER_STREAM_ACTIONS_PLAY_SEQUENTIAL_END,
         menu_action_sublabel_setting_audio_mixer_stream_play_sequential
      },
      {
         MENU_SETTINGS_AUDIO_MIXER_STREAM_ACTIONS_REMOVE_BEGIN,
         MENU_SETTINGS_AUDIO_MIXER_STREAM_ACTIONS_REMOVE_END,
         menu_action_sublabel_setting_audio_mixer_stream_remove
      },
      {
         MENU_SETTINGS_AUDIO_MIXER_STREAM_ACTIONS_STOP_BEGIN,
         MENU_SETTINGS_AUDIO_MIXER_STREAM_ACTIONS_STOP_END,
         menu_action_sublabel_setting_audio_mixer_stream_stop
      },
      {
         MENU_SETTINGS_AUDIO_MIXER_STREAM_ACTIONS_VOLUME_BEGIN,
         MENU_SETTINGS_AUDIO_MIXER_STREAM_ACTIONS_VOLUME_END,
         menu_action_sublabel_setting_audio_mixer_stream_volume
      },
      {
         MENU_SETTINGS_AUDIO_MIXER_STREAM_BEGIN,
         MENU_SETTINGS_AUDIO_MIXER_STREAM_END,
         action_bind_sublabel_audio_mixer_stream
      },
#endif
      {
         MENU_SETTINGS_INPUT_DESC_BEGIN,
         MENU_SETTINGS_INPUT_DESC_END,
         action_bind_sublabel_remap_sublabel
      },
#ifdef HAVE_CHEATS
      {
         MENU_SETTINGS_CHEAT_BEGIN,
         MENU_SETTINGS_CHEAT_END,
         action_bind_sublabel_cheat_desc
      },
#endif
   };

   if (!cbs)
      return -1;

   BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_generic);

   for (i = 0; i < ARRAY_SIZE(info_list); i++)
   {
      if (type >= info_list[i].min && type <= info_list[i].max)
      {
         BIND_ACTION_SUBLABEL(cbs, info_list[i].cb);
         return 0;
      }
   }

   if ((type >= MENU_SETTINGS_CORE_OPTION_START) &&
       (type < MENU_SETTINGS_CHEEVOS_START))
   {
      BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_core_option);
      return 0;
   }

   /* Hotkey binds require special handling */
   if ((cbs->enum_idx >= MENU_ENUM_LABEL_INPUT_HOTKEY_BIND_BEGIN) &&
       (cbs->enum_idx <= MENU_ENUM_LABEL_INPUT_HOTKEY_BIND_END))
   {
      unsigned bind_index = (unsigned)(cbs->enum_idx -
            MENU_ENUM_LABEL_INPUT_HOTKEY_BIND_BEGIN);

      if (bind_index < RARCH_BIND_LIST_END)
      {
         switch (input_config_bind_map_get_retro_key(bind_index))
         {
            case RARCH_FAST_FORWARD_KEY:
               BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_meta_fast_forward_key);
               return 0;
            case RARCH_FAST_FORWARD_HOLD_KEY:
               BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_meta_fast_forward_hold_key);
               return 0;
            case RARCH_SLOWMOTION_KEY:
               BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_meta_slowmotion_key);
               return 0;
            case RARCH_SLOWMOTION_HOLD_KEY:
               BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_meta_slowmotion_hold_key);
               return 0;
            case RARCH_LOAD_STATE_KEY:
               BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_meta_load_state_key);
               return 0;
            case RARCH_SAVE_STATE_KEY:
               BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_meta_save_state_key);
               return 0;
            case RARCH_FULLSCREEN_TOGGLE_KEY:
               BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_meta_fullscreen_toggle_key);
               return 0;
            case RARCH_QUIT_KEY:
#ifdef HAVE_LAKKA
               BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_meta_restart_key);
#else
               BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_meta_quit_key);
#endif
               return 0;
            case RARCH_STATE_SLOT_PLUS:
               BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_meta_state_slot_plus);
               return 0;
            case RARCH_STATE_SLOT_MINUS:
               BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_meta_state_slot_minus);
               return 0;
            case RARCH_REWIND:
               BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_meta_rewind_hotkey);
               return 0;
            case RARCH_BSV_RECORD_TOGGLE:
               BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_meta_bsv_record_toggle);
               return 0;
            case RARCH_PAUSE_TOGGLE:
               BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_meta_pause_toggle);
               return 0;
            case RARCH_FRAMEADVANCE:
               BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_meta_frameadvance);
               return 0;
            case RARCH_RESET:
               BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_meta_reset);
               return 0;
            case RARCH_SHADER_NEXT:
               BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_meta_shader_next);
               return 0;
            case RARCH_SHADER_PREV:
               BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_meta_shader_prev);
               return 0;
            case RARCH_CHEAT_INDEX_PLUS:
#ifdef HAVE_CHEATS
               BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_meta_cheat_index_plus);
#endif
               return 0;
            case RARCH_CHEAT_INDEX_MINUS:
#ifdef HAVE_CHEATS
               BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_meta_cheat_index_minus);
#endif
               return 0;
            case RARCH_CHEAT_TOGGLE:
#ifdef HAVE_CHEATS
               BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_meta_cheat_toggle);
#endif
               return 0;
            case RARCH_SCREENSHOT:
               BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_meta_screenshot);
               return 0;
            case RARCH_MUTE:
               BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_meta_mute);
               return 0;
            case RARCH_OSK:
               BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_meta_osk);
               return 0;
            case RARCH_FPS_TOGGLE:
               BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_meta_fps_toggle);
               return 0;
            case RARCH_SEND_DEBUG_INFO:
               BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_meta_send_debug_info);
               return 0;
            case RARCH_NETPLAY_HOST_TOGGLE:
               BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_meta_netplay_host_toggle);
               return 0;
            case RARCH_NETPLAY_GAME_WATCH:
               BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_meta_netplay_game_watch);
               return 0;
            case RARCH_ENABLE_HOTKEY:
               BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_meta_enable_hotkey);
               return 0;
            case RARCH_VOLUME_UP:
               BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_meta_volume_up);
               return 0;
            case RARCH_VOLUME_DOWN:
               BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_meta_volume_down);
               return 0;
            case RARCH_OVERLAY_NEXT:
               BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_meta_overlay_next);
               return 0;
            case RARCH_DISK_EJECT_TOGGLE:
               BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_meta_disk_eject_toggle);
               return 0;
            case RARCH_DISK_NEXT:
               BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_meta_disk_next);
               return 0;
            case RARCH_DISK_PREV:
               BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_meta_disk_prev);
               return 0;
            case RARCH_GRAB_MOUSE_TOGGLE:
               BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_meta_grab_mouse_toggle);
               return 0;
            case RARCH_GAME_FOCUS_TOGGLE:
               BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_meta_game_focus_toggle);
               return 0;
            case RARCH_UI_COMPANION_TOGGLE:
               BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_meta_ui_companion_toggle);
               return 0;
            case RARCH_MENU_TOGGLE:
               BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_meta_menu_toggle);
               return 0;
            case RARCH_RECORDING_TOGGLE:
               BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_meta_recording_toggle);
               return 0;
            case RARCH_STREAMING_TOGGLE:
               BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_meta_streaming_toggle);
               return 0;
            case RARCH_AI_SERVICE:
               BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_meta_ai_service);
               return 0;
            default:
               break;
         }
      }
   }

   if (cbs->enum_idx != MSG_UNKNOWN)
   {
      switch (cbs->enum_idx)
      {
         case MENU_ENUM_LABEL_FILE_BROWSER_CORE:
         case MENU_ENUM_LABEL_CORE_MANAGER_ENTRY:
            BIND_ACTION_SUBLABEL(cbs, menu_action_sublabel_file_browser_core);
            break;
#ifdef HAVE_NETWORKING
         case MENU_ENUM_LABEL_CORE_UPDATER_ENTRY:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_core_updater_entry);
            break;
#endif
         case MENU_ENUM_LABEL_ADD_TO_MIXER:
         case MENU_ENUM_LABEL_ADD_TO_MIXER_AND_COLLECTION:
#ifdef HAVE_AUDIOMIXER
            BIND_ACTION_SUBLABEL(cbs, menu_action_sublabel_setting_audio_mixer_add_to_mixer);
#endif
            break;
         case MENU_ENUM_LABEL_ADD_TO_MIXER_AND_PLAY:
         case MENU_ENUM_LABEL_ADD_TO_MIXER_AND_COLLECTION_AND_PLAY:
#ifdef HAVE_AUDIOMIXER
            BIND_ACTION_SUBLABEL(cbs, menu_action_sublabel_setting_audio_mixer_add_to_mixer_and_play);
#endif
            break;
         case MENU_ENUM_LABEL_QUICK_MENU_START_STREAMING:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_quick_menu_start_streaming);
            break;
         case MENU_ENUM_LABEL_QUICK_MENU_START_RECORDING:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_quick_menu_start_recording);
            break;
         case MENU_ENUM_LABEL_QUICK_MENU_STOP_STREAMING:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_quick_menu_stop_streaming);
            break;
         case MENU_ENUM_LABEL_QUICK_MENU_STOP_RECORDING:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_quick_menu_stop_recording);
            break;
         case MENU_ENUM_LABEL_QUICK_MENU_OVERRIDE_OPTIONS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_quick_menu_override_options);
            break;
         case MENU_ENUM_LABEL_CRT_SWITCH_RESOLUTION:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_crt_switchres);
            break;
         case MENU_ENUM_LABEL_CRT_SWITCH_RESOLUTION_SUPER:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_crt_switchres_super);
            break;
         case MENU_ENUM_LABEL_CRT_SWITCH_X_AXIS_CENTERING:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_crt_switchres_x_axis_centering);
            break;
         case MENU_ENUM_LABEL_CRT_SWITCH_RESOLUTION_USE_CUSTOM_REFRESH_RATE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_crt_switchres_use_custom_refresh_rate);
            break;
         case MENU_ENUM_LABEL_AUDIO_RESAMPLER_QUALITY:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_audio_resampler_quality);
            break;
         case MENU_ENUM_LABEL_MATERIALUI_ICONS_ENABLE:
#ifdef HAVE_MATERIALUI
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_materialui_icons_enable);
#endif
            break;
         case MENU_ENUM_LABEL_MATERIALUI_LANDSCAPE_LAYOUT_OPTIMIZATION:
#ifdef HAVE_MATERIALUI
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_materialui_landscape_layout_optimization);
#endif
            break;
         case MENU_ENUM_LABEL_MATERIALUI_SHOW_NAV_BAR:
#ifdef HAVE_MATERIALUI
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_materialui_show_nav_bar);
#endif
            break;
         case MENU_ENUM_LABEL_MATERIALUI_AUTO_ROTATE_NAV_BAR:
#ifdef HAVE_MATERIALUI
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_materialui_auto_rotate_nav_bar);
#endif
            break;
         case MENU_ENUM_LABEL_MATERIALUI_DUAL_THUMBNAIL_LIST_VIEW_ENABLE:
#ifdef HAVE_MATERIALUI
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_materialui_dual_thumbnail_list_view_enable);
#endif
            break;
         case MENU_ENUM_LABEL_MATERIALUI_THUMBNAIL_BACKGROUND_ENABLE:
#ifdef HAVE_MATERIALUI
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_materialui_thumbnail_background_enable);
#endif
            break;
         case MENU_ENUM_LABEL_VIDEO_VIEWPORT_CUSTOM_HEIGHT:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_video_viewport_custom_height);
            break;
         case MENU_ENUM_LABEL_VIDEO_VIEWPORT_CUSTOM_WIDTH:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_video_viewport_custom_width);
            break;
         case MENU_ENUM_LABEL_VIDEO_VIEWPORT_CUSTOM_X:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_video_viewport_custom_x);
            break;
         case MENU_ENUM_LABEL_VIDEO_VIEWPORT_CUSTOM_Y:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_video_viewport_custom_y);
            break;
         case MENU_ENUM_LABEL_VIDEO_ASPECT_RATIO:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_video_aspect_ratio);
            break;
         case MENU_ENUM_LABEL_CORE_INFORMATION:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_core_information);
            break;
         case MENU_ENUM_LABEL_DISC_INFORMATION:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_disc_information);
            break;
         case MENU_ENUM_LABEL_CONTENT_SETTINGS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_quick_menu);
            break;
         case MENU_ENUM_LABEL_CHEAT_FILE_SAVE_AS:
#ifdef HAVE_CHEATS
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_cheat_file_save_as);
#endif
            break;
         case MENU_ENUM_LABEL_CHEAT_FILE_LOAD:
#ifdef HAVE_CHEATS
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_cheat_file_load);
#endif
            break;
         case MENU_ENUM_LABEL_CHEAT_FILE_LOAD_APPEND:
#ifdef HAVE_CHEATS
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_cheat_file_load_append);
#endif
            break;
         case MENU_ENUM_LABEL_CHEAT_APPLY_CHANGES:
#ifdef HAVE_CHEATS
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_cheat_apply_changes);
#endif
            break;
         case MENU_ENUM_LABEL_CHEAT_NUM_PASSES:
#ifdef HAVE_CHEATS
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_cheat_num_passes);
#endif
            break;
         case MENU_ENUM_LABEL_VIDEO_SHADER_PARAMETERS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_shader_parameters);
            break;
         case MENU_ENUM_LABEL_VIDEO_SHADER_PRESET_PARAMETERS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_shader_preset_parameters);
            break;
         case MENU_ENUM_LABEL_VIDEO_SHADER_PRESET_SAVE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_shader_preset_save);
            break;
         case MENU_ENUM_LABEL_VIDEO_SHADER_PRESET_REMOVE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_shader_preset_remove);
            break;
         case MENU_ENUM_LABEL_VIDEO_SHADER_PRESET_SAVE_AS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_shader_preset_save_as);
            break;
         case MENU_ENUM_LABEL_VIDEO_SHADER_PRESET_SAVE_GLOBAL:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_shader_preset_save_global);
            break;
         case MENU_ENUM_LABEL_VIDEO_SHADER_PRESET_SAVE_CORE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_shader_preset_save_core);
            break;
         case MENU_ENUM_LABEL_VIDEO_SHADER_PRESET_SAVE_PARENT:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_shader_preset_save_parent);
            break;
         case MENU_ENUM_LABEL_VIDEO_SHADER_PRESET_SAVE_GAME:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_shader_preset_save_game);
            break;
         case MENU_ENUM_LABEL_VIDEO_SHADER_PRESET_REMOVE_GLOBAL:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_shader_preset_remove_global);
            break;
         case MENU_ENUM_LABEL_VIDEO_SHADER_PRESET_REMOVE_CORE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_shader_preset_remove_core);
            break;
         case MENU_ENUM_LABEL_VIDEO_SHADER_PRESET_REMOVE_PARENT:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_shader_preset_remove_parent);
            break;
         case MENU_ENUM_LABEL_VIDEO_SHADER_PRESET_REMOVE_GAME:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_shader_preset_remove_game);
            break;
         case MENU_ENUM_LABEL_VIDEO_SHADER_PRESET:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_shader_preset);
            break;
         case MENU_ENUM_LABEL_VIDEO_SHADER_NUM_PASSES:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_shader_num_passes);
            break;
         case MENU_ENUM_LABEL_SHADER_APPLY_CHANGES:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_shader_apply_changes);
            break;
         case MENU_ENUM_LABEL_SHADER_WATCH_FOR_CHANGES:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_shader_watch_for_changes);
            break;
         case MENU_ENUM_LABEL_VIDEO_FONT_PATH:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_video_font_path);
            break;
         case MENU_ENUM_LABEL_RECORDING_CONFIG_DIRECTORY:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_recording_config_directory);
            break;
         case MENU_ENUM_LABEL_RECORDING_OUTPUT_DIRECTORY:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_recording_output_directory);
            break;
         case MENU_ENUM_LABEL_VIDEO_SHADER_DIR:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_video_shader_directory);
            break;
         case MENU_ENUM_LABEL_AUDIO_FILTER_DIR:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_audio_filter_directory);
            break;
         case MENU_ENUM_LABEL_VIDEO_FILTER_DIR:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_video_filter_directory);
            break;
         case MENU_ENUM_LABEL_CHEAT_DATABASE_PATH:
#ifdef HAVE_CHEATS
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_cheatfile_directory);
#endif
            break;
         case MENU_ENUM_LABEL_OVERLAY_DIRECTORY:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_overlay_directory);
            break;
#ifdef HAVE_VIDEO_LAYOUT
         case MENU_ENUM_LABEL_VIDEO_LAYOUT_DIRECTORY:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_video_layout_directory);
            break;
#endif
         case MENU_ENUM_LABEL_SCREENSHOT_DIRECTORY:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_screenshot_directory);
            break;
         case MENU_ENUM_LABEL_SAVEFILE_DIRECTORY:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_savefile_directory);
            break;
         case MENU_ENUM_LABEL_SAVESTATE_DIRECTORY:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_savestate_directory);
            break;
         case MENU_ENUM_LABEL_ASSETS_DIRECTORY:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_assets_directory);
            break;
         case MENU_ENUM_LABEL_CONTENT_DATABASE_DIRECTORY:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_database_directory);
            break;
         case MENU_ENUM_LABEL_CURSOR_DIRECTORY:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_cursor_directory);
            break;
         case MENU_ENUM_LABEL_CACHE_DIRECTORY:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_cache_directory);
            break;
         case MENU_ENUM_LABEL_PLAYLIST_DIRECTORY:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_playlists_directory);
            break;
         case MENU_ENUM_LABEL_RUNTIME_LOG_DIRECTORY:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_runtime_log_directory);
            break;
         case MENU_ENUM_LABEL_JOYPAD_AUTOCONFIG_DIR:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_joypad_autoconfig_directory);
            break;
         case MENU_ENUM_LABEL_LIBRETRO_INFO_PATH:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_core_info_directory);
            break;
         case MENU_ENUM_LABEL_LIBRETRO_DIR_PATH:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_core_directory);
            break;
         case MENU_ENUM_LABEL_CORE_ASSETS_DIRECTORY:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_core_assets_directory);
            break;
         case MENU_ENUM_LABEL_INPUT_REMAPPING_DIRECTORY:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_remapping_directory);
            break;
         case MENU_ENUM_LABEL_MATERIALUI_MENU_FOOTER_OPACITY:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_footer_opacity);
            break;
         case MENU_ENUM_LABEL_MATERIALUI_MENU_HEADER_OPACITY:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_header_opacity);
            break;
         case MENU_ENUM_LABEL_RGUI_SHOW_START_SCREEN:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_rgui_show_start_screen);
            break;
         case MENU_ENUM_LABEL_CONTENT_SHOW_ADD:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_import_content_tab);
            break;
         case MENU_ENUM_LABEL_CONTENT_SHOW_PLAYLISTS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_playlist_tabs);
            break;
         case MENU_ENUM_LABEL_XMB_MAIN_MENU_ENABLE_SETTINGS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_main_menu_enable_settings);
            break;
         case MENU_ENUM_LABEL_CONTENT_SHOW_HISTORY:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_history_tab);
            break;
         case MENU_ENUM_LABEL_CONTENT_SHOW_SETTINGS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_settings_tab);
            break;
         case MENU_ENUM_LABEL_CONTENT_SHOW_SETTINGS_PASSWORD:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_settings_tab_enable_password);
            break;
         case MENU_ENUM_LABEL_GOTO_IMAGES:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_goto_images);
            break;
         case MENU_ENUM_LABEL_GOTO_MUSIC:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_goto_music);
            break;
         case MENU_ENUM_LABEL_GOTO_VIDEO:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_goto_video);
            break;
         case MENU_ENUM_LABEL_GOTO_FAVORITES:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_goto_favorites);
            break;
         case MENU_ENUM_LABEL_SETTINGS_SHOW_DRIVERS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_settings_show_drivers);
            break;
         case MENU_ENUM_LABEL_SETTINGS_SHOW_VIDEO:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_settings_show_video);
            break;
         case MENU_ENUM_LABEL_SETTINGS_SHOW_AUDIO:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_settings_show_audio);
            break;
         case MENU_ENUM_LABEL_SETTINGS_SHOW_INPUT:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_settings_show_input);
            break;
         case MENU_ENUM_LABEL_SETTINGS_SHOW_LATENCY:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_settings_show_latency);
            break;
         case MENU_ENUM_LABEL_SETTINGS_SHOW_CORE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_settings_show_core);
            break;
         case MENU_ENUM_LABEL_SETTINGS_SHOW_CONFIGURATION:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_settings_show_configuration);
            break;
         case MENU_ENUM_LABEL_SETTINGS_SHOW_SAVING:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_settings_show_saving);
            break;
         case MENU_ENUM_LABEL_SETTINGS_SHOW_LOGGING:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_settings_show_logging);
            break;
         case MENU_ENUM_LABEL_SETTINGS_SHOW_FRAME_THROTTLE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_settings_show_frame_throttle);
            break;
         case MENU_ENUM_LABEL_FRAME_TIME_COUNTER_RESET_AFTER_FASTFORWARDING:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_frame_time_counter_reset_after_fastforwarding);
            break;
         case MENU_ENUM_LABEL_FRAME_TIME_COUNTER_RESET_AFTER_LOAD_STATE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_frame_time_counter_reset_after_load_state);
            break;
         case MENU_ENUM_LABEL_FRAME_TIME_COUNTER_RESET_AFTER_SAVE_STATE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_frame_time_counter_reset_after_save_state);
            break;
         case MENU_ENUM_LABEL_SETTINGS_SHOW_RECORDING:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_settings_show_recording);
            break;
         case MENU_ENUM_LABEL_SETTINGS_SHOW_ONSCREEN_DISPLAY:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_settings_show_onscreen_display);
            break;
         case MENU_ENUM_LABEL_SETTINGS_SHOW_USER_INTERFACE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_settings_show_user_interface);
            break;
         case MENU_ENUM_LABEL_SETTINGS_SHOW_AI_SERVICE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_settings_show_ai_service);
            break;
         case MENU_ENUM_LABEL_SETTINGS_SHOW_POWER_MANAGEMENT:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_settings_show_power_management);
            break;
         case MENU_ENUM_LABEL_SETTINGS_SHOW_ACHIEVEMENTS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_settings_show_achievements);
            break;
         case MENU_ENUM_LABEL_SETTINGS_SHOW_NETWORK:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_settings_show_network);
            break;
         case MENU_ENUM_LABEL_SETTINGS_SHOW_PLAYLISTS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_settings_show_playlists);
            break;
         case MENU_ENUM_LABEL_SETTINGS_SHOW_USER:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_settings_show_user);
            break;
         case MENU_ENUM_LABEL_SETTINGS_SHOW_DIRECTORY:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_settings_show_directory);
            break;
         case MENU_ENUM_LABEL_QUICK_MENU_SHOW_RESUME_CONTENT:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_quick_menu_show_resume_content);
            break;
         case MENU_ENUM_LABEL_QUICK_MENU_SHOW_RESTART_CONTENT:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_quick_menu_show_restart_content);
            break;
         case MENU_ENUM_LABEL_QUICK_MENU_SHOW_CLOSE_CONTENT:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_quick_menu_show_close_content);
            break;
         case MENU_ENUM_LABEL_QUICK_MENU_SHOW_TAKE_SCREENSHOT:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_quick_menu_show_take_screenshot);
            break;
         case MENU_ENUM_LABEL_QUICK_MENU_SHOW_SAVE_LOAD_STATE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_quick_menu_show_save_load_state);
            break;
         case MENU_ENUM_LABEL_QUICK_MENU_SHOW_UNDO_SAVE_LOAD_STATE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_quick_menu_show_undo_save_load_state);
            break;
         case MENU_ENUM_LABEL_QUICK_MENU_SHOW_ADD_TO_FAVORITES:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_quick_menu_show_add_to_favorites);
            break;
         case MENU_ENUM_LABEL_QUICK_MENU_SHOW_START_RECORDING:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_quick_menu_show_start_recording);
            break;
         case MENU_ENUM_LABEL_QUICK_MENU_SHOW_START_STREAMING:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_quick_menu_show_start_streaming);
            break;
         case MENU_ENUM_LABEL_QUICK_MENU_SHOW_SET_CORE_ASSOCIATION:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_quick_menu_show_set_core_association);
            break;
         case MENU_ENUM_LABEL_QUICK_MENU_SHOW_RESET_CORE_ASSOCIATION:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_quick_menu_show_reset_core_association);
            break;
         case MENU_ENUM_LABEL_QUICK_MENU_SHOW_OPTIONS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_quick_menu_show_options);
            break;
         case MENU_ENUM_LABEL_QUICK_MENU_SHOW_CONTROLS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_quick_menu_show_controls);
            break;
         case MENU_ENUM_LABEL_QUICK_MENU_SHOW_CHEATS:
#ifdef HAVE_CHEATS
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_quick_menu_show_cheats);
#endif
            break;
         case MENU_ENUM_LABEL_CONTENT_SHOW_LATENCY:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_content_show_latency);
            break;
         case MENU_ENUM_LABEL_CONTENT_SHOW_REWIND:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_content_show_rewind);
            break;
         case MENU_ENUM_LABEL_CONTENT_SHOW_OVERLAYS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_content_show_overlays);
            break;
#ifdef HAVE_VIDEO_LAYOUT
         case MENU_ENUM_LABEL_CONTENT_SHOW_VIDEO_LAYOUT:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_content_show_video_layout);
            break;
#endif
         case MENU_ENUM_LABEL_QUICK_MENU_SHOW_SHADERS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_quick_menu_show_shaders);
            break;
         case MENU_ENUM_LABEL_QUICK_MENU_SHOW_SAVE_CORE_OVERRIDES:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_quick_menu_show_save_core_overrides);
            break;
         case MENU_ENUM_LABEL_QUICK_MENU_SHOW_SAVE_GAME_OVERRIDES:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_quick_menu_show_save_game_overrides);
            break;
         case MENU_ENUM_LABEL_QUICK_MENU_SHOW_INFORMATION:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_quick_menu_show_information);
            break;
         case MENU_ENUM_LABEL_QUICK_MENU_SHOW_DOWNLOAD_THUMBNAILS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_quick_menu_show_download_thumbnails);
            break;
         case MENU_ENUM_LABEL_MENU_ENABLE_KIOSK_MODE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_enable_kiosk_mode);
            break;
         case MENU_ENUM_LABEL_MENU_DISABLE_KIOSK_MODE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_disable_kiosk_mode);
            break;
         case MENU_ENUM_LABEL_MENU_KIOSK_MODE_PASSWORD:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_kiosk_mode_password);
            break;
         case MENU_ENUM_LABEL_CONTENT_SHOW_FAVORITES:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_favorites_tab);
            break;
         case MENU_ENUM_LABEL_CONTENT_SHOW_IMAGES:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_images_tab);
            break;
         case MENU_ENUM_LABEL_CONTENT_SHOW_MUSIC:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_music_tab);
            break;
         case MENU_ENUM_LABEL_MENU_SHOW_LOAD_CORE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_show_load_core);
            break;
         case MENU_ENUM_LABEL_LOAD_DISC:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_load_disc);
            break;
         case MENU_ENUM_LABEL_DUMP_DISC:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_dump_disc);
            break;
         case MENU_ENUM_LABEL_MENU_SHOW_LOAD_CONTENT:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_show_load_content);
            break;
         case MENU_ENUM_LABEL_MENU_SHOW_LOAD_DISC:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_show_load_disc);
            break;
         case MENU_ENUM_LABEL_MENU_SHOW_DUMP_DISC:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_show_dump_disc);
            break;
         case MENU_ENUM_LABEL_MENU_SHOW_INFORMATION:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_show_information);
            break;
         case MENU_ENUM_LABEL_MENU_SHOW_CONFIGURATIONS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_show_configurations);
            break;
         case MENU_ENUM_LABEL_MENU_SHOW_HELP:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_show_help);
            break;
         case MENU_ENUM_LABEL_MENU_SHOW_QUIT_RETROARCH:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_show_quit_retroarch);
            break;
#ifndef HAVE_LAKKA
         case MENU_ENUM_LABEL_MENU_SHOW_RESTART_RETROARCH:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_show_restart_retroarch);
            break;
#endif
         case MENU_ENUM_LABEL_MENU_SHOW_REBOOT:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_show_reboot);
            break;
         case MENU_ENUM_LABEL_MENU_SHOW_SHUTDOWN:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_show_shutdown);
            break;
         case MENU_ENUM_LABEL_MENU_SHOW_ONLINE_UPDATER:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_show_online_updater);
            break;
         case MENU_ENUM_LABEL_MENU_SHOW_CORE_UPDATER:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_show_core_updater);
            break;
         case MENU_ENUM_LABEL_MENU_SHOW_LEGACY_THUMBNAIL_UPDATER:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_show_legacy_thumbnail_updater);
            break;
         case MENU_ENUM_LABEL_MENU_SCROLL_FAST:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_scroll_fast);
            break;
         case MENU_ENUM_LABEL_CONTENT_SHOW_NETPLAY:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_netplay_tab);
            break;
         case MENU_ENUM_LABEL_CONTENT_SHOW_VIDEO:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_video_tab);
            break;
         case MENU_ENUM_LABEL_XMB_FONT:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_font);
            break;
         case MENU_ENUM_LABEL_XMB_RIBBON_ENABLE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_ribbon_enable);
            break;
         case MENU_ENUM_LABEL_MENU_FRAMEBUFFER_OPACITY:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_framebuffer_opacity);
            break;
         case MENU_ENUM_LABEL_MENU_HORIZONTAL_ANIMATION:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_horizontal_animation);
            break;
         case MENU_ENUM_LABEL_MENU_XMB_ANIMATION_HORIZONTAL_HIGHLIGHT:
#ifdef HAVE_XMB
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_xmb_animation_horizontal_higlight);
#endif
            break;
         case MENU_ENUM_LABEL_MENU_XMB_ANIMATION_MOVE_UP_DOWN:
#ifdef HAVE_XMB
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_xmb_animation_move_up_down);
#endif
            break;
         case MENU_ENUM_LABEL_MENU_XMB_ANIMATION_OPENING_MAIN_MENU:
#ifdef HAVE_XMB
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_xmb_animation_opening_main_menu);
#endif
            break;
         case MENU_ENUM_LABEL_MENU_SCALE_FACTOR:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_scale_factor);
            break;
         case MENU_ENUM_LABEL_MENU_WIDGET_SCALE_AUTO:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_widget_scale_auto);
            break;
         case MENU_ENUM_LABEL_MENU_WIDGET_SCALE_FACTOR:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_widget_scale_factor);
            break;
#if !(defined(RARCH_CONSOLE) || defined(RARCH_MOBILE))
         case MENU_ENUM_LABEL_MENU_WIDGET_SCALE_FACTOR_WINDOWED:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_widget_scale_factor_windowed);
            break;
#endif
         case MENU_ENUM_LABEL_MENU_WALLPAPER_OPACITY:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_wallpaper_opacity);
            break;
         case MENU_ENUM_LABEL_MENU_USE_PREFERRED_SYSTEM_COLOR_THEME:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_use_preferred_system_color_theme);
            break;
         case MENU_ENUM_LABEL_OZONE_MENU_COLOR_THEME:
#ifdef HAVE_OZONE
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_ozone_menu_color_theme);
#endif
            break;
         case MENU_ENUM_LABEL_OZONE_COLLAPSE_SIDEBAR:
#ifdef HAVE_OZONE
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_ozone_collapse_sidebar);
#endif
            break;
         case MENU_ENUM_LABEL_OZONE_TRUNCATE_PLAYLIST_NAME:
#ifdef HAVE_OZONE
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_ozone_truncate_playlist_name);
#endif
            break;
         case MENU_ENUM_LABEL_OZONE_SORT_AFTER_TRUNCATE_PLAYLIST_NAME:
#ifdef HAVE_OZONE
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_ozone_sort_after_truncate_playlist_name);
#endif
            break;
         case MENU_ENUM_LABEL_OZONE_SCROLL_CONTENT_METADATA:
#ifdef HAVE_OZONE
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_ozone_scroll_content_metadata);
#endif
            break;
         case MENU_ENUM_LABEL_MATERIALUI_MENU_COLOR_THEME:
         case MENU_ENUM_LABEL_XMB_MENU_COLOR_THEME:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_color_theme);
            break;
         case MENU_ENUM_LABEL_MATERIALUI_MENU_TRANSITION_ANIMATION:
#ifdef HAVE_MATERIALUI
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_materialui_menu_transition_animation);
#endif
            break;
         case MENU_ENUM_LABEL_MATERIALUI_MENU_THUMBNAIL_VIEW_PORTRAIT:
#ifdef HAVE_MATERIALUI
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_materialui_menu_thumbnail_view_portrait);
#endif
            break;
         case MENU_ENUM_LABEL_MATERIALUI_MENU_THUMBNAIL_VIEW_LANDSCAPE:
#ifdef HAVE_MATERIALUI
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_materialui_menu_thumbnail_view_landscape);
#endif
            break;
         case MENU_ENUM_LABEL_XMB_SHADOWS_ENABLE:
#ifdef HAVE_XMB
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_xmb_shadows_enable);
#endif
            break;
         case MENU_ENUM_LABEL_XMB_VERTICAL_THUMBNAILS:
#ifdef HAVE_XMB
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_xmb_vertical_thumbnails);
#endif
            break;
         case MENU_ENUM_LABEL_MENU_XMB_THUMBNAIL_SCALE_FACTOR:
#ifdef HAVE_XMB
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_xmb_thumbnail_scale_factor);
#endif
            break;
         case MENU_ENUM_LABEL_XMB_LAYOUT:
#ifdef HAVE_XMB
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_xmb_layout);
#endif
            break;
         case MENU_ENUM_LABEL_XMB_THEME:
#ifdef HAVE_XMB
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_xmb_icon_theme);
#endif
            break;
         case MENU_ENUM_LABEL_MENU_THROTTLE_FRAMERATE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_throttle_framerate);
            break;
         case MENU_ENUM_LABEL_DISK_IMAGE_APPEND:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_disk_image_append);
            break;
         case MENU_ENUM_LABEL_SUBSYSTEM_ADD:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_subsystem_add);
            break;
         case MENU_ENUM_LABEL_SUBSYSTEM_LOAD:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_subsystem_load);
            break;
         case MENU_ENUM_LABEL_DISK_TRAY_EJECT:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_disk_tray_eject);
            break;
         case MENU_ENUM_LABEL_DISK_TRAY_INSERT:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_disk_tray_insert);
            break;
         case MENU_ENUM_LABEL_DISK_INDEX:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_disk_index);
            break;
         case MENU_ENUM_LABEL_DISK_OPTIONS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_disk_options);
            break;
         case MENU_ENUM_LABEL_NETPLAY_INPUT_LATENCY_FRAMES_RANGE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_latency_frames_range);
            break;
         case MENU_ENUM_LABEL_NETPLAY_INPUT_LATENCY_FRAMES_MIN:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_latency_frames);
            break;
         case MENU_ENUM_LABEL_RGUI_CONFIG_DIRECTORY:
            BIND_ACTION_SUBLABEL(cbs, action_bind_rgui_config_directory);
            break;
         case MENU_ENUM_LABEL_THUMBNAILS_DIRECTORY:
            BIND_ACTION_SUBLABEL(cbs, action_bind_thumbnails_directory);
            break;
         case MENU_ENUM_LABEL_DYNAMIC_WALLPAPERS_DIRECTORY:
            BIND_ACTION_SUBLABEL(cbs, action_bind_dynamic_wallpapers_directory);
            break;
         case MENU_ENUM_LABEL_CONTENT_DIR:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_content_dir);
            break;
         case MENU_ENUM_LABEL_RGUI_BROWSER_DIRECTORY:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_rgui_browser_directory);
            break;
         case MENU_ENUM_LABEL_SYSTEM_DIRECTORY:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_system_directory);
            break;
         case MENU_ENUM_LABEL_PLAYLIST_ENTRY_RENAME:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_playlist_entry_rename);
            break;
         case MENU_ENUM_LABEL_PLAYLIST_ENTRY_REMOVE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_playlist_entry_remove);
            break;
         case MENU_ENUM_LABEL_THREADED_DATA_RUNLOOP_ENABLE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_threaded_data_runloop_enable);
            break;
         case MENU_ENUM_LABEL_SHOW_ADVANCED_SETTINGS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_show_advanced_settings);
            break;
         case MENU_ENUM_LABEL_CORE_OPTIONS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_core_options);
            break;
         case MENU_ENUM_LABEL_CORE_INPUT_REMAPPING_OPTIONS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_core_input_remapping_options);
            break;
         case MENU_ENUM_LABEL_CORE_CHEAT_OPTIONS:
#ifdef HAVE_CHEATS
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_core_cheat_options);
#endif
            break;
         case MENU_ENUM_LABEL_SHADER_OPTIONS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_shader_options);
            break;
         case MENU_ENUM_LABEL_RESET_TO_DEFAULT_CONFIG:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_reset_to_default_config);
            break;
         case MENU_ENUM_LABEL_SAVE_CURRENT_CONFIG_OVERRIDE_GAME:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_save_current_config_override_game);
            break;
         case MENU_ENUM_LABEL_SAVE_CURRENT_CONFIG_OVERRIDE_CORE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_save_current_config_override_core);
            break;
         case MENU_ENUM_LABEL_SAVE_CURRENT_CONFIG_OVERRIDE_CONTENT_DIR:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_save_current_config_override_content_dir);
            break;
         case MENU_ENUM_LABEL_RESTART_CONTENT:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_restart_content);
            break;
         case MENU_ENUM_LABEL_REWIND_SETTINGS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_meta_rewind);
            break;
         case MENU_ENUM_LABEL_CHEAT_DETAILS_SETTINGS:
#ifdef HAVE_CHEATS
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_meta_cheat_details);
#endif
            break;
         case MENU_ENUM_LABEL_CHEAT_SEARCH_SETTINGS:
#ifdef HAVE_CHEATS
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_meta_cheat_search);
#endif
            break;
         case MENU_ENUM_LABEL_ACCOUNTS_LIST:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_accounts_list);
            break;
         case MENU_ENUM_LABEL_ACCOUNTS_RETRO_ACHIEVEMENTS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_accounts_retro_achievements);
            break;
         case MENU_ENUM_LABEL_UNDO_SAVE_STATE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_undo_save_state);
            break;
         case MENU_ENUM_LABEL_UNDO_LOAD_STATE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_undo_load_state);
            break;
         case MENU_ENUM_LABEL_STATE_SLOT:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_state_slot);
            break;
         case MENU_ENUM_LABEL_RESUME:
         case MENU_ENUM_LABEL_RESUME_CONTENT:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_resume_content);
            break;
         case MENU_ENUM_LABEL_SAVE_STATE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_save_state);
            break;
         case MENU_ENUM_LABEL_LOAD_STATE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_load_state);
            break;
         case MENU_ENUM_LABEL_CLOSE_CONTENT:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_close_content);
            break;
         case MENU_ENUM_LABEL_TAKE_SCREENSHOT:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_take_screenshot);
            break;
         case MENU_ENUM_LABEL_CURSOR_MANAGER:
         case MENU_ENUM_LABEL_CURSOR_MANAGER_LIST:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_cursor_manager);
            break;
         case MENU_ENUM_LABEL_DATABASE_MANAGER:
         case MENU_ENUM_LABEL_DATABASE_MANAGER_LIST:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_database_manager);
            break;
         case MENU_ENUM_LABEL_CORE_ENABLE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_core_enable);
            break;
         case MENU_ENUM_LABEL_GAME_SPECIFIC_OPTIONS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_game_specific_options);
            break;
         case MENU_ENUM_LABEL_GLOBAL_CORE_OPTIONS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_global_core_options);
            break;
         case MENU_ENUM_LABEL_AUTO_OVERRIDES_ENABLE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_auto_overrides_enable);
            break;
         case MENU_ENUM_LABEL_AUTO_REMAPS_ENABLE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_auto_remaps_enable);
            break;
         case MENU_ENUM_LABEL_MENU_FILE_BROWSER_SETTINGS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_filebrowser_settings);
            break;
         case MENU_ENUM_LABEL_FILE_BROWSER_OPEN_UWP_PERMISSIONS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_filebrowser_open_uwp_permissions);
            break;
         case MENU_ENUM_LABEL_FILE_BROWSER_OPEN_PICKER:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_filebrowser_open_picker);
            break;
         case MENU_ENUM_LABEL_ADD_TO_FAVORITES:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_add_to_favorites);
            break;
         case MENU_ENUM_LABEL_DOWNLOAD_PL_ENTRY_THUMBNAILS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_download_pl_entry_thumbnails);
            break;
         case MENU_ENUM_LABEL_RUN:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_run);
            break;
         case MENU_ENUM_LABEL_INFORMATION:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_information);
            break;
         case MENU_ENUM_LABEL_RENAME_ENTRY:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_rename_entry);
            break;
         case MENU_ENUM_LABEL_DELETE_ENTRY:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_delete_entry);
            break;
         case MENU_ENUM_LABEL_NETPLAY_REFRESH_ROOMS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_netplay_refresh_rooms);
            break;
         case MENU_ENUM_LABEL_CORE_UPDATER_AUTO_EXTRACT_ARCHIVE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_core_updater_auto_extract_archive);
            break;
         case MENU_ENUM_LABEL_CORE_UPDATER_SHOW_EXPERIMENTAL_CORES:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_core_updater_show_experimental_cores);
            break;
         case MENU_ENUM_LABEL_CORE_UPDATER_AUTO_BACKUP:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_core_updater_auto_backup);
            break;
         case MENU_ENUM_LABEL_CORE_UPDATER_AUTO_BACKUP_HISTORY_SIZE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_core_updater_auto_backup_history_size);
            break;
         case MENU_ENUM_LABEL_CORE_UPDATER_BUILDBOT_URL:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_core_updater_buildbot_url);
            break;
         case MENU_ENUM_LABEL_BUILDBOT_ASSETS_URL:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_core_updater_buildbot_assets_url);
            break;
         case MENU_ENUM_LABEL_SORT_SAVEFILES_ENABLE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_sort_savefiles_enable);
            break;
         case MENU_ENUM_LABEL_SORT_SAVESTATES_ENABLE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_sort_savestates_enable);
            break;
         case MENU_ENUM_LABEL_VIDEO_SWAP_INTERVAL:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_video_swap_interval);
            break;
         case MENU_ENUM_LABEL_SCAN_FILE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_scan_file);
            break;
         case MENU_ENUM_LABEL_SCAN_DIRECTORY:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_scan_directory);
            break;
         case MENU_ENUM_LABEL_NETPLAY_DISCONNECT:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_netplay_disconnect);
            break;
         case MENU_ENUM_LABEL_NETPLAY_ENABLE_CLIENT:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_netplay_enable_client);
            break;
         case MENU_ENUM_LABEL_NETPLAY_ENABLE_HOST:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_netplay_enable_host);
            break;
         case MENU_ENUM_LABEL_NAVIGATION_WRAPAROUND:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_navigation_wraparound);
            break;
         case MENU_ENUM_LABEL_BATTERY_LEVEL_ENABLE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_battery_level_enable);
            break;
         case MENU_ENUM_LABEL_MENU_SHOW_SUBLABELS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_show_sublabels);
            break;
         case MENU_ENUM_LABEL_TIMEDATE_ENABLE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_timedate_enable);
            break;
         case MENU_ENUM_LABEL_TIMEDATE_STYLE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_timedate_style);
            break;
         case MENU_ENUM_LABEL_TIMEDATE_DATE_SEPARATOR:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_timedate_date_separator);
            break;
         case MENU_ENUM_LABEL_THUMBNAILS:
            {
               const char *menu_ident             = menu_driver_ident();
#ifdef HAVE_RGUI
               if (string_is_equal(menu_ident, "rgui"))
               {
                  BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_thumbnails_rgui);
               }
               else
#endif
#ifdef HAVE_MATERIALUI
                  if (string_is_equal(menu_ident, "glui"))
                  {
                     BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_thumbnails_materialui);
                  }
                  else
#endif
                  {
                     BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_thumbnails);
                  }
            }
            break;
         case MENU_ENUM_LABEL_LEFT_THUMBNAILS:
            {
               const char *menu_ident             = menu_driver_ident();
#ifdef HAVE_RGUI
               if (string_is_equal(menu_ident, "rgui"))
               {
                  BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_left_thumbnails_rgui);
               }
               else
#endif
#ifdef HAVE_OZONE
                  if (string_is_equal(menu_ident, "ozone"))
                  {
                     BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_left_thumbnails_ozone);
                  }
                  else
#endif
#ifdef HAVE_MATERIALUI
                     if (string_is_equal(menu_ident, "glui"))
                     {
                        BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_left_thumbnails_materialui);
                     }
                     else
#endif
                     {
                        BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_left_thumbnails);
                     }
            }
            break;
         case MENU_ENUM_LABEL_MENU_THUMBNAIL_UPSCALE_THRESHOLD:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_thumbnail_upscale_threshold);
            break;
         case MENU_ENUM_LABEL_MOUSE_ENABLE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_mouse_enable);
            break;
         case MENU_ENUM_LABEL_POINTER_ENABLE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_pointer_enable);
            break;
         case MENU_ENUM_LABEL_STDIN_CMD_ENABLE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_stdin_cmd_enable);
            break;
         case MENU_ENUM_LABEL_NETPLAY_PUBLIC_ANNOUNCE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_netplay_public_announce);
            break;
         case MENU_ENUM_LABEL_NETPLAY_NAT_TRAVERSAL:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_netplay_nat_traversal);
            break;
         case MENU_ENUM_LABEL_NETPLAY_CHECK_FRAMES:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_netplay_check_frames);
            break;
         case MENU_ENUM_LABEL_NETPLAY_START_AS_SPECTATOR:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_netplay_start_as_spectator);
            break;
         case MENU_ENUM_LABEL_NETPLAY_ALLOW_SLAVES:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_netplay_allow_slaves);
            break;
         case MENU_ENUM_LABEL_NETPLAY_REQUIRE_SLAVES:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_netplay_require_slaves);
            break;
         case MENU_ENUM_LABEL_NETPLAY_STATELESS_MODE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_netplay_stateless_mode);
            break;
         case MENU_ENUM_LABEL_NETPLAY_PASSWORD:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_netplay_password);
            break;
         case MENU_ENUM_LABEL_NETPLAY_SPECTATE_PASSWORD:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_netplay_spectate_password);
            break;
         case MENU_ENUM_LABEL_NETPLAY_TCP_UDP_PORT:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_netplay_tcp_udp_port);
            break;
         case MENU_ENUM_LABEL_NETPLAY_IP_ADDRESS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_netplay_ip_address);
            break;
         case MENU_ENUM_LABEL_OVERLAY_PRESET:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_overlay_preset);
            break;
         case MENU_ENUM_LABEL_INPUT_OVERLAY_ENABLE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_overlay_enable);
            break;
         case MENU_ENUM_LABEL_OVERLAY_OPACITY:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_overlay_opacity);
            break;
         case MENU_ENUM_LABEL_OVERLAY_SCALE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_overlay_scale);
            break;
         case MENU_ENUM_LABEL_OVERLAY_CENTER_X:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_overlay_center_x);
            break;
         case MENU_ENUM_LABEL_OVERLAY_CENTER_Y:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_overlay_center_y);
            break;
#ifdef HAVE_VIDEO_LAYOUT
         case MENU_ENUM_LABEL_VIDEO_LAYOUT_ENABLE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_video_layout_enable);
            break;
         case MENU_ENUM_LABEL_VIDEO_LAYOUT_PATH:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_video_layout_path);
            break;
#endif
         case MENU_ENUM_LABEL_AUDIO_DSP_PLUGIN:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_audio_dsp_plugin);
            break;
         case MENU_ENUM_LABEL_AUDIO_OUTPUT_RATE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_audio_output_rate);
            break;
         case MENU_ENUM_LABEL_AUDIO_DEVICE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_audio_device);
            break;
         case MENU_ENUM_LABEL_AUDIO_WASAPI_EXCLUSIVE_MODE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_audio_wasapi_exclusive_mode);
            break;
         case MENU_ENUM_LABEL_AUDIO_WASAPI_FLOAT_FORMAT:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_audio_wasapi_float_format);
            break;
         case MENU_ENUM_LABEL_AUDIO_WASAPI_SH_BUFFER_LENGTH:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_audio_wasapi_sh_buffer_length);
            break;
         case MENU_ENUM_LABEL_MENU_WALLPAPER:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_wallpaper);
            break;
         case MENU_ENUM_LABEL_DYNAMIC_WALLPAPER:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_dynamic_wallpaper);
            break;
         case MENU_ENUM_LABEL_NAVIGATION_BROWSER_FILTER_SUPPORTED_EXTENSIONS_ENABLE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_filter_supported_extensions);
            break;
         case MENU_ENUM_LABEL_BLUETOOTH_DRIVER:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_bluetooth_driver);
            break;
         case MENU_ENUM_LABEL_WIFI_DRIVER:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_wifi_driver);
            break;
         case MENU_ENUM_LABEL_RECORD_DRIVER:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_record_driver);
            break;
         case MENU_ENUM_LABEL_MIDI_DRIVER:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_midi_driver);
            break;
         case MENU_ENUM_LABEL_MENU_DRIVER:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_driver);
            break;
         case MENU_ENUM_LABEL_LOCATION_DRIVER:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_location_driver);
            break;
         case MENU_ENUM_LABEL_CAMERA_DRIVER:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_camera_driver);
            break;
         case MENU_ENUM_LABEL_AUDIO_RESAMPLER_DRIVER:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_audio_resampler_driver);
            break;
         case MENU_ENUM_LABEL_JOYPAD_DRIVER:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_joypad_driver);
            break;
         case MENU_ENUM_LABEL_INPUT_DRIVER:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_driver);
            break;
         case MENU_ENUM_LABEL_AUDIO_DRIVER:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_audio_driver);
            break;
         case MENU_ENUM_LABEL_VIDEO_DRIVER:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_video_driver);
            break;
         case MENU_ENUM_LABEL_PAUSE_LIBRETRO:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_pause_libretro);
            break;
         case MENU_ENUM_LABEL_MENU_SAVESTATE_RESUME:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_savestate_resume);
            break;
         case MENU_ENUM_LABEL_MENU_INSERT_DISK_RESUME:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_insert_disk_resume);
            break;
         case MENU_ENUM_LABEL_MENU_INPUT_SWAP_OK_CANCEL:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_swap_ok_cancel);
            break;
         case MENU_ENUM_LABEL_INPUT_AUTODETECT_ENABLE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_autodetect_enable);
            break;
         case MENU_ENUM_LABEL_INPUT_REMAP_BINDS_ENABLE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_remap_binds_enable);
            break;
         case MENU_ENUM_LABEL_AUTOSAVE_INTERVAL:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_autosave_interval);
            break;
         case MENU_ENUM_LABEL_SAVESTATE_THUMBNAIL_ENABLE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_savestate_thumbnail_enable);
            break;
         case MENU_ENUM_LABEL_SAVE_FILE_COMPRESSION:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_save_file_compression);
            break;
         case MENU_ENUM_LABEL_SAVESTATE_FILE_COMPRESSION:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_savestate_file_compression);
            break;
         case MENU_ENUM_LABEL_SAVESTATE_AUTO_SAVE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_savestate_auto_save);
            break;
         case MENU_ENUM_LABEL_SAVESTATE_AUTO_LOAD:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_savestate_auto_load);
            break;
         case MENU_ENUM_LABEL_PERFCNT_ENABLE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_perfcnt_enable);
            break;
         case MENU_ENUM_LABEL_FRONTEND_LOG_LEVEL:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_frontend_log_level);
            break;
         case MENU_ENUM_LABEL_LIBRETRO_LOG_LEVEL:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_libretro_log_level);
            break;
         case MENU_ENUM_LABEL_REWIND_ENABLE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_rewind);
            break;
         case MENU_ENUM_LABEL_CHEAT_APPLY_AFTER_TOGGLE:
#ifdef HAVE_CHEATS
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_cheat_apply_after_toggle);
#endif
            break;
         case MENU_ENUM_LABEL_REWIND_GRANULARITY:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_rewind_granularity);
            break;
         case MENU_ENUM_LABEL_REWIND_BUFFER_SIZE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_rewind_buffer_size);
            break;
         case MENU_ENUM_LABEL_REWIND_BUFFER_SIZE_STEP:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_rewind_buffer_size_step);
            break;
         case MENU_ENUM_LABEL_CHEAT_IDX:
#ifdef HAVE_CHEATS
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_cheat_idx);
#endif
            break;
         case MENU_ENUM_LABEL_CHEAT_MATCH_IDX:
#ifdef HAVE_CHEATS
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_cheat_match_idx);
#endif
            break;
         case MENU_ENUM_LABEL_CHEAT_BIG_ENDIAN:
#ifdef HAVE_CHEATS
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_cheat_big_endian);
#endif
            break;
         case MENU_ENUM_LABEL_CHEAT_START_OR_CONT:
#ifdef HAVE_CHEATS
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_cheat_start_or_cont);
#endif
            break;
         case MENU_ENUM_LABEL_CHEAT_START_OR_RESTART:
#ifdef HAVE_CHEATS
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_cheat_start_or_restart);
#endif
            break;
         case MENU_ENUM_LABEL_CHEAT_SEARCH_EXACT:
#ifdef HAVE_CHEATS
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_cheat_search_exact);
#endif
            break;
         case MENU_ENUM_LABEL_CHEAT_SEARCH_LT:
#ifdef HAVE_CHEATS
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_cheat_search_lt);
#endif
            break;
         case MENU_ENUM_LABEL_CHEAT_SEARCH_GT:
#ifdef HAVE_CHEATS
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_cheat_search_gt);
#endif
            break;
         case MENU_ENUM_LABEL_CHEAT_SEARCH_EQ:
#ifdef HAVE_CHEATS
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_cheat_search_eq);
#endif
            break;
         case MENU_ENUM_LABEL_CHEAT_SEARCH_NEQ:
#ifdef HAVE_CHEATS
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_cheat_search_neq);
#endif
            break;
         case MENU_ENUM_LABEL_CHEAT_SEARCH_EQPLUS:
#ifdef HAVE_CHEATS
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_cheat_search_eqplus);
#endif
            break;
         case MENU_ENUM_LABEL_CHEAT_SEARCH_EQMINUS:
#ifdef HAVE_CHEATS
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_cheat_search_eqminus);
#endif
            break;
         case MENU_ENUM_LABEL_CHEAT_REPEAT_COUNT:
#ifdef HAVE_CHEATS
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_cheat_repeat_count);
#endif
            break;
         case MENU_ENUM_LABEL_CHEAT_REPEAT_ADD_TO_ADDRESS:
#ifdef HAVE_CHEATS
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_cheat_repeat_add_to_address);
#endif
            break;
         case MENU_ENUM_LABEL_CHEAT_REPEAT_ADD_TO_VALUE:
#ifdef HAVE_CHEATS
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_cheat_repeat_add_to_value);
#endif
            break;
         case MENU_ENUM_LABEL_CHEAT_ADD_MATCHES:
#ifdef HAVE_CHEATS
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_cheat_add_matches);
#endif
            break;
         case MENU_ENUM_LABEL_CHEAT_VIEW_MATCHES:
#ifdef HAVE_CHEATS
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_cheat_view_matches);
#endif
            break;
         case MENU_ENUM_LABEL_CHEAT_CREATE_OPTION:
#ifdef HAVE_CHEATS
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_cheat_create_option);
#endif
            break;
         case MENU_ENUM_LABEL_CHEAT_DELETE_OPTION:
#ifdef HAVE_CHEATS
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_cheat_delete_option);
#endif
            break;
         case MENU_ENUM_LABEL_CHEAT_ADD_NEW_TOP:
#ifdef HAVE_CHEATS
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_cheat_add_new_top);
#endif
            break;
         case MENU_ENUM_LABEL_CHEAT_RELOAD_CHEATS:
#ifdef HAVE_CHEATS
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_cheat_reload_cheats);
#endif
            break;
         case MENU_ENUM_LABEL_CHEAT_ADD_NEW_BOTTOM:
#ifdef HAVE_CHEATS
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_cheat_add_new_bottom);
#endif
            break;
         case MENU_ENUM_LABEL_CHEAT_ADDRESS_BIT_POSITION:
#ifdef HAVE_CHEATS
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_cheat_address_bit_position);
#endif
            break;
         case MENU_ENUM_LABEL_CHEAT_DELETE_ALL:
#ifdef HAVE_CHEATS
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_cheat_delete_all);
#endif
            break;
         case MENU_ENUM_LABEL_SLOWMOTION_RATIO:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_slowmotion_ratio);
            break;
         case MENU_ENUM_LABEL_RUN_AHEAD_ENABLED:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_run_ahead_enabled);
            break;
         case MENU_ENUM_LABEL_RUN_AHEAD_SECONDARY_INSTANCE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_run_ahead_secondary_instance);
            break;
         case MENU_ENUM_LABEL_RUN_AHEAD_HIDE_WARNINGS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_run_ahead_hide_warnings);
            break;
         case MENU_ENUM_LABEL_RUN_AHEAD_FRAMES:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_run_ahead_frames);
            break;
         case MENU_ENUM_LABEL_INPUT_BLOCK_TIMEOUT:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_block_timeout);
            break;
         case MENU_ENUM_LABEL_FASTFORWARD_RATIO:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_fastforward_ratio);
            break;
         case MENU_ENUM_LABEL_VRR_RUNLOOP_ENABLE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_vrr_runloop_enable);
            break;
         case MENU_ENUM_LABEL_BLOCK_SRAM_OVERWRITE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_block_sram_overwrite);
            break;
         case MENU_ENUM_LABEL_SAVESTATE_AUTO_INDEX:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_savestate_auto_index);
            break;
         case MENU_ENUM_LABEL_VIDEO_GPU_RECORD:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_video_gpu_record);
            break;
         case MENU_ENUM_LABEL_VIDEO_FULLSCREEN:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_video_fullscreen);
            break;
         case MENU_ENUM_LABEL_VIDEO_WINDOWED_FULLSCREEN:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_video_windowed_fullscreen);
            break;
         case MENU_ENUM_LABEL_VIDEO_FORCE_SRGB_DISABLE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_video_force_srgb_enable);
            break;
         case MENU_ENUM_LABEL_VIDEO_ROTATION:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_video_rotation);
            break;
         case MENU_ENUM_LABEL_SCREEN_ORIENTATION:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_screen_orientation);
            break;
         case MENU_ENUM_LABEL_VIDEO_GPU_SCREENSHOT:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_video_gpu_screenshot);
            break;
         case MENU_ENUM_LABEL_VIDEO_SCALE_INTEGER:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_video_scale_integer);
            break;
         case MENU_ENUM_LABEL_PLAYLISTS_TAB:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_content_collection_list);
            break;
         case MENU_ENUM_LABEL_INPUT_OVERLAY_HIDE_IN_MENU:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_overlay_hide_in_menu);
            break;
         case MENU_ENUM_LABEL_INPUT_OVERLAY_SHOW_PHYSICAL_INPUTS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_overlay_show_physical_inputs);
            break;
         case MENU_ENUM_LABEL_INPUT_OVERLAY_SHOW_PHYSICAL_INPUTS_PORT:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_overlay_show_physical_inputs_port);
            break;
         case MENU_ENUM_LABEL_INPUT_OVERLAY_SHOW_MOUSE_CURSOR:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_overlay_show_mouse_cursor);
            break;
         case MENU_ENUM_LABEL_INPUT_OVERLAY_AUTO_ROTATE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_overlay_auto_rotate);
            break;
         case MENU_ENUM_LABEL_VIDEO_FONT_SIZE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_video_font_size);
            break;
         case MENU_ENUM_LABEL_VIDEO_MESSAGE_POS_X:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_video_message_pos_x);
            break;
         case MENU_ENUM_LABEL_VIDEO_MESSAGE_POS_Y:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_video_message_pos_y);
            break;
         case MENU_ENUM_LABEL_VIDEO_WINDOW_WIDTH:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_video_window_width);
            break;
         case MENU_ENUM_LABEL_VIDEO_WINDOW_HEIGHT:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_video_window_height);
            break;
         case MENU_ENUM_LABEL_VIDEO_FULLSCREEN_X:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_video_fullscreen_x);
            break;
         case MENU_ENUM_LABEL_VIDEO_FULLSCREEN_Y:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_video_fullscreen_y);
            break;
         case MENU_ENUM_LABEL_VIDEO_WINDOW_SAVE_POSITION:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_video_save_window_position);
            break;
         case MENU_ENUM_LABEL_QUIT_RETROARCH:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_quit_retroarch);
            break;
         case MENU_ENUM_LABEL_MENU_WIDGETS_ENABLE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_widgets);
            break;
         case MENU_ENUM_LABEL_RESTART_RETROARCH:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_restart_retroarch);
            break;
         case MENU_ENUM_LABEL_NETWORK_INFORMATION:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_network_information);
            break;
         case MENU_ENUM_LABEL_SYSTEM_INFORMATION:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_system_information);
            break;
         case MENU_ENUM_LABEL_LOAD_CONTENT_LIST:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_content_list);
            break;
         case MENU_ENUM_LABEL_LOAD_CONTENT_SPECIAL:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_content_special);
            break;
         case MENU_ENUM_LABEL_LOAD_CONTENT_HISTORY:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_load_content_history);
            break;
         case MENU_ENUM_LABEL_START_CORE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_start_core);
            break;
         case MENU_ENUM_LABEL_CORE_LIST:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_core_list);
            break;
         case MENU_ENUM_LABEL_SIDELOAD_CORE_LIST:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_sideload_core_list);
            break;
         case MENU_ENUM_LABEL_CORE_UPDATER_LIST:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_download_core);
            break;
         case MENU_ENUM_LABEL_UPDATE_INSTALLED_CORES:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_update_installed_cores);
            break;
         case MENU_ENUM_LABEL_CORE_MANAGER_LIST:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_core_manager_list);
            break;
         case MENU_ENUM_LABEL_VIDEO_POST_FILTER_RECORD:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_video_post_filter_record);
            break;
         case MENU_ENUM_LABEL_NETPLAY_NICKNAME:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_netplay_nickname);
            break;
         case MENU_ENUM_LABEL_CHEEVOS_USERNAME:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_cheevos_username);
            break;
         case MENU_ENUM_LABEL_CHEEVOS_PASSWORD:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_cheevos_password);
            break;
         case MENU_ENUM_LABEL_VIDEO_FILTER:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_video_filter);
            break;
         case MENU_ENUM_LABEL_VIDEO_CROP_OVERSCAN:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_video_crop_overscan);
            break;
         case MENU_ENUM_LABEL_ADD_NULL_DRIVERS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_add_null_drivers);
            break;
         case MENU_ENUM_LABEL_VIDEO_SMOOTH:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_video_smooth);
            break;
         case MENU_ENUM_LABEL_VIDEO_CTX_SCALING:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_video_ctx_scaling);
            break;
         case MENU_ENUM_LABEL_VIDEO_FONT_ENABLE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_onscreen_notifications_enable);
            break;
         case MENU_ENUM_LABEL_INPUT_UNIFIED_MENU_CONTROLS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_input_unified_controls);
            break;
         case MENU_ENUM_LABEL_QUIT_PRESS_TWICE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_quit_press_twice);
            break;
         case MENU_ENUM_LABEL_AUDIO_MAX_TIMING_SKEW:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_audio_max_timing_skew);
            break;
         case MENU_ENUM_LABEL_AUDIO_ENABLE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_audio_enable);
            break;
         case MENU_ENUM_LABEL_AUDIO_ENABLE_MENU:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_audio_enable_menu);
            break;
         case MENU_ENUM_LABEL_VIDEO_REFRESH_RATE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_video_refresh_rate);
            break;
         case MENU_ENUM_LABEL_DUMMY_ON_CORE_SHUTDOWN:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_dummy_on_core_shutdown);
            break;
         case MENU_ENUM_LABEL_CHECK_FOR_MISSING_FIRMWARE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_dummy_check_missing_firmware);
            break;
#ifndef HAVE_DYNAMIC
         case MENU_ENUM_LABEL_ALWAYS_RELOAD_CORE_ON_RUN_CONTENT:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_always_reload_core_on_run_content);
            break;
#endif
         case MENU_ENUM_LABEL_VIDEO_ALLOW_ROTATE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_core_allow_rotate);
            break;
         case MENU_ENUM_LABEL_VIDEO_VSYNC:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_video_vertical_sync);
            break;
         case MENU_ENUM_LABEL_VIDEO_ADAPTIVE_VSYNC:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_video_adaptive_vsync);
            break;
         case MENU_ENUM_LABEL_INPUT_DUTY_CYCLE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_duty_cycle);
            break;
         case MENU_ENUM_LABEL_INPUT_TURBO_PERIOD:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_turbo_period);
            break;
         case MENU_ENUM_LABEL_INPUT_TURBO_MODE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_turbo_mode);
            break;
         case MENU_ENUM_LABEL_INPUT_TURBO_DEFAULT_BUTTON:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_turbo_default_button);
            break;
         case MENU_ENUM_LABEL_INPUT_BIND_TIMEOUT:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_bind_timeout);
            break;
         case MENU_ENUM_LABEL_INPUT_BIND_HOLD:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_bind_hold);
            break;
         case MENU_ENUM_LABEL_INPUT_BUTTON_AXIS_THRESHOLD:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_axis_threshold);
            break;
#if defined(GEKKO)
         case MENU_ENUM_LABEL_INPUT_MOUSE_SCALE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_mouse_scale);
            break;
#endif
         case MENU_ENUM_LABEL_AUDIO_SYNC:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_audio_sync);
            break;
         case MENU_ENUM_LABEL_AUDIO_VOLUME:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_audio_volume);
            break;
         case MENU_ENUM_LABEL_AUDIO_MIXER_VOLUME:
#ifdef HAVE_AUDIOMIXER
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_audio_mixer_volume);
#endif
            break;
         case MENU_ENUM_LABEL_INPUT_ALL_USERS_CONTROL_MENU:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_all_users_control_menu);
            break;
         case MENU_ENUM_LABEL_INPUT_POLL_TYPE_BEHAVIOR:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_poll_type_behavior);
            break;
         case MENU_ENUM_LABEL_INPUT_MAX_USERS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_max_users);
            break;
         case MENU_ENUM_LABEL_LOCATION_ALLOW:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_location_allow);
            break;
         case MENU_ENUM_LABEL_CAMERA_ALLOW:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_camera_allow);
            break;
         case MENU_ENUM_LABEL_AUDIO_RATE_CONTROL_DELTA:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_audio_rate_control_delta);
            break;
         case MENU_ENUM_LABEL_AUDIO_MUTE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_audio_mute);
            break;
         case MENU_ENUM_LABEL_AUDIO_MIXER_MUTE:
#ifdef HAVE_AUDIOMIXER
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_audio_mixer_mute);
#endif
            break;
         case MENU_ENUM_LABEL_AUDIO_FASTFORWARD_MUTE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_audio_fastforward_mute);
            break;
         case MENU_ENUM_LABEL_AUDIO_LATENCY:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_audio_latency);
            break;
         case MENU_ENUM_LABEL_DRIVER_SWITCH_ENABLE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_driver_switch_enable);
            break;
         case MENU_ENUM_LABEL_VIDEO_SHARED_CONTEXT:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_video_shared_context);
            break;
         case MENU_ENUM_LABEL_CONNECT_BLUETOOTH:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_bluetooth_list);
            break;
         case MENU_ENUM_LABEL_CHEEVOS_UNLOCKED_ENTRY:
         case MENU_ENUM_LABEL_CHEEVOS_UNLOCKED_ENTRY_HARDCORE:
         case MENU_ENUM_LABEL_CHEEVOS_LOCKED_ENTRY:
         case MENU_ENUM_LABEL_CHEEVOS_UNSUPPORTED_ENTRY:
         case MENU_ENUM_LABEL_CHEEVOS_UNOFFICIAL_ENTRY:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_cheevos_entry);
            break;
#ifdef HAVE_NETWORKING
         case MENU_ENUM_LABEL_CONNECT_NETPLAY_ROOM:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_netplay_room);
            break;
#endif
         case MENU_ENUM_LABEL_CHEEVOS_ENABLE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_cheevos_enable);
            break;
         case MENU_ENUM_LABEL_CHEEVOS_TEST_UNOFFICIAL:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_cheevos_test_unofficial);
            break;
         case MENU_ENUM_LABEL_CHEEVOS_HARDCORE_MODE_ENABLE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_cheevos_hardcore_mode_enable);
            break;
         case MENU_ENUM_LABEL_CHEEVOS_LEADERBOARDS_ENABLE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_cheevos_leaderboards_enable);
            break;
         case MENU_ENUM_LABEL_CHEEVOS_RICHPRESENCE_ENABLE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_cheevos_richpresence_enable);
            break;
         case MENU_ENUM_LABEL_CHEEVOS_BADGES_ENABLE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_cheevos_badges_enable);
            break;
         case MENU_ENUM_LABEL_CHEEVOS_VERBOSE_ENABLE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_cheevos_verbose_enable);
            break;
         case MENU_ENUM_LABEL_CHEEVOS_AUTO_SCREENSHOT:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_cheevos_auto_screenshot);
            break;
         case MENU_ENUM_LABEL_CHEEVOS_START_ACTIVE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_cheevos_start_active);
            break;
         case MENU_ENUM_LABEL_SETTINGS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_settings);
            break;
         case MENU_ENUM_LABEL_CONFIG_SAVE_ON_EXIT:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_config_save_on_exit);
            break;
         case MENU_ENUM_LABEL_CONFIGURATION_SETTINGS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_configuration_settings_list);
            break;
         case MENU_ENUM_LABEL_CONFIGURATIONS_LIST:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_configurations_list_list);
            break;
         case MENU_ENUM_LABEL_VIDEO_THREADED:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_video_threaded);
            break;
         case MENU_ENUM_LABEL_VIDEO_HARD_SYNC:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_video_hard_sync);
            break;
         case MENU_ENUM_LABEL_VIDEO_HARD_SYNC_FRAMES:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_video_hard_sync_frames);
            break;
         case MENU_ENUM_LABEL_VIDEO_REFRESH_RATE_AUTO:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_video_refresh_rate_auto);
            break;
         case MENU_ENUM_LABEL_VIDEO_REFRESH_RATE_POLLED:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_video_refresh_rate_polled);
            break;
         case MENU_ENUM_LABEL_VIDEO_MONITOR_INDEX:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_video_monitor_index);
            break;
         case MENU_ENUM_LABEL_LOG_VERBOSITY:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_log_verbosity);
            break;
         case MENU_ENUM_LABEL_LOG_TO_FILE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_log_to_file);
            break;
         case MENU_ENUM_LABEL_LOG_TO_FILE_TIMESTAMP:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_log_to_file_timestamp);
            break;
         case MENU_ENUM_LABEL_LOG_DIR:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_log_dir);
            break;
         case MENU_ENUM_LABEL_SHOW_HIDDEN_FILES:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_show_hidden_files);
            break;
         case MENU_ENUM_LABEL_INPUT_MENU_ENUM_TOGGLE_GAMEPAD_COMBO:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_toggle_gamepad_combo);
            break;
         case MENU_ENUM_LABEL_CPU_CORES:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_systeminfo_cpu_cores);
            break;
         case MENU_ENUM_LABEL_SYSTEM_INFO_CONTROLLER_ENTRY:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_systeminfo_controller_entry);
            break;
         case MENU_ENUM_LABEL_VIDEO_BLACK_FRAME_INSERTION:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_video_black_frame_insertion);
            break;
         case MENU_ENUM_LABEL_VIDEO_FRAME_DELAY:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_video_frame_delay);
            break;
         case MENU_ENUM_LABEL_VIDEO_SHADER_DELAY:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_video_shader_delay);
            break;
         case MENU_ENUM_LABEL_ADD_CONTENT_LIST:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_add_content_list);
            break;
         case MENU_ENUM_LABEL_INPUT_HOTKEY_BINDS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_hotkey_settings);
            break;
         case MENU_ENUM_LABEL_INPUT_HOTKEY_BLOCK_DELAY:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_hotkey_block_delay);
            break;
         case MENU_ENUM_LABEL_INPUT_USER_1_BINDS:
         case MENU_ENUM_LABEL_INPUT_USER_2_BINDS:
         case MENU_ENUM_LABEL_INPUT_USER_3_BINDS:
         case MENU_ENUM_LABEL_INPUT_USER_4_BINDS:
         case MENU_ENUM_LABEL_INPUT_USER_5_BINDS:
         case MENU_ENUM_LABEL_INPUT_USER_6_BINDS:
         case MENU_ENUM_LABEL_INPUT_USER_7_BINDS:
         case MENU_ENUM_LABEL_INPUT_USER_8_BINDS:
         case MENU_ENUM_LABEL_INPUT_USER_9_BINDS:
         case MENU_ENUM_LABEL_INPUT_USER_10_BINDS:
         case MENU_ENUM_LABEL_INPUT_USER_11_BINDS:
         case MENU_ENUM_LABEL_INPUT_USER_12_BINDS:
         case MENU_ENUM_LABEL_INPUT_USER_13_BINDS:
         case MENU_ENUM_LABEL_INPUT_USER_14_BINDS:
         case MENU_ENUM_LABEL_INPUT_USER_15_BINDS:
         case MENU_ENUM_LABEL_INPUT_USER_16_BINDS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_user_bind_settings);
            break;
         case MENU_ENUM_LABEL_INFORMATION_LIST:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_information_list_list);
            break;
         case MENU_ENUM_LABEL_NETPLAY:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_netplay_settings);
            break;
         case MENU_ENUM_LABEL_ONLINE_UPDATER:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_online_updater);
            break;
         case MENU_ENUM_LABEL_VIDEO_MAX_SWAPCHAIN_IMAGES:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_max_swapchain_images);
            break;
         case MENU_ENUM_LABEL_STATISTICS_SHOW:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_statistics_show);
            break;
         case MENU_ENUM_LABEL_FPS_SHOW:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_fps_show);
            break;
         case MENU_ENUM_LABEL_FPS_UPDATE_INTERVAL:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_fps_update_interval);
            break;
         case MENU_ENUM_LABEL_FRAMECOUNT_SHOW:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_framecount_show);
            break;
         case MENU_ENUM_LABEL_MEMORY_SHOW:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_memory_show);
            break;
         case MENU_ENUM_LABEL_MENU_VIEWS_SETTINGS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_views_settings_list);
            break;
         case MENU_ENUM_LABEL_SETTINGS_VIEWS_SETTINGS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_settings_views_settings_list);
            break;
         case MENU_ENUM_LABEL_QUICK_MENU_VIEWS_SETTINGS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_quick_menu_views_settings_list);
            break;
         case MENU_ENUM_LABEL_MENU_SETTINGS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_settings_list);
            break;
         case MENU_ENUM_LABEL_VIDEO_SETTINGS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_video_settings_list);
            break;
         case MENU_ENUM_LABEL_VIDEO_SYNCHRONIZATION_SETTINGS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_video_synchronization_settings_list);
            break;
         case MENU_ENUM_LABEL_VIDEO_FULLSCREEN_MODE_SETTINGS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_video_fullscreen_mode_settings_list);
            break;
         case MENU_ENUM_LABEL_VIDEO_WINDOWED_MODE_SETTINGS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_video_windowed_mode_settings_list);
            break;
         case MENU_ENUM_LABEL_VIDEO_SCALING_SETTINGS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_video_scaling_settings_list);
            break;
         case MENU_ENUM_LABEL_VIDEO_OUTPUT_SETTINGS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_video_output_settings_list);
            break;
         case MENU_ENUM_LABEL_CRT_SWITCHRES_SETTINGS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_crt_switchres_settings_list);
            break;
         case MENU_ENUM_LABEL_AUDIO_SETTINGS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_audio_settings_list);
            break;
         case MENU_ENUM_LABEL_AUDIO_RESAMPLER_SETTINGS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_audio_resampler_settings_list);
            break;
         case MENU_ENUM_LABEL_AUDIO_SYNCHRONIZATION_SETTINGS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_audio_synchronization_settings_list);
            break;
         case MENU_ENUM_LABEL_AUDIO_OUTPUT_SETTINGS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_audio_output_settings_list);
            break;
         case MENU_ENUM_LABEL_AUDIO_MIXER_SETTINGS:
#ifdef HAVE_AUDIOMIXER
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_mixer_settings_list);
#endif
            break;
         case MENU_ENUM_LABEL_LATENCY_SETTINGS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_latency_settings_list);
            break;
         case MENU_ENUM_LABEL_RECORDING_SETTINGS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_recording_settings_list);
            break;
         case MENU_ENUM_LABEL_CORE_SETTINGS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_core_settings_list);
            break;
         case MENU_ENUM_LABEL_DRIVER_SETTINGS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_driver_settings_list);
            break;
         case MENU_ENUM_LABEL_SAVING_SETTINGS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_saving_settings_list);
            break;
         case MENU_ENUM_LABEL_LOGGING_SETTINGS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_logging_settings_list);
            break;
         case MENU_ENUM_LABEL_PLAYLIST_SETTINGS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_playlist_settings_list);
            break;
         case MENU_ENUM_LABEL_PLAYLIST_MANAGER_LIST:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_playlist_manager_list);
            break;
         case MENU_ENUM_LABEL_PLAYLIST_MANAGER_DEFAULT_CORE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_playlist_manager_default_core);
            break;
         case MENU_ENUM_LABEL_PLAYLIST_MANAGER_RESET_CORES:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_playlist_manager_reset_cores);
            break;
         case MENU_ENUM_LABEL_PLAYLIST_MANAGER_LABEL_DISPLAY_MODE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_playlist_manager_label_display_mode);
            break;
         case MENU_ENUM_LABEL_PLAYLIST_MANAGER_SORT_MODE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_playlist_manager_sort_mode);
            break;
         case MENU_ENUM_LABEL_PLAYLIST_MANAGER_CLEAN_PLAYLIST:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_playlist_manager_clean_playlist);
            break;
         case MENU_ENUM_LABEL_PLAYLIST_MANAGER_RIGHT_THUMBNAIL_MODE:
            {
               const char *menu_ident             = menu_driver_ident();
               /* Uses same sublabels as MENU_ENUM_LABEL_THUMBNAILS */
#ifdef HAVE_RGUI
               if (string_is_equal(menu_ident, "rgui"))
               {
                  BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_thumbnails_rgui);
               }
               else
#endif
#ifdef HAVE_MATERIALUI
                  if (string_is_equal(menu_ident, "glui"))
                  {
                     BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_thumbnails_materialui);
                  }
                  else
#endif
                  {
                     BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_thumbnails);
                  }
            }
            break;
         case MENU_ENUM_LABEL_PLAYLIST_MANAGER_LEFT_THUMBNAIL_MODE:
            {
               const char *menu_ident             = menu_driver_ident();
               /* Uses same sublabels as MENU_ENUM_LABEL_LEFT_THUMBNAILS */
#ifdef HAVE_RGUI
               if (string_is_equal(menu_ident, "rgui"))
               {
                  BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_left_thumbnails_rgui);
               }
               else
#endif
#ifdef HAVE_OZONE
                  if (string_is_equal(menu_ident, "ozone"))
                  {
                     BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_left_thumbnails_ozone);
                  }
                  else
#endif
#ifdef HAVE_MATERIALUI
                     if (string_is_equal(menu_ident, "glui"))
                     {
                        BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_left_thumbnails_materialui);
                     }
                     else
#endif
                     {
                        BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_left_thumbnails);
                     }
            }
            break;
         case MENU_ENUM_LABEL_DELETE_PLAYLIST:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_delete_playlist);
            break;
         case MENU_ENUM_LABEL_AI_SERVICE_URL:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_ai_service_url);
            break;
         case MENU_ENUM_LABEL_AI_SERVICE_TARGET_LANG:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_ai_service_target_lang);
            break;
         case MENU_ENUM_LABEL_AI_SERVICE_SOURCE_LANG:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_ai_service_source_lang);
            break;
         case MENU_ENUM_LABEL_AI_SERVICE_MODE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_ai_service_mode);
            break;
         case MENU_ENUM_LABEL_AI_SERVICE_PAUSE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_ai_service_pause);
            break;
         case MENU_ENUM_LABEL_AI_SERVICE_ENABLE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_ai_service_enable);
            break;
         case MENU_ENUM_LABEL_AI_SERVICE_SETTINGS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_ai_service_settings_list);
            break;
         case MENU_ENUM_LABEL_ACCESSIBILITY_ENABLED:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_accessibility_enabled);
            break;
         case MENU_ENUM_LABEL_ACCESSIBILITY_NARRATOR_SPEECH_SPEED:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_accessibility_narrator_speech_speed);
            break;
         case MENU_ENUM_LABEL_ACCESSIBILITY_SETTINGS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_accessibility_settings_list);
            break;
         case MENU_ENUM_LABEL_USER_INTERFACE_SETTINGS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_user_interface_settings_list);
            break;
         case MENU_ENUM_LABEL_POWER_MANAGEMENT_SETTINGS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_power_management_settings_list);
            break;
         case MENU_ENUM_LABEL_PRIVACY_SETTINGS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_privacy_settings_list);
            break;
         case MENU_ENUM_LABEL_MIDI_SETTINGS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_midi_settings_list);
            break;
         case MENU_ENUM_LABEL_DIRECTORY_SETTINGS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_directory_settings_list);
            break;
         case MENU_ENUM_LABEL_FRAME_THROTTLE_SETTINGS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_frame_throttle_settings_list);
            break;
         case MENU_ENUM_LABEL_FRAME_TIME_COUNTER_SETTINGS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_frame_time_counter_settings_list);
            break;
         case MENU_ENUM_LABEL_ONSCREEN_DISPLAY_SETTINGS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_onscreen_display_settings_list);
            break;
         case MENU_ENUM_LABEL_NETWORK_SETTINGS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_network_settings_list);
            break;
         case MENU_ENUM_LABEL_NETWORK_ON_DEMAND_THUMBNAILS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_network_on_demand_thumbnails);
            break;
         case MENU_ENUM_LABEL_USER_SETTINGS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_user_settings_list);
            break;
         case MENU_ENUM_LABEL_RETRO_ACHIEVEMENTS_SETTINGS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_retro_achievements_settings_list);
            break;
         case MENU_ENUM_LABEL_INPUT_SETTINGS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_settings_list);
            break;
         case MENU_ENUM_LABEL_INPUT_MENU_SETTINGS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_menu_settings_list);
            break;
         case MENU_ENUM_LABEL_INPUT_HAPTIC_FEEDBACK_SETTINGS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_input_haptic_feedback_settings_list);
            break;
         case MENU_ENUM_LABEL_BLUETOOTH_SETTINGS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_bluetooth_settings_list);
            break;
         case MENU_ENUM_LABEL_WIFI_SETTINGS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_wifi_settings_list);
            break;
         case MENU_ENUM_LABEL_NETPLAY_LAN_SCAN_SETTINGS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_netplay_lan_scan_settings_list);
            break;
         case MENU_ENUM_LABEL_HELP_LIST:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_help_list);
            break;
#ifdef HAVE_LAKKA
         case MENU_ENUM_LABEL_LAKKA_SERVICES:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_services_settings_list);
            break;
         case MENU_ENUM_LABEL_SSH_ENABLE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_ssh_enable);
            break;
         case MENU_ENUM_LABEL_SAMBA_ENABLE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_samba_enable);
            break;
         case MENU_ENUM_LABEL_BLUETOOTH_ENABLE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_bluetooth_enable);
            break;
         case MENU_ENUM_LABEL_LOCALAP_ENABLE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_localap_enable);
            break;
#endif
         case MENU_ENUM_LABEL_USER_LANGUAGE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_user_language);
            break;
         case MENU_ENUM_LABEL_SUSPEND_SCREENSAVER_ENABLE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_suspend_screensaver_enable);
            break;
         case MENU_ENUM_LABEL_VIDEO_SCALE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_video_window_scale);
            break;
         case MENU_ENUM_LABEL_PAUSE_NONACTIVE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_pause_nonactive);
            break;
         case MENU_ENUM_LABEL_VIDEO_DISABLE_COMPOSITION:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_video_disable_composition);
            break;
         case MENU_ENUM_LABEL_HISTORY_LIST_ENABLE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_history_list_enable);
            break;
         case MENU_ENUM_LABEL_CONTENT_HISTORY_SIZE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_content_history_size);
            break;
         case MENU_ENUM_LABEL_CONTENT_FAVORITES_SIZE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_content_favorites_size);
            break;
         case MENU_ENUM_LABEL_NETPLAY_USE_MITM_SERVER:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_netplay_use_mitm_server);
            break;
         case MENU_ENUM_LABEL_NETPLAY_MITM_SERVER:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_netplay_mitm_server);
            break;
         case MENU_ENUM_LABEL_CORE_LOCK:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_core_lock);
            break;
         case MENU_ENUM_LABEL_CORE_DELETE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_core_delete);
            break;
         case MENU_ENUM_LABEL_ACHIEVEMENT_PAUSE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_pause_hardcode_mode);
            break;
         case MENU_ENUM_LABEL_ACHIEVEMENT_RESUME:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_resume_hardcode_mode);
            break;
         case MENU_ENUM_LABEL_MIDI_INPUT:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_midi_input);
            break;
         case MENU_ENUM_LABEL_MIDI_OUTPUT:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_midi_output);
            break;
         case MENU_ENUM_LABEL_MIDI_VOLUME:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_midi_volume);
            break;
         case MENU_ENUM_LABEL_ONSCREEN_OVERLAY_SETTINGS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_onscreen_overlay_settings_list);
            break;
#ifdef HAVE_VIDEO_LAYOUT
         case MENU_ENUM_LABEL_ONSCREEN_VIDEO_LAYOUT_SETTINGS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_onscreen_video_layout_settings_list);
            break;
#endif
         case MENU_ENUM_LABEL_ONSCREEN_NOTIFICATIONS_SETTINGS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_onscreen_notifications_settings_list);
            break;
#ifdef HAVE_QT
         case MENU_ENUM_LABEL_SHOW_WIMP:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_show_wimp);
            break;
#endif
#if defined(HAVE_LAKKA_SWITCH) || defined(HAVE_LIBNX)
         case MENU_ENUM_LABEL_SWITCH_CPU_PROFILE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_switch_cpu_profile);
            break;
#endif
#ifdef HAVE_LAKKA_SWITCH
         case MENU_ENUM_LABEL_SWITCH_GPU_PROFILE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_switch_gpu_profile);
            break;
         case MENU_ENUM_LABEL_SWITCH_BACKLIGHT_CONTROL:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_switch_backlight_control);
            break;
#endif
#if defined(_3DS)
         case MENU_ENUM_LABEL_VIDEO_3DS_LCD_BOTTOM:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_video_3ds_lcd_bottom);
            break;
         case MENU_ENUM_LABEL_VIDEO_3DS_DISPLAY_MODE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_video_3ds_display_mode);
            break;
#endif
#if defined(GEKKO)
         case MENU_ENUM_LABEL_VIDEO_OVERSCAN_CORRECTION_TOP:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_video_overscan_correction_top);
            break;
         case MENU_ENUM_LABEL_VIDEO_OVERSCAN_CORRECTION_BOTTOM:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_video_overscan_correction_bottom);
            break;
#endif
         case MENU_ENUM_LABEL_CHEAT_APPLY_AFTER_LOAD:
#ifdef HAVE_CHEATS
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_cheat_apply_after_load);
#endif
            break;
         case MENU_ENUM_LABEL_DISCORD_ALLOW:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_discord_allow);
            break;
         case MENU_ENUM_LABEL_PLAYLIST_ENTRY:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_playlist_entry);
            break;
         case MENU_ENUM_LABEL_PLAYLIST_SHOW_SUBLABELS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_playlist_show_sublabels);
            break;
         case MENU_ENUM_LABEL_MENU_RGUI_BORDER_FILLER_ENABLE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_rgui_border_filler_enable);
            break;
         case MENU_ENUM_LABEL_MENU_RGUI_BORDER_FILLER_THICKNESS_ENABLE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_rgui_border_filler_thickness_enable);
            break;
         case MENU_ENUM_LABEL_MENU_RGUI_BACKGROUND_FILLER_THICKNESS_ENABLE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_rgui_background_filler_thickness_enable);
            break;
         case MENU_ENUM_LABEL_MENU_LINEAR_FILTER:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_linear_filter);
            break;
         case MENU_ENUM_LABEL_MENU_RGUI_ASPECT_RATIO_LOCK:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_rgui_aspect_ratio_lock);
            break;
         case MENU_ENUM_LABEL_RGUI_MENU_COLOR_THEME:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_rgui_menu_color_theme);
            break;
         case MENU_ENUM_LABEL_RGUI_MENU_THEME_PRESET:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_rgui_menu_theme_preset);
            break;
         case MENU_ENUM_LABEL_MENU_RGUI_SHADOWS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_rgui_shadows);
            break;
         case MENU_ENUM_LABEL_MENU_RGUI_PARTICLE_EFFECT:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_rgui_particle_effect);
            break;
         case MENU_ENUM_LABEL_MENU_RGUI_PARTICLE_EFFECT_SPEED:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_rgui_particle_effect_speed);
            break;
         case MENU_ENUM_LABEL_MENU_RGUI_INLINE_THUMBNAILS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_rgui_inline_thumbnails);
            break;
         case MENU_ENUM_LABEL_MENU_RGUI_SWAP_THUMBNAILS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_rgui_swap_thumbnails);
            break;
         case MENU_ENUM_LABEL_MENU_RGUI_THUMBNAIL_DOWNSCALER:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_rgui_thumbnail_downscaler);
            break;
         case MENU_ENUM_LABEL_MENU_RGUI_THUMBNAIL_DELAY:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_rgui_thumbnail_delay);
            break;
         case MENU_ENUM_LABEL_CONTENT_RUNTIME_LOG:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_content_runtime_log);
            break;
         case MENU_ENUM_LABEL_SCAN_WITHOUT_CORE_MATCH:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_scan_without_core_match);
            break;
         case MENU_ENUM_LABEL_CONTENT_RUNTIME_LOG_AGGREGATE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_content_runtime_log_aggregate);
            break;
         case MENU_ENUM_LABEL_PLAYLIST_SUBLABEL_RUNTIME_TYPE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_playlist_sublabel_runtime_type);
            break;
         case MENU_ENUM_LABEL_PLAYLIST_SUBLABEL_LAST_PLAYED_STYLE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_playlist_sublabel_last_played_style);
            break;
         case MENU_ENUM_LABEL_MENU_RGUI_INTERNAL_UPSCALE_LEVEL:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_rgui_internal_upscale_level);
            break;
         case MENU_ENUM_LABEL_MENU_RGUI_ASPECT_RATIO:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_rgui_aspect_ratio);
            break;
         case MENU_ENUM_LABEL_MENU_TICKER_TYPE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_ticker_type);
            break;
         case MENU_ENUM_LABEL_MENU_TICKER_SPEED:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_ticker_speed);
            break;
         case MENU_ENUM_LABEL_MENU_TICKER_SMOOTH:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_ticker_smooth);
            break;
         case MENU_ENUM_LABEL_PLAYLIST_SHOW_INLINE_CORE_NAME:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_playlist_show_inline_core_name);
            break;
         case MENU_ENUM_LABEL_PLAYLIST_SORT_ALPHABETICAL:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_playlist_sort_alphabetical);
            break;
         case MENU_ENUM_LABEL_PLAYLIST_FUZZY_ARCHIVE_MATCH:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_playlist_fuzzy_archive_match);
            break;
         case MENU_ENUM_LABEL_PLAYLIST_USE_OLD_FORMAT:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_playlist_use_old_format);
            break;
         case MENU_ENUM_LABEL_PLAYLIST_COMPRESSION:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_playlist_compression);
            break;
         case MENU_ENUM_LABEL_MENU_RGUI_FULL_WIDTH_LAYOUT:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_rgui_full_width_layout);
            break;
         case MENU_ENUM_LABEL_MENU_RGUI_EXTENDED_ASCII:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_menu_rgui_extended_ascii);
            break;
         case MENU_ENUM_LABEL_THUMBNAILS_UPDATER_LIST:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_thumbnails_updater_list);
            break;
         case MENU_ENUM_LABEL_PL_THUMBNAILS_UPDATER_LIST:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_pl_thumbnails_updater_list);
            break;
         case MENU_ENUM_LABEL_HELP_SEND_DEBUG_INFO:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_help_send_debug_info);
            break;
         case MENU_ENUM_LABEL_RDB_ENTRY_DETAIL:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_rdb_entry_detail);
            break;
         case MENU_ENUM_LABEL_MANUAL_CONTENT_SCAN_LIST:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_manual_content_scan_list);
            break;
         case MENU_ENUM_LABEL_MANUAL_CONTENT_SCAN_DIR:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_manual_content_scan_dir);
            break;
         case MENU_ENUM_LABEL_MANUAL_CONTENT_SCAN_SYSTEM_NAME:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_manual_content_scan_system_name);
            break;
         case MENU_ENUM_LABEL_MANUAL_CONTENT_SCAN_SYSTEM_NAME_CUSTOM:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_manual_content_scan_system_name_custom);
            break;
         case MENU_ENUM_LABEL_MANUAL_CONTENT_SCAN_CORE_NAME:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_manual_content_scan_core_name);
            break;
         case MENU_ENUM_LABEL_MANUAL_CONTENT_SCAN_FILE_EXTS:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_manual_content_scan_file_exts);
            break;
         case MENU_ENUM_LABEL_MANUAL_CONTENT_SCAN_SEARCH_RECURSIVELY:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_manual_content_scan_search_recursively);
            break;
         case MENU_ENUM_LABEL_MANUAL_CONTENT_SCAN_SEARCH_ARCHIVES:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_manual_content_scan_search_archives);
            break;
         case MENU_ENUM_LABEL_MANUAL_CONTENT_SCAN_DAT_FILE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_manual_content_scan_dat_file);
            break;
         case MENU_ENUM_LABEL_MANUAL_CONTENT_SCAN_DAT_FILE_FILTER:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_manual_content_scan_dat_file_filter);
            break;
         case MENU_ENUM_LABEL_MANUAL_CONTENT_SCAN_OVERWRITE:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_manual_content_scan_overwrite);
            break;
         case MENU_ENUM_LABEL_MANUAL_CONTENT_SCAN_START:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_manual_content_scan_start);
            break;
         case MENU_ENUM_LABEL_CORE_CREATE_BACKUP:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_core_create_backup);
            break;
         case MENU_ENUM_LABEL_CORE_RESTORE_BACKUP_LIST:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_core_restore_backup_list);
            break;
         case MENU_ENUM_LABEL_CORE_DELETE_BACKUP_LIST:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_core_delete_backup_list);
            break;
         case MENU_ENUM_LABEL_CORE_RESTORE_BACKUP_ENTRY:
         case MENU_ENUM_LABEL_CORE_DELETE_BACKUP_ENTRY:
            BIND_ACTION_SUBLABEL(cbs, action_bind_sublabel_core_backup_entry);
            break;
         default:
         case MSG_UNKNOWN:
            return -1;
      }
   }

   return 0;
}
