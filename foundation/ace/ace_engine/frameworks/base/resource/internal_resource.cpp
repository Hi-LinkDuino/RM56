/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "base/resource/internal_resource.h"

#include <map>

#include "base/log/log.h"

// binary/play.svg
// Use objcopy transform to compiled object file.
// The following parameters represent the beginning and end of the file.
extern uint8_t _binary_play_svg_start[];
extern uint8_t _binary_play_svg_end[];

// binary/pause.svg
// Use objcopy transform to compiled object file.
// The following parameters represent the beginning and end of the file.
extern uint8_t _binary_pause_svg_start[];
extern uint8_t _binary_pause_svg_end[];

// binary/select_arrow_normal.png
// Use objcopy transform to compiled object file.
// The following parameters represent the beginning and end of the file.
extern uint8_t _binary_select_arrow_normal_png_start[];
extern uint8_t _binary_select_arrow_normal_png_end[];

// binary/select_arrow_disabled.png
// Use objcopy transform to compiled object file.
// The following parameters represent the beginning and end of the file.
extern uint8_t _binary_select_arrow_disabled_png_start[];
extern uint8_t _binary_select_arrow_disabled_png_end[];

// binary/full_screen.svg
// Use objcopy transform to compiled object file.
// The following parameters represent the beginning and end of the file.
extern uint8_t _binary_full_screen_svg_start[];
extern uint8_t _binary_full_screen_svg_end[];

// binary/exit_full_screen.svg
// Use objcopy transform to compiled object file.
// The following parameters represent the beginning and end of the file.
extern uint8_t _binary_exit_full_screen_svg_start[];
extern uint8_t _binary_exit_full_screen_svg_end[];

// binary/errorcode.json
// Use objcopy transform to compiled object file.
// The following parameters represent the beginning and end of the file.
extern uint8_t _binary_errorcode_json_start[];
extern uint8_t _binary_errorcode_json_end[];

// binary/tv_rate_star_big_off.svg
// Use objcopy transform to compiled object file.
// The following parameters represent the beginning and end of the file.
extern uint8_t _binary_tv_rate_star_big_off_svg_start[];
extern uint8_t _binary_tv_rate_star_big_off_svg_end[];

// binary/tv_rate_star_big_on.svg
// Use objcopy transform to compiled object file.
// The following parameters represent the beginning and end of the file.
extern uint8_t _binary_tv_rate_star_big_on_svg_start[];
extern uint8_t _binary_tv_rate_star_big_on_svg_end[];

// binary/tv_rate_star_small_off.svg
// Use objcopy transform to compiled object file.
// The following parameters represent the beginning and end of the file.
extern uint8_t _binary_tv_rate_star_small_off_svg_start[];
extern uint8_t _binary_tv_rate_star_small_off_svg_end[];

// binary/tv_rate_star_small_on.png
// Use objcopy transform to compiled object file.
// The following parameters represent the beginning and end of the file.
extern uint8_t _binary_tv_rate_star_small_on_svg_start[];
extern uint8_t _binary_tv_rate_star_small_on_svg_end[];

// binary/rate_star_big_off.svg
// Use objcopy transform to compiled object file.
// The following parameters represent the beginning and end of the file.
extern uint8_t _binary_rate_star_big_off_svg_start[];
extern uint8_t _binary_rate_star_big_off_svg_end[];

// binary/rate_star_big_on.svg
// Use objcopy transform to compiled object file.
// The following parameters represent the beginning and end of the file.
extern uint8_t _binary_rate_star_big_on_svg_start[];
extern uint8_t _binary_rate_star_big_on_svg_end[];

// binary/rate_star_small_off.svg
// Use objcopy transform to compiled object file.
// The following parameters represent the beginning and end of the file.
extern uint8_t _binary_rate_star_small_off_svg_start[];
extern uint8_t _binary_rate_star_small_off_svg_end[];

