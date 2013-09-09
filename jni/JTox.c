/* JTox.c
 *
 *  Copyright (C) 2013 Tox project All Rights Reserved.
 *
 *  This file is part of jToxcore
 *
 *  jToxcore is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  jToxcore is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with jToxcore.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <tox/tox.h>

#include "im_tox_jtoxcore_JTox.h"
#include "callbacks.h"

#define ADDR_SIZE_HEX (TOX_FRIEND_ADDRESS_SIZE * 2 + 1)

/**
 * Begin Utilities section
 */

/**
 * Convert a given binary address to a human-readable, \0-terminated hexadecimal string
 */
void addr_to_hex(uint8_t *addr, char *buf) {
	int i;

	for (i = 0; i < TOX_FRIEND_ADDRESS_SIZE; i++) {
		char xx[3];
		snprintf(xx, sizeof(xx), "%02X", addr[i] & 0xff);
		strcat(buf, xx);
	}
}

/**
 * Convert a given human-readable hexadecimal string into binary
 */
void hex_to_addr(const char *hex, uint8_t *buf) {
	size_t len = strlen(hex);
	uint8_t *val = malloc(len);
	char *pos = malloc(strlen(hex) + 1);
	pos = strdup(hex);
	int i;

	for (i = 0; i < len; ++i, pos += 2)
		sscanf(pos, "%2hhx", &buf[i]);
	free(pos);
}

/**
 * Null-terminate the given string. Length is the length of the original string,
 * out must be allocated with a size of at least length+1
 */
void nullterminate(uint8_t *in, uint16_t length, char *out) {
	memcpy(out, in, length);
	out[length] = '\0';
}

/**
 * End Utilities section
 */

/**
 * Begin maintenance section
 */
JNIEXPORT jlong JNICALL Java_im_tox_jtoxcore_JTox_tox_1new(JNIEnv * env,
		jclass clazz) {
	tox_jni_globals_t *globals = malloc(sizeof(tox_jni_globals_t));
	globals->tox = tox_new();
	globals->frqc = 0;
	globals->frmc = 0;
	globals->ac = 0;
	globals->nc = 0;
	globals->smc = 0;
	globals->usc = 0;
	globals->rrc = 0;
	globals->csc = 0;
	return ((jlong) globals);
}

JNIEXPORT void JNICALL Java_im_tox_jtoxcore_JTox_tox_1bootstrap(JNIEnv * env,
		jobject obj, jlong messenger, jbyteArray ip, jint port, jstring address) {
	tox_IP_Port ipport;
	tox_IP _ip;

	jbyte *ip_array = (*env)->GetByteArrayElements(env, ip, 0);
	jsize n = (*env)->GetArrayLength(env, ip);
	int i;

	for (i = 0; i < n; ++i) {
		_ip.c[i] = ip_array[i];
	}

	(*env)->ReleaseByteArrayElements(env, ip, ip_array, 0);
	ipport.ip = _ip;
	ipport.port = htons((uint16_t) port);

	const char *_address = (*env)->GetStringUTFChars(env, address, 0);
	uint8_t __address[TOX_FRIEND_ADDRESS_SIZE];
	hex_to_addr(_address, __address);

	tox_bootstrap(((tox_jni_globals_t *) messenger)->tox, ipport, __address);

	(*env)->ReleaseStringUTFChars(env, address, _address);
}

JNIEXPORT void JNICALL Java_im_tox_jtoxcore_JTox_tox_1do(JNIEnv * env,
		jobject obj, jlong messenger) {
	tox_do(((tox_jni_globals_t *) messenger)->tox);
}

JNIEXPORT jint JNICALL Java_im_tox_jtoxcore_JTox_tox_1isconnected(JNIEnv * env,
		jobject obj, jlong messenger) {
	return tox_isconnected(((tox_jni_globals_t *) messenger)->tox);
}

