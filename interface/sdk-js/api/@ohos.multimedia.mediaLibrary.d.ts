/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

import { AsyncCallback, Callback } from './basic';
import { Context } from './app/context';
import image from './@ohos.multimedia.image';

/**
 * @name mediaLibrary
 * @since 6
 * @syscap SystemCapability.Multimedia.MediaLibrary.Core
 * @import import media from '@ohos.multimedia.mediaLibrary'
 */
declare namespace mediaLibrary {
  /**
   * Obtains a MediaLibrary instance.
   * @since 6
   * @syscap SystemCapability.Multimedia.MediaLibrary.Core
   * @import import mediaLibrary from '@ohos.multimedia.mediaLibrary'
   * @FAModelOnly
   * @return Returns a MediaLibrary instance if the operation is successful; returns null otherwise.
   */
  function getMediaLibrary(): MediaLibrary;
  /**
   * Returns an instance of MediaLibrary
   * @since 8
   * @syscap SystemCapability.Multimedia.MediaLibrary.Core
   * @StageModelOnly
   * @param context hap context information
   * @return Instance of MediaLibrary
   */
  function getMediaLibrary(context: Context): MediaLibrary;

  /**
   * Enumeration types for different kind of Media Files
   * @since 8
   * @syscap SystemCapability.Multimedia.MediaLibrary.Core
   */
  enum MediaType {
    /**
     * File media type
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    FILE = 0,
    /**
     * Image media type
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    IMAGE,
    /**
     * Video media type
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    VIDEO,
    /**
     * Audio media type
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    AUDIO
  }

  /**
   * Describes media resource options.
   * @since 6
   * @syscap SystemCapability.Multimedia.MediaLibrary.Core
   * @import import mediaLibrary from '@ohos.multimedia.mediaLibrary'
   */
  interface MediaAssetOption {
    /**
     * URI of the media source.
     * @since 6
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    src: string;
    /**
     * Multipurpose Internet Mail Extensions (MIME) type of the media.
     * @since 6
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    mimeType: string;
    /**
     * Relative path for storing media resources.
     * @since 6
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    relativePath?: string;
  }

  /**
   * Describes media selection options.
   * @since 6
   * @syscap SystemCapability.Multimedia.MediaLibrary.Core
   * @import import mediaLibrary from '@ohos.multimedia.mediaLibrary'
   */
  interface MediaSelectOption {
    /**
     * Media type, which can be image, video, or media (indicating both image and video).
     * @since 6
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    type: 'image' | 'video' | 'media';
    /**
     * Maximum number of media items that can be selected
     * @since 6
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    count: number;
  }

  /**
   * Provides methods to encapsulate file attributes.
   * @since 7
   * @syscap SystemCapability.Multimedia.MediaLibrary.Core
   * @import import mediaLibrary from '@ohos.multimedia.mediaLibrary'
   */
  interface FileAsset {
    /**
     * File ID.
     * @since 7
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    readonly id: number;
    /**
     * URI of the file.
     * @since 7
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    readonly uri: string;
    /**
     * MIME type, for example, video/mp4, audio/mp4, or audio/amr-wb.
     * @since 7
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    readonly mimeType: string;
    /**
     * Media type, for example, IMAGE, VIDEO, FILE, AUDIO 
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    readonly mediaType: MediaType;
    /**
     * Display name (with a file name extension) of the file.
     * @since 7
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    displayName: string;
    /**
     * File name title (without the file name extension).
     * @since 7
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    title: string;
    /**
     * Relative Path of the file.
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    relativePath: string;
    /**
     * Parent folder's file_id of the file.
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    readonly parent: number;
    /**
     * Data size of the file.
     * @since 7
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    readonly size: number;
    /**
     * Date (timestamp) when the file was added.
     * @since 7
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    readonly dateAdded: number;
    /**
     * Date (timestamp) when the file was modified.
     * @since 7
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    readonly dateModified: number;
    /**
     * Date (timestamp) when the file was taken.
     * @since 7
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    readonly dateTaken: number;
    /**
     * Artist of the audio file.
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    readonly artist: string;
    /**
     * audioAlbum of the audio file.
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    readonly audioAlbum: string;
    /**
     * Display width of the file. This is valid only for videos and images.
     * @since 7
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    readonly width: number;
    /**
     * Display height of the file. This is valid only for videos and images.
     * @since 7
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    readonly height: number;
    /**
     * Rotation angle of the file, in degrees.
     * The rotation angle can be 0, 90, 180, or 270 degrees. This is valid only for videos.
     * @since 7
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    orientation: number;
    /**
     * duration of the audio and video file.
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    readonly duration: number;
    /**
     * ID of the album where the file is located.
     * @since 7
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    readonly albumId: number;
    /**
     * URI of the album where the file is located.
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    readonly albumUri: string;
    /**
     * Name of the album where the file is located.
     * @since 7
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    readonly albumName: string;

    /**
     * If it is a directory where the file is located.
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     * @permission ohos.permission.READ_MEDIA
     * @param callback Callback return the result of isDerectory.
     */
    isDirectory(callback: AsyncCallback<boolean>): void;
    /**
     * If it is a directory where the file is located.
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     * @permission ohos.permission.READ_MEDIA
     */
    isDirectory():Promise<boolean>;
    /**
     * Modify meta data where the file is located.
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     * @permission ohos.permission.READ_MEDIA and ohos.permission.WRITE_MEDIA
     * @param callback no value will be returned.
     */
    commitModify(callback: AsyncCallback<void>): void;
    /**
     * Modify meta data where the file is located.
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     * @permission ohos.permission.READ_MEDIA and ohos.permission.WRITE_MEDIA
     */
    commitModify(): Promise<void>;
    /**
     * Open the file is located.
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     * @permission ohos.permission.READ_MEDIA or ohos.permission.WRITE_MEDIA
     * @param mode mode for open, for example: rw, r, w.
     * @param callback Callback return the fd of the file.
     */
    open(mode: string, callback: AsyncCallback<number>): void;
    /**
     * Open the file is located.
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     * @permission ohos.permission.READ_MEDIA or ohos.permission.WRITE_MEDIA
     * @param mode mode for open, for example: rw, r, w.
     */
    open(mode: string): Promise<number>;
    /**
     * Close the file is located.
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     * @permission ohos.permission.READ_MEDIA or ohos.permission.WRITE_MEDIA
     * @param fd fd of the file which had been opened
     * @param callback no value will be returned.
     */
    close(fd: number, callback: AsyncCallback<void>): void;
    /**
     * Close the file is located.
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     * @permission ohos.permission.READ_MEDIA or ohos.permission.WRITE_MEDIA
     * @param fd fd of the file which had been opened
     */
    close(fd: number): Promise<void>;
    /**
     * Get thumbnail of the file when the file is located.
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     * @permission ohos.permission.READ_MEDIA
     * @param callback Callback used to return the thumbnail's pixelmap.
     */
    getThumbnail(callback: AsyncCallback<image.PixelMap>): void;
    /**
     * Get thumbnail of the file when the file is located.
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     * @permission ohos.permission.READ_MEDIA
     * @param size thumbnail's size
     * @param callback Callback used to return the thumbnail's pixelmap.
     */
    getThumbnail(size: Size, callback: AsyncCallback<image.PixelMap>): void;
    /**
     * Get thumbnail of the file when the file is located.
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     * @permission ohos.permission.READ_MEDIA
     * @param size thumbnail's size
     */
    getThumbnail(size?: Size): Promise<image.PixelMap>;
    /**
     * Set favorite for the file when the file is located.
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     * @permission ohos.permission.READ_MEDIA and ohos.permission.WRITE_MEDIA
     * @param isFavorite ture is favorite file, false is not favorite file
     * @param callback Callback used to return, No value is returned.
     */
    favorite(isFavorite: boolean, callback: AsyncCallback<void>): void;
    /**
     * Set favorite for the file when the file is located.
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     * @permission ohos.permission.READ_MEDIA and ohos.permission.WRITE_MEDIA
     * @param isFavorite ture is favorite file, false is not favorite file
     */
    favorite(isFavorite: boolean): Promise<void>;
    /**
     * If the file is favorite when the file is located.
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     * @permission ohos.permission.READ_MEDIA
     * @param callback Callback used to return true or false.
     */
    isFavorite(callback: AsyncCallback<boolean>): void;
    /**
     * If the file is favorite when the file is located.
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     * @permission ohos.permission.READ_MEDIA
     */
    isFavorite():Promise<boolean>;
    /**
     * Set trash for the file when the file is located.
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     * @permission ohos.permission.READ_MEDIA and ohos.permission.WRITE_MEDIA
     * @param isTrash true is trashed file, false is not trashed file
     * @param callback Callback used to return, No value is returned.
     */
    trash(isTrash: boolean, callback: AsyncCallback<void>): void;
    /**
     * Set trash for the file when the file is located.
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     * @permission ohos.permission.READ_MEDIA and ohos.permission.WRITE_MEDIA
     * @param isTrash true is trashed file, false is not trashed file
     */
    trash(isTrash: boolean): Promise<void>;
    /**
     * If the file is in trash when the file is located.
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     * @permission ohos.permission.READ_MEDIA
     * @param callback Callback used to return true or false.
     */
    isTrash(callback: AsyncCallback<boolean>): void;
    /**
     * If the file is in trash when the file is located.
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     * @permission ohos.permission.READ_MEDIA
     */
    isTrash():Promise<boolean>;
  }

