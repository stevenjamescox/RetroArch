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

#ifndef _MENU_DISPLAYLIST_H
#define _MENU_DISPLAYLIST_H

#include <stdint.h>

#include <boolean.h>
#include <retro_miscellaneous.h>
#include <retro_common_api.h>
#include <lists/file_list.h>

#include "../configuration.h"
#include "../msg_hash.h"
#include "../setting_list.h"

#define MENU_DISPLAYLIST_PARSE_SETTINGS_ENUM(list, label, parse_type, add_empty_entry) menu_displaylist_parse_settings_enum(list, parse_type, add_empty_entry, menu_setting_find_enum(label), label, true, settings->bools.menu_show_advanced_settings)

#define MENU_DISPLAYLIST_PARSE_SETTINGS(list, label, parse_type, add_empty_entry, entry_type) menu_displaylist_parse_settings_enum(list, parse_type, add_empty_entry, menu_setting_find(label), entry_type, false, settings->bools.menu_show_advanced_settings)

RETRO_BEGIN_DECLS

/* NOTE: Order matters - only add new entries at
 * the bottom */
enum menu_displaylist_parse_type
{
   PARSE_NONE = 0,
   PARSE_GROUP,
   PARSE_ACTION,
   PARSE_ONLY_INT,
   PARSE_ONLY_UINT,
   PARSE_ONLY_BOOL,
   PARSE_ONLY_FLOAT,
   PARSE_ONLY_BIND,
   PARSE_ONLY_GROUP,
   PARSE_ONLY_STRING,
   PARSE_ONLY_PATH,
   PARSE_ONLY_STRING_OPTIONS,
   PARSE_ONLY_DIR,
   PARSE_SUB_GROUP,
   PARSE_ONLY_SIZE
};