JNIEXPORT void JNICALL Java_im_tox_jtoxcore_JTox_tox_1kill(JNIEnv * env,
		jobject jobj, jlong messenger) {
	tox_jni_globals_t *globals = (tox_jni_globals_t *) messenger;

	if (globals->frqc) {
		if (globals->frqc->jobj) {
			(*env)->DeleteGlobalRef(env, globals->frqc->jobj);
		}
		free(globals->frqc);
	}

	if (globals->frmc) {
		if (globals->frmc->jobj) {
			(*env)->DeleteGlobalRef(env, globals->frmc->jobj);
		}
		free(globals->frmc);
	}

	if (globals->ac) {
		if (globals->ac->jobj) {
			(*env)->DeleteGlobalRef(env, globals->ac->jobj);
		}
		free(globals->ac);
	}

	if (globals->nc) {
		if (globals->nc->jobj) {
			(*env)->DeleteGlobalRef(env, globals->nc->jobj);
		}
		free(globals->nc);
	}

	if (globals->smc) {
		if (globals->smc->jobj) {
			(*env)->DeleteGlobalRef(env, globals->smc->jobj);
		}
		free(globals->smc);
	}

	if (globals->usc) {
		if (globals->usc->jobj) {
			(*env)->DeleteGlobalRef(env, globals->usc->jobj);
		}
		free(globals->usc);
	}

	if (globals->rrc) {
		if (globals->rrc->jobj) {
			(*env)->DeleteGlobalRef(env, globals->rrc->jobj);
		}
		free(globals->rrc);
	}

	if (globals->csc) {
		if (globals->csc->jobj) {
			(*env)->DeleteGlobalRef(env, globals->csc->jobj);
		}
		free(globals->csc);
	}

	tox_kill(globals->tox);

	free(globals);
}

/**
 * End maintenance section
 */

/**
 * Begin general section
 */

JNIEXPORT jint JNICALL Java_im_tox_jtoxcore_JTox_tox_1addfriend(JNIEnv * env,
		jobject obj, jlong messenger, jstring address, jstring data) {
	const uint8_t *_address = (*env)->GetStringUTFChars(env, address, 0);
	const uint8_t *_data = (*env)->GetStringUTFChars(env, data, 0);

	uint8_t __address[TOX_FRIEND_ADDRESS_SIZE];
	hex_to_addr(_address, __address);
	uint8_t *__data = strdup(_data);

	(*env)->ReleaseStringUTFChars(env, address, _address);
	(*env)->ReleaseStringUTFChars(env, data, _data);

	int ret = tox_addfriend(((tox_jni_globals_t *) messenger)->tox, __address,
			__data, strlen(__data) + 1);

	free(__data);
	return ret;
}

JNIEXPORT jint JNICALL Java_im_tox_jtoxcore_JTox_tox_1addfriend_1norequest(
		JNIEnv * env, jobject obj, jlong messenger, jstring address) {
	const char *_address = (*env)->GetStringUTFChars(env, address, 0);
	uint8_t __address[TOX_FRIEND_ADDRESS_SIZE];
	hex_to_addr(_address, __address);
	(*env)->ReleaseStringUTFChars(env, address, _address);

	return tox_addfriend_norequest(((tox_jni_globals_t *) messenger)->tox,
			__address);
}

JNIEXPORT jstring JNICALL Java_im_tox_jtoxcore_JTox_tox_1getaddress(
		JNIEnv * env, jobject obj, jlong messenger) {
	uint8_t addr[TOX_FRIEND_ADDRESS_SIZE];
	tox_getaddress(((tox_jni_globals_t *) messenger)->tox, addr);
	char id[ADDR_SIZE_HEX] = { 0 };
	addr_to_hex(addr, id);

	jstring result = (*env)->NewStringUTF(env, id);
	return result;
}

JNIEXPORT jint JNICALL Java_im_tox_jtoxcore_JTox_tox_1getfriend_1id(
		JNIEnv * env, jobject obj, jlong messenger, jstring address) {
	const uint8_t *_address = (*env)->GetStringUTFChars(env, address, 0);

	uint8_t __address[TOX_FRIEND_ADDRESS_SIZE];
	hex_to_addr(_address, __address);
	(*env)->ReleaseStringUTFChars(env, address, _address);

	return tox_getfriend_id(((tox_jni_globals_t *) messenger)->tox, __address);
}

