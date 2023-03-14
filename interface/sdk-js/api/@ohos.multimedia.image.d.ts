/*
* Copyright (C) 2021 Huawei Device Co., Ltd.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

import { AsyncCallback } from './basic';

/**
 * @name image
 * @since 6
 * @import import image from '@ohos.multimedia.image';
 */
declare namespace image {

  /**
   * Enumerates pixel map formats.
   * @since 7
   * @syscap SystemCapability.Multimedia.Image.Core
   */
  enum PixelMapFormat {
    /**
     * Indicates an unknown format.
     * @since 7
     * @syscap SystemCapability.Multimedia.Image.Core
     */
    UNKNOWN = 0,

    /**
     * Indicates that each pixel is stored on 16 bits. Only the R, G, and B components are encoded
     * from the higher-order to the lower-order bits: red is stored with 5 bits of precision,
     * green is stored with 6 bits of precision, and blue is stored with 5 bits of precision.
     * @since 7
     * @syscap SystemCapability.Multimedia.Image.Core
     */
    RGB_565 = 2,

    /**
     * Indicates that each pixel is stored on 32 bits. Components R, G, B, and A each occupies 8 bits
     * and are stored from the higher-order to the lower-order bits.
     * @since 7
     * @syscap SystemCapability.Multimedia.Image.Core
     */
    RGBA_8888 = 3,
  }

  /**
   * Describes the size of an image.
   * @since 6
   * @syscap SystemCapability.Multimedia.Image.Core
   */
  interface Size {
    /**
     * Height
     * @since 6
     * @syscap SystemCapability.Multimedia.Image.Core
     */
    height: number;

    /**
     * Width
     * @since 6
     * @syscap SystemCapability.Multimedia.Image.Core
     */
    width: number;
  }

  /**
   * Enumerates exchangeable image file format (Exif) information types of an image.
   * @since 7
   * @syscap SystemCapability.Multimedia.Image.Core
   */
  enum PropertyKey {
    /**
     * Number of bits in each pixel of an image.
     * @since 7
     * @syscap SystemCapability.Multimedia.Image.Core
     */
    BITS_PER_SAMPLE = "BitsPerSample",

    /**
     * Image rotation mode.
     * @since 7
     * @syscap SystemCapability.Multimedia.Image.Core
     */
    ORIENTATION = "Orientation",

    /**
     * Image length.
     * @since 7
     * @syscap SystemCapability.Multimedia.Image.Core
     */
    IMAGE_LENGTH = "ImageLength",

    /**
     * Image width.
     * @since 7
     * @syscap SystemCapability.Multimedia.Image.Core
     */
    IMAGE_WIDTH = "ImageWidth",

    /**
     * GPS latitude.
     * @since 7
     * @syscap SystemCapability.Multimedia.Image.Core
     */
    GPS_LATITUDE = "GPSLatitude",

    /**
     * GPS longitude.
     * @since 7
     * @syscap SystemCapability.Multimedia.Image.Core
     */
    GPS_LONGITUDE = "GPSLongitude",

    /**
     * GPS latitude reference. For example, N indicates north latitude and S indicates south latitude.
     * @since 7
     * @syscap SystemCapability.Multimedia.Image.Core
     */
    GPS_LATITUDE_REF = "GPSLatitudeRef",

    /**
     * GPS longitude reference. For example, E indicates east longitude and W indicates west longitude.
     * @since 7
     * @syscap SystemCapability.Multimedia.Image.Core
     */
    GPS_LONGITUDE_REF = "GPSLongitudeRef"
  }

  /**
   * Enum for image formats.
   * @since 9
   * @syscap SystemCapability.Multimedia.Image.Core
   */
   enum ImageFormat {
    /**
     * YCBCR422 semi-planar format.
     * @since 9
     * @syscap SystemCapability.Multimedia.Image.Core
     */
    YCBCR_422_SP = 1000,

