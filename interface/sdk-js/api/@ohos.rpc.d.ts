/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

import { AsyncCallback } from './basic'

declare namespace rpc {
    /**
     * A data object used for reomote procedure call (RPC).
     * <p>
     * During RPC, the sender can use the write methods provided by {@link MessageParcel} to
     * write the to-be-sent data into a {@link MessageParcel} object in a specific format, and the receiver can use the
     * read methods provided by {@link MessageParcel} to read data of the specific format from the {@link MessageParcel} object.
     * <p>
     * <p>
     * The default capacity of a {@link MessageParcel} instance is 200KB. If you want more or less, use {@link #setCapacity(int)}
     * to change it.
     * </p>
     * <b>Note</b>: Only data of the following data types can be written into or read from a {@link MessageParcel}: byte,
     * byteArray, short, shortArray, int, intArray, long, longArray, float, floatArray, double, doubleArray, boolean,
     * booleanArray, char, charArray, String, StringArray, {@link IRemoteObject}, IRemoteObjectArray,
     * {@link Sequenceable}, and SequenceableArray.
     *
     * @since 7
     * @syscap SystemCapability.Communication.IPC.Core
     * @import import rpc from '@ohos.rpc'
     */
    class MessageParcel {
        /**
         * Creates an empty {@link MessageParcel} object.
         * @return Returns the object created.
         * @since 7
         */
        static create(): MessageParcel;

        /**
         * Reclaims the {@link MessageParcel} object.
         * @since 7
         */
        reclaim(): void;

        /**
         * Serializes a remote object and writes it to the {@link MessageParcel} object.
         * @param object Remote object to serialize.
         * @return Returns true if it is successful; returns false otherwise.
         * @since 7
         */
        writeRemoteObject(object: IRemoteObject): boolean;

        /**
         * Reads a remote object from {@link MessageParcel} object.
         * @return Returns the remote object.
         * @since 7
         */
        readRemoteObject(): IRemoteObject;

        /**
         * Writes an interface token into the {@link MessageParcel} object.
         * @param token Interface descriptor to write.
         * @return Returns {@code true} if the interface token has been written into the {@link MessageParcel};
         *         returns {@code false} otherwise.
         * @since 7
         */
        writeInterfaceToken(token: string): boolean;

        /**
         * Reads an interface token from the {@link MessageParcel} object.
         * @return Returns a string value.
         * @since 7
         */
        readInterfaceToken(): string;

        /**
         * Obtains the size of data (in bytes) contained in the {@link MessageParcel} object.
         * @return Returns the size of data contained in the {@link MessageParcel} object.
         * @since 7
         */
        getSize(): number;

        /**
         * Obtains the storage capacity (in bytes) of the {@link MessageParcel} object.
         * @return Returns the storage capacity of the {@link MessageParcel} object.
         * @since 7
         */
        getCapacity(): number;

        /**
         * Sets the size of data (in bytes) contained in the {@link MessageParcel} object.
         * <p>{@code false} is returned if the data size set in this method is greater
         * than the storage capacity of the {@link MessageParcel}.
         *
         * @param size Indicates the data size of the {@link MessageParcel} object.
         * @return Returns {@code true} if the setting is successful; returns {@code false} otherwise.
         * @since 7
         */
        setSize(size: number): boolean;

        /**
         * Sets the storage capacity (in bytes) of the {@link MessageParcel} object.
         * <p>{@code false} is returned if the capacity set in this method is less than
         * the size of data contained in the {@link MessageParcel}.
         *
         * @param size Indicates the storage capacity of the {@link MessageParcel} object.
         * @return Returns {@code true} if the setting is successful; returns {@code false} otherwise.
         * @since 7
         */
        setCapacity(size: number): boolean;

        /**
         * Obtains the writable data space (in bytes) in the {@link MessageParcel} object.
         * <p>Writable data space = Storage capacity of the {@link MessageParcel} – Size of data contained in the {@link MessageParcel}.
         *
         * @return Returns the writable data space of the {@link MessageParcel} object.
         * @since 7
         */
        getWritableBytes(): number;

        /**
         * Obtains the readable data space (in bytes) in the {@link MessageParcel} object.
         * <p>Readable data space = Size of data contained in the {@link MessageParcel} – Size of data that has been read.
         *
         * @return Returns the readable data space of the {@link MessageParcel} object.
         * @since 7
         */
        getReadableBytes(): number;

        /**
         * Obtains the current read position in the {@link MessageParcel} object.
         * @return Returns the current read position in the {@link MessageParcel} object.
         * @since 7
         */
        getReadPosition(): number;

        /**
         * Obtains the current write position in the {@link MessageParcel} object.
         * @return Returns the current write position in the {@link MessageParcel} object.
         * @since 7
         */
        getWritePosition(): number;

        /**
         * Changes the current read position in the {@link MessageParcel} object.
         * <p>Generally, you are advised not to change the current read position. If you must
         * change it, change it to an accurate position. Otherwise, the read data may be incorrect.
         *
         * @param pos Indicates the target position to start data reading.
         * @return Returns {@code true} if the read position is changed; returns {@code false} otherwise.
         * @since 7
         */
        rewindRead(pos: number): boolean;

        /**
         * Changes the current write position in the {@link MessageParcel} object.
         * <p>Generally, you are advised not to change the current write position. If you must
         * change it, change it to an accurate position. Otherwise, the data to be read may be incorrect.
         *
         * @param pos Indicates the target position to start data writing.
         * @return Returns {@code true} if the write position is changed; returns {@code false} otherwise.
         * @since 7
         */
        rewindWrite(pos: number): boolean;

        /**
         * Writes information to this MessageParcel object indicating that no exception occurred.
         * <p>After handling requests, you should call this method before writing any data to reply {@link MessageParcel}.
         * @since 8
         */
        writeNoException(): void;

        /**
         * Reads the exception information from this MessageParcel object.
         * <p>If exception was thrown in server side, it will be thrown here.
         * This method should be called before reading any data from reply {@link MessageParcel}
         * if {@link writeNoException} was invoked in server side.
         * @throws Throws an exception if it thrown in server side.
         * @since 8
         */
        readException(): void;

        /**
         * Writes a byte value into the {@link MessageParcel} object.
         * @param val Indicates the byte value to write.
         * @return Returns {@code true} if the value has been written into the {@link MessageParcel};
         *         returns {@code false} otherwise.
         * @throws ParcelException When capacity in this parcel is insufficient,
         *         exception message: {@link ParcelException#NO_CAPACITY_ERROR}.
         * @since 7
         */
        writeByte(val: number): boolean;