// binary/rate_star_small_on.svg
// Use objcopy transform to compiled object file.
// The following parameters represent the beginning and end of the file.
extern uint8_t _binary_rate_star_small_on_svg_start[];
extern uint8_t _binary_rate_star_small_on_svg_end[];

// binary/rate_star_small_on.svg
// Use objcopy transform to compiled object file.
// The following parameters represent the beginning and end of the file.
extern uint8_t _binary_rate_star_small_on_svg_start[];
extern uint8_t _binary_rate_star_small_on_svg_end[];

// binary/indexer_arrow.png
// Use objcopy transform to compiled object file.
// The following parameters represent the beginning and end of the file.
extern uint8_t _binary_indexer_arrow_png_start[];
extern uint8_t _binary_indexer_arrow_png_end[];

// binary/correct.svg
// Use objcopy transform to compiled object file.
// The following parameters represent the beginning and end of the file.
extern uint8_t _binary_correct_svg_start[];
extern uint8_t _binary_correct_svg_end[];

// binary/wrong.svg
// Use objcopy transform to compiled object file.
// The following parameters represent the beginning and end of the file.
extern uint8_t _binary_wrong_svg_start[];
extern uint8_t _binary_wrong_svg_end[];

// binary/search.svg
// Use objcopy transform to compiled object file.
// The following parameters represent the beginning and end of the file.
extern uint8_t _binary_search_svg_start[];
extern uint8_t _binary_search_svg_end[];

// binary/close.svg
// Use objcopy transform to compiled object file.
// The following parameters represent the beginning and end of the file.
extern uint8_t _binary_close_svg_start[];
extern uint8_t _binary_close_svg_end[];

// binary/indexletter_bar.json
// Use objcopy transform to compiled object file.
// The following parameters represent the beginning and end of the file.
extern uint8_t _binary_indexletter_bar_json_start[];
extern uint8_t _binary_indexletter_bar_json_end[];

// binary/entry.json
// Use objcopy transform to compiled object file.
// The following parameters represent the beginning and end of the file.
extern uint8_t _binary_entry_json_start[];
extern uint8_t _binary_entry_json_end[];

// binary/up_arrow.svg
// Use objcopy transform to compiled object file.
// The following parameters represent the beginning and end of the file.
extern uint8_t _binary_up_arrow_svg_start[];
extern uint8_t _binary_up_arrow_svg_end[];

// binary/watch_up_arrow.svg
// Use objcopy transform to compiled object file.
// The following parameters represent the beginning and end of the file.
extern uint8_t _binary_watch_up_arrow_svg_start[];
extern uint8_t _binary_watch_up_arrow_svg_end[];

// binary/down_arrow.svg
// Use objcopy transform to compiled object file.
// The following parameters represent the beginning and end of the file.
extern uint8_t _binary_down_arrow_svg_start[];
extern uint8_t _binary_down_arrow_svg_end[];

// binary/watch_down_arrow.svg
// Use objcopy transform to compiled object file.
// The following parameters represent the beginning and end of the file.
extern uint8_t _binary_watch_down_arrow_svg_start[];
extern uint8_t _binary_watch_down_arrow_svg_end[];

// binary/show_password.svg
// Use objcopy transform to compiled object file.
// The following parameters represent the beginning and end of the file.
extern uint8_t _binary_show_password_svg_start[];
extern uint8_t _binary_show_password_svg_end[];

// binary/hide_password.svg
// Use objcopy transform to compiled object file.
// The following parameters represent the beginning and end of the file.
extern uint8_t _binary_hide_password_svg_start[];
extern uint8_t _binary_hide_password_svg_end[];

// binary/semi_modal_bar_down.svg
// Use objcopy transform to compiled object file.
// The following parameters represent the beginning and end of the file.
extern uint8_t _binary_semi_modal_bar_down_svg_start[];
extern uint8_t _binary_semi_modal_bar_down_svg_end[];