    /**
     * JPEG encoding format.
     * @since 9
     * @syscap SystemCapability.Multimedia.Image.Core
     */
    JPEG = 2000
  }

  /**
   * Enumerates alpha types.
   * @since 9
   * @syscap SystemCapability.Multimedia.Image.Core
   */
  enum AlphaType {
    /**
     * Indicates an unknown alpha type.
     * @since 9
     * @syscap SystemCapability.Multimedia.Image.Core
     */
    UNKNOWN = 0,

    /**
     * Indicates that the image has no alpha channel, or all pixels in the image are fully opaque.
     * @since 9
     * @syscap SystemCapability.Multimedia.Image.Core
     */
    OPAQUE = 1,

    /**
     * Indicates that RGB components of each pixel in the image are premultiplied by alpha.
     * @since 9
     * @syscap SystemCapability.Multimedia.Image.Core
     */
    PREMUL = 2,

    /**
     * Indicates that RGB components of each pixel in the image are independent of alpha and are not premultiplied by alpha.
     * @since 9
     * @syscap SystemCapability.Multimedia.Image.Core
     */
    UNPREMUL = 3
  }

  /**
   * Enum for image scale mode.
   * @since 9
   * @syscap SystemCapability.Multimedia.Image.Core
   */
  enum ScaleMode {
    /**
     * Indicates the effect that fits the image into the target size.
     * @since 9
     * @syscap SystemCapability.Multimedia.Image.Core
     */
    FIT_TARGET_SIZE = 0,

    /**
     * Indicates the effect that scales an image to fill the target image area and center-crops the part outside the area.
     * @since 9
     * @syscap SystemCapability.Multimedia.Image.Core
     */
    CENTER_CROP = 1,
  }

  /**
   * The componet type of image.
   * @since 9
   * @syscap SystemCapability.Multimedia.Image.ImageReceiver
   */
  enum ComponentType {
    /**
     * Luma info.
     * @since 9
     * @syscap SystemCapability.Multimedia.Image.ImageReceiver
     */
    YUV_Y = 1,

    /**
     * Chrominance info.
     * @since 9
     * @syscap SystemCapability.Multimedia.Image.ImageReceiver
     */
    YUV_U = 2,

    /**
     * Chroma info.
     * @since 9
     * @syscap SystemCapability.Multimedia.Image.ImageReceiver
     */
    YUV_V = 3,

    /**
     * Jpeg type.
     * @since 9
     * @syscap SystemCapability.Multimedia.Image.ImageReceiver
     */
    JPEG = 4, 
  }

  /**
   * Describes region information.
   * @since 8
   * @syscap SystemCapability.Multimedia.Image.Core
   */
  interface Region {
    /**
     * Image size.
     * @since 7
     * @syscap SystemCapability.Multimedia.Image.Core
     */
    size: Size;

    /**
     * x-coordinate at the upper left corner of the image.
     * @since 7
     * @syscap SystemCapability.Multimedia.Image.Core
     */
    x: number;

    /**
     * y-coordinate at the upper left corner of the image.
     * @since 7
     * @syscap SystemCapability.Multimedia.Image.Core
     */
    y: number;
  }

  /**
   * Describes area information in an image.
   * @since 7
   * @syscap SystemCapability.Multimedia.Image.Core
   */
  interface PositionArea {
    /**
     * Image data that will be read or written.
     * @since 7
     * @syscap SystemCapability.Multimedia.Image.Core
     */
    pixels: ArrayBuffer;

    /**
     * Offset for data reading.
     * @since 7
     * @syscap SystemCapability.Multimedia.Image.Core
     */
    offset: number;

    /**
     * Number of bytes to read.
     * @since 7
     * @syscap SystemCapability.Multimedia.Image.Core
     */
    stride: number;

    /**
     * Region to read.
     * @since 7
     * @syscap SystemCapability.Multimedia.Image.Core
     */
    region: Region;
  }