JNIEXPORT jstring JNICALL Java_im_tox_jtoxcore_JTox_tox_1getclient_1id(
		JNIEnv * env, jobject obj, jlong messenger, jint friendnumber) {
	uint8_t address[TOX_FRIEND_ADDRESS_SIZE];

	if (tox_getclient_id(((tox_jni_globals_t *) messenger)->tox, friendnumber,
			address) != 0) {
		return 0;
	} else {
		char _address[ADDR_SIZE_HEX] = { 0 };
		addr_to_hex(address, _address);
		jstring result = (*env)->NewStringUTF(env, _address);
		return result;
	}
}

JNIEXPORT jboolean JNICALL Java_im_tox_jtoxcore_JTox_tox_1delfriend(
		JNIEnv * env, jobject obj, jlong messenger, jint friendnumber) {
	return tox_delfriend(((tox_jni_globals_t *) messenger)->tox, friendnumber)
			== 0 ? 0 : 1;
}

JNIEXPORT jint JNICALL Java_im_tox_jtoxcore_JTox_tox_1sendmessage__JILjava_lang_String_2(
		JNIEnv *env, jobject obj, jlong messenger, jint friendnumber,
		jstring message) {
	const char *_message = (*env)->GetStringUTFChars(env, message, 0);
	uint8_t *__message = strdup(_message);
	(*env)->ReleaseStringUTFChars(env, message, _message);

	uint32_t mess_id = tox_sendmessage(((tox_jni_globals_t *) messenger)->tox,
			friendnumber, __message, strlen(__message) + 1);
	free(__message);

	return mess_id;
}

JNIEXPORT jint JNICALL Java_im_tox_jtoxcore_JTox_tox_1sendmessage__JILjava_lang_String_2I(
		JNIEnv *env, jobject obj, jlong messenger, jint friendnumber,
		jstring message, jint messageID) {
	const char *_message = (*env)->GetStringUTFChars(env, message, 0);
	uint8_t *__message = strdup(_message);
	(*env)->ReleaseStringUTFChars(env, message, _message);

	uint32_t mess_id = tox_sendmessage_withid(
			((tox_jni_globals_t *) messenger)->tox, friendnumber, messageID,
			__message, strlen(__message) + 1);
	free(__message);

	return mess_id;
}

JNIEXPORT jboolean JNICALL Java_im_tox_jtoxcore_JTox_tox_1sendaction(
		JNIEnv * env, jobject obj, jlong messenger, jint friendnumber,
		jstring action) {
	const char *_action = (*env)->GetStringUTFChars(env, action, 0);
	uint8_t *__action = strdup(_action);
	(*env)->ReleaseStringUTFChars(env, action, _action);

	jboolean ret = tox_sendaction(((tox_jni_globals_t *) messenger)->tox,
			friendnumber, __action, strlen(__action) + 1);
	free(__action);
	return ret;
}

JNIEXPORT jboolean JNICALL Java_im_tox_jtoxcore_JTox_tox_1setname(JNIEnv *env,
		jobject obj, jlong messenger, jstring newname) {
	const char *_newname = (*env)->GetStringUTFChars(env, newname, 0);
	uint8_t *__newname = strdup(_newname);
	(*env)->ReleaseStringUTFChars(env, newname, _newname);

	jboolean ret =
			tox_setname(((tox_jni_globals_t *) messenger)->tox, __newname,
					strlen(__newname) + 1) == 0 ? JNI_FALSE : JNI_TRUE;
	free(__newname);

	return ret;
}