// binary/semi_modal_bar_middle.svg
// Use objcopy transform to compiled object file.
// The following parameters represent the beginning and end of the file.
extern uint8_t _binary_semi_modal_bar_middle_svg_start[];
extern uint8_t _binary_semi_modal_bar_middle_svg_end[];

// binary/left_arrow.svg
// Use objcopy transform to compiled object file.
// The following parameters represent the beginning and end of the file.
extern uint8_t _binary_left_arrow_svg_start[];
extern uint8_t _binary_left_arrow_svg_end[];

// binary/right_arrow.svg
// Use objcopy transform to compiled object file.
// The following parameters represent the beginning and end of the file.
extern uint8_t _binary_right_arrow_svg_start[];
extern uint8_t _binary_right_arrow_svg_end[];

// binary/stepper_back_arrow.svg
// Use objcopy transform to compiled object file.
// The following parameters represent the beginning and end of the file.
extern uint8_t _binary_stepper_back_arrow_svg_start[];
extern uint8_t _binary_stepper_back_arrow_svg_end[];

// binary/stepper_next_arrow.svg
// Use objcopy transform to compiled object file.
// The following parameters represent the beginning and end of the file.
extern uint8_t _binary_stepper_next_arrow_svg_start[];
extern uint8_t _binary_stepper_next_arrow_svg_end[];

// binary/piece_delete.svg
// Use objcopy transform to compiled object file.
// The following parameters represent the beginning and end of the file.
extern uint8_t _binary_piece_delete_svg_start[];
extern uint8_t _binary_piece_delete_svg_end[];

// binary/ic_back.svg
// Use objcopy transform to compiled object file.
// The following parameters represent the beginning and end of the file.
extern uint8_t _binary_ic_back_svg_start[];
extern uint8_t _binary_ic_back_svg_end[];

// binary/ic_more.svg
// Use objcopy transform to compiled object file.
// The following parameters represent the beginning and end of the file.
extern uint8_t _binary_ic_more_svg_start[];
extern uint8_t _binary_ic_more_svg_end[];

// binary/ic_more_dark.svg
// Use objcopy transform to compiled object file.
// The following parameters represent the beginning and end of the file.
extern uint8_t _binary_ic_more_dark_svg_start[];
extern uint8_t _binary_ic_more_dark_svg_end[];

// binary/ic_more_transparent.svg
// Use objcopy transform to compiled object file.
// The following parameters represent the beginning and end of the file.
extern uint8_t _binary_ic_more_transparent_svg_start[];
extern uint8_t _binary_ic_more_transparent_svg_end[];

// binary/translate_c2e.svg
// Use objcopy transform to compiled object file.
// The following parameters represent the beginning and end of the file.
extern uint8_t _binary_translate_c2e_svg_start[];
extern uint8_t _binary_translate_c2e_svg_end[];

// binary/share.svg
// Use objcopy transform to compiled object file.
// The following parameters represent the beginning and end of the file.
extern uint8_t _binary_share_svg_start[];
extern uint8_t _binary_share_svg_end[];

// binary/fa_clock_widget_hour.png
extern uint8_t _binary_fa_clock_widget_hour_png_start[];
extern uint8_t _binary_fa_clock_widget_hour_png_end[];

// binary/fa_clock_widget_minute.png
extern uint8_t _binary_fa_clock_widget_minute_png_start[];
extern uint8_t _binary_fa_clock_widget_minute_png_end[];

// binary/fa_clock_widget_second.png
extern uint8_t _binary_fa_clock_widget_second_png_start[];
extern uint8_t _binary_fa_clock_widget_second_png_end[];

// binary/fa_black_clock_widget_hour.png
extern uint8_t _binary_fa_black_clock_widget_hour_png_start[];
extern uint8_t _binary_fa_black_clock_widget_hour_png_end[];

// binary/fa_black_clock_widget_minute.png
extern uint8_t _binary_fa_black_clock_widget_minute_png_start[];
extern uint8_t _binary_fa_black_clock_widget_minute_png_end[];