enum menu_displaylist_ctl_state
{
   DISPLAYLIST_NONE = 0,
   DISPLAYLIST_DROPDOWN_LIST,
   DISPLAYLIST_DROPDOWN_LIST_SPECIAL,
   DISPLAYLIST_DROPDOWN_LIST_RESOLUTION,
   DISPLAYLIST_DROPDOWN_LIST_AUDIO_DEVICE,
#ifdef HAVE_MICROPHONE
   DISPLAYLIST_DROPDOWN_LIST_MICROPHONE_DEVICE,
#endif
   DISPLAYLIST_DROPDOWN_LIST_VIDEO_SHADER_PARAMETER,
   DISPLAYLIST_DROPDOWN_LIST_VIDEO_SHADER_PRESET_PARAMETER,
   DISPLAYLIST_DROPDOWN_LIST_VIDEO_SHADER_NUM_PASSES,
   DISPLAYLIST_DROPDOWN_LIST_PLAYLIST_DEFAULT_CORE,
   DISPLAYLIST_DROPDOWN_LIST_PLAYLIST_LABEL_DISPLAY_MODE,
   DISPLAYLIST_DROPDOWN_LIST_PLAYLIST_RIGHT_THUMBNAIL_MODE,
   DISPLAYLIST_DROPDOWN_LIST_PLAYLIST_LEFT_THUMBNAIL_MODE,
   DISPLAYLIST_DROPDOWN_LIST_PLAYLIST_SORT_MODE,
   DISPLAYLIST_DROPDOWN_LIST_MANUAL_CONTENT_SCAN_SYSTEM_NAME,
   DISPLAYLIST_DROPDOWN_LIST_MANUAL_CONTENT_SCAN_CORE_NAME,
   DISPLAYLIST_DROPDOWN_LIST_DISK_INDEX,
   DISPLAYLIST_DROPDOWN_LIST_INPUT_DEVICE_TYPE,
   DISPLAYLIST_DROPDOWN_LIST_INPUT_DESCRIPTION,
   DISPLAYLIST_DROPDOWN_LIST_INPUT_DESCRIPTION_KBD,
   DISPLAYLIST_DROPDOWN_LIST_INPUT_SELECT_RESERVED_DEVICE,
#ifdef ANDROID
    DISPLAYLIST_DROPDOWN_LIST_INPUT_SELECT_PHYSICAL_KEYBOARD,
#endif
#ifdef HAVE_NETWORKING
   DISPLAYLIST_DROPDOWN_LIST_NETPLAY_MITM_SERVER,
#endif
   DISPLAYLIST_CDROM_DETAIL_INFO,
   DISPLAYLIST_INFO,
   DISPLAYLIST_HELP,
   DISPLAYLIST_HELP_SCREEN_LIST,
   DISPLAYLIST_MAIN_MENU,
   DISPLAYLIST_GENERIC,
   DISPLAYLIST_SETTINGS_ALL,
   DISPLAYLIST_HORIZONTAL,
   DISPLAYLIST_HORIZONTAL_CONTENT_ACTIONS,
   DISPLAYLIST_HISTORY,
   DISPLAYLIST_EXPLORE,
   DISPLAYLIST_CONTENTLESS_CORES,
   DISPLAYLIST_FAVORITES,
   DISPLAYLIST_PLAYLIST,
   DISPLAYLIST_VIDEO_HISTORY,
   DISPLAYLIST_MUSIC_HISTORY,
   DISPLAYLIST_IMAGES_HISTORY,
   DISPLAYLIST_MUSIC_LIST,
   DISPLAYLIST_PLAYLIST_COLLECTION,
   DISPLAYLIST_DEFAULT,
   DISPLAYLIST_FILE_BROWSER_SELECT_DIR,
   DISPLAYLIST_FILE_BROWSER_SCAN_DIR,
   DISPLAYLIST_FILE_BROWSER_SELECT_FILE,
   DISPLAYLIST_FILE_BROWSER_SELECT_CORE,
   DISPLAYLIST_FILE_BROWSER_SELECT_SIDELOAD_CORE,
   DISPLAYLIST_FILE_BROWSER_SELECT_COLLECTION,
   DISPLAYLIST_CORES,
   DISPLAYLIST_CORES_SUPPORTED,
   DISPLAYLIST_CORES_COLLECTION_SUPPORTED,
   DISPLAYLIST_CORES_UPDATER,
   DISPLAYLIST_CORE_MANAGER_LIST,
#ifdef HAVE_MIST
   DISPLAYLIST_STEAM_SETTINGS_LIST,
   DISPLAYLIST_CORE_MANAGER_STEAM_LIST,
   DISPLAYLIST_CORE_INFORMATION_STEAM_LIST,
#endif
#if 0
/* Thumbnailpack removal */
   DISPLAYLIST_THUMBNAILS_UPDATER,
#endif
   DISPLAYLIST_PL_THUMBNAILS_UPDATER,
   DISPLAYLIST_LAKKA,
   DISPLAYLIST_CORES_DETECTED,
   DISPLAYLIST_SAVESTATE_LIST,
   DISPLAYLIST_CORE_OPTIONS,
   DISPLAYLIST_CORE_OPTION_OVERRIDE_LIST,
   DISPLAYLIST_CORE_INFO,
   DISPLAYLIST_DISC_INFO,
   DISPLAYLIST_PERFCOUNTERS_CORE,
   DISPLAYLIST_PERFCOUNTERS_FRONTEND,
   DISPLAYLIST_SHADER_PASS,
   DISPLAYLIST_SHADER_PRESET,
   DISPLAYLIST_DATABASES,
   DISPLAYLIST_DATABASE_PLAYLISTS,
   DISPLAYLIST_DATABASE_PLAYLISTS_HORIZONTAL,
   DISPLAYLIST_DATABASE_QUERY,
   DISPLAYLIST_DATABASE_ENTRY,
   DISPLAYLIST_AUDIO_FILTERS,
   DISPLAYLIST_VIDEO_FILTERS,
   DISPLAYLIST_CHEAT_FILES,
   DISPLAYLIST_REMAP_FILE_MANAGER,
   DISPLAYLIST_REMAP_FILES,
   DISPLAYLIST_RECORD_CONFIG_FILES,
   DISPLAYLIST_STREAM_CONFIG_FILES,
   DISPLAYLIST_RGUI_THEME_PRESETS,
   DISPLAYLIST_CONFIG_FILES,
   DISPLAYLIST_CONTENT_HISTORY,
   DISPLAYLIST_IMAGES,
   DISPLAYLIST_FONTS,
   DISPLAYLIST_VIDEO_FONTS,
   DISPLAYLIST_LOAD_DISC,
   DISPLAYLIST_DUMP_DISC,
#ifdef HAVE_LAKKA
   DISPLAYLIST_EJECT_DISC,
#endif
   DISPLAYLIST_OVERLAYS,
   DISPLAYLIST_OSK_OVERLAYS,
   DISPLAYLIST_SHADER_PARAMETERS,
   DISPLAYLIST_SHADER_PARAMETERS_PRESET,
   DISPLAYLIST_SHADER_PRESET_SAVE,
   DISPLAYLIST_SHADER_PRESET_REMOVE,
   DISPLAYLIST_NETWORK_INFO,
   DISPLAYLIST_SYSTEM_INFO,
   DISPLAYLIST_ACHIEVEMENT_PAUSE_MENU,
   DISPLAYLIST_ACHIEVEMENT_LIST,
   DISPLAYLIST_USER_BINDS_LIST,
   DISPLAYLIST_ACCOUNTS_LIST,
   DISPLAYLIST_SUBSYSTEM_SETTINGS_LIST,
   DISPLAYLIST_MIXER_STREAM_SETTINGS_LIST,
   DISPLAYLIST_DRIVER_SETTINGS_LIST,
   DISPLAYLIST_VIDEO_FULLSCREEN_MODE_SETTINGS_LIST,
   DISPLAYLIST_VIDEO_WINDOWED_MODE_SETTINGS_LIST,
   DISPLAYLIST_VIDEO_SYNCHRONIZATION_SETTINGS_LIST,
   DISPLAYLIST_VIDEO_OUTPUT_SETTINGS_LIST,
   DISPLAYLIST_VIDEO_SCALING_SETTINGS_LIST,
   DISPLAYLIST_VIDEO_HDR_SETTINGS_LIST,
   DISPLAYLIST_CRT_SWITCHRES_SETTINGS_LIST,
   DISPLAYLIST_VIDEO_SETTINGS_LIST,
   DISPLAYLIST_CONFIGURATION_SETTINGS_LIST,
   DISPLAYLIST_SAVING_SETTINGS_LIST,
   DISPLAYLIST_CLOUD_SYNC_SETTINGS_LIST,
   DISPLAYLIST_LOGGING_SETTINGS_LIST,
   DISPLAYLIST_FRAME_THROTTLE_SETTINGS_LIST,
   DISPLAYLIST_FRAME_TIME_COUNTER_SETTINGS_LIST,
   DISPLAYLIST_REWIND_SETTINGS_LIST,
   DISPLAYLIST_CHEAT_DETAILS_SETTINGS_LIST,
   DISPLAYLIST_CHEAT_SEARCH_SETTINGS_LIST,
   DISPLAYLIST_AUDIO_SETTINGS_LIST,
   DISPLAYLIST_AUDIO_OUTPUT_SETTINGS_LIST,
#ifdef HAVE_MICROPHONE
   DISPLAYLIST_MICROPHONE_SETTINGS_LIST,
#endif
   DISPLAYLIST_AUDIO_SYNCHRONIZATION_SETTINGS_LIST,
   DISPLAYLIST_AUDIO_MIXER_SETTINGS_LIST,
   DISPLAYLIST_CORE_SETTINGS_LIST,
   DISPLAYLIST_INPUT_SETTINGS_LIST,
   DISPLAYLIST_INPUT_TURBO_FIRE_SETTINGS_LIST,
   DISPLAYLIST_INPUT_HAPTIC_FEEDBACK_SETTINGS_LIST,
   DISPLAYLIST_INPUT_MENU_SETTINGS_LIST,
   DISPLAYLIST_LATENCY_SETTINGS_LIST,
   DISPLAYLIST_INPUT_RETROPAD_BINDS_LIST,
   DISPLAYLIST_INPUT_HOTKEY_BINDS_LIST,
#if defined(HAVE_OVERLAY)
   DISPLAYLIST_ONSCREEN_OVERLAY_SETTINGS_LIST,
   DISPLAYLIST_OSK_OVERLAY_SETTINGS_LIST,
   DISPLAYLIST_OVERLAY_LIGHTGUN_SETTINGS_LIST,
   DISPLAYLIST_OVERLAY_MOUSE_SETTINGS_LIST,
#endif
   DISPLAYLIST_AI_SERVICE_SETTINGS_LIST,
   DISPLAYLIST_ACCESSIBILITY_SETTINGS_LIST,
   DISPLAYLIST_ONSCREEN_DISPLAY_SETTINGS_LIST,
   DISPLAYLIST_ONSCREEN_NOTIFICATIONS_SETTINGS_LIST,
   DISPLAYLIST_ONSCREEN_NOTIFICATIONS_VIEWS_SETTINGS_LIST,
   DISPLAYLIST_MENU_FILE_BROWSER_SETTINGS_LIST,
   DISPLAYLIST_MENU_VIEWS_SETTINGS_LIST,
   DISPLAYLIST_QUICK_MENU_VIEWS_SETTINGS_LIST,
   DISPLAYLIST_SETTINGS_VIEWS_SETTINGS_LIST,
   DISPLAYLIST_MENU_SETTINGS_LIST,
#ifdef _3DS
   DISPLAYLIST_MENU_BOTTOM_SETTINGS_LIST,
#endif
   DISPLAYLIST_USER_INTERFACE_SETTINGS_LIST,
   DISPLAYLIST_POWER_MANAGEMENT_SETTINGS_LIST,
   DISPLAYLIST_MENU_SOUNDS_LIST,
   DISPLAYLIST_RETRO_ACHIEVEMENTS_SETTINGS_LIST,
   DISPLAYLIST_CHEEVOS_APPEARANCE_SETTINGS_LIST,
   DISPLAYLIST_CHEEVOS_VISIBILITY_SETTINGS_LIST,
   DISPLAYLIST_UPDATER_SETTINGS_LIST,
   DISPLAYLIST_BLUETOOTH_SETTINGS_LIST,
   DISPLAYLIST_WIFI_SETTINGS_LIST,
   DISPLAYLIST_NETWORK_SETTINGS_LIST,
   DISPLAYLIST_NETWORK_HOSTING_SETTINGS_LIST,
   DISPLAYLIST_NETPLAY_KICK_LIST,
   DISPLAYLIST_NETPLAY_BAN_LIST,
   DISPLAYLIST_NETPLAY_LOBBY_FILTERS_LIST,
   DISPLAYLIST_NETPLAY_LAN_SCAN_SETTINGS_LIST,
   DISPLAYLIST_LAKKA_SERVICES_LIST,
#ifdef HAVE_LAKKA_SWITCH
   DISPLAYLIST_LAKKA_SWITCH_OPTIONS_LIST,
#endif
   DISPLAYLIST_USER_SETTINGS_LIST,
   DISPLAYLIST_DIRECTORY_SETTINGS_LIST,
   DISPLAYLIST_PRIVACY_SETTINGS_LIST,
   DISPLAYLIST_MIDI_SETTINGS_LIST,
   DISPLAYLIST_RECORDING_SETTINGS_LIST,
   DISPLAYLIST_PLAYLIST_SETTINGS_LIST,
   DISPLAYLIST_PLAYLIST_MANAGER_LIST,
   DISPLAYLIST_ADD_TO_PLAYLIST_LIST,
   DISPLAYLIST_ADD_TO_PLAYLIST_QUICKMENU,
   DISPLAYLIST_PLAYLIST_MANAGER_SETTINGS,
   DISPLAYLIST_ACCOUNTS_CHEEVOS_LIST,
   DISPLAYLIST_ACCOUNTS_YOUTUBE_LIST,
   DISPLAYLIST_ACCOUNTS_TWITCH_LIST,
   DISPLAYLIST_ACCOUNTS_FACEBOOK_LIST,
   DISPLAYLIST_BROWSE_URL_LIST,
   DISPLAYLIST_BROWSE_URL_START,
   DISPLAYLIST_LOAD_CONTENT_LIST,
   DISPLAYLIST_LOAD_CONTENT_SPECIAL,
   DISPLAYLIST_INFORMATION_LIST,
   DISPLAYLIST_INFORMATION,
   DISPLAYLIST_CONTENT_SETTINGS,
   DISPLAYLIST_OPTIONS,
   DISPLAYLIST_OPTIONS_CHEATS,
   DISPLAYLIST_OPTIONS_REMAPPINGS,
   DISPLAYLIST_OPTIONS_REMAPPINGS_PORT,
   DISPLAYLIST_OPTIONS_MANAGEMENT,
   DISPLAYLIST_OPTIONS_DISK,
   DISPLAYLIST_OPTIONS_SHADERS,
   DISPLAYLIST_OPTIONS_OVERRIDES,
   DISPLAYLIST_NETPLAY,
   DISPLAYLIST_ADD_CONTENT_LIST,
   DISPLAYLIST_CONFIGURATIONS_LIST,
   DISPLAYLIST_SCAN_DIRECTORY_LIST,
   DISPLAYLIST_NETPLAY_ROOM_LIST,
   DISPLAYLIST_ARCHIVE_ACTION,
   DISPLAYLIST_ARCHIVE_ACTION_DETECT_CORE,
   DISPLAYLIST_CORE_CONTENT,
   DISPLAYLIST_CORE_CONTENT_DIRS,
   DISPLAYLIST_CORE_CONTENT_DIRS_SUBDIR,
   DISPLAYLIST_CORE_SYSTEM_FILES,
#if defined(HAVE_LIBNX)
   DISPLAYLIST_SWITCH_CPU_PROFILE,
#endif
   DISPLAYLIST_MANUAL_CONTENT_SCAN_LIST,
   DISPLAYLIST_MANUAL_CONTENT_SCAN_DAT_FILES,
   DISPLAYLIST_CORE_RESTORE_BACKUP_LIST,
   DISPLAYLIST_CORE_DELETE_BACKUP_LIST,
   DISPLAYLIST_WIFI_NETWORKS_LIST,
#if defined(HAVE_LAKKA)
   DISPLAYLIST_CPU_PERFPOWER_LIST,
   DISPLAYLIST_CPU_POLICY_LIST,
#endif
#ifdef HAVE_GAME_AI
   DISPLAYLIST_OPTIONS_GAME_AI,
#endif
   DISPLAYLIST_PENDING_CLEAR,
   DISPLAYLIST_SHADER_PRESET_PREPEND,
   DISPLAYLIST_SHADER_PRESET_APPEND
};