JNIEXPORT jstring JNICALL Java_im_tox_jtoxcore_JTox_tox_1getselfname(
		JNIEnv *env, jobject obj, jlong messenger) {
	uint8_t *name = malloc(TOX_MAX_NAME_LENGTH);
	uint16_t length = tox_getselfname(((tox_jni_globals_t *) messenger)->tox,
			name, TOX_MAX_NAME_LENGTH);

	if (length == 0) {
		free(name);
		return 0;
	}
	char *_name = malloc(TOX_MAX_NAME_LENGTH + 1);
	nullterminate(name, length, _name);
	jstring __name = (*env)->NewStringUTF(env, _name);
	free(_name);
	free(name);

	return __name;
}

JNIEXPORT jboolean JNICALL Java_im_tox_jtoxcore_JTox_tox_1set_1statusmessage(
		JNIEnv *env, jobject obj, jlong messenger, jstring newstatus) {
	const char *_newstatus = (*env)->GetStringUTFChars(env, newstatus, 0);
	uint8_t *__newstatus = strdup(_newstatus);
	(*env)->ReleaseStringUTFChars(env, newstatus, _newstatus);

	jboolean ret =
			tox_set_statusmessage(((tox_jni_globals_t *) messenger)->tox,
					__newstatus, strlen(__newstatus) + 1) == 0 ?
			JNI_FALSE :
																	JNI_TRUE;
	free(__newstatus);

	return ret;
}

JNIEXPORT jstring JNICALL Java_im_tox_jtoxcore_JTox_tox_1getname(JNIEnv * env,
		jobject obj, jlong messenger, jint friendnumber) {
	uint8_t *name = malloc(TOX_MAX_NAME_LENGTH);
	int ret = tox_getname(((tox_jni_globals_t *) messenger)->tox, friendnumber,
			name);

	if (ret == -1) {
		free(name);
		return 0;
	} else {
		char *_name = malloc(ret + 1);
		nullterminate(name, ret, _name);
		jstring __name = (*env)->NewStringUTF(env, _name);
		free(name);
		free(_name);
		return __name;
	}
}

JNIEXPORT jboolean JNICALL Java_im_tox_jtoxcore_JTox_tox_1set_1userstatus(
		JNIEnv * env, jobject obj, jlong messenger, jint userstatus) {
	return tox_set_userstatus(((tox_jni_globals_t *) messenger)->tox,
			userstatus) == 0 ? JNI_FALSE : JNI_TRUE;
}

JNIEXPORT jstring JNICALL Java_im_tox_jtoxcore_JTox_tox_1getstatusmessage(
		JNIEnv *env, jobject obj, jlong messenger, jint friendnumber) {
	Tox *tox = ((tox_jni_globals_t *) messenger)->tox;
	int length = tox_get_statusmessage_size(tox, friendnumber);
	uint8_t *status = malloc(length);
	tox_copy_statusmessage(tox, friendnumber, status, length);
	char *_status = malloc(length + 1);
	nullterminate(status, length, _status);
	jstring __status = (*env)->NewStringUTF(env, _status);
	free(status);
	free(_status);
	return __status;
}

/**
 * End general section
 */

/**
 * Begin Callback Section
 */

JNIEXPORT void JNICALL Java_im_tox_jtoxcore_JTox_tox_1onfriendrequest(
		JNIEnv * env, jobject obj, jlong messenger, jobject callback) {
	tox_jni_globals_t *_messenger = (tox_jni_globals_t *) messenger;
	if (_messenger->frqc) {
		if (_messenger->frqc->jobj) {
			(*env)->DeleteGlobalRef(env, _messenger->frqc->jobj);
		}
		free(_messenger->frqc);
	}

	friendrequest_callback_t *data = malloc(sizeof(friendrequest_callback_t));
	data->env = env;
	data->jobj = (*env)->NewGlobalRef(env, callback);
	(*env)->DeleteLocalRef(env, callback);
	_messenger->frqc = data;
	tox_callback_friendrequest(_messenger->tox, (void *) callback_friendrequest,
			data);
}