  /**
   * Describes MediaFetchOptions's selection
   * @since 8
   * @syscap SystemCapability.Multimedia.MediaLibrary.Core
   */
  enum FileKey {
    /**
     * File ID
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    ID = "file_id",
    /**
     * Relative Path
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    RELATIVE_PATH = "relative_path",
    /**
     * File name
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    DISPLAY_NAME = "display_name",
    /**
     * Parent folder file id
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    PARENT = "parent",
    /**
     * Mime type of the file
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    MIME_TYPE = "mime_type",
    /**
     * Media type of the file
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    MEDIA_TYPE = "media_type",
    /**
     * Size of the file
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    SIZE = "size",
    /**
     * Date of the file creation
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    DATE_ADDED = "date_added",
    /**
     * Modify date of the file
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    DATE_MODIFIED = "date_modified",
    /**
     * Date taken of the file
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    DATE_TAKEN = "date_taken",
    /**
     * Title of the file
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    TITLE = "title",
    /**
     * Artist of the audio file
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    ARTIST = "artist",
    /**
     * Audio album of the audio file
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    AUDIOALBUM = "audio_album",
    /**
     * Duration of the audio and video file
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    DURATION = "duration",
    /**
     * Width of the image file
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    WIDTH = "width",
    /**
     * Height of the image file
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    HEIGHT = "height",
    /**
     * Orientation of the image file
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    ORIENTATION = "orientation",
    /**
     * Album id of the file
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    ALBUM_ID = "bucket_id",
    /**
     * Album name of the file
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    ALBUM_NAME = "bucket_display_name",
  }

  /**
   * Fetch parameters applicable on images, videos, audios, albums and other media
   * @since 7
   * @syscap SystemCapability.Multimedia.MediaLibrary.Core
   */
  interface MediaFetchOptions {
    /**
     * Fields to retrieve, for example, selections: "media_type =? OR media_type =?".
     * @since 7
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    selections: string;
    /**
     * Conditions for retrieval, for example, selectionArgs: [IMAGE, VIDEO].
     * @since 7
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    selectionArgs: Array<string>;
    /**
     * Sorting criterion of the retrieval results, for example, order: "datetaken DESC,display_name DESC, file_id DESC".
     * @since 7
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    order?: string;
    /**
     * uri for retrieval
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    uri?: string;
    /**
     * networkId for retrieval
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    networkId?: string;
    /**
     * extendArgs for retrieval
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    extendArgs?: string;
  }

  /**
   * Implements file retrieval.
   * @since 7
   * @syscap SystemCapability.Multimedia.MediaLibrary.Core
   * @import import mediaLibrary from '@ohos.multimedia.mediaLibrary'
   */
  interface FetchFileResult {
    /**
     * Obtains the total number of files in the file retrieval result.
     * @since 7
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     * @return Total number of files.
     */
    getCount(): number;
    /**
     * Checks whether the result set points to the last row.
     * @since 7
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     * @return Whether the file is the last one.
     * You need to check whether the file is the last one before calling getNextObject,
     * which returns the next file only when True is returned for this method.
     */
    isAfterLast(): boolean;
    /**
     * Releases the FetchFileResult instance and invalidates it. Other methods cannot be called.
     * @since 7
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    close(): void;
    /**
     * Obtains the first FileAsset in the file retrieval result. This method uses a callback to return the file.
     * @since 7
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     * @param callback Callback used to return the file in the format of a FileAsset instance.
     */
    getFirstObject(callback: AsyncCallback<FileAsset>): void;
    /**
     * Obtains the first FileAsset in the file retrieval result. This method uses a promise to return the file.
     * @since 7
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     * @return A Promise instance used to return the file in the format of a FileAsset instance.
     */
    getFirstObject(): Promise<FileAsset>;
    /**
     * Obtains the next FileAsset in the file retrieval result.
     * This method uses a callback to return the file.
     * Before calling this method, you must use isAfterLast() to check whether the result set points to the last row.
     * This method returns the next file only when True is returned for isAfterLast().
     * @since 7
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     * @param callback Callback used to return the file in the format of a FileAsset instance.
     */
    getNextObject(callback: AsyncCallback<FileAsset>): void;
    /**
     * Obtains the next FileAsset in the file retrieval result.
     * This method uses a promise to return the file.
     * Before calling this method, you must use isAfterLast() to check whether the result set points to the last row.
     * This method returns the next file only when True is returned for isAfterLast().
     * @since 7
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     * @return A Promise instance used to return the file in the format of a FileAsset instance.
     */
    getNextObject(): Promise<FileAsset>;
    /**
     * Obtains the last FileAsset in the file retrieval result. This method uses a callback to return the file.
     * @since 7
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     * @param callback Callback used to return the file in the format of a FileAsset instance.
     */
    getLastObject(callback: AsyncCallback<FileAsset>): void;
    /**
     * Obtains the last FileAsset in the file retrieval result. This method uses a promise to return the file.
     * @since 7
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     * @return A Promise instance used to return the file in the format of a FileAsset instance.
     */
    getLastObject(): Promise<FileAsset>;
    /**
     * Obtains the FileAsset with the specified index in the file retrieval result.
     * This method uses a callback to return the file.
     * @since 7
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     * @param index Index of the file to obtain.
     * @param callback Callback used to return the file in the format of a FileAsset instance.
     */
    getPositionObject(index: number, callback: AsyncCallback<FileAsset>): void;
    /**
     * Obtains the FileAsset with the specified index in the file retrieval result.
     * This method uses a promise to return the file.
     * @since 7
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     * @param index Index of the file to obtain.
     * @return A Promise instance used to return the file in the format of a FileAsset instance.
     */
    getPositionObject(index: number): Promise<FileAsset>;
     /**
     * Obtains all FileAssets in the file retrieval result.
     * This method uses a callback to return the result. After this method is called, 
     * close() is automatically called to release the FetchFileResult instance and invalidate it.
     * In this case, other methods cannot be called.
     * @since 7
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     * @param callback Callback used to return a FileAsset array.
     */
    getAllObject(callback: AsyncCallback<Array<FileAsset>>): void;
    /**
     * Obtains all FileAssets in the file retrieval result.
     * This method uses a promise to return the result. that store the selected media resources.
     * close() is automatically called to release the FetchFileResult instance and invalidate it.
     * In this case, other methods cannot be called.
     * @since 7
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     * @return A Promise instance used to return a FileAsset array.
     */
    getAllObject(): Promise<Array<FileAsset>>;
  }