        /**
         * Writes a short integer value into the {@link MessageParcel} object.
         * @param val Indicates the short integer value to write.
         * @return Returns {@code true} if the value has been written into the {@link MessageParcel};
         *         returns {@code false} otherwise.
         * @throws ParcelException When capacity in this parcel is insufficient,
         *         exception message: {@link ParcelException#NO_CAPACITY_ERROR}.
         * @since 7
         */
        writeShort(val: number): boolean;

        /**
         * Writes an integer value into the {@link MessageParcel} object.
         * @param val Indicates the integer value to write.
         * @return Returns {@code true} if the value has been written into the {@link MessageParcel};
         *         returns {@code false} otherwise.
         * @throws ParcelException When capacity in this parcel is insufficient,
         *         exception message: {@link ParcelException#NO_CAPACITY_ERROR}.
         * @since 7
         */
        writeInt(val: number): boolean;

        /**
         * Writes a long integer value into the {@link MessageParcel} object.
         * @param val Indicates the long integer value to write.
         * @return Returns {@code true} if the value has been written into the {@link MessageParcel};
         *         returns {@code false} otherwise.
         * @throws ParcelException When capacity in this parcel is insufficient,
         *         exception message: {@link ParcelException#NO_CAPACITY_ERROR}.
         * @since 7
         */
        writeLong(val: number): boolean;

        /**
         * Writes a floating point value into the {@link MessageParcel} object.
         * @param val Indicates the floating point value to write.
         * @return Returns {@code true} if the value has been written into the {@link MessageParcel};
         *         returns {@code false} otherwise.
         * @throws ParcelException When capacity in this parcel is insufficient,
         *         exception message: {@link ParcelException#NO_CAPACITY_ERROR}.
         * @since 7
         */
        writeFloat(val: number): boolean;

        /**
         * Writes a double-precision floating point value into the {@link MessageParcel} object.
         * @param val Indicates the double-precision floating point value to write.
         * @return Returns {@code true} if the value has been written into the {@link MessageParcel};
         *         returns {@code false} otherwise.
         * @throws ParcelException When capacity in this parcel is insufficient,
         *         exception message: {@link ParcelException#NO_CAPACITY_ERROR}.
         * @since 7
         */
        writeDouble(val: number): boolean;

        /**
         * Writes a boolean value into the {@link MessageParcel} object.
         * @param val Indicates the boolean value to write.
         * @return Returns {@code true} if the value has been written into the {@link MessageParcel};
         *         returns {@code false} otherwise.
         * @throws ParcelException When capacity in this parcel is insufficient,
         *         exception message: {@link ParcelException#NO_CAPACITY_ERROR}.
         * @since 7
         */
        writeBoolean(val: boolean): boolean;

        /**
         * Writes a single character value into the {@link MessageParcel} object.
         * @param val Indicates the single character value to write.
         * @return Returns {@code true} if the value has been written into the {@link MessageParcel};
         *         returns {@code false} otherwise.
         * @throws ParcelException When capacity in this parcel is insufficient,
         *         exception message: {@link ParcelException#NO_CAPACITY_ERROR}.
         * @since 7
         */
        writeChar(val: number): boolean;

        /**
         * Writes a string value into the {@link MessageParcel} object.
         * @param val Indicates the string value to write.
         * @return Returns {@code true} if the value has been written into the {@link MessageParcel};
         *         returns {@code false} otherwise.
         * @throws ParcelException When capacity in this parcel is insufficient,
         *         exception message: {@link ParcelException#NO_CAPACITY_ERROR}.
         * @since 7
         */
        writeString(val: string): boolean;

        /**
         * Writes a {@link Sequenceable} object into the {@link MessageParcel} object.
         * @param val Indicates the {@link Sequenceable} object to write.
         * @throws ParcelException When capacity in this parcel is insufficient,
         *         exception message: {@link ParcelException#NO_CAPACITY_ERROR}.
         * @since 7
         */
        writeSequenceable(val: Sequenceable): boolean;

        /**
         * Writes a byte array into the {@link MessageParcel} object.
         * @param byteArray Indicates the byte array to write.
         * @return Returns {@code true} if the array has been written into the {@link MessageParcel};
         *         returns {@code false} otherwise.
         * @throws ParcelException When capacity in this parcel is insufficient,
         *         exception message: {@link ParcelException#NO_CAPACITY_ERROR}.
         * @since 7
         */
        writeByteArray(byteArray: number[]): boolean;

        /**
         * Writes a short integer array into the {@link MessageParcel} object.
         * @param shortArray Indicates the short integer array to write.
         * @return Returns {@code true} if the array has been written into the {@link MessageParcel};
         *         returns {@code false} otherwise.
         * @throws ParcelException When capacity in this parcel is insufficient,
         *         exception message: {@link ParcelException#NO_CAPACITY_ERROR}.
         * @Note   Ensure that the data type and size comply with the interface definition.
         *         Otherwise,data may be truncated.
         * @since 7
         */
        writeShortArray(shortArray: number[]): boolean;

        /**
         * Writes an integer array into the {@link MessageParcel} object.
         * @param intArray Indicates the integer array to write.
         * @return Returns {@code true} if the array has been written into the {@link MessageParcel};
         *         returns {@code false} otherwise.
         * @throws ParcelException When capacity in this parcel is insufficient,
         *         exception message: {@link ParcelException#NO_CAPACITY_ERROR}.
         * @Note   Ensure that the data type and size comply with the interface definition.
         *         Otherwise,data may be truncated.
         * @since 7
         */
        writeIntArray(intArray: number[]): boolean;

        /**
         * Writes a long integer array into the {@link MessageParcel} object.
         * @param longArray Indicates the long integer array to write.
         * @return Returns {@code true} if the array has been written into the {@link MessageParcel};
         *         returns {@code false} otherwise.
         * @throws ParcelException When capacity in this parcel is insufficient,
         *         exception message: {@link ParcelException#NO_CAPACITY_ERROR}.
         * @Note   Ensure that the data type and size comply with the interface definition.
         *         Otherwise,data may be truncated.
         * @since 7
         */
        writeLongArray(longArray: number[]): boolean;

        /**
         * Writes a floating point array into the {@link MessageParcel} object.
         * @param floatArray Indicates the floating point array to write.
         * @return Returns {@code true} if the array has been written into the {@link MessageParcel};
         *         returns {@code false} otherwise.
         * @throws ParcelException When capacity in this parcel is insufficient,
         *         exception message: {@link ParcelException#NO_CAPACITY_ERROR}.
         * @Note   Ensure that the data type and size comply with the interface definition.
         *         Otherwise,data may be truncated.
         * @since 7
         */
        writeFloatArray(floatArray: number[]): boolean;