enum filebrowser_enums
{
   FILEBROWSER_NONE              = 0,
   FILEBROWSER_SELECT_DIR,
   FILEBROWSER_SCAN_DIR,
   FILEBROWSER_SCAN_FILE,
   FILEBROWSER_MANUAL_SCAN_DIR,
   FILEBROWSER_SELECT_FILE,
   FILEBROWSER_SELECT_FILE_SUBSYSTEM,
   FILEBROWSER_SELECT_IMAGE,
   FILEBROWSER_SELECT_VIDEO_FONT,
   FILEBROWSER_SELECT_COLLECTION
};

enum menu_dl_flags
{
   MD_FLAG_NONE                          = 0,
   MD_FLAG_NEED_SORT                     = (1 << 0), /* Should the displaylist be sorted by alphabet? */
   MD_FLAG_NEED_REFRESH                  = (1 << 1),
   MD_FLAG_NEED_ENTRIES_REFRESH          = (1 << 2),
   MD_FLAG_NEED_PUSH                     = (1 << 3),
   MD_FLAG_NEED_PUSH_NO_PLAYLIST_ENTRIES = (1 << 4),
   MD_FLAG_NEED_CLEAR                    = (1 << 5), /* Should we clear the displaylist before we push
						      * entries onto it? */
   MD_FLAG_PUSH_BUILTIN_CORES            = (1 << 6),
   MD_FLAG_DOWNLOAD_CORE                 = (1 << 7), /* Should a 'download core' entry be pushed onto the list?
						      * This will be set to true in case there are no currently
						      * installed cores. */
   MD_FLAG_NEED_NAVIGATION_CLEAR         = (1 << 8)  /* Does the navigation index need to be cleared
                                                      * to 0 (first entry) ? */
};