  /**
   * Defines the album.
   *
   * @syscap SystemCapability.Multimedia.MediaLibrary.Core
   * @since 7
   */
  interface Album {
    /**
     * Album ID.
     * @since 7
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    readonly albumId: number;
    /**
     * Album name.
     * @since 7
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    albumName: string;
    /**
     * Album uri.
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    readonly albumUri: string;
    /**
     * Date (timestamp) when the album was last modified.
     * @since 7
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    readonly dateModified: number;
    /**
     * File count for the album
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    readonly count: number;
    /**
     * Relative path for the album
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    readonly relativePath: string;
    /**
     * coverUri for the album
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    readonly coverUri: string;

    /**
     * Modify the meta data for the album
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     * @permission ohos.permission.READ_MEDIA and ohos.permission.WRITE_MEDIA
     * @param callback, no value will be returned.
     */
    commitModify(callback: AsyncCallback<void>): void;
    /**
     * Modify the meta data for the album
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     * @permission ohos.permission.READ_MEDIA and ohos.permission.WRITE_MEDIA
     */
    commitModify(): Promise<void>;
    /**
     * SObtains files in an album. This method uses an asynchronous callback to return the files.
     * @since 7
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     * @permission ohos.permission.READ_MEDIA
     * @param callback Callback used to return the files in the format of a FetchFileResult instance.
     */
    getFileAssets(callback: AsyncCallback<FetchFileResult>): void;
    /**
     * SObtains files in an album. This method uses an asynchronous callback to return the files.
     * @since 7
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     * @permission ohos.permission.READ_MEDIA
     * @param option Media retrieval options.
     * @param callback Callback used to return the files in the format of a FetchFileResult instance.
     */
    getFileAssets(options: MediaFetchOptions, callback: AsyncCallback<FetchFileResult>): void;
    /**
     * Obtains files in an album. This method uses a promise to return the files.
     * @since 7
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     * @permission ohos.permission.READ_MEDIA
     * @param option Media retrieval options.
     * @return A Promise instance used to return the files in the format of a FetchFileResult instance.
     */
    getFileAssets(options?: MediaFetchOptions): Promise<FetchFileResult>;
  }