        /**
         * Writes a double-precision floating point array into the {@link MessageParcel} object.
         * @param doubleArray Indicates the double-precision floating point array to write.
         * @return Returns {@code true} if the array has been written into the {@link MessageParcel};
         *         returns {@code false} otherwise.
         * @throws ParcelException When capacity in this parcel is insufficient,
         *         exception message: {@link ParcelException#NO_CAPACITY_ERROR}.
         * @Note   Ensure that the data type and size comply with the interface definition.
         *         Otherwise,data may be truncated.
         * @since 7
         */
        writeDoubleArray(doubleArray: number[]): boolean;

        /**
         * Writes a boolean array into the {@link MessageParcel} object.
         * @param booleanArray Indicates the boolean array to write.
         * @return Returns {@code true} if the array has been written into the {@link MessageParcel};
         *         returns {@code false} otherwise.
         * @throws ParcelException When capacity in this parcel is insufficient,
         *         exception message: {@link ParcelException#NO_CAPACITY_ERROR}.
         * @Note   Ensure that the data type and size comply with the interface definition.
         *         Otherwise,data may be truncated.
         * @since 7
         */
        writeBooleanArray(booleanArray: boolean[]): boolean;

        /**
         * Writes a single character array into the {@link MessageParcel} object.
         * @param charArray Indicates the single character array to write.
         * @return Returns {@code true} if the array has been written into the {@link MessageParcel};
         *         returns {@code false} otherwise.
         * @throws ParcelException When capacity in this parcel is insufficient,
         *         exception message: {@link ParcelException#NO_CAPACITY_ERROR}.
         * @Note   Ensure that the data type and size comply with the interface definition.
         *         Otherwise,data may be truncated.
         * @since 7
         */
        writeCharArray(charArray: number[]): boolean;

        /**
         * Writes a string array into the {@link MessageParcel} object.
         * @param stringArray Indicates the string array to write.
         * @return Returns {@code true} if the array has been written into the {@link MessageParcel};
         *         returns {@code false} otherwise.
         * @throws ParcelException When capacity in this parcel is insufficient,
         *         exception message: {@link ParcelException#NO_CAPACITY_ERROR}.
         * @Note   Ensure that the data type and size comply with the interface definition.
         *         Otherwise,data may be truncated.
         * @since 7
         */
        writeStringArray(stringArray: string[]): boolean;

        /**
         * Writes a {@link Sequenceable} object array into the {@link MessageParcel} object.
         * @param sequenceableArray Indicates the {@link Sequenceable} object array to write.
         * @return Returns {@code true} if the array has been written into the {@link MessageParcel};
         *         returns {@code false} otherwise.
         * @throws ParcelException When capacity in this parcel is insufficient,
         *         exception message: {@link ParcelException#NO_CAPACITY_ERROR}.
         * @since 7
         */
        writeSequenceableArray(sequenceableArray: Sequenceable[]): boolean;

        /**
         * Writes an array of {@link IRemoteObject} objects to this {@link MessageParcel} object.
         * @param objectArray Array of {@link IRemoteObject} objects to write.
         * @return Returns {@code true} if the {@link IRemoteObject} array is successfully written to the {@link MessageParcel};
         *         returns false if the {@link IRemoteObject} array is null or fails to be written to the {@lini MessageParcel}.
         * @since 8
         */
        writeRemoteObjectArray(objectArray: IRemoteObject[]): boolean;

        /**
         * Reads a byte value from the {@link MessageParcel} object.
         * @return Returns a byte value.
         * @since 7
         */
        readByte(): number;

        /**
         * Reads a short integer value from the {@link MessageParcel} object.
         * @return Returns a short integer value.
         * @since 7
         */
        readShort(): number;

        /**
         * Reads an integer value from the {@link MessageParcel} object.
         * @return Returns an integer value.
         * @since 7
         */
        readInt(): number;

        /**
         * Reads a long integer value from the {@link MessageParcel} object.
         * @return Returns a long integer value.
         * @since 7
         */
        readLong(): number;

        /**
         * Reads a floating point value from the {@link MessageParcel} object.
         * @return Returns a floating point value.
         * @since 7
         */
        readFloat(): number;

        /**
         * Reads a double-precision floating point value from the {@link MessageParcel} object.
         * @return Returns a double-precision floating point value.
         * @since 7
         */
        readDouble(): number;

        /**
         * Reads a boolean value from the {@link MessageParcel} object.
         * @return Returns a boolean value.
         * @since 7
         */
        readBoolean(): boolean;

        /**
         * Reads a single character value from the {@link MessageParcel} object.
         * @return Returns a single character value.
         * @since 7
         */
        readChar(): number;

        /**
         * Reads a string value from the {@link MessageParcel} object.
         * @return Returns a string value.
         * @since 7
         */
        readString(): string;

        /**
         * Reads a {@link Sequenceable} object from the {@link MessageParcel} instance.
         * @param dataIn Indicates the {@link Sequenceable} object that needs to perform the {@code unmarshalling} operation
         *        using the {@link MessageParcel}.
         * @return Returns {@code true} if the unmarshalling is successful; returns {@code false} otherwise.
         * @since 7
         */
        readSequenceable(dataIn: Sequenceable) : boolean;

        /**
         * Writes a byte array into the {@link MessageParcel} object.
         * @param dataIn Indicates the byte array read from MessageParcel.
         * @return Returns {@code true} if the array has been written into the {@link MessageParcel};
         *         returns {@code false} otherwise.
         * @throws ParcelException When capacity in this MessageParcel is insufficient,
         *         exception message: {@link *MessageParcelException#NO_CAPACITY_ERROR}.
         * @since 7
         */
        readByteArray(dataIn: number[]) : void;

        /**
         * Reads a byte array from the {@link MessageParcel} object.
         * @return Returns a byte array.
         * @since 7
         */
        readByteArray(): number[];

        /**
         * Reads a short integer array from the {@link MessageParcel} object.
         * @param dataIn Indicates the short integer array read from MessageParcel.
         * @since 7
         */
        readShortArray(dataIn: number[]) : void;

        /**
         * Reads a short integer array from the {@link MessageParcel} object.
         * @return Returns a short integer array.
         * @since 7
         */
        readShortArray(): number[];

        /**
         * Reads an integer array from the {@link MessageParcel} object.
         * @param dataIn Indicates the integer array to read.
         * @throws ParcelException Throws this exception if reading the integer array fails.
         * @since 7
         */
        readIntArray(dataIn: number[]) : void;