typedef struct menu_displaylist_info
{
   char *path;
   char *path_b;
   char *path_c;
   char *exts;
   char *label;
   file_list_t *list;
   rarch_setting_t *setting;

   size_t directory_ptr;

   uint32_t flags;

   unsigned type;
   unsigned type_default;
   enum msg_hash_enums enum_idx;
} menu_displaylist_info_t;

bool menu_displaylist_process(menu_displaylist_info_t *info);

void menu_displaylist_info_free(menu_displaylist_info_t *info);

unsigned menu_displaylist_build_list(
      file_list_t *list,
      settings_t *settings,
      enum menu_displaylist_ctl_state type,
      bool include_everything);

void menu_displaylist_info_init(menu_displaylist_info_t *info);

bool menu_displaylist_ctl(enum menu_displaylist_ctl_state type, menu_displaylist_info_t *info, settings_t *settings);

bool menu_displaylist_has_subsystems(void);

#if defined(HAVE_LIBRETRODB)
unsigned menu_displaylist_explore(file_list_t *list, settings_t *settings);
#endif
unsigned menu_displaylist_contentless_cores(file_list_t *list,
      enum menu_contentless_cores_display_type core_display_type);

enum filebrowser_enums filebrowser_get_type(void);

void filebrowser_clear_type(void);

void filebrowser_set_type(enum filebrowser_enums type);

int menu_displaylist_parse_settings_enum(
      file_list_t *info_list,
      enum menu_displaylist_parse_type parse_type,
      bool add_empty_entry,
      rarch_setting_t *setting,
      unsigned entry_type,
      bool is_enum, bool menu_show_advanced_settings
      );

RETRO_END_DECLS

#endif
