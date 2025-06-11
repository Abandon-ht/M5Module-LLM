/*
 * SPDX-FileCopyrightText: 2024 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include "api_sys.h"

using namespace m5_module_llm;

static const char* _cmd_ping =
    "{\"request_id\":\"sys_ping\",\"work_id\":\"sys\",\"action\":\"ping\",\"object\":\"None\",\"data\":\"None\"}";
static const char* _cmd_reset =
    "{\"request_id\":\"sys_reset\",\"work_id\":\"sys\",\"action\":\"reset\",\"object\":\"None\",\"data\":\"None\"}";
static const char* _cmd_reboot =
    "{\"request_id\":\"sys_reboot\",\"work_id\":\"sys\",\"action\":\"reboot\",\"object\":\"None\",\"data\":\"None\"}";
static const char* _cmd_version =
    "{\"request_id\":\"sys_version\",\"work_id\":\"sys\",\"action\":\"version\",\"object\":\"None\",\"data\":\"None\"}";

void ApiSys::init(ModuleMsg* moduleMsg)
{
    _module_msg = moduleMsg;
}

int ApiSys::ping()
{
    int ret = MODULE_LLM_WAIT_RESPONSE_TIMEOUT;
    _module_msg->sendCmdAndWaitToTakeMsg(
        _cmd_ping, "sys_ping", [&ret](ResponseMsg_t& msg) { ret = msg.error.code; }, 2000);
    return ret;
}

String ApiSys::version()
{
    String version_str;
    int ret = MODULE_LLM_WAIT_RESPONSE_TIMEOUT;
    _module_msg->sendCmdAndWaitToTakeMsg(
        _cmd_version, "sys_version",
        [&version_str, &ret](ResponseMsg_t& msg) {
            ret = msg.error.code;
            if (ret == MODULE_LLM_OK) {
                JsonDocument doc;
                deserializeJson(doc, msg.raw_msg);
                version_str = doc["data"].as<String>();
            }
        },
        2000);
    return version_str;
}

int ApiSys::reset(bool waitResetFinish)
{
    int ret = MODULE_LLM_WAIT_RESPONSE_TIMEOUT;

    // Send cmd and wait ok response
    _module_msg->sendCmdAndWaitToTakeMsg(
        _cmd_reset, "sys_reset", [&ret](ResponseMsg_t& msg) { ret = msg.error.code; }, 2000);
    if (ret != MODULE_LLM_OK) {
        return ret;
    }

    // Wait reset finish response
    if (waitResetFinish) {
        ret = MODULE_LLM_WAIT_RESPONSE_TIMEOUT;
        _module_msg->responseMsgList.clear();
        _module_msg->waitAndTakeMsg(
            "0", [&ret](ResponseMsg_t& msg) { ret = msg.error.code; }, 15000);
    }
    return ret;
}

int ApiSys::reboot()
{
    int ret = MODULE_LLM_WAIT_RESPONSE_TIMEOUT;
    _module_msg->sendCmdAndWaitToTakeMsg(
        _cmd_reboot, "sys_reboot", [&ret](ResponseMsg_t& msg) { ret = msg.error.code; }, 2000);
    return ret;
}

int ApiSys::setBaudRate(uint32_t baudRate)
{
    int ret = MODULE_LLM_WAIT_RESPONSE_TIMEOUT;
    String cmd =
        "{\"request_id\":\"1\",\"work_id\":\"sys\",\"action\":\"uartsetup\",\"object\":\"sys.uartsetup\",\"data\":{"
        "\"baud\":";
    cmd += baudRate;
    cmd += ",\"data_bits\":8,\"stop_bits\":1,\"parity\":\"n\"}}";

    _module_msg->sendCmdAndWaitToTakeMsg(
        cmd.c_str(), "sys_set_baudrate", [&ret](ResponseMsg_t& msg) { ret = msg.error.code; }, 2000);
    return ret;
}