// binary/fa_black_clock_widget_second.png
extern uint8_t _binary_fa_black_clock_widget_second_png_start[];
extern uint8_t _binary_fa_black_clock_widget_second_png_end[];

// binary/ic_public_drawer.svg
// Use objcopy transform to compiled object file.
// The following parameters represent the beginning and end of the file.
extern uint8_t _binary_ic_public_drawer_svg_start[];
extern uint8_t _binary_ic_public_drawer_svg_end[];

// binary/container_modal_window_close.svg
extern uint8_t _binary_container_modal_window_close_svg_start[];
extern uint8_t _binary_container_modal_window_close_svg_end[];

// binary/container_modal_window_maximize.svg
extern uint8_t _binary_container_modal_window_maximize_svg_start[];
extern uint8_t _binary_container_modal_window_maximize_svg_end[];

// binary/container_modal_window_minimize.svg
extern uint8_t _binary_container_modal_window_minimize_svg_start[];
extern uint8_t _binary_container_modal_window_minimize_svg_end[];

// binary/container_modal_window_recover.svg
extern uint8_t _binary_container_modal_window_recover_svg_start[];
extern uint8_t _binary_container_modal_window_recover_svg_end[];

// binary/container_modal_window_split_left.svg
extern uint8_t _binary_container_modal_window_split_left_svg_start[];
extern uint8_t _binary_container_modal_window_split_left_svg_end[];

// binary/container_modal_window_defocus_close.svg
extern uint8_t _binary_container_modal_window_defocus_close_svg_start[];
extern uint8_t _binary_container_modal_window_defocus_close_svg_end[];

// binary/container_modal_window_defocus_maximize.svg
extern uint8_t _binary_container_modal_window_defocus_maximize_svg_start[];
extern uint8_t _binary_container_modal_window_defocus_maximize_svg_end[];

// binary/container_modal_window_defocus_minimize.svg
extern uint8_t _binary_container_modal_window_defocus_minimize_svg_start[];
extern uint8_t _binary_container_modal_window_defocus_minimize_svg_end[];

// binary/container_modal_window_defocus_recover.svg
extern uint8_t _binary_container_modal_window_defocus_recover_svg_start[];
extern uint8_t _binary_container_modal_window_defocus_recover_svg_end[];

// binary/container_modal_window_defocus_split_left.svg
extern uint8_t _binary_container_modal_window_defocus_split_left_svg_start[];
extern uint8_t _binary_container_modal_window_defocus_split_left_svg_end[];

namespace OHOS::Ace {
namespace {

struct ResourceData final {
    ResourceData(const uint8_t* buf, size_t size) : buf(buf), size(size) {}
    ~ResourceData() = default;

