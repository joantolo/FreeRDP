/**
 * FreeRDP: A Remote Desktop Protocol Implementation
 * RPC Fault Handling
 *
 * Copyright 2012 Marc-Andre Moreau <marcandre.moreau@gmail.com>
 *
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

#include <freerdp/config.h>

#include <freerdp/log.h>

#include "rpc_fault.h"

#include "rpc.h"

#define TAG FREERDP_TAG("core.gateway.rpc")

static ALIGN64 const RPC_FAULT_CODE RPC_FAULT_CODES[] = {
	DEFINE_RPC_FAULT_CODE(nca_s_fault_object_not_found, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(nca_s_fault_cancel, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(nca_s_fault_addr_error, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(nca_s_fault_context_mismatch, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(nca_s_fault_fp_div_zero, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(nca_s_fault_fp_error, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(nca_s_fault_fp_overflow, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(nca_s_fault_fp_underflow, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(nca_s_fault_ill_inst, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(nca_s_fault_int_div_by_zero, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(nca_s_fault_int_overflow, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(nca_s_fault_invalid_bound, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(nca_s_fault_invalid_tag, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(nca_s_fault_pipe_closed, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(nca_s_fault_pipe_comm_error, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(nca_s_fault_pipe_discipline, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(nca_s_fault_pipe_empty, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(nca_s_fault_pipe_memory, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(nca_s_fault_pipe_order, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(nca_s_fault_remote_no_memory, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(nca_s_fault_user_defined, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(nca_s_fault_tx_open_failed, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(nca_s_fault_codeset_conv_error, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(nca_s_fault_no_client_stub, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_INVALID_STRING_BINDING, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_WRONG_KIND_OF_BINDING, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_INVALID_BINDING, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_PROTSEQ_NOT_SUPPORTED, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_INVALID_RPC_PROTSEQ, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_INVALID_STRING_UUID, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_INVALID_ENDPOINT_FORMAT, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_INVALID_NET_ADDR, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_NO_ENDPOINT_FOUND, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_INVALID_TIMEOUT, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_OBJECT_NOT_FOUND, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_ALREADY_REGISTERED, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_TYPE_ALREADY_REGISTERED, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_ALREADY_LISTENING, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_NO_PROTSEQS_REGISTERED, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_NOT_LISTENING, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_UNKNOWN_MGR_TYPE, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_UNKNOWN_IF, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_NO_BINDINGS, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_NO_PROTSEQS, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_CANT_CREATE_ENDPOINT, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_OUT_OF_RESOURCES, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_SERVER_UNAVAILABLE, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_SERVER_TOO_BUSY, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_INVALID_NETWORK_OPTIONS, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_NO_CALL_ACTIVE, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_CALL_FAILED, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_CALL_FAILED_DNE, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_PROTOCOL_ERROR, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_PROXY_ACCESS_DENIED, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_UNSUPPORTED_TRANS_SYN, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_UNSUPPORTED_TYPE, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_INVALID_TAG, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_INVALID_BOUND, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_NO_ENTRY_NAME, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_INVALID_NAME_SYNTAX, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_UNSUPPORTED_NAME_SYNTAX, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_UUID_NO_ADDRESS, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_DUPLICATE_ENDPOINT, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_UNKNOWN_AUTHN_TYPE, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_MAX_CALLS_TOO_SMALL, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_STRING_TOO_LONG, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_PROTSEQ_NOT_FOUND, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_PROCNUM_OUT_OF_RANGE, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_BINDING_HAS_NO_AUTH, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_UNKNOWN_AUTHN_SERVICE, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_UNKNOWN_AUTHN_LEVEL, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_INVALID_AUTH_IDENTITY, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_UNKNOWN_AUTHZ_SERVICE, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(EPT_S_INVALID_ENTRY, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(EPT_S_CANT_PERFORM_OP, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(EPT_S_NOT_REGISTERED, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_NOTHING_TO_EXPORT, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_INCOMPLETE_NAME, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_INVALID_VERS_OPTION, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_NO_MORE_MEMBERS, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_NOT_ALL_OBJS_UNEXPORTED, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_INTERFACE_NOT_FOUND, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_ENTRY_ALREADY_EXISTS, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_ENTRY_NOT_FOUND, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_NAME_SERVICE_UNAVAILABLE, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_INVALID_NAF_ID, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_CANNOT_SUPPORT, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_NO_CONTEXT_AVAILABLE, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_INTERNAL_ERROR, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_ZERO_DIVIDE, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_ADDRESS_ERROR, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_FP_DIV_ZERO, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_FP_UNDERFLOW, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_FP_OVERFLOW, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_X_NO_MORE_ENTRIES, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_X_SS_CHAR_TRANS_OPEN_FAIL, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_X_SS_CHAR_TRANS_SHORT_FILE, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_X_SS_IN_NULL_CONTEXT, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_X_SS_CONTEXT_DAMAGED, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_X_SS_HANDLES_MISMATCH, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_X_SS_CANNOT_GET_CALL_HANDLE, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_X_NULL_REF_POINTER, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_X_ENUM_VALUE_OUT_OF_RANGE, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_X_BYTE_COUNT_TOO_SMALL, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_X_BAD_STUB_DATA, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_NO_INTERFACES, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_CALL_CANCELLED, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_BINDING_INCOMPLETE, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_COMM_FAILURE, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_UNSUPPORTED_AUTHN_LEVEL, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_NO_PRINC_NAME, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_NOT_RPC_ERROR, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_UUID_LOCAL_ONLY, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_SEC_PKG_ERROR, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_NOT_CANCELLED, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_X_INVALID_ES_ACTION, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_X_WRONG_ES_VERSION, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_X_WRONG_STUB_VERSION, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_X_INVALID_PIPE_OBJECT, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_X_WRONG_PIPE_ORDER, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_X_WRONG_PIPE_VERSION, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_COOKIE_AUTH_FAILED, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_GROUP_MEMBER_NOT_FOUND, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(EPT_S_CANT_CREATE, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_INVALID_OBJECT, CAT_GATEWAY)
};

static const RPC_FAULT_CODE RPC_TSG_FAULT_CODES[] = {
	DEFINE_RPC_FAULT_CODE(RPC_S_OK, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_INVALID_ARG, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_OUT_OF_MEMORY, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_OUT_OF_THREADS, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_INVALID_LEVEL, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_BUFFER_TOO_SMALL, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_INVALID_SECURITY_DESC, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_ACCESS_DENIED, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_SERVER_OUT_OF_MEMORY, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_ASYNC_CALL_PENDING, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_UNKNOWN_PRINCIPAL, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_S_TIMEOUT, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_X_NO_MEMORY, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_X_INVALID_BOUND, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_X_INVALID_TAG, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_X_ENUM_VALUE_TOO_LARGE, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_X_SS_CONTEXT_MISMATCH, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_X_INVALID_BUFFER, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_X_PIPE_APP_MEMORY, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(RPC_X_INVALID_PIPE_OPERATION, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(ERROR_ONLY_IF_CONNECTED, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(ERROR_GRACEFUL_DISCONNECT, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(ERROR_OPERATION_ABORTED, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(ERROR_BAD_ARGUMENTS, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(E_PROXY_INTERNALERROR, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(E_PROXY_RAP_ACCESSDENIED, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(E_PROXY_NAP_ACCESSDENIED, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(E_PROXY_TS_CONNECTFAILED, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(E_PROXY_ALREADYDISCONNECTED, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(E_PROXY_QUARANTINE_ACCESSDENIED, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(E_PROXY_NOCERTAVAILABLE, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(E_PROXY_COOKIE_BADPACKET, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(E_PROXY_COOKIE_AUTHENTICATION_ACCESS_DENIED, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(E_PROXY_UNSUPPORTED_AUTHENTICATION_METHOD, CAT_GATEWAY),
	DEFINE_RPC_FAULT_CODE(E_PROXY_CAPABILITYMISMATCH, CAT_GATEWAY),
	{ HRESULT_CODE(E_PROXY_NOTSUPPORTED), "E_PROXY_NOTSUPPORTED", CAT_GATEWAY },
	{ HRESULT_CODE(E_PROXY_TS_CONNECTFAILED), "E_PROXY_TS_CONNECTFAILED", CAT_GATEWAY },
	{ HRESULT_CODE(E_PROXY_MAXCONNECTIONSREACHED), "E_PROXY_MAXCONNECTIONSREACHED", CAT_GATEWAY },
	{ HRESULT_CODE(E_PROXY_INTERNALERROR), "E_PROXY_INTERNALERROR", CAT_GATEWAY },
	{ HRESULT_CODE(E_PROXY_SESSIONTIMEOUT), "E_PROXY_SESSIONTIMEOUT", CAT_GATEWAY },
	{ HRESULT_CODE(E_PROXY_REAUTH_AUTHN_FAILED), "E_PROXY_REAUTH_AUTHN_FAILED", CAT_GATEWAY },
	{ HRESULT_CODE(E_PROXY_REAUTH_CAP_FAILED), "E_PROXY_REAUTH_CAP_FAILED", CAT_GATEWAY },
	{ HRESULT_CODE(E_PROXY_REAUTH_RAP_FAILED), "E_PROXY_REAUTH_RAP_FAILED", CAT_GATEWAY },
	{ HRESULT_CODE(E_PROXY_SDR_NOT_SUPPORTED_BY_TS), "E_PROXY_SDR_NOT_SUPPORTED_BY_TS",
	  CAT_GATEWAY },
	{ HRESULT_CODE(E_PROXY_REAUTH_NAP_FAILED), "E_PROXY_REAUTH_NAP_FAILED", CAT_GATEWAY },
	{ HRESULT_CODE(E_PROXY_CONNECTIONABORTED), "E_PROXY_CONNECTIONABORTED", CAT_GATEWAY },
	{ HRESULT_CODE(E_PROXY_NOCERTAVAILABLE), "E_PROXY_NOCERTAVAILABLE", CAT_GATEWAY },
	{ HRESULT_CODE(RPC_S_CALL_CANCELLED), "RPC_S_CALL_CANCELLED", CAT_GATEWAY }
};

/**
 * [MS-RPCE] 3.1.1.5.5 Returning Win32 Error Values:
 * http://msdn.microsoft.com/en-us/library/ee442005/
 */