  /**
   * Describes image information.
   * @since 6
   * @syscap SystemCapability.Multimedia.Image.Core
   */
  interface ImageInfo {
    /**
     * Indicates image dimensions specified by a {@link Size} interface.
     * @since 6
     * @syscap SystemCapability.Multimedia.Image.Core
     */
    size: Size;
  }

  /**
   * Describes the option for image packing.
   * @since 6
   * @syscap SystemCapability.Multimedia.Image.ImagePacker
   */
  interface PackingOption {
    /**
     * Multipurpose Internet Mail Extensions (MIME) format of the target image, for example, image/jpeg.
     * @since 6
     * @syscap SystemCapability.Multimedia.Image.ImagePacker
     */
    format: string;

    /**
     * Quality of the target image. The value is an integer ranging from 0 to 100. A larger value indicates better
     * image quality but larger space occupied.
     * @since 6
     * @syscap SystemCapability.Multimedia.Image.ImagePacker
     */
    quality: number;
  }

  /**
   * Describes image properties.
   * @since 7
   * @syscap SystemCapability.Multimedia.Image.ImageSource
   */
  interface GetImagePropertyOptions {
    /**
     * Index of an image.
     * @since 7
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     */
    index?: number;

    /**
     * Default property value.
     * @since 7
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     */
    defaultValue?: string;
  }

  /**
   * Describes image decoding parameters.
   * @since 7
   * @syscap SystemCapability.Multimedia.Image.ImageSource
   */
  interface DecodingOptions {
    /**
     * Number of image frames.
     * @since 7
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     */
    index?: number;

    /**
     * Sampling ratio of the image pixel map.
     * @since 7
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     */
    sampleSize?: number;

    /**
     * Rotation angle of the image pixel map. The value ranges from 0 to 360.
     * @since 7
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     */
    rotate?: number;

    /**
     * Whether the image pixel map is editable.
     * @since 7
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     */
    editable?: boolean;

    /**
     * Width and height of the image pixel map. The value (0, 0) indicates that the pixels are decoded
     * based on the original image size.
     * @since 7
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     */
    desiredSize?: Size;

    /**
     * Cropping region of the image pixel map.
     * @since 7
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     */
    desiredRegion?: Region;

    /**
     * Data format of the image pixel map.
     * @since 7
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     */
    desiredPixelFormat?: PixelMapFormat;
  }

  /**
   * Describes image color components.
   * @since 9
   * @syscap SystemCapability.Multimedia.Image.Core
   */
   interface Component {
    /**
     * Component type.
     * @since 9
     * @syscap SystemCapability.Multimedia.Image.Core
     */
    readonly componentType: ComponentType;

    /**
     * Row stride.
     * @since 9
     * @syscap SystemCapability.Multimedia.Image.Core
     */
    readonly rowStride: number;

    /**
     * Pixel stride.
     * @since 9
     * @syscap SystemCapability.Multimedia.Image.Core
     */
    readonly pixelStride: number;

    /**
     * Component buffer.
     * @since 9
     * @syscap SystemCapability.Multimedia.Image.Core
     */
    readonly byteBuffer: ArrayBuffer;
  }

  /**
   * Initialization options for pixelmap.
   * @since 8
   * @syscap SystemCapability.Multimedia.Image.Core
   */
  interface InitializationOptions {
    /**
     * PixelMap size.
     * @since 8
     * @syscap SystemCapability.Multimedia.Image.Core
     */
    size: Size;

    /**
     * PixelMap expected format.
     * @since 8
     * @syscap SystemCapability.Multimedia.Image.Core
     */
    pixelFormat?: PixelMapFormat;

    /**
     * Editable or not.
     * @since 8
     * @syscap SystemCapability.Multimedia.Image.Core
     */
    editable?: boolean;

    /**
     * PixelMap expected alpha type.
     * @since 9
     * @syscap SystemCapability.Multimedia.Image.Core
     */
    alphaType?: AlphaType;

    /**
     * PixelMap expected scaling effect.
     * @since 9
     * @syscap SystemCapability.Multimedia.Image.Core
     */
    scaleMode?: ScaleMode;
  }