  /**
   * Enumeration public directory that predefined
   * @since 8
   * @syscap SystemCapability.Multimedia.MediaLibrary.Core
   */
  enum DirectoryType {
    /**
     * predefined public directory for files token by Camera.
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    DIR_CAMERA = 0,
    /**
     * predefined public directory for VIDEO files.
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    DIR_VIDEO,
    /**
     * predefined public directory for IMAGE files.
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    DIR_IMAGE,
    /**
     * predefined public directory for AUDIO files.
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    DIR_AUDIO,
    /**
     * predefined public directory for DOCUMENTS files.
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    DIR_DOCUMENTS,
    /**
     * predefined public directory for DOWNLOAD files.
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    DIR_DOWNLOAD
  }

  /**
   * Defines the MediaLibrary class and provides functions to access the data in media storage.
   *
   * @syscap SystemCapability.Multimedia.MediaLibrary.Core
   * @since 6
   */
  interface MediaLibrary {
    /**
     * get system predefined root dir, use to create file asset by relative path
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     * @param type, public directory predefined in DirectoryType.
     * @param callback Callback return the FetchFileResult.
     */
    getPublicDirectory(type: DirectoryType, callback: AsyncCallback<string>): void;
    /**
     * get system predefined root dir, use to create file asset by relative path
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     * @param type public directory predefined in DirectoryType.
     * @return A promise instance used to return the public directory in the format of string
     */
    getPublicDirectory(type: DirectoryType): Promise<string>;
    /**
     * query all assets just for count & first cover
     * if need all data, getAllObject from FetchFileResult
     * @since 7
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     * @permission ohos.permission.READ_MEDIA
     * @param options, Media retrieval options.
     * @param callback, Callback return the FetchFileResult.
     */
    getFileAssets(options: MediaFetchOptions, callback: AsyncCallback<FetchFileResult>): void;
    /**
     * query all assets just for count & first cover
     * if need all data, getAllObject from FetchFileResult
     * @since 7
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     * @permission ohos.permission.READ_MEDIA
     * @param options Media retrieval options.
     * @return A promise instance used to return the files in the format of a FetchFileResult instance
     */
    getFileAssets(options: MediaFetchOptions): Promise<FetchFileResult>;
    /**
     * Turn on mornitor the data changes by media type
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     * @param type one of 'deviceChange','albumChange','imageChange','audioChange','videoChange','fileChange','remoteFileChange'
     * @param callback no value returned
     */
    on(type: 'deviceChange'|'albumChange'|'imageChange'|'audioChange'|'videoChange'|'fileChange'|'remoteFileChange', callback: Callback<void>): void;
    /**
     * Turn off mornitor the data changes by media type
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     * @param type one of 'deviceChange','albumChange','imageChange','audioChange','videoChange','fileChange','remoteFileChange'
     * @param callback no value returned
     */
     off(type: 'deviceChange'|'albumChange'|'imageChange'|'audioChange'|'videoChange'|'fileChange'|'remoteFileChange', callback?: Callback<void>): void;
    /**
     * Create File Asset
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     * @permission ohos.permission.READ_MEDIA and ohos.permission.WRITE_MEDIA
     * @param mediaType mediaType for example:IMAGE, VIDEO, AUDIO, FILE
     * @param displayName file name
     * @param relativePath relative path
     * @param callback Callback used to return the FileAsset
     */
    createAsset(mediaType: MediaType, displayName: string, relativePath: string, callback: AsyncCallback<FileAsset>): void;
    /**
     * Create File Asset
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     * @permission ohos.permission.READ_MEDIA and ohos.permission.WRITE_MEDIA
     * @param mediaType mediaType for example:IMAGE, VIDEO, AUDIO, FILE
     * @param displayName file name
     * @param relativePath relative path
     * @return A Promise instance used to return the FileAsset
     */
    createAsset(mediaType: MediaType, displayName: string, relativePath: string): Promise<FileAsset>;
    /**
     * Delete File Asset
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     * @permission ohos.permission.READ_MEDIA and ohos.permission.WRITE_MEDIA
     * @param uri FileAsset's URI
     * @param callback no value returned
     * @systemapi
     */
    deleteAsset(uri: string, callback: AsyncCallback<void>): void;
    /**
     * Delete File Asset
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     * @permission ohos.permission.READ_MEDIA and ohos.permission.WRITE_MEDIA
     * @param uri, FileAsset's URI
     * @return A Promise instance, no value returned
     * @systemapi
     */
    deleteAsset(uri: string): Promise<void>;
    /**
     * Obtains albums based on the media retrieval options. This method uses an asynchronous callback to return.
     * @since 7
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     * @permission ohos.permission.READ_MEDIA
     * @param option Media retrieval options.
     * @param callback Callback used to return an album array.
     */
    getAlbums(options: MediaFetchOptions, callback: AsyncCallback<Array<Album>>): void;
    /**
     * Obtains albums based on the media retrieval options. This method uses a promise to return the albums.
     * @since 7
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     * @permission ohos.permission.READ_MEDIA
     * @param option Media retrieval options.
     * @return A Promise instance used to return an album array.
     */
    getAlbums(options: MediaFetchOptions): Promise<Array<Album>>;
    /**
     * Stores media resources. This method uses an asynchronous callback to return the URI that stores
     * the media resources.
     * @since 6
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     * @param option Media resource option.
     * @param callback Callback used to return the URI that stores the media resources.
     */
    storeMediaAsset(option: MediaAssetOption, callback: AsyncCallback<string>): void;
    /**
     * Stores media resources. This method uses a promise to return the URI that stores the media resources.
     * @since 6
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     * @param option Media resource option.
     * @return Promise used to return the URI that stores the media resources.
     */
    storeMediaAsset(option: MediaAssetOption): Promise<string>;
    /**
     * Starts image preview, with the first image to preview specified. This method uses an asynchronous callback
     * to receive the execution result.
     * @since 6
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     * @param images List of images to preview.
     * @param index Sequence number of the first image to preview.
     * @param callback Callback used for image preview. No value is returned.
     */
    startImagePreview(images: Array<string>, index: number, callback: AsyncCallback<void>): void;
    /**
     * Starts image preview. This method uses an asynchronous callback to receive the execution result.
     * @since 6
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     * @param images List of images to preview.
     * @param callback Callback used for image preview. No value is returned.
     */
    startImagePreview(images: Array<string>, callback: AsyncCallback<void>): void;
    /**
     * Starts image preview, with the first image to preview specified.
     * This method uses a promise to return the execution result.
     * @since 6
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     * @param images List of images to preview.
     * @param index Sequence number of the first image to preview.
     * @return Promise used to return whether the operation is successful.
     */
    startImagePreview(images: Array<string>, index?: number): Promise<void>;
    /**
     * Starts media selection. This method uses an asynchronous callback to
     * return the list of URIs that store the selected media resources.
     * @since 6
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     * @param option Media selection option.
     * @param callback Callback used to return the list of URIs that store the selected media resources.
     */
    startMediaSelect(option: MediaSelectOption, callback: AsyncCallback<Array<string>>): void;
    /**
     * Starts media selection. This method uses a promise to return the list of URIs
     * that store the selected media resources.
     * @since 6
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     * @param option Media selection option.
     * @return Promise used to return the list of URIs that store the selected media resources.
     */
    startMediaSelect(option: MediaSelectOption): Promise<Array<string>>;
    /**
     * Get Active Peer device information
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.DistributedCore
     * @permission ohos.permission.READ_MEDIA
     * @systemapi
     * @param callback, Callback return the list of the active peer devices' information
     */
    getActivePeers(callback: AsyncCallback<Array<PeerInfo>>): void;
    /**
     * Get Active Peer device information
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.DistributedCore
     * @permission ohos.permission.READ_MEDIA
     * @systemapi
     * @return Promise used to return the list of the active peer devices' information
     */
    getActivePeers(): Promise<Array<PeerInfo>>;
    /**
     * Get all the peer devices' information
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.DistributedCore
     * @permission ohos.permission.READ_MEDIA
     * @systemapi
     * @param callback Callback return the list of the all the peer devices' information
     */
    getAllPeers(callback: AsyncCallback<Array<PeerInfo>>): void;
    /**
     * Get all the peer devices' information
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.DistributedCore
     * @permission ohos.permission.READ_MEDIA
     * @systemapi
     * @return Promise used to return the list of the all the peer devices' information
     */
    getAllPeers(): Promise<Array<PeerInfo>>;
    /**
     * Release MediaLibrary instance
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     * @param callback no value returned
     */
    release(callback: AsyncCallback<void>): void;
    /**
     * Release MediaLibrary instance
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    release(): Promise<void>;
  }

  /**
   * thumbnail's size which have width and heigh
   * @syscap SystemCapability.Multimedia.MediaLibrary.Core
   * @since 8
   */
  interface Size {
    /**
     * Width of image file
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    width: number;
    /**
     * Height of image file
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.Core
     */
    height: number;
  }
  