        /**
         * Reads an integer array from the {@link MessageParcel} object.
         * @return Returns an integer array.
         * @since 7
         */
        readIntArray(): number[];

        /**
         * Reads a long integer array from the {@link MessageParcel} object.
         * @param dataIn Indicates the long integer array to read.
         * @throws ParcelException Throws this exception if reading the long array fails.
         * @since 7
         */
        readLongArray(dataIn: number[]) : void;

        /**
         * Reads a long integer array from the {@link MessageParcel} object.
         * @return Returns a long integer array.
         * @since 7
         */
        readLongArray(): number[];

        /**
         * Reads a floating point array from the {@link MessageParcel} object.
         * @param dataIn Indicates the floating point array to read.
         * @throws ParcelException Throws this exception if reading the float array fails.
         * @since 7
         */
        readFloatArray(dataIn: number[]) : void;

        /**
         * Reads a floating point array from the {@link MessageParcel} object.
         * @return Returns a floating point array.
         * @since 7
         */
        readFloatArray(): number[];

        /**
         * Reads a double-precision floating point array from the {@link MessageParcel} object.
         * @param dataIn Indicates the double-precision floating point array to read.
         * @throws ParcelException Throws this exception if reading the double array fails.
         * @since 7
         */
        readDoubleArray(dataIn: number[]) : void;

        /**
         * Reads a double-precision floating point array from the {@link MessageParcel} object.
         * @return Returns a double-precision floating point array.
         * @since 7
         */
        readDoubleArray(): number[];

        /**
         * Reads a boolean array from the {@link MessageParcel} object.
         * @param dataIn Indicates the boolean array to read.
         * @throws ParcelException Throws this exception if reading the boolean array fails.
         * @since 7
         */
        readBooleanArray(dataIn: boolean[]) : void;

        /**
         * Reads a boolean array from the {@link MessageParcel} object.
         * @return Returns a boolean array.
         * @since 7
         */
        readBooleanArray(): boolean[];

        /**
         * Reads a single character array from the {@link MessageParcel} object.
         * @param dataIn Indicates the single character array to read.
         * @throws ParcelException Throws this exception if reading the char array fails.
         * @since 7
         */
        readCharArray(dataIn: number[]) : void;

        /**
         * Reads a single character array from the {@link MessageParcel} object.
         * @return Returns a single character array.
         * @since 7
         */
        readCharArray(): number[];

        /**
         * Reads a string array from the {@link MessageParcel} object.
         * @param dataIn Indicates the string array to read.
         * @throws ParcelException Throws this exception if reading the string array fails.
         * @since 7
         */
        readStringArray(dataIn: string[]) : void;

        /**
         * Reads a string array from the {@link MessageParcel} object.
         * @return Returns a string array.
         * @since 7
         */
        readStringArray(): string[];

        /**
         * Reads the specified {@link Sequenceable} array from this {@link MessageParcel} object.
         * @param sequenceableArray Sequenceable array to read.
         * @since 8
         */
        readSequenceableArray(sequenceableArray: Sequenceable[]): void;

        /**
         * Reads the specified {@link IRemoteObject} array from this {@link MessageParcel} object.
         * @param objects Reads data from this {@link MessageParcel} object to the specified {@link IRemoteObject} array.
         * @since 8
         */
        readRemoteObjectArray(objects: IRemoteObject[]): void;

        /**
         * Reads {@link IRemoteObject} objects from this {@link MessageParcel} object.
         * @return An array of {@link IRemoteObject} objects obtained.
         * @since 8
         */
        readRemoteObjectArray(): IRemoteObject[];

        /**
         * Closes the specified file descriptor.
         * @param fd File descriptor to be closed.
         * @since 8
         */
        static closeFileDescriptor(fd: number): void;

        /**
         * Duplicates the specified file descriptor.
         * @param fd File descriptor to be duplicated.
         * @return A duplicated file descriptor.
         * @since 8
         */
        static dupFileDescriptor(fd: number) :number;

        /**
         * Checks whether this {@link MessageParcel} object contains a file descriptor.
         * @return Returns true if the {@link MessageParcel} object contains a file descriptor;
         * returns false otherwise.
         * @since 8
         */
        containFileDescriptors(): boolean;

        /**
         * Writes a file descriptor to this {@link MessageParcel} object.
         * @param fd File descriptor to wrote.
         * @return Returns true if the operation is successful; returns false otherwise.
         * @since 8
         */
        writeFileDescriptor(fd: number): boolean;

        /**
         * Reads a file descriptor from this {@link MessageParcel} object.
         * @return File descriptor obtained.
         * @since 8
         */
        readFileDescriptor(): number;

        /**
         * Writes an anonymous shared memory object to this {@link MessageParcel} object.
         * @param ashmem Anonymous shared memory object to wrote.
         * @return Returns true if the operation is successful; returns false otherwise.
         * @since 8
         */
        writeAshmem(ashmem: Ashmem): boolean;

        /**
         * Reads the anonymous shared memory object from this {@link MessageParcel} object.
         * @return Anonymous share object obtained.
         * @since 8
         */
        readAshmem(): Ashmem;

        /**
         * Obtains the maximum amount of raw data that can be sent in a time.
         * @return 128 MB.
         * @since 8
         */
        getRawDataCapacity(): number;

        /**
         * Writes raw data to this {@link MessageParcel} object.
         * @param rawData Raw data to wrote.
         * @param size Size of the raw data, in bytes.
         * @return Returns true if the operation is successful; returns false otherwise.
         * @since 8
         */
        writeRawData(rawData: number[], size: number): boolean;

        /**
         * Reads raw data from this {@link MessageParcel} object.
         * @param size Size of the raw data to read.
         * @return Raw data obtained, in bytes.
         * @since 8
         */
        readRawData(size: number): number[];
    }


    /** 
     * @syscap SystemCapability.Communication.IPC.Core
     * @import import rpc from '@ohos.rpc'
     * @since 7
     */
    interface Sequenceable {
        /**
         * Marshals this {@code Sequenceable} object into a {@link MessageParcel}.
         *
         * @param dataOut Indicates the {@link MessageParcel} object to which the {@code Sequenceable}
         *        object will be marshaled..
         * @return Returns {@code true} if the marshalling is successful; returns {@code false} otherwise.
         * @throws ParcelException Throws this exception if the operation fails.
         * @since 7
         */
        marshalling(dataOut: MessageParcel): boolean;