  /**
   * Create pixelmap by data buffer.
   * @since 8
   * @syscap SystemCapability.Multimedia.Image.Core
   */
  function createPixelMap(colors: ArrayBuffer, options: InitializationOptions, callback: AsyncCallback<PixelMap>): void;

  /**
   * Create pixelmap by data buffer.
   * @since 8
   * @syscap SystemCapability.Multimedia.Image.Core
   */
  function createPixelMap(colors: ArrayBuffer, options: InitializationOptions): Promise<PixelMap>;

  /**
   * Creates an ImageSource instance based on the URI.
   * @since 6
   * @syscap SystemCapability.Multimedia.Image.ImageSource
   * @param uri Image source URI.
   * @return Returns the ImageSource instance if the operation is successful; returns null otherwise.
   */
  function createImageSource(uri: string): ImageSource;

  /**
   * Creates an ImageSource instance based on the file descriptor.
   * @since 7
   * @syscap SystemCapability.Multimedia.Image.ImageSource
   * @param fd ID of a file descriptor.
   * @return Returns the ImageSource instance if the operation is successful; returns null otherwise.
   */
  function createImageSource(fd: number): ImageSource;

  /**
   * Creates an ImageSource instance based on the buffer.
   * @since 9
   * @syscap SystemCapability.Multimedia.Image.ImageSource
   * @param buf The buffer of the iamge.
   * @return Returns the ImageSource instance if the operation is successful; returns null otherwise.
   */
  function createImageSource(buf: ArrayBuffer): ImageSource;

  /**
   * Creates an ImageSource instance based on the buffer in incremental.
   * @since 9
   * @syscap SystemCapability.Multimedia.Image.ImageSource
   * @param buf The buffer of the iamge.
   * @return Returns the ImageSource instance if the operation is successful; returns null otherwise.
   */
  function CreateIncrementalSource(buf: ArrayBuffer): ImageSource;

  /**
   * Creates an ImagePacker instance.
   * @since 6
   * @syscap SystemCapability.Multimedia.Image.ImagePacker
   * @return Returns the ImagePacker instance if the operation is successful; returns null otherwise.
   */
  function createImagePacker(): ImagePacker;

  /**
   * Creates an ImageReceiver instance.
   * @since 9
   * @syscap SystemCapability.Multimedia.Image.ImageReceiver
   * @param width The default width in pixels of the Images that this receiver will produce.
   * @param height The default height in pixels of the Images that this receiver will produce.
   * @param format The format of the Image that this receiver will produce. This must be one of the
   *            {@link ImageFormat} constants. Note that not all formats are supported, like ImageFormat.NV21.
   * @param capacity The maximum number of images the user will want to access simultaneously.
   * @return Returns the ImageReceiver instance if the operation is successful; returns null otherwise.
   */
  function createImageReceiver(width: number, height: number, format: number, capacity: number): ImageReceiver;

  /**
   * PixelMap instance.
   * @since 7
   * @syscap SystemCapability.Multimedia.Image.Core
   */
  interface PixelMap {
    /**
     * Whether the image pixel map can be edited.
     * @since 7
     * @syscap SystemCapability.Multimedia.Image.Core
     */
    readonly isEditable: boolean;

    /**
     * Reads image pixel map data and writes the data to an ArrayBuffer. This method uses
     * a promise to return the result.
     * @since 7
     * @syscap SystemCapability.Multimedia.Image.Core
     * @param dst A buffer to which the image pixel map data will be written.
     * @return A Promise instance used to return the operation result. If the operation fails, an error message is returned.
     */
    readPixelsToBuffer(dst: ArrayBuffer): Promise<void>;