  /**
   * peer devices' information
   * @syscap SystemCapability.Multimedia.MediaLibrary.DistributedCore
   * @systemapi
   * @since 8
   */
  interface PeerInfo {
    /**
     * Peer device name
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.DistributedCore
     * @systemapi
     */
    readonly deviceName: string;
    /**
     * Peer device network id
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.DistributedCore
     * @systemapi
     */
    readonly networkId: string;
    /**
     * Peer device type
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.DistributedCore
     * @systemapi
     */
    readonly deviceType: DeviceType;
    /**
     * Peer device online status
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.DistributedCore
     * @systemapi
     */
    readonly isOnline: boolean;
  }

  /**
   * peer device type
   * @syscap SystemCapability.Multimedia.MediaLibrary.DistributedCore
   * @systemapi
   * @since 8
   */
  enum DeviceType {
    /**
     * Unknow device type
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.DistributedCore
     * @systemapi
     */
    TYPE_UNKNOWN = 0,
    /**
     * Laptop device
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.DistributedCore
     * @systemapi
     */
    TYPE_LAPTOP,
    /**
     * Phone device
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.DistributedCore
     * @systemapi
     */
    TYPE_PHONE,
    /**
     * Tablet device
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.DistributedCore
     * @systemapi
     */
    TYPE_TABLET,
    /**
     * Watch device
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.DistributedCore
     * @systemapi
     */
    TYPE_WATCH,
    /**
     * Car device
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.DistributedCore
     * @systemapi
     */
    TYPE_CAR,
    /**
     * TV device
     * @since 8
     * @syscap SystemCapability.Multimedia.MediaLibrary.DistributedCore
     * @systemapi
     */
    TYPE_TV
  }
}

export default mediaLibrary;