static void callback_friendrequest(uint8_t *pubkey, uint8_t *message,
		uint16_t length, void *ptr) {
	friendrequest_callback_t *data = ptr;
	jclass class = (*data->env)->GetObjectClass(data->env, data->jobj);
	jmethodID meth = (*data->env)->GetMethodID(data->env, class, "execute",
			"(Ljava/lang/String;Ljava/lang/String;)V");

	char buf[ADDR_SIZE_HEX] = { 0 };
	addr_to_hex(pubkey, buf);
	char *_message = malloc(length + 1);
	nullterminate(message, length, _message);
	jstring _pubkey = (*data->env)->NewStringUTF(data->env, buf);
	jstring __message = (*data->env)->NewStringUTF(data->env, _message);
	free(_message);

	(*data->env)->CallVoidMethod(data->env, data->jobj, meth, _pubkey,
			__message);
}

JNIEXPORT void JNICALL Java_im_tox_jtoxcore_JTox_tox_1onfriendmessage(
		JNIEnv * env, jobject obj, jlong messenger, jobject callback) {
	tox_jni_globals_t *_messenger = (tox_jni_globals_t *) messenger;
	if (_messenger->frmc) {
		if (_messenger->frmc->jobj) {
			(*env)->DeleteGlobalRef(env, _messenger->frmc->jobj);
		}
		free(_messenger->frmc);
	}

	friendmessage_callback_t *data = malloc(sizeof(friendmessage_callback_t));
	data->env = env;
	data->jobj = (*env)->NewGlobalRef(env, callback);
	(*env)->DeleteLocalRef(env, callback);
	_messenger->frmc = data;
	tox_callback_friendmessage(_messenger->tox, (void *) callback_friendmessage,
			data);
}

static void callback_friendmessage(Tox * tox, int friendnumber,
		uint8_t *message, uint16_t length, void *ptr) {
	friendmessage_callback_t *data = ptr;
	jclass class = (*data->env)->GetObjectClass(data->env, data->jobj);
	jmethodID meth = (*data->env)->GetMethodID(data->env, class, "execute",
			"(ILjava/lang/String;)V");

	char *_message = malloc(length + 1);
	nullterminate(message, length, _message);
	jstring __message = (*data->env)->NewStringUTF(data->env, _message);
	free(_message);
	(*data->env)->CallVoidMethod(data->env, data->jobj, meth, friendnumber,
			__message);
}

JNIEXPORT void JNICALL Java_im_tox_jtoxcore_JTox_tox_1onaction(JNIEnv * env,
		jobject obj, jlong messenger, jobject callback) {
	tox_jni_globals_t *_messenger = (tox_jni_globals_t *) messenger;
	if (_messenger->ac) {
		if (_messenger->ac->jobj) {
			(*env)->DeleteGlobalRef(env, _messenger->ac->jobj);
		}
		free(_messenger->ac);
	}

	action_callback_t *data = malloc(sizeof(action_callback_t));
	data->env = env;
	data->jobj = (*env)->NewGlobalRef(env, callback);
	(*env)->DeleteLocalRef(env, callback);
	_messenger->ac = data;
	tox_callback_action(_messenger->tox, (void *) callback_action, data);
}

static void callback_action(Tox * tox, int friendnumber, uint8_t *action,
		uint16_t length, void *ptr) {
	action_callback_t *data = ptr;
	jclass class = (*data->env)->GetObjectClass(data->env, data->jobj);
	jmethodID meth = (*data->env)->GetMethodID(data->env, class, "execute",
			"(ILjava/lang/String;)V");

	char *_action = malloc(length + 1);
	nullterminate(action, length, _action);
	jstring __action = (*data->env)->NewStringUTF(data->env, _action);
	free(_action);
	(*data->env)->CallVoidMethod(data->env, data->jobj, meth, friendnumber,
			__action);
}