static UINT32 rpc_map_status_code_to_win32_error_code(UINT32 code)
{
	switch (code)
	{
		case nca_s_comm_failure:
			return RPC_S_COMM_FAILURE;

		case nca_s_op_rng_error:
			return RPC_S_PROCNUM_OUT_OF_RANGE;

		case nca_s_unk_if:
			return RPC_S_UNKNOWN_IF;

		case nca_s_wrong_boot_time:
			return nca_s_wrong_boot_time;

		case nca_s_you_crashed:
			return RPC_S_CALL_FAILED;

		case nca_s_proto_error:
			return RPC_S_PROTOCOL_ERROR;

		case nca_s_out_args_too_big:
			return RPC_S_SERVER_OUT_OF_MEMORY;

		case nca_s_server_too_busy:
			return RPC_S_SERVER_TOO_BUSY;

		case nca_s_unsupported_type:
			return RPC_S_UNSUPPORTED_TYPE;

		case nca_s_fault_int_div_by_zero:
			return RPC_S_ZERO_DIVIDE;

		case nca_s_fault_addr_error:
			return RPC_S_ADDRESS_ERROR;

		case nca_s_fault_fp_div_zero:
			return RPC_S_FP_DIV_ZERO;

		case nca_s_fault_fp_underflow:
			return RPC_S_FP_UNDERFLOW;

		case nca_s_fault_fp_overflow:
			return RPC_S_FP_OVERFLOW;

		case nca_s_fault_invalid_tag:
			return RPC_S_INVALID_TAG;

		case nca_s_fault_invalid_bound:
			return RPC_S_INVALID_BOUND;

		case nca_s_rpc_version_mismatch:
			return RPC_S_PROTOCOL_ERROR;

		case nca_s_unspec_reject:
			return RPC_S_CALL_FAILED;

		case nca_s_bad_actid:
			return RPC_S_CALL_FAILED_DNE;

		case nca_s_who_are_you_failed:
			return RPC_S_CALL_FAILED;

		case nca_s_manager_not_entered:
			return RPC_S_CALL_FAILED_DNE;

		case nca_s_fault_cancel:
			return RPC_S_CALL_CANCELLED;

		case nca_s_fault_ill_inst:
			return RPC_S_ADDRESS_ERROR;

		case nca_s_fault_fp_error:
			return RPC_S_FP_OVERFLOW;

		case nca_s_fault_int_overflow:
			return RPC_S_ADDRESS_ERROR;

		case nca_s_fault_unspec:
			return RPC_S_CALL_FAILED;

		case nca_s_fault_remote_comm_failure:
			return nca_s_fault_remote_comm_failure;

		case nca_s_fault_pipe_empty:
			return RPC_X_PIPE_EMPTY;

		case nca_s_fault_pipe_closed:
			return RPC_X_PIPE_CLOSED;

		case nca_s_fault_pipe_order:
			return RPC_X_WRONG_PIPE_ORDER;

		case nca_s_fault_pipe_discipline:
			return RPC_X_PIPE_DISCIPLINE_ERROR;

		case nca_s_fault_pipe_comm_error:
			return RPC_S_COMM_FAILURE;

		case nca_s_fault_pipe_memory:
			return RPC_S_OUT_OF_MEMORY;

		case nca_s_fault_context_mismatch:
			return RPC_X_SS_CONTEXT_MISMATCH;

		case nca_s_fault_remote_no_memory:
			return RPC_S_SERVER_OUT_OF_MEMORY;

		case nca_s_invalid_pres_context_id:
			return RPC_S_PROTOCOL_ERROR;

		case nca_s_unsupported_authn_level:
			return RPC_S_UNSUPPORTED_AUTHN_LEVEL;

		case nca_s_invalid_checksum:
			return RPC_S_CALL_FAILED_DNE;

		case nca_s_invalid_crc:
			return RPC_S_CALL_FAILED_DNE;

		case nca_s_fault_user_defined:
			return nca_s_fault_user_defined;

		case nca_s_fault_tx_open_failed:
			return nca_s_fault_tx_open_failed;

		case nca_s_fault_codeset_conv_error:
			return nca_s_fault_codeset_conv_error;

		case nca_s_fault_object_not_found:
			return nca_s_fault_object_not_found;

		case nca_s_fault_no_client_stub:
			return nca_s_fault_no_client_stub;
		default:
			break;
	}

	return code;
}