        /**
         * Unmarshals this {@code Sequenceable} object from a {@link MessageParcel}.
         *
         * @param dataIn Indicates the {@link MessageParcel} object into which the {@code Sequenceable}
         *        object has been marshaled.
         * @return Returns {@code true} if the unmarshalling is successful; returns {@code false} otherwise.
         * @throws ParcelException Throws this exception if the operation fails.
         * @since 7
         */
        unmarshalling(dataIn: MessageParcel) : boolean;
    }

    /**
     * Defines the response to the request.
     * <p> SendRequestResult object contains four members,
     * namely error code of this operation, request code, data parcel
     * and reply parcel.
     * @syscap SystemCapability.Communication.IPC.Core
     * @import import rpc from '@ohos.rpc'
     * @since 8
     */
    interface SendRequestResult {
        /**
         * Error code. 0 indicates successful, otherwise it is failed.
         * @since 8
         */
        errCode: number;

        /**
         * Message code. It is same as the code in {@link SendRequest} method.
         * @since 8
         */
        code: number;

        /**
         * MessageParcel object sent to the peer process.
         * It is the same object in {@link SendRequest} method.
         * @since 8
         */
        data: MessageParcel;

        /**
         * MessageParcel object returned by the peer process.
         * It is the same object in {@link SendRequest} method.
         * @since 8
         */
        reply: MessageParcel;
    }

    /** 
     * @syscap SystemCapability.Communication.IPC.Core
     * @import import rpc from '@ohos.rpc'
     * @since 7
     */
    interface IRemoteObject {
        /**
         * Queries the description of an interface.
         *
         * <p>A valid {@link IRemoteBroker} object is returned for an interface used by the service provider;
         * {@code null} is returned for an interface used by the service user,
         * indicating that the interface is not a local one.
         *
         * @param descriptor Indicates the interface descriptor.
         * @return Returns the {@link IRemoteBroker} object bound to the specified interface descriptor.
         * @since 7
         */
        queryLocalInterface(descriptor: string): IRemoteBroker;

        /**
         * Sends a {@link MessageParcel} message to the peer process in synchronous or asynchronous mode.
         *
         * <p>If asynchronous mode is set for {@code option}, a response is returned immediately.
         * If synchronous mode is set for {@code option}, the interface will wait for a response from the peer process
         * until the request times out. The user must prepare {@code reply} for receiving the execution result
         * given by the peer process.
         *
         * @param code Indicates the message code, which is determined by both sides of the communication.
         * If the interface is generated by the IDL tool, the message code is automatically generated by IDL.
         * @param data Indicates the {@link MessageParcel} object sent to the peer process.
         * @param reply Indicates the {@link MessageParcel} object returned by the peer process.
         * @param options Indicates the synchronous or asynchronous mode to send messages.
         * @return Returns {@code true} if the method is called successfully; returns {@code false} otherwise.
         * @throws RemoteException Throws this exception if the method fails to be called.
         * @deprecated since 8
         * @since 7
         */
        sendRequest(code: number, data: MessageParcel, reply: MessageParcel, options: MessageOption): boolean;

        /**
         * Sends a {@link MessageParcel} message to the peer process in synchronous or asynchronous mode.
         *
         * <p>If options indicates the asynchronous mode, a promise will be fulfilled immediately
         * and the reply message does not contain any content. If options indicates the synchronous mode,
         * a promise will be fulfilled when the response to sendRequest is returned,
         * and the reply message contains the returned information.
         * param code Message code called by the request, which is determined by the client and server.
         * If the method is generated by an IDL tool, the message code is automatically generated by the IDL tool.
         * param data {@link MessageParcel} object holding the data to send.
         * param reply {@link MessageParcel} object that receives the response.
         * param operations Indicates the synchronous or asynchronous mode to send messages.
         * @returns Promise used to return the {@link SendRequestResult} instance.
         * @throws Throws an exception if the method fails to be called.
         * @since 8
         */
        sendRequest(code: number, data: MessageParcel, reply: MessageParcel, options: MessageOption): Promise<SendRequestResult>;

        /**
         * Sends a {@link MessageParcel} message to the peer process in synchronous or asynchronous mode.
         *
         * <p>If options indicates the asynchronous mode, a callback will be invoked immediately
         * and the reply message does not contain any content. If options indicates the synchronous mode,
         * a callback will be invoked when the response to sendRequest is returned,
         * and the reply message contains the returned information.
         * param code Message code called by the request, which is determined by the client and server.
         * If the method is generated by an IDL tool, the message code is automatically generated by the IDL tool.
         * param data {@link MessageParcel} object holding the data to send.
         * param reply {@link MessageParcel} object that receives the response.
         * param operations Indicates the synchronous or asynchronous mode to send messages.
         * param callback Callback for receiving the sending result.
         * @since 8
         */
        sendRequest(code: number, data: MessageParcel, reply: MessageParcel, options: MessageOption, callback: AsyncCallback<SendRequestResult>): void;

        /**
         * Registers a callback used to receive notifications of the death of a remote object.
         *
         * @param recipient Indicates the callback to be registered.
         * @param flags Indicates the flag of the death notification.
         * @return Returns {@code true} if the callback is registered successfully; returns {@code false} otherwise.
         * @since 7
         */
        addDeathRecipient(recipient: DeathRecipient, flags: number): boolean;

        /**
         * Deregisters a callback used to receive notifications of the death of a remote object.
         *
         * @param recipient Indicates the callback to be deregistered.
         * @param flags Indicates the flag of the death notification.
         * @return Returns {@code true} if the callback is deregistered successfully; returns {@code false} otherwise.
         * @since 7
         */
        removeDeathRecipient(recipient: DeathRecipient, flags: number): boolean;

        /**
         * Obtains the interface descriptor of an object.
         *
         * <p>The interface descriptor is a character string.
         *
         * @return Returns the interface descriptor.
         * @since 7
         */
        getInterfaceDescriptor(): string;

        /**
         * Checks whether an object is dead.
         *
         * @return Returns {@code true} if the object is dead; returns {@code false} otherwise.
         * @since 7
         */
        isObjectDead(): boolean;
    }

    /** 
     * @syscap SystemCapability.Communication.IPC.Core
     * @import import rpc from '@ohos.rpc'
     * @since 7
     */
    interface IRemoteBroker {
        /**
         * Obtains a proxy or remote object. This method must be implemented by its derived classes.
         *
         * @return Returns the RemoteObject if the caller is a RemoteObject; returns the IRemoteObject,
         * that is, the holder of this RemoteProxy object, if the caller is a RemoteProxy object.
         * @since 7
         */
        asObject(): IRemoteObject;
    }

    /** 
     * @since 7
     * @syscap SystemCapability.Communication.IPC.Core
     * @import import rpc from '@ohos.rpc'
     */
    interface DeathRecipient {
        /**
         * Called to perform subsequent operations when a death notification of the remote object is received.
         *
         * @since 7
         */
        onRemoteDied(): void;
    }