JNIEXPORT void JNICALL Java_im_tox_jtoxcore_JTox_tox_1onnamechange(JNIEnv * env,
		jobject obj, jlong messenger, jobject callback) {
	tox_jni_globals_t *_messenger = (tox_jni_globals_t *) messenger;
	if (_messenger->nc) {
		if (_messenger->nc->jobj) {
			(*env)->DeleteGlobalRef(env, _messenger->nc->jobj);
		}
		free(_messenger->nc);
	}

	namechange_callback_t *data = malloc(sizeof(namechange_callback_t));
	data->env = env;
	data->jobj = (*env)->NewGlobalRef(env, callback);
	(*env)->DeleteLocalRef(env, callback);
	_messenger->nc = data;
	tox_callback_namechange(_messenger->tox, (void *) callback_namechange,
			data);
}

static void callback_namechange(Tox * tox, int friendnumber, uint8_t *newname,
		uint16_t length, void *ptr) {
	namechange_callback_t *data = ptr;
	jclass class = (*data->env)->GetObjectClass(data->env, data->jobj);
	jmethodID meth = (*data->env)->GetMethodID(data->env, class, "execute",
			"(ILjava/lang/String;)V");

	char *_newname = malloc(length + 1);
	nullterminate(newname, length, _newname);
	jstring __newname = (*data->env)->NewStringUTF(data->env, _newname);
	free(_newname);
	(*data->env)->CallVoidMethod(data->env, data->jobj, meth, friendnumber,
			__newname);
}

JNIEXPORT void JNICALL Java_im_tox_jtoxcore_JTox_tox_1onstatusmessage(
		JNIEnv * env, jobject obj, jlong messenger, jobject callback) {
	tox_jni_globals_t *_messenger = (tox_jni_globals_t *) messenger;
	if (_messenger->smc) {
		if (_messenger->smc->jobj) {
			(*env)->DeleteGlobalRef(env, _messenger->smc->jobj);
		}
		free(_messenger->smc);
	}

	statusmessage_callback_t *data = malloc(sizeof(statusmessage_callback_t));
	data->env = env;
	data->jobj = (*env)->NewGlobalRef(env, callback);
	(*env)->DeleteLocalRef(env, callback);
	_messenger->smc = data;
	tox_callback_statusmessage(_messenger->tox, (void *) callback_statusmessage,
			data);
}

static void callback_statusmessage(Tox *tox, int friendnumber,
		uint8_t *newstatus, uint16_t length, void *ptr) {
	statusmessage_callback_t *data = ptr;
	jclass class = (*data->env)->GetObjectClass(data->env, data->jobj);
	jmethodID meth = (*data->env)->GetMethodID(data->env, class, "execute",
			"(ILjava/lang/String;)V");

	char *_newstatus = malloc(length + 1);
	nullterminate(newstatus, length, _newstatus);
	jstring __newstatus = (*data->env)->NewStringUTF(data->env, _newstatus);
	free(_newstatus);
	(*data->env)->CallVoidMethod(data->env, data->jobj, meth, friendnumber,
			__newstatus);
}

JNIEXPORT void JNICALL Java_im_tox_jtoxcore_JTox_tox_1on_1userstatus(
		JNIEnv * env, jobject obj, jlong messenger, jobject callback) {
	tox_jni_globals_t *_messenger = (tox_jni_globals_t *) messenger;
	if (_messenger->usc) {
		if (_messenger->usc->jobj) {
			(*env)->DeleteGlobalRef(env, _messenger->usc->jobj);
		}
		free(_messenger->usc);
	}

	userstatus_callback_t *data = malloc(sizeof(read_receipt_callback_t));
	data->env = env;
	data->jobj = (*env)->NewGlobalRef(env, callback);
	(*env)->DeleteLocalRef(env, callback);
	_messenger->usc = data;
	tox_callback_userstatus(_messenger->tox, (void *) callback_userstatus,
			data);
}