    const uint8_t* buf;
    size_t size;
};

} // namespace

InternalResource::InternalResource() = default;

InternalResource::~InternalResource() = default;

const uint8_t* InternalResource::GetResource(const ResourceId id, size_t& size) const
{
    static const std::map<InternalResource::ResourceId, ResourceData> RESOURCE_MAP = {
        { InternalResource::ResourceId::TV_RATE_STAR_BIG_ON_SVG,
            ResourceData(_binary_tv_rate_star_big_on_svg_start,
                static_cast<size_t>(_binary_tv_rate_star_big_on_svg_end - _binary_tv_rate_star_big_on_svg_start)) },
        { InternalResource::ResourceId::TV_RATE_STAR_BIG_OFF_SVG,
            ResourceData(_binary_tv_rate_star_big_off_svg_start,
                static_cast<size_t>(_binary_tv_rate_star_big_off_svg_end - _binary_tv_rate_star_big_off_svg_start)) },
        { InternalResource::ResourceId::TV_RATE_STAR_SMALL_ON_SVG,
            ResourceData(_binary_tv_rate_star_small_on_svg_start,
                static_cast<size_t>(_binary_tv_rate_star_small_on_svg_end - _binary_tv_rate_star_small_on_svg_start)) },
        { InternalResource::ResourceId::TV_RATE_STAR_SMALL_OFF_SVG,
            ResourceData(_binary_tv_rate_star_small_off_svg_start,
                static_cast<size_t>(
                    _binary_tv_rate_star_small_off_svg_end - _binary_tv_rate_star_small_off_svg_start)) },
        { InternalResource::ResourceId::RATE_STAR_BIG_ON_SVG,
            ResourceData(_binary_rate_star_big_on_svg_start,
                static_cast<size_t>(_binary_rate_star_big_on_svg_end - _binary_rate_star_big_on_svg_start)) },
        { InternalResource::ResourceId::RATE_STAR_BIG_OFF_SVG,
            ResourceData(_binary_rate_star_big_off_svg_start,
                static_cast<size_t>(_binary_rate_star_big_off_svg_end - _binary_rate_star_big_off_svg_start)) },
        { InternalResource::ResourceId::RATE_STAR_SMALL_ON_SVG,
            ResourceData(_binary_rate_star_small_on_svg_start,
                static_cast<size_t>(_binary_rate_star_small_on_svg_end - _binary_rate_star_small_on_svg_start)) },
        { InternalResource::ResourceId::RATE_STAR_SMALL_OFF_SVG,
            ResourceData(_binary_rate_star_small_off_svg_start,
                static_cast<size_t>(_binary_rate_star_small_off_svg_end - _binary_rate_star_small_off_svg_start)) },
        { InternalResource::ResourceId::RATE_STAR_SMALL_OFF_SVG,
            ResourceData(_binary_rate_star_small_off_svg_start,
                static_cast<size_t>(_binary_rate_star_small_off_svg_end - _binary_rate_star_small_off_svg_start)) },
        { InternalResource::ResourceId::INDEXER_ARROW_PNG,
            ResourceData(_binary_indexer_arrow_png_start,
                static_cast<size_t>(_binary_indexer_arrow_png_end - _binary_indexer_arrow_png_start)) },
        { InternalResource::ResourceId::CORRECT_SVG,
            ResourceData(
                _binary_correct_svg_start, static_cast<size_t>(_binary_correct_svg_end - _binary_correct_svg_start)) },
        { InternalResource::ResourceId::WRONG_SVG,
            ResourceData(
                _binary_wrong_svg_start, static_cast<size_t>(_binary_wrong_svg_end - _binary_wrong_svg_start)) },
        { InternalResource::ResourceId::SEARCH_SVG,
            ResourceData(
                _binary_search_svg_start, static_cast<size_t>(_binary_search_svg_end - _binary_search_svg_start)) },
        { InternalResource::ResourceId::CLOSE_SVG,
            ResourceData(
                _binary_close_svg_start, static_cast<size_t>(_binary_close_svg_end - _binary_close_svg_start)) },
        { InternalResource::ResourceId::PLAY_SVG,
            ResourceData(_binary_play_svg_start, static_cast<size_t>(_binary_play_svg_end - _binary_play_svg_start)) },
        { InternalResource::ResourceId::PAUSE_SVG,
            ResourceData(
                _binary_pause_svg_start, static_cast<size_t>(_binary_pause_svg_end - _binary_pause_svg_start)) },
        { InternalResource::ResourceId::FULLSCREEN_SVG,
            ResourceData(_binary_full_screen_svg_start,
                static_cast<size_t>(_binary_full_screen_svg_end - _binary_full_screen_svg_start)) },
        { InternalResource::ResourceId::QUIT_FULLSCREEN_SVG,
            ResourceData(_binary_exit_full_screen_svg_start,
                static_cast<size_t>(_binary_exit_full_screen_svg_end - _binary_exit_full_screen_svg_start)) },
        { InternalResource::ResourceId::ERRORINFO_JSON,
            ResourceData(_binary_errorcode_json_start,
                static_cast<size_t>(_binary_errorcode_json_end - _binary_errorcode_json_start)) },
        { InternalResource::ResourceId::SELECT_ARROW_NORMAL_PNG,
            ResourceData(_binary_select_arrow_normal_png_start,
                static_cast<size_t>(_binary_select_arrow_normal_png_end - _binary_select_arrow_normal_png_start)) },
        { InternalResource::ResourceId::SELECT_ARROW_DISABLED_PNG,
            ResourceData(_binary_select_arrow_disabled_png_start,
                static_cast<size_t>(_binary_select_arrow_disabled_png_end - _binary_select_arrow_disabled_png_start)) },
        { InternalResource::ResourceId::INDEXLETTER_BAR_JSON,
            ResourceData(_binary_indexletter_bar_json_start,
                static_cast<size_t>(_binary_indexletter_bar_json_end - _binary_indexletter_bar_json_start)) },
        { InternalResource::ResourceId::ENTRY_JSON,
            ResourceData(
                _binary_entry_json_start, static_cast<size_t>(_binary_entry_json_end - _binary_entry_json_start)) },
        { InternalResource::ResourceId::UP_ARROW_SVG,
            ResourceData(_binary_up_arrow_svg_start,
                static_cast<size_t>(_binary_up_arrow_svg_end - _binary_up_arrow_svg_start)) },
        { InternalResource::ResourceId::WATCH_UP_ARROW_SVG,
            ResourceData(_binary_watch_up_arrow_svg_start,
                static_cast<size_t>(_binary_watch_up_arrow_svg_end - _binary_watch_up_arrow_svg_start)) },
        { InternalResource::ResourceId::DOWN_ARROW_SVG,
            ResourceData(_binary_down_arrow_svg_start,
                static_cast<size_t>(_binary_down_arrow_svg_end - _binary_down_arrow_svg_start)) },
        { InternalResource::ResourceId::WATCH_DOWN_ARROW_SVG,
            ResourceData(_binary_watch_down_arrow_svg_start,
                static_cast<size_t>(_binary_watch_down_arrow_svg_end - _binary_watch_down_arrow_svg_start)) },
        { InternalResource::ResourceId::SHOW_PASSWORD_SVG,
            ResourceData(_binary_show_password_svg_start,
                static_cast<size_t>(_binary_show_password_svg_end - _binary_show_password_svg_start)) },
        { InternalResource::ResourceId::HIDE_PASSWORD_SVG,
            ResourceData(_binary_hide_password_svg_start,
                static_cast<size_t>(_binary_hide_password_svg_end - _binary_hide_password_svg_start)) },
        { InternalResource::ResourceId::SEMI_MODAL_BAR_DOWN_SVG,
            ResourceData(_binary_semi_modal_bar_down_svg_start,
                static_cast<size_t>(_binary_semi_modal_bar_down_svg_end - _binary_semi_modal_bar_down_svg_start)) },
        { InternalResource::ResourceId::SEMI_MODAL_BAR_MIDDLE_SVG,
            ResourceData(_binary_semi_modal_bar_middle_svg_start,
                static_cast<size_t>(_binary_semi_modal_bar_middle_svg_end - _binary_semi_modal_bar_middle_svg_start)) },
        { InternalResource::ResourceId::LEFT_ARROW_SVG,
            ResourceData(_binary_left_arrow_svg_start,
                static_cast<size_t>(_binary_left_arrow_svg_end - _binary_left_arrow_svg_start)) },
        { InternalResource::ResourceId::RIGHT_ARROW_SVG,
            ResourceData(_binary_right_arrow_svg_start,
                static_cast<size_t>(_binary_right_arrow_svg_end - _binary_right_arrow_svg_start)) },
        { InternalResource::ResourceId::STEPPER_BACK_ARROW,
            ResourceData(_binary_stepper_back_arrow_svg_start,
                static_cast<size_t>(_binary_stepper_back_arrow_svg_end - _binary_stepper_back_arrow_svg_start)) },
        { InternalResource::ResourceId::STEPPER_NEXT_ARROW,
            ResourceData(_binary_stepper_next_arrow_svg_start,
                static_cast<size_t>(_binary_stepper_next_arrow_svg_end - _binary_stepper_next_arrow_svg_start)) },
        { InternalResource::ResourceId::PIECE_DELETE_SVG,
            ResourceData(_binary_piece_delete_svg_start,
                static_cast<size_t>(_binary_piece_delete_svg_end - _binary_piece_delete_svg_start)) },
        { InternalResource::ResourceId::IC_BACK,
            ResourceData(_binary_ic_back_svg_start,
                static_cast<size_t>(_binary_ic_back_svg_end - _binary_ic_back_svg_start)) },
        { InternalResource::ResourceId::IC_MORE,
            ResourceData(_binary_ic_more_svg_start,
                static_cast<size_t>(_binary_ic_more_svg_end - _binary_ic_more_svg_start)) },
        { InternalResource::ResourceId::IC_MORE_DARK,
            ResourceData(_binary_ic_more_dark_svg_start,
                static_cast<size_t>(_binary_ic_more_dark_svg_end - _binary_ic_more_dark_svg_start)) },
        { InternalResource::ResourceId::IC_MORE_TRANSPARENT,
            ResourceData(_binary_ic_more_transparent_svg_start,
                static_cast<size_t>(_binary_ic_more_transparent_svg_end - _binary_ic_more_transparent_svg_start)) },
        { InternalResource::ResourceId::TRANSLATE_C2E_SVG,
            ResourceData(_binary_translate_c2e_svg_start,
                static_cast<size_t>(_binary_translate_c2e_svg_end - _binary_translate_c2e_svg_start)) },
        { InternalResource::ResourceId::SHARE_SVG,
            ResourceData(
                _binary_share_svg_start, static_cast<size_t>(_binary_share_svg_end - _binary_share_svg_start)) },
        { InternalResource::ResourceId::FA_CLOCK_WIDGET_HOUR,
            ResourceData(_binary_fa_clock_widget_hour_png_start,
                static_cast<size_t>(_binary_fa_clock_widget_hour_png_end - _binary_fa_clock_widget_hour_png_start)) },
        { InternalResource::ResourceId::FA_CLOCK_WIDGET_MINUTE,
            ResourceData(_binary_fa_clock_widget_minute_png_start,
                static_cast<size_t>(
                    _binary_fa_clock_widget_minute_png_end - _binary_fa_clock_widget_minute_png_start)) },
        { InternalResource::ResourceId::FA_CLOCK_WIDGET_SECOND,
            ResourceData(_binary_fa_clock_widget_second_png_start,
                static_cast<size_t>(
                    _binary_fa_clock_widget_second_png_end - _binary_fa_clock_widget_second_png_start)) },
        { InternalResource::ResourceId::FA_BLACK_CLOCK_WIDGET_HOUR,
            ResourceData(_binary_fa_black_clock_widget_hour_png_start,
                static_cast<size_t>(
                    _binary_fa_black_clock_widget_hour_png_end - _binary_fa_black_clock_widget_hour_png_start)) },
        { InternalResource::ResourceId::FA_BLACK_CLOCK_WIDGET_MINUTE,
            ResourceData(_binary_fa_black_clock_widget_minute_png_start,
                static_cast<size_t>(
                    _binary_fa_black_clock_widget_minute_png_end - _binary_fa_black_clock_widget_minute_png_start)) },
        { InternalResource::ResourceId::FA_BLACK_CLOCK_WIDGET_SECOND,
            ResourceData(_binary_fa_black_clock_widget_second_png_start,
                static_cast<size_t>(
                    _binary_fa_black_clock_widget_second_png_end - _binary_fa_black_clock_widget_second_png_start)) },
        { InternalResource::ResourceId::SIDE_BAR,
            ResourceData(_binary_ic_public_drawer_svg_start,
                static_cast<size_t>(_binary_ic_public_drawer_svg_end - _binary_ic_public_drawer_svg_start)) },
        { InternalResource::ResourceId::CONTAINER_MODAL_WINDOW_CLOSE,
            ResourceData(_binary_container_modal_window_close_svg_start,
                static_cast<size_t>(_binary_container_modal_window_close_svg_end -
                _binary_container_modal_window_close_svg_start)) },
        { InternalResource::ResourceId::CONTAINER_MODAL_WINDOW_MAXIMIZE,
            ResourceData(_binary_container_modal_window_maximize_svg_start,
                static_cast<size_t>(_binary_container_modal_window_maximize_svg_end -
                _binary_container_modal_window_maximize_svg_start)) },
        { InternalResource::ResourceId::CONTAINER_MODAL_WINDOW_MINIMIZE,
            ResourceData(_binary_container_modal_window_minimize_svg_start,
                static_cast<size_t>(_binary_container_modal_window_minimize_svg_end -
                _binary_container_modal_window_minimize_svg_start)) },
        { InternalResource::ResourceId::CONTAINER_MODAL_WINDOW_RECOVER,
            ResourceData(_binary_container_modal_window_recover_svg_start,
                static_cast<size_t>(_binary_container_modal_window_recover_svg_end -
                _binary_container_modal_window_recover_svg_start)) },
        { InternalResource::ResourceId::CONTAINER_MODAL_WINDOW_SPLIT_LEFT,
            ResourceData(_binary_container_modal_window_split_left_svg_start,
                static_cast<size_t>(_binary_container_modal_window_split_left_svg_end -
                _binary_container_modal_window_split_left_svg_start)) },
        { InternalResource::ResourceId::CONTAINER_MODAL_WINDOW_DEFOCUS_CLOSE,
            ResourceData(_binary_container_modal_window_defocus_close_svg_start,
                static_cast<size_t>(_binary_container_modal_window_defocus_close_svg_end -
                                    _binary_container_modal_window_defocus_close_svg_start)) },
        { InternalResource::ResourceId::CONTAINER_MODAL_WINDOW_DEFOCUS_MAXIMIZE,
            ResourceData(_binary_container_modal_window_defocus_maximize_svg_start,
                static_cast<size_t>(_binary_container_modal_window_defocus_maximize_svg_end -
                                    _binary_container_modal_window_defocus_maximize_svg_start)) },
        { InternalResource::ResourceId::CONTAINER_MODAL_WINDOW_DEFOCUS_MINIMIZE,
            ResourceData(_binary_container_modal_window_defocus_minimize_svg_start,
                static_cast<size_t>(_binary_container_modal_window_defocus_minimize_svg_end -
                                    _binary_container_modal_window_defocus_minimize_svg_start)) },
        { InternalResource::ResourceId::CONTAINER_MODAL_WINDOW_DEFOCUS_RECOVER,
            ResourceData(_binary_container_modal_window_defocus_recover_svg_start,
                static_cast<size_t>(_binary_container_modal_window_defocus_recover_svg_end -
                                    _binary_container_modal_window_defocus_recover_svg_start)) },
        { InternalResource::ResourceId::CONTAINER_MODAL_WINDOW_DEFOCUS_SPLIT_LEFT,
            ResourceData(_binary_container_modal_window_defocus_split_left_svg_start,
                static_cast<size_t>(_binary_container_modal_window_defocus_split_left_svg_end -
                                    _binary_container_modal_window_defocus_split_left_svg_start)) },
    };
    auto iter = RESOURCE_MAP.find(id);
    if (iter != RESOURCE_MAP.end()) {
        size = iter->second.size;
        return iter->second.buf;
    }
    return nullptr;
}

} // namespace OHOS::Ace