const char* rpc_error_to_string(UINT32 code)
{
	static char buffer[1024];

	for (size_t index = 0; index < ARRAYSIZE(RPC_FAULT_CODES); index++)
	{
		const RPC_FAULT_CODE* current = &RPC_FAULT_CODES[index];
		if (current->code == code)
		{
			(void)sprintf_s(buffer, ARRAYSIZE(buffer), "%s", current->name);
			goto out;
		}
	}

	for (size_t index = 0; index < ARRAYSIZE(RPC_TSG_FAULT_CODES); index++)
	{
		const RPC_FAULT_CODE* current = &RPC_TSG_FAULT_CODES[index];
		if (current->code == code)
		{
			(void)sprintf_s(buffer, ARRAYSIZE(buffer), "%s", current->name);
			goto out;
		}
	}

	for (size_t index = 0; index < ARRAYSIZE(RPC_TSG_FAULT_CODES); index++)
	{
		const RPC_FAULT_CODE* current = &RPC_TSG_FAULT_CODES[index];
		if (current->code == HRESULT_CODE(code))
		{
			(void)sprintf_s(buffer, ARRAYSIZE(buffer), "%s", current->name);
			goto out;
		}
	}

	(void)sprintf_s(buffer, ARRAYSIZE(buffer), "%s [0x%08" PRIX32 "]", "UNKNOWN", code);
out:
	return buffer;
}

const char* rpc_error_to_category(UINT32 code)
{
	for (size_t index = 0; index < ARRAYSIZE(RPC_FAULT_CODES); index++)
	{
		const RPC_FAULT_CODE* current = &RPC_FAULT_CODES[index];
		if (current->code == code)
			return current->category;
	}

	for (size_t index = 0; index < ARRAYSIZE(RPC_TSG_FAULT_CODES); index++)
	{
		const RPC_FAULT_CODE* current = &RPC_TSG_FAULT_CODES[index];
		if (current->code == code)
			return current->category;
	}

	for (size_t index = 0; index < ARRAYSIZE(RPC_TSG_FAULT_CODES); index++)
	{
		const RPC_FAULT_CODE* current = &RPC_TSG_FAULT_CODES[index];
		if (current->code == HRESULT_CODE(code))
			return current->category;
	}

	return "UNKNOWN";
}

int rpc_recv_fault_pdu(UINT32 status)
{
	UINT32 code = rpc_map_status_code_to_win32_error_code(status);
	WLog_ERR(TAG, "RPC Fault PDU: status=%s", rpc_error_to_string(code));
	return 0;
}