static void callback_userstatus(Tox *tox, int friendnumber,
		TOX_USERSTATUS status, void *ptr) {
	userstatus_callback_t *data = ptr;
	jclass class = (*data->env)->GetObjectClass(data->env, data->jobj);
	jmethodID meth = (*data->env)->GetMethodID(data->env, class, "execute",
			"(ILim/tox/jtoxcore/ToxUserStatus;)V");
	jclass us_enum = (*data->env)->FindClass(data->env,
			"Lim/tox/jtoxcore/ToxUserStatus;");

	char *enum_name;
	switch (status) {
	case TOX_USERSTATUS_NONE:
		enum_name = "TOX_USERSTATUS_NONE";
		break;
	case TOX_USERSTATUS_AWAY:
		enum_name = "TOX_USERSTATUS_AWAY";
		break;
	case TOX_USERSTATUS_BUSY:
		enum_name = "TOX_USERSTATUS_BUSY";
		break;
	default:
		enum_name = "TOX_USERSTATUS_INVALID";
		break;
	}

	jfieldID fieldID = (*data->env)->GetStaticFieldID(data->env, us_enum,
			enum_name, "Lim/tox/jtoxcore/ToxUserStatus;");
	jobject enum_val = (*data->env)->GetStaticObjectField(data->env, us_enum,
			fieldID);
	(*data->env)->CallVoidMethod(data->env, data->jobj, meth, friendnumber,
			enum_val);
}

JNIEXPORT void JNICALL Java_im_tox_jtoxcore_JTox_tox_1on_1read_1receipt(
		JNIEnv * env, jobject obj, jlong messenger, jobject callback) {
	tox_jni_globals_t *_messenger = (tox_jni_globals_t *) messenger;
	if (_messenger->rrc) {
		if (_messenger->rrc->jobj) {
			(*env)->DeleteGlobalRef(env, _messenger->rrc->jobj);
		}
		free(_messenger->rrc);
	}

	read_receipt_callback_t *data = malloc(sizeof(read_receipt_callback_t));
	data->env = env;
	data->jobj = (*env)->NewGlobalRef(env, callback);
	(*env)->DeleteLocalRef(env, callback);
	_messenger->rrc = data;
	tox_callback_read_receipt(_messenger->tox, (void *) callback_read_receipt,
			data);
}

static void callback_read_receipt(Tox *tox, int friendnumber, uint32_t receipt,
		void *ptr) {
	read_receipt_callback_t *data = ptr;
	jclass class = (*data->env)->GetObjectClass(data->env, data->jobj);
	jmethodID meth = (*data->env)->GetMethodID(data->env, class, "execute",
			"(II)V");
	(*data->env)->CallVoidMethod(data->env, data->jobj, meth, friendnumber,
			receipt);
}

JNIEXPORT void JNICALL Java_im_tox_jtoxcore_JTox_tox_1on_1connectionstatus(
		JNIEnv *env, jobject obj, jlong messenger, jobject callback) {
	tox_jni_globals_t *_messenger = (tox_jni_globals_t *) messenger;
	if (_messenger->csc) {
		if (_messenger->csc->jobj) {
			(*env)->DeleteGlobalRef(env, _messenger->csc->jobj);
		}
		free(_messenger->csc);
	}

	connectionstatus_callback_t *data = malloc(sizeof(read_receipt_callback_t));
	data->env = env;
	data->jobj = (*env)->NewGlobalRef(env, callback);
	(*env)->DeleteLocalRef(env, callback);
	_messenger->csc = data;
	tox_callback_connectionstatus(_messenger->tox,
			(void *) callback_connectionstatus, data);
}

static void callback_connectionstatus(Tox *tox, int friendnumber,
		uint8_t newstatus, void *ptr) {
	connectionstatus_callback_t *data = ptr;
	jclass class = (*data->env)->GetObjectClass(data->env, data->jobj);
	jmethodID meth = (*data->env)->GetMethodID(data->env, class, "execute",
			"(IZ)V");
	jboolean _newstatus;
	if (newstatus == 0) {
		_newstatus = JNI_TRUE;
	} else {
		_newstatus = JNI_FALSE;
	}
	(*data->env)->CallVoidMethod(data->env, data->jobj, meth, friendnumber,
			_newstatus);
}