    /**
     * Reads image pixel map data and writes the data to an ArrayBuffer. This method uses
     * a callback to return the result.
     * @since 7
     * @syscap SystemCapability.Multimedia.Image.Core
     * @param dst A buffer to which the image pixel map data will be written.
     * @param callback Callback used to return the operation result. If the operation fails, an error message is returned.
     */
    readPixelsToBuffer(dst: ArrayBuffer, callback: AsyncCallback<void>): void;

    /**
     * Reads image pixel map data in an area. This method uses a promise to return the data read.
     * @since 7
     * @syscap SystemCapability.Multimedia.Image.Core
     * @param area Area from which the image pixel map data will be read.
     * @return A Promise instance used to return the operation result. If the operation fails, an error message is returned.
     */
    readPixels(area: PositionArea): Promise<void>;

    /**
     * Reads image pixel map data in an area. This method uses a callback to return the data read.
     * @since 7
     * @syscap SystemCapability.Multimedia.Image.Core
     * @param area Area from which the image pixel map data will be read.
     * @param callback Callback used to return the operation result. If the operation fails, an error message is returned.
     */
    readPixels(area: PositionArea, callback: AsyncCallback<void>): void;

    /**
     * Writes image pixel map data to the specified area. This method uses a promise to return
     * the operation result.
     * @since 7
     * @syscap SystemCapability.Multimedia.Image.Core
     * @param area Area to which the image pixel map data will be written.
     * @return A Promise instance used to return the operation result. If the operation fails, an error message is returned.
     */
    writePixels(area: PositionArea): Promise<void>;

    /**
     * Writes image pixel map data to the specified area. This method uses a callback to return
     * the operation result.
     * @since 7
     * @syscap SystemCapability.Multimedia.Image.Core
     * @param area Area to which the image pixel map data will be written.
     * @param callback Callback used to return the operation result. If the operation fails, an error message is returned.
     */
    writePixels(area: PositionArea, callback: AsyncCallback<void>): void;

    /**
     * Reads image data in an ArrayBuffer and writes the data to a PixelMap object. This method
     * uses a promise to return the result.
     * @since 7
     * @syscap SystemCapability.Multimedia.Image.Core
     * @param src A buffer from which the image data will be read.
     * @return A Promise instance used to return the operation result. If the operation fails, an error message is returned.
     */
    writeBufferToPixels(src: ArrayBuffer): Promise<void>;

    /**
     * Reads image data in an ArrayBuffer and writes the data to a PixelMap object. This method
     * uses a callback to return the result.
     * @since 7
     * @syscap SystemCapability.Multimedia.Image.Core
     * @param src A buffer from which the image data will be read.
     * @param callback Callback used to return the operation result. If the operation fails, an error message is returned.
     */
    writeBufferToPixels(src: ArrayBuffer, callback: AsyncCallback<void>): void;

    /**
     * Obtains pixel map information about this image. This method uses a promise to return the information.
     * @since 7
     * @syscap SystemCapability.Multimedia.Image.Core
     * @return A Promise instance used to return the image pixel map information. If the operation fails, an error message is returned.
     */
    getImageInfo(): Promise<ImageInfo>;

    /**
     * Obtains pixel map information about this image. This method uses a callback to return the information.
     * @since 7
     * @syscap SystemCapability.Multimedia.Image.Core
     * @param callback Callback used to return the image pixel map information. If the operation fails, an error message is returned.
     */
    getImageInfo(callback: AsyncCallback<ImageInfo>): void;

    /**
     * Obtains the number of bytes in each line of the image pixel map.
     * @since 7
     * @syscap SystemCapability.Multimedia.Image.Core
     * @return Number of bytes in each line.
     */
    getBytesNumberPerRow(): number;

    /**
     * Obtains the total number of bytes of the image pixel map.
     * @since 7
     * @syscap SystemCapability.Multimedia.Image.Core
     * @return Total number of bytes.
     */
    getPixelBytesNumber(): number;

    /**
     * Releases this PixelMap object. This method uses a callback to return the result.
     * @since 7
     * @syscap SystemCapability.Multimedia.Image.Core
     * @param callback Callback invoked for instance release. If the operation fails, an error message is returned.
     */
    release(callback: AsyncCallback<void>): void;