    /** 
     * @syscap SystemCapability.Communication.IPC.Core
     * @import import rpc from '@ohos.rpc'
     * @since 7
     */
    class MessageOption {
        /**
         * Indicates synchronous call.
         * @since 7
         */
        TF_SYNC = 0;

        /**
         * Indicates asynchronous call.
         * @since 7
         */
        TF_ASYNC = 1;

        /**
         * Indicates the sendRequest API for returning the file descriptor.
         * @since 7
         */
        TF_ACCEPT_FDS = 0x10;

        /**
         * Indicates the wait time for RPC, in seconds. It is NOT used in IPC case.
         * @since 7
         */
        TF_WAIT_TIME  = 8;

        /**
         * A constructor used to create a MessageOption instance.
         *
         * @param syncFlags Specifies whether the SendRequest is called synchronously (default) or asynchronously.
         * @param waitTime Maximum wait time for a RPC call. The default value is TF_WAIT_TIME.
         * @since 7
         */
        constructor(syncFlags?: number, waitTime = TF_WAIT_TIME);

        /**
         * Obtains the SendRequest call flag, which can be synchronous or asynchronous.
         *
         * @return Returns whether the SendRequest is called synchronously or asynchronously.
         * @since 7
         */
        getFlags(): number;

        /**
         * Sets the SendRequest call flag, which can be synchronous or asynchronous.
         *
         * @param flags Indicates the call flag, which can be synchronous or asynchronous.
         * @since 7
         */
        setFlags(flags: number): void;

        /**
         * Obtains the maximum wait time for this RPC call.
         *
         * @return Returns maximum wait time obtained.
         * @since 7
         */
        getWaitTime(): number;

        /**
         * Sets the maximum wait time for this RPC call.
         *
         * @param waitTime Indicates maximum wait time to set.
         * @since 7
         */
        setWaitTime(waitTime: number): void;
    }

    /** 
     * @syscap SystemCapability.Communication.IPC.Core
     * @import import rpc from '@ohos.rpc'
     * @since 7
     */
    class RemoteObject implements IRemoteObject {
        /**
         * A constructor to create a RemoteObject instance.
         *
         * @param descriptor Specifies interface descriptor.
         * @since 7
         */
        constructor(descriptor: string);

        /**
         * Queries a remote object using an interface descriptor.
         *
         * @param descriptor Indicates the interface descriptor used to query the remote object.
         * @return Returns the remote object matching the interface descriptor; returns null
         * if no such remote object is found.
         * @since 7
         */
        queryLocalInterface(descriptor: string): IRemoteBroker;

        /**
         * Queries an interface descriptor.
         *
         * @return Returns the interface descriptor.
         * @since 7
         */
        getInterfaceDescriptor(): string;

        /**
         * Sets an entry for receiving requests.
         *
         * <p>This method is implemented by the remote service provider. You need to override this method with
         * your own service logic when you are using IPC.
         *
         * @param code Indicates the service request code sent from the peer end.
         * @param data Indicates the {@link MessageParcel} object sent from the peer end.
         * @param reply Indicates the response message object sent from the remote service.
         * The local service writes the response data to the {@link MessageParcel} object.
         * @param options Indicates whether the operation is synchronous or asynchronous.
         * @return Returns {@code true} if the operation succeeds; returns {@code false} otherwise.
         * @throws RemoteException Throws this exception if a remote service error occurs.
         * @since 7
         */
        onRemoteRequest(code: number, data: MessageParcel, reply: MessageParcel, options: MessageOption): boolean;

        /**
         * Sends a request to the peer object.
         *
         * <p>If the peer object and {@code RemoteObject} are on the same device, the request is sent by the IPC driver.
         * If they are on different devices, the request is sent by the socket driver.
         *
         * @param code Indicates the message code of the request.
         * @param data Indicates the {@link MessageParcel} object storing the data to be sent.
         * @param reply Indicates the {@link MessageParcel} object receiving the response data.
         * @param options Indicates a synchronous (default) or asynchronous request.
         * @return Returns {@code true} if the operation succeeds; returns {@code false} otherwise.
         * @deprecated since 8
         * @since 7
         */
        sendRequest(code: number, data: MessageParcel, reply: MessageParcel, options: MessageOption): boolean;

        /**
         * Sends a {@link MessageParcel} message to the peer process in synchronous or asynchronous mode.
         *
         * <p>If options indicates the asynchronous mode, a promise will be fulfilled immediately
         * and the reply message does not contain any content. If options indicates the synchronous mode,
         * a promise will be fulfilled when the response to sendRequest is returned,
         * and the reply message contains the returned information.
         * param code Message code called by the request, which is determined by the client and server.
         * If the method is generated by an IDL tool, the message code is automatically generated by the IDL tool.
         * param data {@link MessageParcel} object holding the data to send.
         * param reply {@link MessageParcel} object that receives the response.
         * param operations Indicates the synchronous or asynchronous mode to send messages.
         * @returns Promise used to return the {@link SendRequestResult} instance.
         * @throws Throws an exception if the method fails to be called.
         * @since 8
         */
        sendRequest(code: number, data: MessageParcel, reply: MessageParcel, options: MessageOption): Promise<SendRequestResult>;

        /**
         * Sends a {@link MessageParcel} message to the peer process in synchronous or asynchronous mode.
         *
         * <p>If options indicates the asynchronous mode, a callback will be invoked immediately
         * and the reply message does not contain any content. If options indicates the synchronous mode,
         * a callback will be invoked when the response to sendRequest is returned,
         * and the reply message contains the returned information.
         * param code Message code called by the request, which is determined by the client and server.
         * If the method is generated by an IDL tool, the message code is automatically generated by the IDL tool.
         * param data {@link MessageParcel} object holding the data to send.
         * param reply {@link MessageParcel} object that receives the response.
         * param operations Indicates the synchronous or asynchronous mode to send messages.
         * param callback Callback for receiving the sending result.
         * @since 8
         */
        sendRequest(code: number, data: MessageParcel, reply: MessageParcel, options: MessageOption, callback: AsyncCallback<SendRequestResult>): void;

        /**
         * Obtains the PID of the {@link RemoteProxy} object.
         *
         * @return Returns the PID of the {@link RemoteProxy} object.
         * @since 7
         */
        getCallingPid(): number;

        /**
         * Obtains the UID of the {@link RemoteProxy} object.
         *
         * @return Returns the UID of the {@link RemoteProxy} object.
         * @since 7
         */
        getCallingUid(): number;

