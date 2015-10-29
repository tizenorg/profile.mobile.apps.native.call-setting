/*
 * Copyright (c) 2009-2015 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */

#ifndef _CST_PARSER_GENERATOR_DOCOMO_H_
#define _CST_PARSER_GENERATOR_DOCOMO_H_

#include "cst-common-docomo.h"

/*
 * @enum cst_svc_status_code_docomo_t
 * This enumeration defines the SS status code
 * returned from the network for various Telephony operations.
 */
typedef enum {
	/* Represents either Uncontract/Unregister/Deactivate */
	CST_SVC_STATE_CODE_0_DOCOMO = 0,
	/* Represents either Uncontract/Unregister/Activate */
	CST_SVC_STATE_CODE_1_DOCOMO = 1,
	/* Represents either Uncontract/Register/Deactivate */
	CST_SVC_STATE_CODE_2_DOCOMO = 2,
	/* Represents either Uncontract/Register/Activate */
	CST_SVC_STATE_CODE_3_DOCOMO = 3,
	/* Represents either Contract/Unregister/Deactivate */
	CST_SVC_STATE_CODE_4_DOCOMO = 4,
	/* Represents either Contract/Unregister/Activate */
	CST_SVC_STATE_CODE_5_DOCOMO = 5,
	/* Represents either Contract/Register/Deactivate */
	CST_SVC_STATE_CODE_6_DOCOMO = 6,
	/* Represents either Contract/Register/Activate */
	CST_SVC_STATE_CODE_7_DOCOMO = 7,
	/* Represents either Uncontract/Unregister/Deactivate by conflict */
	CST_SVC_STATE_CODE_8_DOCOMO = 8,
	/* Represents either Uncontract/Unregister/Activate by conflict */
	CST_SVC_STATE_CODE_9_DOCOMO = 9,
	/* Represents either Uncontract/Register/Deactivate by conflict */
	CST_SVC_STATE_CODE_10_DOCOMO = 10,
	/* Represents either Uncontract/Register/Activate by conflict */
	CST_SVC_STATE_CODE_11_DOCOMO = 11,
	/* Represents either Contract/Unregister/Deactivate by conflict */
	CST_SVC_STATE_CODE_12_DOCOMO = 12,
	/* Represents either Contract/Unregister/Activate by conflict */
	CST_SVC_STATE_CODE_13_DOCOMO = 13,
	/* Represents either Contract/Register/Activate by conflict */
	CST_SVC_STATE_CODE_14_DOCOMO = 14,
	/* Represents either Contract/Register/Deactivate by conflict */
	CST_SVC_STATE_CODE_15_DOCOMO = 15,
} cst_svc_status_code_docomo_t;

/*
 * @enum cst_svc_state_change_docomo_t
 * This enumeration defines the Service state code which represents the
 * "Activation" status of the Telephony operation executed.
 */
typedef enum {
	/* Represents "Deactive" state before operation and "Deactive" state after
	   operation */
	CST_SVC_STATE_DEACT_DEACT_DOCOMO = 0,
	/* Represents "Deactive" state before operation and "Active" state after
	   operation */
	CST_SVC_STATE_DEACT_ACT_DOCOMO = 1,
	/* Represents "Active" state before operation and "Deactive" state after
	   operation */
	CST_SVC_STATE_ACT_DEACT_DOCOMO = 2,
	/* Represents "Active" state before operation and "Active" state after
	   operation */
	CST_SVC_STATE_ACT_ACT_DOCOMO = 3,
} cst_svc_state_change_docomo_t;

/*
 * @brief Generate the USSD string.
 *
 * @param ussd_data The handle to the cst_req_ussd_data_docomo_t structure
 * containing all the information required to generate the USSD string.
 *
 * @param ussd_string The output character pointer wherein the USSD string is
 * generated and copied.
 *
 * This function uses the 'ussd_data' structure to manipulate and generate the
 * USSD string for the telephony request which is then sent to the network.
 *
 * @see cst_parse_ussd_response_docomo()
 */
void _cst_generate_ussd_string_docomo(cst_req_ussd_data_docomo_t *ussd_data,
									  char *ussd_string);

/*
 * @brief Parse the USSD string.
 *
 * @param ussd_resp The handle to the response USSD string returned by the
 * network for the requested action.
 *
 * @param resp_data The output structure wherein the parsed USSD string SI
 * data's are filled in.
 *
 * @return Returns 'EINA_TRUE' if the parsing of the USSD string was success, or
 * else returns 'EINA_FALSE'.
 *
 * This function uses the 'ussd_resp' string to parse the response from the
 * network for the requested action and accordingly intimate the user about the
 * possible 'success' and 'failure' of the operation.
 *
 * @see cst_generate_ussd_string_docomo()
 */
Eina_Bool _cst_parse_ussd_response_docomo(char *ussd_resp,
		cst_resp_ussd_data_docomo_t *resp_data);

/*
 * @brief Parse the SI data for service status.
 *
 * @param si_data An instance of the SI data received in the USSD response
 * string from the network which represents the service status of the requested
 * network operation.
 *
 * @return The integer value representing the service status which corresponds
 * to the enum 'cst_svc_status_code_docomo_t'.
 *
 * This function uses the 'si_data' to parse the service status of the requested
 * network operation.
 *
 * @see cst_parse_svc_state_change_docomo()
 */
int _cst_parse_svc_status_docomo(char *si_data);

/*
 * @brief Parse the SI data for service state change.
 *
 * @param si_data An instance of the SI data received in the USSD response
 * string from the network which represents the service state of the requested
 * network operation.
 *
 * @return The integer value representing the service state change which
 * corresponds to the enum 'cst_svc_state_change_docomo_t'.
 *
 * This function uses the 'si_data' to parse the service state of the requested
 * network operation which represents both the old/new state after
 * request/response operation is performed.
 *
 * @see cst_parse_svc_status_docomo()
 */
int _cst_parse_svc_state_change_docomo(char *si_data);

/*
 * @brief Parse the Service code from the input USSD response string.
 *
 * @param ussd_resp_str The handle to the response USSD string returned by the
 * network for the requested action.
 *
 * @return Returns the service code defined by the structure
 * 'cst_ussd_code_docomo_t' or returns '-1' if parse error.
 *
 * This function uses the 'ussd_resp_str' string to parse the response from the
 * network for the requested action and accordingly return the service code.
 *
 * @see _cst_parse_ussd_si_datas()
 */
int _cst_parse_ussd_svc_code(char *ussd_resp_str);

/*
 * @brief Parse the SI data's from the USSD string.
 *
 * @param ussd_resp_str The handle to the response USSD string returned by the
 * network for the requested action.
 *
 * @param resp_data The output structure wherein the parsed USSD string SI
 * data's are filled in.
 *
 * @return Returns 'EINA_TRUE' if the parsing of the USSD string was success, or
 * else returns 'EINA_FALSE'.
 *
 * This function uses the 'ussd_resp_str' string to parse the response from the
 * network for the requested action and accordingly update the SI data values in
 * the 'resp_data' structure.
 *
 * @see _cst_parse_ussd_svc_code()
 */
Eina_Bool _cst_parse_ussd_si_datas(char *ussd_resp_str,
		cst_resp_ussd_data_docomo_t *resp_data);

#endif /* _CST_PARSER_GENERATOR_DOCOMO_H_ */