    /**
     * Releases this PixelMap object. This method uses a promise to return the result.
     * @since 7
     * @syscap SystemCapability.Multimedia.Image.Core
     * @return A Promise instance used to return the instance release result. If the operation fails, an error message is returned.
     */
    release(): Promise<void>;
  }

  /**
   * ImageSource instance.
   * @since 6
   * @syscap SystemCapability.Multimedia.Image.ImageSource
   */
  interface ImageSource {
    /**
     * Obtains information about an image with the specified sequence number and uses a callback
     * to return the result.
     * @since 6
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @param index Sequence number of an image.
     * @param callback Callback used to return the image information.
     */
    getImageInfo(index: number, callback: AsyncCallback<ImageInfo>): void;

    /**
     * Obtains information about this image and uses a callback to return the result.
     * @since 6
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @param callback Callback used to return the image information.
     */
    getImageInfo(callback: AsyncCallback<ImageInfo>): void;

    /**
     * Get image information from image source.
     * @since 6
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @param index Sequence number of an image. If this parameter is not specified, the default value 0 is used.
     * @return A Promise instance used to return the image information.
     */
    getImageInfo(index?: number): Promise<ImageInfo>;

    /**
     * Creates a PixelMap object based on image decoding parameters. This method uses a promise to
     * return the object.
     * @since 7
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @param options Image decoding parameters.
     * @return A Promise instance used to return the PixelMap object.
     */
    createPixelMap(options?: DecodingOptions): Promise<PixelMap>;
    
    /**
     * Creates a PixelMap object. This method uses a callback to return the object.
     * @since 7
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @param callback Callback used to return the PixelMap object.
     */
    createPixelMap(callback: AsyncCallback<PixelMap>): void;

    /**
     * Creates a PixelMap object based on image decoding parameters. This method uses a callback to
     * return the object.
     * @since 7
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @param options Image decoding parameters.
     * @param callback Callback used to return the PixelMap object.
     */
    createPixelMap(options: DecodingOptions, callback: AsyncCallback<PixelMap>): void;

    /**
     * Obtains the value of a property in an image with the specified index. This method uses a
     * promise to return the property value in a string.
     * @since 7
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @param key Name of the property whose value is to be obtained.
     * @param options Index of the image.
     * @return A Promise instance used to return the property value. If the operation fails, the default value is returned.
     */
    getImageProperty(key: string, options?: GetImagePropertyOptions): Promise<string>;

    /**
     * Obtains the value of a property in this image. This method uses a callback to return the
     * property value in a string.
     * @since 7
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @param key Name of the property whose value is to be obtained.
     * @param callback Callback used to return the property value. If the operation fails, an error message is returned.
     */
    getImageProperty(key: string, callback: AsyncCallback<string>): void;
	
    /**
     * Obtains the value of a property in an image with the specified index. This method uses
     * a callback to return the property value in a string.
     * @since 7
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @param key Name of the property whose value is to be obtained.
     * @param options Index of the image.
     * @param callback Callback used to return the property value. If the operation fails, the default value is returned.
     */
    getImageProperty(key: string, options: GetImagePropertyOptions, callback: AsyncCallback<string>): void;

    /**
     * Modify the value of a property in an image with the specified key. This method uses a
     * promise to return the property value in a string.
     * @since 9
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @param key Name of the property whose value is to be modified.
     * @param value The value to be set to property.
     * @return A Promise instance used to return the property value.
     */
    modifyImageProperty(key: string, value: string): Promise<void>;

    /**
     * Modify the value of a property in an image with the specified key. This method uses a callback to return the
     * property value in a string.
     * @since 9
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @param key Name of the property whose value is to be obtained.
     * @param value The value to be set to property.
     * @param callback Callback to return the operation result.
     */
    modifyImageProperty(key: string, value: string, callback: AsyncCallback<void>): void;