        /**
         * Modifies the description of the current {@code RemoteObject}.
         *
         * <p>This method is used to change the default descriptor specified during the creation of {@code RemoteObject}.
         *
         * @param localInterface Indicates the {@code RemoteObject} whose descriptor is to be changed.
         * @param descriptor Indicates the new descriptor of the {@code RemoteObject}.
         * @since 7
         */
        attachLocalInterface(localInterface: IRemoteBroker, descriptor: string): void;
    }

    /** 
     * @syscap SystemCapability.Communication.IPC.Core
     * @import import rpc from '@ohos.rpc'
     * @since 7
     */
    class RemoteProxy implements IRemoteObject {
        /**
         * Indicates the message code for a Ping operation.
         * @since 7
         */
        PING_TRANSACTION = ('_' << 24) | ('P' << 16) | ('N' << 8) | 'G';

        /**
         * Indicates the message code for a dump operation.
         * @since 7
         */
        DUMP_TRANSACTION = ('_' << 24) | ('D' << 16) | ('M' << 8) | 'P';

        /**
         * Indicates the message code for a transmission.
         * @since 7
         */
        INTERFACE_TRANSACTION = ('_' << 24) | ('N' << 16) | ('T' << 8) | 'F';

        /**
         * Indicates the minimum value of a valid message code.
         *
         * <p>This constant is used to check the validity of an operation.
         * @since 7
         */
        MIN_TRANSACTION_ID = 0x1;

        /**
         * Indicates the maximum value of a valid message code.
         *
         * <p>This constant is used to check the validity of an operation.
         * @since 7
         */
        MAX_TRANSACTION_ID = 0x00FFFFFF;

        /**
         * Queries a local interface with a specified descriptor.
         *
         * @param descriptor Indicates the descriptor of the interface to query.
         * @return Returns null by default, indicating a proxy interface.
         * @since 7
         */
        queryLocalInterface(interface: string): IRemoteBroker;

        /**
         * Registers a callback used to receive death notifications of a remote object.
         *
         * @param recipient Indicates the callback to be registered.
         * @param flags Indicates the flag of the death notification. This is a reserved parameter. Set it to {@code 0}.
         * @return Returns {@code true} if the callback is registered successfully; returns {@code false} otherwise.
         * @since 7
         */
        addDeathRecipient(recipient: DeathRecipient, flags: number): boolean;

        /**
         * Deregisters a callback used to receive death notifications of a remote object.
         *
         * @param recipient Indicates the callback to be deregistered.
         * @param flags Indicates the flag of the death notification. This is a reserved parameter. Set it to {@code 0}.
         * @return Returns {@code true} if the callback is deregistered successfully; returns {@code false} otherwise.
         * @since 7
         */
        removeDeathRecipient(recipient: DeathRecipient, flags: number): boolean;

        /**
         * Queries the interface descriptor of remote object.
         *
         * @return Returns the interface descriptor.
         * @since 7
         */
        getInterfaceDescriptor(): string;

        /**
         * Sends a request to the peer object.
         *
         * <p>If the peer object and {@code RemoteProxy} are on the same device, the request is sent by the IPC driver.
         * If they are on different devices, the request is sent by the socket driver.
         *
         * @param code Indicates the message code of the request.
         * @param data Indicates the {@link MessageParcel} object storing the data to be sent.
         * @param reply Indicates the {@link MessageParcel} object receiving the response data.
         * @param options Indicates a synchronous (default) or asynchronous request.
         * @return Returns {@code true} if the operation succeeds; returns {@code false} otherwise.
         * @throws RemoteException Throws this exception if a remote object exception occurs.
         * @deprecated since 8
         * @since 7
         */
        sendRequest(code: number, data: MessageParcel, reply: MessageParcel, options: MessageOption): boolean;

        /**
         * Sends a {@link MessageParcel} message to the peer process in synchronous or asynchronous mode.
         *
         * <p>If options indicates the asynchronous mode, a promise will be fulfilled immediately
         * and the reply message does not contain any content. If options indicates the synchronous mode,
         * a promise will be fulfilled when the response to sendRequest is returned,
         * and the reply message contains the returned information.
         * param code Message code called by the request, which is determined by the client and server.
         * If the method is generated by an IDL tool, the message code is automatically generated by the IDL tool.
         * param data {@link MessageParcel} object holding the data to send.
         * param reply {@link MessageParcel} object that receives the response.
         * param operations Indicates the synchronous or asynchronous mode to send messages.
         * @returns Promise used to return the {@link sendRequestResult} instance.
         * @throws Throws an exception if the method fails to be called.
         * @since 8
         */
        sendRequest(code: number, data: MessageParcel, reply: MessageParcel, options: MessageOption): Promise<SendRequestResult>;

        /**
         * Sends a {@link MessageParcel} message to the peer process in synchronous or asynchronous mode.
         *
         * <p>If options indicates the asynchronous mode, a callback will be invoked immediately
         * and the reply message does not contain any content. If options indicates the synchronous mode,
         * a callback will be invoked when the response to sendRequest is returned,
         * and the reply message contains the returned information.
         * param code Message code called by the request, which is determined by the client and server.
         * If the method is generated by an IDL tool, the message code is automatically generated by the IDL tool.
         * param data {@link MessageParcel} object holding the data to send.
         * param reply {@link MessageParcel} object that receives the response.
         * param operations Indicates the synchronous or asynchronous mode to send messages.
         * param callback Callback for receiving the sending result.
         * @since 8
         */
        sendRequest(code: number, data: MessageParcel, reply: MessageParcel, options: MessageOption, callback: AsyncCallback<SendRequestResult>): void;

        /**
         * Checks whether the {@code RemoteObject} corresponding to a {@code RemoteProxy} is dead.
         *
         * @return Returns {@code true} if the {@code RemoteObject} is dead; returns {@code false} otherwise.
         * @since 7
         */
        isObjectDead(): boolean;
    }

    /** 
     * @syscap SystemCapability.Communication.IPC.Core
     * @import import rpc from '@ohos.rpc'
     * @since 7
     */
    class IPCSkeleton {
        /**
         * Obtains a local {@link IRemoteObject} reference of a registered service.
         *
         * <p>This method is static.
         *
         * @return Returns an {@link IRemoteObject} reference of the registered service.
         * @since 7
         */
        static getContextObject(): IRemoteObject;

