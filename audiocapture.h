/*
 * audiocapture.h
 *
 * Copyright (C) 2011 - ollo
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

//FIXME here we have to describe a structure to define all needed parameters

/*
 * Definition of a function that handles incoming traffic.
 * @param[in] array the values that were captured
 * @param[in] length size of the array mentioned above
 */
typedef void (*handle_data_callback_t)(const double* array, const int length);

typedef enum _AUDIO_CAPTURE_RET {
	AUDIO_CAPTURE_RET_OK,
	AUDIO_CAPTURE_RET_ERROR_FATAL,
	AUDIO_CAPTURE_RET_ERROR_IO
} AUDIO_CAPTURE_RET;

/*******************************************************************************
           Functions
 *******************************************************************************/

/* TODO function description */
AUDIO_CAPTURE_RET audiocapture_scan_audiodevices(void);

/* TODO function description */
AUDIO_CAPTURE_RET audiocapture_init(handle_data_callback_t handler);

/* TODO function description */