    /**
     * Update the data in the incremental ImageSource.
     * @since 9
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @param buf The data to be updated.
     * @param isFinished If is it finished.
     * @param value The offset of data.
     * @param length The lenght fo buf.
     * @return A Promise instance used to return the property value.
     */
    updateData(buf: ArrayBuffer, isFinished: boolean, value: number, length: number): Promise<void>;

    /**
     * Update the data in the incremental ImageSource.
     * @since 9
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @param buf The data to be updated.
     * @param isFinished If is it finished.
     * @param value The offset of data.
     * @param length The lenght fo buf.
     * @param callback Callback to return the operation result.
     */
    updateData(buf: ArrayBuffer, isFinished: boolean, value: number, length: number, callback: AsyncCallback<void>): void;

    /**
     * Releases an ImageSource instance and uses a callback to return the result.
     * @since 6
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @param callback Callback to return the operation result.
     */
    release(callback: AsyncCallback<void>): void;

    /**
     * Releases an ImageSource instance and uses a promise to return the result.
     * @since 6
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     * @return A Promise instance used to return the operation result.
     */
    release(): Promise<void>;

    /**
     * Supported image formats.
     * @since 6
     * @syscap SystemCapability.Multimedia.Image.ImageSource
     */
    readonly supportedFormats: Array<string>;
  }

  /**
   * ImagePacker instance.
   * @since 6
   * @syscap SystemCapability.Multimedia.Image.ImagePacker
   */
  interface ImagePacker {
    /**
     * Compresses or packs an image and uses a callback to return the result.
     * @since 6
     * @syscap SystemCapability.Multimedia.Image.ImagePacker
     * @param source Image to be processed.
     * @param option Option for image packing.
     * @param callback Callback used to return the packed data.
     */
    packing(source: ImageSource, option: PackingOption, callback: AsyncCallback<ArrayBuffer>): void;

    /**
     * Compresses or packs an image and uses a promise to return the result.
     * @since 6
     * @syscap SystemCapability.Multimedia.Image.ImagePacker
     * @param source Image to be processed.
     * @param option Option for image packing.
     * @return A Promise instance used to return the compressed or packed data.
     */
    packing(source: ImageSource, option: PackingOption): Promise<ArrayBuffer>;

    /**
     * Compresses or packs an image and uses a callback to return the result.
     * @since 8
     * @syscap SystemCapability.Multimedia.Image.ImagePacker
     * @param source PixelMap to be processed.
     * @param option Option for image packing.
     * @param callback Callback used to return the packed data.
     */
     packing(source: PixelMap, option: PackingOption, callback: AsyncCallback<ArrayBuffer>): void;

     /**
      * Compresses or packs an image and uses a promise to return the result.
      * @since 8
      * @syscap SystemCapability.Multimedia.Image.ImagePacker
      * @param source PixelMap to be processed.
      * @param option Option for image packing.
      * @return A Promise instance used to return the compressed or packed data.
      */
     packing(source: PixelMap, option: PackingOption): Promise<ArrayBuffer>;

    /**
     * Releases an ImagePacker instance and uses a callback to return the result.
     * @since 6
     * @syscap SystemCapability.Multimedia.Image.ImagePacker
     * @param callback Callback to return the operation result.
     */
    release(callback: AsyncCallback<void>): void;

    /**
     * Releases an ImagePacker instance and uses a promise to return the result.
     * @since 6
     * @syscap SystemCapability.Multimedia.Image.ImagePacker
     * @return A Promise instance used to return the operation result.
     */
    release(): Promise<void>;

    /**
     * Supported image formats.
     * @since 6
     * @syscap SystemCapability.Multimedia.Image.ImagePacker
     */
    readonly supportedFormats: Array<string>;
  }

  /**
   * Provides basic image operations, including obtaining image information, and reading and writing image data.
   * @since 9
   * @syscap SystemCapability.Multimedia.Image.Core
   */
  interface Image {
    /**
     * Sets or gets the image area to crop, default is size.
     * @since 9
     * @syscap SystemCapability.Multimedia.Image.Core
     */
    clipRect: Region;