        /**
         * Obtains the PID of a proxy.
         *
         * <p>This method is static. The PID is a positive integer during the communication between
         * the {@link RemoteProxy} object and {@link RemoteObject} object, and resumes to {@code 0}
         * when the communication ends. If this method is called from the {@link RemoteProxy} object,
         * {@code 0} is returned; if this method is called from the {@link RemoteObject} object,
         * the PID of the corresponding {@link RemoteProxy} object is returned.
         *
         * @return Returns the PID of the proxy.
         * @since 7
         */
        static getCallingPid(): number;

        /**
         * Obtains the UID of a proxy.
         *
         * <p>This method is static. The UID is a positive integer during the communication between
         * the {@link RemoteProxy} object and {@link RemoteObject} object, and resumes to {@code 0}
         * when the communication ends. If this method is called from the {@link RemoteProxy} object,
         * {@code 0} is returned; if this method is called from the {@link RemoteObject} object,
         * the UID of the corresponding {@link RemoteProxy} object is returned.
         *
         * @return Returns the UID of the proxy.
         * @since 7
         */
        static getCallingUid(): number;

        /**
         * Obtains the TOKENID.
         *
         * <p>This method is static.
         *
         * @return Returns the TOKENID.
         * @since 8
         */
         static getCallingTokenId(): number;

        /**
         * Obtains the ID of the device where the peer process resides.
         *
         * <p>This method is static.
         *
         * @return Returns the ID of the device where the peer process resides.
         * @since 7
         */
        static getCallingDeviceID(): string;

        /**
         * Obtains the ID of the local device.
         *
         * <p>This method is static.
         *
         * @return Returns the ID of the local device.
         * @since 7
         */
        static getLocalDeviceID(): string;

        /**
         * Checks whether a call is made on the same device.
         *
         * <p>This method is static.
         *
         * @return Returns {@code true} if the call is made on the same device; returns {@code false} otherwise.
         * @since 7
         */
        static isLocalCalling(): boolean;

        /**
         * Flushes all pending commands from a specified {@link RemoteProxy} to the corresponding {@link RemoteObject}.
         *
         * <p>This method is static. You are advised to call this method before performing any time-sensitive operations.
         *
         * @param object Indicates the specified {@link RemoteProxy}.
         * @return Returns {@code 0} if the operation succeeds; returns an error code if the input object is empty
         * or {@link RemoteObject}, or the operation fails.
         * @since 7
         */
        static flushCommands(object: IRemoteObject): number;

        /**
         * Replaces the UID and PID of the remote user with those of the local user.
         *
         * <p>This method is static. It can be used in scenarios like authentication.
         *
         * @return Returns a string containing the UID and PID of the remote user.
         * @since 7
         */
        static resetCallingIdentity(): string;

        /**
         * Restores the UID and PID to those of the remote user.
         *
         * <p>This method is static. It is usually called after {@code resetCallingIdentity} is used
         * and requires the UID and PID of the remote user returned by {@code resetCallingIdentity}.
         *
         * @param identity Indicates the string containing the UID and PID of the remote user,
         * which is returned by {@code resetCallingIdentity}.
         * @return Returns {@code true} if the operation succeeds; returns {@code false} otherwise.
         * @since 7
         */
        static setCallingIdentity(identity: string): boolean;
    }

    /**
     * Provides methods related to anonymous shared memory objects,
     * including creating, closing, mapping, and unmapping an Ashmem object,
     * reading data from and writing data to an Ashmem object,
     * obtaining the Ashmem size, and setting Ashmem protection.
     * @syscap SystemCapability.Communication.IPC.Core
     * @import import rpc from '@ohos.rpc'
     * @since 8
     */
    class Ashmem {
        /**
         * The mapped memory is executable.
         * @since 8
         */
        PROT_EXEC = 4;

        /**
         * The mapped memory is inaccessible.
         * @since 8
         */
        PROT_NONE = 0;

        /**
         * The mapped memory is readable.
         * @since 8
         */
        PROT_READ = 1;

        /**
         * The mapped memory is writable.
         * @since 8
         */
        PROT_WRITE = 2;

        /**
         * Creates an Ashmem object with the specified name and size.
         * @param name Name of the Ashmem object to create.
         * @param size Size (in bytes) of the Ashmem object to create.
         * @return Returns the Ashmem object if it is created successfully; returns null otherwise.
         * @since 8
         */
        static createAshmem(name: string, size: number): Ashmem;

        /**
         * Creates an Ashmem object by copying the file descriptor (FD) of an existing Ashmem object.
         * The two Ashmem objects point to the same shared memory region.
         * @param ashmem Existing Ashmem object.
         * @return Ashmem object created.
         * @since 8
         */
        static createAshmemFromExisting(ashmem: Ashmem): Ashmem;

        /**
         * Closes this Ashmem object.
         * @since 8
         */
        closeAshmem(): void;

        /**
         * Deletes the mappings for the specified address range of this Ashmem object.
         * @since 8
         */
        unmapAshmem(): void;

        /**
         * Obtains the mapped memory size of this Ashmem object.
         * @return Memory size mapped.
         * @since 8
         */
        getAshmemSize(): number;

        /**
         * Creates the shared file mapping on the virtual address space of this process.
         * The size of the mapping region is specified by this Ashmem object.
         * @param mapType Protection level of the memory region to which the shared file is mapped.
         * @return Returns true if the operation is successful; returns false otherwise.
         * @since 8
         */
        mapAshmem(mapType: number): boolean;

        /**
         * Maps the shared file to the readable and writable virtual address space of the process.
         * @return Returns true if the operation is successful; returns false otherwise.
         * @since 8
         */
        mapReadAndWriteAshmem(): boolean;

        /**
         * Maps the shared file to the read-only virtual address space of the process.
         * @return Returns true if the operation is successful; returns false otherwise.
         * @since 8
         */
        mapReadOnlyAshmem(): boolean;

        /**
         * Sets the protection level of the memory region to which the shared file is mapped.
         * @param protectionType Protection type to set.
         * @return Returns true if the operation is successful; returns false otherwise.
         * @since 8
         */
        setProtection(protectionType: number): boolean;

        /**
         * Writes data to the shared file associated with this Ashmem object.
         * @param buf Data to write
         * @param size Size of the data to write
         * @param offset Start position of the data to write in the memory region associated with this Ashmem object.
         * @return Returns true is the data is written successfully; returns false otherwise.
         * @since 8
         */
        writeToAshmem(buf: number[], size: number, offset: number): boolean;

        /**
         * Reads data from the shared file associated with this Ashmem object.
         * @param size Size of the data to read.
         * @param offset Start position of the data to read in the memory region associated with this Ashmem object.
         * @return Data read.
         * @since 8
         */
        readFromAshmem(size: number, offset: number): number[];
    }
}

export default rpc;