    /**
     * Image size.
     * @since 9
     * @syscap SystemCapability.Multimedia.Image.Core
     */
    readonly size: Size;

    /**
     * Image format.
     * @since 9
     * @syscap SystemCapability.Multimedia.Image.Core
     */
    readonly format: number;

    /**
     * Get component buffer from image and uses a callback to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Image.Core
     * @param componentType The componet type of image.
     * @param callback Callback used to return the component buffer.
     */
    getComponent(componentType: ComponentType, callback: AsyncCallback<Component>): void;

    /**
     * Get component buffer from image and uses a promise to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Image.Core
     * @param componentType The componet type of image.
     * @return A Promise instance used to return the component buffer.
     */
    getComponent(componentType: ComponentType): Promise<Component>;

    /**
     * Release current image to receive another and uses a callback to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Image.Core
     * @param callback Callback to return the operation result.
     */
    release(callback: AsyncCallback<void>): void;

    /**
     * Release current image to receive another and uses a promise to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Image.Core
     * @return A Promise instance used to return the operation result.
     */
    release(): Promise<void>;
  }

  /**
   * Image receiver object.
   * @since 9
   * @syscap SystemCapability.Multimedia.Image.ImageReceiver
   */
  interface ImageReceiver {
    /**
     * Image size.
     * @since 9
     * @syscap SystemCapability.Multimedia.Image.ImageReceiver
     */
    readonly size: Size;

    /**
     * Image capacity.
     * @since 9
     * @syscap SystemCapability.Multimedia.Image.ImageReceiver
     */
    readonly capacity: number;

    /**
     * Image format.
     * @since 9
     * @syscap SystemCapability.Multimedia.Image.ImageReceiver
     */
    readonly format: ImageFormat;

    /**
     * get an id which indicates a surface and can be used to set to Camera or other component can receive a surface
     * and uses a callback to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Image.ImageReceiver
     * @param callback Callback used to return the surface id.
     */
    getReceivingSurfaceId(callback: AsyncCallback<string>): void;

    /**
     * get an id which indicates a surface and can be used to set to Camera or other component can receive a surface
     * and uses a promise to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Image.ImageReceiver
     * @return A Promise instance used to return the surface id.
     */
    getReceivingSurfaceId(): Promise<string>;

    /**
     * Get lasted image from receiver and uses a callback to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Image.ImageReceiver
     * @param callback Callback used to return the latest image.
     */
    readLatestImage(callback: AsyncCallback<Image>): void;

    /**
     * Get lasted image from receiver and uses a promise to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Image.ImageReceiver
     * @return A Promise instance used to return the latest image.
     */
    readLatestImage(): Promise<Image>;

    /**
     * Get next image from receiver and uses a callback to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Image.ImageReceiver
     * @param callback Callback used to return the next image.
     */
    readNextImage(callback: AsyncCallback<Image>): void;

    /**
     * Get next image from receiver and uses a promise to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Image.ImageReceiver
     * @return A Promise instance used to return the next image.
     */
    readNextImage(): Promise<Image>;

    /**
     * Subscribe callback when receiving an image
     * @since 9
     * @syscap SystemCapability.Multimedia.Image.ImageReceiver
     * @param type Callback used to return the next image.
     * @param callback Callback used to return image.
     */
    on(type: 'imageArrival', callback: AsyncCallback<void>): void;

    /**
     * Release image receiver instance and uses a callback to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Image.ImageReceiver
     * @param callback Callback to return the operation result.
     */
    release(callback: AsyncCallback<void>): void;

    /**
     * Release image receiver instance and uses a promise to return the result.
     * @since 9
     * @syscap SystemCapability.Multimedia.Image.ImageReceiver
     * @return A Promise instance used to return the operation result.
     */
    release(): Promise<void>;
  }
}

export default image;